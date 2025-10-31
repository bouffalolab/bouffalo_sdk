# 微服务（uService）

## 概述

uService (微服务) 是一种支持RPC请求/应用的交互，并支持状态消息发布的一种服务机制，客户端可以通过发送请求消息并待回复的方式调用uService(微服务)提供的服务，也可以通过订阅服务的事件，来处理服务的事件状态。

serviceTask (服务任务)是利用操作系统的多任务系统，实现消息的分发机制，一个 serviceTask中创建一个OS 的Task。一个serviceTask 下可以注册多个微服务，同一个服务任务下的所有微服务的消息采用先进先处理的顺序执行。


## 组件安装

```bash
yoc init
yoc install uservice
```


## 配置

无。


## 接口列表

uService接口如下所示：


| 函数 | 说明 |
| :--- | :--- |
| rpc_init | 初始化rpc |
| rpc_put_reset | 复位写入rpc参数 |
| rpc_put_int | 把int类型数据写入rpc |
| rpc_put_uint8 | 把uint8类型数据写入rpc |
| rpc_put_double | 把double类型数据写入rpc |
| rpc_put_point | 把指针类型数据写入rpc |
| rpc_put_buffer | 把缓存写入rpc |
| rpc_put_string | 把字符串写入rpc |
| rpc_get_reset | 复位获取rpc参数 |
| rpc_get_int | 获取int类型的rpc参数 |
| rpc_get_uint8 | 获取uint8类型的rpc参数 |
| rpc_get_double | 获取double类型的rpc参数 |
| rpc_get_string | 获取字符串类型的rpc参数 |
| rpc_get_point | 获取指针类型的rpc参数 |
| rpc_get_buffer | 获取rpc对应的参数 |
| rpc_reply | rpc应答 |
| rpc_deinit | rpc回收 |
| uservice_new | 创建微服务 |
| uservice_destroy | 释放微服务 |
| uservice_call_sync | 同步调用微服务命令 |
| uservice_call_async | 异步调用微服务命令 |
| uservice_call | 自定义调用微服务命令 |
| uservice_lock | 微服务上锁 |
| uservice_unlock | 微服务解锁 |
| uservice_process | 进程 |
| uservice_subscribe | 微服务事件订阅 |
| utask_new | 创建微服务任务 |
| utask_destroy | 释放微服务任务 |
| utask_join | 等待资源回收 |
| utask_add | 向微服务任务中添加微服务 |
| utask_remove | 向微服务任务中删除微服务 |
| utask_lock | 微服务任务上锁 |
| utask_unlock | 微服务任务解锁 |
| utask_set_softwdt_timeout | 设置看门狗超时时间 |
| event_service_init | 事件服务初始化 |
| event_subscribe | 事件订阅 |
| event_unsubscribe | 取消事件订阅 |
| event_publish | 事件发布 |
| event_publish_delay | 事件发布延时 |
| event_subscribe_fd | 事件订阅 |
| event_unsubscribe_fd | 事件取消订阅 |
| event_publish_fd | 事件发布 |


## 接口详细说明

### rpc_init

`int rpc_init(rpc_t *rpc, int cmd_id, int timeout_ms);`

- 功能描述:
   - 初始化 RPC，设置 RPC 的命令号(cmd_id), 命令调用的超时时间(timeout_ms)，单位毫秒。

- 参数:
   - `rpc`: rpc。
   - `cmd_id`: RPC 的命令号。
   - `timeout_ms`: 命令调用等待超时时间，0时为异步调用，调用者立即返回，等于 MSP_WAIT_FOREVER为同步调用，等到服务端命令执行完，当 timeout_ms 大于0时，为超时调用，调用者指定的时间内等待服务端返回，如果服务器未返回，则超时退出。

- 返回值:
   - 0: 成功 。
   - ENOMEM: 内存不足。
   - EINVAL: 无效参数。

#### rpc_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| srv | uservice_t | 微服务 |
| cmd_id | uint32_t | RPC 序号 |
| data | rpc_buffer_t | 存RPC相关参数的buffer |


