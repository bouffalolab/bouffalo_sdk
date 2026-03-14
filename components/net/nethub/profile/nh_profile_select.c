#include "nh_profile.h"
#include "nh_wifi_bridge.h"

#if (defined(CONFIG_NETHUB_PROFILE_SDIO) + defined(CONFIG_NETHUB_PROFILE_USB) + defined(CONFIG_NETHUB_PROFILE_SPI)) > 1
#error "Only one CONFIG_NETHUB_PROFILE_* may be selected"
#endif

const nh_profile_t *nh_profile_get(void)
{
#if defined(CONFIG_NETHUB_PROFILE_USB)
    return nh_profile_usb_get();
#elif defined(CONFIG_NETHUB_PROFILE_SPI)
    return nh_profile_spi_get();
#else
    return nh_profile_sdio_get();
#endif
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
    const nh_profile_t *profile = nh_profile_get();
    size_t wifi_endpoint_base = 1;
    size_t wifi_index;

    if (profile == NULL) {
        return NULL;
    }

    if (index == 0) {
        return profile->host_endpoint_ops;
    }

    wifi_index = index - wifi_endpoint_base;
    return nh_wifi_endpoint_get_ops(wifi_index);
}

size_t nh_profile_get_endpoint_count(void)
{
    const nh_profile_t *profile = nh_profile_get();

    return profile ? (1 + nh_wifi_endpoint_get_count()) : 0;
}

const nh_ctrlpath_ops_t *nh_profile_get_ctrlpath_ops(void)
{
    const nh_profile_t *profile = nh_profile_get();

    return profile ? profile->ctrlpath_ops : NULL;
}
