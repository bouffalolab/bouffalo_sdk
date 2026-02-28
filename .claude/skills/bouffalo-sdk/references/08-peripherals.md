# 外设使用指南

## 外设概览

Bouffalo SDK 通过 LHAL 提供统一的外设 API，支持多种外设，包括 GPIO、UART、SPI、I2C、I2S、ADC、DAC、定时器、PWM、DMA 等。

## GPIO (通用输入输出)

### 特性

- 多路 GPIO（不同芯片数量不同）
- 输入/输出模式
- 中断支持（上升沿、下降沿、双边沿）
- 上拉/下拉/浮空

### 初始化

```c
#include "bflb_gpio.h"

// 配置为输出
bflb_gpio_init(0, GPIO_PIN_0, GPIO_OUTPUT);
bflb_gpio_set(0, GPIO_PIN_0);

// 配置为输入（上拉）
bflb_gpio_init(0, GPIO_PIN_1, GPIO_INPUT | GPIO_PULLUP);
int val = bflb_gpio_read(0, GPIO_PIN_1);
```

### 中断

```c
// 配置中断（上升沿）
bflb_gpio_init(0, GPIO_PIN_2, GPIO_INPUT | GPIO_INT_EDGE_RISING);

// 注册中断回调
void gpio_callback(void *arg)
{
    printf("GPIO interrupt\n");
}

bflb_gpio_irq_attach(0, GPIO_PIN_2, gpio_callback, NULL);
bflb_gpio_irq_enable(0, GPIO_PIN_2, true);
```

### 常见应用

- LED控制
- 按键检测
- 中断信号处理
- 外设片选

## UART (通用异步收发器)

### 特性

- 多路 UART
- 可配置数据位、停止位、奇偶校验
- 流控（RTS/CTS）
- DMA 传输
- FIFO 中断

### 初始化

```c
#include "bflb_uart.h"

struct bflb_uart_config cfg = {
    .baudrate = 115200,
    .data_bits = UART_DATABITS_8,
    .stop_bits = UART_STOPBITS_1,
    .parity = UART_PARITY_NONE
};

bflb_device_t *uart = bflb_uart_init(0, &cfg);
```

### 轮询模式

```c
// 发送
bflb_uart_write(uart, "Hello", 5);

// 接收
char buf[100];
int len = bflb_uart_read(uart, buf, sizeof(buf));
```

### 中断模式

```c
// FIFO中断示例
void uart_isr(void)
{
    bflb_uart_irq_handler(uart);
}

void uart_rx_callback(uint8_t *data, uint16_t len)
{
    printf("Received: %d bytes\n", len);
}

bflb_uart_irq_attach(uart, UART_RX_FIFO_INT, uart_rx_callback, NULL);
bflb_irq_attach(IRQ_UART0, uart_isr, NULL);
bflb_irq_enable(IRQ_UART0, true);
```

### DMA模式

```c
// DMA传输
struct bflb_dma_config dma_cfg = {
    .src_addr = (uint32_t)src_buf,
    .dst_addr = (uint32_t)dst_buf,
    .len = 1024,
    .int_en = true
};

bflb_device_t *dma = bflb_dma_init(0, &dma_cfg);
bflb_dma_start(dma);
```

### 常见应用

- 控制台输出
- 模块通信
- 调试日志
- 数据传输

## SPI (串行外设接口)

### 特性

- 主机/从机模式
- 可配置时钟极性、相位
- 多个片选
- DMA 传输
- 全双工/半双工

### 主机模式

```c
#include "bflb_spi.h"

struct bflb_spi_config cfg = {
    .clk_freq = 10000000,
    .mode = SPI_MODE_0,  // CPOL=0, CPHA=0
    .bit_order = SPI_BIT_ORDER_MSB,
    .data_size = SPI_DATASIZE_8BIT
};

bflb_device_t *spi = bflb_spi_init(0, &cfg);

// 传输（全双工）
uint8_t tx_buf[] = {0xAA, 0xBB};
uint8_t rx_buf[2];
bflb_spi_transfer(spi, tx_buf, rx_buf, 2);
```

