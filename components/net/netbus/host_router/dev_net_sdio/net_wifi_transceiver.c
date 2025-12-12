#include <lwip/pbuf.h>
#include <lwip/netifapi.h>
#include <sdiowifi_platform_adapt.h>

//#include <bl_wifi.h>
#include <wifi_mgmr_ext.h>
#include <net_pkt_filter.h>
#include <net_def.h>
#include <sdio_pkt_hooks.h>
#include <sdiowifi_mgmr.h>
#include <sdiowifi_config.h>
#include <trcver_sdio.h>
#include <net_wifi_transceiver.h>
#include <sdio_msg_ext_frame.h>
#include "dual_stack_input.h"
#include "sdiowifi_mgmr_type.h"

#include "sdio_port.h"

typedef void (*bl_custom_tx_callback_t)(void *cb_arg, bool tx_ok);

struct bl_custom_tx_cfm {
    bl_custom_tx_callback_t cb;
    void *cb_arg;
};
int bl_wifi_eth_tx(struct pbuf *p, bool is_sta, struct bl_custom_tx_cfm *custom_cfm);

#ifdef CFG_DUAL_ETH
int sdio_net_if_input(struct pbuf *p, inet_if_t *net_if, bool is_sta)
{
    if (net_if->input(p, net_if))
    {
        return -1;
    }

    return 0;
}

extern inet_if_t sdio_eth_netif;
#endif

static void handle_eth_frame_to_wifi_tx(sdiowifi_mgmr_t *sdm, struct pbuf *p, bool is_sta)
{
#ifdef CFG_DUAL_ETH
    sdio_net_if_input(p, &sdio_eth_netif, is_sta);
#else
#ifdef CONFIG_LWIP_PBUF_CUSTOM_EXT_ENABLE
    p->custom_flags.eth_to_macsw = 1;
#endif
    bl_wifi_eth_tx(p, is_sta, &(struct bl_custom_tx_cfm){NULL, NULL});
    pbuf_free(p);
#endif
}

static void handle_sta_ap_to_wifi_tx(sdiowifi_mgmr_t *sdm, const uint8_t *frame, uint16_t frame_len, bool is_sta)
{
    extern void trcver_sdio_stats_display(void);
    extern void sdiowifi_txbuf_stats_display(void);

    struct pbuf *p;

    if (frame == NULL || frame_len > 2048) {
        HR_LOGE("frame or frame_len error\r\n");
        return;
    }

    p = sdiowifi_tx_buf_alloc(&sdm->tx_desc);

    if (!p) {
        printf("[%ld] hostrouter no more tx pbuf, flow control not working?\r\n");
        hostrouter_stats_display();
        return;
    }

    if (pbuf_take(p, frame, frame_len) != ERR_OK) {
        HR_LOGE("pbuf_take err\r\n");
        pbuf_free(p);
        return;
    }
    p->len = frame_len;
    p->tot_len = frame_len;

    handle_eth_frame_to_wifi_tx(sdm, p, is_sta);
}

static void handle_sniffer_tx(sdiowifi_mgmr_t *sdm, const uint8_t *frame, uint16_t frame_len)
{
#ifdef CFG_BL616
#else
    wifi_mgmr_raw_80211_send((uint8_t *)frame, frame_len);
#endif
}

static void handle_eth_wifi_frame_recv(sdiowifi_mgmr_t *sdm, const uint8_t *frame, uint16_t frame_len)
{
    uint16_t subtype;

    subtype = frame[0] + (frame[1] << 8);
    switch (subtype) {
    case BF1B_MSG_ETH_WIFI_FRAME_SUBTYPE_STA_TO_WIFI_TX:
        handle_sta_ap_to_wifi_tx(sdm, frame + 2, frame_len - 2, true);
        break;
    case BF1B_MSG_ETH_WIFI_FRAME_SUBTYPE_AP_TO_WIFI_TX:
        handle_sta_ap_to_wifi_tx(sdm, frame + 2, frame_len - 2, false);
        break;
    case BF1B_MSG_ETH_WIFI_FRAME_SUBTYPE_SNIFFER_TO_WIFI_TX:
        handle_sniffer_tx(sdm, frame + 2, frame_len - 2);
        break;
    case BF1B_MSG_ETH_WIFI_FRAME_SUBTYPE_EXT:
        sdiowifi_ext_frame_process(sdm, frame + 2, frame_len - 2);
        break;
    default:
        break;
    }
}

