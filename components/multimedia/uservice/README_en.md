# uService

## Overview

uService  is a service mechanism that supports RPC request/application interactions and state message publishing. 
Clients can invoke services provided by uService by sending request messages and waiting for responses. 
They can also subscribe to service events to handle the service's event states. 
ServiceTask is a message distribution mechanism that leverages the multitasking capabilities of the operating system. 
Each ServiceTask creates an OS task. Multiple uservice can be registered under a single ServiceTask, 
and messages from all uservice within the same service task are processed in a first-in, first-out manner.

## Component Installation

```bash
yoc init
yoc install uservice
```


## Configuration

None.


## Interface List

The uService interfaces are as follows:


| Function | Description|
| :--- |:--------------------------------------------|
| rpc_init | 	Initialize RPC|
| rpc_put_reset | Reset RPC parameter writing|
| rpc_put_int | Write an integer type data into RPC|
| rpc_put_uint8 | 	Write a uint8 type data into RPC|
| rpc_put_double | Write a double type data into RPC|
| rpc_put_point | 	Write a pointer type data into RPC|
| rpc_put_buffer | Write a buffer into RPC|
| rpc_put_string | Write a string into RPC|
| rpc_get_reset | 	Reset RPC parameter retrieval|
| rpc_get_int | Retrieve an integer type RPC parameter|
| rpc_get_uint8 | Retrieve a uint8 type RPC parameter|
| rpc_get_double | Retrieve a double type RPC parameter|
| rpc_get_string | Retrieve a string type RPC parameter|
| rpc_get_point | Retrieve a pointer type RPC parameter|
| rpc_get_buffer | Retrieve the corresponding RPC parameter|
| rpc_reply | RPC response|
| rpc_deinit | 	Recycle RPC resources|
| uservice_new | Create a uservice|
| uservice_destroy | Release a uservice|
| uservice_call_sync | Synchronously call a uservice command|
| uservice_call_async | Asynchronously call a uservice command|
| uservice_call | Custom call a uservice command|
| uservice_lock | Lock a uservice|
| uservice_unlock | Unlock a uservice|
| uservice_process | Process|
| uservice_subscribe | Subscribe to uservice events|
| utask_new | Create a uservice task|
| utask_destroy | Release a uservice task|
| utask_join | Wait for resource recycling|
| utask_add | Add a uservice to a uservice task|
| utask_remove | Remove a uservice from a uservice task|
| utask_lock | 	Lock a uservice task|
| utask_unlock | 	Unlock a uservice task|
| utask_set_softwdt_timeout | Set watchdog timeout|
| event_service_init | 	Initialize event service|
| event_subscribe | 	Subscribe to events|
| event_unsubscribe | 	Unsubscribe from events|
| event_publish | Publish an event|
| event_publish_delay | 	Publish a delayed event|
| event_subscribe_fd | 	Subscribe to events via file descriptor|
| event_unsubscribe_fd | Unsubscribe from events via file descriptor|
| event_publish_fd | Publish an event via file descriptor|


## Interface Details

### rpc_init

`int rpc_init(rpc_t *rpc, int cmd_id, int timeout_ms);`

- Function Description:
   - Initializes the RPC system, sets the RPC command ID (cmd_id), and configures the timeout duration (timeout_ms) for command invocations in milliseconds.

- Parameters:
   - `rpc`: rpc.
   - `cmd_id`: RPC command identifier..
   - `timeout_ms`: Timeout duration for command execution: 0 for asynchronous (returns immediately), MSP_WAIT_FOREVER for synchronous (waits indefinitely), 
   or a positive value (milliseconds) for timeout-based calls (exits if server does not respond within the specified time).
  
  
- Return Values:
   - 0: Success .
   - ENOMEM: Out of memory.
   - EINVAL: Invalid parameter.

#### rpc_t

| Member | Type | Description |
| :--- | :--- | :--- |
| srv | uservice_t | uservice |
| cmd_id | uint32_t | RPC sequence number |
| data | rpc_buffer_t | Buffer storing RPC-related parameters |


### rpc_put_reset

`void rpc_put_reset(rpc_t *rpc);`

