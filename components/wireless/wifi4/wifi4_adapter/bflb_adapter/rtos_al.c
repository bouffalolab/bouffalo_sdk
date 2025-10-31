/**
 ****************************************************************************************
 *
 * @file rtos_al.c
 *
 * @brief Implementation of the FreeRTOS abstraction layer.
 *
 * Copyright (C) RivieraWaves 2017-2020
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rtos_def.h"
#include "rtos_al.h"

#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <stdbool.h>

#define TST_SHRAM_PTR(...)
#define ASSERT_ERR(...)

#if configAPPLICATION_ALLOCATED_HEAP
/// Pointer to the start of the HEAP
uint8_t *ucHeap = (uint8_t *)__heap_bottom;
#endif

const int fhost_tcpip_priority = RTOS_TASK_PRIORITY(28);
const int fhost_wifi_priority = RTOS_TASK_PRIORITY(27);
const int fhost_wifi_priority_high = RTOS_TASK_PRIORITY(30);
const int fhost_cntrl_priority = RTOS_TASK_PRIORITY(27);
#if defined(CONFIG_HIGH_PERFORMANCE) && (CONFIG_HIGH_PERFORMANCE == 1)  && CFG_LINK_ALL_IN_FLASH
const int fhost_rx_priority = RTOS_TASK_PRIORITY(30);
#else
const int fhost_rx_priority = RTOS_TASK_PRIORITY(27);
#endif
const int fhost_tx_priority = RTOS_TASK_PRIORITY(29);
const int fhost_wpa_priority = RTOS_TASK_PRIORITY(26);
const int fhost_ipc_priority = RTOS_TASK_PRIORITY(29);
const int fhost_iperf_priority = RTOS_TASK_PRIORITY(27);
const int fhost_connect_priority = RTOS_TASK_PRIORITY(2);
const int fhost_tg_priority = RTOS_TASK_PRIORITY(5);
const int fhost_ping_priority = RTOS_TASK_PRIORITY(27);


/*
 * FUNCTIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief ms to tick
 *
 * @param[in] ms Timeout value in ms
 * @return number of ticks for the specified timeout value.
 *
 ****************************************************************************************
 */
uint32_t rtos_al_ms2tick(int ms)
{
    uint32_t tick;

    tick = (uint32_t)(pdMS_TO_TICKS(ms));

    return tick;
}

/**
 ****************************************************************************************
 * @brief Convert ms to ticks
 *
 * @param[in] timeout_ms Timeout value in ms (use -1 for no timeout).
 * @return number of ticks for the specified timeout value.
 *
 ****************************************************************************************
 */

static TickType_t rtos_timeout_2_tickcount(int timeout_ms)
{
    if (timeout_ms < 0)
    {
        return portMAX_DELAY;
    }
    else
    {
        return pdMS_TO_TICKS(timeout_ms);
    }
}

uint32_t rtos_now(bool isr)
{
    if (isr)
    {
        return xTaskGetTickCountFromISR();
    }
    else
    {
        return xTaskGetTickCount();
    }
}

/**
 * NOTE:  This function takes a relatively long time to complete and should be
 * used sparingly.
 *
 * @return The handle of the task that has the human readable name task_name.
 */
rtos_task_t rtos_task_get_handle(const char *task_name)
{
    if (task_name == NULL) {
        return (rtos_task_handle)xTaskGetCurrentTaskHandle();
    }
    rtos_task_t hdl = (rtos_task_t)xTaskGetHandle(task_name);

    return hdl;
}

int rtos_task_create(rtos_task_fct func,
                     const char * const name,
                     enum rtos_task_id task_id,
                     const uint16_t stack_depth,
                     void * const params,
                     rtos_prio prio,
                     rtos_task_handle * const task_handle)
{
    BaseType_t res;
    rtos_task_handle handle;

    res = xTaskCreate(func, name, stack_depth, params, prio, &handle);

    if (res == pdFAIL)
        return 1;

    #if ( configUSE_TRACE_FACILITY == 1 ) || ( configUSE_RW_TASK_ID == 1 )
    vTaskSetTaskNumber(handle, task_id);
    #endif

    if (task_handle)
        *task_handle = handle;

    return 0;
}

void rtos_task_delete(rtos_task_handle task_handle)
{
    if (!task_handle)
        task_handle = xTaskGetCurrentTaskHandle();
    if (eTaskGetState(task_handle) != eDeleted)
        vTaskDelete(task_handle);
}

void rtos_task_suspend(int duration)
{
    if (duration <= 0)
        return;
    vTaskDelay(pdMS_TO_TICKS(duration));
}

int rtos_task_init_notification(rtos_task_handle task)
{
    return 0;
}

int rtos_task_wait_notification(int timeout)
{
    return ulTaskNotifyTake(pdTRUE, rtos_timeout_2_tickcount(timeout));
}

void rtos_task_notify(rtos_task_handle task, bool isr)
{
    if (isr)
    {
        BaseType_t task_woken = pdFALSE;

        vTaskNotifyGiveFromISR(task, &task_woken);
        portYIELD_FROM_ISR(task_woken);
    }
    else
    {
        xTaskNotifyGive(task);
    }
}

int rtos_queue_create(int elt_size, int nb_elt, rtos_queue *queue)
{
    *queue = xQueueCreate(nb_elt, elt_size);

    if ( *queue == NULL )
        return -1;

    return 0;
}

