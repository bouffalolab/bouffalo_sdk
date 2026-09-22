/**
 * @file iperf_udp_raw.c
 * @brief Task-driven lwIP Raw API implementation for Classic iPerf2 UDP tests.
 *
 * Each instance owns one worker task and one receive queue. The lwIP
 * receive callback only transfers pbuf ownership and endpoint metadata to the
 * queue. The worker handles pacing, protocol state, and shutdown while holding
 * the TCP/IP core lock only around Raw API calls.
 */

#include <string.h>

#include <lwip/ip.h>
#include <lwip/ip_addr.h>
#include <lwip/pbuf.h>
#include <lwip/sys.h>
#include <lwip/tcpip.h>
#include <lwip/udp.h>

#include <queue.h>

/** @brief Logging category for the UDP Raw backend. */
#define DBG_TAG "IPERF_UDP_RAW"
#include "log.h"

#include "iperf_internal.h"

#if !LWIP_TCPIP_CORE_LOCKING
#error "iPerf2 UDP Raw requires LWIP_TCPIP_CORE_LOCKING"
#endif

/** @brief FreeRTOS worker task name used by UDP Raw instances. */
#define IPERF_UDP_RAW_TASK_NAME      "iperf_udp_raw"
/** @brief Worker stack depth passed to xTaskCreate(), in StackType_t units. */
#define IPERF_UDP_RAW_TASK_STACK     512U
/** @brief Maximum number of received datagrams retained per instance. */
#define IPERF_UDP_RAW_RX_QUEUE_LEN   64U
/** @brief Maximum client FIN attempts while awaiting AckFIN. */
#define IPERF_UDP_RAW_FIN_RETRIES    10U
/** @brief Client FIN retry interval, in milliseconds. */
#define IPERF_UDP_RAW_FIN_TIMEOUT_MS 250U
/** @brief Server duplicate-FIN response window, in milliseconds. */
#define IPERF_UDP_RAW_ACK_WAIT_MS    1000U
/** @brief Requested queue-wait timeout for stop polling, not a latency bound. */
#define IPERF_UDP_RAW_IDLE_WAIT_MS   10U

/** @brief One datagram transferred from the lwIP callback to the worker. */
typedef struct {
    struct pbuf *p;        /**< Received pbuf owned by this queue item. */
    ip_addr_t remote_addr; /**< Datagram source address. */
    ip_addr_t local_addr;  /**< Datagram destination address. */
    uint64_t arrival_us;   /**< Callback arrival time for jitter accounting. */
    u16_t remote_port;     /**< Datagram source port. */
} iperf_udp_raw_rx_item_t;

/** @brief Complete state for one UDP Raw worker instance. */
typedef struct {
    bflb_iperf_t *iperf;                                      /**< Borrowed instance valid until completion. */
    struct udp_pcb *udp;                                      /**< PCB accessed only under the core lock. */
    QueueHandle_t rx_queue;                                   /**< Callback-to-worker receive queue. */
    ip_addr_t local_addr;                                     /**< Bound local address copied under the core lock. */
    u16_t local_port;                                         /**< Bound local port copied under the core lock. */
    ip_addr_t remote_addr;                                    /**< Connected or selected peer address. */
    u16_t remote_port;                                        /**< Connected or selected peer port. */
    uint8_t client_header[BFLB_IPERF_UDP_CLIENT_HEADER_SIZE]; /**< Worker-owned cached client settings and mutable sequence/time. */
} iperf_udp_raw_context_t;

/**
 * @brief Send one pbuf and release the caller's reference.
 * @param[in,out] context Active UDP Raw context.
 * @param[in,out] p Caller-owned pbuf; its reference is released on all outcomes.
 * @param[in] address Optional destination; NULL uses the connected peer.
 * @param[in] port Destination port used when address is non-NULL.
 * @return ERR_OK on acceptance, or an lwIP error.
 * @note As required by the lwIP netif contract, an asynchronous lower layer
 * retains its own reference before returning from udp_send/udp_sendto.
 * @pre Worker context outside the core lock; context->udp is valid.
 */
