#include "btblecontroller_port_os.h"
#include "FreeRTOS.h"
#include "rtos_al.h"
#include "task.h"

__attribute__((weak)) int btblecontroller_task_new(btblecontroller_TaskFunction_t taskFunction,const char *name, int stack_size, void *arg, int prio,void *taskHandler)
{
    return rtos_task_create( (rtos_task_fct)taskFunction, name, 0, stack_size, arg, prio,(rtos_task_handle * const)taskHandler);
}

__attribute__((weak)) void btblecontroller_task_delete(uint32_t taskHandler)
{
    rtos_task_delete((TaskHandle_t)taskHandler);
}

__attribute__((weak)) int btblecontroller_queue_new(uint32_t size, uint32_t max_msg,btblecontroller_QueueHandle_t *queue)
{
    return rtos_queue_create(max_msg,size,queue);
}

__attribute__((weak)) void btblecontroller_queue_free(btblecontroller_QueueHandle_t q)
{
    rtos_queue_delete(q);
}

__attribute__((weak)) int btblecontroller_queue_send(btblecontroller_QueueHandle_t q, void *msg, uint32_t size, uint32_t timeout)
{
    int status = rtos_queue_write(q, msg, timeout,false);
    if(status != 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

__attribute__((weak)) int btblecontroller_queue_recv(btblecontroller_QueueHandle_t q, void *msg, uint32_t timeout)
{
    int status = rtos_queue_read(q, msg, timeout,false);
    if(status == 0)
    {
       return 1;
    }
    else
    {
       return 0;
    }
}

__attribute__((weak)) int btblecontroller_queue_send_from_isr(btblecontroller_QueueHandle_t q, void *msg, uint32_t size)
{
    rtos_queue_write(q, msg, 0, true);
    return 0;
}

__attribute__((weak)) int btblecontroller_xport_is_inside_interrupt()
{
    return HalIsIntActive();
}

__attribute__((weak)) void btblecontroller_task_delay(uint32_t ms)
{
    rtos_task_suspend(ms);
}