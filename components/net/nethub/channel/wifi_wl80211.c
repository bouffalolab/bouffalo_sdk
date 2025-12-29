/**
 * @file port_sta.c
 * @brief STA interface port adaptation layer implementation
 */

#include <stdio.h>
#include "nhsdiowifi.h"

#include "bflb_core.h"
#include "bflb_mtimer.h"

#include "board.h"

#include "FreeRTOS.h"
#include "task.h"

#include "bflb_sdio_drv.h"
#include "bflb_msg_ctrl.h"
#include "bflb_frame_buff_ctrl.h"
#include "bflb_tty.h"
#include <transportsdio.h>
#include <netpacket_filter.h>
#include <lwip/pbuf.h>
#include "lwip/netif.h"
#include <log.h>

#ifdef CONFIG_WL80211

#else
#include <net_al.h>
#include <fhost.h>
#include <wifi_pkt_hooks.h>
#endif

#include "nethub_types.h"

#define DEBUG_STA_LOG_ENABLE     (0)// 1-enable log. 0-no log
#define DEBUG_BYPASS_WIFI_TX     (0)// 1-not send to wifi fw. 0-normal
#define DEBUG_BYPASS_WIFI_RX     (0)// 1-not send to wifi fw. 0-normal

#define DEBUG_DUMP_WIFITX_ENABLE (0)// 1-dump wifi tx data. 0-not dump wifi tx data
#define DEBUG_DUMP_WIFIRX_ENABLE (0)// 1-dump wifi tx data. 0-not dump wifi tx data

/* Debug output for example program */
#define USER_DBG(fmt, ...)     //printf("[WiFi] " fmt, ##__VA_ARGS__)
#if (DEBUG_DUMP_WIFITX_ENABLE | DEBUG_DUMP_WIFIRX_ENABLE)
#define USER_INFO(fmt, ...)    printf("[WiFi]" fmt, ##__VA_ARGS__)
#else
#define USER_INFO(...)
#endif

/* NETHUB statistics global variable */
nethub_statistics_t g_nethub_stats = {0};


#ifdef CONFIG_WL80211
#else
extern int fhost_tx_start(net_al_if_t net_if, net_al_tx_t net_buf,
                   cb_fhost_tx cfm_cb, void *cfm_cb_arg);
#endif

// ========================================= filter =========================================
#ifndef CONFIG_NETHUB_FILTER_LOCAL_PORT_MIN
#define CONFIG_NETHUB_FILTER_LOCAL_PORT_MIN 0
#endif
#ifndef CONFIG_NETHUB_FILTER_LOCAL_PORT_MAX
#define CONFIG_NETHUB_FILTER_LOCAL_PORT_MAX 0
#endif
#define FILTER_LOCAL   (1 << 0)    /* 本地处理 */
#define FILTER_HOST    (1 << 1)    /* 转发给主机 */

#if ((CONFIG_NETHUB_FILTER_LOCAL_PORT_MIN != 0) || (CONFIG_NETHUB_FILTER_LOCAL_PORT_MAX != 0))
static bool is_port_in_local_range(const struct pbuf *skb)
{
    uint16_t port;
    if (!npf_is_ip4(skb)) {
        return false;
    }
    if (!(skb->len >= NPF_ETH_HDR_LEN + NPF

        _IP4_HDR_LEN + NPF_TCP4_HDR_LEN)) {
        return false;
    }
    const struct tcp_hdr *tcphdr = (const struct tcp_hdr *)(skb->payload + NPF_ETH_HDR_LEN + NPF_IP4_HDR_LEN);
    port = tcphdr->dest;// it's udp or tcp
    if ((lwip_htons(port) >= CONFIG_NETHUB_FILTER_LOCAL_PORT_MIN) &&
        (lwip_htons(port) <= CONFIG_NETHUB_FILTER_LOCAL_PORT_MAX)) {
        return true;
    }
    return false;
}
#endif
static int eth_input_filter(void *pkt)
{
    int filter_action = 0;
    /*
     * This segment of logic will be delegated to the abstraction layer for processing.
     * 1. arp double
     * 2. dhcp local
     * 3. icmp local
     */
    if (npf_is_arp(pkt)) {
        filter_action = FILTER_LOCAL | FILTER_HOST;
    } else if (npf_is_dhcp4(pkt)) {
        filter_action = FILTER_LOCAL;
    } else if (npf_is_icmp4(pkt)) {
        filter_action = FILTER_LOCAL;
#if ((CONFIG_NETHUB_FILTER_LOCAL_PORT_MIN != 0) || (CONFIG_NETHUB_FILTER_LOCAL_PORT_MAX != 0))
    } else if (is_port_in_local_range(pkt)) {
        filter_action = FILTER_LOCAL;
#endif
    } else {
        // ipv6 to host
        filter_action = FILTER_HOST;
    }

#if 0
    /*
     * filter by port
     */
    if (npf_is_tcp4_port(p, 50001) || npf_is_udp4_port(p, 50001)) {
        filter_action = FILTER_LOCAL;
    }
#endif
    return filter_action;
}
// ========================================= filter =========================================