static err_t iperf_udp_raw_send_pbuf(iperf_udp_raw_context_t *context,
                                     struct pbuf *p,
                                     const ip_addr_t *address,
                                     u16_t port)
{
    err_t error;

    LOCK_TCPIP_CORE();
    error = (address == NULL) ? udp_send(context->udp, p) :
                                udp_sendto(context->udp, p, address, port);
    UNLOCK_TCPIP_CORE();

    pbuf_free(p);
    return error;
}

/**
 * @brief Build and send one setup, data, or FIN datagram.
 * @param[in,out] context Active UDP Raw context.
 * @param[in] length UDP payload length.
 * @param[in] id Signed packet ID.
 * @param[in] now_us Caller-captured monotonic timestamp in microseconds.
 * @pre Connected worker context outside the core lock.
 * @note Positive IDs with at least 80 bytes carry extended settings; every
 * datagram uses the 16-byte SEQ64 prefix. Negative IDs encode FIN.
 * Client settings must be cached before the first call; only sequence/time
 * are updated per send, then copied into the independently owned pbuf.
 * @return ERR_OK, ERR_MEM for transient backpressure, or another lwIP error.
 */
static err_t iperf_udp_raw_send_data(iperf_udp_raw_context_t *context,
                                     uint16_t length,
                                     int64_t id,
                                     uint64_t now_us)
{
    struct pbuf *p;
    uint16_t header_len = BFLB_IPERF_UDP_HEADER_SIZE;

    if (length < BFLB_IPERF_UDP_HEADER_SIZE) {
        return ERR_ARG;
    }
    p = pbuf_alloc(PBUF_TRANSPORT, length, PBUF_RAM);
    if (p == NULL) {
        return ERR_MEM;
    }

    // memset(p->payload, '0', length);
    if (id > 0 && length >= BFLB_IPERF_UDP_CLIENT_HEADER_SIZE) {
        header_len = BFLB_IPERF_UDP_CLIENT_HEADER_SIZE;
    }
    iperf_write_udp_header(context->client_header, id, now_us);
    memcpy(p->payload, context->client_header, header_len);
    return iperf_udp_raw_send_pbuf(context, p, NULL, 0U);
}

/**
 * @brief Send one server AckFIN report to the selected peer.
 * @param[in,out] context Active UDP Raw server context.
 * @param[in] format Layout selected by the validated setup.
 * @param[in] fin_id Negative FIN packet ID echoed in the report.
 * @pre format is supported; a selected peer and finalized statistics exist.
 * @note Allocate/send 112 bytes for SEQ32, 128 for either SEQ64 layout,
 * including zero compatibility padding; no extended statistics are emitted.
 * @return ERR_OK, ERR_MEM for transient backpressure, or another lwIP error.
 */
static err_t iperf_udp_raw_send_report(iperf_udp_raw_context_t *context,
                                       iperf_udp_setup_t format,
                                       int64_t fin_id)
{
    struct pbuf *p;
    uint16_t report_len = iperf_udp_server_report_size(format);

    p = pbuf_alloc(PBUF_TRANSPORT, report_len, PBUF_RAM);
    if (p == NULL) {
        return ERR_MEM;
    }
    iperf_write_udp_server_report(p->payload, context->iperf, format, fin_id);
    return iperf_udp_raw_send_pbuf(context, p, &context->remote_addr,
                                   context->remote_port);
}

/**
 * @brief Transfer one received pbuf to the worker queue.
 * @param[in,out] arg UDP Raw context registered with udp_recv().
 * @param[in] pcb Receiving PCB.
 * @param[in,out] p Received pbuf transferred to the queue or freed; NULL ignored.
 * @param[in] address Datagram source address.
 * @param[in] port Datagram source port.
 * @note A full queue drops and frees the datagram immediately.
 * @pre lwIP task callback with the core lock held, never an ISR. The queue is
 * alive until this callback is detached; enqueue never blocks or prints.
 */
