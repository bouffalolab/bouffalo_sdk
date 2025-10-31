#include "btblecontroller_port_os.h"
#include "FreeRTOS.h"

__attribute__((weak)) int btblecontroller_task_new(btblecontroller_TaskFunction_t taskFunction,const char *name, int stack_size, void *arg, int prio,void *taskHandler)
{
    return xTaskCreate( (TaskFunction_t)taskFunction, name, stack_size, arg, prio,(TaskHandle_t * const)taskHandler);
}

__attribute__((weak)) void btblecontroller_task_delete(uint32_t taskHandler)
{
    vTaskDelete((TaskHandle_t)taskHandler);
}

__attribute__((weak)) int btblecontroller_queue_new(uint32_t size, uint32_t max_msg,btblecontroller_QueueHandle_t *queue)
{
    
    *queue= xQueueCreate(size, max_msg);
    if ( *queue == NULL )
       return -1;
    return 0;
}

__attribute__((weak)) void btblecontroller_queue_free(btblecontroller_QueueHandle_t q)
{
    vQueueDelete(q);
}

__attribute__((weak)) int btblecontroller_queue_send(btblecontroller_QueueHandle_t q, void *msg, uint32_t size, uint32_t timeout)
{
    return xQueueSend(q, msg, timeout);
}

__attribute__((weak)) int btblecontroller_queue_recv(btblecontroller_QueueHandle_t q, void *msg, uint32_t timeout)
{
    return xQueueReceive(q, msg, timeout);
}

__attribute__((weak)) int btblecontroller_queue_send_from_isr(btblecontroller_QueueHandle_t q, void *msg, uint32_t size)
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

__attribute__((weak)) int btblecontroller_xport_is_inside_interrupt(void)
{
    return xPortIsInsideInterrupt();
}

__attribute__((weak)) void btblecontroller_task_delay(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

__attribute__((weak)) void * btblecontroller_task_get_current_task_handle(void)
{
    return xTaskGetCurrentTaskHandle();
}

__attribute__((weak)) void *btblecontroller_malloc(size_t xWantedSize)
{
    return pvPortMalloc(xWantedSize);
}

__attribute__((weak)) void btblecontroller_free(void *buf)
{
    vPortFree(buf);
}