// ========================================= dnld  =========================================
/* Note:
 * sizeof(trans_desc_t) must < 32 Bytes to avoid memory corruption
 * when fhost_tx_desc_tag content is modified during tx process
 * */
typedef struct _trans_desc {
    struct pbuf_custom pbuf;                // 20
    void (*free_cb)(void *arg);             // 4
    void *cb_arg;                           // 4
} trans_desc_t;
// For channel dnld
static int sta_input_portwifi_eth_tx(struct pbuf *msg, bool is_sta)
{
    struct pbuf *p = (struct pbuf *)msg;
#ifdef CONFIG_WL80211
    pbuf_free(p);
#else
    net_al_if_t *net_if;

    // get intf
    if (is_sta) {
        net_if = fhost_env.vif[0].net_if;
    } else {
        net_if = fhost_env.vif[1].net_if;
    }
    if ((!net_if) || (!netif_is_up((struct netif *)net_if))) {
        USER_DBG("%s:%d net_if:%p, netif not up! so drop.\r\n", __func__, __LINE__, net_if);
        /* Statistics: Consumer 1 - drop due to network interface not up */
        g_nethub_stats.dnld_consumer_netif_down++;
        g_nethub_stats.dnld_total_dropped++;
        pbuf_free(p);
    	return -1;
    }

#if DEBUG_DUMP_WIFITX_ENABLE
    void nethub_dumpbuf(uint8_t *str, uint8_t *buf, int len);
    nethub_dumpbuf("txstart", (uint8_t *)((uint32_t)p + 448), 32);
#endif
    if (0 != fhost_tx_start(net_if, p, NULL, NULL)) {
        USER_DBG("%s:%d fhost_tx_start return error! so drop.\r\n", __func__, __LINE__);
        /* Statistics: Consumer 2 - drop due to tx failure */
        g_nethub_stats.dnld_consumer_tx_fail++;
        g_nethub_stats.dnld_total_dropped++;
        pbuf_free(p);
    } else {
        /* tx success, successful processing statistics */
        g_nethub_stats.dnld_total_success++;
    }
#endif
    return 0;
}
static void sta_input_dnld_custom_free(struct pbuf *p)
{
    trans_desc_t *dnmsg = (trans_desc_t *)p;
#if DEBUG_DUMP_WIFITX_ENABLE
    USER_INFO("%s dnld free p:%p, real_cb:%p, real_ctx:%p, real_arg:%p\r\n\r\n", __func__, p, dnmsg->free_cb, dnmsg->ctx, dnmsg->cb_arg);
    void nethub_dumpbuf(uint8_t *str, uint8_t *buf, int len);
    nethub_dumpbuf("txdone", (uint8_t *)((uint32_t)p + 448), 32);
#endif
    /* Statistics: Consumer 3 - packet release (normal case) */
    g_nethub_stats.dnld_consumer_free_normal++;
    dnmsg->free_cb(dnmsg->cb_arg);
    //printf("dnmsg->free_cb:%p, dnmsg->ctx:%p, dnmsg->cb_arg:%p\r\n",
    //    dnmsg->free_cb, dnmsg->ctx, dnmsg->cb_arg);
}
static int sta_input_dnld(nh_skb_t *skb, void *arg)
{
    trans_desc_t *trans_desc = NULL;
    /* Statistics: Total processed packets increase */
    g_nethub_stats.dnld_total_packets++;

#if DEBUG_BYPASS_WIFI_TX
    skb->free_cb(skb->cb_priv, skb->cb_arg);
#else
#if DEBUG_DUMP_WIFITX_ENABLE
    USER_INFO(" ---1 %s, line:%d, skb->data:%p, skb->len:%d, skb->cb_arg:%p\r\n", __func__, __LINE__, skb->data, skb->len, skb->cb_arg);
#endif
    trans_desc = (trans_desc_t *)((uintptr_t)skb->cb_arg + FRAME_BUFF_ELEM_SIZE);
    trans_desc->free_cb = skb->free_cb;
    trans_desc->cb_arg = skb->cb_arg;

    pbuf_alloced_custom(0,       // offset
            skb->len,            //
            (PBUF_ALLOC_FLAG_DATA_CONTIGUOUS | PBUF_TYPE_ALLOC_SRC_MASK_STD_HEAP),
            &trans_desc->pbuf,
            NULL,
            FRAME_BUFF_PAYLOAD);

    trans_desc->pbuf.pbuf.payload = skb->data;
    trans_desc->pbuf.custom_free_function = sta_input_dnld_custom_free;

#if DEBUG_STA_LOG_ENABLE
    _dump_pbuf("<--- Send ", &trans_desc->pbuf.pbuf);
#endif
#if DEBUG_DUMP_WIFITX_ENABLE
    USER_INFO(" ---2 %s, line:%d, head:%p, pld:%p\r\n", __func__, __LINE__, trans_desc, trans_desc->pbuf.pbuf.payload);
#endif
    sta_input_portwifi_eth_tx((struct pbuf *)trans_desc, 1);
#endif // end DEBUG_BYPASS_WIFI_TX
    return 0;
}
// ========================================= dnld  =========================================

