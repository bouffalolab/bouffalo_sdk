/**
 * @file cdc_ecm_lwip_template.c
 * @brief CherryUSB host CDC ECM network interface for lwIP.
 *
 * This module owns a single CDC ECM class instance and coordinates its startup,
 * asynchronous URBs, lwIP traffic, and teardown through a shared state machine.
 */

#include "bflb_mtimer.h"

#include "lwip/opt.h"
#include "lwip/tcpip.h"
#include "lwip/netif.h"
#if LWIP_DHCP
#include "lwip/dhcp.h"
#endif
#include "lwip/netifapi.h"
#include "netif/etharp.h"

#include "usbh_core.h"
#include "usbh_cdc_ecm.h"

#define DBG_TAG "LWIP_ECM"
#include "log.h"

ip_addr_t g_ipaddr;
ip_addr_t g_netmask;
ip_addr_t g_gateway;

struct netif g_cdc_ecm_netif;

/** @brief Maximum Ethernet frame size accepted by the host ECM buffers. */
#define CONFIG_USBHOST_CDC_ECM_ETH_MAX_SIZE 1514U

/** @brief Lifecycle states for the CDC ECM lwIP worker. */
enum usbh_cdc_ecm_state {
    USBH_CDC_ECM_STOPPED = 0, /**< No worker or URB is active. */
    USBH_CDC_ECM_STARTING,    /**< Worker is creating the lwIP interface. */
    USBH_CDC_ECM_RUNNING,     /**< Interface is active and URBs may be submitted. */
    USBH_CDC_ECM_STOPPING,    /**< Stop requested; the worker is cancelling outstanding URBs. */
};

/** @brief Reason that initiated the current CDC ECM teardown. */
enum usbh_cdc_ecm_stop_reason {
    USBH_CDC_ECM_STOP_NONE = 0,   /**< No stop has been requested. */
    USBH_CDC_ECM_STOP_DISCONNECT, /**< Device disconnect or explicit stop. */
    USBH_CDC_ECM_STOP_INIT_ERROR, /**< lwIP interface initialization failed. */
    USBH_CDC_ECM_STOP_INT_ERROR,  /**< Interrupt-IN transfer failed. */
    USBH_CDC_ECM_STOP_RX_ERROR,   /**< Bulk-IN transfer failed. */
    USBH_CDC_ECM_STOP_TX_ERROR,   /**< Bulk-OUT transfer failed. */
};

/** @brief Shared state and synchronization objects for one ECM worker. */
struct usbh_cdc_ecm_lwip_context {
    struct usbh_cdc_ecm *cdc_ecm_class;        /**< Active class instance. */
    volatile enum usbh_cdc_ecm_state state;    /**< Lifecycle state shared by worker, TX, and stop callers. */
    enum usbh_cdc_ecm_stop_reason stop_reason; /**< First recorded teardown cause. */
    usb_osal_mutex_t state_mutex;              /**< Serializes ownership, stop reason, and shared TX submission. */
    usb_osal_sem_t worker_sem;                 /**< Wakes the worker for state or receive progress. */
    usb_osal_sem_t tx_sem;                     /**< Grants exclusive ownership of the bulk-OUT transfer slot. */
    usb_osal_sem_t stopped_sem;                /**< Signals completion of worker teardown. */
    volatile bool stop_waiting;                /**< True while a caller is synchronously waiting for teardown. */
    volatile bool int_busy;                    /**< Interrupt-IN URB is outstanding. */
    volatile bool out_busy;                    /**< Bulk-OUT URB is outstanding. */
    volatile bool in_busy;                     /**< Bulk-IN URB is outstanding. */
    volatile uint32_t in_size;                 /**< Bytes delivered by the latest bulk-IN completion. */
};

static struct usbh_cdc_ecm_lwip_context g_ecm_ctx = {
    .state = USBH_CDC_ECM_STOPPED,
};

