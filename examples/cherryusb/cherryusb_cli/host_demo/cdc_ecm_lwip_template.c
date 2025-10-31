#include "bflb_mtimer.h"

#include "lwip/opt.h"
#include "lwip/tcpip.h"
#include "lwip/timeouts.h"
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

#define CONFIG_USBHOST_CDC_ECM_ETH_MAX_SIZE 1514U

static struct usbh_cdc_ecm *g_cdc_ecm_class;

static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t g_cdc_ecm_out_buffer[USB_ALIGN_UP(CONFIG_USBHOST_CDC_ECM_ETH_MAX_SIZE, CONFIG_USB_ALIGN_SIZE)];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t g_cdc_ecm_in_buffer[USB_ALIGN_UP(CONFIG_USBHOST_CDC_ECM_ETH_MAX_SIZE, CONFIG_USB_ALIGN_SIZE)];
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t g_cdc_ecm_inttx_buffer[USB_ALIGN_UP(16, CONFIG_USB_ALIGN_SIZE)];

static volatile bool g_ecm_exit_flag = false;
static volatile bool g_ecm_int_busy_flag = false;
static volatile bool g_ecm_out_busy_flag = false;
static volatile bool g_ecm_in_busy_flag = false;
static volatile uint32_t g_ecm_in_size = 0;

static usb_osal_sem_t g_usbh_cdc_ecm_in_sem = NULL;
static usb_osal_sem_t g_usbh_cdc_ecm_out_sem = NULL;

#if LWIP_DHCP

#define DHCP_OFF              (uint8_t)0
#define DHCP_START            (uint8_t)1
#define DHCP_WAIT_ADDRESS     (uint8_t)2
#define DHCP_ADDRESS_ASSIGNED (uint8_t)3
#define DHCP_TIMEOUT          (uint8_t)4
#define DHCP_LINK_DOWN        (uint8_t)5

#define DHCP_MAX_DHCP_TRIES   (3)
#define DHCP_RETRY_DELAY_MS   (10000)

static int dhcp_sta = DHCP_LINK_DOWN;
static int link_sta = 0;

void dhcp_sta_update(struct netif *netif)
{
    static uint32_t time_ms;

    struct dhcp *dhcp;
    ip4_addr_t ip_zero;

    uint8_t iptxt[20];

    switch (dhcp_sta) {
        case DHCP_OFF:
            /* dhcp disable */
            break;

        case DHCP_START:
            link_sta = netif_is_link_up(netif);
            if (!link_sta) {
                dhcp_sta = DHCP_LINK_DOWN;
                break;
            }

            /* dhcp start */
            ip4_addr_set_zero(&ip_zero);
            netifapi_netif_set_addr(netif, &ip_zero, &ip_zero, &ip_zero);
            netifapi_dhcp_start(netif);
            dhcp_sta = DHCP_WAIT_ADDRESS;
            LOG_I("State: Looking for DHCP server ...\r\n");
            break;

        case DHCP_WAIT_ADDRESS:
            /* check link sta */
            if (!link_sta) {
                netifapi_dhcp_stop(netif);
                dhcp_sta = DHCP_LINK_DOWN;
                LOG_W("DHCP aborted due to link down!\r\n");
                break;
            }
            /* wait and retry */
            if (dhcp_supplied_address(netif)) {
                dhcp_sta = DHCP_ADDRESS_ASSIGNED;
                snprintf((char *)iptxt, sizeof(iptxt), "%s", ip4addr_ntoa(netif_ip4_addr(netif)));
                LOG_I("IP address assigned by a DHCP server: %s\r\n", iptxt);
            } else {
                dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
                if (dhcp->tries > DHCP_MAX_DHCP_TRIES) {
                    netifapi_dhcp_stop(netif);
                    time_ms = bflb_mtimer_get_time_ms();
                    dhcp_sta = DHCP_TIMEOUT;
                    LOG_E("DHCP Timeout !! \r\n");
                }
            }
            break;

        case DHCP_TIMEOUT:
            /* dhcp timeout */
            link_sta = netif_is_link_up(netif);
            if (!link_sta) {
                dhcp_sta = DHCP_LINK_DOWN;
            }
            /* retry */
            if (bflb_mtimer_get_time_ms() - time_ms > DHCP_RETRY_DELAY_MS) {
                dhcp_sta = DHCP_START;
            }
            break;

        case DHCP_ADDRESS_ASSIGNED:
            link_sta = netif_is_link_up(netif);
            if (!link_sta) {
                netifapi_dhcp_stop(netif);
                dhcp_sta = DHCP_LINK_DOWN;
            }
            break;

        case DHCP_LINK_DOWN:
            if (netif_is_link_up(netif) && !link_sta) {
                dhcp_sta = DHCP_START;
            }
            break;

        default:
            LOG_E("dhcp sta err:%d\r\n", dhcp_sta);
            break;
    }
}

