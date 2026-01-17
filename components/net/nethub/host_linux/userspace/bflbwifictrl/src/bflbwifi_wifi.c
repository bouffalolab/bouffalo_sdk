/**
 * @file bflbwifi.c
 * @brief BFLB WiFi module API implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#include "../include/bflbwifi_log.h"
#include "../include/bflbwifi.h"

/* TTY callback setup functions (defined in bflbwifi_tty.c) */
extern void bflbwifi_tty_set_disconnect_callback(void (*callback)(void));
extern void bflbwifi_tty_set_reconnect_callback(void (*callback)(void));

/* ========== Constant Definitions ========== */

/* TTY command response status (corresponds to cmd_response_t in bflb_tty.c) */
#define CMD_RESP_PENDING     0
#define CMD_RESP_OK          1
#define CMD_RESP_ERROR       2
#define CMD_RESP_TIMEOUT     3

/* Error code constants */
#define E_SUCCESS               0
#define E_ERR                  -1
#define E_ERR_INVALID_PARAM     -4
#define E_ERR_NOT_INIT         -9
#define E_ERR_TTY_FAIL         -2
#define E_ERR_BUFFER_TOO_SMALL -11

/* ========== Forward Declarations ========== */

/* TTY interface */
extern int bflbwifi_tty_init(const char *dev, int baudrate,
                        void (*callback)(const char *line, void *user_data),
                        void *user_data);
extern int bflbwifi_tty_send(const char *data, int len);
extern int bflbwifi_tty_send_command(const char *cmd);
extern int bflbwifi_tty_send_command_sync(const char *cmd, int timeout_ms);
extern void bflbwifi_tty_deinit(void);
extern int bflbwifi_tty_write_raw(const void *data, int len);
extern int bflbwifi_tty_wait_for(const char *expected, int timeout_ms);
extern int bflbwifi_tty_get_last_response(char *buf, size_t len);
extern void bflbwifi_tty_clear_last_response(void);
extern int bflbwifi_tty_suspend_recv_thread(void);
extern int bflbwifi_tty_resume_recv_thread(void);
extern int bflbwifi_tty_get_fd(void);

/* State management interface */
extern int bflbwifi_state_init(void);
extern void bflbwifi_state_deinit(void);
extern void bflbwifi_state_handle_urc(int type, const char *line);
extern int bflbwifi_state_get_mode(bflbwifi_mode_t *mode);
extern void bflbwifi_state_set_mode(bflbwifi_mode_t mode);
extern int bflbwifi_state_get_sta_state(bflbwifi_sta_state_t *state);
extern void bflbwifi_state_set_sta_state(bflbwifi_sta_state_t state);
extern int bflbwifi_state_get_sta_info(bflbwifi_sta_connection_info_t *info);
extern void bflbwifi_state_set_sta_info(const bflbwifi_sta_connection_info_t *info);
extern void bflbwifi_state_reset_scan(void);
extern int bflbwifi_state_wait_scan_complete(int timeout_ms);
extern int bflbwifi_state_get_scan_results(bflbwifi_ap_info_t *ap_list, int max_count, int *actual_count);
extern int bflbwifi_state_wait_sta_state(bflbwifi_sta_state_t target_state, int timeout_ms);

/* Parser interface */
extern void bflbwifi_parser_process_line(const char *line, void *user_data);
extern int bflbwifi_parser_parse_cwjap(const char *line, bflbwifi_sta_connection_info_t *info);
extern void bflbwifi_state_set_sta_info(const bflbwifi_sta_connection_info_t *info);

/* ========== Global Variables ========== */

static bool g_initialized = false;

/* ========== Helper Functions ========== */

/**
 * @brief Convert error code to string
 */
const char* bflbwifi_strerror(int err)
{
    switch (err) {
#define X(name, val, desc) case name: return desc;
        BFLBWIFI_ERRORS
#undef X
        default:
            return "Unknown error";
    }
}

/**
 * @brief TTY data callback
 */
