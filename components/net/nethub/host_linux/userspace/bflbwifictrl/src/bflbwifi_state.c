/**
 * @file bflbwifi_state.c
 * @brief State manager implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include "../include/bflbwifi_log.h"
#include "../include/bflbwifi.h"

/* URC type definition (consistent with parser) */
typedef enum {
    URC_UNKNOWN = 0,
    URC_WIFI_CONNECTING,
    URC_WIFI_CONNECTED,
    URC_WIFI_GOTIP,
    URC_WIFI_ERROR,
    URC_WIFI_DISCONNECTED,
    URC_WIFI_SCAN_DONE,
    URC_AP_STA_CONNECTED,
    URC_AP_STA_DISCONNECTED,
    URC_AP_STA_IP_ASSIGNED,
    URC_CWLAP,
} urc_type_t;

/* Parser function forward declarations */
extern int bflbwifi_parser_parse_cwlap(const char *line, bflbwifi_ap_info_t *ap);
extern int bflbwifi_parser_parse_wifi_error(const char *line);
extern int bflbwifi_parser_parse_mac(const char *line, char *mac);
extern int bflbwifi_parser_parse_cwjap(const char *line, bflbwifi_sta_connection_info_t *info);

/* Internal function forward declarations */
void bflbwifi_state_set_sta_info(const bflbwifi_sta_connection_info_t *info);

/* Maximum number of scanned APs */
#define MAX_SCAN_COUNT  50

/* Maximum number of AP stations */
#define MAX_AP_STA_COUNT  8

/**
 * @brief Global state context
 */
typedef struct {
    /* Basic state */
    bool initialized;
    bflbwifi_mode_t mode;
    bflbwifi_sta_state_t sta_state;  /* WiFi state enumeration */

    /* Station connection information */
    bflbwifi_sta_connection_info_t sta_info;

    /* Scan results */
    bflbwifi_ap_info_t scan_results[MAX_SCAN_COUNT];
    int scan_count;
    bool scan_complete;

    /* AP mode station list */
    bflbwifi_sta_info_t ap_sta_list[MAX_AP_STA_COUNT];
    int ap_sta_count;

    /* Mutexes and condition variables */
    pthread_mutex_t mutex;
    pthread_cond_t state_cond;
    pthread_cond_t scan_cond;

    /* Error information */
    int last_error;
    char last_error_str[128];
} wifi_state_context_t;

/* Global state instance */
static wifi_state_context_t g_state = {
    .initialized = false,
    .mode = WIFI_MODE_IDLE,
    .sta_state = WIFI_STATE_IDLE,
};

/**
 * @brief Initialize state manager
 */
int bflbwifi_state_init(void)
{
    int ret;

    if (g_state.initialized) {
        BFLB_LOGE("State already initialized");
        return -1;
    }

    memset(&g_state, 0, sizeof(wifi_state_context_t));
    g_state.mode = WIFI_MODE_IDLE;
    g_state.sta_state = WIFI_STATE_IDLE;

    /* Initialize mutex */
    ret = pthread_mutex_init(&g_state.mutex, NULL);
    if (ret != 0) {
        BFLB_LOGE("Failed to init mutex");
        return -1;
    }

    /* Initialize condition variable */
    ret = pthread_cond_init(&g_state.state_cond, NULL);
    if (ret != 0) {
        BFLB_LOGE("Failed to init state cond");
        pthread_mutex_destroy(&g_state.mutex);
        return -1;
    }

    ret = pthread_cond_init(&g_state.scan_cond, NULL);
    if (ret != 0) {
        BFLB_LOGE("Failed to init scan cond");
        pthread_cond_destroy(&g_state.state_cond);
        pthread_mutex_destroy(&g_state.mutex);
        return -1;
    }

    g_state.initialized = true;
    BFLB_LOGI("State manager initialized");

    return 0;
}

/**
 * @brief Clean up state manager
 */