static void iperf_udp_raw_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                               const ip_addr_t *address, u16_t port)
{
    iperf_udp_raw_context_t *context = arg;
    iperf_udp_raw_rx_item_t item;

    LWIP_UNUSED_ARG(pcb);
    if (p == NULL) {
        return;
    }

    item.p = p;
    ip_addr_copy(item.remote_addr, *address);
    ip_addr_copy(item.local_addr, *ip_current_dest_addr());
    item.remote_port = port;
    item.arrival_us = iperf_now_us();
    if (xQueueSend(context->rx_queue, &item, 0U) != pdPASS) {
        pbuf_free(p);
    }
}

/**
 * @brief Validate and import one client-side AckFIN candidate.
 * @param[in,out] context Active UDP Raw client context.
 * @param[in] item Received datagram and peer metadata.
 * @return true when a valid AckFIN completes the client test.
 * @note Checks peer and base report length/flags, not all protocol fields or
 * correspondence to the sent FIN. The caller retains and must free item->p.
 */
static bool iperf_udp_raw_client_report_received(iperf_udp_raw_context_t *context,
                                                 const iperf_udp_raw_rx_item_t *item)
{
    /* Only the 56-byte base prefix is needed, even for padded peer reports. */
    uint8_t report[BFLB_IPERF_UDP_ACK_SIZE];
    uint16_t copied;

    if (!ip_addr_cmp(&item->remote_addr, &context->remote_addr) ||
        item->remote_port != context->remote_port) {
        return false;
    }
    copied = LWIP_MIN((uint16_t)sizeof(report), item->p->tot_len);
    pbuf_copy_partial(item->p, report, copied, 0U);
    if (!iperf_udp_client_report_valid(report, copied)) {
        return false;
    }
    iperf_udp_client_read_report(context->iperf, report, copied);
    return true;
}

/**
 * @brief Send UDP FIN datagrams and wait for the server AckFIN report.
 * @param[in,out] context Active UDP Raw client context.
 * @param[in] next_id Sequence number following the final data datagram.
 * @retval ERR_OK AckFIN was received or stop was requested.
 * @retval ERR_TIMEOUT The configured FIN attempts were exhausted.
 * @return Another lwIP error when FIN transmission fails.
 */
static int iperf_udp_raw_wait_report(iperf_udp_raw_context_t *context,
                                     uint64_t next_id)
{
    bflb_iperf_t *iperf = context->iperf;
    int64_t fin_id = -(int64_t)next_id;
    uint32_t attempt;

    for (attempt = 0; attempt < IPERF_UDP_RAW_FIN_RETRIES; attempt++) {
        uint64_t deadline_us;
        err_t error;

        if (iperf->stop_requested) {
            return ERR_OK;
        }
        error = iperf_udp_raw_send_data(context, iperf->config.buffer_len, fin_id, iperf_now_us());
        if (error != ERR_OK && error != ERR_MEM && error != ERR_BUF) {
            return error;
        }
        deadline_us = iperf_now_us() + (uint64_t)IPERF_UDP_RAW_FIN_TIMEOUT_MS * 1000ULL;

        while (!iperf->stop_requested && iperf_now_us() < deadline_us) {
            iperf_udp_raw_rx_item_t item;

            if (xQueueReceive(context->rx_queue, &item, pdMS_TO_TICKS(IPERF_UDP_RAW_IDLE_WAIT_MS)) == pdPASS) {
                bool received = iperf_udp_raw_client_report_received(context, &item);

                pbuf_free(item.p);
                if (received) {
                    return ERR_OK;
                }
            }
        }
    }
    return iperf->stop_requested ? ERR_OK : ERR_TIMEOUT;
}

/**
 * @brief Send the server AckFIN and respond to duplicate FIN datagrams.
 * @param[in,out] context Active UDP Raw server context.
 * @param[in,out] udp_rx Receiver tracker to finalize once.
 * @param[in] fin_id Initial negative FIN packet ID.
 * @retval ERR_OK At least one report was sent, or stop was requested.
 * @return Last send error if no report was sent and stop was not requested.
 * @note Repeated negative IDs from the selected peer extend the response window
 * without changing frozen statistics; total completion time is not bounded.
 */
