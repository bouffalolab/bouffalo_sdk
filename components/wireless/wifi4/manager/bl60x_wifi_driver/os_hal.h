#ifndef __OS_HAL_H__
#define __OS_HAL_H__

#ifdef OS_USING_RTTHREAD
#include <rtthread.h>

#define os_printf(...) rt_kprintf(__VA_ARGS__)

/**
 ****************************************************************************************
 *
 * @file os_hal.h
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

#define os_free rt_free
#define os_malloc rt_malloc

/*event definition*/
typedef struct rt_event os_event_t;
#define os_event_init(ev) rt_event_init(ev, "wifi_" #ev, RT_IPC_FLAG_FIFO);
#define os_event_send rt_event_send
#define os_event_sendFromISR rt_event_send
#define os_event_recv(irq_event, bits, timeout, val) rt_event_recv(irq_event, bits, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, timeout, &val)
#define os_event_delete rt_event_detach
/*mutex*/
typedef rt_mutex_t os_mutex_t;
#define os_mutex_create(name) rt_mutex_create(name, RT_IPC_FLAG_FIFO);
#define os_mutex_take rt_mutex_take
#define os_mutex_give rt_mutex_release
#define OS_WAITING_FOREVER RT_WAITING_FOREVER
/*int related*/
#define os_interrupt_enter(...) rt_interrupt_enter(...)
#define os_interrupt_leave(...) rt_interrupt_leave(...)
/*thread related*/
#define os_thread_delay rt_thread_delay
#define os_tick_get rt_tick_get
#define os_tick_getFromISR rt_tick_get
#define os_thread_create(name, entry, priority, stack_size, param) do { \
    rt_thread_t tid;\
    tid = rt_thread_create(name,\
        entry, param,\
        stack_size, priority, 20);\
    RT_ASSERT(RT_NULL != tid);\
    if (tid != RT_NULL) {\
        rt_thread_startup(tid);\
    }\
} while(0)
/*message queue related*/
typedef rt_messagequeue os_messagequeue_t;
#define os_mq_init(mq, name, buffer, msgsize, buffersize) rt_mq_init(\
            mq, \
            name, \
            buffer, \
            msgsize, \
            buffersize \
            RT_IPC_FLAG_FIFO \
);
#define os_mq_send(mq, msg, len) rt_mq_send(mq, msg, len);
#define os_mq_recv(mq, msg, len) rt_mq_recv(mq, msg, len, RT_WAITING_FOREVER)
/*timer related*/
typedef rt_timer_t os_timer_t;
typedef void* timer_cb_arg_t;
#define OS_TIMER_TYPE_ONESHOT RT_TIMER_FLAG_ONE_SHOT
#define os_timer_delete(timer) rt_timer_detach(timer)
#define os_timer_delete_nodelay(timer) rt_timer_detach(timer)
#define os_timer_init(timer, name, cb, param, ticks, type) rt_timer_init(timer, \
        name, \
        cb, \
        param, \
        ticks, \
        type \
);
#define os_timer_start(timer) rt_timer_start(timer)
#define os_timer_startFromISR(timer) rt_timer_start(timer)
#define os_timer_data(timer) (timer)

#endif



#define OS_USING_FREERTOS
#ifdef OS_USING_FREERTOS
#include <FreeRTOS.h>
#include <semphr.h>
#include <event_groups.h>
#include <message_buffer.h>
#include <timers.h>
#include <stdio.h>

#if 0
int os_printf(const char* format, ...);
#else
#define os_printf(...) printf(__VA_ARGS__)
#endif
/*mem*/
#define os_free vPortFree
#define os_malloc pvPortMalloc

/*event definition*/
typedef StaticEventGroup_t os_event_t;
#define os_event_init(ev) xEventGroupCreateStatic(ev)
#define os_event_send(ev, bits) xEventGroupSetBits((EventGroupHandle_t)ev, bits)
#define os_event_sendFromISR(ev, bits) do { \
    BaseType_t xHigherPriorityTaskWoken, xResult; \
    (void) xHigherPriorityTaskWoken; \
    (void) xResult; \
    xResult = xEventGroupSetBitsFromISR(ev, bits, &xHigherPriorityTaskWoken); \
} while (0)

#define os_event_recv(irq_event, bits, timeout, val) do { \
    val = xEventGroupWaitBits((EventGroupHandle_t)irq_event, bits, pdTRUE, pdFALSE, timeout); \
} while (0)
#define os_event_delete vEventGroupDelete
/*mutex*/
typedef SemaphoreHandle_t os_mutex_t;
#define os_mutex_create(name) xSemaphoreCreateMutex()
#define os_mutex_take xSemaphoreTake
#define os_mutex_give xSemaphoreGive
#define os_mutex_giveFromISR xSemaphoreGiveFromISR
#define OS_WAITING_FOREVER portMAX_DELAY
#endif
/*int related*/
#define os_interrupt_enter(...) do {} while(0)
#define os_interrupt_leave(...) do {} while(0)
/*thread related*/
#define os_thread_delay vTaskDelay
#define os_tick_get xTaskGetTickCount
#define os_tick_getFromISR xTaskGetTickCountFromISR
#define os_thread_create(name, entry, priority, stack_size, param) do { \
    TaskHandle_t xHandle = NULL; \
    xTaskCreate( \
        entry, \
        name, \
        stack_size >> 2,      /* Stack size in words, not bytes. */ \
        param,    /* Parameter passed into the task. */ \
        priority, /* Priority at which the task is created. */ \
        &xHandle);      /* Used to pass out the created task's handle. */ \
} while(0)
/*message queue related*/
typedef StaticQueue_t os_messagequeue_t;
#define os_mq_init(mq, name, buffer, msgsize, buffersize) (NULL != xQueueCreateStatic((buffersize) / (msgsize), msgsize, buffer, mq) ? 0 : 1)
#define os_mq_send(mq, msg, len) (xQueueSend((QueueHandle_t)mq, msg, portMAX_DELAY) > 0 ? 0 : 1)
#define os_mq_send_nowait(mq, msg, len) (xQueueSend((QueueHandle_t)mq, msg, 0) > 0 ? 0 : 1)
#define os_mq_recv(mq, msg, len) (xQueueReceive((QueueHandle_t)mq, msg, portMAX_DELAY) > 0 ? 0 : 1)
/*timer related*/
typedef StaticTimer_t os_timer_t;
#define OS_TIMER_TYPE_ONESHOT pdFALSE
#define OS_TIMER_TYPE_REPEATED pdTRUE
#define os_timer_delete(timer) do { \
    xTimerStop(timer, portMAX_DELAY); \
    xTimerDelete(timer, portMAX_DELAY); \
} while (0)
#define os_timer_delete_nodelay(timer) do { \
    xTimerStop((TimerHandle_t)timer, 0); \
    xTimerDelete((TimerHandle_t)timer, 0); \
} while (0)
typedef TimerHandle_t timer_cb_arg_t;
#define os_timer_init(timer, name, cb, param, ticks, type) xTimerCreateStatic( \
    name, \
    ticks, \
    type, \
    param, \
    cb, \
    timer \
)
#define os_timer_start(timer) xTimerStart((TimerHandle_t)timer, portMAX_DELAY);
#define os_timer_startFromISR(timer) xTimerStartFromISR(timer, portMAX_DELAY);
#define os_timer_data(timer) pvTimerGetTimerID(timer)

#endif
