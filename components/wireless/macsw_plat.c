#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

#include "macsw.h"

#include "FreeRTOS.h"
#include "task.h"

#include "coexm.h"

#define DBG_TAG "MACSW"

/**
 * WiFi MACSW RTOS porting implemention
 */

/// Stack size of the WIFI task
#define WIFI_TASK_STACK_SIZE 1324

/// Handle of the WiFi task
static TaskHandle_t wifi_task_handle;

/* Main wifi stack entry point */
extern void wifi_main(void *param);

void wifi_task_create(void) {
  BaseType_t res;

  res = xTaskCreate(wifi_main, "wifi fw", WIFI_TASK_STACK_SIZE, NULL, 27,
                    &wifi_task_handle);

  configASSERT(res == pdTRUE);
}

void wifi_task_suspend(void) {
  if (ps_is_coex_mode() && (pm_coex_sleep() == 0))
  {
    // Wait for notification in coex sleep mode
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    pm_coex_wakeup();

    return;
  }

  // wait for notification
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
}

void wifi_task_resume(bool isr) {
  if (isr) {
    BaseType_t task_woken = pdFALSE;

    vTaskNotifyGiveFromISR(wifi_task_handle, &task_woken);
    portYIELD_FROM_ISR(task_woken);
  } else {
    xTaskNotifyGive(wifi_task_handle);
  }
}

uint32_t wifi_sys_now_ms(bool isr) {
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
