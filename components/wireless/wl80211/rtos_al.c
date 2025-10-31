#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "rtos_al.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

static TimerHandle_t timeout_tmr;

uint32_t rtos_al_ms2tick(int ms) {
  uint32_t tick;

  tick = (uint32_t)(pdMS_TO_TICKS(ms));

  return tick;
}

static TickType_t rtos_timeout_2_tickcount(int timeout_ms) {
  if (timeout_ms < 0) {
    return portMAX_DELAY;
  } else {
    return pdMS_TO_TICKS(timeout_ms);
  }
}

void rtos_lock(void) { vTaskEnterCritical(); }

void rtos_unlock(void) { vTaskExitCritical(); }

uint32_t rtos_now(bool isr) {
  if (isr) {
    return xTaskGetTickCountFromISR();
  } else {
    return xTaskGetTickCount();
  }
}

int rtos_semaphore_create(rtos_semaphore *semaphore, int max_count,
                          int init_count) {
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

void rtos_semaphore_delete(rtos_semaphore semaphore) {
  vSemaphoreDelete(semaphore);
}

int rtos_semaphore_wait(rtos_semaphore semaphore, int timeout) {
  BaseType_t res = pdPASS;

  res = xSemaphoreTake(semaphore, rtos_timeout_2_tickcount(timeout));

  return (res == errQUEUE_EMPTY);
}

int rtos_semaphore_signal(rtos_semaphore semaphore, bool isr) {
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

int rtos_mutex_create(rtos_mutex *mutex) {
  int res = -1;

  *mutex = xSemaphoreCreateMutex();
  if (*mutex != 0) {
    res = 0;
  }

  return res;
}

void rtos_mutex_delete(rtos_mutex mutex) {
  assert(xSemaphoreGetMutexHolder(mutex) == NULL);
  vSemaphoreDelete(mutex);
}

void rtos_mutex_lock(rtos_mutex mutex) { xSemaphoreTake(mutex, portMAX_DELAY); }

void rtos_mutex_unlock(rtos_mutex mutex) { xSemaphoreGive(mutex); }

extern unsigned int timeouts_handler(void);
static void timeout_tmr_do(TimerHandle_t xTimer) {
  unsigned int t = timeouts_handler();
  if (xTimerChangePeriod(timeout_tmr, t, 0) != pdPASS) {
    printf("Change timeout timer period failed!!!");
  }
}

void rtos_timeouts_init(void) {
  timeout_tmr =
      xTimerCreate("sys_timeout", portMAX_DELAY, pdTRUE, NULL, timeout_tmr_do);
  if (timeout_tmr == NULL) {
    abort();
  }

  if (xTimerStart(timeout_tmr, portMAX_DELAY) != pdPASS) {
    abort();
  }
}

void rtos_timeouts_start(unsigned int delay) {
  xTimerChangePeriod(timeout_tmr, delay, portMAX_DELAY);
}

/* User defined wifi event handler */
extern void wl80211_event_handler(uint32_t code1, uint32_t code2);

/* async event handler */
static void async_event_handler(void *arg1, uint32_t arg2) {
  /* XXX: Prevents blocking operations in the Timer context. */
  vTaskSuspendAll();

  wl80211_event_handler(arg2, (uint32_t)arg1);

  xTaskResumeAll();
}

/**
****************************************************************************************
* @brief Post Event to upper layer
*
* @param[in] catalogue Type of event.
* @param[in] code Code of event.
****************************************************************************************
*/

void wl80211_post_event(int code1, int code2) {
  BaseType_t xReturn;

  xReturn = xTimerPendFunctionCall(async_event_handler, (void *)code2, code1,
                                   portMAX_DELAY);
  configASSERT(xReturn == pdPASS);
}
