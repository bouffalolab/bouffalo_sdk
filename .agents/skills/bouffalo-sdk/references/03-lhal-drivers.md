# LHAL 统一外设驱动架构

## LHAL 概述

LHAL（Low Level Hardware Abstraction Layer）是 Bouffalo Lab 设计的低级别硬件抽象层，为所有 Bouffalo 芯片提供统一的通用外设驱动 API。

### 设计目标

- **统一API**: 所有芯片使用相同的外设接口
- **易于移植**: 应用代码可在不同芯片间快速移植
- **性能优化**: 最小抽象开销，支持硬件加速
- **向后兼容**: 新芯片保持API兼容

### 架构层次

```
┌─────────────────────────────────────┐
│      应用代码 (Application)          │
└─────────────────────────────────────┘
               ↓
┌─────────────────────────────────────┐
│    LHAL 统一API (bflb_xxx_*)        │
│  - bflb_uart_init()                 │
│  - bflb_gpio_init()                 │
│  - bflb_dma_init()                  │
└─────────────────────────────────────┘
               ↓
┌─────────────────────────────────────┐
│     核心抽象层 (Core Abstraction)   │
└─────────────────────────────────────┘
               ↓
┌─────────────────────────────────────┐
│    设备表 (Device Table)            │
└─────────────────────────────────────┘
               ↓
┌─────────────────────────────────────┐
│  芯片配置层 (drivers/lhal/config/)  │
│  - bl602/, bl702/, bl616/, bl618dg/ │
└─────────────────────────────────────┘
               ↓
┌─────────────────────────────────────┐
│   SOC驱动层 (drivers/soc/)          │
└─────────────────────────────────────┘
               ↓
┌─────────────────────────────────────┐
│      硬件寄存器 (Hardware)          │
└─────────────────────────────────────┘
```

## API 设计模式

### 标准API模式

所有外设驱动遵循统一的API模式：

```
配置 (Config) → 初始化 (Init) → 控制 (Control) → I/O (Input/Output)
```

#### 1. 配置 (Configuration)

定义外设的配置参数：

```c
struct bflb_uart_config {
    uint32_t baudrate;      // 波特率
    uint8_t  data_bits;     // 数据位
    uint8_t  stop_bits;     // 停止位
    uint8_t  parity;        // 奇偶校验
    bool      flow_control; // 流控
};
```

#### 2. 初始化 (Initialization)

初始化外设并返回设备句柄：

```c
bflb_device_t* bflb_uart_init(uint8_t id, const struct bflb_uart_config *cfg);
```

#### 3. 控制 (Control)

运行时控制外设：

```c
int bflb_uart_control(bflb_device_t *dev, int cmd, void *arg);
```

#### 4. I/O (Input/Output)

数据传输接口：

```c
// 同步I/O
int bflb_uart_write(bflb_device_t *dev, const uint8_t *data, size_t len);
int bflb_uart_read(bflb_device_t *dev, uint8_t *data, size_t len);

// 异步I/O
int bflb_uart_write_async(bflb_device_t *dev, const uint8_t *data, size_t len, callback_t cb);
int bflb_uart_read_async(bflb_device_t *dev, uint8_t *data, size_t len, callback_t cb);
```

## 核心外设驱动

### 通信类外设

#### UART (通用异步收发器)

**特性：**
- 多路UART支持
- 可配置数据位、停止位、奇偶校验
- 流控支持（RTS/CTS）
- DMA传输
- FIFO中断

**API示例：**
```c
#include "bflb_uart.h"

// 配置
struct bflb_uart_config cfg = {
    .baudrate = 115200,
    .data_bits = 8,
    .stop_bits = 1,
    .parity = UART_PARITY_NONE
};

// 初始化
bflb_device_t *uart = bflb_uart_init(0, &cfg);

// 发送
bflb_uart_write(uart, "Hello", 5);

// 接收
char buf[100];
int len = bflb_uart_read(uart, buf, sizeof(buf));
```

#### SPI (串行外设接口)

**特性：**
- 主机/从机模式
- 可配置时钟极性、相位
- 多个片选
- DMA传输
- 全双工/半双工

**API示例：**
```c
#include "bflb_spi.h"

struct bflb_spi_config cfg = {
    .clk_freq = 10000000,
    .mode = SPI_MODE_0,
    .bit_order = SPI_BIT_ORDER_MSB
};

bflb_device_t *spi = bflb_spi_init(0, &cfg);

// 传输
uint8_t tx_buf[] = {0xAA, 0xBB};
uint8_t rx_buf[2];
bflb_spi_transfer(spi, tx_buf, rx_buf, 2);
```