#endif

static void usbh_cdc_ecm_int_callback(void *arg, int nbytes)
{
    struct usbh_cdc_ecm *cdc_ecm_class = (struct usbh_cdc_ecm *)arg;

    if (nbytes < 0) {
        USB_LOG_ERR("USBH int error,ret:%d\r\n", nbytes);
        g_ecm_int_busy_flag = false;
        return;
    }

    if (g_cdc_ecm_inttx_buffer[1] == CDC_ECM_NOTIFY_CODE_NETWORK_CONNECTION) {
        if (g_cdc_ecm_inttx_buffer[2] == CDC_ECM_NET_CONNECTED) {
            USB_LOG_INFO("CDC ECM link up\r\n");
            cdc_ecm_class->connect_status = true;
        } else {
            USB_LOG_INFO("CDC ECM link down\r\n");
            cdc_ecm_class->connect_status = false;
        }
    } else if (g_cdc_ecm_inttx_buffer[1] == CDC_ECM_NOTIFY_CODE_CONNECTION_SPEED_CHANGE) {
        memcpy(cdc_ecm_class->speed, &g_cdc_ecm_inttx_buffer[8], 8);
        USB_LOG_INFO("CDC ECM speed change, up: %uMbps, down:%uMbps\r\n", cdc_ecm_class->speed[0] / 1000000, cdc_ecm_class->speed[1] / 1000000);
    }

    g_ecm_int_busy_flag = false;

    if (g_usbh_cdc_ecm_in_sem) {
        usb_osal_sem_give(g_usbh_cdc_ecm_in_sem);
    }
}

static void usbh_cdc_ecm_in_callback(void *arg, int nbytes)
{
    // struct usbh_cdc_ecm *cdc_ecm_class = (struct usbh_cdc_ecm *)arg;

    if (nbytes < 0) {
        USB_LOG_ERR("USBH bulk in error,ret:%d\r\n", nbytes);
        g_ecm_in_size = 0;
    } else {
        USB_LOG_DBG("in len:%d\r\n", nbytes);
        g_ecm_in_size = nbytes;
    }

    g_ecm_in_busy_flag = false;

    if (g_usbh_cdc_ecm_in_sem) {
        usb_osal_sem_give(g_usbh_cdc_ecm_in_sem);
    }
}

static void usbh_cdc_ecm_out_callback(void *arg, int nbytes)
{
    // struct usbh_cdc_ecm *cdc_ecm_class = (struct usbh_cdc_ecm *)arg;

    if (nbytes < 0) {
        USB_LOG_ERR("USBH bulk out error,ret:%d\r\n", nbytes);
    } else {
        USB_LOG_DBG("out len:%d\r\n", nbytes);
    }

    g_ecm_out_busy_flag = false;

    if (g_usbh_cdc_ecm_out_sem) {
        usb_osal_sem_give(g_usbh_cdc_ecm_out_sem);
    }
}