static int iperf_udp_raw_report_server(iperf_udp_raw_context_t *context,
                                       iperf_udp_rx_t *udp_rx,
                                       int64_t fin_id)
{
    bflb_iperf_t *iperf = context->iperf;
    uint64_t deadline_us;
    err_t last_error = ERR_MEM;
    bool report_sent = false;

    iperf_udp_server_rx_finish(iperf, udp_rx, fin_id);
    deadline_us = iperf_now_us() + (uint64_t)IPERF_UDP_RAW_ACK_WAIT_MS * 1000ULL;

    while (!iperf->stop_requested && iperf_now_us() < deadline_us) {
        iperf_udp_raw_rx_item_t item;
        err_t error = iperf_udp_raw_send_report(context, udp_rx->format, fin_id);

        if (error == ERR_OK) {
            report_sent = true;
        } else {
            last_error = error;
            if (error != ERR_MEM && error != ERR_BUF) {
                break;
            }
            vTaskDelay(5U);
            continue;
        }
        while (!iperf->stop_requested && iperf_now_us() < deadline_us) {
            uint8_t header[BFLB_IPERF_UDP_HEADER_SIZE];
            uint16_t copied;
            int64_t packet_id;

            if (xQueueReceive(context->rx_queue, &item, pdMS_TO_TICKS(IPERF_UDP_RAW_IDLE_WAIT_MS)) != pdPASS) {
                continue;
            }
            if (ip_addr_cmp(&item.remote_addr, &context->remote_addr) &&
                item.remote_port == context->remote_port) {
                copied = LWIP_MIN((uint16_t)sizeof(header), item.p->tot_len);
                copied = pbuf_copy_partial(item.p, header, copied, 0U);
                if (iperf_udp_decode_id(udp_rx->format, header, copied, &packet_id) && packet_id < 0) {
                    fin_id = packet_id;
                    deadline_us = iperf_now_us() + (uint64_t)IPERF_UDP_RAW_ACK_WAIT_MS * 1000ULL;
                    pbuf_free(item.p);
                    break;
                }
            }
            pbuf_free(item.p);
        }
    }
    return (iperf->stop_requested || report_sent) ? ERR_OK : last_error;
}

/**
 * @brief Release all receive pbufs still queued during shutdown.
 * @param[in,out] context UDP Raw context being closed.
 * @pre Receive callback detached; the queue still exists and cannot gain items.
 */
static void iperf_udp_raw_flush_rx(iperf_udp_raw_context_t *context)
{
    iperf_udp_raw_rx_item_t item;

    while (xQueueReceive(context->rx_queue, &item, 0U) == pdPASS) {
        pbuf_free(item.p);
    }
}

/**
 * @brief Run a paced UDP Raw client through FIN/AckFIN completion.
 * @param[in,out] context Initialized UDP Raw client context.
 * @retval ERR_OK The test completed or a stop request was observed.
 * @return ERR_TIMEOUT when AckFIN retries expire, or another lwIP error.
 */