#### I2C (I2C总线)

**特性：**
- 标准/快速/高速模式
- 主机/从机模式
- 多主机支持
- DMA传输

**API示例：**
```c
#include "bflb_i2c.h"

struct bflb_i2c_config cfg = {
    .clk_freq = 400000,
    .addr_bits = 7
};

bflb_device_t *i2c = bflb_i2c_init(0, &cfg);

// 写入
uint8_t data[] = {0x10, 0x20};
bflb_i2c_write(i2c, 0x50, data, 2);

// 读取
uint8_t buf[10];
bflb_i2c_read(i2c, 0x50, buf, sizeof(buf));
```

#### I2S (I2S音频接口)

**特性：**
- 主机/从机模式
- 可配置采样率、位深
- 立体声/单声道
- DMA传输

**API示例：**
```c
#include "bflb_i2s.h"

struct bflb_i2s_config cfg = {
    .sample_rate = 44100,
    .bit_depth = 16,
    .channels = 2
};

bflb_device_t *i2s = bflb_i2s_init(0, &cfg);
```

### 定时类外设

#### TIMER (定时器)

**特性：**
- 多路独立定时器
- 定时中断
- PWM输出
- 输入捕获

**API示例：**
```c
#include "bflb_timer.h"

struct bflb_timer_config cfg = {
    .period = 1000,
    .reload = 1000
};

bflb_device_t *timer = bflb_timer_init(0, &cfg);
bflb_timer_start(timer);
```

#### PWM (脉冲宽度调制)

**特性：**
- PWM v1 (BL602/BL702)
- PWM v2 (BL616/BL618DG)
- 多路独立输出
- 可配置频率、占空比

**API示例：**
```c
#include "bflb_pwm_v2.h"

struct bflb_pwm_v2_config cfg = {
    .clk_freq = 1000000,
    .period = 1000
};

bflb_device_t *pwm = bflb_pwm_v2_init(0, &cfg);
bflb_pwm_v2_set_duty(pwm, 0, 500); // 50%
bflb_pwm_v2_start(pwm);
```

#### RTC (实时时钟)

**特性：**
- 闹钟功能
- 周期性中断
- 低功耗模式保持

**API示例：**
```c
#include "bflb_rtc.h"

// 设置时间
struct bflb_rtc_time time = {
    .year = 2024,
    .month = 1,
    .day = 1,
    .hour = 12,
    .minute = 0,
    .second = 0
};

bflb_rtc_set_time(&time);

// 设置闹钟
struct bflb_rtc_alarm alarm = {
    .hour = 12,
    .minute = 30
};
bflb_rtc_set_alarm(&alarm, alarm_callback);
```

### 存储类外设

#### FLASH (内部Flash)

**特性：**
- 读、写、擦除
- XIP（执行-in-Place）
- 加密支持

**API示例：**
```c
#include "bflb_flash.h"

// 读取
uint8_t buf[256];
bflb_flash_read(0x1000, buf, 256);

// 擦除
bflb_flash_erase(0x2000, 0x1000);

// 写入
uint8_t data[] = {0x11, 0x22};
bflb_flash_write(0x1000, data, 2);
```

#### DMA (直接内存访问)

**特性：**
- 多路DMA通道
- 内存到内存、内存到外设、外设到内存
- 链式传输
- 中断完成

**API示例：**
```c
#include "bflb_dma.h"

struct bflb_dma_config cfg = {
    .src_addr = (uint32_t)src_buf,
    .dst_addr = (uint32_t)dst_buf,
    .len = 1024,
    .int_en = true
};

bflb_device_t *dma = bflb_dma_init(0, &cfg);
bflb_dma_start(dma);
```

### GPIO (通用输入输出)

**特性：**
- 多路GPIO
- 输入/输出模式
- 中断支持（上升沿、下降沿、双边沿）
- 上拉/下拉

**API示例：**
```c
#include "bflb_gpio.h"

// 配置为输出
bflb_gpio_init(0, GPIO_PIN_0, GPIO_OUTPUT);
bflb_gpio_set(0, GPIO_PIN_0);

// 配置为输入
bflb_gpio_init(0, GPIO_PIN_1, GPIO_INPUT | GPIO_PULLUP);
int val = bflb_gpio_read(0, GPIO_PIN_1);

// 配置中断
bflb_gpio_init(0, GPIO_PIN_2, GPIO_INPUT | GPIO_INT_EDGE_RISING);
bflb_gpio_irq_attach(0, GPIO_PIN_2, gpio_callback, NULL);
bflb_gpio_irq_enable(0, GPIO_PIN_2, true);
```

