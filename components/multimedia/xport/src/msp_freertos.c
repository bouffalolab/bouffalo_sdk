/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <event_groups.h>

#include <msp/kernel.h>
#include <xutils/debug.h>

#define ms2tick                     pdMS_TO_TICKS
#define bzero(stack, stack_size)    memset(stack, 0, stack_size)

#ifndef pvPortRealloc
#define pvPortRealloc          realloc
#endif

int msp_task_new_ext(msp_task_t *task, const char *name, void (*fn)(void *), void *arg,
                     int stack_size, int prio)
{
    TaskHandle_t xHandle;
    /* vreify param */
    if (fn == NULL || (stack_size % 4 != 0) ) {
        return -1;
    }

    /*create task */
    if (name == NULL) {
        name = "default_name";
    }
    BaseType_t ret = xTaskCreate(fn, name, stack_size>>2, arg, configMAX_PRIORITIES-prio, &xHandle);

    if (ret == pdPASS) {
        if(task) {
            *task = xHandle;
        }
        //aos_task_create_hook_lwip_thread_sem(task);
        return 0;
    } else {
        return -1;
    }

    return 0;
}

int msp_task_new(const char *name, void (*fn)(void *), void *arg,
                 int stack_size)
{
    return msp_task_new_ext(NULL, name, fn, arg, stack_size, MSP_DEFAULT_APP_PRI);
}

void msp_task_delete(msp_task_t *task)
{
    if(task && *task) {
        vTaskDelete(*task);
    }
}

void msp_task_exit(int code)
{
    (void)code;
    vTaskDelete(NULL);
}

int msp_mutex_new(msp_mutex_t *mutex)
{
    if (NULL == mutex) {
        return -1;
    }

    SemaphoreHandle_t mux = xSemaphoreCreateMutex();
    *mutex = mux;
    return mux != NULL ? 0 : -1;
}

void msp_mutex_free(msp_mutex_t *mutex)
{
    if (mutex && *mutex) {
        vSemaphoreDelete(*mutex);
    }
}

int msp_mutex_lock(msp_mutex_t *mutex, unsigned int ms)
{
    if (mutex && *mutex) {
        if(xPortIsInsideInterrupt()) {
            BaseType_t temp = pdFALSE;
            xSemaphoreTakeFromISR(*mutex, &temp);
        } else {
            xSemaphoreTake(*mutex, ms == MSP_WAIT_FOREVER ? portMAX_DELAY : pdMS_TO_TICKS(ms));
        }
        return 0;
    }
    return -1;
}

int msp_mutex_unlock(msp_mutex_t *mutex)
{
    if (mutex && *mutex) {
        if(xPortIsInsideInterrupt()) {
            BaseType_t temp = pdFALSE;
            xSemaphoreGiveFromISR(*mutex, &temp);
        } else {
            xSemaphoreGive(*mutex);
        }
    }
    return 0;
}

int msp_mutex_is_valid(msp_mutex_t *mutex)
{
    return (((mutex != NULL) && (*mutex != NULL)) ? 1 : 0);
}

int msp_sem_new(msp_sem_t *sem, int count)
{
    if (NULL == sem) {
        return -1;
    }

    SemaphoreHandle_t s = xSemaphoreCreateCounting(1024, count);
    *sem = s;

    return 0;
}

void msp_sem_free(msp_sem_t *sem)
{
    if (NULL == sem || NULL == *sem) {
        return;
    }

    vSemaphoreDelete(*sem);
}

int msp_sem_wait(msp_sem_t *sem, unsigned int ms)
{
    if (NULL == sem || NULL == *sem) {
        return -1;
    }
    int ret;
    if(xPortIsInsideInterrupt()) {
        BaseType_t pxHiProTskWkup = pdTRUE;
        ret = xSemaphoreTakeFromISR(*sem, &pxHiProTskWkup);
    } else {
        ret = xSemaphoreTake(*sem, ms == MSP_WAIT_FOREVER ? portMAX_DELAY : pdMS_TO_TICKS(ms));
    }
    return ret == pdPASS ? 0 : -1;
}

void msp_sem_signal(msp_sem_t *sem)
{
    if (NULL == sem || NULL == *sem) {
        return;
    }
    if(xPortIsInsideInterrupt()) {
        BaseType_t  temp = pdTRUE;
        xSemaphoreGiveFromISR(*sem, &temp);
    } else {
        xSemaphoreGive(*sem);
    }
}

int msp_sem_is_valid(msp_sem_t *sem)
{
    return (((sem != NULL) && (*sem != NULL)) ? 1 : 0);
}