### rpc_put_reset

`void rpc_put_reset(rpc_t *rpc);`

- 功能描述:
   -清空 rpc 内部的参数。rpc 参数是调用者向服务端传递输入参数，也是服务端向调用者反回参数。编写服务端程序时，在处理完成 rpc 的参数，需要向调用者返回值时，通过调用该函数，清空参数区，然后通过 rpc_put_xxx 函数向 rpc 写入返回值。

- 参数:
   - `rpc`: rpc。

- 返回值:
   - 无。


### rpc_put_int

`int rpc_put_int(rpc_t *rpc, int v);`

- 功能描述:
   - 向 rpc 参数区写入参数， rpc 的参数可以写入多个参数，根据写入的顺序存放。

- 参数:
   - `rpc`: rpc。
   - `v`: int类型的数据。

- 返回值:
   - 0: 成功 。
   - ENOMEM: 内存不足。

### rpc_put_uint8

`int rpc_put_uint8(rpc_t *rpc, uint8_t v);`

- 功能描述:
   - 向 rpc 参数区写入参数， rpc 的参数可以写入多个参数，根据写入的顺序存放。

- 参数:
   - `rpc`: rpc。
   - `v`: uint8_t类型的数据。

- 返回值:
   - 0: 成功 。
   - ENOMEM: 内存不足。

### rpc_put_double

`int rpc_put_double(rpc_t *rpc, double v);`

- 功能描述:
   - 向 rpc 参数区写入参数， rpc 的参数可以写入多个参数，根据写入的顺序存放。

- 参数:
   - `rpc`: rpc。
   - `v`: double类型的数据。

- 返回值:
   - 0: 成功 。
   - ENOMEM: 内存不足。

### rpc_put_point

`int rpc_put_point(rpc_t *rpc, const void *v);`

- 功能描述:
   - 向 rpc 参数区写入参数， rpc 的参数可以写入多个参数，根据写入的顺序存放。

- 参数:
   - `rpc`: rpc。
   - `v`: point类型的数据。

- 返回值:
   - 0: 成功 。
   - ENOMEM: 内存不足。

### rpc_put_buffer

`int rpc_put_buffer(rpc_t *rpc, const void *data, size_t size);`

- 功能描述:
   - 向 rpc 参数区写入参数， rpc 的参数可以写入多个参数，根据写入的顺序存放。

- 参数:
   - `rpc`: rpc。
   - `data`: array类型的数据。
   - `size`: 数据长度。

- 返回值:
   - 0: 成功 。
   - ENOMEM: 内存不足。

### rpc_put_string

`int rpc_put_string(rpc_t *rpc, char *str);`

- 功能描述:
   - 向 rpc 参数区写入参数， rpc 的参数可以写入多个参数，根据写入的顺序存放。

- 参数:
   - `rpc`: rpc。
   - `str`: string类型的数据。

- 返回值:
   - 0: 成功 。
   - ENOMEM: 内存不足。


### rpc_get_reset

`void rpc_get_reset(rpc_t *rpc);`

- 功能描述:
   - RPC 参数区读取位复。rpc 参数区的参数通管 rpc_get_xxx 一组函数读取，每调用一次读取函数则取出一个参数，序号递增，如果需要从头重新读取参数，则通过 rpc_get_reset 函数，将参数序号移到起始位置。

- 参数:
   - `rpc`: rpc。

- 返回值:
   - 无。


### rpc_get_int

`int rpc_get_int(rpc_t *rpc);`

- 功能描述:
   - 从参数区读取参数，在参数读取时，要调用与写入时的参数顺序一致，参数区的参数是顺序读取，每调用一次 rpc_get_int 函数，则依次读出一个参数。参数读取类型不一致时，会导致异常。

- 参数:
   - `rpc`: rpc。

- 返回值:
   - 返回int类型参数值。

### rpc_get_uint8

`uint8_t rpc_get_uint8(rpc_t *rpc);`

