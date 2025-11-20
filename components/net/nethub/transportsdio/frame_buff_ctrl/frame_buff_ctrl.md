# bflb_frame_buff_ctrl Frame Buffer Controller Module (AI)

## Overview

`bflb_frame_buff_ctrl` is a flexible memory pool management module based on FreeRTOS queues, implementing a simple and efficient memory block management mechanism. This module adopts a "header metadata + data buffer" design pattern, where each memory block stores descriptor information in its header, and memory pool allocation and recycling are managed through FreeRTOS queues.

## Core Design Philosophy

### 1. Header Embedded Design
- **Metadata Pre-placement**: The first 64 bytes of each memory block serve as a header area to store the frame_elem_t descriptor
- **Zero-copy Transfer**: Memory block transfer with zero-copy is achieved by passing frame_elem_t pointers
- **Self-description Capability**: Each memory block contains complete metadata, supporting self-management

### 2. Multi-queue Architecture
- **pool_queue**: Memory pool queue, managing allocation and recycling of free memory blocks
- **output_queue**: Output queue, implementing FIFO buffering of data blocks
- **input_queue**: Input queue, implementing FIFO buffering of data blocks

### 3. Callback Mechanism
- **before_free_cb**: Pre-release callback, used for data cleanup or statistics
- **after_free_cb**: Post-release callback, used for status updates or notifications

## Architecture Design

### Memory Layout Architecture

```
Continuous Memory Region Provided by Application (frame_buff)
┌─────────────────────────────────────────────────────────────────┐
│     Frame 0      │     Frame 1      │  ...  │   Frame N-1      │
│┌────────────────┐│┌────────────────┐│       │┌────────────────┐│
││  Header(64B)   │││  Header(64B)   ││       ││  Header(64B)   ││
││frame_elem_t    │││frame_elem_t    ││       ││frame_elem_t    ││
││                │││                ││       ││                ││
│├────────────────┤│├────────────────┤│       │├────────────────┤│
││                │││                ││       ││                ││
││  Data Buffer   │││  Data Buffer   ││       ││  Data Buffer   ││
││                │││                ││       ││                ││
││                │││                ││       ││                ││
│└────────────────┘│└────────────────┘│       │└────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

### frame_elem_t Structure Layout

```c
struct frame_elem_t {
    /**** Immutable Fields (Fixed After Initialization) ****/
    uint16_t frame_type;                 // Frame type identifier
    uint16_t frame_id;                   // Frame ID (0 to N-1)
    frame_queue_ctrl_t *frame_ctrl;      // Owner controller pointer

    void *buff_addr;                     // Data buffer address
    uint16_t buff_size;                  // Buffer size

    /**** Mutable Fields (Runtime Variable) ****/
    uint16_t data_size;                  // Actual data size

    uint8_t ex_data[0];                  // Extended data area
};
```

### Controller Structure

```c
struct frame_queue_ctrl_t {
    uint16_t frame_type;                 // Frame type
    uint16_t frame_elem_cnt;             // Frame element count
    uint16_t frame_elem_size;            // Each frame element size
    uint16_t frame_elem_hdr_size;        // Frame element header size(64 bytes)

    // Callback functions
    frame_queue_free_cb_t before_free_cb;
    void *before_free_cb_arg;
    frame_queue_free_cb_t after_free_cb;
    void *after_free_cb_arg;

    // Three queues
    QueueHandle_t pool_queue;            // Memory pool queue
    QueueHandle_t output_queue;          // Output queue
    QueueHandle_t input_queue;           // Input queue

    void *frame_buff;                    // Frame buffer pointer
    char *name;                          // Controller name
};
```

## Data Lifecycle Management

### Lifecycle State Diagram

```
[INITIALIZED]
    │
    ▼
