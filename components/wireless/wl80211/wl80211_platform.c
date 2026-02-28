#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>

#include "wl80211_mac.h"
#include "wl80211_platform.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

#include "wl80211.h"

#include "async_event.h"

extern int mfg_media_read_macaddr_with_lock(uint8_t mac[6], uint8_t reload);

static TimerHandle_t timeout_tmr;
const struct platform_feature wl80211_platform_feature[] = { { 2, 1, 1 }, { 1, 0, 0 } };

/// RX Packet Reordering Buffer Size
#define MACSW_AMPDU_RX_BUF_SIZE CFG_REORD_BUF
#if (MACSW_AMPDU_RX && ((MACSW_AMPDU_RX_BUF_SIZE < 4) || (MACSW_AMPDU_RX_BUF_SIZE > 64)))
#error "Incorrect reordering buffer size"
#endif

// for station/ap mode
#define WL80211_RX_DESC_MPDU_LEN  (sizeof(struct wl80211_mac_rx_desc) + CO_ALIGN4_HI(RX_MAX_AMSDU_SUBFRAME_LEN + 1))
// for monitor mode
#define WL80211_RX_DESC_AMSDU_LEN (sizeof(struct wl80211_mac_rx_desc) + CO_ALIGN4_HI(MACSW_CONFIG(MACSW_MAX_AMSDU_RX)))

#define WL80211_RX_BUF_MEM_LEN    ((MACSW_MAX_BA_RX * MACSW_AMPDU_RX_BUF_SIZE + 2) * WL80211_RX_DESC_MPDU_LEN)
const unsigned int wl80211_rx_buf_mem_len = WL80211_RX_BUF_MEM_LEN;
uint32_t wl80211_rx_buf_mem[CO_ALIGN4_HI(WL80211_RX_BUF_MEM_LEN) / sizeof(uint32_t)] __SHAREDRAM;

uint32_t rtos_ms2tick(int ms)
{
    uint32_t tick;

    tick = (uint32_t)(pdMS_TO_TICKS(ms));

    return tick;
}

static TickType_t rtos_timeout_2_tickcount(int timeout_ms)
{
    if (timeout_ms < 0) {
        return portMAX_DELAY;
    } else {
        return pdMS_TO_TICKS(timeout_ms);
    }
}

void rtos_lock(void)
{
    vTaskEnterCritical();
}

void rtos_unlock(void)
{
    vTaskExitCritical();
}

uint32_t rtos_now(bool isr)
{
    if (isr) {
        return xTaskGetTickCountFromISR();
    } else {
        return xTaskGetTickCount();
    }
}

int rtos_semaphore_create(rtos_semaphore *semaphore, int max_count, int init_count)
{
    int res = -1;

    if (max_count == 1) {
        *semaphore = xSemaphoreCreateBinary();

        if (*semaphore != 0) {
            if (init_count) {
                xSemaphoreGive(*semaphore);
            }
            res = 0;
        }
    } else {
        *semaphore = xSemaphoreCreateCounting(max_count, init_count);

        if (*semaphore != 0) {
            res = 0;
        }
    }

    return res;
}

void rtos_semaphore_delete(rtos_semaphore semaphore)
{
    vSemaphoreDelete(semaphore);
}

int rtos_semaphore_wait(rtos_semaphore semaphore, int timeout)
{
    BaseType_t res = pdPASS;

    res = xSemaphoreTake(semaphore, rtos_timeout_2_tickcount(timeout));

    return (res == errQUEUE_EMPTY);
}

int rtos_semaphore_signal(rtos_semaphore semaphore, bool isr)
{
    BaseType_t res;

    if (isr) {
        BaseType_t task_woken = pdFALSE;

        res = xSemaphoreGiveFromISR(semaphore, &task_woken);
        portYIELD_FROM_ISR(task_woken);
    } else {
        res = xSemaphoreGive(semaphore);
    }

    return (res == errQUEUE_FULL);
}

int rtos_mutex_create(rtos_mutex *mutex)
{
    int res = -1;

    *mutex = xSemaphoreCreateMutex();
    if (*mutex != 0) {
        res = 0;
    }

    return res;
}

void rtos_mutex_delete(rtos_mutex mutex)
{
    assert(xSemaphoreGetMutexHolder(mutex) == NULL);
    vSemaphoreDelete(mutex);
}

void rtos_mutex_lock(rtos_mutex mutex)
{
    xSemaphoreTake(mutex, portMAX_DELAY);
}

void rtos_mutex_unlock(rtos_mutex mutex)
{
    xSemaphoreGive(mutex);
}

