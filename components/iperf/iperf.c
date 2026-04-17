#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <FreeRTOS.h>
#include <task.h>

#include "iperf.h"
#include "bflb_core.h"
#include <bflb_mtimer.h>

// TODO move to common
#define xTaskCreatePinnedToCore(pvTaskCode, pcName, usStackDepth, \
                                pvParameters, uxPriority,         \
                                pvCreatedTask, xCoreID_)          \
    xTaskCreate(pvTaskCode, pcName, usStackDepth, pvParameters,   \
                uxPriority, pvCreatedTask)

#define iperf_timer_get_time bflb_mtimer_get_time_us
#define iperf_delay_us       bflb_mtimer_delay_us
#define IRAM_ATTR            ATTR_TCM_SECTION // on tcm run
#define IPERF_V6             0                // TODO sync with lwip config
#define iperf_err_t          int
#define IPERF_OK             0  /*!< iperf_err_t value indicating success (no error) */
#define IPERF_FAIL           -1 /*!< Generic iperf_err_t code indicating failure */

#define IPERF_LOGE(tag, format, ...)                       \
    do {                                                   \
        (void)tag;                                         \
        printf("[%s] " format "\r\n", tag, ##__VA_ARGS__); \
    } while (0)

#define IPERF_LOGW(tag, format, ...)                       \
    do {                                                   \
        (void)tag;                                         \
        printf("[%s] " format "\r\n", tag, ##__VA_ARGS__); \
    } while (0)

#define IPERF_LOGI(tag, format, ...)                       \
    do {                                                   \
        (void)tag;                                         \
        printf("[%s] " format "\r\n", tag, ##__VA_ARGS__); \
    } while (0)

#define IPERF_LOGD(tag, format, ...)                       \
    do {                                                   \
        (void)tag;                                         \
        printf("[%s] " format "\r\n", tag, ##__VA_ARGS__); \
    } while (0)

#define IPERF_LOGV(tag, format, ...)                       \
    do {                                                   \
        (void)tag;                                         \
        printf("[%s] " format "\r\n", tag, ##__VA_ARGS__); \
    } while (0)

#define IPERF_GOTO_ON_FALSE(a, err_code, goto_tag, log_tag, format, ...) \
    do {                                                                 \
        if (unlikely(!(a))) {                                            \
            IPERF_LOGE(log_tag, format, ##__VA_ARGS__);                  \
            ret = err_code;                                              \
            goto goto_tag;                                               \
        }                                                                \
    } while (0)

#define NL "\r\n"

typedef struct {
    iperf_cfg_t cfg;
    bool finish;
    uint32_t actual_len;
    uint32_t tot_len;
    uint32_t buffer_len;
    uint8_t *buffer;
    uint32_t sockfd;
    uint64_t udp_start_us;
    int64_t udp_last_transit_us;
    double udp_jitter_us;
    int32_t udp_last_id;
    uint32_t udp_datagram_cnt;
    uint32_t udp_error_cnt;
    uint32_t udp_outorder_cnt;
} iperf_ctrl_t;

static bool s_iperf_is_running = false;
static iperf_ctrl_t s_iperf_ctrl;
static const char *TAG = "iperf";

inline static bool iperf_is_udp_client(void)
{
    return ((s_iperf_ctrl.cfg.flag & IPERF_FLAG_CLIENT) && (s_iperf_ctrl.cfg.flag & IPERF_FLAG_UDP));
}

inline static bool iperf_is_udp_server(void)
{
    return ((s_iperf_ctrl.cfg.flag & IPERF_FLAG_SERVER) && (s_iperf_ctrl.cfg.flag & IPERF_FLAG_UDP));
}

inline static bool iperf_is_tcp_client(void)
{
    return ((s_iperf_ctrl.cfg.flag & IPERF_FLAG_CLIENT) && (s_iperf_ctrl.cfg.flag & IPERF_FLAG_TCP));
}

inline static bool iperf_is_tcp_dual_client(void)
{
    return ((s_iperf_ctrl.cfg.flag & IPERF_FLAG_CLIENT) && (s_iperf_ctrl.cfg.flag & IPERF_FLAG_TCP) && (s_iperf_ctrl.cfg.flag & IPERF_FLAG_DUAL));
}

inline static bool iperf_is_tcp_server(void)
{
    return ((s_iperf_ctrl.cfg.flag & IPERF_FLAG_SERVER) && (s_iperf_ctrl.cfg.flag & IPERF_FLAG_TCP));
}

static int iperf_get_socket_error_code(int sockfd)
{
    (void)sockfd;
    return errno;
}

static int iperf_show_socket_error_reason(const char *str, int sockfd)
{
    int err = errno;
    if (err != 0) {
        IPERF_LOGW(TAG, "%s error, error code: %d, reason: %s", str, err, strerror(err));
    }

    return err;
}

static void iperf_report_task(void *arg)
{
    uint32_t interval = s_iperf_ctrl.cfg.interval;
    uint32_t time = s_iperf_ctrl.cfg.time;
    TickType_t delay_interval = (interval * 1000) / portTICK_PERIOD_MS;
    uint32_t cur = 0;
    double average = 0;
    double actual_bandwidth = 0;
    double actual_transfer = 0;
    int k = 1;

    printf("[ ID] Interval       Transfer     Bandwidth\r\n");
    while (!s_iperf_ctrl.finish) {
        vTaskDelay(delay_interval);
        actual_bandwidth = (s_iperf_ctrl.actual_len / 1e6 * 8) / interval;
        actual_transfer = s_iperf_ctrl.actual_len / 1e6;
        printf("[%3d] %2d.0-%2d.0 sec  %.2f MByte  %.2f Mbits/sec\r\n",
               s_iperf_ctrl.sockfd, cur, cur + interval, actual_transfer, actual_bandwidth);
        cur += interval;
        average = ((average * (k - 1) / k) + (actual_bandwidth / k));
        k++;
        s_iperf_ctrl.actual_len = 0;
        if (cur >= time) {
            actual_transfer = s_iperf_ctrl.tot_len / 1e6;
            printf("[%3d] %2d.0-%2d.0 sec  %.2f MByte  %.2f Mbits/sec\r\n",
                   s_iperf_ctrl.sockfd, 0, time, actual_transfer, average);
            printf("iperf done\r\n");
            break;
        }
    }

    s_iperf_ctrl.finish = true;
    vTaskDelete(NULL);
}

static iperf_err_t iperf_start_report(void)
{
    int ret;

    ret = xTaskCreatePinnedToCore(iperf_report_task, IPERF_REPORT_TASK_NAME, IPERF_REPORT_TASK_STACK, NULL, s_iperf_ctrl.cfg.traffic_task_priority + 1, NULL, portNUM_PROCESSORS - 1);

    if (ret != pdPASS) {
        IPERF_LOGE(TAG, "create task %s failed", IPERF_REPORT_TASK_NAME);
        return IPERF_FAIL;
    }

    return IPERF_OK;
}

typedef struct {
    int32_t flags;
    int32_t numThreads;
    int32_t mPort;
    int32_t bufferlen;
    int32_t mWindowSize;
    int32_t mAmount;
    int32_t mRate;
    int32_t mUDPRateUnits;
    int32_t mRealtime;
} iperf_client_hdr_t;
#define HEADER_VERSION1 0x80000000
#define RUN_NOW         0x00000001
#define UNITS_PPS       0x00000002
#define IPERF_UDP_ACK_RETRY_MAX 20
#define IPERF_UDP_SEND_FAIL_DELAY_US 200

typedef struct {
    int32_t id;
    uint32_t tv_sec;
    uint32_t tv_usec;
} iperf_udp_datagram_t;

typedef struct {
    int32_t flags;
    int32_t total_len1;
    int32_t total_len2;
    int32_t stop_sec;
    int32_t stop_usec;
    int32_t error_cnt;
    int32_t outorder_cnt;
    int32_t datagrams;
    int32_t jitter1;
    int32_t jitter2;
} iperf_server_hdr_t;

typedef struct {
    iperf_udp_datagram_t datagram;
    iperf_server_hdr_t hdr;
} iperf_udp_ackfin_t;

static inline uint64_t iperf_time_now_us(void)
{
    return iperf_timer_get_time();
}

static void iperf_set_udp_datagram(iperf_udp_datagram_t *datagram, int32_t packet_id, uint64_t now_us)
{
    datagram->id = htonl(packet_id);
    datagram->tv_sec = htonl((uint32_t)(now_us / 1000000ULL));
    datagram->tv_usec = htonl((uint32_t)(now_us % 1000000ULL));
}

static void iperf_update_udp_jitter_lost(int32_t packet_id, uint32_t sent_sec, uint32_t sent_usec, uint64_t now_us)
{
    int64_t transit_us;
    int64_t delta_us;
    uint64_t sent_us = ((uint64_t)sent_sec * 1000000ULL) + (uint64_t)sent_usec;
    int32_t expected = s_iperf_ctrl.udp_last_id + 1;

    s_iperf_ctrl.udp_datagram_cnt++;
    if (s_iperf_ctrl.udp_start_us == 0) {
        s_iperf_ctrl.udp_start_us = now_us;
    }

    if (packet_id != expected) {
        if (packet_id < expected) {
            s_iperf_ctrl.udp_outorder_cnt++;
        } else {
            s_iperf_ctrl.udp_error_cnt += (uint32_t)(packet_id - expected);
        }
    }
    if (packet_id > s_iperf_ctrl.udp_last_id) {
        s_iperf_ctrl.udp_last_id = packet_id;
    }

    if (sent_us != 0) {
        transit_us = (int64_t)now_us - (int64_t)sent_us;
        if (transit_us < 0) {
            transit_us = -transit_us;
        }
        if (s_iperf_ctrl.udp_last_transit_us != 0) {
            delta_us = transit_us - s_iperf_ctrl.udp_last_transit_us;
            if (delta_us < 0) {
                delta_us = -delta_us;
            }
            s_iperf_ctrl.udp_jitter_us += ((double)delta_us - s_iperf_ctrl.udp_jitter_us) / 16.0;
        }
        s_iperf_ctrl.udp_last_transit_us = transit_us;
    }
}

static void iperf_udp_server_send_ackfin(int recv_socket, const struct sockaddr *peer_addr, socklen_t peer_len, int32_t fin_packet_id)
{
    iperf_udp_ackfin_t ack = { 0 };
    uint64_t duration_us = 0;
    uint64_t jitter_us = (uint64_t)(s_iperf_ctrl.udp_jitter_us);

    if (s_iperf_ctrl.udp_start_us > 0) {
        duration_us = iperf_time_now_us() - s_iperf_ctrl.udp_start_us;
    }

    iperf_set_udp_datagram(&ack.datagram, -fin_packet_id, iperf_time_now_us());
    ack.hdr.flags = htonl(0x80000000);
    ack.hdr.total_len1 = htonl((uint32_t)((uint64_t)s_iperf_ctrl.tot_len >> 32));
    ack.hdr.total_len2 = htonl((uint32_t)((uint64_t)s_iperf_ctrl.tot_len & 0xFFFFFFFFULL));
    ack.hdr.stop_sec = htonl((uint32_t)(duration_us / 1000000ULL));
    ack.hdr.stop_usec = htonl((uint32_t)(duration_us % 1000000ULL));
    ack.hdr.error_cnt = htonl(s_iperf_ctrl.udp_error_cnt);
    ack.hdr.outorder_cnt = htonl(s_iperf_ctrl.udp_outorder_cnt);
    ack.hdr.datagrams = htonl(s_iperf_ctrl.udp_datagram_cnt);
    ack.hdr.jitter1 = htonl((uint32_t)(jitter_us / 1000000ULL));
    ack.hdr.jitter2 = htonl((uint32_t)(jitter_us % 1000000ULL));

    sendto(recv_socket, &ack, sizeof(ack), 0, peer_addr, peer_len);
}

static void iperf_udp_print_server_summary(void)
{
    double jitter_ms = s_iperf_ctrl.udp_jitter_us / 1000.0;
    double loss_pct = 0.0;

    if (s_iperf_ctrl.udp_datagram_cnt > 0) {
        loss_pct = (100.0 * (double)s_iperf_ctrl.udp_error_cnt) / (double)s_iperf_ctrl.udp_datagram_cnt;
    }

    printf("[SUM] UDP jitter %.3f ms  lost %u/%u (%.2f%%)  out-of-order %u\r\n",
           jitter_ms,
           s_iperf_ctrl.udp_error_cnt,
           s_iperf_ctrl.udp_datagram_cnt,
           loss_pct,
           s_iperf_ctrl.udp_outorder_cnt);
}

static void iperf_udp_client_wait_ackfin(int client_socket, struct sockaddr *dest_addr, socklen_t socklen, int32_t last_packet_id)
{
    int retry = 0;
    int recv_len;
    int32_t fin_id = last_packet_id;
    struct timeval timeout = { 0 };
    iperf_udp_ackfin_t ack = { 0 };
    double jitter_ms;
    uint32_t lost;
    uint32_t total;
    uint32_t out_of_order;
    double loss_pct = 0.0;

    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    for (retry = 0; retry < IPERF_UDP_ACK_RETRY_MAX; retry++) {
        iperf_set_udp_datagram((iperf_udp_datagram_t *)s_iperf_ctrl.buffer, -fin_id, iperf_time_now_us());
        sendto(client_socket, s_iperf_ctrl.buffer, s_iperf_ctrl.buffer_len, 0, dest_addr, socklen);
        recv_len = recvfrom(client_socket, s_iperf_ctrl.buffer, s_iperf_ctrl.buffer_len, 0, NULL, NULL);
        if (recv_len >= (int)sizeof(iperf_udp_ackfin_t)) {
            memcpy(&ack, s_iperf_ctrl.buffer, sizeof(ack));
            lost = ntohl(ack.hdr.error_cnt);
            total = ntohl(ack.hdr.datagrams);
            out_of_order = ntohl(ack.hdr.outorder_cnt);
            jitter_ms = (double)ntohl(ack.hdr.jitter1) * 1000.0 + (double)ntohl(ack.hdr.jitter2) / 1000.0;
            if (total > 0) {
                loss_pct = (100.0 * (double)lost) / (double)total;
            }
            printf("[SVR] UDP jitter %.3f ms  lost %u/%u (%.2f%%)  out-of-order %u\r\n",
                   jitter_ms, lost, total, loss_pct, out_of_order);
            return;
        }
    }

    printf("[SVR] WARNING: no UDP AckFIN received\r\n");
}

static void socket_recv(int recv_socket, struct sockaddr_storage listen_addr, uint8_t type)
{
    bool iperf_recv_start = true;
    uint8_t *buffer;
    int want_recv = 0;
    int actual_recv = 0;
    struct sockaddr_storage peer_addr = { 0 };
    iperf_udp_datagram_t *udp_data;
#if IPERF_V6
    socklen_t socklen = (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6) ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in);
#else
    socklen_t socklen = sizeof(struct sockaddr_in);
#endif
    const char *error_log = (type == IPERF_TRANS_TYPE_TCP) ? "tcp server recv" : "udp server recv";
    (void)listen_addr;

    buffer = s_iperf_ctrl.buffer;
    want_recv = s_iperf_ctrl.buffer_len;
    while (!s_iperf_ctrl.finish) {
        socklen = sizeof(peer_addr);
        actual_recv = recvfrom(recv_socket, buffer, want_recv, 0, (struct sockaddr *)&peer_addr, &socklen);
        if (actual_recv < 0) {
            int err = iperf_get_socket_error_code(recv_socket);
            if (type == IPERF_TRANS_TYPE_UDP && (err == EAGAIN || err == EWOULDBLOCK)) {
                continue;
            }
            iperf_show_socket_error_reason(error_log, recv_socket);
            s_iperf_ctrl.finish = true;
            break;
        } else {
            if (iperf_recv_start) {
                iperf_start_report();
                iperf_recv_start = false;
            }
            s_iperf_ctrl.actual_len += actual_recv;
            s_iperf_ctrl.tot_len += actual_recv;
            if (type == IPERF_TRANS_TYPE_UDP && actual_recv >= (int)sizeof(iperf_udp_datagram_t)) {
                int32_t packet_id;

                udp_data = (iperf_udp_datagram_t *)buffer;
                packet_id = (int32_t)ntohl(udp_data->id);
                if (packet_id == 0) {
                    if (s_iperf_ctrl.udp_last_id < 0) {
                        s_iperf_ctrl.udp_last_id = 0;
                    }
                    continue;
                }
                if (packet_id < 0) {
                    packet_id = -packet_id;
                    iperf_udp_server_send_ackfin(recv_socket, (struct sockaddr *)&peer_addr, socklen, packet_id);
                    break;
                }
                iperf_update_udp_jitter_lost(packet_id,
                                             ntohl(udp_data->tv_sec),
                                             ntohl(udp_data->tv_usec),
                                             iperf_time_now_us());
            }
            if (s_iperf_ctrl.cfg.num_bytes > 0 && s_iperf_ctrl.tot_len > s_iperf_ctrl.cfg.num_bytes) {
                break;
            }
        }
    }
    if (type == IPERF_TRANS_TYPE_UDP) {
        iperf_udp_print_server_summary();
    }
}

static void socket_recv_dual(int recv_socket, struct sockaddr_storage listen_addr, uint8_t type)
{
    uint8_t *buffer;
    int want_recv = 0;
    int actual_recv = 0;
    socklen_t socklen = sizeof(struct sockaddr_in);
    (void)type;

#define RECV_DUAL_BUF_LEN (4 * 1024)
    buffer = malloc(RECV_DUAL_BUF_LEN);
    want_recv = RECV_DUAL_BUF_LEN;
    if (!buffer) {
        return;
    }
    while (1) {
        actual_recv = recvfrom(recv_socket, buffer, want_recv, 0, (struct sockaddr *)&listen_addr, &socklen);
        if (actual_recv <= 0) {
            break;
        }
    }
    free(buffer);
}

static void send_dual_header(int sock, struct sockaddr *addr, socklen_t socklen)
{
    iperf_client_hdr_t hdr = {};
    iperf_cfg_t *cfg = &s_iperf_ctrl.cfg;

    hdr.flags = htonl(HEADER_VERSION1 | RUN_NOW);
    hdr.numThreads = htonl(1);
    hdr.mPort = htonl(cfg->sport);
    hdr.mAmount = htonl(-(cfg->time * 100));

    sendto(sock, &hdr, sizeof(hdr), 0, addr, socklen);
}

static void socket_send(int send_socket, struct sockaddr_storage dest_addr, uint8_t type, int bw_lim)
{
    uint8_t *buffer;
    int32_t *pkt_id_p;
    int32_t pkt_cnt = 0;
    int32_t last_udp_packet_id = 0;
    bool udp_sent_any = false;
    int actual_send = 0;
    int want_send = 0;
    int period_us = -1;
    int delay_us = 0;
    int64_t prev_time = 0;
    int64_t send_time = 0;
    int err = 0;
#if IPERF_V6
    const socklen_t socklen = (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6) ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in);
#else
    const socklen_t socklen = sizeof(struct sockaddr_in);
#endif
    const char *error_log = (type == IPERF_TRANS_TYPE_TCP) ? "tcp client send" : "udp client send";

    buffer = s_iperf_ctrl.buffer;
    pkt_id_p = (int32_t *)s_iperf_ctrl.buffer;
    want_send = s_iperf_ctrl.buffer_len;
    iperf_start_report();

    if (bw_lim > 0) {
        period_us = want_send * 8 / bw_lim;
    }

    if (iperf_is_tcp_dual_client()) {
        send_dual_header(send_socket, (struct sockaddr *)&dest_addr, socklen);
    }

    while (!s_iperf_ctrl.finish) {
        if (period_us > 0) {
            send_time = iperf_timer_get_time();
            if (actual_send > 0) {
                // Last packet "send" was successful, check how much off the previous loop duration was to the ideal send period. Result will adjust the
                // next send delay.
                delay_us += period_us + (int32_t)(prev_time - send_time);
            } else {
                // Last packet "send" was not successful. Ideally we should try to catch up the whole previous loop duration (e.g. prev_time - send_time).
                // However, that's not possible since the most probable reason why the send was unsuccessful is the HW was not able to process the packet.
                // Hence, we cannot queue more packets with shorter (or no) delay to catch up since we are already at the performance edge. The best we
                // can do is to reset the send delay (which is probably big negative number) and start all over again.
                delay_us = 0;
            }
            prev_time = send_time;
        }
        *pkt_id_p = htonl(pkt_cnt); // datagrams need to be sequentially numbered
        if (type == IPERF_TRANS_TYPE_UDP) {
            iperf_set_udp_datagram((iperf_udp_datagram_t *)buffer, pkt_cnt, iperf_time_now_us());
        }
        actual_send = sendto(send_socket, buffer, want_send, 0, (struct sockaddr *)&dest_addr, socklen);
        if (actual_send != want_send) {
            if (type == IPERF_TRANS_TYPE_UDP) {
                err = iperf_get_socket_error_code(send_socket);
                // ENOMEM is expected under heavy load => do not print it
                if (err != ENOMEM) {
                    iperf_show_socket_error_reason(error_log, send_socket);
                }
                // A tiny backoff avoids busy-looping on TX queue pressure.
                iperf_delay_us(IPERF_UDP_SEND_FAIL_DELAY_US);
            } else if (type == IPERF_TRANS_TYPE_TCP) {
                iperf_show_socket_error_reason(error_log, send_socket);
                break;
            }
        } else {
            if (pkt_cnt >= INT32_MAX) {
                pkt_cnt = 0;
            } else {
                pkt_cnt++;
            }
            if (type == IPERF_TRANS_TYPE_UDP) {
                last_udp_packet_id = pkt_cnt - 1;
                udp_sent_any = true;
            }
            s_iperf_ctrl.actual_len += actual_send;
            s_iperf_ctrl.tot_len += actual_send;
            if (s_iperf_ctrl.cfg.num_bytes > 0 && s_iperf_ctrl.tot_len >= s_iperf_ctrl.cfg.num_bytes) {
                break;
            }
        }
        // The send delay may be negative, it indicates we are trying to catch up and hence to not delay the loop at all.
        if (delay_us > 0) {
            iperf_delay_us(delay_us);
        }
    }
    if (type == IPERF_TRANS_TYPE_UDP && udp_sent_any) {
        iperf_udp_client_wait_ackfin(send_socket, (struct sockaddr *)&dest_addr, socklen, last_udp_packet_id);
    }
}

static iperf_err_t IRAM_ATTR iperf_run_tcp_server(void)
{
    int listen_socket = -1;
    int client_socket = -1;
    int opt = 1;
    int err = 0;
    iperf_err_t ret = IPERF_OK;
    struct sockaddr_in remote_addr;
    struct timeval timeout = { 0 };
    socklen_t addr_len = sizeof(struct sockaddr);
    struct sockaddr_storage listen_addr = { 0 };
#if IPERF_V6
    struct sockaddr_in6 listen_addr6 = { 0 };
#endif
    struct sockaddr_in listen_addr4 = { 0 };

    IPERF_GOTO_ON_FALSE((s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6 || s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4), IPERF_FAIL, exit, TAG, "Ivalid AF types");

#if IPERF_V6
    if (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6) {
        // The TCP server listen at the address "::", which means all addresses can be listened to.
        inet6_aton("::", &listen_addr6.sin6_addr);
        listen_addr6.sin6_family = AF_INET6;
        listen_addr6.sin6_port = htons(s_iperf_ctrl.cfg.sport);

        listen_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_IPV6);
        IPERF_GOTO_ON_FALSE((listen_socket >= 0), IPERF_FAIL, exit, TAG, "Unable to create socket: errno %d", errno);

        setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        setsockopt(listen_socket, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt));

        IPERF_LOGI(TAG, "Socket created");

        err = bind(listen_socket, (struct sockaddr *)&listen_addr6, sizeof(listen_addr6));
        IPERF_GOTO_ON_FALSE((err == 0), IPERF_FAIL, exit, TAG, "Socket unable to bind: errno %d, IPPROTO: %d", errno, AF_INET6);
        err = listen(listen_socket, 1);
        IPERF_GOTO_ON_FALSE((err == 0), IPERF_FAIL, exit, TAG, "Error occurred during listen: errno %d", errno);

        timeout.tv_sec = IPERF_SOCKET_RX_TIMEOUT;
        setsockopt(listen_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        memcpy(&listen_addr, &listen_addr6, sizeof(listen_addr6));
    } else
#endif
        if (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4) {
        listen_addr4.sin_family = AF_INET;
        listen_addr4.sin_port = htons(s_iperf_ctrl.cfg.sport);
        listen_addr4.sin_addr.s_addr = s_iperf_ctrl.cfg.source_ip4;

        listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        IPERF_GOTO_ON_FALSE((listen_socket >= 0), IPERF_FAIL, exit, TAG, "Unable to create socket: errno %d", errno);

        setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        IPERF_LOGI(TAG, "Socket created");

        err = bind(listen_socket, (struct sockaddr *)&listen_addr4, sizeof(listen_addr4));
        IPERF_GOTO_ON_FALSE((err == 0), IPERF_FAIL, exit, TAG, "Socket unable to bind: errno %d, IPPROTO: %d", errno, AF_INET);

        err = listen(listen_socket, 5);
        IPERF_GOTO_ON_FALSE((err == 0), IPERF_FAIL, exit, TAG, "Error occurred during listen: errno %d", errno);
        memcpy(&listen_addr, &listen_addr4, sizeof(listen_addr4));
    }

    client_socket = accept(listen_socket, (struct sockaddr *)&remote_addr, &addr_len);
    IPERF_GOTO_ON_FALSE((client_socket >= 0), IPERF_FAIL, exit, TAG, "Unable to accept connection: errno %d", errno);
    IPERF_LOGI(TAG, "accept: %s,%d\n", inet_ntoa(remote_addr.sin_addr), htons(remote_addr.sin_port));

    timeout.tv_sec = IPERF_SOCKET_RX_TIMEOUT;
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    socket_recv(client_socket, listen_addr, IPERF_TRANS_TYPE_TCP);
exit:
    if (client_socket != -1) {
        close(client_socket);
    }

    if (listen_socket != -1) {
        shutdown(listen_socket, 0);
        close(listen_socket);
        IPERF_LOGI(TAG, "TCP Socket server is closed.");
    }
    s_iperf_ctrl.finish = true;
    return ret;
}

static void IRAM_ATTR iperf_tcp_dual_server_task(void *pvParameters)
{
    int listen_socket = -1;
    int client_socket = -1;
    int opt = 1;
    int err = 0;
    iperf_err_t ret = IPERF_OK;
    struct sockaddr_in remote_addr;
    struct timeval timeout = { 0 };
    socklen_t addr_len = sizeof(struct sockaddr);
    struct sockaddr_storage listen_addr = { 0 };
    struct sockaddr_in listen_addr4 = { 0 };

    (void)ret;
    if (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4) {
        listen_addr4.sin_family = AF_INET;
        listen_addr4.sin_port = htons(s_iperf_ctrl.cfg.sport);
        listen_addr4.sin_addr.s_addr = s_iperf_ctrl.cfg.source_ip4;

        listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        IPERF_GOTO_ON_FALSE((listen_socket >= 0), IPERF_FAIL, exit, TAG, "Unable to create socket: errno %d", errno);

        setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        IPERF_LOGI(TAG, "Socket created");

        err = bind(listen_socket, (struct sockaddr *)&listen_addr4, sizeof(listen_addr4));
        IPERF_GOTO_ON_FALSE((err == 0), IPERF_FAIL, exit, TAG, "Socket unable to bind: errno %d, IPPROTO: %d", errno, AF_INET);

        err = listen(listen_socket, 5);
        IPERF_GOTO_ON_FALSE((err == 0), IPERF_FAIL, exit, TAG, "Error occurred during listen: errno %d", errno);
        memcpy(&listen_addr, &listen_addr4, sizeof(listen_addr4));
    }

    client_socket = accept(listen_socket, (struct sockaddr *)&remote_addr, &addr_len);
    IPERF_GOTO_ON_FALSE((client_socket >= 0), IPERF_FAIL, exit, TAG, "Unable to accept connection: errno %d", errno);
    IPERF_LOGI(TAG, "accept: %s,%d\n", inet_ntoa(remote_addr.sin_addr), htons(remote_addr.sin_port));

    timeout.tv_sec = IPERF_SOCKET_RX_TIMEOUT;
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    socket_recv_dual(client_socket, listen_addr, IPERF_TRANS_TYPE_TCP);
exit:
    if (client_socket != -1) {
        close(client_socket);
    }

    if (listen_socket != -1) {
        shutdown(listen_socket, 0);
        close(listen_socket);
        IPERF_LOGI(TAG, "TCP Socket server is closed.");
    }

    vTaskDelete(NULL);
}

static iperf_err_t iperf_run_tcp_client(void)
{
    int client_socket = -1;
    int err = 0;
    iperf_err_t ret = IPERF_OK;
    struct sockaddr_storage dest_addr = { 0 };
#if IPERF_V6
    struct sockaddr_in6 dest_addr6 = { 0 };
#endif
    struct sockaddr_in dest_addr4 = { 0 };
    int opt = s_iperf_ctrl.cfg.tos;

    IPERF_GOTO_ON_FALSE((s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6 || s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4), IPERF_FAIL, exit, TAG, "Ivalid AF types");

    if (iperf_is_tcp_dual_client()) {
        xTaskCreate(iperf_tcp_dual_server_task, "dual_rx", IPERF_TRAFFIC_TASK_STACK, NULL, s_iperf_ctrl.cfg.traffic_task_priority, NULL);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
#if IPERF_V6
    if (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6) {
        client_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_IPV6);
        IPERF_GOTO_ON_FALSE((client_socket >= 0), IPERF_FAIL, exit, TAG, "Unable to create socket: errno %d", errno);
        setsockopt(client_socket, IPPROTO_IP, IP_TOS, &opt, sizeof(opt));

        inet6_aton(s_iperf_ctrl.cfg.destination_ip6, &dest_addr6.sin6_addr);
        dest_addr6.sin6_family = AF_INET6;
        dest_addr6.sin6_port = htons(s_iperf_ctrl.cfg.dport);

        err = connect(client_socket, (struct sockaddr *)&dest_addr6, sizeof(struct sockaddr_in6));
        IPERF_GOTO_ON_FALSE((err == 0), IPERF_FAIL, exit, TAG, "Socket unable to connect: errno %d", errno);
        IPERF_LOGI(TAG, "Successfully connected");
        memcpy(&dest_addr, &dest_addr6, sizeof(dest_addr6));
    } else
#endif
        if (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4) {
        client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        IPERF_GOTO_ON_FALSE((client_socket >= 0), IPERF_FAIL, exit, TAG, "Unable to create socket: errno %d", errno);
        setsockopt(client_socket, IPPROTO_IP, IP_TOS, &opt, sizeof(opt));

        dest_addr4.sin_family = AF_INET;
        dest_addr4.sin_port = htons(s_iperf_ctrl.cfg.dport);
        dest_addr4.sin_addr.s_addr = s_iperf_ctrl.cfg.destination_ip4;
        err = connect(client_socket, (struct sockaddr *)&dest_addr4, sizeof(struct sockaddr_in));
        IPERF_GOTO_ON_FALSE((err == 0), IPERF_FAIL, exit, TAG, "Socket unable to connect: errno %d", errno);
        IPERF_LOGI(TAG, "Successfully connected");
        memcpy(&dest_addr, &dest_addr4, sizeof(dest_addr4));
    }

    socket_send(client_socket, dest_addr, IPERF_TRANS_TYPE_TCP, s_iperf_ctrl.cfg.bw_lim);
exit:
    if (client_socket != -1) {
        shutdown(client_socket, 0);
        close(client_socket);
        IPERF_LOGI(TAG, "TCP Socket client is closed.");
    }
    s_iperf_ctrl.finish = true;
    return ret;
}

static iperf_err_t IRAM_ATTR iperf_run_udp_server(void)
{
    int listen_socket = -1;
    int opt = 1;
    int err = 0;
    iperf_err_t ret = IPERF_OK;
    struct timeval timeout = { 0 };
    struct sockaddr_storage listen_addr = { 0 };
#if IPERF_V6
    struct sockaddr_in6 listen_addr6 = { 0 };
#endif
    struct sockaddr_in listen_addr4 = { 0 };

    IPERF_GOTO_ON_FALSE((s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6 || s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4), IPERF_FAIL, exit, TAG, "Ivalid AF types");

#if IPERF_V6
    if (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6) {
        // The UDP server listen at the address "::", which means all addresses can be listened to.
        inet6_aton("::", &listen_addr6.sin6_addr);
        listen_addr6.sin6_family = AF_INET6;
        listen_addr6.sin6_port = htons(s_iperf_ctrl.cfg.sport);

        listen_socket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
        IPERF_GOTO_ON_FALSE((listen_socket >= 0), IPERF_FAIL, exit, TAG, "Unable to create socket: errno %d", errno);
        IPERF_LOGI(TAG, "Socket created");

        setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        err = bind(listen_socket, (struct sockaddr *)&listen_addr6, sizeof(struct sockaddr_in6));
        IPERF_GOTO_ON_FALSE((err == 0), IPERF_FAIL, exit, TAG, "Socket unable to bind: errno %d", errno);
        IPERF_LOGI(TAG, "Socket bound, port %d", listen_addr6.sin6_port);

        memcpy(&listen_addr, &listen_addr6, sizeof(listen_addr6));
    } else
#endif
        if (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4) {
        listen_addr4.sin_family = AF_INET;
        listen_addr4.sin_port = htons(s_iperf_ctrl.cfg.sport);
        listen_addr4.sin_addr.s_addr = s_iperf_ctrl.cfg.source_ip4;

        listen_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        IPERF_GOTO_ON_FALSE((listen_socket >= 0), IPERF_FAIL, exit, TAG, "Unable to create socket: errno %d", errno);
        IPERF_LOGI(TAG, "Socket created");

        setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        err = bind(listen_socket, (struct sockaddr *)&listen_addr4, sizeof(struct sockaddr_in));
        IPERF_GOTO_ON_FALSE((err == 0), IPERF_FAIL, exit, TAG, "Socket unable to bind: errno %d", errno);
        IPERF_LOGI(TAG, "Socket bound, port %d", listen_addr4.sin_port);
        memcpy(&listen_addr, &listen_addr4, sizeof(listen_addr4));
    }

    timeout.tv_sec = IPERF_SOCKET_RX_TIMEOUT;
    setsockopt(listen_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    socket_recv(listen_socket, listen_addr, IPERF_TRANS_TYPE_UDP);
exit:
    if (listen_socket != -1) {
        shutdown(listen_socket, 0);
        close(listen_socket);
    }
    IPERF_LOGI(TAG, "Udp socket server is closed.");
    s_iperf_ctrl.finish = true;
    return ret;
}

static iperf_err_t iperf_run_udp_client(void)
{
    int client_socket = -1;
    int opt = 1;
    iperf_err_t ret = IPERF_OK;
    struct sockaddr_storage dest_addr = { 0 };
#if IPERF_V6
    struct sockaddr_in6 dest_addr6 = { 0 };
#endif
    struct sockaddr_in dest_addr4 = { 0 };

    IPERF_GOTO_ON_FALSE((s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6 || s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4), IPERF_FAIL, exit, TAG, "Ivalid AF types");

#if IPERF_V6
    if (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6) {
        inet6_aton(s_iperf_ctrl.cfg.destination_ip6, &dest_addr6.sin6_addr);
        dest_addr6.sin6_family = AF_INET6;
        dest_addr6.sin6_port = htons(s_iperf_ctrl.cfg.dport);

        client_socket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_IPV6);
        IPERF_GOTO_ON_FALSE((client_socket >= 0), IPERF_FAIL, exit, TAG, "Unable to create socket: errno %d", errno);
        IPERF_LOGI(TAG, "Socket created, sending to %s:%d", s_iperf_ctrl.cfg.destination_ip6, s_iperf_ctrl.cfg.dport);

        setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        opt = s_iperf_ctrl.cfg.tos;
        setsockopt(client_socket, IPPROTO_IP, IP_TOS, &opt, sizeof(opt));
        memcpy(&dest_addr, &dest_addr6, sizeof(dest_addr6));
    } else
#endif
        if (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4) {
        dest_addr4.sin_family = AF_INET;
        dest_addr4.sin_port = htons(s_iperf_ctrl.cfg.dport);
        dest_addr4.sin_addr.s_addr = s_iperf_ctrl.cfg.destination_ip4;

        client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        IPERF_GOTO_ON_FALSE((client_socket >= 0), IPERF_FAIL, exit, TAG, "Unable to create socket: errno %d", errno);
        IPERF_LOGI(TAG, "Socket created, sending to %s:%d", inet_ntoa(dest_addr4.sin_addr), s_iperf_ctrl.cfg.dport);

        setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        opt = s_iperf_ctrl.cfg.tos;
        setsockopt(client_socket, IPPROTO_IP, IP_TOS, &opt, sizeof(opt));
        memcpy(&dest_addr, &dest_addr4, sizeof(dest_addr4));
    }

    socket_send(client_socket, dest_addr, IPERF_TRANS_TYPE_UDP, s_iperf_ctrl.cfg.bw_lim);
exit:
    if (client_socket != -1) {
        shutdown(client_socket, 0);
        close(client_socket);
    }
    s_iperf_ctrl.finish = true;
    IPERF_LOGI(TAG, "UDP Socket client is closed");
    return ret;
}

static void iperf_task_traffic(void *arg)
{
    if (iperf_is_udp_client()) {
        iperf_run_udp_client();
    } else if (iperf_is_udp_server()) {
        iperf_run_udp_server();
    } else if (iperf_is_tcp_client()) {
        iperf_run_tcp_client();
    } else {
        iperf_run_tcp_server();
    }

    if (s_iperf_ctrl.buffer) {
        free(s_iperf_ctrl.buffer);
        s_iperf_ctrl.buffer = NULL;
    }
    printf("iperf exit\r\n");
    s_iperf_is_running = false;
    vTaskDelete(NULL);
}

static uint32_t iperf_get_buffer_len(void)
{
    if (iperf_is_udp_client()) {
        return (s_iperf_ctrl.cfg.len_buf == 0 ? IPERF_UDP_TX_LEN : s_iperf_ctrl.cfg.len_buf);
    } else if (iperf_is_udp_server()) {
        return IPERF_UDP_RX_LEN;
    } else if (iperf_is_tcp_client()) {
        return (s_iperf_ctrl.cfg.len_buf == 0 ? IPERF_TCP_TX_LEN : s_iperf_ctrl.cfg.len_buf);
    } else {
        return (s_iperf_ctrl.cfg.len_buf == 0 ? IPERF_TCP_RX_LEN : s_iperf_ctrl.cfg.len_buf);
    }
    return 0;
}

static void net_iperf_print_header(iperf_cfg_t *cfg)
{
    printf("------------------------------------------------------------\r\n");
    if (iperf_is_udp_server()) {
        printf("Server listening on UDP port %d\r\n",
               cfg->sport);
    } else if (iperf_is_tcp_server()) {
        printf("Server listening on TCP port %d\r\n",
               cfg->sport);
    } else if (iperf_is_udp_client()) {
        printf("Client connecting to %s, UDP port %d\r\n"
               "Sending %d byte datagrams\r\n",
               inet_ntoa(cfg->destination_ip4),
               cfg->dport, cfg->num_bytes);
    } else if (iperf_is_tcp_client()) {
        printf("Client connecting to %s, TCP port %d\r\n",
               inet_ntoa(cfg->destination_ip4), cfg->dport);
    }
    printf("------------------------------------------------------------\r\n");
}

iperf_err_t iperf_start(iperf_cfg_t *cfg)
{
    BaseType_t ret;

    if (!cfg) {
        return IPERF_FAIL;
    }

    if (s_iperf_is_running) {
        IPERF_LOGW(TAG, "iperf is running");
        printf("iperf is running\r\n");
        return IPERF_FAIL;
    }

    memset(&s_iperf_ctrl, 0, sizeof(s_iperf_ctrl));
    memcpy(&s_iperf_ctrl.cfg, cfg, sizeof(*cfg));
    s_iperf_ctrl.udp_last_id = -1;
    s_iperf_is_running = true;
    s_iperf_ctrl.finish = false;
    s_iperf_ctrl.buffer_len = iperf_get_buffer_len();
    s_iperf_ctrl.buffer = (uint8_t *)malloc(s_iperf_ctrl.buffer_len);
    if (!s_iperf_ctrl.buffer) {
        IPERF_LOGE(TAG, "create buffer: not enough memory");
        return IPERF_FAIL;
    }
    memset(s_iperf_ctrl.buffer, 0, s_iperf_ctrl.buffer_len);
    net_iperf_print_header(cfg);
    ret = xTaskCreatePinnedToCore(iperf_task_traffic, IPERF_TRAFFIC_TASK_NAME, IPERF_TRAFFIC_TASK_STACK, NULL, s_iperf_ctrl.cfg.traffic_task_priority, NULL, portNUM_PROCESSORS - 1);
    if (ret != pdPASS) {
        IPERF_LOGE(TAG, "create task %s failed", IPERF_TRAFFIC_TASK_NAME);
        free(s_iperf_ctrl.buffer);
        s_iperf_ctrl.buffer = NULL;
        return IPERF_FAIL;
    }
    return IPERF_OK;
}

iperf_err_t iperf_stop(void)
{
    if (s_iperf_is_running) {
        s_iperf_ctrl.finish = true;
    }

    return IPERF_OK;
}
