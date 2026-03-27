#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

#include "macsw.h"

#include "FreeRTOS.h"
#include "task.h"

#include "coexm.h"
#include "wl_api.h"

#ifdef CFG_BL_WIFI_PS_ENABLE
#include "bl_lp.h"
static volatile iot2lp_para_t *piot2lp_para = (iot2lp_para_t *)IOT2LP_PARA_ADDR;
#endif

#define DBG_TAG              "MACSW"

/**
 * WiFi MACSW RTOS porting implemention
 */

/// Stack size of the WIFI task
#define WIFI_TASK_STACK_SIZE 1324

/// Handle of the WiFi task
static TaskHandle_t wifi_task_handle;

/* Main wifi stack entry point */
extern void wifi_main(void *param);

void wifi_task_create(void)
{
    BaseType_t res;

    res = xTaskCreate(wifi_main, "wifi fw", WIFI_TASK_STACK_SIZE, NULL, 27, &wifi_task_handle);

    configASSERT(res == pdTRUE);
}

void wifi_task_suspend(void)
{
    // Coex sleep duration is event-driven; coordinator commits sleep only at safe point.
    bool slept_committed = coex_coord_on_wifi_suspend_enter();

    // wait for notification
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    coex_coord_on_wifi_wake(slept_committed);
}

void wifi_task_resume(bool isr)
{
    if (isr) {
        BaseType_t task_woken = pdFALSE;

        vTaskNotifyGiveFromISR(wifi_task_handle, &task_woken);
        portYIELD_FROM_ISR(task_woken);
    } else {
        xTaskNotifyGive(wifi_task_handle);
    }
}

uint32_t wifi_sys_now_ms(bool isr)
{
    if (isr) {
        return xTaskGetTickCountFromISR();
    } else {
        return xTaskGetTickCount();
    }
}

/**
 * WiFi syslog implementation
 */

void wifi_syslog(int priority, const char *fmt, ...)
{
    static const struct {
        const char *level;
        const char *color_start;
        const char *color_end;
    } log_config[] = {
        { "E", "\033[0;31m", "\033[0m" }, /* 0: Error - Red */
        { "W", "\033[0;33m", "\033[0m" }, /* 1: Warning - Yellow */
        { "I", "\033[0m", "\033[0m" },    /* 2: Info - Default */
        { "D", "\033[0m", "\033[0m" },    /* 3: Debug - Default */
        { "R", "", "" },                  /* 4: Raw - Default */
    };

    const char *level_str = "I";
    const char *color_start = "";
    const char *color_end = "";

    if (priority >= 0 && priority < 5) {
        level_str = log_config[priority].level;
        color_start = log_config[priority].color_start;
        color_end = log_config[priority].color_end;
    }

    va_list args;
    va_start(args, fmt);

    if (priority == 4) {
        vprintf(fmt, args);
        va_end(args);
        return;
    }

    printf("%s[%s][" DBG_TAG "] ", color_start, level_str);
    vprintf(fmt, args);
    printf("%s", color_end);

    va_end(args);
}

#ifdef CFG_BL_WIFI_PS_ENABLE
int8_t hal_macsw_lp_rssi_restore(void)
{
    return piot2lp_para->wifi_parameter->bcn_target_level;
}

int hal_macsw_lp_get_resume_wifi(void)
{
    return bl_lp_get_resume_wifi();
}

void hal_macsw_lp_set_resume_wifi(void)
{
    bl_lp_set_resume_wifi();
}

void hal_macsw_lp_clear_resume_wifi(void)
{
    bl_lp_clear_resume_wifi();
}

int hal_macsw_lp_is_wake_by_traffic(void)
{
    return bl_lp_get_wake_reason() & LPFW_WAKEUP_WIFI;
}

int hal_macsw_lp_is_wake_by_ap_existence_check(void)
{
    return bl_lp_get_wake_reason() & LPFW_WAKEUP_AP_LOSS;
}

int hal_macsw_lp_is_wake_by_ap_disconnected(void)
{
    return bl_lp_get_wake_reason() & LPFW_WAKEUP_LOSS_CFG_OVER;
}
#else
int8_t hal_macsw_lp_rssi_restore(void)
{
    return -100;
}

int hal_macsw_lp_get_resume_wifi(void)
{
    return 0;
}

void hal_macsw_lp_clear_resume_wifi(void)
{
}

int hal_macsw_lp_is_wake_by_traffic(void)
{
    return 0;
}

int hal_macsw_lp_is_wake_by_ap_existence_check(void)
{
    return 0;
}

int hal_macsw_lp_is_wake_by_ap_disconnected(void)
{
    return 0;
}
#endif
