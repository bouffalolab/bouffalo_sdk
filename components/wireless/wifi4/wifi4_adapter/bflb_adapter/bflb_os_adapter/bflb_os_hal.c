/****************************************************************************
 * components/platform/soc/bl602/bl602_os_adapter/bl602_os_adapter/bl602_os_hal.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <bflb_os_hal.h>
#include <bflb_os_adapter/bflb_os_adapter.h>
#include <bflb_os_adapter/bflb_os_log.h>
#include <stdbool.h>

#ifdef OS_USING_NUTTX
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <debug.h>
#include <sys/time.h>
#include <timer/timer.h>
#include <clock/clock.h>
#include <syslog.h>
#include <nuttx/config.h>
#include <nuttx/irq.h>
#include <nuttx/mqueue.h>
#include <nuttx/kmalloc.h>
#include <nuttx/pthread.h>
#include <nuttx/wqueue.h>
#include <nuttx/signal.h>
#include <nuttx/semaphore.h>

#include <bl602_netdev.h>
#endif

#ifdef OS_USING_FREERTOS
#include <stdio.h>
#include <stdarg.h>

#include <FreeRTOS.h>
#include <semphr.h>
#include <queue.h>
#include <event_groups.h>
#include <message_buffer.h>
#include <timers.h>

#include <bflb_irq.h>
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct mq_adpt
{
};

struct irq_adpt
{
    void (*func)(void *arg); /* Interrupt callback function */
    void *arg;               /* Interrupt private data */
};

enum bflb_os_timer_mode
{
    BL_OS_TIEMR_ONCE = 0,
    BL_OS_TIEMR_CYCLE
};

typedef enum bflb_os_timer_mode bflb_os_timer_mode_t;

struct timer_adpt
{
    TimerHandle_t handle;
    int32_t delay;
    bflb_os_timer_mode_t mode;
    void (*func)(void *arg);
    void *arg;
};

/****************************************************************************
 * Public Data
 ****************************************************************************/

extern void *__attribute__((weak)) _wifi_log_flag;

bl_ops_funcs_t g_bl_ops_funcs =
{
    ._version = BL_OS_ADAPTER_VERSION,
    ._printf = bflb_os_printf,
    ._puts = bflb_os_puts,
    ._assert = bflb_os_assert_func,
    ._init = bflb_os_api_init,
    ._enter_critical = bflb_os_enter_critical,
    ._exit_critical = bflb_os_exit_critical,
    ._msleep = bflb_os_msleep,
    ._sleep = bflb_os_sleep,
    ._event_group_create = bflb_os_event_create,
    ._event_group_delete = bflb_os_event_delete,
    ._event_group_send = bflb_os_event_send,
    ._event_group_wait = bflb_os_event_wait,
    ._event_register = bflb_os_event_register,
    ._event_notify = bflb_os_event_notify,
    ._task_create = bflb_os_task_create,
    ._task_delete = bflb_os_task_delete,
    ._task_get_current_task = bflb_os_task_get_current_task,
    ._task_notify_create = bflb_os_task_notify_create,
    ._task_notify = bflb_os_task_notify,
    ._task_wait = bflb_os_task_wait,
    ._irq_attach = bflb_os_irq_attach,
    ._irq_enable = bflb_os_irq_enable,
    ._irq_disable = bflb_os_irq_disable,
    ._workqueue_create = bflb_os_workqueue_create,
    ._workqueue_submit_hp = bflb_os_workqueue_submit_hpwork,
    ._workqueue_submit_lp = bflb_os_workqueue_submit_lpwork,
    ._timer_create = bflb_os_timer_create,
    ._timer_delete = bflb_os_timer_delete,
    ._timer_start_once = bflb_os_timer_start_once,
    ._timer_start_periodic = bflb_os_timer_start_periodic,
    ._sem_create = bflb_os_sem_create,
    ._sem_delete = bflb_os_sem_delete,
    ._sem_take = bflb_os_sem_take,
    ._sem_give = bflb_os_sem_give,
    ._mutex_create = bflb_os_mutex_create,
    ._mutex_delete = bflb_os_mutex_delete,
    ._mutex_lock = bflb_os_mutex_lock,
    ._mutex_unlock = bflb_os_mutex_unlock,
    ._queue_create = bflb_os_mq_creat,
    ._queue_delete = bflb_os_mq_delete,
    ._queue_send_wait = bflb_os_mq_send_wait,
    ._queue_send = bflb_os_mq_send,
    ._queue_recv = bflb_os_mq_recv,
    ._malloc = bflb_os_malloc,
    ._free = bflb_os_free,
    ._zalloc = bflb_os_zalloc,
    ._get_time_ms = bflb_os_clock_gettime_ms,
    ._get_tick = bflb_os_get_tick,
    ._log_write = bflb_os_log_write,
    ._task_notify_isr = bflb_os_task_notify_isr,
    ._yield_from_isr = bflb_os_yield_from_isr,
    ._ms_to_tick = bflb_os_ms_to_tick,
    ._set_timeout = bflb_os_set_timeout,
    ._check_timeout = bflb_os_check_timeout
};