void rtos_queue_delete(rtos_queue queue)
{
    vQueueDelete(queue);
}

#define GLOBAL_INT_DISABLE()                                                       \
    do {                                                                           \
    int irq_rest;                                                                  \
    asm volatile ("csrrci %0, mstatus, %1" : "=r" (irq_rest) : "i" (1UL << 3)); \
                                                                                   \
    /* Increment the critical section nesting level */                             \
    // CRITICAL_NESTING_INC();

#define GLOBAL_INT_RESTORE()                                                      \
    /* Decrement the critical section nesting level */                            \
    /* CRITICAL_NESTING_DEC(); */                                                 \
    /* Restore mstatus to its previous value */                                   \
    asm volatile ("csrw mstatus, %0" : /* no output */ : "r" (irq_rest));     \
} while(0);

bool rtos_queue_is_empty(rtos_queue queue)
{
    BaseType_t res;

    GLOBAL_INT_DISABLE();
    res = xQueueIsQueueEmptyFromISR(queue);
    GLOBAL_INT_RESTORE();

    return (res == pdTRUE);
}

bool rtos_queue_is_full(rtos_queue queue)
{
    BaseType_t res;

    GLOBAL_INT_DISABLE();
    res = xQueueIsQueueFullFromISR(queue);
    GLOBAL_INT_RESTORE();

    return (res == pdTRUE);
}

int rtos_queue_cnt(rtos_queue queue)
{
    UBaseType_t res;

    GLOBAL_INT_DISABLE();
    res = uxQueueMessagesWaitingFromISR(queue);
    GLOBAL_INT_RESTORE();

    return ((int)res);
}

int rtos_queue_write(rtos_queue queue, void *msg, int timeout, bool isr)
{
    BaseType_t res;

    if (isr)
    {
        BaseType_t task_woken = pdFALSE;

        res = xQueueSendToBackFromISR(queue, msg, &task_woken);
        portYIELD_FROM_ISR(task_woken);
    }
    else
    {
        res = xQueueSendToBack(queue, msg, rtos_timeout_2_tickcount(timeout));
    }

    return (res == errQUEUE_FULL);
}

int rtos_queue_read(rtos_queue queue, void *msg, int timeout, bool isr)
{
    BaseType_t res = pdPASS;

    if (isr)
    {
        BaseType_t task_woken = pdFALSE;

        res = xQueueReceiveFromISR(queue, msg, &task_woken);
        portYIELD_FROM_ISR(task_woken);
    }
    else
    {
        res = xQueueReceive(queue, msg, rtos_timeout_2_tickcount(timeout));
    }

    return (res == errQUEUE_EMPTY);
}

int rtos_semaphore_create(rtos_semaphore *semaphore, int max_count, int init_count)
{
    int res = -1;

    if (max_count == 1)
    {
        *semaphore = xSemaphoreCreateBinary();

        if (*semaphore != 0)
        {
            if (init_count)
            {
                xSemaphoreGive(*semaphore);
            }
            res = 0;
        }
    }
    else
    {
        *semaphore = xSemaphoreCreateCounting(max_count, init_count);

        if (*semaphore != 0)
        {
            res = 0;
        }
    }

    return res;
}

void rtos_semaphore_delete(rtos_semaphore semaphore)
{
    vSemaphoreDelete(semaphore);
}

int rtos_semaphore_get_count(rtos_semaphore semaphore)
{
    return uxSemaphoreGetCount(semaphore);
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

    if (isr)
    {
        BaseType_t task_woken = pdFALSE;

        res = xSemaphoreGiveFromISR(semaphore, &task_woken);
        portYIELD_FROM_ISR(task_woken);
    }
    else
    {
        res = xSemaphoreGive(semaphore);
    }

    return (res == errQUEUE_FULL);
}

int rtos_mutex_create(rtos_mutex *mutex)
{
    int res = -1;

    *mutex = xSemaphoreCreateMutex();
    if (*mutex != 0)
    {
        res = 0;
    }

    return res;
}

void rtos_mutex_delete(rtos_mutex mutex)
{
    ASSERT_ERR(xSemaphoreGetMutexHolder(mutex) == NULL);
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

uint32_t rtos_protect(void)
{
    taskENTER_CRITICAL();
    return 1;
}

void rtos_unprotect(uint32_t protect)
{
    (void) protect;
    taskEXIT_CRITICAL();
}

rtos_task_handle rtos_get_task_handle()
{
    return xTaskGetCurrentTaskHandle();
}

void rtos_trace_task(int id, void *task)
{
}

void rtos_trace_mem(int id, void *ptr, int size, int free_size)
{
}

void rtos_priority_set(rtos_task_handle handle, rtos_prio priority)
{
    vTaskPrioritySet(handle, priority);
}

void *rtos_malloc(uint32_t size)
{
    void *ptr;

    ptr = malloc(size);
    if (NULL == ptr) {
        abort();
    }
    return ptr;
}

void *rtos_calloc(uint32_t nb_elt, uint32_t size)
{
    void *res = malloc(nb_elt * size);
    if (res) {
        memset(res, 0, nb_elt * size);
    } else {
        abort();
    }

    return res;
}

void rtos_free(void *ptr)
{
    free(ptr);
}

#if (configCHECK_FOR_STACK_OVERFLOW > 0)
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    printf("stack overflow task name %s\r\n", pcTaskName);
    while(1);
}
#endif /* (configCHECK_FOR_STACK_OVERFLOW > 0) */