msp_status_t msp_queue_create(msp_queue_t *queue, size_t size, size_t max_msg, uint32_t options)
{
    xQueueHandle q;
    (void)options;
    /* verify param */
    if(NULL == queue || 0 == size) {
        return -1;
    }

    /* create queue object */
    q = xQueueCreate(size / max_msg, max_msg);
    if(NULL == q) {
        return -1;
    }
    *queue = q;

    return 0;
}

int msp_queue_new(msp_queue_t *queue, void *buf, unsigned int size, int max_msg)
{
    xQueueHandle q;
    (void)(buf);
    /* verify param */
    if(NULL == queue || 0 == size) {
        return -1;
    }

    /* create queue object */
    q = xQueueCreate(size / max_msg, max_msg);
    if(NULL == q) {
        return -1;
    }
    *queue = q;

    return 0;
}

void msp_queue_free(msp_queue_t *queue)
{
    /* delete queue object */
    if(queue && *queue) {
        vQueueDelete(*queue);
    }
}

int msp_queue_send(msp_queue_t *queue, void *msg, unsigned int size)
{
    /* verify param */
    if(NULL == queue || NULL == *queue || NULL == msg || 0 == size) {
        return -1;
    }

    /* send msg  to specific queue */
    return xQueueSend(*queue, msg, portMAX_DELAY) == pdPASS ? 0 : -1;
}

int msp_queue_recv(msp_queue_t *queue, unsigned int ms, void *msg,
                   unsigned int *size)
{
    /* verify param */
    if(NULL == queue || NULL == *queue || NULL == msg || 0 == size) {
        return -1;
    }

    /* receive msg from specific queue */
    return xQueueReceive(*queue, msg, ms == MSP_WAIT_FOREVER ? portMAX_DELAY : pdMS_TO_TICKS(ms)) == pdPASS ? 0 : -1;
}

int msp_queue_is_valid(msp_queue_t *queue)
{
    return (((queue != NULL) && (*queue != NULL)) ? 1 : 0);
}

int msp_queue_get_count(msp_queue_t *queue)
{
    if (NULL == queue || NULL == *queue) {
        return -1;
    }

    BaseType_t ret;
    ret = uxQueueMessagesWaiting(*queue);
    return ret;
}

void *msp_queue_buf_ptr(msp_queue_t *queue)
{
    return NULL;
}

typedef struct tmr_adapter {
    TimerHandle_t timer;
    void (*func)(void *, void *);
    void *func_arg;
    uint8_t bIsRepeat;
} tmr_adapter_t;

static void tmr_adapt_cb(TimerHandle_t xTimer)
{
    tmr_adapter_t *pTmrObj = (tmr_adapter_t *)pvTimerGetTimerID(xTimer);

    if(pTmrObj->func)
        pTmrObj->func(pTmrObj, pTmrObj->func_arg);
    return;
}
int msp_timer_new(msp_timer_t *timer, void (*fn)(void *, void *),
                  void *arg, int ms, int repeat)
{
    return msp_timer_new_ext(*timer, fn, arg, ms, repeat, 1);
}

int msp_timer_new_ext(msp_timer_t *timer, void (*fn)(void *, void *),
                      void *arg, int ms, int repeat, unsigned char auto_run)
{
    /* verify param */
    if (NULL == timer || NULL == *timer || 0 == ms|| NULL == fn) {
        return -1;
    }

    /* create timer wrap object ,then initlize timer object */
    tmr_adapter_t *tmr_adapter = pvPortMalloc(sizeof(tmr_adapter_t));

    if (NULL == tmr_adapter) {
        return -1;
    }

    tmr_adapter->func = fn;
    tmr_adapter->func_arg = arg;
    tmr_adapter->bIsRepeat = repeat;

    /* create timer by kernel api */
    TimerHandle_t ptimer = xTimerCreate("Timer", pdMS_TO_TICKS(ms), repeat, tmr_adapter, tmr_adapt_cb);

    if (NULL == timer) {
        vPortFree(tmr_adapter);
        return -1;
    }

    tmr_adapter->timer = ptimer;
    *timer = (void*)tmr_adapter;

    /* start timer if auto_run == TRUE */
    if(auto_run) {
        if(msp_timer_start(*timer) != 0) {
            return -1;
        }
    }

    return 0;
}

void msp_timer_free(msp_timer_t *timer)
{
    if (NULL == timer || NULL == *timer) {
        return;
    }

    tmr_adapter_t *tmr_adapter = *timer;
    int ret = xTimerDelete(tmr_adapter->timer, 0);

    if (!ret) {
        return ;
    }

    vPortFree(tmr_adapter);
    timer = NULL;

    return ;
}