extern void vprint(const char *fmt, va_list argp);
extern volatile bool sys_log_all_enable;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: bflb_os_assert_func
 *
 * Description:
 *   Validation program developer's expected result
 *
 * Input Parameters:
 *   file  - configASSERT file
 *   line  - configASSERT line
 *   func  - configASSERT function
 *   expr  - configASSERT condition
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void bflb_os_assert_func(const char *file, int line, const char *func, const char *expr)
{
    printf("Assert failed in %s, %s:%d (%s)", func, file, line, expr);

    configASSERT(0);
}

/****************************************************************************
 * Name: bflb_os_event_create
 *
 * Description:
 *   Create event group
 *
 * Input Parameters:
 *
 * Returned Value:
 *   Event group data pointer
 *
 ****************************************************************************/

BL_EventGroup_t bflb_os_event_create(void)
{
    struct EventGroupDef_t *event;

    event = xEventGroupCreate();

    if (event == NULL) {
        configASSERT(0);
    }

    return event;
}

/****************************************************************************
 * Name: bflb_os_event_delete
 *
 * Description:
 *   Delete event and free resource
 *
 * Input Parameters:
 *   event  - event data point
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void bflb_os_event_delete(BL_EventGroup_t event)
{
    vEventGroupDelete((EventGroupHandle_t)event);
}

/****************************************************************************
 * Name: bflb_os_event_send
 *
 * Description:
 *   Set event bits
 *
 * Input Parameters:
 *
 * Returned Value:
 *   Event value after setting
 *
 ****************************************************************************/

uint32_t bflb_os_event_send(BL_EventGroup_t event, uint32_t bits)
{
    EventGroupHandle_t p_event = (EventGroupHandle_t)event;
    BaseType_t xHigherPriorityTaskWoken;
    BaseType_t xResult;

    /* Being interrupted */

    if (xPortIsInsideInterrupt()) {
        xResult = xEventGroupSetBitsFromISR(p_event, bits, &xHigherPriorityTaskWoken);
    } else {
        xResult = xEventGroupSetBits(p_event, bits);
    }

    return xResult;
}

/****************************************************************************
 * Name: bflb_os_event_wait
 *
 * Description:
 *   Delete timer and free resource
 *
 * Input Parameters:
 *   event
 *   bits_to_wait_for
 *   clear_on_exit
 *   wait_for_all_bits
 *   block_time_tick
 *
 * Returned Value:
 *   Current event value
 *
 ****************************************************************************/
uint32_t bflb_os_event_wait(BL_EventGroup_t event, uint32_t bits_to_wait_for, int clear_on_exit,
                          int wait_for_all_bits, uint32_t block_time_tick)
{
    EventGroupHandle_t p_event = (EventGroupHandle_t)event;

    return xEventGroupWaitBits((EventGroupHandle_t)p_event,
                               bits_to_wait_for,
                               (clear_on_exit != BL_OS_FALSE ? pdTRUE : pdFALSE),
                               (wait_for_all_bits != BL_OS_FALSE ? pdTRUE : pdFALSE),
                               (block_time_tick == BL_OS_WAITING_FOREVER ? portMAX_DELAY : block_time_tick));
}