static void handle_cmd_recv(sdiowifi_mgmr_t *sdm, const uint8_t *data, uint16_t data_len)
{
    rnms_cmd_input_raw(sdm->rnm, data, data_len);
}

static void sdio_recv_cb(void *arg, const void *data_ptr, uint16_t data_len)
{
    bl602_sdio_msg_t *msg;
    sdiowifi_mgmr_t *sdm;

    msg = (bl602_sdio_msg_t *)data_ptr;
    sdm = &g_sdiowifi_mgmr;
    switch (msg->type) {
    case BF1B_MSG_TYPE_ETH_WIFI_FRAME:
        HR_LOGD("sdio_recv_cb BF1B_MSG_TYPE_ETH_WIFI_FRAME\r\n");
        handle_eth_wifi_frame_recv(sdm, msg->payload, msg->len - sizeof(*msg));
        break;
    case BF1B_MSG_TYPE_CMD:
        HR_LOGD("sdio_recv_cb BF1B_MSG_TYPE_CMD\r\n");
        handle_cmd_recv(sdm, msg->payload, msg->len - sizeof(*msg));
        break;
    default:
        HR_LOGE("sdio_recv_cb UNKNOW type:%d\r\n", msg->type);
        break;
    }
}

static int bflb_net_wifi_trcver_write_mac(net_wifi_trcver_t *trcver)
{
    int i;
    uint8_t mac[6];

#ifdef CFG_BL616
    // FIXME
#ifdef CFG_DUAL_ETH
    extern void sdioeth_netif_mac_get(uint8_t *mac);
    sdioeth_netif_mac_get(mac);
#else
    wifi_mgmr_sta_mac_get(mac);
#endif
#else
    bl_wifi_mac_addr_get(mac);
#endif

    for (i = 0; i < 6; ++i) {
        bf1b_write_s_reg(trcver, WIFI_MAC_ADDR_SCRATCH_OFFSET + i, mac[i]);
    }

    return 0;
}

#if SDIOWIFI_ACK_BOOT_SRC
static void start_ack_bootsrc(void *pvParameters)
{
    net_wifi_trcver_t *trcver;

    if ((uintptr_t)pvParameters & 1) {
        uint8_t v;
        sdiowifi_tick_t tick = sdiowifi_tick_now(0);
        trcver = (net_wifi_trcver_t *)((uintptr_t)pvParameters & ~1);

        while (1) {
            v = bf1b_read_s_reg(trcver, BOOT_SRC_INFO_SCRATCH_OFFSET);
            if (v == BL_BOOT_SRC_PING_VALUE) {
                HR_LOGI("Got host sdio scratch reg ping, time cost %lu ms\r\n", sdiowifi_tick_now(0) - tick);
                bf1b_write_s_reg(trcver, BOOT_SRC_INFO_SCRATCH_OFFSET, BL_BOOT_SRC_PONG_VALUE);
                break;
            }
            if (sdiowifi_tick_now(0) - tick > BOOT_SRC_PONG_TIMEOUT_MS) {
                HR_LOGE("Get host sdio scratch reg ping timed out\r\n");
                break;
            }
            sdiowifi_delay_ms(1);
        }
        sdiowifi_task_delete(NULL);
    } else {
        if (0 != sdiowifi_task_create(start_ack_bootsrc, "ack_bootsrc", 0, 1024, (void *)((uintptr_t)pvParameters | 1), 20, NULL)) {
            HR_LOGE("create ack_bootsrc task failed\r\n");
        }
    }
}
#endif


// XXX copied from bl_rx.h. Remember to keep identical!
/////////////// start of copy ///////////////
#define PBUF_FLAG_AMSDU 0x80U
/////////////// end of copy ///////////////