/** @brief DMA-accessible Ethernet transmit buffer. */
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t g_cdc_ecm_out_buffer[USB_ALIGN_UP(CONFIG_USBHOST_CDC_ECM_ETH_MAX_SIZE, CONFIG_USB_ALIGN_SIZE)];
/** @brief DMA-accessible Ethernet receive buffer. */
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t g_cdc_ecm_in_buffer[USB_ALIGN_UP(CONFIG_USBHOST_CDC_ECM_ETH_MAX_SIZE, CONFIG_USB_ALIGN_SIZE)];
/** @brief DMA-accessible CDC ECM notification buffer. */
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t g_cdc_ecm_inttx_buffer[USB_ALIGN_UP(16, CONFIG_USB_ALIGN_SIZE)];

/** @brief Lazily create the synchronization objects used by the ECM context. */
static void usbh_cdc_ecm_context_init(void)
{
    if (g_ecm_ctx.state_mutex == NULL) {
        g_ecm_ctx.state_mutex = usb_osal_mutex_create();
        g_ecm_ctx.worker_sem = usb_osal_sem_create(0);
        g_ecm_ctx.tx_sem = usb_osal_sem_create(0);
        g_ecm_ctx.stopped_sem = usb_osal_sem_create(0);
    }
}

/**
 * @brief Record a stop request for the active ECM context.
 * @param[in] reason Reason for requesting teardown.
 * @pre The caller holds `g_ecm_ctx.state_mutex`.
 */
static void usbh_cdc_ecm_request_stop_locked(enum usbh_cdc_ecm_stop_reason reason)
{
    if (g_ecm_ctx.state == USBH_CDC_ECM_STARTING ||
        g_ecm_ctx.state == USBH_CDC_ECM_RUNNING) {
        g_ecm_ctx.state = USBH_CDC_ECM_STOPPING;
        g_ecm_ctx.stop_reason = reason;
    }
}

/**
 * @brief Request teardown and wake the ECM worker.
 * @param[in] reason Reason for requesting teardown.
 */
static void usbh_cdc_ecm_request_stop(enum usbh_cdc_ecm_stop_reason reason)
{
    usb_osal_mutex_take(g_ecm_ctx.state_mutex);
    usbh_cdc_ecm_request_stop_locked(reason);
    usb_osal_mutex_give(g_ecm_ctx.state_mutex);

    usb_osal_sem_give(g_ecm_ctx.worker_sem);
}

#if LWIP_DHCP && LWIP_NETIF_STATUS_CALLBACK
/**
 * @brief Log an IPv4 address assigned to the ECM interface.
 * @param[in] netif lwIP network interface whose status changed.
 */
static void usbh_cdc_ecm_netif_status_callback(struct netif *netif)
{
    if (netif_is_up(netif) && !ip4_addr_isany_val(*netif_ip4_addr(netif))) {
        LOG_I("IPv4 address: %s\r\n", ip4addr_ntoa(netif_ip4_addr(netif)));
    }
}
#endif

/**
 * @brief Stop addressing services and remove an ECM interface from lwIP.
 * @param[in,out] netif Network interface to take down and remove.
 */
static void usbh_cdc_ecm_netif_cleanup(struct netif *netif)
{
    netif_set_link_down(netif);
#if LWIP_DHCP
    dhcp_release_and_stop(netif);
    dhcp_cleanup(netif);
#endif
    netif_set_down(netif);
    netif_remove(netif);
}

/**
 * @brief Process an ECM notification transfer completion.
 * @param[in,out] arg ECM context supplied with the URB.
 * @param[in] nbytes Number of notification bytes, or a negative error code.
 * @note The callback only updates completion-visible state, clears `int_busy`,
 *       and wakes the worker; it does not submit another URB.
 */