static void tty_data_callback(const char *line, void *user_data)
{
    /* Pass to parser for processing */
    bflbwifi_parser_process_line(line, user_data);
}

/**
 * @brief TTY disconnect callback
 */
static void tty_disconnect_callback(void)
{
    BFLB_LOGW("TTY disconnected, waiting for reconnection...");
}

/**
 * @brief TTY reconnect success callback
 */
static void tty_reconnect_callback(void)
{
    int ret;

    BFLB_LOGI("TTY reconnected, reinitializing AT module...");

    /* Wait for device to stabilize */
    usleep(100000);  /* 100ms */

    /* Resend initialization commands */
    ret = bflbwifi_tty_send_command_sync("AT+CWMODE=1", 2000);
    if (ret != CMD_RESP_OK) {
        BFLB_LOGW("AT+CWMODE=1 failed after reconnect");
    }

    ret = bflbwifi_tty_send_command_sync("AT+CWMODE?", 2000);
    if (ret != CMD_RESP_OK) {
        BFLB_LOGW("AT+CWMODE? failed after reconnect");
    }

    BFLB_LOGI("AT module reinitialized");
}

/* ========== Initialization and Cleanup ========== */

int bflbwifi_init(const char *tty_dev, int baudrate)
{
    int ret;

    if (!tty_dev) {
        BFLB_LOGE("Invalid parameters");
        return E_ERR_INVALID_PARAM;
    }

    if (g_initialized) {
        BFLB_LOGE("Already initialized");
        return -12;
    }

    /* Initialize state manager */
    ret = bflbwifi_state_init();
    if (ret != 0) {
        BFLB_LOGE("Failed to init state manager");
        return -1;
    }

    /* Initialize TTY (first initialize without callback) */
    ret = bflbwifi_tty_init(tty_dev, baudrate, tty_data_callback, NULL);
    if (ret != 0) {
        BFLB_LOGE("Failed to init TTY");
        bflbwifi_state_deinit();
        return E_ERR_TTY_FAIL;
    }

    /* Register disconnect and reconnect callbacks */
    bflbwifi_tty_set_disconnect_callback(tty_disconnect_callback);
    bflbwifi_tty_set_reconnect_callback(tty_reconnect_callback);

    /* Wait for TTY device to stabilize */
    usleep(100000);  /* 100ms */

    /* Send initialization command sequence */
    BFLB_LOGI("Initializing AT module...");

    /* 1. Set to Station mode (AT+CWMODE=1) */
    ret = bflbwifi_tty_send_command_sync("AT+CWMODE=1", 2000);
    if (ret != CMD_RESP_OK) {
        BFLB_LOGW("AT+CWMODE=1 failed, module may already be in correct mode");
    }

    /* 2. Query current WiFi mode */
    ret = bflbwifi_tty_send_command_sync("AT+CWMODE?", 2000);
    if (ret != CMD_RESP_OK) {
        BFLB_LOGW("AT+CWMODE? failed");
    }

    g_initialized = true;
    BFLB_LOGI("BFLB WiFi initialized");

    return 0;
}

int bflbwifi_deinit(void)
{
    if (!g_initialized) {
        return E_ERR_NOT_INIT;
    }

    BFLB_LOGI("Deinitializing BFLB WiFi...");

    /* Clean up TTY */
    bflbwifi_tty_deinit();

    /* Clean up state manager */
    bflbwifi_state_deinit();

    g_initialized = false;

    BFLB_LOGI("BFLB WiFi deinitialized");

    return 0;
}

/* ========== Mode Configuration ========== */

int bflbwifi_set_mode(bflbwifi_mode_t mode)
{
    char cmd[64];
    int ret;

    if (!g_initialized) {
        return E_ERR_NOT_INIT;
    }

    if (mode < WIFI_MODE_IDLE || mode > WIFI_MODE_STA_AP) {
        return E_ERR_INVALID_PARAM;
    }

    /* Send AT+CWMODE command and wait for OK */
    snprintf(cmd, sizeof(cmd), "AT+CWMODE=%d", mode);
    ret = bflbwifi_tty_send_command_sync(cmd, 5000);
    if (ret != CMD_RESP_OK) {
        BFLB_LOGE("Set mode failed");
        return E_ERR_TTY_FAIL;
    }

    /* Update status */
    bflbwifi_state_set_mode(mode);

    BFLB_LOGI("Set mode: %d", mode);

    return 0;
}