- Function Description:
   -Clear the internal parameters of RPC. RPC parameters are used for both the input parameters passed from the caller to the server and the parameters returned from the server to the caller.
    When writing server-side programs, after processing the RPC parameters and needing to return values to the caller, call this function to clear the parameter area first, and then use the rpc_put_xxx functions to write the return values to RPC.
- Parameters:
   - `rpc`: rpc.

- Return Values:
   - None.


### rpc_put_int

`int rpc_put_int(rpc_t *rpc, int v);`

- Function Description:
   - Write parameters to the RPC parameter area. Multiple parameters can be written sequentially, and they will be stored in the order they are written.
- Parameters:
   - `rpc`: rpc.
   - `v`: Integer data type.

- Return Values:
   - 0: Success.
   - ENOMEM: Insufficient memory.

### rpc_put_uint8

`int rpc_put_uint8(rpc_t *rpc, uint8_t v);`

- Function Description:
   - Write parameters to the RPC parameter area. Multiple parameters can be written sequentially, and they will be stored in the order they are written.

- Parameters:
   - `rpc`: rpc.
   - `v`: uint8_t data type.

- Return Values:
   - 0: Success.
   - ENOMEM: Insufficient memory.

### rpc_put_double

`int rpc_put_double(rpc_t *rpc, double v);`

- Function Description:
   - Write parameters to the RPC parameter area. Multiple parameters can be written sequentially, and they will be stored in the order they are written.

- Parameters:
   - `rpc`: rpc.
   - `v`: double data type.

- Return Values:
   - 0: Success.
   - ENOMEM: Insufficient memory.

### rpc_put_point

`int rpc_put_point(rpc_t *rpc, const void *v);`

- Function Description:
   - Write parameters to the RPC parameter area. Multiple parameters can be written sequentially, and they will be stored in the order they are written.

- Parameters:
   - `rpc`: rpc.
   - `v`: point data type.

- Return Values:
   - 0: Success.
   - ENOMEM: Insufficient memory.

### rpc_put_buffer

`int rpc_put_buffer(rpc_t *rpc, const void *data, size_t size);`

- Function Description:
   - Write parameters to the RPC parameter area. Multiple parameters can be written sequentially, and they will be stored in the order they are written.

- Parameters:
   - `rpc`: rpc.
   - `data`: array data type.
   - `size`: data length.

- Return Values:
   - 0: Success.
   - ENOMEM: Insufficient memory.

### rpc_put_string

`int rpc_put_string(rpc_t *rpc, char *str);`

- Function Description:
   - Write parameters to the RPC parameter area. Multiple parameters can be written sequentially, and they will be stored in the order they are written.

- Parameters:
   - `rpc`: rpc.
   - `str`: string data type.

- Return Values:
   - 0: Success.
   - ENOMEM: Insufficient memory.


### rpc_get_reset

`void rpc_get_reset(rpc_t *rpc);`

- Function Description:
   - The parameters in the RPC parameter area are read through a set of rpc_get_xxx functions. 
     Each call to a reading function retrieves one parameter, and the sequence number increments accordingly. 
     If it is necessary to re-read the parameters from the beginning, the rpc_get_reset function is used to move the parameter sequence number back to the starting position.

- Parameters:
   - `rpc`: rpc.

- Return Values:
   - None.


### rpc_get_int

`int rpc_get_int(rpc_t *rpc);`

- Function Description:
   - When reading parameters from the parameter area, the order of reading must be consistent with the order in which the parameters were written. 
     Parameters in the parameter area are read sequentially: each call to the rpc_get_int function reads the next parameter in sequence. 
     Mismatched parameter types during reading will cause exceptions.
- Parameters:
   - `rpc`: rpc.

- Return Values:
   - Return an integer parameter value.

### rpc_get_uint8

`uint8_t rpc_get_uint8(rpc_t *rpc);`

- Function Description:
   - When reading parameters from the parameter area, you must call the corresponding reading functions in the same order as the parameters were written. 
     Parameters in the parameter area are read sequentially. Each call to the rpc_get_uint8 function reads the next parameter in the sequence as an 8-bit unsigned integer (uint8_t). 
     Mismatched parameter types during reading will cause exceptions.

- Parameters:
   - `rpc`: rpc.

- Return Values:
   - Return a uint8 Parameter Value.

### rpc_get_double

