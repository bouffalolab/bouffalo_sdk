# 内存管理

## 内存层次

Bouffalo SDK 的内存层次从应用层到硬件层，包括：

```
应用内存堆 (TLSF/Freertos Heap)
   ↓
PSRAM (外部，可选)
   ↓
SRAM (内部，高速）
   ↓
Flash (代码存储，XIP）
```

## 芯片内存配置

| 芯片系列 | Flash | SRAM | PSRAM |
|---------|-------|------|-------|
| BL602/BL604 | 2MB | 320KB | - |
| BL702/BL704/BL706 | 2MB | 320KB | - |
| BL616/BL618 | 4MB | 640KB | √ |
| BL616D | 4MB | 640KB | √ |
| BL616L | 2MB | 320KB | √ |
| BL808 | 16MB | 8MB | √ |

## 内存管理方案

### 1. TLSF (Two-Level Segregated Fit)

**特性：**
- 实时性：确定性的内存分配/释放时间
- 低碎片：良好的内存利用率
- 适合嵌入式系统

**使用：**
```c
#include "mm/mm.h"

// 分配内存
void *ptr = mm_alloc(1024);
if (ptr == NULL) {
    printf("Alloc failed\n");
    return -1;
}

// 使用内存
memset(ptr, 0, 1024);

// 释放内存
mm_free(ptr);
```

**配置：**
```kconfig
config CONFIG_TLSF
    bool "Enable TLSF"
    default y

config CONFIG_HEAP_SIZE
    int "Heap size"
    default 65536
```

**统计信息：**
```c
// 查看内存统计
mm_print_stats();

// 输出示例：
// Total: 65536, Used: 1024, Free: 64512
```

### 2. FreeRTOS Heap

**特性：**
- 简单、可靠
- 与FreeRTOS集成
- 支持堆5种分配策略

**使用：**
```c
#include "FreeRTOS.h"
#include "task.h"

// 分配内存
void *ptr = pvPortMalloc(1024);
if (ptr == NULL) {
    printf("Malloc failed\n");
    return -1;
}

// 使用内存
memset(ptr, 0, 1024);

// 释放内存
vPortFree(ptr);
```

**堆策略：**

| 策略 | 说明 |
|-----|------|
| heap_1 | 最简单，不支持释放 |
| heap_2 | 支持释放，不合并碎片 |
| heap_3 | 支持释放，合并相邻空闲块 |
| heap_4 | 合并相邻空闲块，最佳选择 |
| heap_5 | 带内存对齐，适合性能要求高 |

**配置：**
```kconfig
config CONFIG_FREERTOS_HEAP
    int "FreeRTOS heap type"
    default 4  # heap_4
```

### 3. NuttX MM

**特性：**
- 虚拟内存支持
- 分页管理
- 保护机制

**使用：**
```c
#include <nuttx/mm/mm.h>

// 分配内存
void *ptr = mm_malloc(1024);
if (ptr == NULL) {
    printf("Malloc failed\n");
    return -1;
}

// 释放内存
mm_free(ptr);
```

## Flash 管理

### Flash 特性

- **读/写/擦除**
- **XIP（eXecute-In-Place）**: 代码直接从Flash执行
- **加密支持**
- **保护机制**

### Flash 操作

```c
#include "bflb_flash.h"

// 读取
uint8_t buf[256];
bflb_flash_read(0x1000, buf, 256);

// 擦除（4KB对齐）
bflb_flash_erase(0x1000, 0x1000);

// 写入（256字节对齐）
uint8_t data[] = {0x11, 0x22, 0x33};
bflb_flash_write(0x1000, data, 3);
```

### Flash 分区

**分区类型：**

| 分区 | 说明 |
|-----|------|
| boot2 | 二级启动代码 |
| dts | 设备树 |
| ptb | 分区表 |
| FW | 固件 |
| PS | 参数存储 |
| OTA | OTA固件 |
| FS | 文件系统 |

**分区配置：**
```ini
[PTB]
# Partition Table
boot2: 0x00, 0x20, 0x00, 0x10
dts:   0x00, 0x30, 0x00, 0x10
ptb:   0x00, 0x40, 0x00, 0x10
FW:    0x00, 0x50, 0x00, 0x100
PS:    0x01, 0x50, 0x00, 0x20
OTA:   0x01, 0x70, 0x00, 0x100
FS:    0x02, 0x70, 0x00, 0x400
```

### Flash 加密

**特性：**
- AES-256加密
- 硬件加速
- 透明加密

**配置：**
```kconfig
config CONFIG_FLASH_ENCRYPTION
    bool "Enable Flash encryption"
    default n

config CONFIG_FLASH_KEY
    hex "Flash encryption key"
    default 0x00000000000000000000000000000000
```

## PSRAM 管理

### PSRAM 特性

- **外部存储**: 扩展内存空间
- **高速**: 类似SRAM速度
- **XIP支持**: 代码可从PSRAM执行

### PSRAM 配置

```kconfig
config CONFIG_PSRAM
    bool "Enable PSRAM"
    default y if BL616 || BL808

config CONFIG_PSRAM_SIZE
    int "PSRAM size (MB)"
    default 8
```

### PSRAM 使用

