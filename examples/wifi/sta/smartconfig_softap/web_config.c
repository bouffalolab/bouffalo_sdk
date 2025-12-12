#include "web_config.h"
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include <lwip/apps/httpd.h>
#include <lwip/ip4_addr.h>
#include "cJSON.h"
#include "wifi_mgmr_ext.h"

#define DBG_TAG "WEB"
#include "log.h"

#define SCAN_AP_MAX 30
#define AP_CLOSE_DELAY_MS 10000

static web_config_t *g_ctx = NULL;
static TimerHandle_t g_ap_close_timer = NULL;
static volatile int g_scanning = 0;

typedef struct {
    wifi_mgmr_ap_item_t items[SCAN_AP_MAX];
    uint16_t count;
} scan_result_t;

static scan_result_t g_scan_cache = {0};

static void scan_item_cb(wifi_mgmr_ap_item_t *env, uint32_t *param1, wifi_mgmr_ap_item_t *item)
{
    scan_result_t *result = (scan_result_t *)param1;
    if (result->count >= SCAN_AP_MAX) return;
    memcpy(&result->items[result->count], item, sizeof(wifi_mgmr_ap_item_t));
    result->count++;
}

static void scan_task(void *param)
{
    g_scanning = 1;
    wifi_mgmr_scan(NULL, NULL);
    vTaskDelay(pdMS_TO_TICKS(4000));
    
    scan_result_t temp = {0};
    wifi_mgmr_scan_ap_all(NULL, (uint32_t *)&temp, scan_item_cb);

    if (temp.count > 0) {
        memcpy(&g_scan_cache, &temp, sizeof(scan_result_t));
    }
    g_scanning = 0;
    LOG_I("Scan done, found %d APs\r\n", temp.count);
    vTaskDelete(NULL);
}

void web_config_do_scan(void)
{
    if (!g_scanning) {
        xTaskCreate(scan_task, "scan", 1024, NULL, 12, NULL);
    }
}

static const char *cgi_scan(int idx, int num, char *param[], char *val[])
{
    cJSON *array = cJSON_CreateArray();
    if (!array) return NULL;

    if (g_scan_cache.count == 0) {
        cJSON_AddItemToArray(array, cJSON_CreateString("null"));
    } else {
        for (int i = 0; i < g_scan_cache.count; i++) {
            cJSON *obj = cJSON_CreateObject();
            if (obj) {
                cJSON_AddNumberToObject(obj, "chan", g_scan_cache.items[i].channel);
                cJSON_AddNumberToObject(obj, "rssi", g_scan_cache.items[i].rssi);
                cJSON_AddStringToObject(obj, "ssid", g_scan_cache.items[i].ssid);
                cJSON_AddNumberToObject(obj, "auth", g_scan_cache.items[i].auth);
                cJSON_AddItemToArray(array, obj);
            }
        }
    }

    char *json = cJSON_PrintUnformatted(array);
    cJSON_Delete(array);

    web_config_do_scan();

    return json;
}

static const char *cgi_connect(int idx, int num, char *param[], char *val[])
{
    char ssid[33] = {0}, passwd[65] = {0};
    int ok = 0;

    for (int i = 0; i < num; i++) {
        if (strcmp(param[i], "ssid") == 0) {
            strncpy(ssid, val[i], sizeof(ssid) - 1);
            ok = 1;
        } else if (strcmp(param[i], "password") == 0) {
            strncpy(passwd, val[i], sizeof(passwd) - 1);
        }
    }

    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;

    if (ok) {
        g_ctx->conn_status = CONN_STATUS_CONNECTING;
        wifi_mgmr_sta_connect(NULL, ssid, passwd, NULL, NULL, 0, 0);
        cJSON_AddStringToObject(obj, "state", "connecting");
    } else {
        cJSON_AddStringToObject(obj, "state", "failed");
    }

    char *json = cJSON_PrintUnformatted(obj);
    cJSON_Delete(obj);
    return json;
}

static const char *cgi_status(int idx, int num, char *param[], char *val[])
{
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;

    conn_status_t st = g_ctx->conn_status;
    
    if (st == CONN_STATUS_AP_CLOSED || st == CONN_STATUS_SUCCESS) {
        uint32_t ip, gw, mask;
        ip4_addr_t addr;
        wifi_mgmr_sta_ip_get(&ip, &gw, &mask);
        addr.addr = ip;
        cJSON_AddStringToObject(obj, "state", st == CONN_STATUS_AP_CLOSED ? "ap_closed" : "connected");
        cJSON_AddStringToObject(obj, "ip", ip4addr_ntoa(&addr));
        cJSON_AddStringToObject(obj, "msg", st == CONN_STATUS_AP_CLOSED ? "AP Closed" : "AP will close in 10s");
    } else {
        cJSON_AddStringToObject(obj, "state", st == CONN_STATUS_CONNECTING ? "connecting" : "idle");
    }

    char *json = cJSON_PrintUnformatted(obj);
    cJSON_Delete(obj);
    return json;
}

static const tCGI cgi_handlers[] = {
    {"/wifiscan", cgi_scan},
    {"/wificonfig", cgi_connect},
    {"/wifiget", cgi_status},
};

static void ap_close_timer_cb(TimerHandle_t xTimer)
{
    LOG_I("Closing AP after successful connection\r\n");
    wifi_mgmr_ap_stop(NULL);
    if (g_ctx) {
        g_ctx->conn_status = CONN_STATUS_AP_CLOSED;
    }
}

void web_config_notify_result(int success)
{
    if (g_ctx && success) {
        g_ctx->conn_status = CONN_STATUS_SUCCESS;
        LOG_I("WiFi connected, AP will close in 10 seconds\r\n");
        
        if (g_ap_close_timer == NULL) {
            g_ap_close_timer = xTimerCreate("ap_close", pdMS_TO_TICKS(AP_CLOSE_DELAY_MS), 
                                            pdFALSE, NULL, ap_close_timer_cb);
        }
        if (g_ap_close_timer) {
            xTimerStart(g_ap_close_timer, 0);
        }
    }
}

int web_config_init(web_config_t *ctx)
{
    if (!ctx) return -1;
    g_ctx = ctx;
    ctx->conn_status = CONN_STATUS_IDLE;
    httpd_init();
    http_set_cgi_handlers(cgi_handlers, sizeof(cgi_handlers) / sizeof(cgi_handlers[0]));
    return 0;
}
