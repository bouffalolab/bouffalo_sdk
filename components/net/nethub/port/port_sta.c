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

#include <net_al.h>
#include <fhost.h>
#include "nethub_types.h"

#define DEBUG_STA_LOG_ENABLE     (0)// 1-enable log. 0-no log
#define DEBUG_BYPASS_WIFI_TX     (0)// 1-not send to wifi fw. 0-normal
#define DEBUG_BYPASS_WIFI_RX     (0)// 1-not send to wifi fw. 0-normal
#define DEBUG_DUMP_WIFITX_ENABLE (0)// 1-dump wifi tx data. 0-not dump wifi tx data
#define DEBUG_DUMP_WIFIRX_ENABLE (0)// 1-dump wifi tx data. 0-not dump wifi tx data

/* Debug output for example program */
#ifdef CONFIG_NETHUB_DEBUG
#define EXAMPLE_DEBUG LOG_D//(fmt, ...) printf("[PORTSTA] " fmt, ##__VA_ARGS__)
#else
#define EXAMPLE_DEBUG(fmt, ...)
#endif

/* NETHUB statistics global variable */
nethub_statistics_t g_nethub_stats = {0};

#if DEBUG_STA_LOG_ENABLE
static void _dump_pbuf(void *str, struct pbuf *p)
{
    /* Print pbuf information: source address, destination address, protocol type, message length, port number, etc. */
    if (p && p->payload && p->len >= NPF_ETH_HDR_LEN) {
        const struct eth_hdr *eth_hdr = (const struct eth_hdr *)p->payload;
        char *protocol = "UNKNOWN";
        char extra_info[256];
        uint16_t eth_type = lwip_ntohs(eth_hdr->type);
        char src_ip_str[16] = {0};
        char dst_ip_str[16] = {0};
        uint16_t src_port = 0, dst_port = 0;

        memset(extra_info, 0, sizeof(extra_info));
        switch (eth_type) {
            case ETHTYPE_ARP:
                protocol = "ARP";
                if (p->len >= NPF_ETH_HDR_LEN + 28) { // Minimum ARP packet length
                    uint8_t *arp_data = (uint8_t *)p->payload + NPF_ETH_HDR_LEN;
                    uint16_t opcode = lwip_ntohs(*(uint16_t *)(arp_data + 6));
                    snprintf(extra_info, sizeof(extra_info), "%s", (opcode == 1) ? " (Request)" : " (Reply)");
                }
                break;
            case ETHTYPE_IP:
                protocol = "IP";
                if (p->len >= NPF_ETH_HDR_LEN + NPF_IP4_HDR_LEN) {
                    const struct ip_hdr *iphdr = (const struct ip_hdr *)(p->payload + NPF_ETH_HDR_LEN);
                    uint8_t ip_proto = IPH_PROTO(iphdr);

                    /* Extract IP addresses */
                    snprintf(src_ip_str, sizeof(src_ip_str), "%u.%u.%u.%u",
                           ip4_addr1(&iphdr->src), ip4_addr2(&iphdr->src),
                           ip4_addr3(&iphdr->src), ip4_addr4(&iphdr->src));
                    snprintf(dst_ip_str, sizeof(dst_ip_str), "%u.%u.%u.%u",
                           ip4_addr1(&iphdr->dest), ip4_addr2(&iphdr->dest),
                           ip4_addr3(&iphdr->dest), ip4_addr4(&iphdr->dest));

                    switch (ip_proto) {
                        case IP_PROTO_TCP:
                            if (p->len >= NPF_ETH_HDR_LEN + NPF_IP4_HDR_LEN + NPF_TCP4_HDR_LEN) {
                                const struct tcp_hdr *tcphdr = (const struct tcp_hdr *)(p->payload + NPF_ETH_HDR_LEN + NPF_IP4_HDR_LEN);
                                src_port = lwip_ntohs(tcphdr->src);
                                dst_port = lwip_ntohs(tcphdr->dest);
                                snprintf(extra_info, sizeof(extra_info), " TCP %s:%u -> %s:%u", src_ip_str, src_port, dst_ip_str, dst_port);
                            }
                            break;
                        case IP_PROTO_UDP:
                            if (p->len >= NPF_ETH_HDR_LEN + NPF_IP4_HDR_LEN + NPF_UDP4_HDR_LEN) {
                                const struct udp_hdr *udphdr = (const struct udp_hdr *)(p->payload + NPF_ETH_HDR_LEN + NPF_IP4_HDR_LEN);
                                src_port = lwip_ntohs(udphdr->src);
                                dst_port = lwip_ntohs(udphdr->dest);
                                int info_len = snprintf(extra_info, sizeof(extra_info), " UDP %s:%u -> %s:%u", src_ip_str, src_port, dst_ip_str, dst_port);

                                /* Special UDP protocol identification */
                                if ((src_port == 67 && dst_port == 68) || (src_port == 68 && dst_port == 67)) {
                                    snprintf(extra_info + info_len, sizeof(extra_info) - info_len, " [DHCP]");
                                } else if (src_port == 53 || dst_port == 53) {
                                    snprintf(extra_info + info_len, sizeof(extra_info) - info_len, " [DNS]");
                                } else if (src_port == 123 || dst_port == 123) {
                                    snprintf(extra_info + info_len, sizeof(extra_info) - info_len, " [NTP]");
                                }
                            }
                            break;
                        case IP_PROTO_ICMP:
                            snprintf(extra_info, sizeof(extra_info), " ICMP %s -> %s", src_ip_str, dst_ip_str);
                            break;
                        default:
                            snprintf(extra_info, sizeof(extra_info), " Proto%u %s -> %s", ip_proto, src_ip_str, dst_ip_str);
                            break;
                    }
                }
                break;
            case ETHTYPE_IPV6:
                protocol = "IPv6";
                break;
            case ETHTYPE_8021X:
                protocol = "802.1X";
                break;
            default:
                protocol = "OTHER";
                break;
        }

        printf("%s: %02X:%02X:%02X:%02X:%02X:%02X -> %02X:%02X:%02X:%02X:%02X:%02X [%s] len:%d%s\n",
                     str,
                     eth_hdr->src.addr[0], eth_hdr->src.addr[1], eth_hdr->src.addr[2],
                     eth_hdr->src.addr[3], eth_hdr->src.addr[4], eth_hdr->src.addr[5],
                     eth_hdr->dest.addr[0], eth_hdr->dest.addr[1], eth_hdr->dest.addr[2],
                     eth_hdr->dest.addr[3], eth_hdr->dest.addr[4], eth_hdr->dest.addr[5],
                     protocol, p->len, extra_info);
    }
}
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
    if (!(skb->len >= NPF_ETH_HDR_LEN + NPF_IP4_HDR_LEN + NPF_TCP4_HDR_LEN)) {
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
        printf("is within\r\n");
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
typedef struct _trans_desc {
    struct pbuf_custom pbuf;                      // 20
    void (*free_cb)(void *ctx, void *arg);        // 4
    void *ctx;                                    // 4
    void *arg;                                    // 4
    uint8_t buf[FRAME_BUFF_HWDESCTAG];            // 388 = 97*4
} trans_desc_t;
// For channel dnld
static int sta_input_portwifi_eth_tx(struct pbuf *msg, bool is_sta)
{
    struct pbuf *p = (struct pbuf *)msg;
    net_al_if_t *net_if;

    // get intf
    if (is_sta) {
        net_if = fhost_env.vif[0].net_if;
    } else {
        net_if = fhost_env.vif[1].net_if;
    }
    if ((!net_if) || (!netif_is_up((struct netif *)net_if))) {
        EXAMPLE_DEBUG("%s:%d net_if:%p, netif not up! so drop.\r\n", __func__, __LINE__, net_if);
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
        EXAMPLE_DEBUG("%s:%d fhost_tx_start return error! so drop.\r\n", __func__, __LINE__);
        /* Statistics: Consumer 2 - drop due to tx failure */
        g_nethub_stats.dnld_consumer_tx_fail++;
        g_nethub_stats.dnld_total_dropped++;
        pbuf_free(p);
    } else {
        /* tx success, successful processing statistics */
        g_nethub_stats.dnld_total_success++;
    }
}
static void sta_input_dnld_custom_free(struct pbuf *p)
{
    trans_desc_t *dnmsg = (trans_desc_t *)p;
#if DEBUG_DUMP_WIFITX_ENABLE
    EXAMPLE_DEBUG("%s dnld free p:%p, real_cb:%p, real_ctx:%p, real_arg:%p\r\n\r\n", __func__, p, dnmsg->free_cb, dnmsg->ctx, dnmsg->arg);
    void nethub_dumpbuf(uint8_t *str, uint8_t *buf, int len);
    nethub_dumpbuf("txdone", (uint8_t *)((uint32_t)p + 448), 32);
#endif
    /* Statistics: Consumer 3 - packet release (normal case) */
    g_nethub_stats.dnld_consumer_free_normal++;
    dnmsg->free_cb(dnmsg->ctx, dnmsg->arg);
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
    printf(" ---1 %s, line:%d, skb->data:%p, skb->len:%d, skb->cb_arg:%p\r\n", __func__, __LINE__, skb->data, skb->len, skb->cb_arg);
#endif
    trans_desc = (trans_desc_t *)((uintptr_t)skb->cb_arg + FRAME_BUFF_ELEM_SIZE);
    trans_desc->free_cb = skb->free_cb;
    trans_desc->ctx = skb->cb_priv;
    trans_desc->arg = skb->cb_arg;

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
#if DEBUG_DUMP_WIFITX_ENABE
    printf(" ---2 %s, line:%d, head:%p, pld:%p\r\n", __func__, __LINE__, trans_desc, trans_desc->pbuf.pbuf.payload);
#endif
    sta_input_portwifi_eth_tx(trans_desc, 1);
#endif // end DEBUG_BYPASS_WIFI_TX
    return 0;
}
// ========================================= dnld  =========================================

// ========================================= upld  =========================================
static int upld_custom_free(void *arg)
{
    struct pbuf *p = (struct pbuf *)arg;
#if DEBUG_DUMP_WIFIRX_ENABLE
    printf("%s ------------------------------------ p:%p, p->ref:%d\r\n\r\n", __func__, arg, p->ref);
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
        skb.cb_priv = NULL;// necessary?
        skb.cb_arg = p;
        skb.next = NULL;
#if DEBUG_DUMP_WIFIRX_ENABLE
        printf("%s ++++++++++++++++++++++++++++++ p=%p, pld:%p\r\n", __func__, skb.cb_arg, p->payload);
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
            //EXAMPLE_DEBUG("Data consumed, forwarding stopped\n");
        } else if (result == NH_FORWARD_CONTINUE) {
            //EXAMPLE_DEBUG("Data forwarding continues\n");
        } else {
            //EXAMPLE_DEBUG("Forwarding result abnormal: %d\n", result);
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
    bl_pkt_eth_input_hook_register(eth_input_hook_upld, NULL);//&ctx->rx_env);
    return NH_OK;
}

/* STA interface input callback */
static nh_forward_result_t _sta_input_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  /* Avoid unused parameter warning */
    // never here
    EXAMPLE_DEBUG("STA input Never here\n");
    return NH_FORWARD_CONTINUE;
}

/* STA interface output callback */
static nh_forward_result_t _sta_output_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  /* Avoid unused parameter warning */
    //EXAMPLE_DEBUG("@%p: %d\r\n\r\n", skb->data, skb->len);
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