/****************************************************************************
 * Name: bflb_os_event_register
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int bflb_os_event_register(int type, void *cb, void *arg)
{
    return 0;
}

/****************************************************************************
 * Name: bflb_os_event_notify
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int bflb_os_event_notify(int evt, int val)
{
    return -1;
}

/****************************************************************************
 * Name: bflb_os_task_create
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int bflb_os_task_create(const char *name,
                      void *entry,
                      uint32_t stack_depth,
                      void *param,
                      uint32_t prio,
                      BL_TaskHandle_t task_handle)
{
    return xTaskCreate((TaskFunction_t)entry,
                       name,
                       (stack_depth >> 2),
                       param,
                       prio,
                       (TaskHandle_t *)&task_handle);
}

/****************************************************************************
 * Name: bflb_os_task_delete
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

void bflb_os_task_delete(BL_TaskHandle_t task_handle)
{
    vTaskDelete((TaskHandle_t)task_handle);
}

/****************************************************************************
 * Name: bflb_os_task_get_current_task
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

BL_TaskHandle_t bflb_os_task_get_current_task(void)
{
    return xTaskGetCurrentTaskHandle();
}

/****************************************************************************
 * Name: bflb_os_task_notify_create
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

BL_TaskHandle_t bflb_os_task_notify_create(void)
{
    return xTaskGetCurrentTaskHandle();
}

/****************************************************************************
 * Name: bflb_os_task_notify
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

void bflb_os_task_notify(BL_TaskHandle_t task_handle)
{
    BaseType_t xHigherPriorityTaskWoken;

    configASSERT(task_handle);

    if (xPortIsInsideInterrupt()) {
        vTaskNotifyGiveFromISR(task_handle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        xTaskNotifyGive(task_handle);
    }
}

/****************************************************************************
 * Name: bflb_os_task_wait
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

void bflb_os_task_wait(BL_TaskHandle_t task_handle, uint32_t tick)
{
    ulTaskNotifyTake(pdTRUE, tick);
}

/****************************************************************************
 * Name: bflb_os_api_init
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int bflb_os_api_init(void)
{
    return pdTRUE;
}

/****************************************************************************
 * Name: bflb_os_lock_gaint
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

void bflb_os_lock_gaint(void)
{
}

/****************************************************************************
 * Name: bflb_os_unlock_gaint
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

void bflb_os_unlock_gaint(void)
{
}

/****************************************************************************
 * Name: bflb_os_enter_critical
 *
 * Description:
 *   Enter critical state
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   CPU PS value
 *
 ****************************************************************************/

uint32_t bflb_os_enter_critical(void)
{
    portENTER_CRITICAL();

    return 0;
}

/****************************************************************************
 * Name: bflb_os_exit_critical
 *
 * Description:
 *   Exit from critical state
 *
 * Input Parameters:
 *   level - CPU PS value
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void bflb_os_exit_critical(uint32_t level)
{
    portEXIT_CRITICAL();
}

/****************************************************************************
 * Name: bflb_os_msleep
 *
 * Description:
 *   Sleep in milliseconds
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int bflb_os_msleep(long msec)
{
    const TickType_t ms = msec / portTICK_RATE_MS;

    vTaskDelay(ms);

    return pdTRUE;
}

/****************************************************************************
 * Name: bflb_os_sleep
 *
 * Description:
 *   Sleep in seconds
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int bflb_os_sleep(unsigned int seconds)
{
    const TickType_t ms = seconds * 1000 / portTICK_RATE_MS;

    vTaskDelay(ms);

    return pdTRUE;
}

/****************************************************************************
 * Name: bflb_os_printf
 *
 * Description:
 *   Output format string and its arguments
 *
 * Input Parameters:
 *   format - format string
 *
 * Returned Value:
 *   0
 *
 ****************************************************************************/