int bflbwifi_get_mode(bflbwifi_mode_t *mode)
{
    if (!g_initialized) {
        return E_ERR_NOT_INIT;
    }

    return bflbwifi_state_get_mode(mode);
}

/* ========== Station Mode API ========== */

int bflbwifi_sta_connect(bflbwifi_sta_config_t *config, int timeout_ms)
{
    char cmd[256];
    int ret;
    int default_timeout = 30000;  /* Default 30 seconds */

    if (!g_initialized) {
        return E_ERR_NOT_INIT;
    }

    if (!config) {
        return E_ERR_INVALID_PARAM;
    }

    /* Validate timeout parameter */
    if (timeout_ms < 0) {
        BFLB_LOGE("Invalid timeout: %d", timeout_ms);
        return E_ERR_INVALID_PARAM;
    }

    /* Validate SSID and password length */
    if (strlen(config->ssid) > 32) {
        BFLB_LOGE("SSID too long: %zu > 32", strlen(config->ssid));
        return E_ERR_INVALID_PARAM;
    }

    if (strlen(config->pwd) > 64) {
        BFLB_LOGE("Password too long: %zu > 64", strlen(config->pwd));
        return E_ERR_INVALID_PARAM;
    }

    /* Use default timeout */
    if (timeout_ms == 0) {
        timeout_ms = default_timeout;
    }

    BFLB_LOGI("Connecting to %s...", config->ssid);

    /* Construct AT+CWJAP command */
    snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"", config->ssid, config->pwd);

    /* Send command and wait for OK (5 second timeout) */
    ret = bflbwifi_tty_send_command_sync(cmd, 5000);
    if (ret != CMD_RESP_OK) {
        BFLB_LOGE("Send connect command failed");
        return E_ERR_TTY_FAIL;
    }

    /* Wait for CONNECTED status (wait for +CW:CONNECTED URC) */
    ret = bflbwifi_state_wait_sta_state(WIFI_STATE_CONNECTED, timeout_ms);
    if (ret != 0) {
        BFLB_LOGE("Connect failed: %s", bflbwifi_strerror(ret));
        return ret;
    }

    BFLB_LOGI("Connected to %s", config->ssid);

    /* Actively query connection information (SSID, BSSID, Channel, RSSI) */
    /* Note: Since WiFi side DHCP is disabled, there will be no +CW:GOTIP event */
    /* Need to actively query WiFi connection information */
    ret = bflbwifi_tty_send_command_sync("AT+CWJAP?", 2000);
    if (ret != CMD_RESP_OK) {
        BFLB_LOGI("Failed to query connection info");
    }
    /* Parser will automatically handle +CWJAP response and update status */

    return 0;
}

int bflbwifi_sta_disconnect(void)
{
    int ret;

    if (!g_initialized) {
        return E_ERR_NOT_INIT;
    }

    BFLB_LOGI("Disconnecting...");

    /* Send AT+CWQAP command and wait for OK */
    ret = bflbwifi_tty_send_command_sync("AT+CWQAP", 5000);
    if (ret != CMD_RESP_OK) {
        BFLB_LOGE("Disconnect failed");
        return E_ERR_TTY_FAIL;
    }

    /* Update status */
    bflbwifi_state_set_sta_state(WIFI_STATE_IDLE);

    BFLB_LOGI("Disconnected");

    return 0;
}