`double rpc_get_double(rpc_t *rpc);`

- Function Description:
   - When reading parameters from the parameter area, you must call the corresponding reading functions in the same order as the parameters were written. 
     Parameters in the parameter area are read sequentially. Each call to the rpc_get_double function reads the next parameter in the sequence as an 8-bit unsigned integer (uint8_t). 
     Mismatched parameter types during reading will cause exceptions.
  
- Parameters:
   - `rpc`: rpc.

- Return Values:
   - Return a double Parameter Value.

### rpc_get_string

`char *rpc_get_string(rpc_t *rpc);`

- Function Description:
   - When reading parameters from the parameter area, you must call the corresponding reading functions in the same order as the parameters were written. 
     Parameters in the parameter area are read sequentially. Each call to the rpc_get_string function reads the next parameter in the sequence as an 8-bit unsigned integer (uint8_t). 
     Mismatched parameter types during reading will cause exceptions.
  
- Parameters:
   - `rpc`: rpc.

- Return Values:
   - Return a char* Parameter Value.

### rpc_get_point

`void *rpc_get_point(rpc_t *rpc);`

- Function Description:
   - When reading parameters from the parameter area, you must call the corresponding reading functions in the same order as the parameters were written. 
     Parameters in the parameter area are read sequentially. Each call to the rpc_get_point function reads the next parameter in the sequence as an 8-bit unsigned integer (uint8_t). 
     Mismatched parameter types during reading will cause exceptions.
  
- Parameters:
   - `rpc`: rpc.

- Return Values:
   - Return a void* Parameter Value.

### rpc_get_buffer

`void *rpc_get_buffer(rpc_t *rpc, int *count);`

- Function Description:
   - When reading parameters from the parameter area, you must call the corresponding reading functions in the same order as the parameters were written. 
     Parameters in the parameter area are read sequentially. Each call to the rpc_get_buffer function reads the next parameter in the sequence as an 8-bit unsigned integer (uint8_t). 
     Mismatched parameter types during reading will cause exceptions.
  
- Parameters:
   - `rpc`: rpc.

- Return Values:
   - Return a void* Parameter Value.

### rpc_reply

`void rpc_reply(rpc_t *rpc);`

- Function Description:
   - This function is called when writing the server. After the server processes an RPC, 
   this function must be invoked to complete the RPC task processing..

- Parameters:
   - `rpc`: rpc.

- Return Values:
   - None.

### rpc_deinit

`void rpc_deinit(rpc_t *rpc);`

- Function Description:
   - This function is executed on the caller side. When the caller uses the uservice_call function to invoke an RPC, 
  after processing the RPC return values, the rpc_deinit function needs to be called to reclaim the RPC resources.
- Parameters:
   - `rpc`: rpc.

- Return Values:
   - None.


### uservice_new

`uservice_t *uservice_new(const char *name, process_t process_rpc, void *context);`

- Function Description:
   - Create a uservice based on the specified uservice name, context, and maximum command ID. 
  If the creation is successful, a uservice_t pointer is returned; if it fails, NULL is returned.

- Parameters:
   - `name`: The service name.
   - `process_rpc`: The service's remote call processing function.
   - `context`: The service's custom context, which can be used by users when writing uservice processing functions.

- Return Values:
   - Returns a uservice_t pointer if creation is successful; returns NULL if failed.

#### process_t

`typedef int (*process_t)(void *context, rpc_t *rpc);`

- Function Description:
   - The main processing function for remote procedure calls (RPC) in uservice. 
  During uservice development, the processing time of process_rpc should be minimized as much as possible.

- Parameters:
   - `context`: The context passed in when creating the uservice.
   - `rpc`：The remote procedure call object.

- Return Values:
   - 0: Success.
   - -1: Failure.


### uservice_destroy

`void uservice_destroy(uservice_t *srv);`

- Function Description:
   - Release all resources occupied by the uservice srv. Before releasing srv, it is necessary to ensure that the uService is removed from all uservice_task instances..

- Parameters:
   - `srv`: uservice.

- Return Values:
   - None.

#### uservice_t

| Member | Type | Description |
| :--- | :--- | :--- |
| name | const char * | Name |
| context | void | 	Context handle |
| process_rpc | process_t | Service remote call processing function |
| task | utask_t | Task |
| next | msp_slist_t | 	Pointer field |


