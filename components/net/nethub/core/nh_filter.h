#ifndef NH_FILTER_H
#define NH_FILTER_H

#include "nethub_filter.h"
#include "nethub_defs.h"

#include <stddef.h>
#include <lwip/pbuf.h>

typedef nethub_wifi_rx_filter_action_t nh_filter_action_t;

#define NH_FILTER_ACTION_DROP  NETHUB_WIFI_RX_FILTER_DROP
#define NH_FILTER_ACTION_LOCAL NETHUB_WIFI_RX_FILTER_LOCAL
#define NH_FILTER_ACTION_HOST  NETHUB_WIFI_RX_FILTER_HOST
#define NH_FILTER_ACTION_BOTH  NETHUB_WIFI_RX_FILTER_BOTH

typedef enum {
    NH_FILTER_MATCH_8021X = 0,
    NH_FILTER_MATCH_ARP,
    NH_FILTER_MATCH_DHCP4,
    NH_FILTER_MATCH_ICMP4,
    NH_FILTER_MATCH_TCP4_DST_PORT_RANGE,
    NH_FILTER_MATCH_UDP4_DST_PORT_RANGE,
} nh_filter_match_t;

typedef struct {
    nh_filter_match_t match;
    nh_filter_action_t action;
    uint16_t port_min;
    uint16_t port_max;
} nh_filter_rule_t;

typedef struct {
    const nh_filter_rule_t *rules;
    size_t rule_count;
    nh_filter_action_t default_action;
} nh_filter_policy_t;

nh_filter_action_t nh_filter_wifi_rx(nethub_channel_t src_channel, const struct pbuf *pkt);
nh_filter_action_t nh_filter_apply_policy(const struct pbuf *pkt, const nh_filter_policy_t *policy);
bool nh_filter_should_deliver_local(nh_filter_action_t action);
bool nh_filter_should_deliver_host(nh_filter_action_t action);
bool nh_filter_custom_wifi_rx_is_active(void);

#endif
