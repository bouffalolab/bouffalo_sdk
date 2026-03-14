#include "nh_profile.h"
#include "nh_profile_builtin.h"
#include "transport_sdio.h"

static const nh_profile_t g_sdio_bridge_profile = {
    .name = "sdio_bridge",
    .host_link_type = NETHUB_CHANNEL_SDIO,
    .wifi_rx_policy = {
        .rules = g_nh_profile_builtin_wifi_rx_rules,
        .rule_count = NH_PROFILE_BUILTIN_WIFI_RX_RULE_COUNT,
        .default_action = NH_PROFILE_BUILTIN_WIFI_RX_DEFAULT_ACTION,
    },
    .host_endpoint_ops = &nhsdio_ops,
    .ctrlpath_ops = &nhsdio_ctrlpath_ops,
};

const nh_profile_t *nh_profile_sdio_get(void)
{
    return &g_sdio_bridge_profile;
}