┌─────────────┐    frame_queue_alloc_elem()
│   FREE      │ ─────────────────────────────► ┌─────────────┐
│ (pool_queue)│                                │  ALLOCATED  │
└─────────────┘                                └─────────────┘
    ▲                                                  │
    │                                                  │
    │ frame_queue_free_elem()                         │ App Usage
    │                                                  ▼
    │                                          ┌─────────────┐
    └──────────────────────────────────────────│   IN_USE    │
                                               └─────────────┘
```

### Detailed Lifecycle Analysis

#### 1. Initialization Phase

```c
frame_queue_ctrl_t *frame_queue_create(frame_queue_ctrl_init_cfg_t *cfg)
{
    // Create controller structure
    frame_queue_ctrl_t *ctrl = kmalloc(sizeof(frame_queue_ctrl_t));

    // Create three queues
    QueueHandle_t pool_queue = xQueueCreate(cfg->frame_elem_cnt, sizeof(uintptr_t));
    QueueHandle_t output_queue = xQueueCreate(cfg->frame_elem_cnt, sizeof(uintptr_t));
    QueueHandle_t input_queue = xQueueCreate(cfg->frame_elem_cnt, sizeof(uintptr_t));

    // Initialize each frame_elem_t
    for (int i = 0; i < cfg->frame_elem_cnt; i++) {
        frame_elem_t *elem = (frame_elem_t *)((uintptr_t)cfg->frame_buff + cfg->frame_elem_size * i);

        // Set immutable fields
        elem->frame_type = cfg->frame_type;
        elem->frame_id = i;
        elem->frame_ctrl = ctrl;

        // Set buffer address (skip 64-byte header)
        elem->buff_addr = (void *)((uintptr_t)elem + FRAME_BUFF_HEADER_ROOM);
        elem->buff_size = cfg->frame_elem_size - FRAME_BUFF_HEADER_ROOM;
        elem->data_size = 0;

        // Put element into pool queue
        frame_queue_send(pool_queue, &elem, 0);
    }
}
```

**Key Design Points**:

- Each frame_elem_t is directly stored in the first 64 bytes of the memory block
- buff_addr points to the data area after the header
- frame_id serves as a unique identifier for debugging and tracking

#### 2. Allocation Phase (FREE → ALLOCATED)

```c
int frame_queue_alloc_elem(frame_queue_ctrl_t *ctrl,
                          frame_elem_t **frame_elem,
                          TickType_t xTicksToWait)
{
    // Get frame_elem_t pointer from pool queue
    return frame_queue_receive(ctrl->pool_queue, frame_elem, xTicksToWait);
}
```

**Allocation Characteristics**:

- O(1) time complexity allocation operation
- Supports both blocking and non-blocking allocation
- Automatically supports task and interrupt contexts

#### 3. Usage Phase (ALLOCATED → IN_USE)

Typical usage pattern after the application layer obtains frame_elem_t:

```c
void application_use_frame(frame_elem_t *elem)
{
    // 1. Write data to buffer
    memcpy(elem->buff_addr, source_data, data_length);
    elem->data_size = data_length;

    // 2. Can use ex_data area to store additional info
    // memcpy(elem->ex_data, extra_info, extra_size);

    // 3. Pass to other modules for processing
    send_to_next_module(elem);
}
```

#### 4. Release Phase (IN_USE → FREE)

```c
int frame_queue_free_elem(frame_elem_t *frame_elem)
{
    frame_queue_ctrl_t *ctrl = frame_elem->frame_ctrl;

    // Execute before-free callback
    if (ctrl->before_free_cb) {
        ctrl->before_free_cb(frame_elem, ctrl->before_free_cb_arg);
    }

    // Reset mutable fields
    frame_elem->data_size = 0;

    // Return to pool queue
    int ret = frame_queue_send(frame_elem->frame_ctrl->pool_queue, &frame_elem, 0);

    // Execute after-free callback
    if (ctrl->after_free_cb) {
        ctrl->after_free_cb(frame_elem, ctrl->after_free_cb_arg);
    }

    return ret;
}
```

**Release Characteristics**:

- Automatically finds the owning pool through frame_elem->frame_ctrl
- Supports pre and post-release callbacks for statistics and cleanup
- Resets data_size to ensure data consistency

## Core API Interfaces

### Controller Management Interfaces

```c
// Create frame queue controller
frame_queue_ctrl_t *frame_queue_create(frame_queue_ctrl_init_cfg_t *cfg);