static err_t usbh_cdc_ecm_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q;
    uint16_t byte_copy = 0;
    int ret;

    if (g_cdc_ecm_class->connect_status == false) {
        return ERR_IF;
    }

    if (g_ecm_exit_flag == true) {
        return ERR_IF;
    }

    if (g_ecm_out_busy_flag) {
        uint32_t time_start = bflb_mtimer_get_time_ms();
        while (g_ecm_out_busy_flag) {
            if ((bflb_mtimer_get_time_ms() - time_start) > 100) {
                USB_LOG_ERR("ecm out busy timeout\r\n");
                return ERR_TIMEOUT;
            }
            if (g_usbh_cdc_ecm_out_sem) {
                usb_osal_sem_take(g_usbh_cdc_ecm_out_sem, 10);
            } else {
                USB_LOG_WRN("usbh ecm sem null\r\n");
            }
        }
    }

    for (q = p; q != NULL; q = q->next) {
        /* size check */
        if (byte_copy + q->len > CONFIG_USBHOST_CDC_ECM_ETH_MAX_SIZE) {
            LOG_E("tx size over\r\n");
            return ERR_BUF;
        }
        /* data copy */
        memcpy((char *)g_cdc_ecm_out_buffer + byte_copy, q->payload, q->len);
        byte_copy += q->len;
    }

    g_ecm_out_busy_flag = true;
    usbh_bulk_urb_fill(&g_cdc_ecm_class->bulkout_urb, g_cdc_ecm_class->hport, g_cdc_ecm_class->bulkout, g_cdc_ecm_out_buffer, byte_copy, 0, usbh_cdc_ecm_out_callback, g_cdc_ecm_class);
    ret = usbh_submit_urb(&g_cdc_ecm_class->bulkout_urb);
    if (ret < 0) {
        USB_LOG_RAW("bulk out submit error,ret:%d\r\n", ret);
        g_ecm_out_busy_flag = false;
        g_ecm_exit_flag = true;
        return ERR_IF;
    }

    return 0;
}

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

    netif_set_up(netif);
    return ERR_OK;
}

static volatile bool usbh_ecm_run_flag = false;