void bflbwifi_state_deinit(void)
{
    if (!g_state.initialized) {
        return;
    }

    pthread_cond_destroy(&g_state.scan_cond);
    pthread_cond_destroy(&g_state.state_cond);
    pthread_mutex_destroy(&g_state.mutex);

    memset(&g_state, 0, sizeof(wifi_state_context_t));
    g_state.initialized = false;

    BFLB_LOGI("State manager deinitialized");
}

/* ========== URC Handler Functions ========== */

/**
 * @brief Handle WiFi connecting URC
 */
static void handle_wifi_connecting(void)
{
    pthread_mutex_lock(&g_state.mutex);
    g_state.sta_state = WIFI_STATE_CONNECTING;
    pthread_cond_signal(&g_state.state_cond);
    pthread_mutex_unlock(&g_state.mutex);

    BFLB_LOGI("WiFi: CONNECTING");
}

/**
 * @brief Handle WiFi connected URC
 */
static void handle_wifi_connected(void)
{
    pthread_mutex_lock(&g_state.mutex);
    g_state.sta_state = WIFI_STATE_CONNECTED;
    pthread_cond_signal(&g_state.state_cond);
    pthread_mutex_unlock(&g_state.mutex);

    BFLB_LOGI("WiFi: CONNECTED");
}

/**
 * @brief Handle WiFi got IP URC
 */
static void handle_wifi_gotip(void)
{
    pthread_mutex_lock(&g_state.mutex);
    g_state.sta_state = WIFI_STATE_GOTIP;
    pthread_cond_signal(&g_state.state_cond);
    pthread_mutex_unlock(&g_state.mutex);

    BFLB_LOGI("WiFi: GOTIP");
}

/**
 * @brief Handle AT+CWJAP? query response
 * @param line Response line, format: +CWJAP:<ssid>,<bssid>,<channel>,<rssi>,<wep_en>
 */
void bflbwifi_state_handle_cwjap(const char *line)
{
    bflbwifi_sta_connection_info_t info;

    if (!line) {
        return;
    }

    /* Get current information (keep IP field)*/
    pthread_mutex_lock(&g_state.mutex);
    memcpy(&info, &g_state.sta_info, sizeof(bflbwifi_sta_connection_info_t));
    pthread_mutex_unlock(&g_state.mutex);

    /* Parse +CWJAP response */
    if (bflbwifi_parser_parse_cwjap(line, &info) == 0) {
        /* Update connection information */
        bflbwifi_state_set_sta_info(&info);
        BFLB_LOGI("WiFi info updated: %s (%s, CH:%d, RSSI:%d)",
                  info.ssid, info.bssid, info.channel, info.rssi);
    } else {
        BFLB_LOGE("Failed to parse CWJAP response");
    }
}

/**
 * @brief Handle WiFi error URC
 */
static void handle_wifi_error(const char *line)
{
    int error_code = bflbwifi_parser_parse_wifi_error(line);

    pthread_mutex_lock(&g_state.mutex);
    g_state.sta_state = WIFI_STATE_ERROR;
    g_state.last_error = error_code;
    snprintf(g_state.last_error_str, sizeof(g_state.last_error_str),
             "WiFi error: %d", error_code);
    pthread_cond_signal(&g_state.state_cond);
    pthread_mutex_unlock(&g_state.mutex);

    BFLB_LOGE("WiFi: ERROR (%d)", error_code);
}

/**
 * @brief Handle WiFi disconnected URC
 */
static void handle_wifi_disconnected(void)
{
    pthread_mutex_lock(&g_state.mutex);
    g_state.sta_state = WIFI_STATE_DISCONNECTED;
    pthread_cond_signal(&g_state.state_cond);
    pthread_mutex_unlock(&g_state.mutex);

    BFLB_LOGI("WiFi: DISCONNECTED");
}

/**
 * @brief Handle WiFi scan complete URC
 */