### uservice_call_sync

`int uservice_call_sync(uservice_t *srv, int cmd, void *param, void *resp, size_t resp_size);`

- Function Description:
   - Send a synchronous execution command to the uservice.

- Parameters:
   - `srv`: The uservice.
   - `cmd`：The command number to be executed.
   - `param`: The parameters of the command.
   - `resp`：The return values of the command.
   - `resp_size`: The size of the return values buffer.
- Return Values：
   - 0: Success.
   - -1: Failure.


### uservice_call_async

`int uservice_call_async(uservice_t *srv, int cmd, void *param, size_t param_size);`

- Function Description:
   - Send an asynchronous execution command to the uservice.

- Parameters:
   - `srv`: The uservice.
   - `cmd`：The command number to be executed.
   - `param`: The parameters of the command.
   - `param_size`: The size of the parameters buffer.

- Return Values：
   - 0: Success.
   - -1: Failure.


### uservice_call

`int uservice_call(uservice_t *srv, rpc_t *rpc);`

- Function Description:
   - Use custom RPC to implement uservice command invocations. This command supports synchronous, asynchronous, and timeout-based invocations via the rpc_t structure. 
  You can use rpc_put_xxx functions to pass multiple parameters into the RPC. The server can also return multiple values via the RPC, enabling complex parameter transmission and invocation.
- Parameters:
   - `srv`: The uservice.
   - `rpc`: The RPC instruction being called.

- Return Values：
   - 0: Success.
   - -1: Failure.


### uservice_lock

`void uservice_lock(uservice_t *srv);`

- Function Description:
   - Lock the uservice.

- Parameters:
   - `srv`:  The uservice.

- Return Values：
   - None.


### uservice_unlock

`void uservice_unlock(uservice_t *srv);`

- Function Description:
   - Unlock the uservice.

- Parameters:
   - `srv`: uservice.

- Return Values：
   - None.


### uservice_process

`int uservice_process(void *context, rpc_t *rpc, const rpc_process_t rpcs[]);`

- Function Description:
   - Unlock the uservice.

- Parameters:
   - `context`: uservice.
   - `rpc`: rpc.
   - `rpcs`: uservice.

- Return Values：
   - None.

#### rpc_process_t

| Member | Type | Description |
| :--- | :--- | :--- |
| cmd_id | int | Command ID |
| process_rpc | process | Service remote call processing function |


### uservice_subscribe

`void uservice_subscribe(uservice_t *srv, uint32_t event_id);`

- Function Description:
   - This function is used in uservices. Calling this function subscribes to an event and sends the event to the RPC task queue of the uservice.
  The cmd_id of the RPC is equal to the event_id, and the data parameter of the event can be obtained through rpc_get_pointer.
- Parameters:
   - `srv`: uservice.
   - `event_id`: event id

- Return Values：
   - None.


### utask_new

`utask_t *utask_new(const char *name, size_t stack_size, int queue_length, int prio);`

- Function Description:
   - Create a uservice.

- Parameters:
   - `name`: Name of the utask
   - `stack_size`: Stack size
   - `queue_length`: Number of queues
   - `prio`: Priority

- Return Values：
   - not NULL: Success.
   - NULL: Failure.

#### utask_t

| Member | Type | Description |
| :--- | :--- | :--- |
| task | msp_task_t | Task |
| running | int8_t | Running status |
| queue_count | uint8_t | Queue count |
| queue_max_used | uint8_t | Maximum queue length used |
| rpc_reclist | msp_slist_t | Pointer field |
| queue_buffer | void* | Buffer |
| queue | msp_queue_t | Queue |
| mutex | msp_mutex_t | Mutex |
| current_rpc | rpc_t* | Current rpc  |
| running_wait | msp_sem_t | Semaphore |
| uservice_lists | msp_slist_t | Linked list  |
| rpc_buffer_gc_cache | msp_slist_t | Linked list  |
| node | msp_slist_t | Linked list  |


### utask_destroy

`void utask_destroy(utask_t *task);`

- Function Description:
   - Release the uservice.

- Parameters:
   - `task`: Uservice task handle.

- Return Values：
   - None.


### utask_join

