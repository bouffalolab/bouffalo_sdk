# 外设入口

路径相对 SDK 根目录。API 头文件和实现通常位于 `drivers/lhal/include/`、`drivers/lhal/src/`；设备名、引脚、时钟、外设版本和 DMA 请求要按芯片、板级设备表与示例确认，见 [03-lhal-drivers.md](03-lhal-drivers.md)。

| 外设 | 头文件 / 示例 | 选择时注意 |
| --- | --- | --- |
| GPIO | `drivers/lhal/include/bflb_gpio.h`、`examples/peripherals/gpio/` | 中断触发与引脚由目标芯片和板级决定 |
| UART | `drivers/lhal/include/bflb_uart.h`、`examples/peripherals/uart/` | 检查与 console 的资源冲突；DMA 见 [UART DMA](42-uart-dma.md) |
| SPI | `drivers/lhal/include/bflb_spi.h`、`examples/peripherals/spi/` | 主从、帧宽、对齐和片选依据目标芯片及设备配置；SPI NAND 见[存储](28-filesystems-storage.md) |
| I2C | `drivers/lhal/include/bflb_i2c.h`、`examples/peripherals/i2c/` | 消息、重启条件及引脚依据芯片和外设 |
| I2S | `drivers/lhal/include/bflb_i2s.h`、`examples/peripherals/i2s/` | 时钟、数据格式和 codec 要匹配板级硬件，见[音视频](32-audio-video-media.md) |
| ADC | `drivers/lhal/include/bflb_adc.h`、`drivers/lhal/include/bflb_adc_v2.h`、`drivers/lhal/include/bflb_adc_v3.h`；`examples/peripherals/adc/`、`examples/peripherals/adc_v2/`、`examples/peripherals/adc_v3/` | 不同 ADC 版本的接口不同，按芯片与编入的驱动选择 |
| DAC | `drivers/lhal/include/bflb_dac.h`、`examples/peripherals/dac/` | 以目标芯片构建配置确认支持 |
| Timer、PWM、RTC | `drivers/lhal/include/bflb_timer.h`、`drivers/lhal/include/bflb_pwm_v1.h`、`drivers/lhal/include/bflb_pwm_v2.h`、`drivers/lhal/include/bflb_rtc.h`；`examples/peripherals/timer/`、`examples/peripherals/pwm_v1/`、`examples/peripherals/pwm_v2/`、`examples/peripherals/rtc/` | PWM 版本和唤醒能力以芯片及配置为准；低功耗唤醒见[低功耗](12-power-management.md) |
| DMA | `drivers/lhal/include/bflb_dma.h`、`drivers/lhal/src/bflb_dma.c`、`examples/peripherals/dma/` | 预先核对描述符容量、地址可访问性、缓冲生命周期及缓存要求；外设还需配置对应请求 |
| Flash、PSRAM | `examples/peripherals/flash/`、`examples/peripherals/psram/` | 先核对目标板、分区和擦写范围；见[内存与分区](10-memory-management.md) |

具体 API 和使用顺序直接查选中版本的头文件及示例。示例可能占用硬件资源、擦写 Flash 或覆盖 PSRAM，运行前先核对其行为。
