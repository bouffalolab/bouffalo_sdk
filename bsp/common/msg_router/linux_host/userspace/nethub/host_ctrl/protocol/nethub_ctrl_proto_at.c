#include "nethub_ctrl_proto.h"

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "nethub_ctrl_link.h"
#include "nethub_log.h"
#include "nethub_netif.h"
#include "nethub_wifi_parser_at.h"

#define AT_LINE_MAX       512
#define AT_RESPONSE_MAX   4096
#define AT_DEFAULT_MS     5000
#define AT_CONNECT_MS     30000
#define AT_SCAN_MS        30000
#define AT_OTA_DEFAULT_MS 300000
#define AT_OTA_CHUNK_MS   10000
#define AT_OTA_FINISH_MS  15000
#define AT_OTA_REBOOT_MS  10000
#define AT_OTA_CHUNK_SIZE 1024
#define AT_OTA_HEADER_LEN 512
#define AT_OTA_TYPE_OFF   16
#define AT_OTA_LEN_OFF    20

typedef enum {
    AT_OP_NONE = 0,
    AT_OP_COMMAND,
    AT_OP_CONNECT,
    AT_OP_OTA_PROMPT,
    AT_OP_OTA_SEND,
} at_op_type_t;

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    pthread_mutex_t op_mutex;
    int initialized;
    int link_synced;

    char line[AT_LINE_MAX];
    size_t line_len;

    at_op_type_t op_type;
    int op_done;
    int op_result;
    char response[AT_RESPONSE_MAX];
    size_t response_len;

    int scan_done;
    int scan_count;
    nethub_wifi_ap_info_t scan_cache[NETHUB_WIFI_SCAN_MAX_APS];

    int last_wifi_reason;
    int last_cwjap_error;
    nethub_wifi_status_t status;
} at_ctx_t;

static at_ctx_t g_at = {
    .lock = PTHREAD_MUTEX_INITIALIZER,
    .cond = PTHREAD_COND_INITIALIZER,
    .op_mutex = PTHREAD_MUTEX_INITIALIZER,
};

static void make_deadline(struct timespec *ts, int timeout_ms)
{
    clock_gettime(CLOCK_REALTIME, ts);
    ts->tv_sec += timeout_ms / 1000;
    ts->tv_nsec += (long)(timeout_ms % 1000) * 1000000L;
    if (ts->tv_nsec >= 1000000000L) {
        ts->tv_sec++;
        ts->tv_nsec -= 1000000000L;
    }
}

static int map_cwjap_error(int error_code)
{
    switch (error_code) {
        case 1:
            return NETHUB_WIFI_ERR_TIMEOUT;
        case 2:
            return NETHUB_WIFI_ERR_WRONG_PASSWORD;
        case 3:
            return NETHUB_WIFI_ERR_NO_AP_FOUND;
        case 4:
        default:
            return NETHUB_WIFI_ERR_CONNECT_FAILED;
    }
}

static int map_wifi_reason(int reason_code)
{
    switch (reason_code) {
        case 8:
        case 17:
        case 18:
            return NETHUB_WIFI_ERR_WRONG_PASSWORD;
        case 12:
            return NETHUB_WIFI_ERR_NO_AP_FOUND;
        default:
            return NETHUB_WIFI_ERR_TIMEOUT;
    }
}

static int append_response_locked(const char *line)
{
    size_t len;

    if (!line || g_at.op_type == AT_OP_NONE) {
        return 0;
    }

    len = strlen(line);
    if (g_at.response_len + len + 1u >= sizeof(g_at.response)) {
        return -1;
    }

    memcpy(g_at.response + g_at.response_len, line, len);
    g_at.response_len += len;
    g_at.response[g_at.response_len++] = '\n';
    g_at.response[g_at.response_len] = '\0';
    return 0;
}

static void set_terminal_locked(int result)
{
    if (g_at.op_type != AT_OP_NONE && !g_at.op_done) {
        g_at.op_result = result;
        g_at.op_done = 1;
    }
}

static void clear_station_locked(void)
{
    g_at.status.ssid[0] = '\0';
    g_at.status.bssid[0] = '\0';
    g_at.status.ip[0] = '\0';
    g_at.status.gateway[0] = '\0';
    g_at.status.netmask[0] = '\0';
    g_at.status.dns[0] = '\0';
    g_at.status.channel = 0;
    g_at.status.rssi = 0;
}