`void utask_join(utask_t *task);`

- Function Description:
   - Wait for the uservice task to exit and reclaim the occupied resources.

- Parameters:
   - `task`: Uservice task handle.

- Return Values：
   - None.


### utask_add

`void utask_add(utask_t *task, uservice_t *srv);`

- Function Description:
   - Add the uservice to the uservice task.

- Parameters:
   - `task`: Uservice task handle.
   - `srv`: uservice.

- Return Values：
   - None.


### utask_remove

`void utask_remove(utask_t *task, uservice_t *srv);`

- Function Description:
   - Remove the uservice from the uservice task.

- Parameters:
   - `task`: Uservice task handle.
   - `srv`: uservice.

- Return Values：
   - None.


### utask_lock

`void utask_lock(utask_t *task);`

- Function Description:
   - Lock the uservice task.

- Parameters:
   - `task`: Uservice task handle.

- Return Values：
   - None.


### utask_unlock

`void utask_unlock(utask_t *task);`

- Function Description:
   - Unlock the uservice task.

- Parameters:
   - `task`: Uservice task handle.

- Return Values：
   - None.


### utask_set_softwdt_timeout

`void utask_set_softwdt_timeout(int ms);`

- Function Description:
   - If the uservice task fails to unlock, you can set a watchdog timeout.

- Parameters:
   - `ms`: Timeout duration in milliseconds.

- Return Values：
   - None.


### event_service_init

`int  event_service_init(utask_t *task);`

- Function Description:
   - Event service initialization.

- Parameters:
   - `task`: Uservice task handle.

- Return Values:
   - 0: Success.
   - -1: Failure.


### event_subscribe

`void event_subscribe(uint32_t event_id, event_callback_t cb, void *context);`

- Function Description:
   - Subscribe to a normal event.

- Parameters:
   - `event_id`: Event number.
   - `cb`: Event handling function. This parameter cannot be NULL; otherwise, an exception will be triggered.
   - `context`: User data, which will be passed into the context parameter of cb.

- Return Values:
   - None.

#### event_callback_t

`typedef void (*event_callback_t)(uint32_t event_id, const void *data, void *context);`

- Function Description:
   - Event handling function type.

- Parameters:
   - `event_id`: Event number.
   - `data`: array data type.
   - `context`: User data, which will be passed into the context parameter of the callback (cb).

- Return Values:
   - None.


### event_unsubscribe

`void event_unsubscribe(uint32_t event_id, event_callback_t cb, void *context);`

- Function Description:
   - Unsubscribe from an event. To unsubscribe, the parameters passed in must be consistent with those used when subscribing.

- Parameters:
   - `event_id`: Event number.
   - `cb`: Event handling function. This parameter cannot be NULL; otherwise, an exception will be triggered.
   - `context`: User data, which will be passed into the context parameter of cb.

- Return Values:
   - None.


### event_publish

`void event_publish(uint32_t event_id, void *data);`

- Function Description:
   - Publish an event.

- Parameters:
   - `event_id`: Event number.
   - `data`: Data attached to the event, which will be passed to the data parameter of the subscriber's callback function. 
  Since events are processed asynchronously, the user must pay attention to the scope of data to ensure that subscribers can access this data normally.
- Return Values:
   - None.


### event_publish_delay

`void event_publish_delay(uint32_t event_id, void *data, int timeout_ms);`

- Function Description:
   - Publish a delayed event.

- Parameters:
   - `event_id`: Event number.
   - `data`: Data attached to the event, which will be passed to the data parameter of the subscriber's callback function. 
  Since events are processed asynchronously, the user must pay attention to the scope of data to ensure that subscribers can access this data normally.
   - `timeout_ms`: Timeout duration, unit: ms.

- Return Values:
   - None.


### event_subscribe_fd

`void event_subscribe_fd(uint32_t fd, event_callback_t cb, void *context);`

- Function Description:
   - Subscribe to the readable event of device (network) handle data.

- Parameters:
   - `fd`: Device/network handle.
   - `cb`: Event handling function. This parameter cannot be NULL; otherwise, an exception will be triggered.
   - `context`: User data, which will be passed into the context parameter of cb.
- Return Values:
   - None.


### event_unsubscribe_fd