static int iperf_udp_raw_client(iperf_udp_raw_context_t *context)
{
    bflb_iperf_t *iperf = context->iperf;
    ip_addr_t local_addr;
    uint64_t next_deadline_us;
    uint64_t packet_interval_us;
    uint64_t now_us;
    uint64_t packet_id = 2U;
    iperf_stats_t stats;
    err_t error = ERR_OK;

    LOCK_TCPIP_CORE();
    context->udp = udp_new_ip_type(IPADDR_TYPE_V4);
    if (context->udp == NULL) {
        UNLOCK_TCPIP_CORE();
        LOG_E("UDP Raw client PCB allocation failed\r\n");
        return ERR_MEM;
    }
    context->udp->tos = iperf->config.tos;
    ip_addr_set_ip4_u32(&local_addr, iperf->config.local_ip4);
    if (iperf->config.local_ip4 != 0U || iperf->config.local_port != 0U) {
        error = udp_bind(context->udp, &local_addr, iperf->config.local_port);
    }
    ip_addr_set_ip4_u32(&context->remote_addr, iperf->config.remote_ip4);
    context->remote_port = iperf->config.port;
    if (error == ERR_OK) {
        error = udp_connect(context->udp, &context->remote_addr, context->remote_port);
    }
    if (error == ERR_OK) {
        ip_addr_copy(context->local_addr, context->udp->local_ip);
        context->local_port = context->udp->local_port;
        udp_recv(context->udp, iperf_udp_raw_recv, context);
    }
    UNLOCK_TCPIP_CORE();
    if (error != ERR_OK) {
        LOG_E("UDP Raw client bind/connect failed: error=%d\r\n", error);
        return error;
    }

    iperf_log_client_preamble(iperf);

    /* Packet ID one advertises normal-mode settings before paced traffic. */
    now_us = iperf_now_us();
    iperf_write_udp_client_header(context->client_header, iperf, now_us);
    error = iperf_udp_raw_send_data(context, iperf->config.buffer_len, 1, now_us);
    if (error != ERR_OK) {
        LOG_E("UDP Raw client setup send failed: error=%d\r\n", error);
        return error;
    }

    iperf_log_connection(iperf,
                         ip_2_ip4(&context->local_addr)->addr, context->local_port,
                         ip_2_ip4(&context->remote_addr)->addr, context->remote_port);

    iperf_test_begin(iperf);
    iperf_account_transfer(iperf, iperf->config.buffer_len);
    uint32_t rate_bps = iperf->config.bandwidth_bps ? iperf->config.bandwidth_bps : BFLB_IPERF_DEFAULT_UDP_RATE_BPS;
    packet_interval_us = (uint64_t)iperf->config.buffer_len * 8ULL * 1000000ULL / rate_bps;
    iperf_stats_snapshot(iperf, &stats);
    next_deadline_us = stats.start_us + packet_interval_us;

    while (true) {
        iperf_udp_raw_rx_item_t item;
        uint16_t length = iperf->config.buffer_len;

        now_us = iperf_now_us();
        if (iperf_limit_reached(iperf, now_us)) {
            break;
        }
        if (iperf_test_report_periodic(iperf, now_us)) {
            /* Discard pacing debt from earlier report intervals. */
            next_deadline_us = now_us;
        }
        /* No receive data is expected before FIN; release any stray packets. */
        while (xQueueReceive(context->rx_queue, &item, 0U) == pdPASS) {
            pbuf_free(item.p);
        }

        /* Absolute deadlines avoid accumulating delay and send-call execution time. */
        if (now_us < next_deadline_us) {
            uint32_t remaining_ms = (next_deadline_us - now_us + 500ULL) / 1000ULL;
            if (remaining_ms > IPERF_UDP_RAW_IDLE_WAIT_MS) {
                remaining_ms = IPERF_UDP_RAW_IDLE_WAIT_MS;
            }
            if (remaining_ms > 0) {
                vTaskDelay(pdMS_TO_TICKS(remaining_ms));
            }
            continue;
        }

        length = (uint16_t)LWIP_MIN((uint64_t)length, iperf_bytes_remaining(iperf));
        if (length < BFLB_IPERF_UDP_HEADER_SIZE) {
            break;
        }

        error = iperf_udp_raw_send_data(context, length, (int64_t)packet_id, now_us);
        if (error == ERR_OK) {
            iperf_account_transfer(iperf, length);
            packet_id++;
            next_deadline_us += packet_interval_us;
            continue;
        }
        if (error == ERR_MEM || error == ERR_BUF) {
            vTaskDelay(1U);
            continue;
        }
        LOG_E("UDP Raw client data send failed: id=%llu error=%d\r\n", (unsigned long long)packet_id, error);
        return error;
    }

    iperf_test_end(iperf);

    if (!iperf->stop_requested) {
        error = iperf_udp_raw_wait_report(context, packet_id);
        if (error != ERR_OK) {
            LOG_E("UDP Raw client FIN/AckFIN exchange failed: error=%d\r\n", error);
        }
    }

    return error;
}