static void usbh_cdc_ecm_int_callback(void *arg, int nbytes)
{
    struct usbh_cdc_ecm_lwip_context *ctx = (struct usbh_cdc_ecm_lwip_context *)arg;
    struct usbh_cdc_ecm *cdc_ecm_class = ctx->cdc_ecm_class;

    if (nbytes < 0) {
        USB_LOG_ERR("USBH int error,ret:%d\r\n", nbytes);
    } else if (g_cdc_ecm_inttx_buffer[1] == CDC_ECM_NOTIFY_CODE_NETWORK_CONNECTION) {
        if (g_cdc_ecm_inttx_buffer[2] == CDC_ECM_NET_CONNECTED && cdc_ecm_class->connect_status == false) {
            USB_LOG_INFO("CDC ECM link up\r\n");
            cdc_ecm_class->connect_status = true;
        } else if (g_cdc_ecm_inttx_buffer[2] == CDC_ECM_NET_DISCONNECTED && cdc_ecm_class->connect_status == true) {
            USB_LOG_INFO("CDC ECM link down\r\n");
            cdc_ecm_class->connect_status = false;
            cdc_ecm_class->speed[0] = 0;
            cdc_ecm_class->speed[1] = 0;
        }
    } else if (g_cdc_ecm_inttx_buffer[1] == CDC_ECM_NOTIFY_CODE_CONNECTION_SPEED_CHANGE) {
        if (memcmp(&g_cdc_ecm_inttx_buffer[8], cdc_ecm_class->speed, 8) != 0) {
            memcpy(cdc_ecm_class->speed, &g_cdc_ecm_inttx_buffer[8], 8);
            USB_LOG_INFO("CDC ECM speed change, up: %uMbps, down:%uMbps\r\n", cdc_ecm_class->speed[0] / 1000000, cdc_ecm_class->speed[1] / 1000000);
        }
    }

    /* No callback-visible data may be accessed after waking the worker. */
    ctx->int_busy = false;
    usb_osal_sem_give(ctx->worker_sem);
}

/**
 * @brief Record a bulk-IN transfer completion and wake the worker.
 * @param[in,out] arg ECM context supplied with the URB.
 * @param[in] nbytes Number of received bytes, or a negative error code.
 * @note The callback only updates receive state and wakes the worker.
 */
static void usbh_cdc_ecm_in_callback(void *arg, int nbytes)
{
    struct usbh_cdc_ecm_lwip_context *ctx = (struct usbh_cdc_ecm_lwip_context *)arg;

    if (nbytes < 0) {
        USB_LOG_ERR("USBH bulk in error,ret:%d\r\n", nbytes);
        ctx->in_size = 0;
    } else {
        USB_LOG_DBG("in len:%d\r\n", nbytes);
        ctx->in_size = nbytes;
    }

    ctx->in_busy = false;
    usb_osal_sem_give(ctx->worker_sem);
}

/**
 * @brief Record a bulk-OUT transfer completion and wake a waiting sender.
 * @param[in,out] arg ECM context supplied with the URB.
 * @param[in] nbytes Number of transmitted bytes, or a negative error code.
 * @note The callback only clears transfer state and signals `tx_sem`.
 */
static void usbh_cdc_ecm_out_callback(void *arg, int nbytes)
{
    struct usbh_cdc_ecm_lwip_context *ctx = (struct usbh_cdc_ecm_lwip_context *)arg;

    if (nbytes < 0) {
        USB_LOG_ERR("USBH bulk out error,ret:%d\r\n", nbytes);
    } else {
        USB_LOG_DBG("out len:%d\r\n", nbytes);
    }

    ctx->out_busy = false;
    usb_osal_sem_give(ctx->tx_sem);
}

/**
 * @brief Submit an lwIP Ethernet frame through the ECM bulk-OUT endpoint.
 * @param[in] netif ECM lwIP interface; currently unused.
 * @param[in] p Packet buffer chain to transmit; ownership remains with lwIP.
 * @retval ERR_OK The frame was copied and submitted.
 * @retval ERR_IF The interface stopped, disconnected, or submission failed.
 * @retval ERR_BUF The complete frame could not be copied.
 * @retval ERR_TIMEOUT The bulk-OUT transfer slot was not released in time.
 * @note `tx_sem` serializes transmitters before they access the shared buffer
 *       and URB. The state mutex then protects the class ownership check and
 *       prevents new submissions after STOPPING.
 */
