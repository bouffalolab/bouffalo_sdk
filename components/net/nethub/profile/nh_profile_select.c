#include "nh_profile.h"
#include "nh_wifi_bridge.h"

static const nh_profile_t *g_host_profiles[] = {
#if defined(CONFIG_NETHUB_PROFILE_SDIO) || (!defined(CONFIG_NETHUB_PROFILE_USB) && !defined(CONFIG_NETHUB_PROFILE_SPI))
    NULL,
#endif
#if defined(CONFIG_NETHUB_PROFILE_USB)
    NULL,
#endif
#if defined(CONFIG_NETHUB_PROFILE_SPI)
    NULL,
#endif
};

static void nh_profile_init_hosts(void)
{
    size_t i = 0;

#if defined(CONFIG_NETHUB_PROFILE_SDIO) || (!defined(CONFIG_NETHUB_PROFILE_USB) && !defined(CONFIG_NETHUB_PROFILE_SPI))
    g_host_profiles[i++] = nh_profile_sdio_get();
#endif
#if defined(CONFIG_NETHUB_PROFILE_USB)
    g_host_profiles[i++] = nh_profile_usb_get();
#endif
#if defined(CONFIG_NETHUB_PROFILE_SPI)
    g_host_profiles[i++] = nh_profile_spi_get();
#endif
    (void)i;
}

const nh_profile_t *nh_profile_get(void)
{
    nh_profile_init_hosts();
    return nh_profile_get_host(0);
}

size_t nh_profile_get_host_count(void)
{
    nh_profile_init_hosts();
    return ARRAY_SIZE(g_host_profiles);
}

const nh_profile_t *nh_profile_get_host(size_t index)
{
    nh_profile_init_hosts();
    if (index >= ARRAY_SIZE(g_host_profiles)) {
        return NULL;
    }

    return g_host_profiles[index];
}

const nh_profile_t *nh_profile_get_by_host(nethub_channel_t host_link)
{
    size_t i;

    nh_profile_init_hosts();
    for (i = 0; i < ARRAY_SIZE(g_host_profiles); i++) {
        if (g_host_profiles[i] != NULL &&
            g_host_profiles[i]->host_link_type == host_link) {
            return g_host_profiles[i];
        }
    }

    return NULL;
}

const char *nh_profile_get_name(void)
{
    const nh_profile_t *profile = nh_profile_get();

    return profile ? profile->name : "invalid";
}

nethub_channel_t nh_profile_get_host_link_type(void)
{
    const nh_profile_t *profile = nh_profile_get();

    return profile ? profile->host_link_type : NETHUB_CHANNEL_MAX;
}

const nh_filter_policy_t *nh_profile_get_wifi_rx_policy(void)
{
    const nh_profile_t *profile = nh_profile_get();

    return profile ? &profile->wifi_rx_policy : NULL;
}

const struct nhif_ops *nh_profile_get_endpoint_ops(size_t index)
{
    size_t host_count = nh_profile_get_host_count();
    size_t wifi_index;

    if (index < host_count) {
        const nh_profile_t *profile = nh_profile_get_host(index);
        return profile ? profile->host_endpoint_ops : NULL;
    }

    wifi_index = index - host_count;
    return nh_wifi_endpoint_get_ops(wifi_index);
}

size_t nh_profile_get_endpoint_count(void)
{
    return nh_profile_get_host_count() + nh_wifi_endpoint_get_count();
}

const nh_vchan_ops_t *nh_profile_get_vchan_ops(void)
{
    const nh_profile_t *profile = nh_profile_get();

    return profile ? profile->vchan_ops : NULL;
}
