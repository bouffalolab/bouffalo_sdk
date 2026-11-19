# Tiny JPEG DBI 显示

[English](README.md) | [简体中文](README_CN.md)

## 使用条件

**本示例只能使用具备 YUV 转 RGB 能力的硬件 DBI 外设。不支持硬件 SPI、软件模拟接口或视频扫描输出，即使屏幕的接线接口也称为 DBI Type C。**

程序独占 DBI 外设，运行期间不能与其他屏幕、设备、任务或直接调用 LHAL 的使用者共享。
支持 BL616/BL618、BL616CL、BL618DG；所选开发板须具备对应接口引脚和工程所需的 PSRAM。

## 功能

- 默认屏幕为 ILI9488、320×480，使用硬件 DBI 外设的 Type C 4-wire 模式，请求时钟 40 MHz，硬件 CS。
- 屏幕旋转 90°，逻辑尺寸为 480×320。启动时在深色背景上显示两行彩色文字和三圈 RGB 边框，持续 500 ms。
- Tiny JPEG 循环解码五张内置 320×240 图片，不缩放，在 (0, 0) 位置显示；每张显示完成后延时 1 s。
- display 的正常像素格式为紧凑 `RGB888`，内存字节顺序为 `[B,G,R]`。JPEG 解码输出为紧凑 YUV444，顺序为 `[Y,U,V]`，同样每像素 3 字节。
- 每次提交 JPEG 前，先发送 DCS NOP，确保屏幕 endpoint 的正常配置已应用，再由应用通过 LHAL 临时切换为 `DBI_PIXEL_INPUT_FORMAT_YUV444`，使用 `display_draw_async()` 提交。
  DBI 硬件将 YUV 转成 RGB，屏幕接收的仍是 RGB 像素。完成回调先恢复 `DBI_PIXEL_INPUT_FORMAT_RGB_888` 并清空 FIFO，再允许复用缓冲。提交失败也会恢复 RGB；显示错误会停止程序，不对未知控制器状态盲目重试。
- 仅使用一个 230400 字节的 PSRAM 图像缓冲，完成后才复用；尾部额外保留 3 字节供 DBI 按整 word 读取。JPEG 输入容量为 32 KiB，解码工作区为 16 KiB。超出 `max_draw_bytes` 时按完整行拆分为起址对齐的条带；无法容纳对齐条带的配置会被拒绝。

## 接线

屏幕模块须设置为 DBI Type C 4-wire，连接 SCL、SDA、D/C、CS、公共 GND 和合适的电源。默认板级引脚如下：

| 信号 | BL616DK GPIO | BL618DGDK GPIO |
| --- | --- | --- |
| SCL | 12 | 11 |
| SDA | 14 | 13 |
| D/C | 15 | 14 |
| 硬件 CS | 13 | 12 |

其他开发板须核对自身引脚映射。默认不控制复位、电源开关和 GPIO 背光；须由模块电路或外部接线提供供电、背光和非复位状态的 RESET，RESET 不得悬空。
如需 GPIO 控制，配置相应的 `DISPLAY_DEFAULT_RESET_*`、`DISPLAY_DEFAULT_POWER_*` 或 `DISPLAY_DEFAULT_BACKLIGHT_*`，使用实际独立接线且不冲突的 GPIO。

## 配置

`defconfig` 启用 `BSP_DISPLAY`、`DISPLAY_DEFAULT`、GRAM、`DISPLAY_DEFAULT_IF_DBI_C4`、`DISPLAY_DEFAULT_MODULE_ILI9488_320X480`、`DISPLAY_DEFAULT_FORMAT_RGB888`、`TJPGDEC`、PSRAM 和 `simple_font` 的 8×16 字体。
像素默认使用 `dma0_ch3` 和 40 个 DMA 描述符；将 `DISPLAY_DEFAULT_DMA` 设为空字符串可使用 CPU 传输。
BL618DG 使用 `RAM_FOR_AP_SIZE=0x48000`、`WRAM_FOR_AP_SIZE=0x28000`，为两核预留 SRAM；配套运行的其他核心固件须使用一致的分区配置。视频/DSI 后端关闭。

更换屏幕时保持 RGB888 和硬件 DBI 接口，选择匹配的模块、接口与实际接线，确认屏幕支持对应的 24 位线格式及 90° 方向。
图片须同时满足旋转后的屏幕尺寸和 76800 像素输出容量限制；解码使用原始分辨率，不会缩放超大 JPEG。
修改默认配置时使用新的 `BUILD_DIR` 或更新已保存的 menuconfig，避免旧配置覆盖 `defconfig`。

### 可选 SD 卡输入

启用 `FATFS`、`FATFS_SDH_SDCARD`、`BSP_SDH_SD`，并按开发板配置 SDH 驱动。循环会读取 `/sd/img0.jpg` 至 `/sd/img4.jpg`，代替内置数据。
文件须为解码器支持的 baseline JPEG，不超过 32 KiB，并满足上述尺寸和容量要求。无效或无法读取的文件会被跳过。

**BL616DK 默认 SDH 使用 GPIO10～15，与 DBI-C4 引脚重叠，不能同时使用这两套默认接线。** 开启 SD 卡输入前，必须提供不冲突的板级引脚配置；其他开发板也须逐一核对 SDH 与显示引脚。

## 构建与烧录

配置 SDK 工具链后，在本示例目录执行：

- BL616/BL618：`make CHIP=bl616 BOARD=bl616dk BUILD_DIR=build_bl616`
- BL616CL：`make CHIP=bl616cl BOARD=bl616cldk BUILD_DIR=build_bl616cl`
- BL618DG AP：`make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap BUILD_DIR=build_ap`
- BL618DG NP：`make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=np BUILD_DIR=build_np`

烧录时沿用相同的芯片、板型、核心和构建目录参数，追加 `flash COMX=/dev/ttyUSB0`，并替换为实际串口设备。