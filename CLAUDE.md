# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

这是 Bouffalo Lab 的 Bouffalo SDK，一个支持所有 Bouffalo 芯片系列的 IoT 和 MCU 软件开发套件。该项目结合了 bl_mcu_sdk 和 bl_iot_sdk，提供完整的嵌入式开发解决方案。

### 支持的芯片系列
- **BL602/BL604** - WiFi + Bluetooth LE
- **BL702/BL704/BL706** - Zigbee + Thread
- **BL616/BL618** - WiFi 6 + Bluetooth LE
- **BL618DG** - 多核芯片
- **BL616CL** - 专用变体

## 快速导航

- [根目录导航](../CLAUDE.md) - 项目整体概览和环境设置
- [WiFi 驱动组件](components/wireless/wl80211/CLAUDE.md) - wl80211 WiFi 驱动详细文档

## 项目架构

### 核心目录结构
```
bouffalo_sdk/
├── drivers/
│   ├── lhal/              # 低级别硬件抽象层（通用外设）
│   └── soc/               # 芯片特定驱动（非通用外设）
├── components/
│   ├── wireless/          # 无线协议栈（WiFi、BLE、Zigbee、Thread）
│   ├── net/               # 网络协议（lwIP、MQTT、HTTP）
│   ├── fs/                # 文件系统（FATFS、LittleFS、ROMFS）
│   ├── multimedia/        # 多媒体组件（音频、视频）
│   └── debug/             # 调试工具
├── bsp/                   # 板级支持包（时钟、引脚、内存）
├── examples/              # 官方示例代码
├── tools/                 # 开发工具（调试、烧录、分析）
└── tests/                 # 单元测试（Unity 框架）
```

### LHAL 架构
LHAL（Low Level HAL）是 Bouffalo Lab 设计的通用外设低级别硬件抽象层：
- **统一的 API** - 所有 Bouffalo 芯片使用相同的驱动接口
- **外设覆盖** - GPIO、UART、SPI、I2C、PWM、ADC、DAC、定时器等
- **移植性** - 应用代码可在不同芯片间无缝迁移

### 支持的无线协议
- **WiFi 4/6** - IEEE 802.11 a/b/g/n/ac/ax（完整协议栈）
- **Bluetooth/BLE** - Bluetooth 5.x（GATT、GAP、Mesh）
- **Zigbee** - IEEE 802.15.4（ZCL、Zigbee 3.0）
- **Thread** - 基于 802.15.4 的 IPv6 mesh 网络

## 核心开发原则

### 1. 代码风格
遵循嵌入式 C 语言最佳实践：

```c
// 函数命名：模块_动作_对象
int wifi_mgmr_sta_connect(const wifi_mgmr_sta_connect_params_t *params);
int uart_send_data(uint8_t *data, uint32_t len);

// 变量命名：小写 + 下划线
uint32_t wifi_channel = 6;
uint8_t *rx_buffer = NULL;

// 宏定义：大写 + 下划线
#define WIFI_MAX_CHANNEL    14
#define UART_BUFFER_SIZE    256

// 结构体：下划线后缀 _t
typedef struct {
    uint8_t ssid[32];
    uint8_t password[64];
} wifi_connect_params_t;
```

### 2. 错误处理模式
统一使用返回值表示错误：

```c
// 返回 0 表示成功，负值表示错误
int ret = wifi_mgmr_sta_connect(&params);
if (ret < 0) {
    printf("WiFi connect failed: %d\r\n", ret);
    return ret;
}

// 使用错误码常量
#define WIFI_ERR_TIMEOUT    -1
#define WIFI_ERR_MEMORY     -2
#define WIFI_ERR_PARAM      -3
```

### 3. 内存管理最佳实践
```c
// 优先使用静态分配（避免碎片化）
static uint8_t rx_buffer[RX_BUFFER_SIZE];

// 动态分配必须检查返回值
uint8_t *buffer = malloc(size);
if (buffer == NULL) {
    printf("Memory allocation failed\r\n");
    return -1;
}

// 及时释放内存
free(buffer);
buffer = NULL;  // 防止悬空指针
```