int msp_timer_start(msp_timer_t *timer)
{
    /* verify param */
    if (NULL == timer || NULL == *timer) {
        return -1;
    }

    /* start timer  */
    tmr_adapter_t *tmr_adapter = *timer;
    int ret;

    if(xPortIsInsideInterrupt()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        ret = xTimerStartFromISR(tmr_adapter->timer, &xHigherPriorityTaskWoken);
    } else {
        ret = xTimerStart(tmr_adapter->timer, 0);
    }
    if (ret != pdPASS) {
        return -1;
    }

    return 0;
}

int msp_timer_stop(msp_timer_t *timer)
{
    /* verify param */
    if (NULL == timer || NULL == *timer) {
        return -1;
    }

    /* stop timer */
    tmr_adapter_t *tmr_adapter = *timer;
    int ret;

    if(xPortIsInsideInterrupt()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        ret = xTimerStopFromISR(tmr_adapter->timer, &xHigherPriorityTaskWoken);
    } else {
        ret = xTimerStop(tmr_adapter->timer, 0);
    }

    if (ret != pdPASS) {
        return -1;
    }

    return 0;
}

int msp_timer_change(msp_timer_t *timer, int ms)
{
    /* verify param */
    if (NULL == timer || NULL == *timer) {
        return -1;
    }
    BaseType_t  xHigherProTskWoken  = pdFALSE;
    /* change timer period value */
    tmr_adapter_t *pTimer = *timer;
    int ret;
    if(xPortIsInsideInterrupt()) {
        ret = xTimerChangePeriodFromISR(pTimer->timer,pdMS_TO_TICKS(ms), &xHigherProTskWoken);
    } else {
        ret = xTimerChangePeriod(pTimer->timer, pdMS_TO_TICKS(ms), 10);
    }

    if(ret != pdPASS) {
        return -1;
    }

    return 0;
}

int msp_timer_is_valid(msp_timer_t *timer)
{
    if (NULL == timer || NULL == *timer) {
        return -1;
    }
    tmr_adapter_t *pTimer = *timer;
    if( xTimerIsTimerActive(pTimer->timer) != pdFALSE ) {
        // pTimer is active, do something.
        return 1;
    }
    else {
        // pTimer is not active, do something else.
        return 0;
    }
}

int msp_timer_change_once(msp_timer_t *timer, int ms)
{
    int ret = -1;
    if (NULL == timer || NULL == *timer) {
        return -1;
    }
    //msp_check_return_einval(time);
    tmr_adapter_t *pTimer = *timer;
    if( xTimerIsTimerActive(pTimer->timer) != pdFALSE ) {
        xTimerStop(pTimer->timer, 100);
    }
    if( xTimerIsTimerActive(pTimer->timer) == pdFALSE ) {

        if(xPortIsInsideInterrupt()) {
            BaseType_t xHigherProTskWoken = pdFALSE;
            ret = xTimerChangePeriodFromISR(pTimer->timer, pdMS_TO_TICKS(ms), &xHigherProTskWoken);
        } else {
            ret = xTimerChangePeriod(pTimer->timer, pdMS_TO_TICKS(ms), 100);
        }

        if (ret == pdPASS) {
            return 0;
        } else {
            return -1;
        }
    }

    return ret;
}

// Suspend the scheduler.
// return time in ticks, for how long the system can sleep or power-down.
void msp_kernel_sched_suspend(void)
{
    vTaskSuspendAll();
}

// Resume the scheduler.
// param[in]     sleep_ticks   time in ticks for how long the system was in sleep or power-down mode.
void msp_kernel_sched_resume()
{
    xTaskResumeAll();
}

void *msp_zalloc(unsigned int size)
{
    void* ptr = pvPortMalloc(size);
    if(ptr) {
        bzero(ptr,size);
        //memset(ptr, 0, size);
    }

    return ptr;
}

void *msp_malloc(unsigned int size)
{
    void *p = NULL;
    p = pvPortMalloc(size);

    return p;
}

void msp_free(void *mem)
{
    vPortFree(mem);
}

void msp_freep(char **ptr)
{
    if (ptr && (*ptr)) {
        msp_free(*ptr);
        *ptr = NULL;
    }
}