static void handle_line_locked(const char *line)
{
    nethub_wifi_ap_info_t ap;
    int reason;

    if (!line || line[0] == '\0') {
        return;
    }

    NETHUB_LOGD("AT RX: %s", line);
    append_response_locked(line);

    if (strcmp(line, "+CW:CONNECTING") == 0) {
        g_at.status.sta_state = NETHUB_WIFI_STA_CONNECTING;
    } else if (strcmp(line, "+CW:CONNECTED") == 0) {
        g_at.status.sta_state = NETHUB_WIFI_STA_CONNECTED;
    } else if (strncmp(line, "+CW:GOTIP", 9) == 0) {
        if (nethub_at_parse_gotip(line, &g_at.status) == 0 &&
            g_at.status.ip[0] != '\0' &&
            g_at.status.gateway[0] != '\0' &&
            g_at.status.netmask[0] != '\0') {
            nethub_netif_config_from_env(g_at.status.ip,
                                         g_at.status.netmask,
                                         g_at.status.gateway,
                                         g_at.status.dns);
        }
    } else if (strcmp(line, "+CW:DISCONNECTED") == 0) {
        g_at.status.sta_state = NETHUB_WIFI_STA_DISCONNECTED;
        clear_station_locked();
    } else if (strncmp(line, "+CW:ERROR,", 10) == 0) {
        g_at.last_wifi_reason = nethub_at_parse_wifi_error(line);
        g_at.status.sta_state = NETHUB_WIFI_STA_ERROR;
        if (g_at.op_type == AT_OP_CONNECT) {
            set_terminal_locked(map_wifi_reason(g_at.last_wifi_reason));
        }
    } else if (strcmp(line, "+CW:SCAN_DONE") == 0) {
        g_at.scan_done = 1;
    } else if (nethub_at_parse_cwlap(line, &ap) == 0) {
        if (g_at.scan_count < NETHUB_WIFI_SCAN_MAX_APS) {
            g_at.scan_cache[g_at.scan_count++] = ap;
        }
    } else if (nethub_at_parse_cwjap_error(line, &reason) == 0) {
        g_at.last_cwjap_error = reason;
        if (g_at.op_type == AT_OP_CONNECT) {
            g_at.status.sta_state = NETHUB_WIFI_STA_ERROR;
            set_terminal_locked(map_cwjap_error(reason));
        } else {
            g_at.status.sta_state = NETHUB_WIFI_STA_DISCONNECTED;
            clear_station_locked();
        }
    } else if (nethub_at_parse_cwjap(line, &g_at.status) == 0) {
        if (g_at.op_type == AT_OP_CONNECT) {
            set_terminal_locked(NETHUB_WIFI_OK);
        }
    } else if (strcmp(line, "OK") == 0 && g_at.op_type == AT_OP_COMMAND) {
        set_terminal_locked(NETHUB_WIFI_OK);
    } else if (strcmp(line, "SEND OK") == 0 && g_at.op_type == AT_OP_OTA_SEND) {
        set_terminal_locked(NETHUB_WIFI_OK);
    } else if (strcmp(line, "ERROR") == 0 || strcmp(line, "FAIL") == 0 || strcmp(line, "SEND FAIL") == 0) {
        set_terminal_locked(NETHUB_WIFI_ERR_PROTOCOL);
    }

    if (g_at.op_type == AT_OP_CONNECT &&
        (g_at.status.sta_state == NETHUB_WIFI_STA_CONNECTED ||
         g_at.status.sta_state == NETHUB_WIFI_STA_GOT_IP)) {
        set_terminal_locked(NETHUB_WIFI_OK);
    }

    pthread_cond_broadcast(&g_at.cond);
}

static void at_rx_cb(const uint8_t *data, size_t len, void *user_data)
{
    size_t i;

    (void)user_data;
    pthread_mutex_lock(&g_at.lock);
    for (i = 0; i < len; i++) {
        uint8_t ch = data[i];

        if (ch == '\r') {
            continue;
        }

        if (ch == '\n') {
            if (g_at.line_len > 0) {
                g_at.line[g_at.line_len] = '\0';
                handle_line_locked(g_at.line);
                g_at.line_len = 0;
            }
            continue;
        }

        if (ch == '>' && g_at.op_type == AT_OP_OTA_PROMPT) {
            g_at.line_len = 0;
            set_terminal_locked(NETHUB_WIFI_OK);
            pthread_cond_broadcast(&g_at.cond);
            continue;
        }

        if (g_at.line_len + 1u < sizeof(g_at.line)) {
            g_at.line[g_at.line_len++] = (char)ch;
        } else {
            g_at.line_len = 0;
        }
    }
    pthread_mutex_unlock(&g_at.lock);
}

static void at_link_event_cb(nethub_wifi_link_state_t state, void *user_data)
{
    (void)user_data;

    pthread_mutex_lock(&g_at.lock);
    g_at.status.link_state = state;
    if (state == NETHUB_WIFI_LINK_DOWN) {
        g_at.link_synced = 0;
        g_at.line_len = 0;
        if (g_at.op_type != AT_OP_NONE) {
            set_terminal_locked(NETHUB_WIFI_ERR_LINK_DOWN);
        }
    }
    pthread_cond_broadcast(&g_at.cond);
    pthread_mutex_unlock(&g_at.lock);
}

