#include "btblecontroller_port_os.h"
#include "FreeRTOS.h"

#define BTBLE_OS_PORT_ASSERT(x)                                                           \
    if ((x) == 0) {                                                                       \
        printf("[BTBLE ASSERT] %s in function %s\r\n", (const char *)(#x), __FUNCTION__); \
        __asm__ volatile ("ebreak"); \
    }

__attribute__((weak)) int btblecontroller_task_new(btblecontroller_TaskFunction_t taskFunction, const char *name, int stack_size, void *arg, int prio, void *taskHandler)
{
    return xTaskCreate((TaskFunction_t)taskFunction, name, stack_size, arg, prio, (TaskHandle_t *const)taskHandler);
}

__attribute__((weak)) void btblecontroller_task_delete(uint32_t taskHandler)
{
    vTaskDelete((TaskHandle_t)taskHandler);
}

__attribute__((weak)) int btblecontroller_queue_new(uint32_t size, uint32_t max_msg, btblecontroller_QueueHandle_t *queue)
{
    *queue = xQueueCreate(size, max_msg);
    BTBLE_OS_PORT_ASSERT(*queue);
    if (*queue == NULL)
        return -1;
    return 0;
}

__attribute__((weak)) void btblecontroller_queue_free(btblecontroller_QueueHandle_t q)
{
    BTBLE_OS_PORT_ASSERT(q);
    vQueueDelete(q);
}

__attribute__((weak)) int btblecontroller_queue_send(btblecontroller_QueueHandle_t q, void *msg, uint32_t size, uint32_t timeout)
{
    BTBLE_OS_PORT_ASSERT(q);
    return xQueueSend(q, msg, timeout);
}

__attribute__((weak)) int btblecontroller_queue_recv(btblecontroller_QueueHandle_t q, void *msg, uint32_t timeout)
{
    BTBLE_OS_PORT_ASSERT(q);
    return xQueueReceive(q, msg, timeout);
}

__attribute__((weak)) int btblecontroller_queue_send_from_isr(btblecontroller_QueueHandle_t q, void *msg, uint32_t size)
{
    BaseType_t xHigherPriorityTaskWoken;
    int ret;

    xHigherPriorityTaskWoken = pdFALSE;

    BTBLE_OS_PORT_ASSERT(q);

    ret = xQueueSendFromISR(q, msg, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    return ret;
}

__attribute__((weak)) int btblecontroller_xport_is_inside_interrupt()
{
    return xPortIsInsideInterrupt();
}

__attribute__((weak)) void btblecontroller_task_delay(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}