int bflbwifi_scan(bflbwifi_ap_info_t *ap_list, int max_count, int *actual_count, int timeout_ms)
{
    int ret;
    int default_timeout = 30000;  /* Default 30 seconds */

    if (!g_initialized) {
        return E_ERR_NOT_INIT;
    }

    if (!ap_list || max_count <= 0 || !actual_count) {
        return E_ERR_INVALID_PARAM;
    }

    /* Validate timeout parameter */
    if (timeout_ms < 0) {
        BFLB_LOGE("Invalid timeout: %d", timeout_ms);
        return E_ERR_INVALID_PARAM;
    }

    /* Use default timeout */
    if (timeout_ms == 0) {
        timeout_ms = default_timeout;
    }

    BFLB_LOGI("Scanning APs...");

    /* Reset scan results */
    bflbwifi_state_reset_scan();

    /* Send AT+CWLAP command and wait for OK (immediate response, 5 second timeout) */
    ret = bflbwifi_tty_send_command_sync("AT+CWLAP", 5000);
    if (ret != CMD_RESP_OK) {
        BFLB_LOGE("Scan command failed");
        return E_ERR_TTY_FAIL;
    }

    /* Wait for scan completion (wait for +CW:SCAN_DONE URC) */
    ret = bflbwifi_state_wait_scan_complete(timeout_ms);
    if (ret != 0) {
        BFLB_LOGE("Scan timeout");
        return ret;
    }

    /* Get scan results */
    ret = bflbwifi_state_get_scan_results(ap_list, max_count, actual_count);
    if (ret == 0) {
        BFLB_LOGI("Scan complete: found %d APs", *actual_count);
    } else if (ret == -11) {
        BFLB_LOGI("Scan complete: found %d APs (buffer too small)", *actual_count);
    }

    return ret;
}

int bflbwifi_set_station_dhcpc(bool enable)
{
    char cmd[64];
    int ret;

    if (!g_initialized) {
        return E_ERR_NOT_INIT;
    }

    /* Send AT+CWDHCP command and wait for OK */
    /* operate: 1=enable, 0=disable */
    /* mode: 1=Station (bit0) */
    snprintf(cmd, sizeof(cmd), "AT+CWDHCP=%d,1", enable ? 1 : 0);
    ret = bflbwifi_tty_send_command_sync(cmd, 5000);
    if (ret != CMD_RESP_OK) {
        BFLB_LOGE("Set DHCP failed");
        return E_ERR_TTY_FAIL;
    }

    BFLB_LOGI("DHCP %s", enable ? "enabled" : "disabled");

    return 0;
}

int bflbwifi_set_static_ip(const char *ip, const char *gateway, const char *netmask)
{
    char cmd[256];
    int ret;

    if (!g_initialized) {
        return E_ERR_NOT_INIT;
    }

    if (!ip || !gateway || !netmask) {
        return E_ERR_INVALID_PARAM;
    }

    /* Send AT+CIPSTA command and wait for OK */
    snprintf(cmd, sizeof(cmd), "AT+CIPSTA=\"%s\",\"%s\",\"%s\"", ip, gateway, netmask);
    ret = bflbwifi_tty_send_command_sync(cmd, 5000);
    if (ret != CMD_RESP_OK) {
        BFLB_LOGE("Set static IP failed");
        return E_ERR_TTY_FAIL;
    }

    BFLB_LOGI("Static IP set: %s", ip);

    return 0;
}

int bflbwifi_sta_get_state(bflbwifi_sta_state_t *state)
{
    if (!g_initialized) {
        return E_ERR_NOT_INIT;
    }

    if (!state) {
        return E_ERR_INVALID_PARAM;
    }

    return bflbwifi_state_get_sta_state(state);
}

int bflbwifi_get_sta_connection_info(bflbwifi_sta_connection_info_t *info)
{
    if (!g_initialized) {
        return E_ERR_NOT_INIT;
    }

    if (!info) {
        return E_ERR_INVALID_PARAM;
    }

    return bflbwifi_state_get_sta_info(info);
}

/* ========== AP Mode API ========== */

