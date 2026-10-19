#include "app_wifi.h"

#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"

#include "async_event.h"
#include "fhost_api.h"
#include "mm.h"
#include "wifi_mgmr.h"
#include "wifi_mgmr_ext.h"

#include "nethub.h"
#include "nethub_filter.h"
#include "lwip/def.h"
#include "lwip/pbuf.h"
#include "lwip/prot/ethernet.h"
#include "lwip/prot/ip.h"
#include "lwip/prot/ip4.h"
#include "lwip/prot/udp.h"

#define DBG_TAG "APP_WIFI"
#include "log.h"

#define APP_WIFI_TASK_STACK     4096
#define APP_WIFI_TASK_PRIORITY  24
#define APP_WIFI_START_DELAY_MS 1500

#define APP_ETH_HDR_LEN     14U
#define APP_IP4_HDR_MIN_LEN 20U
#define APP_UDP4_HDR_LEN    8U
#define APP_ETHTYPE_8021X   0x888eU

static bool wifi_init_done;

static void wifi_event_handler(async_input_event_t ev, void *priv)
{
    uint32_t code = ev->code;

    switch (code) {
        case CODE_WIFI_ON_INIT_DONE:
            LOG_I("CODE_WIFI_ON_INIT_DONE\r\n");
            wifi_mgmr_task_start();
            break;
        case CODE_WIFI_ON_MGMR_DONE:
            LOG_I("CODE_WIFI_ON_MGMR_DONE\r\n");
            wifi_init_done = true;
            break;
        case CODE_WIFI_ON_SCAN_DONE:
            LOG_I("CODE_WIFI_ON_SCAN_DONE\r\n");
            break;
        case CODE_WIFI_ON_CONNECTED:
            LOG_I("CODE_WIFI_ON_CONNECTED\r\n");
            break;
#ifdef CODE_WIFI_ON_GOT_IP_ABORT
        case CODE_WIFI_ON_GOT_IP_ABORT:
            LOG_I("CODE_WIFI_ON_GOT_IP_ABORT\r\n");
            break;
#endif
#ifdef CODE_WIFI_ON_GOT_IP_TIMEOUT
        case CODE_WIFI_ON_GOT_IP_TIMEOUT:
            LOG_I("CODE_WIFI_ON_GOT_IP_TIMEOUT\r\n");
            break;
#endif
        case CODE_WIFI_ON_GOT_IP:
            LOG_I("CODE_WIFI_ON_GOT_IP, free heap %d bytes\r\n", (int)kfree_size(0));
            break;
        case CODE_WIFI_ON_DISCONNECT:
            LOG_I("CODE_WIFI_ON_DISCONNECT\r\n");
            break;
        case CODE_WIFI_ON_AP_STARTED:
            LOG_I("CODE_WIFI_ON_AP_STARTED\r\n");
            break;
        case CODE_WIFI_ON_AP_STOPPED:
            LOG_I("CODE_WIFI_ON_AP_STOPPED\r\n");
            break;
        case CODE_WIFI_ON_AP_STA_ADD:
            LOG_I("CODE_WIFI_ON_AP_STA_ADD %p\r\n", priv);
            break;
        case CODE_WIFI_ON_AP_STA_DEL:
            LOG_I("CODE_WIFI_ON_AP_STA_DEL %p\r\n", priv);
            break;
        default:
            LOG_I("unknown wifi event code %u\r\n", (unsigned int)code);
            break;
    }
}

int app_wifi_init(void)
{
    LOG_I("Starting wifi ...\r\n");

    wifi_init_done = false;
    async_register_event_filter(EV_WIFI, wifi_event_handler, NULL);

    wifi_task_create();
    vTaskDelay(pdMS_TO_TICKS(500));

    LOG_I("Starting fhost ...\r\n");
    fhost_init();

    while (!wifi_init_done) {
        vTaskDelay(1);
    }

    LOG_I("wifi ready\r\n");
    return 0;
}