static err_t usbh_cdc_ecm_output(struct netif *netif, struct pbuf *p)
{
    struct usbh_cdc_ecm *cdc_ecm_class;
    uint16_t byte_copy;
    int ret;

    ret = usb_osal_sem_take(g_ecm_ctx.tx_sem, 100);
    if (ret < 0) {
        USB_LOG_ERR("ecm out busy timeout\r\n");
        return ERR_TIMEOUT;
    }

    usb_osal_mutex_take(g_ecm_ctx.state_mutex);

    cdc_ecm_class = g_ecm_ctx.cdc_ecm_class;
    if (g_ecm_ctx.state != USBH_CDC_ECM_RUNNING ||
        cdc_ecm_class == NULL ||
        cdc_ecm_class->connect_status == false) {
        usb_osal_mutex_give(g_ecm_ctx.state_mutex);
        usb_osal_sem_give(g_ecm_ctx.tx_sem);
        return ERR_IF;
    }

    byte_copy = pbuf_copy_partial(p, g_cdc_ecm_out_buffer, p->tot_len, 0);
    if (byte_copy != p->tot_len) {
        usb_osal_mutex_give(g_ecm_ctx.state_mutex);
        usb_osal_sem_give(g_ecm_ctx.tx_sem);
        LOG_E("tx copy failed\r\n");
        return ERR_BUF;
    }

    g_ecm_ctx.out_busy = true;
    usbh_bulk_urb_fill(&cdc_ecm_class->bulkout_urb, cdc_ecm_class->hport, cdc_ecm_class->bulkout,
                       g_cdc_ecm_out_buffer, byte_copy, 0, usbh_cdc_ecm_out_callback, &g_ecm_ctx);
    ret = usbh_submit_urb(&cdc_ecm_class->bulkout_urb);
    if (ret < 0) {
        USB_LOG_RAW("bulk out submit error,ret:%d\r\n", ret);
        g_ecm_ctx.out_busy = false;
        usbh_cdc_ecm_request_stop_locked(USBH_CDC_ECM_STOP_TX_ERROR);
        usb_osal_mutex_give(g_ecm_ctx.state_mutex);
        usb_osal_sem_give(g_ecm_ctx.tx_sem);
        usb_osal_sem_give(g_ecm_ctx.worker_sem);
        return ERR_IF;
    }

    usb_osal_mutex_give(g_ecm_ctx.state_mutex);
    return ERR_OK;
}

/**
 * @brief Configure lwIP properties and output handlers for the ECM interface.
 * @param[in,out] netif Network interface being initialized.
 * @retval ERR_OK The interface was configured.
 */
static err_t usbh_cdc_ecm_if_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

    netif->mtu = 1500;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
    netif->state = NULL;
    netif->name[0] = 'E';
    netif->name[1] = 'X';
    netif->output = etharp_output;
    netif->linkoutput = usbh_cdc_ecm_output;

#if LWIP_DHCP && LWIP_NETIF_STATUS_CALLBACK
    netif_set_status_callback(netif, usbh_cdc_ecm_netif_status_callback);
#endif
    netif_set_up(netif);
    return ERR_OK;
}

/**
 * @brief Poll the CDC ECM connection status and submit the next notification URB.
 * @param[in,out] ctx Shared ECM context.
 * @param[in] cdc_ecm_class Active CDC ECM class instance.
 * @param[in,out] netif Associated lwIP network interface.
 * @param[in,out] netif_link_up Locally tracked lwIP link state.
 * @param[in,out] int_time Timestamp of the previous notification URB submission.
 * @retval 0 Status processing completed or no notification URB was required.
 * @return A negative USB error code if notification URB submission failed.
 */
