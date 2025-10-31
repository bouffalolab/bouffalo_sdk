#include "btblecontroller_port_os.h"
#include <aos/kernel.h>
#include <aos/aos.h>
#include <k_api.h>

__attribute__((weak)) int btblecontroller_task_new(btblecontroller_TaskFunction_t taskFunction,const char *name, int stack_size, void *arg, int prio,void *taskHandler)
{
    return aos_task_new_ext((aos_task_t*)taskHandler, name, taskFunction, arg, stack_size, prio);
}

__attribute__((weak)) void btblecontroller_task_delete(uint32_t taskHandler)
{ 
    void* hdl = ((aos_task_t*)taskHandler)->hdl;
    krhino_task_dyn_del((ktask_t *)hdl);
    hdl = NULL;
}

__attribute__((weak)) int btblecontroller_queue_new(uint32_t size, uint32_t max_msg,btblecontroller_QueueHandle_t *queue)
{
    uint32_t buf_size = size*max_msg;
    void *msg_start = (void*)aos_malloc(buf_size);
    aos_queue_new(queue, msg_start, buf_size, max_msg);
    if ( *queue == NULL )
       return -1;
    return 0;
}

__attribute__((weak)) void btblecontroller_queue_free(btblecontroller_QueueHandle_t q)
{
    kbuf_queue_t *buf_queue = q.hdl;
    aos_free(buf_queue->buf);
    aos_queue_free(&q);
}

__attribute__((weak)) int btblecontroller_queue_send(btblecontroller_QueueHandle_t q, void *msg, uint32_t size, uint32_t timeout)
{
    int status = aos_queue_send(q, msg, size);
    if(status == 0)
    {
       return 1;
    }
    else
    {
       return 0;
    }
}

__attribute__((weak)) int btblecontroller_queue_recv(btblecontroller_QueueHandle_t q, void *msg, uint32_t timeout)
{
    unsigned int len;
    unsigned int delay = 0xffffffff;
    int status = aos_queue_recv(&q,delay,msg,&len);
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
    aos_queue_send(q, msg, size);
}

__attribute__((weak)) int btblecontroller_xport_is_inside_interrupt()
{
    return 0;
}

__attribute__((weak)) void btblecontroller_task_delay(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

__attribute__((weak)) void *btblecontroller_malloc(size_t xWantedSize)
{
    return aos_malloc(xWantedSize);
}

__attribute__((weak)) void btblecontroller_free(void *buf)
{
    return aos_free(buf);
}

