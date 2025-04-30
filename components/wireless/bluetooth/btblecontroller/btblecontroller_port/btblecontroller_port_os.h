#ifndef BTBLECONTROLLER_PORT_OS_API_H_
#define BTBLECONTROLLER_PORT_OS_API_H_

#include "FreeRTOS.h"
#include "queue.h"

#if defined(CFG_FREERTOS)
typedef QueueHandle_t btblecontroller_QueueHandle_t;
#elif defined(CFG_OPENHARMONY)
typedef rtos_queue btblecontroller_QueueHandle_t;
#elif defined(CFG_AOS)
typedef aos_queue_t btblecontroller_QueueHandle_t;
#endif

typedef void (*btblecontroller_TaskFunction_t)(void *);

int btblecontroller_task_new(btblecontroller_TaskFunction_t taskFunction,const char *name, int stack_size, void *arg, int prio,void *taskHandler);
void btblecontroller_task_delete(uint32_t taskHandler);
int btblecontroller_queue_new(uint32_t size, uint32_t max_msg,btblecontroller_QueueHandle_t *queue);
void btblecontroller_queue_free(btblecontroller_QueueHandle_t q);
int btblecontroller_queue_send(btblecontroller_QueueHandle_t q, void *msg, uint32_t size, uint32_t timeout);
int btblecontroller_queue_recv(btblecontroller_QueueHandle_t q, void *msg, uint32_t timeout);
int btblecontroller_queue_send_from_isr(btblecontroller_QueueHandle_t q, void *msg, uint32_t size);
int btblecontroller_xport_is_inside_interrupt(void);
void btblecontroller_task_delay(uint32_t ms);
void *btblecontroller_task_get_current_task_handle(void);
void *btblecontroller_malloc(size_t xWantedSize);
void btblecontroller_free(void *buf);
#endif