void bflb_os_printf(const char *__fmt, ...)
{
    va_list args;

    if (sys_log_all_enable) {
        /* args point to the first variable parameter */
        va_start(args, __fmt);

        /* You can add your code under here. */
        vprintf(__fmt, args);

        va_end(args);
    }
}

/****************************************************************************
 * Name: bflb_os_puts
 *
 * Description:
 *   Output format string
 *
 * Input Parameters:
 *   s - string
 *
 * Returned Value:
 *   0
 *
 ****************************************************************************/

void bflb_os_puts(const char *s)
{
    if (s != NULL) {
        puts(s);
    }
}

/****************************************************************************
 * Name: bflb_os_malloc
 *
 * Description:
 *   Allocate a block of memory
 *
 * Input Parameters:
 *   size - memory size
 *
 * Returned Value:
 *   Memory pointer
 *
 ****************************************************************************/

void *bflb_os_malloc(unsigned int size)
{
    return pvPortMalloc(size);
}

/****************************************************************************
 * Name: bflb_os_free
 *
 * Description:
 *   Free a block of memory
 *
 * Input Parameters:
 *   ptr - memory block
 *
 * Returned Value:
 *   No
 *
 ****************************************************************************/

void bflb_os_free(void *ptr)
{
    vPortFree(ptr);
}

/****************************************************************************
 * Name: bflb_os_zalloc
 *
 * Description:
 *   Allocate a block of memory
 *
 * Input Parameters:
 *   size - memory size
 *
 * Returned Value:
 *   Memory pointer
 *
 ****************************************************************************/

void *bflb_os_zalloc(unsigned int size)
{
    void * pv = NULL;

    {
        pv = pvPortMalloc( size );

        if( pv != NULL )
        {
            ( void ) memset( pv, 0, size );
        }
    }

    return pv;
}

/****************************************************************************
 * Name: bflb_os_errno_trans
 *
 * Description:
 *   Transform from nuttx Os error code to Wi-Fi adapter error code
 *
 * Input Parameters:
 *   ret - NuttX error code
 *
 * Returned Value:
 *   Wi-Fi adapter error code
 *
 ****************************************************************************/

static inline int32_t bflb_os_errno_trans(int ret)
{
    if (ret == pdTRUE) {
        return 0;
    } else {
        return 1;
    }
}

/****************************************************************************
 * Name: bflb_os_mq_creat
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

BL_MessageQueue_t bflb_os_mq_creat(uint32_t queue_len, uint32_t item_size)
{
    return xQueueCreate(queue_len, item_size);
}

/****************************************************************************
 * Name: bflb_os_mq_delete
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

void bflb_os_mq_delete(BL_MessageQueue_t mq)
{
    vQueueDelete((QueueHandle_t)mq);
}

/****************************************************************************
 * Name: bflb_os_mq_send_wait
 *
 * Description:
 *   Generic send message to queue within a certain period of time
 *
 * Input Parameters:
 *   queue - Message queue data pointer
 *   item  - Message data pointer
 *   ticks - Wait ticks
 *   prio  - Message priority
 *
 * Returned Value:
 *   True if success or false if fail
 *
 ****************************************************************************/

int bflb_os_mq_send_wait(BL_MessageQueue_t queue, void *item, uint32_t len, uint32_t ticks, int prio)
{
    int ret;
    QueueHandle_t mq = (QueueHandle_t)queue;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Wi-Fi interrupt function will call this adapter function to send
       * message to message queue, so here we should call kernel API
       * instead of application API
       */

    if (xPortIsInsideInterrupt()) {
        ret = xQueueSendFromISR(mq, item, &xHigherPriorityTaskWoken);
    } else {
        ret = xQueueSend(mq, item, ticks);
    }

    return bflb_os_errno_trans(ret);
}

/****************************************************************************
 * Name: bflb_os_mq_send
 *
 * Description:
 *   Send message of low priority to queue within a certain period of time
 *
 * Input Parameters:
 *   queue - Message queue data pointer
 *   item  - Message data pointer
 *   ticks - Wait ticks
 *
 * Returned Value:
 *   True if success or false if fail
 *
 ****************************************************************************/