static int usbh_cdc_ecm_poll_status(struct usbh_cdc_ecm_lwip_context *ctx,
                                    struct usbh_cdc_ecm *cdc_ecm_class,
                                    struct netif *netif,
                                    bool *netif_link_up,
                                    uint32_t *int_time)
{
    err_t err;
    int ret = 0;

    if (ctx->int_busy) {
        return 0;
    }

    if (cdc_ecm_class->connect_status == false) {
        if (*netif_link_up) {
            USB_LOG_RAW("CDC ECM netif link down\r\n");
            err = netifapi_netif_set_link_down(netif);
            if (err == ERR_OK) {
                *netif_link_up = false;
            } else {
                LOG_E("netifapi_netif_set_link_down failed: %d\r\n", err);
            }
        }
    } else if (*netif_link_up == false) {
        USB_LOG_RAW("CDC ECM netif link up\r\n");
        err = netifapi_netif_set_link_up(netif);
        if (err == ERR_OK) {
            *netif_link_up = true;
        } else {
            LOG_E("netifapi_netif_set_link_up failed: %d\r\n", err);
        }
    }

    if (bflb_mtimer_get_time_ms() - *int_time <= 10) {
        return 0;
    }
    *int_time = bflb_mtimer_get_time_ms();

    if (ctx->state == USBH_CDC_ECM_RUNNING && ctx->int_busy == false) {
        ctx->int_busy = true;
        usbh_int_urb_fill(&cdc_ecm_class->intin_urb, cdc_ecm_class->hport, cdc_ecm_class->intin,
                          g_cdc_ecm_inttx_buffer, 16, 0, usbh_cdc_ecm_int_callback, ctx);
        ret = usbh_submit_urb(&cdc_ecm_class->intin_urb);
        if (ret < 0) {
            USB_LOG_ERR("bulk int submit error,ret:%d\r\n", ret);
            ctx->int_busy = false;
            usbh_cdc_ecm_request_stop(USBH_CDC_ECM_STOP_INT_ERROR);
        }
    }

    return ret;
}

/**
 * @brief Own the ECM lwIP interface, receive path, and ordered teardown.
 *
 * The worker installs the lwIP interface, submits notification and receive
 * URBs only while RUNNING, passes received frames to lwIP, and kills all URBs
 * before removing the interface and releasing the class pointer.
 *
 * @note The ECM context is supplied through the USB OSAL thread argument macro.
 * @note STOPPING prevents new lwIP transmit submissions. Once the worker leaves
 *       its loop, it submits no more receive URBs and cancels all remaining URBs.
 */