int bflbwifi_ap_config(const bflbwifi_ap_config_t *config)
{
    char cmd[256];
    int ret;

    if (!g_initialized) {
        return E_ERR_NOT_INIT;
    }

    if (!config) {
        return E_ERR_INVALID_PARAM;
    }

    /* Send AT+CWSAP command */
    snprintf(cmd, sizeof(cmd), "AT+CWSAP=\"%s\",\"%s\",%d,%d,%d,%d",
             config->ssid, config->pwd, config->channel,
             config->enc, config->max_conn, config->ssid_hidden ? 1 : 0);

    ret = bflbwifi_tty_send_command(cmd);
    if (ret < 0) {
        return E_ERR_TTY_FAIL;
    }

    BFLB_LOGI("AP configured: %s", config->ssid);

    return 0;
}

int bflbwifi_ap_start(bflbwifi_ap_config_t *config, int timeout_ms)
{
    int ret;
    int default_timeout = 30000;  /* Default 30 seconds */
    char cmd[512];

    if (!g_initialized) {
        return E_ERR_NOT_INIT;
    }

    if (!config) {
        return E_ERR_INVALID_PARAM;
    }

    /* Validate timeout parameter */
    if (timeout_ms < 0) {
        BFLB_LOGE("Invalid timeout: %d", timeout_ms);
        return E_ERR_INVALID_PARAM;
    }

    /* Use default timeout */
    if (timeout_ms == 0) {
        timeout_ms = default_timeout;
    }

    BFLB_LOGI("Starting SoftAP: %s...", config->ssid);

    /* Construct AT+CWSAP command */
    snprintf(cmd, sizeof(cmd),
             "AT+CWSAP=\"%s\",\"%s\",%d,%u",
             config->ssid, config->pwd, config->channel, config->enc);

    /* Send configuration command */
    ret = bflbwifi_tty_send_command_sync(cmd, 5000);
    if (ret != CMD_RESP_OK) {
        BFLB_LOGE("Failed to configure AP");
        return E_ERR_TTY_FAIL;
    }

    /* Set max connections and hidden SSID */
    /* Note: Simplified here, may need adjustment based on AT command format */
    if (config->max_conn > 0 || config->ssid_hidden) {
        /* TODO: Set max connections and hidden SSID based on specific AT commands */
    }

    /* AP mode is started through set_mode */
    ret = bflbwifi_set_mode(WIFI_MODE_AP);
    if (ret != 0) {
        BFLB_LOGE("Failed to set AP mode");
        return ret;
    }

    /* Wait for AP to start */
    /* Status check logic can be added here */

    BFLB_LOGI("SoftAP started: %s", config->ssid);

    return 0;
}

int bflbwifi_ap_stop(void)
{
    /* Switch to IDLE mode to stop AP */
    return bflbwifi_set_mode(WIFI_MODE_IDLE);
}

int bflbwifi_ap_get_sta_list(bflbwifi_sta_info_t *sta_list, int max_count, int *actual_count)
{
    /* TODO: Implement station list query */
    /* Need to query through AT+CWLIF command */
    if (!g_initialized) {
        return E_ERR_NOT_INIT;
    }

    if (!sta_list || max_count <= 0 || !actual_count) {
        return E_ERR_INVALID_PARAM;
    }

    *actual_count = 0;
    return -10;  /* E_ERR_NOT_SUPPORTED */
}

int bflbwifi_ap_disconnect_sta(const char *mac)
{
    char cmd[64];
    int ret;

    if (!g_initialized) {
        return E_ERR_NOT_INIT;
    }

    if (!mac) {
        return E_ERR_INVALID_PARAM;
    }

    /* Send AT+CWQIF command */
    snprintf(cmd, sizeof(cmd), "AT+CWQIF=\"%s\"", mac);

    ret = bflbwifi_tty_send_command(cmd);
    if (ret < 0) {
        return E_ERR_TTY_FAIL;
    }

    BFLB_LOGI("Station disconnected: %s", mac);

    return 0;
}