static void handle_wifi_scan_done(void)
{
    pthread_mutex_lock(&g_state.mutex);
    g_state.scan_complete = true;
    pthread_cond_signal(&g_state.scan_cond);
    pthread_mutex_unlock(&g_state.mutex);

    BFLB_LOGI("WiFi: SCAN_DONE (found %d APs)", g_state.scan_count);
}

/**
 * @brief Handle scanned AP URC
 */
static void handle_cwlap(const char *line)
{
    bflbwifi_ap_info_t ap;

    /* First parse AP information */
    if (bflbwifi_parser_parse_cwlap(line, &ap) != 0) {
        return;
    }

    /* Lock and then check and add */
    pthread_mutex_lock(&g_state.mutex);
    if (g_state.scan_count < MAX_SCAN_COUNT) {
        memcpy(&g_state.scan_results[g_state.scan_count], &ap, sizeof(ap));
        g_state.scan_count++;
    }
    pthread_mutex_unlock(&g_state.mutex);

    BFLB_LOGD("Scan: %s (%d dBm)", ap.ssid, ap.rssi);
}

/**
 * @brief Handle AP mode station connected URC
 */
static void handle_ap_sta_connected(const char *line)
{
    char mac[18];

    if (bflbwifi_parser_parse_mac(line, mac) == 0) {
        pthread_mutex_lock(&g_state.mutex);
        if (g_state.ap_sta_count < MAX_AP_STA_COUNT) {
            /* Use memcpy to avoid compiler warning */
            memcpy(g_state.ap_sta_list[g_state.ap_sta_count].mac, mac, sizeof(mac));
            g_state.ap_sta_list[g_state.ap_sta_count].mac[sizeof(mac) - 1] = '\0';
            g_state.ap_sta_count++;
        }
        pthread_mutex_unlock(&g_state.mutex);

        BFLB_LOGI("AP: STA_CONNECTED %s", mac);
    }
}

/**
 * @brief Handle AP mode station disconnected URC
 */
static void handle_ap_sta_disconnected(const char *line)
{
    char mac[18];

    if (bflbwifi_parser_parse_mac(line, mac) == 0) {
        pthread_mutex_lock(&g_state.mutex);
        /* Remove from list */
        for (int i = 0; i < g_state.ap_sta_count; i++) {
            if (strcmp(g_state.ap_sta_list[i].mac, mac) == 0) {
                /* Move subsequent elements */
                for (int j = i; j < g_state.ap_sta_count - 1; j++) {
                    g_state.ap_sta_list[j] = g_state.ap_sta_list[j + 1];
                }
                g_state.ap_sta_count--;
                break;
            }
        }
        pthread_mutex_unlock(&g_state.mutex);

        BFLB_LOGI("AP: STA_DISCONNECTED %s", mac);
    }
}

/**
 * @brief URC handler entry function
 */
void bflbwifi_state_handle_urc(int type, const char *line)
{
    switch (type) {
        case URC_WIFI_CONNECTING:
            handle_wifi_connecting();
            break;
        case URC_WIFI_CONNECTED:
            handle_wifi_connected();
            break;
        case URC_WIFI_GOTIP:
            handle_wifi_gotip();
            break;
        case URC_WIFI_ERROR:
            handle_wifi_error(line);
            break;
        case URC_WIFI_DISCONNECTED:
            handle_wifi_disconnected();
            break;
        case URC_WIFI_SCAN_DONE:
            handle_wifi_scan_done();
            break;
        case URC_CWLAP:
            handle_cwlap(line);
            break;
        case URC_AP_STA_CONNECTED:
            handle_ap_sta_connected(line);
            break;
        case URC_AP_STA_DISCONNECTED:
            handle_ap_sta_disconnected(line);
            break;
        default:
            BFLB_LOGD("Unknown URC type: %d", type);
            break;
    }
}

/* ========== Status Query Interface ========== */

int bflbwifi_state_get_mode(bflbwifi_mode_t *mode)
{
    if (!mode) {
        return -1;
    }

    pthread_mutex_lock(&g_state.mutex);
    *mode = g_state.mode;
    pthread_mutex_unlock(&g_state.mutex);

    return 0;
}

