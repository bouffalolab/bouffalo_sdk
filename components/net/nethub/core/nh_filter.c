#include "nh_filter.h"
#include "nh_hub.h"
#include "nh_packet_filter.h"
#include "nh_profile.h"

#define DBG_TAG "NETHUB_FILTER"
#include "log.h"

static nethub_wifi_rx_filter_cb_t g_wifi_rx_filter_cb;
static void *g_wifi_rx_filter_user_ctx;
static bool g_wifi_rx_filter_invalid_action_warned;

static bool nh_filter_action_is_valid(nh_filter_action_t action)
{
    return action == NH_FILTER_ACTION_DROP ||
           action == NH_FILTER_ACTION_LOCAL ||
           action == NH_FILTER_ACTION_HOST ||
           action == NH_FILTER_ACTION_BOTH;
}

static bool nh_filter_match_port_range(const struct pbuf *skb, uint8_t ip_proto,
                                       uint16_t port_min, uint16_t port_max)
{
    const struct ip_hdr *iphdr;

    if (port_min == 0 && port_max == 0) {
        return false;
    }

    if (port_min > port_max) {
        return false;
    }

    if (!npf_is_ip4(skb)) {
        return false;
    }

    iphdr = (const struct ip_hdr *)((uint8_t *)skb->payload + NPF_ETH_HDR_LEN);
    if (IPH_PROTO(iphdr) == IP_PROTO_TCP && ip_proto == IP_PROTO_TCP) {
        const struct tcp_hdr *tcphdr;
        uint16_t port;

        if (!(skb->len >= NPF_ETH_HDR_LEN + NPF_IP4_HDR_LEN + NPF_TCP4_HDR_LEN)) {
            return false;
        }

        tcphdr = (const struct tcp_hdr *)((uint8_t *)skb->payload + NPF_ETH_HDR_LEN + NPF_IP4_HDR_LEN);
        port = lwip_ntohs(tcphdr->dest);
        return port >= port_min && port <= port_max;
    }

    if (IPH_PROTO(iphdr) == IP_PROTO_UDP && ip_proto == IP_PROTO_UDP) {
        const struct udp_hdr *udphdr;
        uint16_t port;

        if (!(skb->len >= NPF_ETH_HDR_LEN + NPF_IP4_HDR_LEN + NPF_UDP4_HDR_LEN)) {
            return false;
        }

        udphdr = (const struct udp_hdr *)((uint8_t *)skb->payload + NPF_ETH_HDR_LEN + NPF_IP4_HDR_LEN);
        port = lwip_ntohs(udphdr->dest);
        return port >= port_min && port <= port_max;
    }

    return false;
}

static bool nh_filter_rule_matches(const struct pbuf *pkt, const nh_filter_rule_t *rule)
{
    if (pkt == NULL || rule == NULL) {
        return false;
    }

    switch (rule->match) {
        case NH_FILTER_MATCH_8021X:
            return npf_is_8021X(pkt);
        case NH_FILTER_MATCH_ARP:
            return npf_is_arp(pkt);
        case NH_FILTER_MATCH_DHCP4:
            return npf_is_dhcp4(pkt);
        case NH_FILTER_MATCH_ICMP4:
            return npf_is_icmp4(pkt);
        case NH_FILTER_MATCH_TCP4_DST_PORT_RANGE:
            return nh_filter_match_port_range(pkt, IP_PROTO_TCP, rule->port_min, rule->port_max);
        case NH_FILTER_MATCH_UDP4_DST_PORT_RANGE:
            return nh_filter_match_port_range(pkt, IP_PROTO_UDP, rule->port_min, rule->port_max);
        default:
            return false;
    }
}

nh_filter_action_t nh_filter_apply_policy(const struct pbuf *pkt, const nh_filter_policy_t *policy)
{
    size_t i;

    if (pkt == NULL) {
        return NH_FILTER_ACTION_DROP;
    }

    if (policy == NULL) {
        return NH_FILTER_ACTION_HOST;
    }

    for (i = 0; i < policy->rule_count; i++) {
        if (nh_filter_rule_matches(pkt, &policy->rules[i])) {
            return policy->rules[i].action;
        }
    }

    return policy->default_action;
}

int nethub_set_wifi_rx_filter(nethub_wifi_rx_filter_cb_t filter_cb, void *user_ctx)
{
    if (nethub_is_initialized()) {
        return NETHUB_ERR_INVALID_STATE;
    }

    g_wifi_rx_filter_cb = filter_cb;
    g_wifi_rx_filter_user_ctx = user_ctx;
    g_wifi_rx_filter_invalid_action_warned = false;

    if (filter_cb == NULL) {
        LOG_I("wifi rx filter restored to builtin policy\r\n");
    } else {
        LOG_W("custom wifi rx filter is enabled, builtin policy will be bypassed\r\n");
    }

    return NETHUB_OK;
}

nh_filter_action_t nh_filter_wifi_rx(nethub_channel_t src_channel, const struct pbuf *pkt)
{
    nh_filter_action_t action;

    if (g_wifi_rx_filter_cb != NULL) {
        action = g_wifi_rx_filter_cb(src_channel, pkt, g_wifi_rx_filter_user_ctx);
        if (nh_filter_action_is_valid(action)) {
            return action;
        }

        if (!g_wifi_rx_filter_invalid_action_warned) {
            g_wifi_rx_filter_invalid_action_warned = true;
            LOG_W("custom wifi rx filter returned invalid action=%d, packet dropped\r\n",
                  (int)action);
        }
        return NH_FILTER_ACTION_DROP;
    }

    return nh_filter_apply_policy(pkt, nh_profile_get_wifi_rx_policy());
}

bool nh_filter_should_deliver_local(nh_filter_action_t action)
{
    return (action & NH_FILTER_ACTION_LOCAL) != 0;
}

bool nh_filter_should_deliver_host(nh_filter_action_t action)
{
    return (action & NH_FILTER_ACTION_HOST) != 0;
}

bool nh_filter_custom_wifi_rx_is_active(void)
{
    return g_wifi_rx_filter_cb != NULL;
}
