/**
 * @file bflbwifid_service.c
 * @brief WiFi command execution and response formatting for bflbwifid
 */

#include "bflbwifid_service.h"

#include <stdio.h>
#include <string.h>

#include "../include/bflbwifi.h"
#include "bflbwifid_log.h"
#include "bflbwifid_runtime.h"

static void service_copy_string(char *dst, size_t dst_size, const char *src)
{
    size_t copy_len;

    if (!dst || dst_size == 0) {
        return;
    }

    if (!src) {
        dst[0] = '\0';
        return;
    }

    copy_len = strlen(src);
    if (copy_len >= dst_size) {
        copy_len = dst_size - 1u;
    }

    memcpy(dst, src, copy_len);
    dst[copy_len] = '\0';
}

static int service_write_error(char *response, size_t response_len, int err)
{
    if (!response || response_len == 0) {
        return -1;
    }

    snprintf(response, response_len, "ERROR %d %s\nEND\n", err, bflbwifi_strerror(err));
    return 0;
}

static const char *service_wifi_state_name(bflbwifi_sta_state_t state)
{
    switch (state) {
        case WIFI_STATE_IDLE:
            return "IDLE";
        case WIFI_STATE_CONNECTING:
            return "CONNECTING";
        case WIFI_STATE_CONNECTED:
            return "CONNECTED";
        case WIFI_STATE_GOTIP:
            return "GOTIP";
        case WIFI_STATE_DISCONNECTED:
            return "DISCONNECTED";
        case WIFI_STATE_ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

static int service_handle_connect_ap(char *response, size_t response_len, const bflbwifi_ipc_request_t *request)
{
    int ret;
    const char *ssid = request->argv[1];
    const char *pwd = (request->argc > 2 && request->argv[2]) ? request->argv[2] : "";
    bflbwifi_sta_config_t config;

    BFLBWIFID_LOGI("Connect to AP: %s", ssid);

    memset(&config, 0, sizeof(config));
    service_copy_string(config.ssid, sizeof(config.ssid), ssid);
    service_copy_string(config.pwd, sizeof(config.pwd), pwd);

    ret = bflbwifi_sta_connect(&config, 30000);
    if (ret == 0) {
        bflbwifi_sta_connection_info_t info;

        if (bflbwifi_sta_get_connection_info(&info) == 0) {
            snprintf(response, response_len,
                     "OK CONNECTED\n"
                     "SSID: %s\n"
                     "BSSID: %s\n"
                     "Channel: %d\n"
                     "RSSI: %d dBm\n"
                     "END\n",
                     info.ssid,
                     info.bssid,
                     info.channel,
                     info.rssi);
        } else {
            snprintf(response, response_len, "OK CONNECTED\nEND\n");
        }
        return 0;
    }

    return service_write_error(response, response_len, ret);
}

static int service_handle_disconnect(char *response, size_t response_len)
{
    int ret;

    BFLBWIFID_LOGI("Disconnect");

    ret = bflbwifi_sta_disconnect();
    if (ret == 0) {
        snprintf(response, response_len, "OK DISCONNECTED\nEND\n");
        return 0;
    }

    return service_write_error(response, response_len, ret);
}

static int service_handle_scan(char *response, size_t response_len)
{
    int ret;
    int actual_count;
    int remaining;
    char *ptr;
    bflbwifi_ap_info_t ap_list[64];

    BFLBWIFID_LOGI("Scan APs");

    ret = bflbwifi_scan(ap_list, 64, &actual_count, 30000);
    if (ret != 0 && ret != E_ERR_BUFFER_TOO_SMALL) {
        return service_write_error(response, response_len, ret);
    }

    ptr = response;
    remaining = (int)response_len;

    ptr += snprintf(ptr, remaining, "OK SCAN_COMPLETE\n");
    remaining = (int)(response_len - (size_t)(ptr - response));

    for (int i = 0; i < actual_count && remaining > 200; i++) {
        int len = snprintf(ptr,
                           remaining,
                           "AP[%d]: %s (CH:%d, RSSI:%d, MAC:%s)\n",
                           i + 1,
                           ap_list[i].ssid,
                           ap_list[i].channel,
                           ap_list[i].rssi,
                           ap_list[i].bssid);
        if (len < 0 || len >= remaining) {
            break;
        }
        ptr += len;
        remaining -= len;
    }

    snprintf(ptr, (size_t)remaining, "TOTAL: %d\nEND\n", actual_count);
    return 0;
}

static int service_handle_status(char *response, size_t response_len)
{
    char vchan_state_line[64];
    const char *state_str;
    bflbwifi_ctrl_status_t ctrl_status;
    bflbwifi_sta_state_t state;
    bflbwifid_state_t daemon_state;
    bflbwifi_sta_connection_info_t info;

    BFLBWIFID_LOGI("Get status");

    if (bflbwifid_refresh_runtime_state(false) != 0 ||
        bflbwifi_sta_get_state(&state) != 0) {
        snprintf(response, response_len, "ERROR -1 Failed to get status\nEND\n");
        return -1;
    }

    bflbwifid_get_runtime_snapshot(&daemon_state, &ctrl_status);
    state_str = service_wifi_state_name(state);
    bflbwifid_get_vchan_host_state_line(vchan_state_line, sizeof(vchan_state_line), ctrl_status.ctrl_backend);

    if (bflbwifi_sta_get_connection_info(&info) == 0 &&
        (state == WIFI_STATE_CONNECTED || state == WIFI_STATE_GOTIP)) {
        snprintf(response, response_len,
                 "OK STATUS\n"
                 "DaemonState: %s\n"
                 "Backend: %s\n"
                 "BackendState: %s\n"
                 "%s"
                 "LinkEvents: %s\n"
                 "OTA: %s\n"
                 "State: %s\n"
                 "SSID: %s\n"
                 "BSSID: %s\n"
                 "Channel: %d\n"
                 "RSSI: %d dBm\n"
                 "IP: %s\n"
                 "Gateway: %s\n"
                 "Netmask: %s\n"
                 "DNS: %s\n"
                 "END\n",
                 bflbwifid_state_name(daemon_state),
                 bflbwifi_ctrl_backend_name(ctrl_status.ctrl_backend),
                 bflbwifi_ctrl_link_state_name(ctrl_status.link_state),
                 vchan_state_line,
                 ctrl_status.link_events_supported ? "YES" : "NO",
                 ctrl_status.ota_in_progress ? "IN_PROGRESS" : "IDLE",
                 state_str,
                 info.ssid,
                 info.bssid,
                 info.channel,
                 info.rssi,
                 info.ip,
                 info.gateway,
                 info.netmask,
                 info.dns);
    } else {
        snprintf(response, response_len,
                 "OK STATUS\n"
                 "DaemonState: %s\n"
                 "Backend: %s\n"
                 "BackendState: %s\n"
                 "%s"
                 "LinkEvents: %s\n"
                 "OTA: %s\n"
                 "State: %s\n"
                 "END\n",
                 bflbwifid_state_name(daemon_state),
                 bflbwifi_ctrl_backend_name(ctrl_status.ctrl_backend),
                 bflbwifi_ctrl_link_state_name(ctrl_status.link_state),
                 vchan_state_line,
                 ctrl_status.link_events_supported ? "YES" : "NO",
                 ctrl_status.ota_in_progress ? "IN_PROGRESS" : "IDLE",
                 state_str);
    }

    return 0;
}

static int service_handle_version(char *response, size_t response_len)
{
    char version[64];

    BFLBWIFID_LOGI("Get version");

    if (bflbwifi_get_version(version, sizeof(version)) == 0) {
        snprintf(response, response_len,
                 "OK VERSION\n"
                 "Version: %s\n"
                 "END\n",
                 version);
        return 0;
    }

    snprintf(response, response_len, "ERROR -1 Failed to get version\nEND\n");
    return -1;
}

static int service_handle_reboot(char *response, size_t response_len)
{
    int ret;

    BFLBWIFID_LOGI("Reboot WiFi module");

    ret = bflbwifi_restart();
    if (ret == 0) {
        snprintf(response, response_len, "OK REBOOTING\nEND\n");
        return 0;
    }

    return service_write_error(response, response_len, ret);
}

static int service_handle_ota(char *response, size_t response_len, const bflbwifi_ipc_request_t *request)
{
    int ret;
    const char *filepath = (request->argc > 1) ? request->argv[1] : NULL;

    if (!filepath || filepath[0] == '\0') {
        snprintf(response, response_len, "ERROR -4 Missing OTA file path\nEND\n");
        return -1;
    }

    BFLBWIFID_LOGI("OTA upgrade: %s", filepath);

    ret = bflbwifi_ota_upgrade(filepath);
    if (ret == 0) {
        snprintf(response, response_len, "OK OTA_COMPLETE\nEND\n");
        return 0;
    }

    return service_write_error(response, response_len, ret);
}

static int service_handle_start_ap(char *response, size_t response_len, const bflbwifi_ipc_request_t *request)
{
    int ret;
    const char *ssid = request->argv[1];
    const char *password = (request->argc > 2 && request->argv[2]) ? request->argv[2] : NULL;
    bflbwifi_ap_config_t config;

    if (!ssid || ssid[0] == '\0') {
        snprintf(response, response_len, "ERROR -4 Missing SSID\nEND\n");
        return -1;
    }

    BFLBWIFID_LOGI("Start AP: %s", ssid);

    memset(&config, 0, sizeof(config));
    service_copy_string(config.ssid, sizeof(config.ssid), ssid);
    if (password) {
        service_copy_string(config.pwd, sizeof(config.pwd), password);
    }
    config.channel = 11;
    config.enc = WIFI_ENC_WPA2;
    config.max_conn = 4;
    config.ssid_hidden = 0;

    ret = bflbwifi_ap_start(&config, 30000);
    if (ret == 0) {
        snprintf(response, response_len,
                 "OK AP_STARTED\n"
                 "SSID: %s\n"
                 "Channel: %d\n"
                 "IP: 192.168.4.1\n"
                 "DHCP: 192.168.4.50 - 192.168.4.200\n"
                 "END\n",
                 config.ssid,
                 config.channel);
        return 0;
    }

    return service_write_error(response, response_len, ret);
}

static int service_handle_stop_ap(char *response, size_t response_len)
{
    int ret;

    BFLBWIFID_LOGI("Stop AP");

    ret = bflbwifi_ap_stop();
    if (ret == 0) {
        snprintf(response, response_len, "OK AP_STOPPED\nEND\n");
        return 0;
    }

    return service_write_error(response, response_len, ret);
}

int bflbwifid_service_execute(char *response,
                              size_t response_len,
                              const bflbwifi_command_desc_t *command,
                              const bflbwifi_ipc_request_t *request)
{
    if (!response || response_len == 0 || !command || !request) {
        return -1;
    }

    switch (command->id) {
        case BFLBWIFI_COMMAND_CONNECT_AP:
            return service_handle_connect_ap(response, response_len, request);
        case BFLBWIFI_COMMAND_DISCONNECT:
            return service_handle_disconnect(response, response_len);
        case BFLBWIFI_COMMAND_SCAN:
            return service_handle_scan(response, response_len);
        case BFLBWIFI_COMMAND_STATUS:
            return service_handle_status(response, response_len);
        case BFLBWIFI_COMMAND_VERSION:
            return service_handle_version(response, response_len);
        case BFLBWIFI_COMMAND_REBOOT:
            return service_handle_reboot(response, response_len);
        case BFLBWIFI_COMMAND_OTA:
            return service_handle_ota(response, response_len, request);
        case BFLBWIFI_COMMAND_START_AP:
            return service_handle_start_ap(response, response_len, request);
        case BFLBWIFI_COMMAND_STOP_AP:
            return service_handle_stop_ap(response, response_len);
        default:
            snprintf(response, response_len, "ERROR -10 Command not supported\nEND\n");
            return -1;
    }
}
