#include "nh_runtime.h"

static nethub_channel_t g_active_wifi_channel = NETHUB_CHANNEL_WIFI_STA;
static nethub_statistics_t g_nethub_statistics;

nethub_channel_t nh_runtime_get_active_wifi_channel(void)
{
    return g_active_wifi_channel;
}

int nh_runtime_set_active_wifi_channel(nethub_channel_t channel)
{
    if ((channel != NETHUB_CHANNEL_WIFI_STA) &&
        (channel != NETHUB_CHANNEL_WIFI_AP)) {
        return -1;
    }

    g_active_wifi_channel = channel;
    return 0;
}

nethub_statistics_t *nh_runtime_stats(void)
{
    return &g_nethub_statistics;
}

const nethub_statistics_t *nh_runtime_stats_readonly(void)
{
    return &g_nethub_statistics;
}