int bflbwifi_ap_set_dhcp_range(const char *start_ip, const char *end_ip, int lease_min)
{
    char cmd[256];
    int ret;

    if (!g_initialized) {
        return E_ERR_NOT_INIT;
    }

    if (!start_ip || !end_ip) {
        return E_ERR_INVALID_PARAM;
    }

    /* Send AT+CWDHCPS command */
    snprintf(cmd, sizeof(cmd), "AT+CWDHCPS=1,%d,\"%s\",\"%s\"", lease_min, start_ip, end_ip);

    ret = bflbwifi_tty_send_command(cmd);
    if (ret < 0) {
        return E_ERR_TTY_FAIL;
    }

    BFLB_LOGI("AP DHCP range set: %s - %s", start_ip, end_ip);

    return 0;
}

/* ========== Utility Functions ========== */

int bflbwifi_get_version(char *buf, size_t len)
{
    int ret;

    if (!g_initialized) {
        return E_ERR_NOT_INIT;
    }

    if (!buf || len == 0) {
        return E_ERR_INVALID_PARAM;
    }

    BFLB_LOGI("Getting version...");

    /* Send AT+GMR command */
    ret = bflbwifi_tty_send_command_sync("AT+GMR", 2000);
    if (ret != CMD_RESP_OK) {
        BFLB_LOGE("Get version failed");
        return E_ERR_TTY_FAIL;
    }

    /* Get version from TTY last response buffer */
    bflbwifi_tty_get_last_response(buf, len);

    BFLB_LOGI("Version: %s", buf);

    return 0;
}

int bflbwifi_restart(void)
{
    int ret;

    if (!g_initialized) {
        return E_ERR_NOT_INIT;
    }

    BFLB_LOGI("Restarting module...");

    /* Send AT+RST command */
    ret = bflbwifi_tty_send_command("AT+RST");
    if (ret < 0) {
        return E_ERR_TTY_FAIL;
    }

    /* Wait for module to restart */
    sleep(2);

    BFLB_LOGI("Module restarted");

    return 0;
}

/* ========== OTA Functionality ========== */

/* OTA progress flag (atomic for thread safety) */
static volatile bool g_ota_in_progress = false;
static time_t g_ota_start_time = 0;

/**
 * @brief Send command to OTA TTY (direct access)
 */
static int ota_send_cmd(int fd, const char *cmd)
{
    char cmd_buf[128];
    int len;

    snprintf(cmd_buf, sizeof(cmd_buf), "%s\r\n", cmd);
    len = strlen(cmd_buf);

    BFLB_LOGD("OTA TX: %s", cmd_buf);

    if (write(fd, cmd_buf, len) != len) {
        BFLB_LOGE("Failed to send OTA command: %s", strerror(errno));
        return -1;
    }

    return 0;
}

/**
 * @brief Wait for response on OTA TTY (mimics easyat.c's wait_response)
 */
static int ota_wait_response(int fd, const char *target, int timeout_sec)
{
    char buffer[2048];
    int buffer_pos = 0;
    time_t start_time;
    fd_set read_fds;
    struct timeval tv;
    int ret;

    start_time = time(NULL);

    while (time(NULL) - start_time < timeout_sec) {
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);

        tv.tv_sec = 1;  /* Wait for 1 second each time */
        tv.tv_usec = 0;

        ret = select(fd + 1, &read_fds, NULL, NULL, &tv);

        if (ret < 0) {
            if (errno == EINTR) {
                continue;  /* Interrupted by signal */
            }
            BFLB_LOGE("OTA select failed: %s", strerror(errno));
            return -1;
        } else if (ret == 0) {
            /* Timeout, continue loop */
            continue;
        }

        if (FD_ISSET(fd, &read_fds)) {
            int bytes_read = read(fd, buffer + buffer_pos, sizeof(buffer) - buffer_pos - 1);
            if (bytes_read > 0) {
                buffer_pos += bytes_read;
                buffer[buffer_pos] = '\0';

                /* Display received data */
                BFLB_LOGD("OTA RX: %.*s", bytes_read, buffer + buffer_pos - bytes_read);

                /* Check if target string received */
                if (strstr(buffer, target) != NULL) {
                    return 1;  /* Successfully found target string */
                }

                /* Prevent buffer overflow */
                if (buffer_pos > (int)sizeof(buffer) / 2) {
                    memmove(buffer, buffer + buffer_pos / 2, buffer_pos / 2);
                    buffer_pos /= 2;
                }
            } else if (bytes_read < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    continue;
                }
                BFLB_LOGE("OTA read failed: %s", strerror(errno));
                return -1;
            } else {
                /* bytes_read == 0, connection closed */
                BFLB_LOGE("OTA TTY connection closed");
                return -1;
            }
        }
    }

    BFLB_LOGE("OTA timeout waiting for: %s", target);
    return 0;  /* Timeout, target string not found */
}