int bflb_os_mq_send(BL_MessageQueue_t queue, void *item, uint32_t len)
{
    return bflb_os_mq_send_wait(queue, item, len, 0, 0);
}

/****************************************************************************
 * Name: bflb_os_mq_recv
 *
 * Description:
 *   Receive message from queue within a certain period of time
 *
 * Input Parameters:
 *   queue - Message queue data pointer
 *   item  - Message data pointer
 *   ticks - Wait ticks
 *
 * Returned Value:
 *   True if success or false if fail
 *
 ****************************************************************************/

int bflb_os_mq_recv(BL_MessageQueue_t queue, void *item, uint32_t len, uint32_t tick)
{
    int ret;
    QueueHandle_t mq = (QueueHandle_t)queue;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Wi-Fi interrupt function will call this adapter function to send
       * message to message queue, so here we should call kernel API
       * instead of application API
       */

    if (xPortIsInsideInterrupt()) {
        ret = xQueueReceiveFromISR(mq, item, &xHigherPriorityTaskWoken);
    } else {
        ret = xQueueReceive(mq, item, tick);
    }

    return bflb_os_errno_trans(ret);
}

/****************************************************************************
 * Name: bflb_os_timer_callback
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

static void bflb_os_timer_callback(TimerHandle_t arg)
{
    TimerHandle_t handle;
    struct timer_adpt *timer;

    handle = (TimerHandle_t)arg;
    timer = (struct timer_adpt *)pvTimerGetTimerID(handle);

    if (timer != NULL && timer->func) {
        timer->func(timer->arg);
    }
}

/****************************************************************************
 * Name: bflb_os_timer_create
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

BL_Timer_t bflb_os_timer_create(void *func, void *argv)
{
    struct timer_adpt *timer;

    timer = (struct timer_adpt *)bflb_os_zalloc(sizeof(struct timer_adpt));

    if (!timer) {
        configASSERT(0);
    }

    timer->handle = xTimerCreate(NULL, 1000, pdFALSE, (void *)timer, bflb_os_timer_callback);

    if (timer->handle == NULL) {
        configASSERT(0);
    }

    timer->func = (void (*)(void *arg))func;
    timer->arg = argv;

    return timer;
}

/****************************************************************************
 * Name: bflb_os_timer_delete
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int bflb_os_timer_delete(BL_Timer_t timerid, uint32_t tick)
{
    struct timer_adpt *timer;

    timer = (struct timer_adpt *)timerid;

    if (!timer) {
        return pdFALSE;
    }

    xTimerStop(timer->handle, tick);

    xTimerDelete(timer->handle, tick);

    vPortFree(timer);

    return pdTRUE;
}

/****************************************************************************
 * Name: bflb_os_timer_start_once
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int bflb_os_timer_start_once(BL_Timer_t timerid, long t_sec, long t_nsec)
{
    struct timer_adpt *timer;
    int32_t tick;
    int ret;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    timer = (struct timer_adpt *)timerid;

    if (!timer) {
        return pdFALSE;
    }

    tick = pdMS_TO_TICKS((t_sec * 1000) + ((t_nsec > 1e6) ? (t_nsec / 1e6) : 0));

    timer->mode = BL_OS_TIEMR_ONCE;
    timer->delay = tick;

    vTimerSetReloadMode(timer->handle, pdFALSE);

    if (xPortIsInsideInterrupt()) {
        ret = xTimerChangePeriodFromISR(timer->handle, tick, &xHigherPriorityTaskWoken);
    }
    else {
        ret = xTimerChangePeriod(timer->handle, tick, portMAX_DELAY);

    }

    return bflb_os_errno_trans(ret);
}

/****************************************************************************
 * Name: bflb_os_timer_start_periodic
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int bflb_os_timer_start_periodic(BL_Timer_t timerid, long t_sec, long t_nsec)
{
    struct timer_adpt *timer;
    int32_t tick;
    int ret;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    timer = (struct timer_adpt *)timerid;

    if (!timer) {
        return pdFALSE;
    }

    tick = pdMS_TO_TICKS((t_sec * 1000) + ((t_nsec > 1e6) ? (t_nsec / 1e6) : 0));

    timer->mode = BL_OS_TIEMR_CYCLE;
    timer->delay = tick;

    vTimerSetReloadMode(timer->handle, pdTRUE);

    if (xPortIsInsideInterrupt()) {
        ret = xTimerChangePeriodFromISR(timer->handle, tick, &xHigherPriorityTaskWoken);
    }
    else {
        ret = xTimerChangePeriod(timer->handle, tick, portMAX_DELAY);

    }

    return bflb_os_errno_trans(ret);
}

/****************************************************************************
 * Name: bflb_os_workqueue_create
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

void *bflb_os_workqueue_create(void)
{
    return xTaskGetCurrentTaskHandle();
}

/****************************************************************************
 * Name: bflb_os_workqueue_submit_hpwork
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int bflb_os_workqueue_submit_hpwork(void *work, void *worker, void *argv, long tick)
{
    bflb_os_task_notify(work);

    return 0;
}

/****************************************************************************
 * Name: bflb_os_workqueue_submit_lpwork
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int bflb_os_workqueue_submit_lpwork(void *work, void *worker, void *argv, long tick)
{
    bflb_os_task_notify(work);

    return 0;
}

/****************************************************************************
 * Name: bflb_os_clock_gettime_ms
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

uint64_t bflb_os_clock_gettime_ms(void)
{
    TimeOut_t pxTimeOut;
    unsigned long long ms = 0;

    vTaskSetTimeOutState(&pxTimeOut);

    ms = ( uint64_t ) ( pxTimeOut.xOverflowCount ) << ( sizeof( TickType_t ) * 8 );
    ms += ((long long)pxTimeOut.xTimeOnEntering);
    ms = ms * portTICK_PERIOD_MS;

    return ms;
}

/****************************************************************************
 * Name: bflb_os_get_tick
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

uint32_t bflb_os_get_tick()
{
    uint32_t tick;

    if (xPortIsInsideInterrupt()) {
        tick = xTaskGetTickCountFromISR();
    }
    else {
        tick = xTaskGetTickCount();
    }

    return tick;
}

/****************************************************************************
 * Name: bflb_os_irq_attach
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

void bflb_os_irq_attach(int32_t n, void *f, void *arg)
{
    struct irq_adpt *adapter;

    //blog_info("irq attach: n=%ld f=%p arg=%p\n", n, f, arg);

    bflb_irq_clear_pending(n);

    bflb_irq_attach(n, (irq_callback)f, arg);
    bflb_irq_enable(n);
}

/****************************************************************************
 * Name: bflb_os_irq_enable
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

void bflb_os_irq_enable(int32_t n)
{
    bflb_irq_enable(n);
}

/****************************************************************************
 * Name: bflb_os_irq_disable
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

void bflb_os_irq_disable(int32_t n)
{
    bflb_irq_disable(n);
}

/****************************************************************************
 * Name: bflb_os_mutex_create
 *
 * Description:
 *   Create mutex
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   Mutex data pointer
 *
 ****************************************************************************/