static void app_wifi_task(void *param)
{
    (void)param;

    vTaskDelay(pdMS_TO_TICKS(APP_WIFI_START_DELAY_MS));

    if (app_wifi_init() != 0) {
        LOG_E("app_wifi_init failed\r\n");
        while (1) {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

    vTaskDelete(NULL);
}

void app_wifi_start(void)
{
    xTaskCreate(app_wifi_task, "app_wifi", APP_WIFI_TASK_STACK, NULL, APP_WIFI_TASK_PRIORITY, NULL);
}

static bool app_pkt_has_len(const struct pbuf *pkt, uint16_t len)
{
    return pkt != NULL && pkt->payload != NULL && pkt->len >= len;
}

static bool app_pkt_is_eth_type(const struct pbuf *pkt, uint16_t eth_type)
{
    const struct eth_hdr *eth;

    if (!app_pkt_has_len(pkt, APP_ETH_HDR_LEN)) {
        return false;
    }

    eth = (const struct eth_hdr *)pkt->payload;
    return eth->type == PP_HTONS(eth_type);
}

static bool app_pkt_is_ip4(const struct pbuf *pkt)
{
    return app_pkt_has_len(pkt, APP_ETH_HDR_LEN + APP_IP4_HDR_MIN_LEN) && app_pkt_is_eth_type(pkt, ETHTYPE_IP);
}

static const struct ip_hdr *app_pkt_ip4_hdr(const struct pbuf *pkt)
{
    if (!app_pkt_is_ip4(pkt)) {
        return NULL;
    }

    return (const struct ip_hdr *)((const uint8_t *)pkt->payload + APP_ETH_HDR_LEN);
}

static bool app_pkt_is_dhcp4(const struct pbuf *pkt)
{
    const struct ip_hdr *ip;
    const struct udp_hdr *udp;
    uint16_t ip_hlen;
    uint16_t src_port;
    uint16_t dst_port;

    ip = app_pkt_ip4_hdr(pkt);
    if (ip == NULL || IPH_PROTO(ip) != IP_PROTO_UDP) {
        return false;
    }

    ip_hlen = (uint16_t)(IPH_HL(ip) * 4U);
    if (ip_hlen < APP_IP4_HDR_MIN_LEN || !app_pkt_has_len(pkt, APP_ETH_HDR_LEN + ip_hlen + APP_UDP4_HDR_LEN)) {
        return false;
    }

    udp = (const struct udp_hdr *)((const uint8_t *)pkt->payload + APP_ETH_HDR_LEN + ip_hlen);
    src_port = lwip_ntohs(udp->src);
    dst_port = lwip_ntohs(udp->dest);

    return (src_port == 67U && dst_port == 68U) || (src_port == 68U && dst_port == 67U);
}

static bool app_pkt_is_icmp4(const struct pbuf *pkt)
{
    const struct ip_hdr *ip = app_pkt_ip4_hdr(pkt);

    return ip != NULL && IPH_PROTO(ip) == IP_PROTO_ICMP;
}

static nethub_wifi_rx_filter_action_t app_nethub_wifi_rx_filter(nethub_channel_t src_channel, const struct pbuf *pkt,
                                                                void *user_ctx)
{
    (void)user_ctx;

    if (src_channel != NETHUB_CHANNEL_WIFI_STA && src_channel != NETHUB_CHANNEL_WIFI_AP) {
        return NETHUB_WIFI_RX_FILTER_HOST;
    }

    if (app_pkt_is_eth_type(pkt, APP_ETHTYPE_8021X)) {
        return NETHUB_WIFI_RX_FILTER_LOCAL;
    }

    if (app_pkt_is_eth_type(pkt, ETHTYPE_ARP) || app_pkt_is_dhcp4(pkt) || app_pkt_is_icmp4(pkt)) {
        return NETHUB_WIFI_RX_FILTER_BOTH;
    }

    return NETHUB_WIFI_RX_FILTER_HOST;
}

void app_wifi_rx_filter_init(void)
{
    int ret = nethub_set_wifi_rx_filter(app_nethub_wifi_rx_filter, NULL);

    if (ret != NETHUB_OK) {
        LOG_W("NetHub WiFi RX host filter init failed: %d\r\n", ret);
    } else {
        LOG_I("NetHub WiFi RX host filter ready\r\n");
    }
}