- 功能描述:
   - 从参数区读取参数，在参数读取时，要调用与写入时的参数顺序一致，参数区的参数是顺序读取，每调用一次 rpc_get_uint8 函数，则依次读出一个参数。参数读取类型不一致时，会导致异常。

- 参数:
   - `rpc`: rpc。

- 返回值:
   - 返回uint8类型参数值。

### rpc_get_double

`double rpc_get_double(rpc_t *rpc);`

- 功能描述:
   - 从参数区读取参数，在参数读取时，要调用与写入时的参数顺序一致，参数区的参数是顺序读取，每调用一次 rpc_get_double 函数，则依次读出一个参数。参数读取类型不一致时，会导致异常。

- 参数:
   - `rpc`: rpc。

- 返回值:
   - 返回double类型参数值。

### rpc_get_string

`char *rpc_get_string(rpc_t *rpc);`

- 功能描述:
   - 从参数区读取参数，在参数读取时，要调用与写入时的参数顺序一致，参数区的参数是顺序读取，每调用一次 rpc_get_string 函数，则依次读出一个参数。参数读取类型不一致时，会导致异常。

- 参数:
   - `rpc`: rpc。

- 返回值:
   - 返回char*类型参数值。

### rpc_get_point

`void *rpc_get_point(rpc_t *rpc);`

- 功能描述:
   - 从参数区读取参数，在参数读取时，要调用与写入时的参数顺序一致，参数区的参数是顺序读取，每调用一次 rpc_get_point 函数，则依次读出一个参数。参数读取类型不一致时，会导致异常。

- 参数:
   - `rpc`: rpc。

- 返回值:
   - 返回void*类型参数值。

### rpc_get_buffer

`void *rpc_get_buffer(rpc_t *rpc, int *count);`

- 功能描述:
   - 从参数区读取参数，在参数读取时，要调用与写入时的参数顺序一致，参数区的参数是顺序读取，每调用一次 rpc_get_buffer 函数，则依次读出一个参数。参数读取类型不一致时，会导致异常。

- 参数:
   - `rpc`: rpc。

- 返回值:
   - 返回void*类型参数值。

### rpc_reply

`void rpc_reply(rpc_t *rpc);`

- 功能描述:
   - 该函数在编写服务端时调用，当服务端处理完一条 rpc 时，必须调用该函数完成 rpc 的任务处理。

- 参数:
   - `rpc`: rpc。

- 返回值:
   - 无。

### rpc_deinit

`void rpc_deinit(rpc_t *rpc);`

- 功能描述:
   - 该函数在调用端执行，当调用者使用 uservice_call 函数调用 rpc时，在处理完 rpc 返回值后，需要调用 rpc_deinit 函数回收 rpc 资源。

- 参数:
   - `rpc`: rpc。

- 返回值:
   - 无。


### uservice_new

`uservice_t *uservice_new(const char *name, process_t process_rpc, void *context);`

- 功能描述:
   - 根据指定的微服务名称，上下文、最大命令ID，创建微服务，创建成功返回 uservice_t 指针，失败返回NULL。

- 参数:
   - `name`: 服务名字。
   - `process_rpc`: 服务远程调用处理函数。
   - `context`: 服务自定义上下文，用户在编写微服务处理函数时，可使用该上下文。

- 返回值:
   - 创建成功返回 uservice_t 指针，失败返回NULL。

#### process_t

`typedef int (*process_t)(void *context, rpc_t *rpc);`

- 功能描述:
   - 微服务的远程调用(RPC) 的主处理函数，在微服务开发时，尽可能减少 process_rpc 的处理时间。

- 参数:
   - `context`: 创建微服务时传入的 context。
   - `rpc`：远程序调用对象。

- 返回值:
   - 0: 成功。
   - -1: 失败。


### uservice_destroy

`void uservice_destroy(uservice_t *srv);`

- 功能描述:
   - 释放微服务 srv 所占用所有资源，释放srv 之前，需要确定 众uservice_task 中移出 uService。

- 参数:
   - `srv`: 微服务。

- 返回值:
   - 无。

