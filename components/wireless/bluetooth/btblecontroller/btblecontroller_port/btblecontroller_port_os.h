#ifndef BTBLECONTROLLER_PORT_OS_API_H_
#define BTBLECONTROLLER_PORT_OS_API_H_

#include "FreeRTOS.h"
#include "queue.h"
 
typedef QueueHandle_t btblecontroller_QueueHandle_t;
typedef TaskFunction_t btblecontroller_TaskFunction_t;

int btblecontroller_task_new(btblecontroller_TaskFunction_t taskFunction,const char *name, int stack_size, void *arg, int prio,void *taskHandler);
void btblecontroller_task_delete(uint32_t taskHandler);
btblecontroller_QueueHandle_t btblecontroller_queue_new(uint32_t size, uint32_t max_msg);
void btblecontroller_queue_free(btblecontroller_QueueHandle_t q);
int btblecontroller_queue_send(btblecontroller_QueueHandle_t q, void *msg, uint32_t timeout);
int btblecontroller_queue_recv(btblecontroller_QueueHandle_t q, void *msg, uint32_t timeout);
int btblecontroller_queue_send_fromisr(btblecontroller_QueueHandle_t q, void *msg);
int btblecontroller_xPortIsInsideInterrupt();
void btblecontroller_TaskDelay(uint32_t ms);
#endif