### 从机模式

```c
// 配置为从机
struct bflb_spi_config cfg = {
    .master = false,
    .clk_freq = 10000000,
    .mode = SPI_MODE_0
};

bflb_device_t *spi = bflb_spi_init(0, &cfg);

// 等待传输
bflb_spi_slave_transfer(spi, tx_buf, rx_buf, len);
```

### 常见应用

- Flash 读写
- 传感器通信
- 显示屏驱动
- SD卡

## I2C (I2C总线)

### 特性

- 标准/快速/高速模式
- 主机/从机模式
- 多主机支持
- DMA 传输

### 主机模式

```c
#include "bflb_i2c.h"

struct bflb_i2c_config cfg = {
    .clk_freq = 400000,  // 400kHz
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

### 常见应用

- EEPROM
- 温度传感器
- 加速度计
- 实时时钟

## I2S (I2S音频接口)

### 特性

- 主机/从机模式
- 可配置采样率、位深
- 立体声/单声道
- DMA 传输

### 初始化

```c
#include "bflb_i2s.h"

struct bflb_i2s_config cfg = {
    .sample_rate = 44100,
    .bit_depth = 16,
    .channels = 2,  // 立体声
    .mode = I2S_MODE_MASTER
};

bflb_device_t *i2s = bflb_i2s_init(0, &cfg);

// 发送/接收
bflb_i2s_send(i2s, audio_data, data_len);
bflb_i2s_recv(i2s, audio_buf, buf_len);
```

### 常见应用

- 音频DAC/ADC
- 音频编解码器
- 蓝牙音频

## ADC (模数转换器)

### 特性

- 多通道 ADC
- 可配置分辨率
- 轮询/中断/DMA 模式
- 内部参考电压

### 轮询模式

```c
#include "bflb_adc.h"

struct bflb_adc_config cfg = {
    .clk_freq = 2000000,
    .vref = ADC_VREF_3P3V,
    .conv_mode = ADC_CONV_MODE_SINGLE
};

bflb_device_t *adc = bflb_adc_init(0, &cfg);

// 读取单次
uint16_t value = bflb_adc_read(adc, ADC_CHANNEL_0);
float voltage = bflb_adc_parse_result(adc, value);
```

### 中断模式

```c
// 配置中断
bflb_adc_irq_attach(adc, ADC_CONV_COMPLETE_INT, adc_callback, NULL);
bflb_adc_irq_enable(adc, ADC_CONV_COMPLETE_INT, true);

// 开始转换
bflb_adc_start(adc);
```

### DMA模式

```c
// DMA读取
bflb_adc_start_dma(adc, dma_channel, dma_buf, dma_len);
```

### 常见应用

- 电压测量
- 传感器读取
- 温度监测
- 电池电量

## DAC (数模转换器)

### 特性

- 多通道 DAC
- 可配置分辨率
- 轮询/中断/DMA 模式

### 使用

```c
#include "bflb_dac.h"

struct bflb_dac_config cfg = {
    .clk_freq = 2000000
};

bflb_device_t *dac = bflb_dac_init(0, &cfg);

// 设置输出
bflb_dac_set_value(dac, 2048);  // 12位：0-4095
```

### 常见应用

- 音频输出
- 电压发生器
- 波形生成

## 定时器

### 特性

- 多路独立定时器
- 定时中断
- PWM 输出
- 输入捕获

### 初始化

```c
#include "bflb_timer.h"

struct bflb_timer_config cfg = {
    .period = 1000,
    .reload = 1000
};

bflb_device_t *timer = bflb_timer_init(0, &cfg);
```

### 中断模式

```c
// 定时器回调
void timer_callback(void *arg)
{
    printf("Timer interrupt\n");
}

bflb_timer_irq_attach(timer, timer_callback, NULL);
bflb_timer_start(timer);
```

### 常见应用

- 周期性任务
- 精确定时
- 任务调度
- 看门狗

## PWM (脉冲宽度调制)

### 特性

- PWM v1 (BL602/BL702)
- PWM v2 (BL616/BL618DG)
- 多路独立输出
- 可配置频率、占空比

### PWM v1 (BL602/BL702)

```c
#include "bflb_pwm_v1.h"