#### uservice_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| name | const char * | 名称 |
| context | void | 上下文句柄 |
| process_rpc | process_t | 服务远程调用处理函数 |
| task | utask_t | 任务 |
| next | msp_slist_t | 指针域 |


### uservice_call_sync

`int uservice_call_sync(uservice_t *srv, int cmd, void *param, void *resp, size_t resp_size);`

- 功能描述:
   - 向微服务发送一条同步执行命令。

- 参数:
   - `srv`: 微服务。
   - `cmd`：指定需要执行的命令号。
   - `param`: 命令的参数。
   - `resp`：命令的返回值。
   - `resp_size`: 命令返回值空间大小。
- 返回值：
   - 0: 成功。
   - -1: 失败。


### uservice_call_async

`int uservice_call_async(uservice_t *srv, int cmd, void *param, size_t param_size);`

- 功能描述:
   - 向微服务发送一条异步执行命令。

- 参数:
   - `srv`: 微服务。
   - `cmd`：指定需要执行的命令号。
   - `param`: 命令的参数
   - `param_size`: 命令参数空间大小。

- 返回值：
   - 0: 成功。
   - -1: 失败。


### uservice_call

`int uservice_call(uservice_t *srv, rpc_t *rpc);`

- 功能描述:
   - 使用自定义 rpc 实现微服务的命令调用，该命令可以通过 rpc_t 实现同步、异步、超时调用，也可以用 rpc_put_xxx 函数，向 rpc 中传入多个参数。服务器也可以通过 rpc 返回多个数据，可以实现复杂参数传输调用。

- 参数:
   - `srv`: 微服务。
   - `rpc`: 调用的 RPC 指令。

- 返回值：
   - 0: 成功。
   - -1: 失败。


### uservice_lock

`void uservice_lock(uservice_t *srv);`

- 功能描述:
   - 微服务上锁。

- 参数:
   - `srv`: 微服务。

- 返回值：
   - 无。


### uservice_unlock

`void uservice_unlock(uservice_t *srv);`

- 功能描述:
   - 微服务解锁。

- 参数:
   - `srv`: 微服务。

- 返回值：
   - 无。


### uservice_process

`int uservice_process(void *context, rpc_t *rpc, const rpc_process_t rpcs[]);`

- 功能描述:
   - 微服务解锁。

- 参数:
   - `context`: 微服务。
   - `rpc`: rpc。
   - `rpcs`: 微服务。

- 返回值：
   - 无。

#### rpc_process_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| cmd_id | int | 命令id |
| process_rpc | process | 服务远程调用处理函数 |


### uservice_subscribe

`void uservice_subscribe(uservice_t *srv, uint32_t event_id);`

- 功能描述:
   - 该函数用于微服务中，调用该函数订阅事件，并将事件发送到微服务的 RPC 任务队列中，RPC 的 cmd_id 等于 event_id，通过 rpc_get_pointer 可以获取 事件的 data 参数。

- 参数:
   - `srv`: 微服务。
   - `event_id`: 事件id

- 返回值：
   - 无。


### utask_new

`utask_t *utask_new(const char *name, size_t stack_size, int queue_length, int prio);`

- 功能描述:
   - 创建微服务。

- 参数:
   - `name`: utask名
   - `stack_size`: 栈大小
   - `queue_length`: 队列个数
   - `prio`: 优先级

- 返回值：
   - 非NULL: 成功。
   - NULL: 失败。

#### utask_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| task | msp_task_t | 任务 |
| running | int8_t | 运行状态 |
| queue_count | uint8_t | 队列计数 |
| queue_max_used | uint8_t | 队列最大长度 |
| rpc_reclist | msp_slist_t | 指针域 |
| queue_buffer | void* | 缓存 |
| queue | msp_queue_t | 队列 |
| mutex | msp_mutex_t | 锁 |
| current_rpc | rpc_t* | 当前rpc |
| running_wait | msp_sem_t | 信号量 |
| uservice_lists | msp_slist_t | 链表 |
| rpc_buffer_gc_cache | msp_slist_t | 链表 |
| node | msp_slist_t | 链表 |