/**
 * @brief Send binary data to OTA TTY
 */
static int ota_send_binary(int fd, const void *data, int size)
{
    int bytes_sent = 0;

    while (bytes_sent < size) {
        int result = write(fd, (const char *)data + bytes_sent, size - bytes_sent);
        if (result < 0) {
            BFLB_LOGE("OTA failed to send binary data: %s", strerror(errno));
            return -1;
        }
        bytes_sent += result;
    }

    BFLB_LOGD("OTA sent %d bytes raw data", bytes_sent);
    return bytes_sent;
}

/* Internal access for daemon */
bool bflbwifi_is_ota_in_progress(void)
{
    return g_ota_in_progress;
}

/* Internal access for daemon auto-recovery */
time_t bflbwifi_ota_get_start_time(void)
{
    return g_ota_start_time;
}

/* Internal access for daemon auto-recovery */
void bflbwifi_ota_reset(void)
{
    g_ota_in_progress = false;
    g_ota_start_time = 0;
    BFLB_LOGW("OTA status reset (auto-recovery)");
}

/**
 * @brief OTA firmware upgrade
 * @param filepath Path to OTA binary file
 * @return 0 for success, negative error code for failure
 *
 * @note This function is thread-safe and blocks during OTA
 * @note OTA must not be interrupted by other operations
 * @note Device will reboot after successful OTA
 */
