# bflb_msg_ctrl 消息控制器模块 (AI)

## 概述

`bflb_msg_ctrl` 是基于 SDIO 的双向消息路由和管理系统，实现了标签分发的消息处理架构。该模块作为设备端的核心消息路由器，负责将来自不同应用层的消息进行标签分类、队列管理和回调分发，是整个消息路由系统的调度中枢。

## 核心设计理念

### 1. 标签路由机制
- **消息标签化**：每个消息包含 tag 字段用于路由识别
- **回调分发**：基于 tag 将消息分发到不同的回调函数
- **双向支持**：同时支持上行发送(upload)和下行接收(download)

### 2. 异步处理架构

- **中断驱动**：SDIO 传输完成通过中断通知
- **任务调度**：专用任务处理消息队列和回调分发
- **非阻塞操作**：支持非阻塞的消息发送和接收
- **分层处理**：区分中断上下文和任务上下文的处理职责

**处理层次设计**：

- **中断层**：负责硬件事件响应、基本校验、队列操作
- **任务层**：负责复杂业务逻辑、回调分发、错误恢复
- **应用层**：负责具体的协议处理、数据转换、业务逻辑

**上行与下行处理差异**：

- **下行路径**：`中断接收` → `校验入队` → `任务分发` → `应用回调`
- **上行路径**：`中断发送完成` → `直接应用回调`

这种设计确保了系统的实时性和处理效率的平衡。

### 3. 资源池管理
- **帧缓冲池**：复用 frame_buff_ctrl 管理下行接收缓冲区
- **队列缓冲**：使用 FreeRTOS 队列管理上行和下行消息流
- **生命周期追踪**：通过回调机制追踪消息生命周期

## 架构设计

### 系统架构图

```
Application Layer
┌─────────┬─────────┬─────────┬─────────┬─────────┬─────────┐
│  TEST   │  SYS    │  WIFI   │  ETH    │  TTY    │  ...    │
│ TAG_1/2 │  TAG    │  TAG    │  TAG    │  TAG    │         │
└─────────┴─────────┴─────────┴─────────┴─────────┴─────────┘
     │         │         │         │         │         │
     └─────────┼─────────┼─────────┼─────────┼─────────┘
               ▼         ▼         ▼         ▼
┌─────────────────────────────────────────────────────────────┐
│                bflb_msg_ctrl (Message Router)                 │
│  ┌─────────────────┐    ┌─────────────────┐                │
│  │ Upload Queue    │    │ Download Queue  │                │
│  │ (upld_queue)    │    │ (dnld_queue)    │                │
│  └─────────────────┘    └─────────────────┘                │
│                                                             │
│  ┌─────────────────────────────────────────────────────────┐│
│  │            Message Processing Task                      ││
│  │  - Upload Send Management                               ││
│  │  - Download Receive Management                          ││
│  │  - Callback Distribution                                ││
│  └─────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────┘
               ▲         ▲         ▼         ▼
┌─────────────────────────────────────────────────────────────┐
│                 SDIO2 Hardware Interface                    │
│  ┌─────────────────┐    ┌─────────────────┐                │
│  │ Upload Channel  │    │ Download Channel│                │
│  │ (Device->Host)  │    │ (Host->Device)  │                │
│  └─────────────────┘    └─────────────────┘                │
└─────────────────────────────────────────────────────────────┘
               ▲                     ▼
          ┌──────────────────────────────────┐
          │         Linux Host               │
          └──────────────────────────────────┘
```

### 消息包结构

```c
struct bflb_msg_packt {
    uint8_t tag;            // Message routing tag (0-7)
    uint8_t sub_tag;        // Message sub-tag for fine-grained classification
    uint16_t len;           // Data payload length
    uint8_t data[0];        // Variable length data payload
};
```

### 控制器结构体