BL_Mutex_t bflb_os_mutex_create(void)
{
    SemaphoreHandle_t xSemaphore;

    xSemaphore = xSemaphoreCreateMutex();
    configASSERT(xSemaphore);

    return xSemaphore;
}

/****************************************************************************
 * Name: bflb_os_mutex_delete
 *
 * Description:
 *   Delete mutex
 *
 * Input Parameters:
 *   mutex_data - mutex data pointer
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void bflb_os_mutex_delete(BL_Mutex_t mutex_data)
{
    SemaphoreHandle_t sem = (SemaphoreHandle_t)mutex_data;

    vSemaphoreDelete(sem);
}

/****************************************************************************
 * Name: bflb_os_mutex_lock
 *
 * Description:
 *   Lock mutex
 *
 * Input Parameters:
 *   mutex_data - mutex data pointer
 *
 * Returned Value:
 *   True if success or false if fail
 *
 ****************************************************************************/

int32_t bflb_os_mutex_lock(BL_Mutex_t mutex_data)
{
    SemaphoreHandle_t sem = (SemaphoreHandle_t)mutex_data;
    BaseType_t xHigherPriorityTaskWoken;
    int ret = pdFALSE;

    if (sem != NULL) {
        if (xPortIsInsideInterrupt()) {
            ret = xSemaphoreTakeFromISR(sem, &xHigherPriorityTaskWoken);
        } else {
            ret = xSemaphoreTake(sem, portMAX_DELAY);
        }
    }

    return bflb_os_errno_trans(ret);
}