int bflbwifi_ota_upgrade(const char *filepath)
{
    FILE *fp;
    unsigned char chunk[512];
    char cmd[128];
    int ret;
    long file_size;
    int bytes_sent;
    int chunk_size;
    size_t read_size;
    int ota_fd;

    if (!g_initialized) {
        return E_ERR_NOT_INIT;
    }

    if (!filepath) {
        return E_ERR_INVALID_PARAM;
    }

    /* Check if OTA already in progress */
    if (g_ota_in_progress) {
        BFLB_LOGE("OTA already in progress");
        return E_ERR_BUSY;
    }

    /* Open OTA file */
    fp = fopen(filepath, "rb");
    if (!fp) {
        BFLB_LOGE("Failed to open OTA file: %s", filepath);
        return E_ERR_INVALID_PARAM;
    }

    /* Get file size */
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (file_size <= 0) {
        BFLB_LOGE("Invalid OTA file size: %ld", file_size);
        fclose(fp);
        return E_ERR_INVALID_PARAM;
    }

    /* Check file size limit (16MB) */
    if (file_size > 16 * 1024 * 1024) {
        BFLB_LOGE("OTA file too large: %ld bytes (max 16MB)", file_size);
        fclose(fp);
        return E_ERR_INVALID_PARAM;
    }

    BFLB_LOGI("Starting OTA upgrade: %s (%ld bytes)", filepath, file_size);

    /* Set OTA flag (atomic operation) */
    __sync_bool_compare_and_swap(&g_ota_in_progress, false, true);
    g_ota_start_time = time(NULL);  /* Record start time */

    /* Suspend receive thread to get exclusive TTY access */
    if (bflbwifi_tty_suspend_recv_thread() != 0) {
        BFLB_LOGE("Failed to suspend receive thread");
        goto error;
    }

    /* Get TTY fd for direct access */
    ota_fd = bflbwifi_tty_get_fd();
    if (ota_fd < 0) {
        BFLB_LOGE("Invalid TTY fd");
        goto resume_error;
    }

    BFLB_LOGI("OTA mode: Direct TTY access (fd=%d)", ota_fd);

    /* Step 1: Query OTA status */
    if (ota_send_cmd(ota_fd, "AT+OTASTART?") != 0) {
        BFLB_LOGE("Failed to send OTASTART? command");
        goto resume_error;
    }

    ret = ota_wait_response(ota_fd, "OK", 2);
    if (ret != 1) {
        BFLB_LOGE("OTASTART? failed: no OK response");
        goto resume_error;
    }

    /* Step 2: Start OTA mode */
    if (ota_send_cmd(ota_fd, "AT+OTASTART=1") != 0) {
        BFLB_LOGE("Failed to send OTASTART=1 command");
        goto resume_error;
    }

    ret = ota_wait_response(ota_fd, "OK", 2);
    if (ret != 1) {
        BFLB_LOGE("OTASTART=1 failed: no OK response");
        goto resume_error;
    }

    BFLB_LOGI("OTA mode started, sending data...");

    /* Step 3: Send data chunks */
    bytes_sent = 0;
    while (bytes_sent < file_size) {
        /* Calculate chunk size (max 512 bytes) */
        int remaining = (int)(file_size - bytes_sent);
        chunk_size = (remaining > 512) ? 512 : remaining;

        /* Send OTASEND command */
        snprintf(cmd, sizeof(cmd), "AT+OTASEND=%d", chunk_size);
        if (ota_send_cmd(ota_fd, cmd) != 0) {
            BFLB_LOGE("Failed to send OTASEND command at offset %d", bytes_sent);
            goto resume_error;
        }

        ret = ota_wait_response(ota_fd, "OK", 2);
        if (ret != 1) {
            BFLB_LOGE("OTASEND command failed at offset %d: no OK response", bytes_sent);
            goto resume_error;
        }

        /* Read chunk from file */
        read_size = fread(chunk, 1, chunk_size, fp);
        if ((int)read_size != chunk_size) {
            BFLB_LOGE("Failed to read file at offset %d", bytes_sent);
            goto resume_error;
        }

        /* Send raw binary data */
        if (ota_send_binary(ota_fd, chunk, chunk_size) != chunk_size) {
            BFLB_LOGE("Failed to send binary data at offset %d", bytes_sent);
            goto resume_error;
        }

        /* Wait for SEND OK */
        ret = ota_wait_response(ota_fd, "SEND OK", 3);
        if (ret != 1) {
            BFLB_LOGE("No SEND OK confirmation at offset %d", bytes_sent);
            goto resume_error;
        }

        bytes_sent += chunk_size;

        /* Print progress after each chunk */
        BFLB_LOGI("OTA: %d%% (%d/%ld bytes)", (int)(bytes_sent * 100 / file_size), bytes_sent, file_size);
    }

    /* Step 4: Finish OTA */
    if (ota_send_cmd(ota_fd, "AT+OTAFIN") != 0) {
        BFLB_LOGE("Failed to send OTAFIN command");
        goto resume_error;
    }

    ret = ota_wait_response(ota_fd, "OK", 2);
    if (ret != 1) {
        BFLB_LOGE("OTAFIN failed: no OK response");
        goto resume_error;
    }

    /* Success - resume receive thread before returning */
    bflbwifi_tty_resume_recv_thread();
    fclose(fp);
    g_ota_in_progress = false;
    g_ota_start_time = 0;
    BFLB_LOGI("OTA upgrade completed successfully");

    return 0;

resume_error:
    /* Resume receive thread on error */
    bflbwifi_tty_resume_recv_thread();

error:
    fclose(fp);
    g_ota_in_progress = false;
    g_ota_start_time = 0;
    BFLB_LOGE("OTA upgrade failed");

    return E_ERR_TTY_FAIL;
}