/**
 * @brief Run a single-peer UDP Raw server through its AckFIN response window.
 * @param[in,out] context Initialized UDP Raw server context.
 * @note Only the first setup rejection per server worker logs its reason.
 * @retval ERR_OK The test completed or a stop request was observed.
 * @return An lwIP error on PCB allocation, bind, peer connect or report failure.
 */
static int iperf_udp_raw_server(iperf_udp_raw_context_t *context)
{
    bflb_iperf_t *iperf = context->iperf;
    ip_addr_t local_addr;
    iperf_udp_rx_t udp_rx;
    uint8_t peer_set = 0U;
    bool setup_warned = false;
    err_t error = ERR_OK;

    uint8_t header[BFLB_IPERF_UDP_CLIENT_HEADER_SIZE];

    LOCK_TCPIP_CORE();
    context->udp = udp_new_ip_type(IPADDR_TYPE_V4);
    if (context->udp == NULL) {
        UNLOCK_TCPIP_CORE();
        LOG_E("UDP Raw server PCB allocation failed\r\n");
        return ERR_MEM;
    }
    context->udp->tos = iperf->config.tos;
    ip_addr_set_ip4_u32(&local_addr, iperf->config.local_ip4);
    error = udp_bind(context->udp, &local_addr, iperf->config.port);
    if (error == ERR_OK) {
        ip_addr_copy(context->local_addr, context->udp->local_ip);
        context->local_port = context->udp->local_port;
        udp_recv(context->udp, iperf_udp_raw_recv, context);
    }
    UNLOCK_TCPIP_CORE();
    if (error != ERR_OK) {
        LOG_E("UDP Raw server bind failed: error=%d\r\n", error);
        return error;
    }

    iperf_log_server_preamble(iperf);

    while (!iperf->stop_requested) {
        iperf_udp_raw_rx_item_t item;
        uint16_t packet_len;
        uint16_t copied;
        int64_t packet_id;
        iperf_udp_setup_t setup_type;

        iperf_test_report_periodic(iperf, iperf_now_us());
        if (xQueueReceive(context->rx_queue, &item, pdMS_TO_TICKS(IPERF_UDP_RAW_IDLE_WAIT_MS)) != pdPASS) {
            continue;
        }

        packet_len = item.p->tot_len;
        copied = LWIP_MIN((uint16_t)sizeof(header), packet_len);
        copied = pbuf_copy_partial(item.p, header, copied, 0U);
        pbuf_free(item.p);

        /* Only a valid setup packet may claim this single-peer server session. */
        if (peer_set == 0U) {
            const char *reason;

            setup_type = iperf_udp_client_setup_type(header, copied, &reason);
            if (setup_type == IPERF_UDP_SETUP_INVALID) {
                if (!setup_warned) {
                    LOG_W("UDP Raw server setup rejected: %s (prefix=%u bytes)\r\n", reason, (unsigned int)copied);
                    setup_warned = true;
                }
                continue;
            }

            LOCK_TCPIP_CORE();
            error = udp_connect(context->udp, &item.remote_addr, item.remote_port);
            UNLOCK_TCPIP_CORE();
            if (error != ERR_OK) {
                LOG_E("UDP Raw server connect failed: error=%d\r\n", error);
                return error;
            }
            ip_addr_copy(context->remote_addr, item.remote_addr);
            context->remote_port = item.remote_port;

            peer_set = 1U;
            iperf_log_connection(iperf,
                                 ip_2_ip4(&item.local_addr)->addr, context->local_port,
                                 ip_2_ip4(&item.remote_addr)->addr, item.remote_port);
            iperf_test_begin(iperf);
            iperf_udp_server_rx_init(&udp_rx, setup_type);

        } else if (!ip_addr_cmp(&item.remote_addr, &context->remote_addr) ||
                   (item.remote_port != context->remote_port)) {
            /* Ignore stray datagrams from other peers. */
            continue;
        }
        if (!iperf_udp_decode_id(udp_rx.format, header, copied, &packet_id)) {
            continue;
        }

        if (packet_id < 0) {
            error = iperf_udp_raw_report_server(context, &udp_rx, packet_id);
            if (error != ERR_OK) {
                LOG_E("UDP Raw server AckFIN send failed: error=%d\r\n", error);
            }
            break;
        }

        iperf_udp_server_rx_account(iperf, &udp_rx, packet_id,
                                    iperf_read_udp_timestamp(header),
                                    item.arrival_us, packet_len);
    }

    return error;
}