```c
struct bflb_msg_ctrl_t {
    // Hardware Interface
    struct bflb_device_s *sdio2_hd;         // SDIO2 device handle

    // Configuration Limits
    uint32_t msg_dnld_max_size;             // Max download message size
    uint32_t msg_upld_max_size;             // Max upload message size

    // Queue Management
    QueueHandle_t dnld_queue;               // Download processing queue
    QueueHandle_t upld_queue;               // Upload pending queue
    frame_queue_ctrl_t *dnld_queue_ctrl;    // Download buffer pool controller

    // Callback Arrays (indexed by tag)
    bflb_msg_cb_t bflb_msg_dnld_recv_cb[BL_MSG_TAG_MAX];        // Download callbacks
    void *bflb_msg_dnld_recv_arg[BL_MSG_TAG_MAX];             // Download callback args
    bflb_msg_cb_t bflb_msg_upld_send_cb[BL_MSG_TAG_MAX];        // Upload callbacks
    void *bflb_msg_upld_send_arg[BL_MSG_TAG_MAX];             // Upload callback args
};
```

## 数据流架构

### 上行数据流 (Upload Path)

```
Application
     │ bflb_msg_ctrl_alloc()
     ▼
┌─────────────┐
│ Frame Pool  │ ← frame_elem_t allocation
│ (dnld_ctrl) │
└─────────────┘
     │ fill msg_packt & bflb_msg_ctrl_send()
     ▼
┌─────────────┐
│Upload Queue │ ← queued for transmission
│(upld_queue) │
└─────────────┘
     │ msg_proc_task dequeue
     ▼
┌─────────────┐
│SDIO2 Upload │ ← sdio2_upld_start()
│  Channel    │
└─────────────┘
     │ hardware completion interrupt
     ▼
┌─────────────┐    (ISR Context)
│Upload Done  │ ← bflb_msg_upld_send_done_cb()
│  Callback   │   - Direct callback
└─────────────┘   - No task queuing
     │ tag-based routing (immediate)
     ▼
┌─────────────┐    (Still ISR Context)
│Application  │ ← upld_send_cb[tag]()
│  Callback   │   - Simple operations only
└─────────────┘   - Resource release
                  - Statistics update
```

### 下行数据流 (Download Path)

```
Linux Host
     │ SDIO transmission
     ▼
┌─────────────┐
│SDIO2 Dnload │ ← hardware receives data
│  Channel    │
└─────────────┘
     │ completion interrupt
     ▼
┌─────────────┐    (ISR Context)
│Download Done│ ← bflb_msg_dnld_recv_done_cb()
│  Callback   │   - Fast validation
└─────────────┘   - Queue to task
     │ validation & queuing
     ▼
┌─────────────┐
│Download     │ ← queued for processing
│Queue        │
│(dnld_queue) │
└─────────────┘
     │ msg_proc_task dequeue
     ▼
┌─────────────┐    (Task Context)
│Tag-based    │ ← dnld_recv_cb[tag]()
│Distribution │   - Complex processing OK
└─────────────┘
     │ per-tag routing
     ▼
┌─────────────┐
│Application  │ ← specific callback execution
│  Callback   │   - Business logic
└─────────────┘   - File operations
                  - Network forwarding
```

## 核心机制分析

### 1. 消息处理任务 (msg_proc_task)

这是整个模块的核心调度任务，运行一个三阶段的处理循环：

```c
static void msg_proc_task(void *arg)
{
    // Initialization: wait for SDIO2 ready
    while (sdio2_is_ready()) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    while (1) {
        // Suspend management for power efficiency
        if (upld_send_suspend && dnld_recv_suspend && dnld_proc_suspend) {
            ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));
        }

        // Stage 1: Upload Send Management
        if (!sdio2_upld_is_busy() && !upld_send_suspend) {
            // Try to start upload transmission
            frame_queue_receive(upld_queue, &frame_elem, 0);
            sdio2_upld_start(frame_elem, msg_ctrl);
        }

        // Stage 2: Download Receive Management
        if (!sdio2_dnld_is_busy() && !dnld_recv_suspend) {
            // Prepare buffer for download reception
            frame_queue_alloc_elem(dnld_queue_ctrl, &frame_elem, 0);
            sdio2_dnld_start(frame_elem, msg_ctrl);
        }

        // Stage 3: Download Message Processing
        if (!dnld_proc_suspend) {
            // Process received messages via tag-based callbacks
            frame_queue_receive(dnld_queue, &frame_elem, 0);
            dnld_recv_cb[msg_packt->tag](frame_elem, dnld_recv_arg[tag]);
        }
    }
}
```

**关键设计特点**：

- **状态机管理**：通过三个 suspend 标志管理任务状态
- **轮询调度**：循环处理三个阶段，确保及时响应
- **非阻塞操作**：所有队列操作都是非阻塞的 (xTicksToWait=0)
- **功耗优化**：全部挂起时进入等待状态，通过通知唤醒