// Remove frame queue controller
int frame_queue_remove(frame_queue_ctrl_t *ctrl);

// Register free callback functions
int frame_queue_free_cb_register(frame_queue_ctrl_t *ctrl,
                                 frame_queue_free_cb_t before_free_cb,
                                 void *before_free_cb_arg,
                                 frame_queue_free_cb_t after_free_cb,
                                 void *after_free_cb_arg);
```

### Memory Management Interfaces

```c
// Allocate memory block from pool
int frame_queue_alloc_elem(frame_queue_ctrl_t *ctrl,
                          frame_elem_t **frame_elem,
                          TickType_t xTicksToWait);

// Free memory block back to pool
int frame_queue_free_elem(frame_elem_t *frame_elem);

// Get number of available elements in pool
int frame_queue_pool_get_available(frame_queue_ctrl_t *ctrl);
```

### FreeRTOS Queue Wrapper Interfaces

```c
// Send element to queue (supports ISR)
int frame_queue_send(QueueHandle_t xQueue,
                    const void *pvItemToQueue,
                    TickType_t xTicksToWait);

// Receive element from queue (supports ISR)
int frame_queue_receive(QueueHandle_t xQueue,
                       void *pvBuffer,
                       TickType_t xTicksToWait);

// Get number of messages waiting in queue
int frame_queue_messages_waiting(QueueHandle_t xQueue);