void bflbwifi_state_set_mode(bflbwifi_mode_t mode)
{
    pthread_mutex_lock(&g_state.mutex);
    g_state.mode = mode;
    pthread_mutex_unlock(&g_state.mutex);
}

int bflbwifi_state_get_sta_state(bflbwifi_sta_state_t *state)
{
    if (!state) {
        return -1;
    }

    pthread_mutex_lock(&g_state.mutex);
    *state = g_state.sta_state;
    pthread_mutex_unlock(&g_state.mutex);

    return 0;
}

void bflbwifi_state_set_sta_state(bflbwifi_sta_state_t state)
{
    pthread_mutex_lock(&g_state.mutex);
    g_state.sta_state = state;
    pthread_mutex_unlock(&g_state.mutex);
}

int bflbwifi_state_get_sta_info(bflbwifi_sta_connection_info_t *info)
{
    if (!info) {
        return -1;
    }

    pthread_mutex_lock(&g_state.mutex);
    memcpy(info, &g_state.sta_info, sizeof(bflbwifi_sta_connection_info_t));
    pthread_mutex_unlock(&g_state.mutex);

    return 0;
}

void bflbwifi_state_set_sta_info(const bflbwifi_sta_connection_info_t *info)
{
    if (!info) {
        return;
    }

    pthread_mutex_lock(&g_state.mutex);
    memcpy(&g_state.sta_info, info, sizeof(bflbwifi_sta_connection_info_t));
    pthread_mutex_unlock(&g_state.mutex);
}

/* ========== Scan Result Management ========== */

void bflbwifi_state_reset_scan(void)
{
    pthread_mutex_lock(&g_state.mutex);
    g_state.scan_count = 0;
    g_state.scan_complete = false;
    pthread_mutex_unlock(&g_state.mutex);
}

int bflbwifi_state_wait_scan_complete(int timeout_ms)
{
    struct timespec ts;
    int ret;

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    pthread_mutex_lock(&g_state.mutex);
    while (!g_state.scan_complete) {
        ret = pthread_cond_timedwait(&g_state.scan_cond, &g_state.mutex, &ts);
        if (ret != 0) {
            pthread_mutex_unlock(&g_state.mutex);
            return -3;  /* ETIMEDOUT */
        }
    }
    pthread_mutex_unlock(&g_state.mutex);

    return 0;
}

int bflbwifi_state_get_scan_results(bflbwifi_ap_info_t *ap_list, int max_count, int *actual_count)
{
    int copy_count;

    if (!ap_list || !actual_count) {
        return -1;
    }

    pthread_mutex_lock(&g_state.mutex);
    *actual_count = g_state.scan_count;
    copy_count = (g_state.scan_count < max_count) ? g_state.scan_count : max_count;
    memcpy(ap_list, g_state.scan_results, copy_count * sizeof(bflbwifi_ap_info_t));
    pthread_mutex_unlock(&g_state.mutex);

    return (copy_count == g_state.scan_count) ? 0 : -11;  /* E_ERR_BUFFER_TOO_SMALL */
}

/* ========== Status Wait Interface ========== */

int bflbwifi_state_wait_sta_state(bflbwifi_sta_state_t target_state, int timeout_ms)
{
    struct timespec ts;
    int ret;

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    pthread_mutex_lock(&g_state.mutex);
    while (g_state.sta_state != target_state &&
           g_state.sta_state != WIFI_STATE_ERROR) {
        ret = pthread_cond_timedwait(&g_state.state_cond, &g_state.mutex, &ts);
        if (ret != 0) {
            pthread_mutex_unlock(&g_state.mutex);
            return -3;  /* ETIMEDOUT */
        }
    }

    if (g_state.sta_state == WIFI_STATE_ERROR) {
        pthread_mutex_unlock(&g_state.mutex);
        return -6;  /* E_ERR_CONNECT_FAILED */
    }

    pthread_mutex_unlock(&g_state.mutex);
    return 0;
}