### 2. 中断回调机制

#### 上行发送完成回调

```c
int bflb_msg_upld_send_done_cb(bflb_msg_ctrl_t *msg_ctrl, frame_elem_t *frame_elem)
{
    bflb_msg_packt_t *msg_packt = (bflb_msg_packt_t *)frame_elem->buff_addr;

    // Wake up processing task
    msg_proc_task_wakeup();

    // Tag validation
    if (msg_packt->tag >= BL_MSG_TAG_MAX) {
        goto err_exit; // Invalid tag, free frame
    }

    // Callback dispatch
    bflb_msg_cb_t upld_send_cb = msg_ctrl->bflb_msg_upld_send_cb[msg_packt->tag];
    if (upld_send_cb != NULL) {
        ret = upld_send_cb(frame_elem, msg_ctrl->bflb_msg_upld_send_arg[msg_packt->tag]);
        if (ret < 0) {
            goto err_exit; // Callback failed, free frame
        }
        // Callback handles frame lifecycle (ret >= 0)
    } else {
        goto err_exit; // No callback registered, free frame
    }

    return 0;

err_exit:
    frame_queue_free_elem(frame_elem); // Return to pool
    return 0;
}
```

#### 下行接收完成回调

```c
int bflb_msg_dnld_recv_done_cb(bflb_msg_ctrl_t *msg_ctrl, frame_elem_t *frame_elem)
{
    bflb_msg_packt_t *msg_packt = (bflb_msg_packt_t *)frame_elem->buff_addr;

    // Wake up processing task
    msg_proc_task_wakeup();

    // Validation checks
    if (msg_packt->tag >= BL_MSG_TAG_MAX) {
        goto err_exit; // Invalid tag
    }

    if (frame_elem->data_size < msg_packt->len + sizeof(bflb_msg_packt_t)) {
        goto err_exit; // Size mismatch
    }

    // Queue for processing (non-blocking)
    frame_queue_send(msg_ctrl->dnld_queue, &frame_elem, 0);
    return 0;

err_exit:
    frame_queue_free_elem(frame_elem); // Return to pool
    return 0;
}
```

**中断回调特点**：

- **ISR安全**：所有操作都支持中断上下文
- **快速返回**：最小化中断处理时间
- **错误处理**：自动释放无效或失败的消息
- **任务通知**：唤醒处理任务进行后续处理

**重要设计说明**：

- **下行接收处理**：接收完成后仅在中断中进行基本校验和入队，实际的回调分发延迟到任务上下文中执行。这是因为接收到的数据通常需要上层进行复杂的业务逻辑处理，可能涉及文件操作、网络转发、协议解析等耗时操作，不适合在中断上下文中执行。

- **上行发送处理**：发送完成后直接在中断上下文中执行应用回调。这是因为发送完成的处理通常只涉及资源释放、统计更新、状态标记等简单快速的操作，不会产生显著的中断延迟，可以立即完成处理并释放资源。

### 3. 内存管理策略

#### 下行缓冲池管理

```c
// Static frame buffer pool (aligned and non-cached)
FRAME_BUFFER_ATTR uint8_t msg_dnld_frame_buff[BL_MSG_CTRL_DNLD_FRAME_NUM][BL_MSG_CTRL_DNLD_FRAME_SIZE];

// Pool configuration
frame_queue_ctrl_init_cfg_t cfg = {
    .frame_buff = msg_dnld_frame_buff,           // Static buffer array
    .frame_type = BL_MSG_CTRL_DNLD_FRAME_TYPE,   // Type identifier (0x01)
    .frame_elem_cnt = BL_MSG_CTRL_DNLD_FRAME_NUM, // 16 frames
    .frame_elem_size = BL_MSG_CTRL_DNLD_FRAME_SIZE, // 2048 + 64 bytes
    .name = "msg_ctrl_queue",
};
```

**内存管理特点**：

- **静态分配**：编译期确定内存布局，避免运行时分配
- **硬件对齐**：32字节对齐确保 DMA 效率
- **非缓存区域**：避免缓存一致性问题
- **固定大小**：16个2048字节缓冲区，满足大部分消息需求

#### 生命周期追踪