/**
 * @brief Run one UDP Raw worker and own its common resource lifecycle.
 * @param[in,out] arg Initialized iperf_udp_raw_context_t.
 * @warning iperf_worker_exit() may make context destroyable; only task
 * self-deletion may follow it.
 */
static void iperf_udp_raw_task(void *arg)
{
    iperf_udp_raw_context_t *context = arg;
    bflb_iperf_t *iperf = context->iperf;
    int error;

    if (!iperf_worker_enter(iperf)) {
        error = 0;
    } else if (iperf->config.role == BFLB_IPERF_ROLE_CLIENT) {
        error = iperf_udp_raw_client(context);
    } else {
        error = iperf_udp_raw_server(context);
    }

    if (context->udp != NULL) {
        LOCK_TCPIP_CORE();
        udp_recv(context->udp, NULL, NULL);
        udp_remove(context->udp);
        context->udp = NULL;
        UNLOCK_TCPIP_CORE();
    }
    iperf_udp_raw_flush_rx(context);

    vQueueDelete(context->rx_queue);
    context->rx_queue = NULL;
    context->iperf = NULL;

    iperf_worker_exit(iperf, error);
    vTaskDelete(NULL);
}

/**
 * @brief Create the receive queue and UDP Raw worker task.
 * @param[in,out] iperf Validated instance retained until completion.
 * @param[in,out] private_context Zero-initialized UDP Raw context.
 * @retval 0 The worker was created successfully.
 * @retval -1 Queue or task creation failed.
 * @post Failure rolls back the queue, leaves no worker and emits no events.
 * @note Events may occur before return; no context writes follow successful
 * task creation. The core frees the context after synchronous failure.
 */
static int iperf_udp_raw_launch(bflb_iperf_t *iperf, void *private_context)
{
    iperf_udp_raw_context_t *context = private_context;

    context->iperf = iperf;
    context->rx_queue = xQueueCreate(IPERF_UDP_RAW_RX_QUEUE_LEN, sizeof(iperf_udp_raw_rx_item_t));
    if (context->rx_queue == NULL) {
        LOG_E("UDP Raw receive queue creation failed\r\n");
        context->iperf = NULL;
        return -1;
    }
    if (xTaskCreate(iperf_udp_raw_task, IPERF_UDP_RAW_TASK_NAME,
                    IPERF_UDP_RAW_TASK_STACK, context,
                    iperf->config.task_priority, NULL) != pdPASS) {
        LOG_E("UDP Raw task creation failed\r\n");
        vQueueDelete(context->rx_queue);
        context->rx_queue = NULL;
        context->iperf = NULL;
        return -1;
    }
    return 0;
}

/**
 * @brief Accept a cooperative stop request for a UDP Raw worker.
 * @param[in] iperf Instance whose stop flag was set by the core.
 * @param[in] private_context Core-owned context; unused and possibly cleaned up.
 * @note Queue waits are bounded by IPERF_UDP_RAW_IDLE_WAIT_MS. Do not touch
 * the queue here: the worker may already have detached RX and deleted it.
 * This hook is a no-op, not a queue wakeup. Queue timeouts and retry delays
 * permit stop polling but do not guarantee a real-time destroy deadline.
 */
static void iperf_udp_raw_request_stop(bflb_iperf_t *iperf, void *private_context)
{
    LWIP_UNUSED_ARG(iperf);
    LWIP_UNUSED_ARG(private_context);
}

/** @brief Backend operation table for task-driven Classic iPerf2 UDP Raw tests. */
const iperf_backend_ops_t g_iperf_udp_raw_ops = {
    .context_size = sizeof(iperf_udp_raw_context_t),
    .launch = iperf_udp_raw_launch,
    .request_stop = iperf_udp_raw_request_stop,
};