static void sdio_pbuf_cfm_cb(int idx, void *arg)
{
    pbuf_free(arg);
}

int dual_stack_input(void *pkt, void *arg)
{
    net_wifi_trcver_t *trcver = &g_sdiowifi_mgmr.trcver;
    struct pbuf *p = (struct pbuf *)pkt;
    bool is_amsdu = p->flags & PBUF_FLAG_AMSDU;
    int ret = bf1b_msg_send_pbuf(trcver, BF1B_MSG_TYPE_ETH_WIFI_FRAME, BF1B_MSG_ETH_WIFI_FRAME_SUBTYPE_STA_FROM_WIFI_RX, p, is_amsdu, sdio_pbuf_cfm_cb, pkt);
    return ret;
}

static void *eth_input_hook(bool is_sta, void *pkt, void *arg)
{
    struct pbuf *p = (struct pbuf *)pkt;

#ifdef CFG_BL616
    if (npf_is_8021X(p)) {
        return pkt;
    }
#endif

    bool input_emb = true;
    bool input_host = false;
    struct pbuf *p_dup = NULL;
    struct pbuf *ret = NULL;

#ifdef DHCP_IN_EMB
    if (npf_is_arp(p)) {
        input_emb = true;
        input_host = true;
    } else if (npf_is_dhcp4(p)) {
        input_emb = true;
        input_host = false;
    }
#else
    input_emb = false;
    input_host = true;
#endif
    // TODO Add custom filter here
    // Example of handling some TCP traffic in emb:
#ifdef CONF_SDIOWIFI_TEST
    if (npf_is_tcp4_port(p, 50001) || npf_is_udp4_port(p, 50001)) {
        input_emb = true;
        input_host = false;
    }
    if (npf_is_icmp4(p)) {
        input_emb = true;
        input_host = false;
    }
#endif

    if (!input_emb && !input_host) {
        pbuf_free(p);
        return NULL;
    }

    if (input_emb && input_host) {
        p_dup = pbuf_alloc(PBUF_RAW, p->tot_len, PBUF_RAM);
        if (p_dup == NULL) {
            pbuf_free(p);
            return NULL;
        }
        pbuf_copy(p_dup, p);
    }

    if (input_host) {

        // XXX distinguish STA/AP
        int ret = dual_stack_input(p, NULL);
        if (ret) {
            /* printf("RX FRM swdesc %p failed, drop\r\n", swdesc); */
            pbuf_free(p);
        }
    }

    if (p_dup) {
        // both emb & host
        ret = p_dup;
    } else if (input_emb) {
        ret = p;
    } else {
        ret = NULL;
    }
    return ret;
}

#ifdef CFG_DUAL_ETH
static int sdio_eth_output_hook(bool is_sta, void *pkt, void *arg)
{
    net_wifi_trcver_t *trcver = (net_wifi_trcver_t *)arg;
    (void)trcver;

    struct pbuf *p = (struct pbuf *)pkt;
    // XXX distinguish STA/AP
    int ret = dual_stack_input(p, NULL);
    if (ret) {
        printf("RX FRM swdesc failed, drop\r\n");
    }

    return ret;
}
#endif

static void *eth_output_hook(bool is_sta, void *pkt, void *arg)
{
    net_wifi_trcver_t *trcver = (net_wifi_trcver_t *)arg;

    (void)trcver;
    struct pbuf *p = (struct pbuf *)pkt;

    if (npf_is_8021X(p)) {
        return pkt;
    }

#ifdef DHCP_IN_EMB
    // Allow ARP & DHCP output
    if (npf_is_arp(p) || npf_is_dhcp4(p)) {
        return pkt;
    }
#endif
    // TODO Add custom filter here
#ifdef CONF_SDIOWIFI_TEST
    if (npf_is_tcp4_port(p, 50001) || npf_is_udp4_port(p, 50001)) {
        return pkt;
    }
#endif
    HR_LOGI("dropping local TCP/IP stack traffic\r\n");

    return NULL;
}

typedef void *(*bl_pkt_eth_input_hook_cb_t)(bool is_sta, void *pkt, void *arg);
extern void bl_pkt_eth_input_hook_register(bl_pkt_eth_input_hook_cb_t cb, void *cb_arg);