### utask_destroy

`void utask_destroy(utask_t *task);`

- 功能描述:
   - 释放微服务。

- 参数:
   - `task`: 微服务任务句柄。

- 返回值：
   - 无。


### utask_join

`void utask_join(utask_t *task);`

- 功能描述:
   - 等待微服务任务退出并收回占用资源。

- 参数:
   - `task`: 微服务任务句柄。

- 返回值：
   - 无。


### utask_add

`void utask_add(utask_t *task, uservice_t *srv);`

- 功能描述:
   - 向微服务任务中添加微服务。

- 参数:
   - `task`: 微服务任务句柄。
   - `srv`: 微服务。

- 返回值：
   - 无。


### utask_remove

`void utask_remove(utask_t *task, uservice_t *srv);`

- 功能描述:
   - 向微服务任务中删除微服务。

- 参数:
   - `task`: 微服务任务句柄。
   - `srv`: 微服务。

- 返回值：
   - 无。


### utask_lock

`void utask_lock(utask_t *task);`

- 功能描述:
   - 微服务任务上锁。

- 参数:
   - `task`: 微服务任务句柄。

- 返回值：
   - 无。


### utask_unlock

`void utask_unlock(utask_t *task);`

- 功能描述:
   - 微服务任务解锁。

- 参数:
   - `task`: 微服务任务句柄。

- 返回值：
   - 无。


### utask_set_softwdt_timeout

`void utask_set_softwdt_timeout(int ms);`

- 功能描述:
   - 设置看门狗超时时间。

- 参数:
   - `ms`: 超时时间，单位：ms。

- 返回值：
   - 无。


### event_service_init

`int  event_service_init(utask_t *task);`

- 功能描述:
   - 事件服务初始化。

- 参数:
   - `task`: 微服务任务句柄。

- 返回值:
   - 0: 成功。
   - -1: 失败。


### event_subscribe

`void event_subscribe(uint32_t event_id, event_callback_t cb, void *context);`

- 功能描述:
   - 订阅普通事件。

- 参数:
   - `event_id`: 事件号。
   - `cb`: 事件处理函数，该参数不能为 NULL，否则会触发异常。
   - `context`: 用户数据，该数据会传入到 cb 的 context 参数中。

- 返回值:
   - 无。

#### event_callback_t

`typedef void (*event_callback_t)(uint32_t event_id, const void *data, void *context);`

- 功能描述:
   - 事件处理函数类型。

- 参数:
   - `event_id`: 事件号。
   - `data`: array类型的数据。
   - `context`: 用户数据，该数据会传入到 cb 的 context 参数中。

- 返回值:
   - 无。


### event_unsubscribe

`void event_unsubscribe(uint32_t event_id, event_callback_t cb, void *context);`

- 功能描述:
   - 取消事件订阅，取消事件订阅时，要求传入的参数与订阅时的参数一致方可取消。

- 参数:
   - `event_id`: 事件号。
   - `cb`: 事件处理函数，该参数不能为 NULL，否则会触发异常。
   - `context`: 用户数据，该数据会传入到 cb 的 context 参数中。

- 返回值:
   - 无。


### event_publish

`void event_publish(uint32_t event_id, void *data);`

- 功能描述:
   - 发布事件。

- 参数:
   - `event_id`: 事件号。
   - `data`: 事件附带数据，该数据会传到订阅者的回调函数参数 data中，由于事件是异步处理，使用者要小心使用 data 的作用域，订阅者能正常访问该数据。

- 返回值:
   - 无。


### event_publish_delay

`void event_publish_delay(uint32_t event_id, void *data, int timeout_ms);`

- 功能描述:
   - 发布延时事件。

- 参数:
   - `event_id`: 事件号。
   - `data`: 事件附带数据，该数据会传到订阅者的回调函数参数 data中，由于事件是异步处理，使用者要小心使用 data 的作用域，订阅者能正常访问该数据。
   - `timeout_ms`: 超时时间，单位：ms。

