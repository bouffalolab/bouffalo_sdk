#include "btblecontroller_port_os.h"
#include "FreeRTOS.h"

__attribute__((weak)) int btblecontroller_task_new(btblecontroller_TaskFunction_t taskFunction,const char *name, int stack_size, void *arg, int prio,void *taskHandler)
{
    return xTaskCreate( taskFunction, name, stack_size, arg, prio,(TaskHandle_t * const)taskHandler);
}

__attribute__((weak)) void btblecontroller_task_delete(uint32_t taskHandler)
{
    vTaskDelete((TaskHandle_t)taskHandler);
}

__attribute__((weak)) btblecontroller_QueueHandle_t btblecontroller_queue_new(uint32_t size, uint32_t max_msg)
{
    btblecontroller_QueueHandle_t q;
    q = xQueueCreate(size, max_msg);
    return q;
}

__attribute__((weak)) void btblecontroller_queue_free(btblecontroller_QueueHandle_t q)
{
    vQueueDelete(q);
}

__attribute__((weak)) int btblecontroller_queue_send(btblecontroller_QueueHandle_t q, void *msg, uint32_t timeout)
{
    return xQueueSend(q, msg, timeout);
}

__attribute__((weak)) int btblecontroller_queue_recv(btblecontroller_QueueHandle_t q, void *msg, uint32_t timeout)
{
    return xQueueReceive(q, msg, timeout);
}

__attribute__((weak)) int btblecontroller_queue_send_fromisr(btblecontroller_QueueHandle_t q, void *msg)
{
    BaseType_t xHigherPriorityTaskWoken;
    int ret;

    xHigherPriorityTaskWoken = pdFALSE;

    ret = xQueueSendFromISR(q, msg, &xHigherPriorityTaskWoken);
    if(xHigherPriorityTaskWoken == pdTRUE)
    {
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
    return ret;
}

__attribute__((weak)) int btblecontroller_xPortIsInsideInterrupt()
{
    return xPortIsInsideInterrupt();
}

__attribute__((weak)) void btblecontroller_TaskDelay(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}