```c
// Register callback for frame lifecycle tracking
frame_queue_free_cb_register(dnld_queue_ctrl,
                             NULL, NULL,                    // before_free_cb
                             bflb_msg_ctrl_free_after_cb,     // after_free_cb
                             NULL);

void bflb_msg_ctrl_free_after_cb(frame_elem_t *frame_elem, void *arg)
{
    // Wake up message processing task when frame is freed
    msg_proc_task_wakeup();
}
```

### 4. 队列架构分析

#### 三队列设计

1. **upld_queue**: 上行待发送队列
   - 深度：32个指针
   - 用途：缓存应用层待发送的消息
   - 特点：承接突发流量，平滑硬件传输

2. **dnld_queue**: 下行待处理队列
   - 深度：16个指针
   - 用途：缓存接收完成待分发的消息
   - 特点：解耦中断处理和回调分发

3. **pool_queue**: 下行缓冲池队列
   - 深度：16个指针
   - 用途：管理空闲接收缓冲区
   - 特点：复用 frame_buff_ctrl 的内存池管理

#### 队列流转图

```
Pool Management Flow:
┌─────────────┐    alloc     ┌─────────────┐    sdio2_dnld   ┌─────────────┐
│ pool_queue  │ ───────────► │ SDIO2 RX    │ ──────────────► │ dnld_queue  │
│ (free buffs)│              │ (receiving) │                 │ (received)  │
└─────────────┘              └─────────────┘                 └─────────────┘
       ▲                                                             │
       │                                                             │
       │ free                                                        │ process
       │                                                             ▼
┌─────────────┐              ┌─────────────┐    callback     ┌─────────────┐
│ FREED       │◄─────────────│ Application │◄────────────────│ MSG PROC    │
│ (returned)  │              │ (processed) │                 │ TASK        │
└─────────────┘              └─────────────┘                 └─────────────┘
```

## 标签路由系统

### 消息标签定义

```c
enum {
    BL_MSG_TAG_NONE,        // 0x00: Invalid/No tag

    BL_MSG_TAG_TEST_1,      // 0x01: Test functionality 1
    BL_MSG_TAG_TEST_2,      // 0x02: Test functionality 2
    BL_MSG_TAG_TEST_3,      // 0x03: Test functionality 3

    BL_MSG_TAG_SYS,         // 0x04: System management
    BL_MSG_TAG_WIFI,        // 0x05: WiFi subsystem
    BL_MSG_TAG_ETH,         // 0x06: Ethernet subsystem
    BL_MSG_TAG_TTY,         // 0x07: TTY/Console subsystem

    BL_MSG_TAG_MAX          // 0x08: Boundary check
};
```

### 回调注册机制

```c
int bflb_msg_cb_register(bflb_msg_ctrl_t *msg_ctrl, uint8_t tag,
                       bflb_msg_cb_t dnld_recv_cb, void *dnld_recv_arg,
                       bflb_msg_cb_t upld_send_cb, void *upld_send_arg)
{
    // Parameter validation
    if (msg_ctrl == NULL || tag >= BL_MSG_TAG_MAX) {
        return -1;
    }

    // Atomic registration
    flag = bflb_irq_save();

    // Register download receive callback
    if (dnld_recv_cb != NULL) {
        msg_ctrl->bflb_msg_dnld_recv_cb[tag] = dnld_recv_cb;
        msg_ctrl->bflb_msg_dnld_recv_arg[tag] = dnld_recv_arg;
    }

    // Register upload send done callback
    if (upld_send_cb != NULL) {
        msg_ctrl->bflb_msg_upld_send_cb[tag] = upld_send_cb;
        msg_ctrl->bflb_msg_upld_send_arg[tag] = upld_send_arg;
    }

    bflb_irq_restore(flag);
    return 0;
}
```

**回调机制特点**：

- **原子注册**：中断保护确保注册过程的原子性
- **可选回调**：支持只注册下行或上行回调
- **参数传递**：每个回调都有独立的参数指针
- **标签索引**：O(1) 时间复杂度的回调查找

### 消息分发逻辑

#### 下行消息分发

