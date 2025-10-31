#include <assert.h>
#include <stdio.h>

#include "macsw.h"

#include "FreeRTOS.h"
#include "task.h"

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