static int send_line(const char *cmd)
{
    char wire[512];
    int n;

    if (!cmd || cmd[0] == '\0') {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    n = snprintf(wire, sizeof(wire), "%s\r\n", cmd);
    if (n <= 0 || (size_t)n >= sizeof(wire)) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    NETHUB_LOGD("AT TX: %s", cmd);
    return nethub_ctrl_link_send(wire, (size_t)n);
}

static void op_begin_locked(at_op_type_t type)
{
    g_at.op_type = type;
    g_at.op_done = 0;
    g_at.op_result = NETHUB_WIFI_ERR_TIMEOUT;
    g_at.response_len = 0;
    g_at.response[0] = '\0';
}

static void op_end_locked(void)
{
    g_at.op_type = AT_OP_NONE;
    g_at.op_done = 0;
}

static int send_cmd_wait_locked(const char *cmd, int timeout_ms, char *response, size_t response_len)
{
    struct timespec deadline;
    int ret;

    if (timeout_ms <= 0) {
        timeout_ms = AT_DEFAULT_MS;
    }

    pthread_mutex_lock(&g_at.lock);
    op_begin_locked(AT_OP_COMMAND);
    pthread_mutex_unlock(&g_at.lock);

    ret = send_line(cmd);
    if (ret != 0) {
        pthread_mutex_lock(&g_at.lock);
        op_end_locked();
        pthread_mutex_unlock(&g_at.lock);
        if (ret == NETHUB_WIFI_ERR_LINK_DOWN) {
            pthread_mutex_lock(&g_at.lock);
            g_at.link_synced = 0;
            g_at.status.link_state = NETHUB_WIFI_LINK_DOWN;
            pthread_mutex_unlock(&g_at.lock);
        }
        return ret;
    }

    make_deadline(&deadline, timeout_ms);
    pthread_mutex_lock(&g_at.lock);
    while (!g_at.op_done) {
        ret = pthread_cond_timedwait(&g_at.cond, &g_at.lock, &deadline);
        if (ret == ETIMEDOUT) {
            g_at.op_result = NETHUB_WIFI_ERR_TIMEOUT;
            break;
        }
    }

    ret = g_at.op_result;
    if (response && response_len > 0) {
        snprintf(response, response_len, "%s", g_at.response);
    }
    op_end_locked();
    pthread_mutex_unlock(&g_at.lock);
    if (ret == NETHUB_WIFI_OK && strcmp(cmd, "AT") == 0) {
        pthread_mutex_lock(&g_at.lock);
        g_at.link_synced = 1;
        g_at.status.link_state = NETHUB_WIFI_LINK_UP;
        pthread_mutex_unlock(&g_at.lock);
    }
    return ret;
}

static int ensure_link_synced_locked(void)
{
    int need_sync;
    int ret = NETHUB_WIFI_OK;
    int i;

    pthread_mutex_lock(&g_at.lock);
    need_sync = !g_at.link_synced;
    pthread_mutex_unlock(&g_at.lock);
    if (!need_sync) {
        return NETHUB_WIFI_OK;
    }

    for (i = 0; i < 2; i++) {
        ret = send_cmd_wait_locked("AT", 1500, NULL, 0);
        if (ret == NETHUB_WIFI_OK) {
            return NETHUB_WIFI_OK;
        }
        usleep(100 * 1000);
    }

    return ret;
}

static int send_cmd_wait(const char *cmd, int timeout_ms, char *response, size_t response_len)
{
    int ret;

    pthread_mutex_lock(&g_at.op_mutex);
    if (cmd && strcmp(cmd, "AT") != 0) {
        ret = ensure_link_synced_locked();
        if (ret != NETHUB_WIFI_OK) {
            pthread_mutex_unlock(&g_at.op_mutex);
            return ret;
        }
    }
    ret = send_cmd_wait_locked(cmd, timeout_ms, response, response_len);
    pthread_mutex_unlock(&g_at.op_mutex);
    return ret;
}

static int wait_op_done_until_locked(const struct timespec *deadline)
{
    int ret;

    while (!g_at.op_done) {
        ret = pthread_cond_timedwait(&g_at.cond, &g_at.lock, deadline);
        if (ret == ETIMEDOUT) {
            g_at.op_result = NETHUB_WIFI_ERR_TIMEOUT;
            break;
        }
    }

    return g_at.op_result;
}

static int deadline_remaining_ms(const struct timespec *deadline)
{
    struct timespec now;
    int64_t ms;

    clock_gettime(CLOCK_REALTIME, &now);
    ms = (int64_t)(deadline->tv_sec - now.tv_sec) * 1000;
    ms += (int64_t)(deadline->tv_nsec - now.tv_nsec) / 1000000;
    if (ms <= 0) {
        return 0;
    }
    if (ms > INT32_MAX) {
        return INT32_MAX;
    }
    return (int)ms;
}

static int min_positive_timeout(int a, int b)
{
    if (a <= 0 || b <= 0) {
        return 0;
    }
    return a < b ? a : b;
}

static uint32_t ota_get_le32(const uint8_t *p)
{
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static int ota_read_exact(int fd, uint8_t *buf, size_t len)
{
    size_t offset = 0;

    while (offset < len) {
        ssize_t nread = read(fd, buf + offset, len - offset);

        if (nread < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (nread == 0) {
            return -1;
        }
        offset += (size_t)nread;
    }

    return 0;
}

static int ota_validate_file(int fd, off_t file_size)
{
    uint8_t header[AT_OTA_HEADER_LEN];
    char magic[17];
    char type[5];
    uint32_t body_len;
    uint64_t expected_size;

    if (file_size <= AT_OTA_HEADER_LEN) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    if (lseek(fd, 0, SEEK_SET) < 0 ||
        ota_read_exact(fd, header, sizeof(header)) != 0 ||
        lseek(fd, 0, SEEK_SET) < 0) {
        return NETHUB_WIFI_ERR;
    }

    memcpy(magic, header, 16);
    magic[16] = '\0';
    memcpy(type, header + AT_OTA_TYPE_OFF, 4);
    type[4] = '\0';
    body_len = ota_get_le32(header + AT_OTA_LEN_OFF);
    expected_size = (uint64_t)AT_OTA_HEADER_LEN + body_len;

    if (strstr(magic, "BL60X_OTA") == NULL ||
        (strstr(type, "XZ") == NULL && strstr(type, "RAW") == NULL) ||
        expected_size > (uint64_t)file_size) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    if (expected_size != (uint64_t)file_size) {
        NETHUB_LOGW("OTA image size mismatch: header=%llu file=%llu",
                    (unsigned long long)expected_size,
                    (unsigned long long)file_size);
    }

    return NETHUB_WIFI_OK;
}

static void mark_link_down_after_send_error(int ret)
{
    if (ret == NETHUB_WIFI_ERR_LINK_DOWN) {
        pthread_mutex_lock(&g_at.lock);
        g_at.link_synced = 0;
        g_at.status.link_state = NETHUB_WIFI_LINK_DOWN;
        pthread_mutex_unlock(&g_at.lock);
    }
}

static int ota_wait_op(at_op_type_t type, const char *cmd, int timeout_ms)
{
    struct timespec deadline;
    int ret;

    pthread_mutex_lock(&g_at.lock);
    op_begin_locked(type);
    pthread_mutex_unlock(&g_at.lock);

    ret = send_line(cmd);
    if (ret != 0) {
        pthread_mutex_lock(&g_at.lock);
        op_end_locked();
        pthread_mutex_unlock(&g_at.lock);
        mark_link_down_after_send_error(ret);
        return ret;
    }

    make_deadline(&deadline, timeout_ms);
    pthread_mutex_lock(&g_at.lock);
    ret = wait_op_done_until_locked(&deadline);
    op_end_locked();
    pthread_mutex_unlock(&g_at.lock);
    return ret;
}

static int ota_send_chunk_locked(const uint8_t *data, size_t len, int timeout_ms)
{
    char cmd[64];
    struct timespec deadline;
    int ret;

    if (!data || len == 0 || len > AT_OTA_CHUNK_SIZE || timeout_ms <= 0) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    snprintf(cmd, sizeof(cmd), "AT+OTASEND=%u", (unsigned int)len);
    ret = ota_wait_op(AT_OP_OTA_PROMPT, cmd, timeout_ms);
    if (ret != NETHUB_WIFI_OK) {
        return ret;
    }

    pthread_mutex_lock(&g_at.lock);
    op_begin_locked(AT_OP_OTA_SEND);
    pthread_mutex_unlock(&g_at.lock);

    ret = nethub_ctrl_link_send(data, len);
    if (ret != 0) {
        pthread_mutex_lock(&g_at.lock);
        op_end_locked();
        pthread_mutex_unlock(&g_at.lock);
        mark_link_down_after_send_error(ret);
        return ret;
    }

    make_deadline(&deadline, timeout_ms);
    pthread_mutex_lock(&g_at.lock);
    ret = wait_op_done_until_locked(&deadline);
    op_end_locked();
    pthread_mutex_unlock(&g_at.lock);
    return ret;
}

static void ota_abort_best_effort_locked(void)
{
    (void)send_cmd_wait_locked("AT+OTASTART=0", 3000, NULL, 0);
}

static int ota_finish_locked(int timeout_ms)
{
    struct timespec deadline;
    int ret;

    pthread_mutex_lock(&g_at.lock);
    op_begin_locked(AT_OP_COMMAND);
    pthread_mutex_unlock(&g_at.lock);

    ret = send_line("AT+OTAFIN");
    if (ret != 0) {
        pthread_mutex_lock(&g_at.lock);
        op_end_locked();
        pthread_mutex_unlock(&g_at.lock);
        mark_link_down_after_send_error(ret);
        return ret;
    }

    make_deadline(&deadline, timeout_ms);
    pthread_mutex_lock(&g_at.lock);
    ret = wait_op_done_until_locked(&deadline);
    op_end_locked();
    pthread_mutex_unlock(&g_at.lock);

    /* After OTAFIN is accepted, a quick link-down usually means the device has
     * reset into the new image before the final OK reached the host. */
    return ret == NETHUB_WIFI_ERR_LINK_DOWN ? NETHUB_WIFI_OK : ret;
}

static int ota_wait_reboot_locked(int timeout_ms)
{
    struct timespec deadline;
    int ret;

    make_deadline(&deadline, timeout_ms);
    pthread_mutex_lock(&g_at.lock);
    while (g_at.status.link_state != NETHUB_WIFI_LINK_DOWN) {
        ret = pthread_cond_timedwait(&g_at.cond, &g_at.lock, &deadline);
        if (ret == ETIMEDOUT) {
            pthread_mutex_unlock(&g_at.lock);
            return NETHUB_WIFI_ERR_PROTOCOL;
        }
    }
    pthread_mutex_unlock(&g_at.lock);
    return NETHUB_WIFI_OK;
}

static int wait_connect_locked(const struct timespec *deadline)
{
    int ret;

    while (!g_at.op_done) {
        ret = pthread_cond_timedwait(&g_at.cond, &g_at.lock, deadline);
        if (ret == ETIMEDOUT) {
            if (g_at.last_cwjap_error != 0) {
                return map_cwjap_error(g_at.last_cwjap_error);
            }
            return map_wifi_reason(g_at.last_wifi_reason);
        }
    }

    return g_at.op_result;
}

static int send_connect_wait(const char *cmd, int timeout_ms)
{
    struct timespec deadline;
    int ret;

    if (timeout_ms <= 0) {
        timeout_ms = AT_CONNECT_MS;
    }

    pthread_mutex_lock(&g_at.op_mutex);
    ret = ensure_link_synced_locked();
    if (ret != NETHUB_WIFI_OK) {
        pthread_mutex_unlock(&g_at.op_mutex);
        return ret;
    }

    pthread_mutex_lock(&g_at.lock);
    op_begin_locked(AT_OP_CONNECT);
    g_at.last_wifi_reason = 0;
    g_at.last_cwjap_error = 0;
    g_at.status.sta_state = NETHUB_WIFI_STA_CONNECTING;
    pthread_mutex_unlock(&g_at.lock);

    ret = send_line(cmd);
    if (ret != 0) {
        pthread_mutex_lock(&g_at.lock);
        op_end_locked();
        pthread_mutex_unlock(&g_at.lock);
        pthread_mutex_unlock(&g_at.op_mutex);
        return ret;
    }

    make_deadline(&deadline, timeout_ms);
    pthread_mutex_lock(&g_at.lock);
    ret = wait_connect_locked(&deadline);
    op_end_locked();
    pthread_mutex_unlock(&g_at.lock);
    pthread_mutex_unlock(&g_at.op_mutex);
    return ret;
}

static int wait_scan_done(int timeout_ms)
{
    struct timespec deadline;
    int ret;

    if (timeout_ms <= 0) {
        timeout_ms = AT_SCAN_MS;
    }

    make_deadline(&deadline, timeout_ms);
    pthread_mutex_lock(&g_at.lock);
    while (!g_at.scan_done) {
        ret = pthread_cond_timedwait(&g_at.cond, &g_at.lock, &deadline);
        if (ret == ETIMEDOUT) {
            pthread_mutex_unlock(&g_at.lock);
            return NETHUB_WIFI_ERR_TIMEOUT;
        }
    }
    pthread_mutex_unlock(&g_at.lock);
    return 0;
}

static int at_escape_param(const char *src, size_t src_max_len, char *dst, size_t dst_size)
{
    size_t out = 0;
    size_t src_len;
    size_t i;

    if (!src || src_max_len == 0 || !dst || dst_size == 0) {
        return -1;
    }

    src_len = strnlen(src, src_max_len + 1u);
    if (src_len > src_max_len) {
        return -1;
    }

    for (i = 0; i < src_len; i++) {
        unsigned char ch = (unsigned char)src[i];
        if (ch == '\r' || ch == '\n' || ch < 0x20) {
            return -1;
        }
        if (ch == '"' || ch == '\\') {
            if (out + 1u >= dst_size) {
                return -1;
            }
            dst[out++] = '\\';
        }
        if (out + 1u >= dst_size) {
            return -1;
        }
        dst[out++] = (char)ch;
    }
    dst[out] = '\0';
    return 0;
}

static int proto_at_get_link_status(nethub_wifi_status_t *status)
{
    nethub_wifi_link_state_t link_state = NETHUB_WIFI_LINK_DOWN;

    nethub_ctrl_link_get_state(&link_state);
    pthread_mutex_lock(&g_at.lock);
    if (link_state == NETHUB_WIFI_LINK_DOWN) {
        g_at.link_synced = 0;
    }
    if (status) {
        *status = g_at.status;
        status->link_state = link_state;
    }
    pthread_mutex_unlock(&g_at.lock);
    return 0;
}

static int proto_at_init(void)
{
    int ret;

    pthread_mutex_lock(&g_at.lock);
    memset(&g_at.status, 0, sizeof(g_at.status));
    g_at.status.link_state = NETHUB_WIFI_LINK_DOWN;
    g_at.status.mode = NETHUB_WIFI_MODE_IDLE;
    g_at.status.sta_state = NETHUB_WIFI_STA_IDLE;
    g_at.link_synced = 0;
    g_at.scan_done = 0;
    g_at.scan_count = 0;
    pthread_mutex_unlock(&g_at.lock);

    ret = nethub_ctrl_link_init(at_rx_cb, NULL);
    if (ret != 0) {
        return ret;
    }

    nethub_ctrl_link_register_event_callback(at_link_event_cb, NULL);

    ret = send_cmd_wait("AT", 1000, NULL, 0);
    if (ret != 0) {
        nethub_ctrl_link_register_event_callback(NULL, NULL);
        nethub_ctrl_link_deinit();
        return ret;
    }

    pthread_mutex_lock(&g_at.lock);
    g_at.initialized = 1;
    g_at.link_synced = 1;
    g_at.status.link_state = NETHUB_WIFI_LINK_UP;
    pthread_mutex_unlock(&g_at.lock);
    return 0;
}

static void proto_at_deinit(void)
{
    pthread_mutex_lock(&g_at.lock);
    g_at.initialized = 0;
    g_at.link_synced = 0;
    pthread_mutex_unlock(&g_at.lock);
    nethub_ctrl_link_register_event_callback(NULL, NULL);
    nethub_ctrl_link_deinit();
}

static int proto_at_get_status(nethub_wifi_status_t *status)
{
    if (!status) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    if (nethub_ctrl_link_get_state(&status->link_state) == 0 && status->link_state == NETHUB_WIFI_LINK_UP) {
        (void)send_cmd_wait("AT+CWJAP?", 1000, NULL, 0);
    }

    return proto_at_get_link_status(status);
}

static int proto_at_get_version(char *buf, size_t len)
{
    char response[AT_RESPONSE_MAX];
    char *line;
    char *saveptr = NULL;
    size_t out = 0;
    int ret;

    if (!buf || len == 0) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    ret = send_cmd_wait("AT+GMR", 2000, response, sizeof(response));
    if (ret != 0) {
        return ret;
    }

    buf[0] = '\0';
    line = strtok_r(response, "\n", &saveptr);
    while (line) {
        if (strcmp(line, "OK") != 0 && strcmp(line, "ERROR") != 0) {
            size_t line_len = strlen(line);
            if (out + line_len + 2u > len) {
                return NETHUB_WIFI_ERR_BUFFER_TOO_SMALL;
            }
            memcpy(buf + out, line, line_len);
            out += line_len;
            buf[out++] = '\n';
            buf[out] = '\0';
        }
        line = strtok_r(NULL, "\n", &saveptr);
    }

    if (out > 0 && buf[out - 1u] == '\n') {
        buf[out - 1u] = '\0';
    }
    return 0;
}

static int proto_at_reboot(void)
{
    int ret = send_line("AT+RST");

    if (ret == 0) {
        sleep(2);
    }
    return ret;
}

static int proto_at_sta_connect(const nethub_wifi_sta_config_t *cfg, int timeout_ms)
{
    char ssid[2 * NETHUB_WIFI_SSID_MAX_LEN + 1];
    char pwd[2 * NETHUB_WIFI_PASSWORD_MAX_LEN + 1];
    char cmd[256];
    int ret;

    if (!cfg || cfg->ssid[0] == '\0' || timeout_ms < 0) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }
    if (at_escape_param(cfg->ssid, NETHUB_WIFI_SSID_MAX_LEN, ssid, sizeof(ssid)) != 0 ||
        at_escape_param(cfg->password, NETHUB_WIFI_PASSWORD_MAX_LEN, pwd, sizeof(pwd)) != 0) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    ret = send_cmd_wait("AT+CWMODE=1", 5000, NULL, 0);
    if (ret != 0) {
        return ret;
    }
    (void)send_cmd_wait("AT+CWDHCP=1,1", 5000, NULL, 0);

    snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"", ssid, pwd);
    ret = send_connect_wait(cmd, timeout_ms);
    if (ret != 0) {
        return ret;
    }

    (void)send_cmd_wait("AT+CWJAP?", 2000, NULL, 0);
    pthread_mutex_lock(&g_at.lock);
    g_at.status.mode = NETHUB_WIFI_MODE_STA;
    if (g_at.status.sta_state == NETHUB_WIFI_STA_CONNECTING) {
        g_at.status.sta_state = NETHUB_WIFI_STA_CONNECTED;
    }
    pthread_mutex_unlock(&g_at.lock);
    return 0;
}

static int proto_at_sta_disconnect(void)
{
    int ret = send_cmd_wait("AT+CWQAP", 5000, NULL, 0);

    if (ret == 0) {
        pthread_mutex_lock(&g_at.lock);
        g_at.status.sta_state = NETHUB_WIFI_STA_IDLE;
        clear_station_locked();
        pthread_mutex_unlock(&g_at.lock);
    }
    return ret;
}

static int proto_at_scan(nethub_wifi_ap_info_t *aps, int max_count, int *actual_count, int timeout_ms)
{
    int ret;
    int copy_count;

    if (!aps || max_count <= 0 || !actual_count || timeout_ms < 0) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    pthread_mutex_lock(&g_at.lock);
    g_at.scan_done = 0;
    g_at.scan_count = 0;
    memset(g_at.scan_cache, 0, sizeof(g_at.scan_cache));
    pthread_mutex_unlock(&g_at.lock);

    ret = send_cmd_wait("AT+CWLAP", 5000, NULL, 0);
    if (ret != 0) {
        return ret;
    }

    ret = wait_scan_done(timeout_ms);
    if (ret != 0) {
        return ret;
    }

    pthread_mutex_lock(&g_at.lock);
    copy_count = g_at.scan_count;
    *actual_count = g_at.scan_count;
    if (copy_count > max_count) {
        copy_count = max_count;
        ret = NETHUB_WIFI_ERR_BUFFER_TOO_SMALL;
    }
    memcpy(aps, g_at.scan_cache, (size_t)copy_count * sizeof(aps[0]));
    pthread_mutex_unlock(&g_at.lock);

    return ret;
}

static int proto_at_ap_start(const nethub_wifi_ap_config_t *cfg, int timeout_ms)
{
    char ssid[2 * NETHUB_WIFI_SSID_MAX_LEN + 1];
    char pwd[2 * NETHUB_WIFI_PASSWORD_MAX_LEN + 1];
    char cmd[256];
    uint8_t channel;
    uint8_t max_conn;
    nethub_wifi_enc_t enc;
    int ret;

    (void)timeout_ms;
    if (!cfg || cfg->ssid[0] == '\0') {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }
    if (at_escape_param(cfg->ssid, NETHUB_WIFI_SSID_MAX_LEN, ssid, sizeof(ssid)) != 0 ||
        at_escape_param(cfg->password, NETHUB_WIFI_PASSWORD_MAX_LEN, pwd, sizeof(pwd)) != 0) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    channel = cfg->channel ? cfg->channel : 11;
    max_conn = cfg->max_conn ? cfg->max_conn : 4;
    enc = cfg->password[0] == '\0' ? NETHUB_WIFI_ENC_OPEN : cfg->enc;
    if (enc == NETHUB_WIFI_ENC_OPEN) {
        pwd[0] = '\0';
    }

    ret = send_cmd_wait("AT+CWMODE=2", 5000, NULL, 0);
    if (ret != 0) {
        return ret;
    }
    ret = send_cmd_wait("AT+CIPAP=\"192.168.4.1\",\"192.168.4.1\",\"255.255.255.0\"", 5000, NULL, 0);
    if (ret != 0) {
        return ret;
    }
    ret = send_cmd_wait("AT+CWDHCPS=1,3,\"192.168.4.50\",\"192.168.4.200\"", 5000, NULL, 0);
    if (ret != 0) {
        return ret;
    }

    snprintf(cmd, sizeof(cmd), "AT+CWSAP=\"%s\",\"%s\",%u,%u,%u,%u",
             ssid,
             pwd,
             channel,
             (unsigned int)enc,
             max_conn,
             cfg->ssid_hidden ? 1u : 0u);
    ret = send_cmd_wait(cmd, 5000, NULL, 0);
    if (ret == 0) {
        pthread_mutex_lock(&g_at.lock);
        g_at.status.mode = NETHUB_WIFI_MODE_AP;
        pthread_mutex_unlock(&g_at.lock);
    }
    return ret;
}

static int proto_at_ap_stop(void)
{
    int ret = send_cmd_wait("AT+CWMODE=0", 5000, NULL, 0);

    if (ret == 0) {
        pthread_mutex_lock(&g_at.lock);
        g_at.status.mode = NETHUB_WIFI_MODE_IDLE;
        pthread_mutex_unlock(&g_at.lock);
    }
    return ret;
}

static int proto_at_ota_update(const char *fw_path, int timeout_ms)
{
    struct timespec total_deadline;
    struct stat st;
    uint8_t chunk[AT_OTA_CHUNK_SIZE];
    off_t sent = 0;
    int fd;
    int ret;
    int last_percent = -1;

    if (!fw_path || fw_path[0] == '\0' || timeout_ms < 0) {
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }
    if (timeout_ms == 0) {
        timeout_ms = AT_OTA_DEFAULT_MS;
    }

    fd = open(fw_path, O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        NETHUB_LOGE("Failed to open OTA image: %s", fw_path);
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    if (fstat(fd, &st) != 0 || !S_ISREG(st.st_mode)) {
        close(fd);
        return NETHUB_WIFI_ERR_INVALID_PARAM;
    }

    ret = ota_validate_file(fd, st.st_size);
    if (ret != NETHUB_WIFI_OK) {
        close(fd);
        return ret;
    }

    NETHUB_LOGI("OTA start: path=%s size=%llu", fw_path, (unsigned long long)st.st_size);

    make_deadline(&total_deadline, timeout_ms);
    pthread_mutex_lock(&g_at.op_mutex);

    ret = ensure_link_synced_locked();
    if (ret != NETHUB_WIFI_OK) {
        pthread_mutex_unlock(&g_at.op_mutex);
        close(fd);
        return ret;
    }

    ret = min_positive_timeout(deadline_remaining_ms(&total_deadline), AT_DEFAULT_MS);
    if (ret > 0) {
        ret = send_cmd_wait_locked("AT+OTASTART=1", ret, NULL, 0);
    } else {
        ret = NETHUB_WIFI_ERR_TIMEOUT;
    }
    if (ret != NETHUB_WIFI_OK) {
        pthread_mutex_unlock(&g_at.op_mutex);
        close(fd);
        return ret;
    }

    while (sent < st.st_size) {
        size_t want = sizeof(chunk);
        int step_timeout;

        if ((off_t)want > st.st_size - sent) {
            want = (size_t)(st.st_size - sent);
        }

        if (ota_read_exact(fd, chunk, want) != 0) {
            ret = NETHUB_WIFI_ERR;
            break;
        }

        step_timeout = min_positive_timeout(deadline_remaining_ms(&total_deadline), AT_OTA_CHUNK_MS);
        if (step_timeout <= 0) {
            ret = NETHUB_WIFI_ERR_TIMEOUT;
            break;
        }

        ret = ota_send_chunk_locked(chunk, want, step_timeout);
        if (ret != NETHUB_WIFI_OK) {
            break;
        }

        sent += (off_t)want;
        if (st.st_size > 0) {
            int percent = (int)((sent * 100) / st.st_size);
            if (percent == 100 || percent >= last_percent + 10) {
                NETHUB_LOGI("OTA progress: %d%% (%llu/%llu)",
                            percent,
                            (unsigned long long)sent,
                            (unsigned long long)st.st_size);
                last_percent = percent;
            }
        }
    }

    if (ret == NETHUB_WIFI_OK) {
        int finish_timeout = min_positive_timeout(deadline_remaining_ms(&total_deadline), AT_OTA_FINISH_MS);
        if (finish_timeout <= 0) {
            ret = NETHUB_WIFI_ERR_TIMEOUT;
        } else {
            ret = ota_finish_locked(finish_timeout);
            if (ret == NETHUB_WIFI_OK &&
                ota_wait_reboot_locked(AT_OTA_REBOOT_MS) != NETHUB_WIFI_OK) {
                NETHUB_LOGW("OTA finish accepted, but reboot/link-down event was not observed");
            }
        }
    } else if (ret != NETHUB_WIFI_ERR_LINK_DOWN) {
        ota_abort_best_effort_locked();
    }

    pthread_mutex_lock(&g_at.lock);
    g_at.link_synced = 0;
    pthread_mutex_unlock(&g_at.lock);

    pthread_mutex_unlock(&g_at.op_mutex);
    close(fd);

    if (ret == NETHUB_WIFI_OK) {
        NETHUB_LOGI("OTA finish accepted, device will reboot");
    } else {
        NETHUB_LOGE("OTA failed: ret=%d sent=%llu/%llu",
                    ret,
                    (unsigned long long)sent,
                    (unsigned long long)st.st_size);
    }
    return ret;
}

static const nethub_ctrl_proto_ops_t g_proto_at = {
    .name = "at",
    .init = proto_at_init,
    .deinit = proto_at_deinit,
    .get_status = proto_at_get_status,
    .get_version = proto_at_get_version,
    .reboot = proto_at_reboot,
    .sta_connect = proto_at_sta_connect,
    .sta_disconnect = proto_at_sta_disconnect,
    .scan = proto_at_scan,
    .ap_start = proto_at_ap_start,
    .ap_stop = proto_at_ap_stop,
    .ota_update = proto_at_ota_update,
};

const nethub_ctrl_proto_ops_t *nethub_ctrl_proto_at(void)
{
    return &g_proto_at;
}