`void event_unsubscribe_fd(uint32_t fd, event_callback_t cb, void *context);`


- Function Description:
   - Unsubscribe from the event. To unsubscribe, the parameters passed in must be consistent with those used when subscribing.

- Parameters:
   - `fd`: Device/network handle.
   - `cb`: Event handling function. This parameter cannot be NULL; otherwise, an exception will be triggered.
   - `context`: User data, which will be passed into the context parameter of cb.

- Return Values:
   - None.


### event_publish_fd

`void event_publish_fd(uint32_t fd, void *data, int sync);`

- Function Description:
   - Publish an event.

- Parameters:
   - `fd`: Device/network handle.
   - `data`: Data attached to the event, which will be passed to the data parameter of the subscriber's callback function. 
  Since events are processed asynchronously, the user must be cautious about the scope of data to ensure that subscribers can access this data normally.
   - `sync`: Synchronization flag, 1: synchronous, 0: asynchronous.

- Return Values:
   - None.


## Example

### Service Interface and Event Definition

```c
#ifndef DEMO_SERVICE_H
#define DEMO_SERVICE_H

#define EVENT_A 0x1000
#define EVENT_B 0x1001
#define EVENT_C 0x1002

int demo_add(int a, int b);

#endif
```

### Service Implementation

```c
#include <uservice/uservice.h>

struct demo_uservice {
    uservice_t *srv;
    int a, b, c;
    rpc_t *current_cmd;
    rpc_t *current_oob;
};

/* cmd list */
enum DEMO_CMD {
    CMD_1 = 0,
    CMD_2 = 1,
    CMD_RECV,

    CMD_DATA = 10,
    CMD_MAX
};

/* Event ID */
#define EVENT_A 0x1000
#define EVENT_B 0x1001
#define EVENT_C 0x1002

struct cmd_param {
    int a, b, c;
};

/* uservice Handler Function*/
static int process_rpc(void *context, rpc_t *rpc)
{
    struct demo_uservice *d = (struct demo_uservice *)context;

    if (rpc->cmd_id < CMD_DATA) {
        d->current_cmd = rpc;
    }

    switch (rpc->cmd_id) {
        case CMD_RECV:
            // d->current_cmd = rpc;
            break;

        case CMD_1: {
            /* Addition CMD processing. */
            struct cmd_param *p = (struct cmd_param *)rpc_get_point(rpc);

            d->c =  p->a + p->b;

            /* Publish the EVENT_A event */
            event_publish(EVENT_A, (void *)d->c);
            break;
        }

        case CMD_DATA:
            if (d->current_cmd != NULL) {
                rpc_reply(d->current_cmd);
                d->current_cmd = NULL;
                event_publish(EVENT_A, (void *)d->c);
            }
			/* RPC Response */
            rpc_reply(rpc);
            break;

        default:
            /* RPC Response */
            rpc_reply(rpc);
            break;
    }

    return 0;
}

static struct demo_uservice demo;
void demo_service_init()
{
    static utask_t *srv;
	/* Create a uservice */
    srv = utask_new("demo_srv", 2 * 1024, QUEUE_MSG_COUNT, MSP_DEFAULT_APP_PRI);
	/* Create utask */
    demo.srv = uservice_new("demo", process_rpc, &demo);

    /* Integrate the uservice into utask */
    utask_add(srv, demo.srv);
}

void demo_add(int a, int b)
{
    struct cmd_param cmd;
    cmd.a = a;
    cmd.b = b;
    int c;

    /* uservice Synchronous Invocation CMD_1 */
    uservice_call_sync(demo.srv, CMD_1, &cmd, &c, sizeof(int));
}
```

### Service Invocation

```c
int demo_subscribe(uint16_t event_id, event_callback_t cb, void *context)
{
    return uservice_subscribe(demo.srv, event_id, cb, context);
}

static void event_a_callback(uint16_t event_id, const void *data, void *context)
{
    printf("%d\n", (int)data);
}

void demo_test()
{
    demo_service_init();
    demo_subscribe(EVENT_A, event_a_callback, NULL);
    demo_add(1, 4);
}
```


## Diagnostic Error Code
None.


## Runtime Resources
None.


## Dependency Resources
  - minilibc
  - aos


## Component Reference
None.