/****************************************************************************
 * Name: bflb_os_mutex_unlock
 *
 * Description:
 *   Lock mutex
 *
 * Input Parameters:
 *   mutex_data - mutex data pointer
 *
 * Returned Value:
 *   True if success or false if fail
 *
 ****************************************************************************/

int32_t bflb_os_mutex_unlock(BL_Mutex_t mutex_data)
{
    SemaphoreHandle_t sem = (SemaphoreHandle_t)mutex_data;
    BaseType_t xHigherPriorityTaskWoken;
    int ret = pdFALSE;

    if (sem != NULL) {
        if (xPortIsInsideInterrupt()) {
            ret = xSemaphoreGiveFromISR(sem, &xHigherPriorityTaskWoken);
        } else {
            ret = xSemaphoreGive(sem);
        }
    }

    return bflb_os_errno_trans(ret);
}

/****************************************************************************
 * Name: bflb_os_sem_create
 *
 * Description:
 *   Create and initialize semaphore
 *
 * Input Parameters:
 *   max  - No mean
 *   init - semaphore initialization value
 *
 * Returned Value:
 *   Semaphore data pointer
 *
 ****************************************************************************/

BL_Sem_t bflb_os_sem_create(uint32_t init)
{
    SemaphoreHandle_t xSemaphore;

    xSemaphore = xSemaphoreCreateBinary();
    configASSERT(xSemaphore);

    return xSemaphore;
}

/****************************************************************************
 * Name: bflb_os_sem_delete
 *
 * Description:
 *   Delete semaphore
 *
 * Input Parameters:
 *   semphr - Semaphore data pointer
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void bflb_os_sem_delete(BL_Sem_t semphr)
{
    SemaphoreHandle_t sem = (SemaphoreHandle_t)semphr;

    vSemaphoreDelete(sem);
}

/****************************************************************************
 * Name: bflb_os_sem_take
 *
 * Description:
 *   Wait semaphore within a certain period of time
 *
 * Input Parameters:
 *   semphr - Semaphore data pointer
 *   ticks  - Wait system ticks
 *
 * Returned Value:
 *   True if success or false if fail
 *
 ****************************************************************************/

int32_t bflb_os_sem_take(BL_Sem_t semphr, uint32_t ticks)
{
    SemaphoreHandle_t sem = (SemaphoreHandle_t)semphr;
    BaseType_t xHigherPriorityTaskWoken;
    int ret = pdFALSE;

    if (sem != NULL) {
        if (xPortIsInsideInterrupt()) {
            ret = xSemaphoreTakeFromISR(sem, &xHigherPriorityTaskWoken);
        } else {
            ret = xSemaphoreTake(sem, portMAX_DELAY);
        }
    }

    return bflb_os_errno_trans(ret);
}

/****************************************************************************
 * Name: bflb_os_sem_give
 *
 * Description:
 *   Post semaphore
 *
 * Input Parameters:
 *   semphr - Semaphore data pointer
 *
 * Returned Value:
 *   True if success or false if fail
 *
 ****************************************************************************/

int32_t bflb_os_sem_give(BL_Sem_t semphr)
{
    SemaphoreHandle_t sem = (SemaphoreHandle_t)semphr;
    BaseType_t xHigherPriorityTaskWoken;
    int ret = pdFALSE;

    if (sem != NULL) {
        if (xPortIsInsideInterrupt()) {
            ret = xSemaphoreGiveFromISR(sem, &xHigherPriorityTaskWoken);
        } else {
            ret = xSemaphoreGive(sem);
        }
    }

    return bflb_os_errno_trans(ret);
}

