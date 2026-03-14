#include "nh_profile.h"
#include "nh_profile_builtin.h"
#include "transport_spi.h"

static const nh_profile_t g_spi_bridge_profile = {
    .name = "spi_bridge",
    .host_link_type = NETHUB_CHANNEL_SPI,
    .wifi_rx_policy = {
        .rules = g_nh_profile_builtin_wifi_rx_rules,
        .rule_count = NH_PROFILE_BUILTIN_WIFI_RX_RULE_COUNT,
        .default_action = NH_PROFILE_BUILTIN_WIFI_RX_DEFAULT_ACTION,
    },
    .host_endpoint_ops = &nhspi_ops,
    .ctrlpath_ops = &nhspi_ctrlpath_ops,
};

const nh_profile_t *nh_profile_spi_get(void)
{
    return &g_spi_bridge_profile;
}