struct bflb_pwm_v1_config cfg = {
    .clk_freq = 1000000,
    .period = 1000  // 1kHz
};

bflb_device_t *pwm = bflb_pwm_v1_init(0, &cfg);
bflb_pwm_v1_set_duty(pwm, 0, 500);  // 50%
bflb_pwm_v1_start(pwm);
```

### PWM v2 (BL616/BL618DG)

```c
#include "bflb_pwm_v2.h"

struct bflb_pwm_v2_config cfg = {
    .clk_freq = 1000000,
    .period = 1000
};

bflb_device_t *pwm = bflb_pwm_v2_init(0, &cfg);
bflb_pwm_v2_set_duty(pwm, 0, 500);
bflb_pwm_v2_start(pwm);
```

### 常见应用

- LED亮度控制
- 电机控制
- 舵机控制
- 音频生成

## RTC (实时时钟)

### 特性

- 闹钟功能
- 周期性中断
- 低功耗模式保持

### 设置时间

```c
#include "bflb_rtc.h"

struct bflb_rtc_time time = {
    .year = 2024,
    .month = 1,
    .day = 1,
    .hour = 12,
    .minute = 0,
    .second = 0
};

bflb_rtc_set_time(&time);
```

### 闹钟

```c
// 设置闹钟
struct bflb_rtc_alarm alarm = {
    .hour = 12,
    .minute = 30
};

void alarm_callback(void)
{
    printf("Alarm triggered\n");
}

bflb_rtc_set_alarm(&alarm, alarm_callback);
```

### 常见应用

- 实时时钟
- 定时唤醒
- 低功耗任务

## DMA (直接内存访问)

### 特性

- 多路 DMA 通道
- 内存到内存、内存到外设、外设到内存
- 链式传输
- 中断完成

### 初始化

```c
#include "bflb_dma.h"

struct bflb_dma_config cfg = {
    .src_addr = (uint32_t)src_buf,
    .dst_addr = (uint32_t)dst_buf,
    .len = 1024,
    .int_en = true,
    .src_req = DMA_REQUEST_MEMORY,
    .dst_req = DMA_REQUEST_MEMORY
};

bflb_device_t *dma = bflb_dma_init(0, &cfg);
```

### 启动传输

```c
// 启动DMA
bflb_dma_start(dma);

// 等待完成
while(!dma_complete);
```

### 常见应用

- 高速数据传输
- 减少CPU负载
- 外设数据传输

## 外设最佳实践

### 1. 初始化顺序

```c
// 正确的初始化顺序
void system_init(void)
{
    // 1. 时钟初始化
    bflb_clk_init();

    // 2. 外设初始化
    uart = bflb_uart_init(0, &uart_cfg);
    spi = bflb_spi_init(0, &spi_cfg);

    // 3. 中断使能
    bflb_irq_enable(IRQ_UART0, true);
}
```

### 2. 错误处理

```c
// 检查初始化结果
if (bflb_uart_init(0, &cfg) == NULL) {
    printf("UART init failed\n");
    return -1;
}
```

### 3. 资源释放

```c
// 使用完后释放
bflb_uart_deinit(uart);
```

### 4. RTOS使用

```c
// 在RTOS中使用外设时，注意线程安全
void task_uart(void *arg)
{
    // 使用互斥锁保护外设访问
    xSemaphoreTake(uart_mutex, portMAX_DELAY);
    bflb_uart_write(uart, data, len);
    xSemaphoreGive(uart_mutex);
}
```

### 5. 低功耗

```c
// 外设空闲时关闭
bflb_uart_deinit(uart);

// 进入低功耗模式前
pmu_enter_sleep();
```

## 总结

Bouffalo SDK通过LHAL提供统一的外设API，支持多种外设。初始化→配置→使用→释放是标准流程。在RTOS中使用时注意线程安全，低功耗场景下及时关闭不使用的外设。