// Check if queue is full
int frame_queue_is_full(QueueHandle_t xQueue);
```

## Thread Safety Mechanism

### 1. FreeRTOS Queue Protection

All memory allocation and release operations are performed through FreeRTOS queues, naturally possessing thread safety characteristics:

```c
int frame_queue_send(QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait)
{
    BaseType_t ret;

    if (xPortIsInsideInterrupt()) {
        // Interrupt context processing
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
        ret = xQueueSendFromISR(xQueue, pvItemToQueue, &pxHigherPriorityTaskWoken);
        if (pxHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else {
        // Task context processing
        ret = xQueueSend(xQueue, pvItemToQueue, xTicksToWait);
    }

    return ret == pdTRUE ? 0 : -1;
}
```

### 2. Automatic Context Detection

The module automatically detects the current running context (task/interrupt) and selects the appropriate FreeRTOS API:

- Task context: Uses standard queue API
- Interrupt context: Uses FromISR version API and handles task switching

### 3. Memory Consistency Guarantee

- **Atomic Pointer Operations**: Queues store frame_elem_t pointers, and pointer read/write operations are atomic
- **Descriptor Protection**: Fixed fields in frame_elem_t are not modified after initialization
- **Callback Synchronization**: Callback functions execute serially during the release process

## Performance Characteristics

### 1. Time Complexity

- **Allocation Operation**: O(1) - Queue dequeue operation
- **Release Operation**: O(1) - Queue enqueue operation
- **Status Query**: O(1) - Direct queue status read

### 2. Memory Efficiency

- **Fixed Allocation**: Avoids malloc/free overhead and fragmentation
- **Compact Layout**: Descriptors and data are in the same memory block, cache-friendly
- **Zero Copy**: Avoids data copying through pointer passing

### 3. Real-time Guarantee

- **Deterministic Latency**: All operations have fixed time complexity
- **Interrupt Support**: Fully supports operations in interrupt context
- **Non-blocking Option**: Supports non-blocking allocation and release

## Application Scenarios and Usage Patterns

### 1. Basic Memory Pool Usage

```c
// System initialization
static uint8_t g_frame_buffer[32][2048];
frame_queue_ctrl_init_cfg_t cfg = {
    .frame_buff = g_frame_buffer,
    .frame_type = 0xA5,
    .frame_elem_cnt = 32,
    .frame_elem_size = 2048,
    .name = "main_pool"
};
frame_queue_ctrl_t *pool = frame_queue_create(&cfg);

// Use memory blocks
frame_elem_t *elem;
if (frame_queue_alloc_elem(pool, &elem, pdMS_TO_TICKS(100)) == 0) {
    // Use memory
    generate_data(elem->buff_addr, &elem->data_size);
    process_data(elem);
    // Free memory
    frame_queue_free_elem(elem);
}
```

### 2. Producer-Consumer Pattern

```c
// Producer task
void producer_task(void *param)
{
    frame_elem_t *elem;
    while (1) {
        if (frame_queue_alloc_elem(g_pool, &elem, portMAX_DELAY) == 0) {
            // Generate data
            produce_data(elem->buff_addr, &elem->data_size);
            // Send to output queue
            frame_queue_send(g_pool->output_queue, &elem, portMAX_DELAY);
        }
    }
}

// Consumer task
void consumer_task(void *param)
{
    frame_elem_t *elem;
    while (1) {
        if (frame_queue_receive(g_pool->output_queue, &elem, portMAX_DELAY) == 0) {
            // Process data
            process_data(elem->buff_addr, elem->data_size);
            // Free memory
            frame_queue_free_elem(elem);
        }
    }
}
```

### 3. Callback Function Applications

```c
// Statistics callback function
void frame_stats_callback(frame_elem_t *frame_elem, void *arg)
{
    stats_t *stats = (stats_t *)arg;
    stats->free_count++;
    stats->total_bytes_freed += frame_elem->data_size;
}

// Register callback
frame_queue_free_cb_register(pool,
                             NULL, NULL,                    // before_free_cb
                             frame_stats_callback, &stats); // after_free_cb
```

## Debugging and Monitoring

### 1. Pool Status Monitoring

```c
void print_pool_status(frame_queue_ctrl_t *ctrl)
{
    int available = frame_queue_pool_get_available(ctrl);
    int total = ctrl->frame_elem_cnt;
    int used = total - available;

    printf("Pool [%s]: %d/%d used (%.1f%%)\n",
           ctrl->name, used, total,
           (float)used / total * 100);
}
```

### 2. Memory Leak Detection

```c
int check_memory_leaks(frame_queue_ctrl_t *ctrl)
{
    int available = frame_queue_pool_get_available(ctrl);
    int expected = ctrl->frame_elem_cnt;

    if (available != expected) {
        printf("MEMORY LEAK: Pool [%s] missing %d blocks\n",
               ctrl->name, expected - available);
        return expected - available;
    }
    return 0;
}
```

### 3. Queue Status Query

```c
void print_queue_status(frame_queue_ctrl_t *ctrl)
{
    printf("Pool Queue: %d/%d\n",
           frame_queue_messages_waiting(ctrl->pool_queue),
           ctrl->frame_elem_cnt);
    printf("Output Queue: %d/%d\n",
           frame_queue_messages_waiting(ctrl->output_queue),
           ctrl->frame_elem_cnt);
    printf("Input Queue: %d/%d\n",
           frame_queue_messages_waiting(ctrl->input_queue),
           ctrl->frame_elem_cnt);
}
```

## Design Advantages

1. **Simple and Efficient**: Clean design based on FreeRTOS queues, easy to understand and maintain
2. **Zero Fragmentation**: Fixed-size allocation completely avoids memory fragmentation issues
3. **Self-descriptive**: Each memory block contains complete metadata, supporting self-management
4. **Multi-queue Flexibility**: Three queues support complex data flow management
5. **Callback Extension**: Supports flexible extension functionality through callback functions
6. **Thread Safety**: Fully supports multi-threaded and interrupt environments
7. **Real-time Guarantee**: All operations have deterministic time complexity

This module provides a reliable and efficient memory management foundation for the entire message routing system and is a key component for implementing zero-copy data transmission.
