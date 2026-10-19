#include "nethub_wifi_service.h"

#include <stdlib.h>
#include <string.h>

#include "nethub_ctrl_proto.h"

static const nethub_ctrl_proto_ops_t *g_proto;
static int g_service_initialized;

static const nethub_ctrl_proto_ops_t *select_proto(void)
{
    const char *proto = getenv("NETHUB_WIFI_PROTO");

    if (proto && strcmp(proto, "private") == 0) {
        return nethub_ctrl_proto_private();
    }

    return nethub_ctrl_proto_at();
}

int nethub_wifi_service_init(void)
{
    int ret;

    if (g_service_initialized) {
        return 0;
    }

    g_proto = select_proto();
    if (!g_proto || !g_proto->init) {
        return NETHUB_WIFI_ERR_PROTOCOL;
    }

    ret = g_proto->init();
    if (ret != 0) {
        g_proto = NULL;
        return ret;
    }

    g_service_initialized = 1;
    return 0;
}

void nethub_wifi_service_deinit(void)
{
    if (g_service_initialized && g_proto && g_proto->deinit) {
        g_proto->deinit();
    }
    g_service_initialized = 0;
    g_proto = NULL;
}

static int require_service(void)
{
    return g_service_initialized && g_proto ? 0 : NETHUB_WIFI_ERR_NOT_INITIALIZED;
}

int nethub_wifi_service_get_status(nethub_wifi_status_t *status)
{
    int ret = require_service();
    if (ret != 0) {
        return ret;
    }
    return g_proto->get_status ? g_proto->get_status(status) : NETHUB_WIFI_ERR_NOT_SUPPORTED;
}

int nethub_wifi_service_get_version(char *buf, size_t len)
{
    int ret = require_service();
    if (ret != 0) {
        return ret;
    }
    return g_proto->get_version ? g_proto->get_version(buf, len) : NETHUB_WIFI_ERR_NOT_SUPPORTED;
}

int nethub_wifi_service_reboot(void)
{
    int ret = require_service();
    if (ret != 0) {
        return ret;
    }
    return g_proto->reboot ? g_proto->reboot() : NETHUB_WIFI_ERR_NOT_SUPPORTED;
}

int nethub_wifi_service_sta_connect(const nethub_wifi_sta_config_t *cfg, int timeout_ms)
{
    int ret = require_service();
    if (ret != 0) {
        return ret;
    }
    return g_proto->sta_connect ? g_proto->sta_connect(cfg, timeout_ms) : NETHUB_WIFI_ERR_NOT_SUPPORTED;
}

int nethub_wifi_service_sta_disconnect(void)
{
    int ret = require_service();
    if (ret != 0) {
        return ret;
    }
    return g_proto->sta_disconnect ? g_proto->sta_disconnect() : NETHUB_WIFI_ERR_NOT_SUPPORTED;
}

int nethub_wifi_service_scan(nethub_wifi_ap_info_t *aps, int max_count, int *actual_count, int timeout_ms)
{
    int ret = require_service();
    if (ret != 0) {
        return ret;
    }
    return g_proto->scan ? g_proto->scan(aps, max_count, actual_count, timeout_ms) : NETHUB_WIFI_ERR_NOT_SUPPORTED;
}

int nethub_wifi_service_ap_start(const nethub_wifi_ap_config_t *cfg, int timeout_ms)
{
    int ret = require_service();
    if (ret != 0) {
        return ret;
    }
    return g_proto->ap_start ? g_proto->ap_start(cfg, timeout_ms) : NETHUB_WIFI_ERR_NOT_SUPPORTED;
}

int nethub_wifi_service_ap_stop(void)
{
    int ret = require_service();
    if (ret != 0) {
        return ret;
    }
    return g_proto->ap_stop ? g_proto->ap_stop() : NETHUB_WIFI_ERR_NOT_SUPPORTED;
}

int nethub_wifi_service_ota_update(const char *fw_path, int timeout_ms)
{
    int ret = require_service();
    if (ret != 0) {
        return ret;
    }
    return g_proto->ota_update ? g_proto->ota_update(fw_path, timeout_ms) : NETHUB_WIFI_ERR_NOT_SUPPORTED;
}
