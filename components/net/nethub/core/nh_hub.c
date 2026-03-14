#include "nh_hub.h"
#include "nh_filter.h"
#include "nh_internal.h"
#include "nh_profile.h"
#include "nh_runtime.h"

#define DBG_TAG "NETHUB"
#include "log.h"

static nethub_context_t g_nethub_ctx = {
    .host_link_type = NETHUB_CHANNEL_MAX,
};

nethub_context_t *nh_ctx_get(void)
{
    return &g_nethub_ctx;
}

int nethub_init(const nethub_config_t *config)
{
    const nh_profile_t *profile;

    NETHUB_UNUSED(config);

    if (g_nethub_ctx.initialized) {
        return NETHUB_ERR_ALREADY_EXISTS;
    }

    profile = nh_profile_get();
    if (profile == NULL) {
        return NETHUB_ERR_INTERNAL;
    }

    g_nethub_ctx.profile = profile;
    g_nethub_ctx.host_link_type = profile->host_link_type;
    g_nethub_ctx.initialized = true;
    g_nethub_ctx.started = false;

    LOG_I("init profile=%s host_link=%s\r\n",
          profile->name,
          nethub_channel_to_string(g_nethub_ctx.host_link_type));
    return NETHUB_OK;
}

int nethub_deinit(void)
{
    if (!g_nethub_ctx.initialized) {
        return NETHUB_ERR_NOT_INITIALIZED;
    }

    if (g_nethub_ctx.started) {
        int ret = nethub_stop();
        if (ret != NETHUB_OK) {
            return ret;
        }
    }

    nhif_unregister_all();
    g_nethub_ctx.initialized = false;
    g_nethub_ctx.started = false;
    g_nethub_ctx.profile = NULL;
    g_nethub_ctx.host_link_type = NETHUB_CHANNEL_MAX;

    return NETHUB_OK;
}

int nethub_start(void)
{
    int ret;

    if (!g_nethub_ctx.initialized) {
        return NETHUB_ERR_NOT_INITIALIZED;
    }

    if (g_nethub_ctx.started) {
        return NETHUB_OK;
    }

    ret = nhif_init_all();
    if (ret != NETHUB_OK) {
        return ret;
    }

    g_nethub_ctx.started = true;
    return NETHUB_OK;
}

int nethub_stop(void)
{
    int ret;

    if (!g_nethub_ctx.started) {
        return NETHUB_OK;
    }

    ret = nhif_deinit_all();
    if (ret != NETHUB_OK) {
        return ret;
    }

    g_nethub_ctx.started = false;
    return NETHUB_OK;
}

nethub_route_result_t nethub_send_output(nethub_frame_t *frame, nethub_channel_t dst_type)
{
    nhif_instance_t *instance;

    if (!g_nethub_ctx.initialized || !g_nethub_ctx.started) {
        return NETHUB_ROUTE_ERROR;
    }

    if (frame == NULL) {
        return NETHUB_ROUTE_ERROR;
    }

    instance = nhif_find_by_type(dst_type);
    if (instance == NULL || instance->state != NETHUB_ENDPOINT_STATE_ACTIVE) {
        return NETHUB_ROUTE_ERROR;
    }

    if (instance->ops == NULL || instance->ops->output == NULL) {
        return NETHUB_ROUTE_ERROR;
    }

    return instance->ops->output(frame, instance->private_data);
}

nethub_route_result_t nethub_process_input(nethub_frame_t *frame, nethub_channel_t src_type)
{
    nethub_channel_t dst_type;

    if (!g_nethub_ctx.initialized || !g_nethub_ctx.started) {
        return NETHUB_ROUTE_ERROR;
    }

    if (frame == NULL) {
        return NETHUB_ROUTE_ERROR;
    }

    switch (src_type) {
        case NETHUB_CHANNEL_WIFI_STA:
        case NETHUB_CHANNEL_WIFI_AP:
            dst_type = g_nethub_ctx.host_link_type;
            break;
        case NETHUB_CHANNEL_SDIO:
        case NETHUB_CHANNEL_USB:
        case NETHUB_CHANNEL_SPI:
            dst_type = nh_runtime_get_active_wifi_channel();
            break;
        default:
            return NETHUB_ROUTE_ERROR;
    }

    return nethub_send_output(frame, dst_type);
}

int nethub_get_status(nethub_runtime_status_t *status)
{
    if (status == NULL) {
        return NETHUB_ERR_INVALID_PARAM;
    }

    status->initialized = g_nethub_ctx.initialized;
    status->started = g_nethub_ctx.started;
    status->custom_wifi_rx_filter_active = nh_filter_custom_wifi_rx_is_active();
    status->profile_name = nethub_profile_name();
    status->host_channel = nethub_host_channel();
    status->active_wifi_channel = nethub_get_active_wifi_channel();
    status->statistics = *nh_runtime_stats_readonly();

    return NETHUB_OK;
}

bool nethub_is_initialized(void)
{
    return g_nethub_ctx.initialized;
}

bool nethub_is_started(void)
{
    return g_nethub_ctx.started;
}

const char *nethub_profile_name(void)
{
    return g_nethub_ctx.profile ? g_nethub_ctx.profile->name : nh_profile_get_name();
}

nethub_channel_t nethub_host_channel(void)
{
    return g_nethub_ctx.host_link_type;
}

nethub_channel_t nethub_get_active_wifi_channel(void)
{
    return nh_runtime_get_active_wifi_channel();
}