```c
// In msg_proc_task() Stage 3:
msg_packt = (bflb_msg_packt_t *)frame_elem->buff_addr;

if (msg_ctrl->bflb_msg_dnld_recv_cb[msg_packt->tag] == NULL) {
    // No callback registered for this tag
    LOG_E("msg dnld recv err: tag 0x%02X is not registered, drop it!\r\n", msg_packt->tag);
    frame_queue_free_elem(frame_elem);
} else {
    // Dispatch to registered callback
    ret = msg_ctrl->bflb_msg_dnld_recv_cb[msg_packt->tag](frame_elem,
                                                         msg_ctrl->bflb_msg_dnld_recv_arg[msg_packt->tag]);
    if (ret < 0) {
        LOG_E("msg dnld recv err: tag 0x%02X callback failed, drop it!\r\n", msg_packt->tag);
        frame_queue_free_elem(frame_elem);
    }
    // ret >= 0: callback handles frame lifecycle
}
```

#### 上行消息分发

```c
// In bflb_msg_upld_send_done_cb():
upld_send_cb = msg_ctrl->bflb_msg_upld_send_cb[msg_packt->tag];
upld_send_arg = msg_ctrl->bflb_msg_upld_send_arg[msg_packt->tag];

if (upld_send_cb != NULL) {
    ret = upld_send_cb(frame_elem, upld_send_arg);
    if (ret < 0) {
        goto err_exit; // Callback failed, free frame
    }
    // ret >= 0: callback took ownership of frame
} else {
    goto err_exit; // No callback registered, free frame
}
```

## 核心API接口

### 生命周期管理

```c
// Initialize message controller
bflb_msg_ctrl_t *bflb_msg_ctrl_init(void);

// Cleanup message controller
int bflb_msg_ctrl_deinit(bflb_msg_ctrl_t *msg_ctrl);
```

### 消息操作接口

```c
// Allocate frame for message sending
frame_elem_t *bflb_msg_ctrl_alloc(bflb_msg_ctrl_t *msg_ctrl, TickType_t xTicksToWait);

// Send message through controller
int bflb_msg_ctrl_send(bflb_msg_ctrl_t *msg_ctrl, frame_elem_t *frame_elem, uint8_t tofront);
```

### 回调管理接口

```c
// Register message callbacks for specific tag
int bflb_msg_cb_register(bflb_msg_ctrl_t *msg_ctrl, uint8_t tag,
                       bflb_msg_cb_t dnld_recv_cb, void *dnld_recv_arg,
                       bflb_msg_cb_t upld_send_cb, void *upld_send_arg);

// Unregister callbacks for specific tag
int bflb_msg_cb_unregister(bflb_msg_ctrl_t *msg_ctrl, uint8_t tag);
```

### ISR回调接口

```c
// Called from SDIO2 upload completion ISR
int bflb_msg_upld_send_done_cb(bflb_msg_ctrl_t *msg_ctrl, frame_elem_t *frame_elem);

// Called from SDIO2 download completion ISR
int bflb_msg_dnld_recv_done_cb(bflb_msg_ctrl_t *msg_ctrl, frame_elem_t *frame_elem);
```

## 线程安全与同步机制

### 1. 中断安全设计

所有可能在中断上下文调用的函数都进行了ISR适配：

```c
static int msg_proc_task_wakeup(void)
{
    if (xPortIsInsideInterrupt()) {
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(msg_proc_task_hd, &pxHigherPriorityTaskWoken);
        if (pxHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
        xTaskNotifyGive(msg_proc_task_hd);
    }
    return 0;
}
```

### 2. 回调注册保护

```c
int bflb_msg_cb_register(...)
{
    uintptr_t flag;

    // Critical section for atomic callback registration
    flag = bflb_irq_save();

    // Modify callback arrays atomically
    msg_ctrl->bflb_msg_dnld_recv_cb[tag] = dnld_recv_cb;
    msg_ctrl->bflb_msg_upld_send_cb[tag] = upld_send_cb;

    bflb_irq_restore(flag);
    return 0;
}
```

### 3. 队列同步

- **FreeRTOS队列**：天然的线程安全队列操作
- **非阻塞模式**：避免中断上下文阻塞
- **原子指针传递**：队列中传递指针，确保原子性

## 性能特性

### 1. 时间复杂度分析

- **消息发送**：O(1) - 队列入队 + 任务通知
- **消息接收**：O(1) - 队列出队 + 标签索引
- **回调分发**：O(1) - 数组索引访问
- **标签查找**：O(1) - 直接数组索引

### 2. 内存效率