```c
#include "bflb_psram.h"

// 初始化PSRAM
bflb_psram_init();

// 映射PSRAM到地址空间
uint8_t *psram_base = bflb_psram_map();

// 使用PSRAM
memcpy(psram_base, data, len);

// 取消映射
bflb_psram_unmap();
```

### PSRAM 分配

**使用TLSF分配PSRAM：**
```c
// 配置TLSF使用PSRAM
mm_add_pool(psram_base, psram_size);

// 从PSRAM分配
void *ptr = mm_alloc(1024);
```

## 内存对齐

### 对齐要求

不同处理器和内存类型有对齐要求：

| 数据类型 | 对齐要求 |
|---------|---------|
| char | 1 |
| short | 2 |
| int | 4 |
| long | 4/8 |
| float | 4 |
| double | 8 |
| 指针 | 4/8 |

### 对齐分配

```c
// 使用TLSF对齐分配
void *ptr = mm_aligned_alloc(1024, 64);  // 64字节对齐

// 使用FreeRTOS对齐分配
void *ptr = pvPortMallocAligned(1024, 64);
```

## 内存优化

### 1. 减少堆使用

```c
// 使用静态分配代替动态分配
static uint8_t buffer[1024];

// 或使用栈分配
void function(void)
{
    uint8_t buffer[1024];  // 栈分配
    // ...
}
```

### 2. 复用内存

```c
// 内存池
static uint8_t pool[1024];
static uint8_t used = 0;

void* alloc_from_pool(size_t size)
{
    if (used + size > sizeof(pool)) {
        return NULL;
    }

    void *ptr = pool + used;
    used += size;
    return ptr;
}
```

### 3. 使用const

```c
// 常量数据放入Flash
const uint8_t lookup_table[256] = {...};
```

### 4. 编译器优化

```c
// 使用__attribute__((section()))指定段
static uint8_t fast_data[1024] __attribute__((section(".fast_data")));

// 使用__attribute__((aligned()))指定对齐
static uint8_t aligned_data[1024] __attribute__((aligned(64)));
```

## 内存调试

### 内存泄漏检测

**TLSF统计：**
```c
// 分配前
mm_print_stats();

// 分配内存
void *ptr = mm_alloc(1024);

// 分配后
mm_print_stats();

// 应该看到Used增加
```

**FreeRTOS统计：**
```c
// 查看堆使用情况
size_t free_heap = xPortGetFreeHeapSize();
printf("Free heap: %d\n", free_heap);

// 查看任务栈使用
vTaskList(task_list);
printf("%s\n", task_list);
```

### 内存破坏检测

**使用Magic Number：**
```c
#define MAGIC 0xDEADBEEF

struct alloc_block {
    uint32_t magic;
    void *ptr;
    size_t size;
    uint32_t magic2;
};

void check_memory(struct alloc_block *block)
{
    if (block->magic != MAGIC || block->magic2 != MAGIC) {
        printf("Memory corruption detected!\n");
    }
}
```

### 堆栈溢出检测

**FreeRTOS任务堆栈检测：**
```kconfig
config CONFIG_FREERTOS_STACK_OVERFLOW_CHECK
    bool "Enable stack overflow check"
    default y
```

```c
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    printf("Stack overflow: %s\n", pcTaskName);
}
```

## 低功耗内存管理

### 休眠时内存处理

```c
// 进入休眠前
void enter_sleep(void)
{
    // 保存关键数据到Flash
    save_to_flash();

    // 清空缓存
    mm_clean_cache();

    // 进入休眠
    pmu_enter_sleep();
}
```

### 唤醒时恢复

```c
// 唤醒后
void wake_up(void)
{
    // 从Flash恢复数据
    restore_from_flash();

    // 初始化内存
    mm_init();
}
```

## 内存映射

### 内存布局

```
0x00000000: Flash (XIP)
0x20000000: SRAM
0x30000000: PSRAM (如果启用）
0x40000000: 外设寄存器
```

### 查看内存映射

```bash
# 使用objdump查看段
riscv64-unknown-elf-objdump -h build/build_out/my_project.elf

# 使用map文件查看地址
cat build/build_out/my_project.map
```

## 最佳实践

### 1. 选择合适的内存管理方案

```kconfig
# 简单应用使用FreeRTOS Heap
# 实时性要求高的应用使用TLSF
# 需要虚拟内存使用NuttX MM
```

### 2. 监控内存使用

```c
// 定期检查内存使用
void monitor_memory(void)
{
    mm_print_stats();
    printf("Free heap: %d\n", xPortGetFreeHeapSize());
}
```

### 3. 处理分配失败

```c
void *ptr = mm_alloc(size);
if (ptr == NULL) {
    // 优雅处理分配失败
    printf("Out of memory\n");
    // 清理资源
    cleanup();
    return;
}
```

### 4. 避免碎片

```c
// 避免频繁分配/释放不同大小的内存
// 使用内存池或固定大小分配器
static uint8_t pool[1024];
mm_add_pool(pool, sizeof(pool));
```

### 5. 使用XIP减少RAM占用

```kconfig
# 将代码和数据放入Flash
CONFIG_XIP=y
```

## 总结

Bouffalo SDK提供多种内存管理方案，包括TLSF、FreeRTOS Heap、NuttX MM。选择合适的方案，合理配置内存大小，监控内存使用，避免内存泄漏和溢出。对于大内存需求，启用PSRAM扩展内存空间。