### 4. RTOS 任务设计
```c
// 合理设置任务优先级（避免优先级反转）
#define WIFI_TASK_PRIORITY     4
#define UART_TASK_PRIORITY     3
#define SENSOR_TASK_PRIORITY   2

// 任务栈大小要预留余量
#define WIFI_TASK_STACK_SIZE   1024  // bytes

// 使用互斥锁保护共享资源
static pthread_mutex_t wifi_mutex;
pthread_mutex_lock(&wifi_mutex);
// 访问共享资源
pthread_mutex_unlock(&wifi_mutex);
```

### 5. 功耗管理
```c
// 空闲时进入低功耗模式
void app_idle_task(void) {
    while (1) {
        // 等待事件或超时
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));

        // 进入低功耗模式
        __asm__ volatile("wfi");
    }
}

// WiFi 省电模式
wifi_mgmr_sta_ps_enter();  // 进入省电模式
wifi_mgmr_sta_ps_exit();   // 退出省电模式
```

## 常用构建命令

### 基本构建
```bash
# 进入示例目录并构建
cd examples/helloworld
make CHIP=bl616 BOARD=bl616dk

# 使用 ninja 构建（更快）
make ninja CHIP=bl616 BOARD=bl616dk

# 多核芯片构建（BL618DG）
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap  # AP 核心
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=np  # NP 核心
```

### 烧录命令
```bash
# 基本烧录
make flash CHIP=bl616 COMX=/dev/ttyUSB0

# 多核芯片烧录
make flash CHIP=bl618dg CPU_ID=ap COMX=/dev/ttyUSB0

# 指定波特率
make flash CHIP=bl616 COMX=/dev/ttyUSB0 BAUDRATE=2000000
```

### 清理和重新构建
```bash
# 清理构建产物
make clean

# 完全清理（包括配置）
make distclean

# 重新配置
make menuconfig CHIP=bl616 BOARD=bl616dk
```

## 开发环境设置

### 工具链要求
- **riscv64-unknown-elf-gcc** - RISC-V 交叉编译工具链
- **make** 或 **ninja** - 构建工具
- **CKLink 调试器** - 硬件调试（可选）
- **Python 3.x** - 脚本工具支持

### 配置系统
- **Kconfig** - 配置管理（类似 Linux）
- **menuconfig** - 图形化配置界面
- **proj.conf** - 项目配置文件

```bash
# 打开配置菜单
make menuconfig CHIP=bl616 BOARD=bl616dk
```

## Git 工作流

### 分支命名规范
```
feature/wifi-enhancement      # 新功能
bugfix/uart-timeout-error     # Bug 修复
refactor/driver-architecture  # 代码重构
docs/api-documentation        # 文档更新
```

### 提交信息格式
```
[模块] 简短描述（50字符以内）

详细说明（可选）：
- 修改了什么
- 为什么修改
- 影响范围

示例：
[wifi] 添加 WiFi 6 支持

- 实现 802.11ax 协议栈
- 添加 OFDMA 调度器
- 优化功耗管理
```

### 代码审查标准
每次提交前检查：
- ✅ 编译通过（无警告）
- ✅ 代码风格一致
- ✅ 错误处理完整
- ✅ 内存管理正确（无泄漏）
- ✅ 注释清晰（复杂逻辑）
- ✅ 测试覆盖（关键功能）

## 测试和验证

### 单元测试
- **位置** - `tests/` 目录
- **框架** - Unity 测试框架
- **运行** - `make test CHIP=bl616`

```c
// 示例测试用例
void test_wifi_connect_success(void) {
    wifi_connect_params_t params = {
        .ssid = "TestNetwork",
        .password = "password123"
    };

    int ret = wifi_mgmr_sta_connect(&params);
    TEST_ASSERT_EQUAL_INT(0, ret);
}
```

### 示例验证
每个外设都有对应示例：
- **基础示例** - helloworld、gpio_blink、uart_echo
- **外设示例** - spi_master、i2c_slave、pwm_led
- **无线示例** - wifi_station、wifi_ap、ble_peripheral
- **系统示例** - freertos_demo、psram_test、lowpower

## 调试支持

