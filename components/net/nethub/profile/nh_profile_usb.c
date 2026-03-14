#include "nh_profile.h"
#include "nh_profile_builtin.h"
#include "transport_usb.h"

static const nh_profile_t g_usb_bridge_profile = {
    .name = "usb_bridge",
    .host_link_type = NETHUB_CHANNEL_USB,
    .wifi_rx_policy = {
        .rules = g_nh_profile_builtin_wifi_rx_rules,
        .rule_count = NH_PROFILE_BUILTIN_WIFI_RX_RULE_COUNT,
        .default_action = NH_PROFILE_BUILTIN_WIFI_RX_DEFAULT_ACTION,
    },
    .host_endpoint_ops = &nhusb_ops,
    .ctrlpath_ops = &nhusb_ctrlpath_ops,
};

const nh_profile_t *nh_profile_usb_get(void)
{
    return &g_usb_bridge_profile;
}