// ========================================= upld  =========================================
static void upld_custom_free(void *arg)
{
    struct pbuf *p = (struct pbuf *)arg;
#if DEBUG_DUMP_WIFIRX_ENABLE
    USER_INFO("%s ------------------------------------ p:%p, p->ref:%d\r\n\r\n", __func__, arg, p->ref);
#endif

    /* Statistics: UPLD final release */
    g_nethub_stats.upld_free_count++;

    pbuf_free(p);
}

// upld callback by bl_pkt_eth_input_hook_register config
static void *eth_input_hook_upld(bool is_sta, void *pkt, void *arg)
{
    int filter_action = 0;

    nh_forward_result_t result;
    nh_skb_t skb;// stack memory
    struct pbuf *p = (struct pbuf *)pkt;
    struct pbuf *p_dup = NULL;
    struct pbuf *ret = NULL;

    if (npf_is_8021X(p)) {
        return pkt;
    }

#if DEBUG_STA_LOG_ENABLE
    _dump_pbuf("---> Recv ", p);
#endif

    filter_action = eth_input_filter(p);

    // check if double ?
    if (filter_action == (FILTER_LOCAL | FILTER_HOST)) {
        p_dup = pbuf_alloc(PBUF_RAW, p->tot_len, PBUF_RAM);
        if (p_dup == NULL) {
            pbuf_free(p);
            return NULL;
        }
        pbuf_copy(p_dup, p);
    }

    // to host
    if (filter_action & FILTER_HOST) {
        skb.data = p->payload;
        skb.len = p->len;
        skb.free_cb = upld_custom_free;
        skb.cb_arg = p;
        skb.next = NULL;
#if DEBUG_DUMP_WIFIRX_ENABLE
        USER_INFO("%s ++++++++++++++++++++++++++++++ p=%p, pld:%p\r\n", __func__, skb.cb_arg, p->payload);
#endif
        /* Statistics: UPLD producer - WiFi data enters system */
        g_nethub_stats.upld_producer_count++;
        g_nethub_stats.upld_total_packets++;
#if DEBUG_BYPASS_WIFI_RX
        upld_custom_free(p);
#else
#if DEBUG_STA_LOG_ENABLE
        _dump_pbuf("     To host", p);
#endif
        result = nethub_process_input(&skb, NHIF_TYPE_STA);
        if (result == NH_FORWARD_STOP) {
            //USER_DBG("Data consumed, forwarding stopped\n");
        } else if (result == NH_FORWARD_CONTINUE) {
            //USER_DBG("Data forwarding continues\n");
        } else {
            //USER_DBG("Forwarding result abnormal: %d\n", result);
        }
#endif
    }

    if (p_dup) {
        // both local & host
        ret = p_dup;
    } else if (filter_action & FILTER_LOCAL) {
        ret = p;
    } else {
        ret = NULL;
    }
    return ret;
}
// ========================================= upld  =========================================
// ========================================= init  =========================================
static int _sta_init(void)
{
#if CONFIG_WL80211
#else
    bl_pkt_eth_input_hook_register(eth_input_hook_upld, NULL);//&ctx->rx_env);
#endif
    return NH_OK;
}

/* STA interface input callback */
static nh_forward_result_t _sta_input_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  /* Avoid unused parameter warning */
    // never here
    USER_DBG("STA input Never here\n");
    return NH_FORWARD_CONTINUE;
}

/* STA interface output callback */
static nh_forward_result_t _sta_output_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  /* Avoid unused parameter warning */
    //USER_DBG("@%p: %d\r\n\r\n", skb->data, skb->len);
    sta_input_dnld(skb, arg);
    return NH_FORWARD_CONTINUE;
}

/* STA interface operation structure */
const struct nhif_ops nhsta_ops = {
    .type = NHIF_TYPE_STA,
    .init = _sta_init,
    .deinit = NULL,
    .input = _sta_input_callback,
    .output = _sta_output_callback,
};

// ========================================= ap module  =========================================
/* AP interface input callback */
static nh_forward_result_t ap_input_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  /* Avoid unused parameter warning */
    USER_DBG("AP INPUT received data: %s\n", skb->data);
    return NH_FORWARD_CONTINUE;
}

/* AP interface output callback */
static nh_forward_result_t ap_output_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  /* Avoid unused parameter warning */
    USER_DBG("AP OUTPUT send data: %s\n", skb->data);
    return NH_FORWARD_CONTINUE;
}

/* AP interface operation structure */
const struct nhif_ops nhap_ops = {
    .type = NHIF_TYPE_AP,
    .init = NULL,
    .deinit = NULL,
    .input = ap_input_callback,
    .output = ap_output_callback,
};