- **零拷贝传输**：通过指针传递，避免数据拷贝
- **池化管理**：复用帧缓冲池，避免动态分配
- **紧凑结构**：消息头部仅4字节，开销极小

### 3. 吞吐量优化

- **批量处理**：任务循环处理多个阶段
- **管道并行**：上行、下行、处理三个阶段并行
- **中断驱动**：硬件完成即时通知，减少轮询开销
- **分层回调**：上行中断直接回调，下行任务延迟回调，平衡实时性和处理能力

### 4. 实时性分析

**上行发送路径实时性**：

- 发送完成中断 → 立即回调处理 → 资源释放
- 中断延迟：< 10μs（简单的释放和统计操作）
- 适用场景：资源管理、状态更新、简单统计

**下行接收路径实时性**：

- 接收完成中断 → 快速入队 → 任务调度 → 回调处理
- 中断延迟：< 5μs（仅校验和入队操作）
- 任务响应时间：根据系统负载动态调整
- 适用场景：协议解析、业务逻辑、文件操作、网络转发

## 应用场景和使用模式

### 1. 基本消息收发

```c
// System initialization
bflb_msg_ctrl_t *msg_ctrl = bflb_msg_ctrl_init();

// Register WiFi message callbacks
int wifi_dnld_callback(frame_elem_t *frame_elem, void *arg) {
    bflb_msg_packt_t *msg = (bflb_msg_packt_t *)frame_elem->buff_addr;
    // Process WiFi command from host
    process_wifi_command(msg->data, msg->len);
    frame_queue_free_elem(frame_elem); // Release frame
    return 0;
}

int wifi_upld_callback(frame_elem_t *frame_elem, void *arg) {
    // WiFi response sent successfully
    frame_queue_free_elem(frame_elem); // Release frame
    return 0;
}

// Register WiFi callbacks
bflb_msg_cb_register(msg_ctrl, BL_MSG_TAG_WIFI,
                   wifi_dnld_callback, wifi_context,
                   wifi_upld_callback, wifi_context);

// Send WiFi status message
frame_elem_t *frame = bflb_msg_ctrl_alloc(msg_ctrl, portMAX_DELAY);
if (frame != NULL) {
    bflb_msg_packt_t *msg = (bflb_msg_packt_t *)frame->buff_addr;
    msg->tag = BL_MSG_TAG_WIFI;
    msg->sub_tag = WIFI_STATUS;
    msg->len = status_data_len;
    memcpy(msg->data, status_data, status_data_len);

    frame->data_size = sizeof(bflb_msg_packt_t) + msg->len;
    bflb_msg_ctrl_send(msg_ctrl, frame, 0);
}
```

### 2. 多协议复用

```c
// Register multiple protocol handlers
typedef struct {
    bflb_msg_ctrl_t *msg_ctrl;
    protocol_context_t *ctx;
} protocol_handler_t;

// Ethernet handler
int eth_message_handler(frame_elem_t *frame_elem, void *arg) {
    protocol_handler_t *handler = (protocol_handler_t *)arg;
    bflb_msg_packt_t *msg = (bflb_msg_packt_t *)frame_elem->buff_addr;

    switch (msg->sub_tag) {
        case ETH_CONFIG:
            configure_ethernet(msg->data, msg->len);
            break;
        case ETH_DATA:
            forward_to_ethernet(msg->data, msg->len);
            break;
        default:
            LOG_W("Unknown ETH sub_tag: 0x%02X\r\n", msg->sub_tag);
            break;
    }

    frame_queue_free_elem(frame_elem);
    return 0;
}

// TTY handler
int tty_message_handler(frame_elem_t *frame_elem, void *arg) {
    bflb_msg_packt_t *msg = (bflb_msg_packt_t *)frame_elem->buff_addr;

    // Forward TTY data to console
    console_write(msg->data, msg->len);
    frame_queue_free_elem(frame_elem);
    return 0;
}

// Register all protocols
protocol_handler_t eth_handler = {msg_ctrl, eth_ctx};
protocol_handler_t tty_handler = {msg_ctrl, tty_ctx};

bflb_msg_cb_register(msg_ctrl, BL_MSG_TAG_ETH, eth_message_handler, &eth_handler, NULL, NULL);
bflb_msg_cb_register(msg_ctrl, BL_MSG_TAG_TTY, tty_message_handler, &tty_handler, NULL, NULL);
```