static void usbh_cdc_ecm_input_thread(CONFIG_USB_OSAL_THREAD_SET_ARGV)
{
    struct usbh_cdc_ecm_lwip_context *ctx = (struct usbh_cdc_ecm_lwip_context *)CONFIG_USB_OSAL_THREAD_GET_ARGV;
    struct usbh_cdc_ecm *cdc_ecm_class = ctx->cdc_ecm_class;
    struct netif *netif;
    struct pbuf *p;
    uint32_t int_time = 0;
    bool netif_link_up = false;
    bool netif_added = false;
    enum usbh_cdc_ecm_stop_reason stop_reason;
    err_t err;
    int ret;

    USB_LOG_RAW("USBH CDC ECM LWIP test start\r\n");

    cdc_ecm_class->connect_status = false;
    usb_osal_msleep(10);

    if (ctx->state != USBH_CDC_ECM_STARTING) {
        goto exit;
    }

    /* lwip netif init */
    {
        netif = &g_cdc_ecm_netif;
        netif->hwaddr_len = 6;
        memcpy(netif->hwaddr, cdc_ecm_class->mac, 6);
        IP4_ADDR(&g_ipaddr, 0, 0, 0, 0);
        IP4_ADDR(&g_netmask, 0, 0, 0, 0);
        IP4_ADDR(&g_gateway, 0, 0, 0, 0);
        err = netifapi_netif_add(netif, ip_2_ip4(&g_ipaddr), ip_2_ip4(&g_netmask), ip_2_ip4(&g_gateway),
                                 NULL, usbh_cdc_ecm_if_init, tcpip_input);
        if (err != ERR_OK) {
            LOG_E("netifapi_netif_add failed: %d\r\n", err);
            usbh_cdc_ecm_request_stop(USBH_CDC_ECM_STOP_INIT_ERROR);
            goto exit;
        }
        netif_added = true;

        err = netifapi_netif_set_default(netif);
        if (err != ERR_OK) {
            LOG_E("netifapi_netif_set_default failed: %d\r\n", err);
            usbh_cdc_ecm_request_stop(USBH_CDC_ECM_STOP_INIT_ERROR);
            goto exit;
        }

#if LWIP_DHCP
        err = netifapi_dhcp_start(netif);
        if (err != ERR_OK) {
            LOG_E("netifapi_dhcp_start failed: %d\r\n", err);
            usbh_cdc_ecm_request_stop(USBH_CDC_ECM_STOP_INIT_ERROR);
            goto exit;
        }
        LOG_I("DHCP client started\r\n");
#endif
    }

    if (usbh_find_class_instance("/dev/cdc_ether") == NULL) {
        usbh_cdc_ecm_request_stop(USBH_CDC_ECM_STOP_DISCONNECT);
        goto exit;
    }

    usb_osal_mutex_take(ctx->state_mutex);
    if (ctx->state == USBH_CDC_ECM_STARTING) {
        ctx->state = USBH_CDC_ECM_RUNNING;
    }
    usb_osal_mutex_give(ctx->state_mutex);

    while (ctx->state == USBH_CDC_ECM_RUNNING) {
        /* Poll ECM notifications and synchronize the lwIP link state. */
        if (ctx->int_busy == false) {
            ret = usbh_cdc_ecm_poll_status(ctx, cdc_ecm_class, netif, &netif_link_up, &int_time);
            if (ret < 0) {
                break;
            }
        }

        /* wait for input transfer */
        if (ctx->in_busy == false) {
            /* lwip input */
            if (ctx->in_size) {
#if PBUF_POOL_SIZE > 0
                p = pbuf_alloc(PBUF_RAW, ctx->in_size, PBUF_POOL);
#else
                p = pbuf_alloc(PBUF_RAW, ctx->in_size, PBUF_RAM);
#endif
                if (p != NULL) {
                    if (pbuf_take(p, g_cdc_ecm_in_buffer, ctx->in_size) != ERR_OK ||
                        netif->input(p, netif) != ERR_OK) {
                        pbuf_free(p);
                    }
                }
                ctx->in_size = 0;
            }

            /* start next input transfer */
            if (cdc_ecm_class->connect_status && ctx->in_busy == false) {
                ctx->in_busy = true;
                ctx->in_size = 0;

                usbh_bulk_urb_fill(&cdc_ecm_class->bulkin_urb, cdc_ecm_class->hport, cdc_ecm_class->bulkin,
                                   g_cdc_ecm_in_buffer, CONFIG_USBHOST_CDC_ECM_ETH_MAX_SIZE, 0, usbh_cdc_ecm_in_callback, ctx);
                ret = usbh_submit_urb(&cdc_ecm_class->bulkin_urb);
                if (ret < 0) {
                    USB_LOG_ERR("bulk in submit error,ret:%d\r\n", ret);
                    ctx->in_busy = false;
                    usbh_cdc_ecm_request_stop(USBH_CDC_ECM_STOP_RX_ERROR);
                }
            }
        }

        /* wait for transfer complete or stop */
        usb_osal_sem_take(ctx->worker_sem, 10);
    }

exit:
    usb_osal_mutex_take(ctx->state_mutex);
    usbh_cdc_ecm_request_stop_locked(USBH_CDC_ECM_STOP_INIT_ERROR);
    usb_osal_mutex_give(ctx->state_mutex);

    /* No new receive URBs are submitted after the worker leaves the loop, and
     * STOPPING blocks new lwIP transmit submissions. Cancel any remaining URBs;
     * the class disconnect may already have killed them. */
    usbh_kill_urb(&cdc_ecm_class->bulkin_urb);
    usbh_kill_urb(&cdc_ecm_class->bulkout_urb);
    usbh_kill_urb(&cdc_ecm_class->intin_urb);

    if (netif_added) {
        err = netifapi_netif_common(netif, usbh_cdc_ecm_netif_cleanup, NULL);
        if (err != ERR_OK) {
            LOG_E("CDC ECM netif cleanup failed: %d\r\n", err);
        }
    }

    usb_osal_mutex_take(ctx->state_mutex);
    ctx->cdc_ecm_class = NULL;
    ctx->int_busy = false;
    ctx->out_busy = false;
    ctx->in_busy = false;
    ctx->in_size = 0;
    stop_reason = ctx->stop_reason;
    if (ctx->stop_waiting == false) {
        ctx->state = USBH_CDC_ECM_STOPPED;
    }
    usb_osal_mutex_give(ctx->state_mutex);

    USB_LOG_WRN("USBH CDC ECM LWIP test end, reason:%d\r\n", stop_reason);
    usb_osal_sem_give(ctx->stopped_sem);
    usb_osal_thread_delete(NULL);
}