static void register_pkt_hooks(net_wifi_trcver_t *trcver)
{
#ifdef CFG_DUAL_ETH
    bl_sdio_pkt_eth_output_hook_register(sdio_eth_output_hook, trcver);
#else
#ifndef SDIO_TEST_NO_HOST
    bl_pkt_eth_input_hook_register(eth_input_hook, trcver);

    // Allowing all output.
    // Leave output hook here as it may be added back.
    (void)eth_output_hook;
    /* bl_pkt_eth_output_hook_register(eth_output_hook, trcver); */
#endif
#endif
}

int bf1b_net_wifi_trcver_init(net_wifi_trcver_t *trcver, void *arg)
{
    sdiowifi_mutex_create(&trcver->tx_lock);
    bl_sdu_init();
    bl_sdio_handshake();
    bl_sdio_read_cb_register(NULL, sdio_recv_cb, trcver);
    bflb_net_wifi_trcver_write_mac(trcver);
#if SDIOWIFI_ACK_BOOTSRC
    start_ack_bootsrc(trcver);
#endif
    register_pkt_hooks(trcver);
    trcver->arg = (void *)arg;
    return 0;
}

int bf1b_net_wifi_trcver_reinit(net_wifi_trcver_t *trcver)
{
    bflb_net_wifi_trcver_write_mac(trcver);

    return 0;
}

int bflb_net_wifi_trcver_set_present(net_wifi_trcver_t *trcver, bool present)
{
    if (!trcver) {
        return -1;
    }
    trcver->host_present = present;
    return 0;
}

int bf1b_msg_send(net_wifi_trcver_t *trcver, uint16_t type, const void *payload, uint16_t payload_len)
{
    int ret;
    if (!trcver->host_present) {
        return BF1B_MSG_ERR_HOST_NOT_READY;
    }
    sdiowifi_mutex_lock(trcver->tx_lock);
    ret = bl_sdio_write_cmd(NULL, type, 0, payload, payload_len, NULL, 0);
    sdiowifi_mutex_unlock(trcver->tx_lock);
    return ret;
}

int bf1b_msg_send_pbuf(net_wifi_trcver_t *trcver, uint16_t type, uint16_t subtype, struct pbuf *p, bool is_amsdu, void *cb, void *cb_arg)
{
    int ret;
    if (!trcver->host_present) {
        return BF1B_MSG_ERR_HOST_NOT_READY;
    }
    sdiowifi_mutex_lock(trcver->tx_lock);
    ret = bl_sdio_write_pbuf(NULL, type, subtype, p, is_amsdu, cb, cb_arg);
    sdiowifi_mutex_unlock(trcver->tx_lock);
    if (ret == BF1B_MSG_ERR_DESC_USED) {
        HR_LOGW("pbuf BF1B_MSG_ERR_DESC_USED -> sdiowifi_mgmr_reinit -> SDU_WORKER_EV_RESET\r\n");
        hr_coredump();
        sdiowifi_mgmr_reinit(false);
    }
    return ret;
}

int bf1b_msg_send_frame_ext(net_wifi_trcver_t *trcver, uint16_t type, uint16_t subtype, const void *headroom, uint16_t headroom_len, const void *tailroom, uint16_t tailroom_len)
{
    int ret;
    if (!trcver->host_present) {
        return BF1B_MSG_ERR_HOST_NOT_READY;
    }

    sdiowifi_mutex_lock(trcver->tx_lock);
    ret = bl_sdio_write_cmd(NULL, type, subtype, headroom, headroom_len, tailroom, tailroom_len);
    sdiowifi_mutex_unlock(trcver->tx_lock);
    return ret;
}

uint8_t bf1b_read_s_reg(net_wifi_trcver_t *trcver, uint32_t offset)
{
    uint8_t ret;

    ret = sdu_read_s_reg(offset);
    return ret;
}

void bf1b_write_s_reg(net_wifi_trcver_t *trcver, uint32_t offset, uint8_t val)
{
    sdu_write_s_reg(offset, val);
}