### CKLink 调试器
- **工具位置** - `tools/cklink/`
- **GDB 脚本** - `tools/gdbinit/`
- **Eclipse 配置** - `tools/eclipse_gdb_launch/`

### 调试命令
```bash
# 启动 GDB 调试
riscv64-unknown-elf-gdb build/out.elf
(gdb) target remote localhost:1025
(gdb) break main
(gdb) continue
```

### 崩溃转储分析
```bash
# 使用崩溃分析工具
python tools/crash_tools/crash_analyzer.py crash_dump.bin
```

### 日志调试
```c
// 使用 log 组件
#include <log.h>

LOG_INFO("WiFi connected to %s", ssid);
LOG_WARNING("Low memory: %d bytes free", free_heap);
LOG_ERROR("UART send failed: %d", ret);
```

## 性能优化指南

### 内存优化
```c
// 1. 使用 const 修饰只读数据（放入 Flash）
const char *error_msg = "Error occurred";

// 2. 压缩数据结构（减少内存占用）
typedef struct __attribute__((packed)) {
    uint8_t channel;
    uint8_t rssi;
    uint16_t beacon_interval;
} wifi_ap_info_t;

// 3. 使用位域（节省空间）
typedef struct {
    uint8_t is_connected : 1;
    uint8_t is_scanning : 1;
    uint8_t reserved : 6;
} wifi_state_t;
```

### 功耗优化
```c
// 1. 减少唤醒频率
#define SENSOR_READ_INTERVAL_MS  5000  // 5秒读取一次

// 2. 关闭不必要的外设时钟
system_clock_disable(CLK_UART1);

// 3. 使用中断而非轮询
gpio_enable_interrupt(GPIO_PIN_5, GPIO_TRIGGER_FALLING);
```

### 实时性优化
```c
// 1. 关键代码使用 IRAM_ATTR（放入 RAM）
void IRAM_ATTR timer_isr_handler(void) {
    // 快速响应的中断处理
}

// 2. 减少 ISR 执行时间
void uart_isr_handler(void) {
    uint8_t data = uart_read_byte();
    xQueueSendFromISR(uart_queue, &data, NULL);  // 快速传递数据
}

// 3. 使用 DMA 传输大数据
spi_dma_transfer(tx_buffer, rx_buffer, length);
```

## 特殊注意事项

### 多核支持
- **BL618DG** 为多核芯片（AP + NP）
- **必须指定 CPU_ID** - `CPU_ID=ap` 或 `CPU_ID=np`
- **独立启动** - 每个核心有独立的启动代码和链接脚本
- **核间通信** - 使用共享内存或消息队列

### 内存管理
- **PSRAM 支持** - 扩展外部 RAM（高达 4MB）
- **内存分配器** - TLSF（Two-Level Segregated Fit）
- **内存布局** - 可配置的 SRAM/PSRAM 分配策略

### 文件系统支持
- **FATFS** - SD 卡、USB 存储
- **LittleFS** - Flash 文件系统（掉电安全）
- **ROMFS** - 只读文件系统（资源文件）

## 工具链文件位置
- **compile_commands.json** - 项目根目录（IDE 集成）
- **CMake 配置** - `cmake/` 目录
- **工具链配置** - `cmake/toolchain.cmake`
- **链接脚本** - `drivers/soc/bl616/ld/`

## 常见问题排查

### 编译错误：找不到头文件
**原因：** 未指定芯片类型

**解决：** 添加 `CHIP=bl616` 参数

### 链接错误：未定义的引用
**原因：** 组件未启用或缺少源文件

**解决：**
```bash
make menuconfig CHIP=bl616 BOARD=bl616dk
# 启用所需组件
```

### 运行时错误：HardFault
**原因：** 内存访问错误或栈溢出

**解决：**
1. 检查指针有效性
2. 增加任务栈大小
3. 使用 GDB 调试定位问题

## 相关文档

- [根目录导航](../CLAUDE.md) - 环境设置和项目概览
- [WiFi 驱动文档](components/wireless/wl80211/CLAUDE.md) - wl80211 详细 API
- [Bouffalo SDK 开发指南](.claude/skills/bl-dev/bouffalo-sdk) - AI 辅助开发技能