### ADC (模数转换器)

**特性：**
- 多通道ADC
- 可配置分辨率
- 轮询、中断、DMA模式
- 内部参考电压

**API示例：**
```c
#include "bflb_adc.h"

struct bflb_adc_config cfg = {
    .clk_freq = 2000000,
    .vref = ADC_VREF_3P3V,
    .conv_mode = ADC_CONV_MODE_SINGLE
};

bflb_device_t *adc = bflb_adc_init(0, &cfg);

// 读取
uint16_t value = bflb_adc_read(adc, ADC_CHANNEL_0);
float voltage = bflb_adc_parse_result(adc, value);
```

### 安全类外设

#### SEC_AES (AES加密)

**特性：**
- ECB、CBC、CTR模式
- 128/192/256位密钥
- DMA加速

**API示例：**
```c
#include "bflb_sec_aes.h"

uint8_t key[16] = {0x00, 0x01, ...};
uint8_t plaintext[16] = {0xAA, 0xBB, ...};
uint8_t ciphertext[16];

bflb_sec_aes_encrypt(key, 128, plaintext, ciphertext, AES_MODE_ECB);
```

#### SEC_SHA (哈希)

**特性：**
- SHA-1、SHA-224、SHA-256
- DMA加速

**API示例：**
```c
#include "bflb_sec_sha.h"

uint8_t data[100];
uint8_t hash[32];

bflb_sec_sha256(data, sizeof(data), hash);
```

#### SEC_TRNG (真随机数生成器)

**特性：**
- 硬件真随机数
- 连续输出

**API示例：**
```c
#include "bflb_sec_trng.h"

uint32_t rand = bflb_sec_trng_read();
```

## 中断管理

### IRQ (中断控制器)

**特性：**
- 中断优先级
- 中断嵌套
- 中断向量表

**API示例：**
```c
#include "bflb_irq.h"

void uart_isr(void)
{
    bflb_irq_clear(IRQ_UART0);
    // 处理中断
}

bflb_irq_attach(IRQ_UART0, uart_isr, NULL);
bflb_irq_enable(IRQ_UART0);
```

## 跨芯片兼容性

### 设备表机制

LHAL使用设备表实现硬件无关的设备访问：

```c
// 设备表定义
struct bflb_device uart0_dev = {
    .name = "uart0",
    .priv = &bl616_uart0_config,
    .ops = &uart_ops
};

// 统一访问
bflb_device_t *dev = bflb_device_get_by_name("uart0");
bflb_uart_write(dev, "Hello", 5);
```

### 芯片配置层

每个芯片在 `drivers/lhal/config/blxxx/` 下有特定的配置：

```
drivers/lhal/config/
├── bl602/      # BL602配置
├── bl702/      # BL702配置
├── bl616/      # BL616配置
└── bl618dg/    # BL618DG配置
```

## 性能优化

### ROM API

部分LHAL函数固化在ROM中，节省Flash空间：

```c
// 使用ROM API（性能更好）
bflb_flash_read_rom(addr, buf, len);

// 使用Flash API（可更新）
bflb_flash_read(addr, buf, len);
```

### DMA加速

大多数外设支持DMA传输，减少CPU负载：

```c
struct bflb_dma_config cfg = {
    .src_addr = (uint32_t)src,
    .dst_addr = (uint32_t)dst,
    .len = 1024,
    .int_en = true
};

bflb_dma_init(0, &cfg);
bflb_dma_start(0);
```

## 最佳实践

### 1. 使用设备表

```c
// 推荐：使用设备名
bflb_device_t *uart = bflb_device_get_by_name("uart0");

// 不推荐：直接使用ID
bflb_uart_init(0, &cfg);
```

### 2. 配置检查

```c
if (bflb_uart_init(0, &cfg) == NULL) {
    printf("UART init failed\n");
    return -1;
}
```

### 3. 资源释放

```c
bflb_uart_deinit(uart);
```

### 4. 错误处理

```c
int ret = bflb_uart_write(uart, data, len);
if (ret < 0) {
    // 处理错误
}
```

## 总结

LHAL提供了统一的硬件抽象层，使得应用代码可以在不同Bouffalo芯片间无缝移植。通过遵循标准的API模式和使用设备表机制，开发者可以编写可移植、高性能的嵌入式应用。