static void usbh_cdc_ecm_input_thread(CONFIG_USB_OSAL_THREAD_SET_ARGV)
{
    struct usbh_cdc_ecm *cdc_ecm_class = (struct usbh_cdc_ecm *)CONFIG_USB_OSAL_THREAD_GET_ARGV;
    struct pbuf *p, *q;
    uint32_t byte_copy;
    uint32_t dhcp_time = 0;
    uint32_t int_time = 0;
    int ret;

    USB_LOG_RAW("USBH CDC ECM LWIP test start\r\n");

    g_ecm_int_busy_flag = false;
    g_ecm_out_busy_flag = false;
    g_ecm_exit_flag = false;
    g_ecm_in_busy_flag = false;
    g_ecm_in_size = 0;

    g_cdc_ecm_class = cdc_ecm_class;
    cdc_ecm_class->connect_status = false;

    g_usbh_cdc_ecm_in_sem = usb_osal_sem_create(0);
    g_usbh_cdc_ecm_out_sem = usb_osal_sem_create(0);
    usb_osal_msleep(10);

    /* lwip netif init */
    struct netif *netif = &g_cdc_ecm_netif;
    netif->hwaddr_len = 6;
    memcpy(netif->hwaddr, cdc_ecm_class->mac, 6);
    IP4_ADDR(&g_ipaddr, 0, 0, 0, 0);
    IP4_ADDR(&g_netmask, 0, 0, 0, 0);
    IP4_ADDR(&g_gateway, 0, 0, 0, 0);
    netif = netif_add(netif, &g_ipaddr, &g_netmask, &g_gateway, NULL, usbh_cdc_ecm_if_init, tcpip_input);
    netif_set_default(netif);

#if LWIP_DHCP
    dhcp_sta = DHCP_LINK_DOWN;
    link_sta = 0;
#endif

    if (usbh_find_class_instance("/dev/cdc_ether") == NULL) {
        goto exit_del;
    }

    while (1) {
        usb_osal_sem_take(g_usbh_cdc_ecm_in_sem, 10);

        if (g_ecm_exit_flag == true) {
            USB_LOG_ERR("usbh ecm exit\r\n");
            g_ecm_exit_flag = false;
            goto exit_del;
        }

        /* get connect status */
        if (g_ecm_int_busy_flag == false) {
            if (cdc_ecm_class->connect_status == false) {
                if (netif_is_link_up(netif)) {
                    USB_LOG_RAW("CDC ECM netif link down\r\n");
                    netif_set_link_down(netif);
                }
            } else {
                if (!netif_is_link_up(netif)) {
                    USB_LOG_RAW("CDC ECM netif link up\r\n");
                    netif_set_link_up(netif);
                }
            }

            if (bflb_mtimer_get_time_ms() - int_time > 10) {
                int_time = bflb_mtimer_get_time_ms();

                g_ecm_int_busy_flag = true;
                usbh_int_urb_fill(&cdc_ecm_class->intin_urb, cdc_ecm_class->hport, cdc_ecm_class->intin, g_cdc_ecm_inttx_buffer, 16, 0, usbh_cdc_ecm_int_callback, cdc_ecm_class);
                ret = usbh_submit_urb(&cdc_ecm_class->intin_urb);
                if (ret < 0) {
                    USB_LOG_ERR("bulk int submit error,ret:%d\r\n", ret);
                    goto exit_del;
                }
            }
        }

#if LWIP_DHCP
        /* dhcp update */
        if (bflb_mtimer_get_time_ms() - dhcp_time > 100) {
            dhcp_time = bflb_mtimer_get_time_ms();
            dhcp_sta_update(netif);
        }
#endif

        /* wait for input transfer */
        if (g_ecm_in_busy_flag == true) {
            continue;
        }

        /* lwip input */
        if (g_ecm_in_size) {
            p = pbuf_alloc(PBUF_RAW, g_ecm_in_size, PBUF_POOL);
            if (p == NULL) {
                USB_LOG_ERR("No memory to alloc pbuf\r\n");
                g_ecm_in_size = 0;
                continue;
            }
            byte_copy = 0;
            for (q = p; q != NULL; q = q->next) {
                memcpy(q->payload, (char *)g_cdc_ecm_in_buffer + byte_copy, q->len);
                byte_copy += q->len;
            }
            if (netif->input(p, netif) != ERR_OK) {
                pbuf_free(p);
            }
            g_ecm_in_size = 0;
        }

        /* wait for connect (link up) */
        if (cdc_ecm_class->connect_status == false) {
            continue;
        }

        g_ecm_in_busy_flag = true;
        g_ecm_in_size = 0;
        /* start next input transfer */
        usbh_bulk_urb_fill(&cdc_ecm_class->bulkin_urb, cdc_ecm_class->hport, cdc_ecm_class->bulkin, g_cdc_ecm_in_buffer, CONFIG_USBHOST_CDC_ECM_ETH_MAX_SIZE, 0, usbh_cdc_ecm_in_callback, cdc_ecm_class);
        ret = usbh_submit_urb(&cdc_ecm_class->bulkin_urb);
        if (ret < 0) {
            USB_LOG_ERR("bulk in submit error,ret:%d\r\n", ret);
            goto exit_del;
        }
    }

exit_del:

#if LWIP_DHCP
    dhcp_stop(netif);
    dhcp_cleanup(netif);
#endif
    netif_set_down(netif);
    netif_remove(netif);

    usb_osal_sem_delete(g_usbh_cdc_ecm_in_sem);
    g_usbh_cdc_ecm_in_sem = NULL;
    usb_osal_sem_delete(g_usbh_cdc_ecm_out_sem);
    g_usbh_cdc_ecm_out_sem = NULL;

    USB_LOG_WRN("USBH CDC ECM LWIP test end\r\n");
    usbh_ecm_run_flag = false;
    usb_osal_thread_delete(NULL);
}

void usbh_cdc_ecm_run(struct usbh_cdc_ecm *cdc_ecm_class)
{
    USB_LOG_INFO("USBH CDC ECM run\r\n");

    if (usbh_ecm_run_flag == true) {
        USB_LOG_ERR("USBH CDC ECM test already running\r\n");
        return;
    }

    usbh_ecm_run_flag = true;
    usb_osal_thread_create("usbh_cdc_ecm_rx", 2048, CONFIG_USBHOST_PSC_PRIO - 1, usbh_cdc_ecm_input_thread, cdc_ecm_class);
}

void usbh_cdc_ecm_stop(struct usbh_cdc_ecm *cdc_ecm_class)
{
    USB_LOG_INFO("USBH CDC ECM stop\r\n");
    g_ecm_exit_flag = true;
    if (g_usbh_cdc_ecm_in_sem) {
        usb_osal_sem_give(g_usbh_cdc_ecm_in_sem);
    }

    while (usbh_ecm_run_flag) {
        usb_osal_msleep(5);
    }
}