/****************************************************************************
 * Name: bflb_os_log_writev
 *
 * Description:
 *   Output log with by format string and its arguments
 *
 * Input Parameters:
 *   level  - log level, no mean here
 *   tag    - log TAG, no mean here
 *   file   - file name
 *   line   - assert line
 *   format - format string
 *   args   - arguments list
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

static void bflb_os_log_writev(uint32_t level,
                             const char *tag,
                             const char *file,
                             int line,
                             const char *format,
                             va_list args)
{
#if (CFG_COMPONENT_BLOG_ENABLE == 1)
    if ((level >= REFC_LEVEL(__COMPONENT_NAME_DEQUOTED__)) &&
        (level >= REFF_LEVEL(__COMPONENT_FILE_NAME_DEQUOTED__)))
    {
        bflb_os_printf("[%10u][%s: %s:%4d] ",
                     bflb_os_get_tick(),
                     tag,
                     file,
                     line);

        if (sys_log_all_enable) {
            vprint(format, args);
        }
    }
#endif
}

/****************************************************************************
 * Name: bflb_os_log_write
 *
 * Description:
 *   Output log with by format string and its arguments
 *
 * Input Parameters:
 *   level  - log level, no mean here
 *   file   - file name
 *   line   - assert line
 *   tag    - log TAG, no mean here
 *   format - format string
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void bflb_os_log_write(uint32_t level, const char *tag, const char *file, int line, const char *format, ...)
{
#if 0
    va_list list;
    va_start(list, format);

    switch (level) {
        case LOG_LEVEL_ERROR:
        {
            bflb_os_log_writev(BLOG_LEVEL_ERROR, "\x1b[31mERROR \x1b[0m", file, line, format, list);
            break;
        }
        case LOG_LEVEL_WARN:
        {
            bflb_os_log_writev(BLOG_LEVEL_WARN, "\x1b[33mWARN  \x1b[0m", file, line, format, list);
            break;
        }
        case LOG_LEVEL_INFO:
        {
            bflb_os_log_writev(BLOG_LEVEL_INFO, "\x1b[32mINFO  \x1b[0m", file, line, format, list);
            break;
        }
        case LOG_LEVEL_DEBUG:
        {
            bflb_os_log_writev(BLOG_LEVEL_DEBUG, "DEBUG ", file, line, format, list);
            break;
        }
    }

    va_end(list);
#endif
}

/****************************************************************************
 * Name: bflb_os_task_notify_isr
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int bflb_os_task_notify_isr(BL_TaskHandle_t task_handle)
{
    BaseType_t xHigherPriorityTaskWoken;

    configASSERT(task_handle);

    vTaskNotifyGiveFromISR(task_handle, &xHigherPriorityTaskWoken);

    return xHigherPriorityTaskWoken;
}

/****************************************************************************
 * Name: bflb_os_yield_from_isr
 *
 * Description:
 *
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

void bflb_os_yield_from_isr(int xYield)
{
    portYIELD_FROM_ISR(xYield);
}

/****************************************************************************
 * Name: bflb_os_ms_to_tick
 *
 * Description:
 *  
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

unsigned int bflb_os_ms_to_tick(unsigned int ms)
{
    return pdMS_TO_TICKS(ms);
}

/****************************************************************************
 * Name: bflb_os_set_timeout
 *
 * Description:
 *  
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

BL_TimeOut_t bflb_os_set_timeout(void)
{
    TimeOut_t *xTimeOut = NULL;

    xTimeOut = (TimeOut_t *)bflb_os_malloc(sizeof(TimeOut_t));

    vTaskSetTimeOutState(xTimeOut);

    return (BL_TimeOut_t)xTimeOut;
}

/****************************************************************************
 * Name: bflb_os_check_timeout
 *
 * Description:
 *  
 * Input Parameters:
 *
 * Returned Value:
 *
 ****************************************************************************/

int bflb_os_check_timeout(BL_TimeOut_t xTimeOut, BL_TickType_t *xTicksToWait)
{
    return xTaskCheckForTimeOut((TimeOut_t *)xTimeOut, (TickType_t *)xTicksToWait);
}