/**
 * @brief Start lwIP networking for a CDC ECM host class instance.
 * @param[in] cdc_ecm_class Enumerated CDC ECM class instance.
 * @note Only one instance can use the module-owned buffers and context.
 */
void usbh_cdc_ecm_run(struct usbh_cdc_ecm *cdc_ecm_class)
{
    USB_LOG_INFO("USBH CDC ECM run\r\n");

    usbh_cdc_ecm_context_init();
    usb_osal_mutex_take(g_ecm_ctx.state_mutex);
    if (g_ecm_ctx.state != USBH_CDC_ECM_STOPPED) {
        usb_osal_mutex_give(g_ecm_ctx.state_mutex);
        USB_LOG_ERR("USBH CDC ECM test already running\r\n");
        return;
    }

    usb_osal_sem_reset(g_ecm_ctx.worker_sem);
    usb_osal_sem_reset(g_ecm_ctx.tx_sem);
    usb_osal_sem_reset(g_ecm_ctx.stopped_sem);
    usb_osal_sem_give(g_ecm_ctx.tx_sem);
    g_ecm_ctx.cdc_ecm_class = cdc_ecm_class;
    g_ecm_ctx.stop_reason = USBH_CDC_ECM_STOP_NONE;
    g_ecm_ctx.stop_waiting = false;
    g_ecm_ctx.int_busy = false;
    g_ecm_ctx.out_busy = false;
    g_ecm_ctx.in_busy = false;
    g_ecm_ctx.in_size = 0;
    g_ecm_ctx.state = USBH_CDC_ECM_STARTING;
    usb_osal_mutex_give(g_ecm_ctx.state_mutex);

    usb_osal_thread_create("usbh_cdc_ecm_rx", 2048, CONFIG_USBHOST_PSC_PRIO - 1,
                           usbh_cdc_ecm_input_thread, &g_ecm_ctx);
}

/**
 * @brief Stop lwIP networking for a CDC ECM host class instance.
 * @param[in] cdc_ecm_class Class instance expected to own the active context.
 * @note This function blocks indefinitely until the worker has killed all
 *       URBs, removed the lwIP interface, and cleared the class pointer.
 */
void usbh_cdc_ecm_stop(struct usbh_cdc_ecm *cdc_ecm_class)
{
    USB_LOG_INFO("USBH CDC ECM stop\r\n");

    if (g_ecm_ctx.state_mutex == NULL) {
        return;
    }

    usb_osal_mutex_take(g_ecm_ctx.state_mutex);
    if (g_ecm_ctx.state == USBH_CDC_ECM_STOPPED ||
        g_ecm_ctx.cdc_ecm_class != cdc_ecm_class) {
        usb_osal_mutex_give(g_ecm_ctx.state_mutex);
        return;
    }
    g_ecm_ctx.stop_waiting = true;
    usbh_cdc_ecm_request_stop_locked(USBH_CDC_ECM_STOP_DISCONNECT);
    usb_osal_mutex_give(g_ecm_ctx.state_mutex);

    usb_osal_sem_give(g_ecm_ctx.worker_sem);
    usb_osal_sem_take(g_ecm_ctx.stopped_sem, USB_OSAL_WAITING_FOREVER);

    usb_osal_mutex_take(g_ecm_ctx.state_mutex);
    g_ecm_ctx.stop_waiting = false;
    g_ecm_ctx.state = USBH_CDC_ECM_STOPPED;
    usb_osal_mutex_give(g_ecm_ctx.state_mutex);
}
