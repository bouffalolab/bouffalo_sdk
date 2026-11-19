# LHAL 外设驱动架构

LHAL 提供 `bflb_*` 外设接口，但不同芯片、核心、配置和硬件版本所编入的驱动及可用能力不同。实际调用和完整示例见 [08-peripherals.md](08-peripherals.md)；使用前对照当前 SDK 的头文件、设备表、板级引脚和示例。

## 查找设备和驱动

1. `drivers/lhal/CMakeLists.txt` 按 `CHIP`、`CPU_ID`、`CPU_MODEL` 和功能配置选择源文件，编入 `config/${CHIP}/device_table.c`。
2. `drivers/lhal/include/bflb_core.h` 中 `struct bflb_device_s` 保存名称、寄存器地址、IRQ、索引和用户数据；`bflb_device_get_by_name()` 按设备表获取指针。名称宏见 `bflb_name.h`，实际设备实例以目标设备表为准。部分实现找不到名称时会断言，不能依赖它总是返回 NULL。
3. 应用通常先执行 `board_init()` 和对应板级 pinmux，再获取设备指针并调用具体驱动；初始化函数参数和返回值各不相同。例如 UART、SPI、I2C 的 `init` 返回 `void`，不要将其当成设备句柄或检查为 NULL。
4. 配置字段、IRQ 和实际设备还可能随芯片或核心变化，例如 `config/bl618dg/device_table.c` 有 `CPU_MODEL_A0`、`CPU_LP` 条件。头文件有名称宏并不保证目标配置有该设备。

## 芯片实现选择

下表描述当前 `drivers/lhal/CMakeLists.txt` 中的源文件选择，不是芯片完整能力表：

| CHIP | ADC 驱动 | PWM 驱动 | 编入 `bflb_dac.c` |
| --- | --- | --- | --- |
| bl602、bl702 | `bflb_adc.c` | v1 | 是 |
| bl702l | `bflb_adc.c` | v1、v2 | 否 |
| bl616 | `bflb_adc.c` | v2 | 是 |
| bl616cl | `bflb_adc_v2.c` | v2 | 否 |
| bl618dg | `bflb_adc_v3.c` | v2 | 否 |

ADC 各版配置和初始化接口有区别，不能混用；未编入通用 DAC 不代表芯片没有其他音频 DAC。CherryUSB 开启时，BL702 选 USB v1，BL602/BL702L 分支不编入 USB 驱动，其余分支选 USB v2。多媒体和 ROM API patch 等还受芯片、核心和 Kconfig 条件影响。

## 常见驱动入口

| 功能 | 头文件和示例 | 核对重点 |
| --- | --- | --- |
| GPIO | `bflb_gpio.h`；`examples/peripherals/gpio/` | 设备句柄、引脚复用、触发模式及中断回调 |
| UART | `bflb_uart.h`；`examples/peripherals/uart/` | 波特率、方向、FIFO、中断和 DMA |
| SPI | `bflb_spi.h`；`examples/peripherals/spi/` | 主从、数据宽度、字节顺序及片选 |
| I2C | `bflb_i2c.h`；`examples/peripherals/i2c/` | 消息数组、停止条件、芯片差异和返回码 |
| I2S | `bflb_i2s.h`；`examples/peripherals/i2s/` | 时钟、声道和 DMA 与 codec 板级配置 |
| ADC、DAC | `bflb_adc*.h`、`bflb_dac.h`；`examples/peripherals/adc*/`、`dac/` | ADC 版本、通道、参考电压和引脚 |
| Timer、PWM、RTC | `bflb_timer.h`、`bflb_pwm_v*.h`、`bflb_rtc.h`；相应 `examples/peripherals/` | 输入时钟、计数配置、比较中断及芯片条件 |
| DMA | `bflb_dma.h`；`examples/peripherals/dma/` | 通道请求、传输宽度、LLI 容量和缓存一致性 |
| Flash、安全 | `bflb_flash.h`、`bflb_sec_*.h` | 见内存/分区和安全参考；确认地址、擦写范围及密钥来源 |

外设的中断通常通过 `bflb_irq_attach(dev->irq_num, handler, arg)` 和 `bflb_irq_enable(dev->irq_num)` 注册和使能；具体处理函数签名、屏蔽及清除方法以该驱动和示例为准，GPIO 有自己的 pin 回调接口。

## ROM API 与迁移

ROM API 选择受芯片和构建配置约束；当前 CMake 在 `CONFIG_ROMAPI` 下只对 BL616CL 或 BL618DG AP 加入对应 LHAL patch。不要假定 ROM 与非 ROM 实现、性能及缺陷完全一致。迁移时检查目标 `device_table.c`、`CMakeLists.txt`、头文件条件宏、板级时钟/引脚，再分别验证构建和目标硬件行为。