void msp_msleep(int ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

long long msp_now_ms(void)
{
    // long long ms;
    TickType_t ticks = 0;
    // BaseType_t overflow_count = 0;

    ticks = xTaskGetTickCount();

    //TODO test this formula
    //ms = ((long long)ticks) + ((TickType_t)(-1) * ((long long)overflow_count));

    return ticks;
}

int msp_event_new(msp_event_t *event, unsigned int flags)
{
    EventGroupHandle_t event_handle;
    //event = NULL;
    msp_check_return_einval(event);

    /* create event handle */
    event_handle = xEventGroupCreate();
    /* initlized event */
    if(event_handle != NULL) {
        xEventGroupSetBits(event_handle, flags);
    } else {
        return -1;
    }
    *event = event_handle;

    return 0;
}

void msp_event_free(msp_event_t *event)
{
    if (NULL == event || NULL == *event) {
        return;
    }
    vEventGroupDelete((EventGroupHandle_t)(*event));

    *event = NULL;
}

int msp_event_get
(
    msp_event_t *event,
    unsigned int flags,
    unsigned char opt,
    unsigned int *actl_flags,
    unsigned int timeout
)
{
    uint32_t   wait_bits = 0;
    (void)opt;
    msp_check_return_einval(event);

    if (timeout == MSP_WAIT_FOREVER) {
        wait_bits = xEventGroupWaitBits(*event,
                                        flags,
                                        pdTRUE,
                                        pdFALSE,
                                        0xffffffff
                                       );
    } else {
        wait_bits = xEventGroupWaitBits(*event,
                                        flags,
                                        pdTRUE,
                                        pdFALSE,
                                        pdMS_TO_TICKS(timeout)
                                       );
    }

    *actl_flags = wait_bits;

    if(wait_bits & flags) {
        return 0;
    } else {
        return -1;
    }
}

int msp_event_set(msp_event_t *event, unsigned int flags, unsigned char opt)
{
    msp_check_return_einval(event && *event);

    uint32_t wait_bits = 0;

    if(!flags) {
        /* Clear the all bits in event group */
        wait_bits = xEventGroupWaitBits(*event, 0xffffff, pdTRUE, pdFALSE, 0);
        return 0;
    }

    if(MSP_FLAGS_AND_MASK & opt) {
        /* If opt is MSP_EVENT_AND, overwrite the event group */
        wait_bits = xEventGroupWaitBits(*event, 0xffffff, pdTRUE, pdFALSE, 0);
        flags &= wait_bits;
    }

    /* Make sure the user is not attempting to set the bits used by the kernel itself. */
    if(xPortIsInsideInterrupt()) {
        BaseType_t xHighProTaskWoken = pdFALSE;
        xEventGroupSetBitsFromISR(*event, flags & 0xffffff, &xHighProTaskWoken);
    } else {
        xEventGroupSetBits(*event, flags & 0xffffff);
    }

    return 0;
}

int msp_event_is_valid(msp_event_t *event)
{
    if (NULL == event || NULL == *event) {
        return 0;
    }

    return 1;
}

void *_yoc_malloc(int32_t size, void *caller)
{
    (void)caller;
    return pvPortMalloc(size);

}

void *_yoc_realloc(void *ptr, size_t size, void *caller)
{
    (void)caller;
    return pvPortRealloc(ptr,size);

}

void _yoc_free(void *ptr, void *caller)
{
    (void)caller;
    vPortFree(ptr);
}


void *msp_zalloc_check(size_t size)
{
    void *ptr = _yoc_malloc(size, __builtin_return_address(0));

    //msp_check_mem(ptr);
    if (ptr) {
        memset(ptr, 0, size);
    }

    return ptr;
}

void *msp_malloc_check(size_t size)
{
    void *p = _yoc_malloc(size, __builtin_return_address(0));
    //msp_check_mem(p);

    return p;
}

void *msp_calloc_check(size_t size, size_t num)
{
    return msp_zalloc_check(size * num);
}


void *msp_realloc(void *mem, size_t size)
{
    return pvPortRealloc(mem,size);
}

void *msp_realloc_check(void *ptr, size_t size)
{
    void *new_ptr = _yoc_realloc(ptr, size, __builtin_return_address(0));
    //msp_check_mem(new_ptr);

    return new_ptr;
}

const char *msp_task_get_name(msp_task_t *task)
{
    if (NULL == task || NULL == *task) {
        return NULL;
    }

    return pcTaskGetTaskName(*task);
}

msp_task_t msp_task_self(void)
{
    return xTaskGetCurrentTaskHandle();
}

void msp_task_wdt_attach(void (*will)(void *), void *args)
{
}

void msp_task_wdt_detach()
{
}

void msp_task_wdt_feed(int time)
{
}

int msp_irq_context(void)
{
    return xPortIsInsideInterrupt();
}

// int msp_is_sched_disable(void)
// {

// }

// int msp_is_irq_disable(void)
// {

// }