extern unsigned int timeouts_handler(void);
static void timeout_tmr_do(TimerHandle_t xTimer)
{
    unsigned int t = timeouts_handler();
    if (xTimerChangePeriod(timeout_tmr, t, 0) != pdPASS) {
        wl80211_printf("Change timeout timer period failed!!!");
    }
}

void rtos_timeouts_init(void)
{
    timeout_tmr = xTimerCreate("sys_timeout", portMAX_DELAY, pdTRUE, NULL, timeout_tmr_do);
    if (timeout_tmr == NULL) {
        abort();
    }

    if (xTimerStart(timeout_tmr, portMAX_DELAY) != pdPASS) {
        abort();
    }
}

void rtos_timeouts_start(unsigned int delay)
{
    xTimerChangePeriod(timeout_tmr, delay, portMAX_DELAY);
}

#ifdef COMPAT_WIFI_MGMR
static void evt_handler_wrapper(void (*handler)(void))
{
    assert(handler != NULL);
    handler();

    vTaskDelete(NULL);
}

void rtos_start_evt_task(void (*handler)(void))
{
    if (!handler) {
        return;
    }

    xTaskCreate((void *)evt_handler_wrapper, "evt", 256, handler, 20, NULL);
}
#endif

/**
****************************************************************************************
* @brief Post event
****************************************************************************************
*/
void wl80211_post_event(int code1, int code2)
{
    async_post_event(EV_WL80211, code1, code2);
}

// platform feature set index
int platform_feature_index(void)
{
#if defined(BL618DG)
    return 0;
#elif defined(BL616CL) || defined(BL616)
    return 1;
#endif
}

int platform_get_mac(uint8_t vif_type, uint8_t mac[6])
{
    (void)vif_type;
    return mfg_media_read_macaddr_with_lock((uint8_t *)mac, 1);
}

int wl80211_printf(const char *fmt, ...)
{
    va_list args;
    int ret;
    va_start(args, fmt);
    ret = vprintf(fmt, args);
    va_end(args);

    return ret;
}

/*
 ****************************************************************************************
 * CLI COMMAND REGISTRATION (Platform-Specific)
 ****************************************************************************************
 */

#if defined(CONFIG_SHELL) && defined(COMPAT_WIFI_MGMR)
#include "shell.h"

/* Forward declarations of CLI command functions from wifi_mgmr_cli.c */
extern int wifi_connect_cmd(int argc, char **argv);
extern int wifi_disconnect_cmd(int argc, char **argv);
extern void wifi_scan_cmd(int argc, char **argv);
extern void wifi_info_cmd(int argc, char **argv);
extern void wifi_sta_autoconnect_enable_cmd(int argc, char **argv);
extern void wifi_sta_autoconnect_disable_cmd(int argc, char **argv);
extern void wifi_country_code_set_cmd(int argc, char **argv);
extern void wifi_ap_start_cmd(int argc, char **argv);
extern void wifi_ap_stop_cmd(int argc, char **argv);
extern void wifi_sta_list_cmd(int argc, char **argv);

/* CLI command exports - automatically registered when shell component is enabled */
SHELL_CMD_EXPORT_ALIAS(wifi_connect_cmd, wifi_sta_connect, wifi station connect);
SHELL_CMD_EXPORT_ALIAS(wifi_disconnect_cmd, wifi_sta_disconnect, wifi station disconnect);
SHELL_CMD_EXPORT_ALIAS(wifi_scan_cmd, wifi_scan, wifi scan);
SHELL_CMD_EXPORT_ALIAS(wifi_info_cmd, wifi_info, wifi info);
SHELL_CMD_EXPORT_ALIAS(wifi_sta_autoconnect_enable_cmd, wifi_sta_autoconnect_enable, wifi sta enable autoconnect);
SHELL_CMD_EXPORT_ALIAS(wifi_sta_autoconnect_disable_cmd, wifi_sta_autoconnect_disable, wifi sta disable autoconnect);
SHELL_CMD_EXPORT_ALIAS(wifi_country_code_set_cmd, wifi_country_code_set, wifi set country code);
SHELL_CMD_EXPORT_ALIAS(wifi_ap_start_cmd, wifi_ap_start, wifi ap start);
SHELL_CMD_EXPORT_ALIAS(wifi_ap_stop_cmd, wifi_ap_stop, wifi ap stop);
SHELL_CMD_EXPORT_ALIAS(wifi_sta_list_cmd, wifi_sta_list, wifi ap sta list);

#endif /* CONFIG_SHELL */