- 返回值:
   - 无。


### event_subscribe_fd

`void event_subscribe_fd(uint32_t fd, event_callback_t cb, void *context);`

- 功能描述:
   - 订阅设备（网络）句柄数据可读事件。

- 参数:
   - `fd`: 设备/网络句柄。
   - `cb`: 事件处理函数，该参数不能为 NULL，否则会触发异常。
   - `context`: 用户数据，该数据会传入到 cb 的 context 参数中。
- 返回值:
   - 无。


### event_unsubscribe_fd

`void event_unsubscribe_fd(uint32_t fd, event_callback_t cb, void *context);`


- 功能描述:
   - 取消事件订阅，取消事件订阅时，要求传入的参数与订阅时的参数一致方可取消。

- 参数:
   - `fd`: 设备/网络句柄。
   - `cb`: 事件处理函数，该参数不能为 NULL，否则会触发异常。
   - `context`: 用户数据，该数据会传入到 cb 的 context 参数中。

- 返回值:
   - 无。


### event_publish_fd

`void event_publish_fd(uint32_t fd, void *data, int sync);`

- 功能描述:
   - 发布事件。

- 参数:
   - `fd`: 设备/网络句柄。
   - `data`: 事件附带数据，该数据会传到订阅者的回调函数参数 data中，由于事件是异步处理，使用者要小心使用 data 的作用域，订阅者能正常访问该数据。
   - `sync`: 同步标志，1：同步 0：异步。

- 返回值:
   - 无。


## 示例

### 服务接口及事件定义

```c
#ifndef DEMO_SERVICE_H
#define DEMO_SERVICE_H

#define EVENT_A 0x1000
#define EVENT_B 0x1001
#define EVENT_C 0x1002

int demo_add(int a, int b);

#endif
```

### 服务实现

```c
#include <uservice/uservice.h>

struct demo_uservice {
    uservice_t *srv;
    int a, b, c;
    rpc_t *current_cmd;
    rpc_t *current_oob;
};

/* cmd 列表 */
enum DEMO_CMD {
    CMD_1 = 0,
    CMD_2 = 1,
    CMD_RECV,

    CMD_DATA = 10,
    CMD_MAX
};

/* 事件ID */
#define EVENT_A 0x1000
#define EVENT_B 0x1001
#define EVENT_C 0x1002

struct cmd_param {
    int a, b, c;
};

/* 微服务处理函数*/
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
            /* 加法cmd 处理 */
            struct cmd_param *p = (struct cmd_param *)rpc_get_point(rpc);

            d->c =  p->a + p->b;

            /* 发布 EVENT_A 事件 */
            event_publish(EVENT_A, (void *)d->c);
            break;
        }

        case CMD_DATA:
            if (d->current_cmd != NULL) {
                rpc_reply(d->current_cmd);
                d->current_cmd = NULL;
                event_publish(EVENT_A, (void *)d->c);
            }
			/* rpc应答 */
            rpc_reply(rpc);
            break;

        default:
            /* rpc应答 */
            rpc_reply(rpc);
            break;
    }

    return 0;
}

static struct demo_uservice demo;
void demo_service_init()
{
    static utask_t *srv;
	/* 创建微服务 */
    srv = utask_new("demo_srv", 2 * 1024, QUEUE_MSG_COUNT, MSP_DEFAULT_APP_PRI);
	/* 创建utask */
    demo.srv = uservice_new("demo", process_rpc, &demo);

    /* 将微服务加入到utask中 */
    utask_add(srv, demo.srv);
}

void demo_add(int a, int b)
{
    struct cmd_param cmd;
    cmd.a = a;
    cmd.b = b;
    int c;

    /* 微服务同步调用 CMD_1 */
    uservice_call_sync(demo.srv, CMD_1, &cmd, &c, sizeof(int));
}
```

### 服务调用

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


## 诊断错误码
无。


## 运行资源
无。


## 依赖资源
  - minilibc
  - aos


## 组件参考
无。

