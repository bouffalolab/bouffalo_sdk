#include "nh_profile_builtin.h"

const nh_filter_rule_t g_nh_profile_builtin_wifi_rx_rules[] = {
    {
        .match = NH_FILTER_MATCH_8021X,
        .action = NH_FILTER_ACTION_LOCAL,
    },
    {
        .match = NH_FILTER_MATCH_ARP,
        .action = NH_FILTER_ACTION_BOTH,
    },
    {
        .match = NH_FILTER_MATCH_DHCP4,
        .action = NH_FILTER_ACTION_LOCAL,
    },
    {
        .match = NH_FILTER_MATCH_ICMP4,
        .action = NH_FILTER_ACTION_LOCAL,
    },
#if ((CONFIG_NETHUB_FILTER_LOCAL_PORT_MIN != 0) || (CONFIG_NETHUB_FILTER_LOCAL_PORT_MAX != 0))
    {
        .match = NH_FILTER_MATCH_TCP4_DST_PORT_RANGE,
        .action = NH_FILTER_ACTION_LOCAL,
        .port_min = CONFIG_NETHUB_FILTER_LOCAL_PORT_MIN,
        .port_max = CONFIG_NETHUB_FILTER_LOCAL_PORT_MAX,
    },
    {
        .match = NH_FILTER_MATCH_UDP4_DST_PORT_RANGE,
        .action = NH_FILTER_ACTION_LOCAL,
        .port_min = CONFIG_NETHUB_FILTER_LOCAL_PORT_MIN,
        .port_max = CONFIG_NETHUB_FILTER_LOCAL_PORT_MAX,
    },
#endif
};