### 3. 异步响应处理

```c
// Asynchronous command-response pattern
typedef struct {
    uint32_t request_id;
    TaskHandle_t waiting_task;
    int result;
} async_request_t;

int system_command_handler(frame_elem_t *frame_elem, void *arg) {
    bflb_msg_packt_t *cmd = (bflb_msg_packt_t *)frame_elem->buff_addr;

    // Process system command
    int result = process_system_command(cmd->data, cmd->len);

    // Send response back
    frame_elem_t *resp_frame = bflb_msg_ctrl_alloc(msg_ctrl, pdMS_TO_TICKS(100));
    if (resp_frame != NULL) {
        bflb_msg_packt_t *resp = (bflb_msg_packt_t *)resp_frame->buff_addr;
        resp->tag = BL_MSG_TAG_SYS;
        resp->sub_tag = SYS_RESPONSE;
        resp->len = sizeof(result);
        memcpy(resp->data, &result, sizeof(result));

        resp_frame->data_size = sizeof(bflb_msg_packt_t) + resp->len;
        bflb_msg_ctrl_send(msg_ctrl, resp_frame, 0);
    }

    frame_queue_free_elem(frame_elem);
    return 0;
}
```

## 错误处理和调试

### 1. 错误检测机制

```c
// Tag validation
if (msg_packt->tag >= BL_MSG_TAG_MAX) {
    LOG_E("Invalid message tag: 0x%02X\r\n", msg_packt->tag);
    goto error_cleanup;
}

// Size validation
if (frame_elem->data_size < msg_packt->len + sizeof(bflb_msg_packt_t)) {
    LOG_E("Message size mismatch: frame_size=%d, msg_len=%d\r\n",
          frame_elem->data_size, msg_packt->len);
    goto error_cleanup;
}

// Callback validation
if (msg_ctrl->bflb_msg_dnld_recv_cb[msg_packt->tag] == NULL) {
    LOG_E("No callback registered for tag 0x%02X\r\n", msg_packt->tag);
    goto error_cleanup;
}
```

### 2. 资源泄漏预防

```c
// Automatic cleanup in error paths
err_exit:
    frame_queue_free_elem(frame_elem); // Always free frame on error
    return 0; // ISR callbacks should always return 0
```

### 3. 运行状态监控

```c
void print_msg_ctrl_status(bflb_msg_ctrl_t *msg_ctrl) {
    printf("=== Message Controller Status ===\r\n");
    printf("Upload Queue: %d/%d\r\n",
           uxQueueMessagesWaiting(msg_ctrl->upld_queue),
           BL_MSG_CTRL_UPLD_QUEUE_DEPTH);
    printf("Download Queue: %d/%d\r\n",
           uxQueueMessagesWaiting(msg_ctrl->dnld_queue),
           BL_MSG_CTRL_DNLD_FRAME_NUM);
    printf("Download Pool: %d/%d available\r\n",
           frame_queue_pool_get_available(msg_ctrl->dnld_queue_ctrl),
           BL_MSG_CTRL_DNLD_FRAME_NUM);

    printf("Registered Callbacks:\r\n");
    for (int i = 0; i < BL_MSG_TAG_MAX; i++) {
        if (msg_ctrl->bflb_msg_dnld_recv_cb[i] || msg_ctrl->bflb_msg_upld_send_cb[i]) {
            printf("  Tag 0x%02X: dnld=%p, upld=%p\r\n", i,
                   msg_ctrl->bflb_msg_dnld_recv_cb[i],
                   msg_ctrl->bflb_msg_upld_send_cb[i]);
        }
    }
}
```

## 设计优势

1. **高度模块化**：基于标签的路由机制，支持多协议复用
2. **异步非阻塞**：中断驱动 + 任务调度，响应及时
3. **内存高效**：池化管理 + 零拷贝传输，资源利用率高
4. **线程安全**：全面的ISR适配和原子操作保护
5. **易于扩展**：标签数组设计，新协议只需注册回调
6. **错误健壮**：完善的参数校验和错误恢复机制
7. **调试友好**：详细的日志输出和状态监控接口

该模块作为消息路由系统的核心调度器，为上层应用提供了统一、高效、可靠的消息传输服务，是实现复杂协议栈和多应用通信的关键基础设施。
