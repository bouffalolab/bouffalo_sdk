# BL618DG Orion

复用 BL618DG DK 的启动、Flash/PSRAM 初始化和 RF 实现，提供 Orion 音频接线。
选择 `CHIP=bl618dg BOARD=bl618dg_orion CPU_ID=ap`。

| 文件 | 配置内容 |
| --- | --- |
| `board_config.h` | 音频引脚、DMA、PA 时序和硬件槽位 |
| `board_gpio.c/.h` | 控制台、GPIO、PLL/MCLK、共享 I2C 保护 |
| `board_audio.c/.h` | Codec 初始化、数据接线默认值和共享 I2C 访问保护 |
| `Kconfig.defconfig` | JY6316 和 16 MiB PSRAM 默认值 |

音频使用 JY6316（JY6311 DAC + JY6166 ADC）。I2S 的 FS/DI/DO/BCLK 为
GPIO29/30/31/32，MCLK 为 GPIO17，Codec I2C 的 SCL/SDA 为 GPIO20/21，
PA 为 GPIO19。DMA TX 使用 DMA0 通道0，RX 使用 DMA0 通道1。
总线为四槽 PCM16；`xcodec_io` 默认回放 CH3 麦克风，放音写入槽0。

新接口的 `xcodec_dev_board_init()` 无参一次初始化 GPIO、共享 I2C、Codec 和共享 I2S 数据接口，
返回的 `xcodec_board_handle_t` 提供 Codec 控制接口及录放音数据接口。
外部 Codec 初始化及控制调用 `bsp/common/audio_codec/audio_codec.c`。

Board 使用驱动默认宏和本板 DMA、四槽 DSP/TDM、非合并 FIFO、PLL 回调创建共享 I2S 数据接口。
main 可在初始化前修改驱动的 `xcodec_channel_default_config` 覆盖 DMA 和超时，
初始化后使用返回的接口创建独立录放音设备。
示例在 main 逐字段配置录放音格式，只在设备 `open()` 传入；
运行采样率由数据驱动配置 PLL/I2S，并通过控制接口下发到 Codec。

应用持有设备，停止读写后先关闭并删除设备，再调用 `xcodec_dev_board_deinit()`，
由 Board 释放共享数据和 Codec 控制接口。
同一物理 I2S 只创建一个数据接口，录放音分别借用它。
