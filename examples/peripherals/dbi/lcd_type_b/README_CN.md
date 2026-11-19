# DBI Type B LCD

[English](README.md) | [简体中文](README_CN.md)

## 功能

使用 `bsp/common/display` 的默认 GRAM 屏，统一配置为 ST7796、320×480、DBI Type B（8080 x8）、RGB565。
不按芯片切换屏幕型号；接线须按实际开发板调整。

- 黑色背景上固定显示中央 100×100 多色渐变图，左上角为 `((width - 100) / 2, (height - 100) / 2)`。
- “DBI Display Test” 分为 `DBI`、`Display`、`Test` 三行居中显示，使用 `display_simple_font_draw()` 和内置 8×16 ASCII 字体。
  各行水平居中，在图内的顶部坐标分别为 y=22、42、62，行间空隙 4 px，文字总高 56 px。
  前景为不透明白色 ARGB `0xffffffff`，背景 `0x00000000` 为透明，保留原渐变。
- 六圈动态矩形边框宽 2 px、空隙 3 px，距屏幕边缘的 inset 分别为 0、5、10、15、20、25 px。
  从内向外 `depth=0..5`：色相为 `phase + 80 * depth`，亮度为 `255 - 28 * depth`；色相以 1536 为周期。
- 初始化依次清屏、生成并上传一次中心图像、绘制初始边框，之后开启背光。
  主循环每次将 phase 增加 6，仅同步更新边框，更新完成后延时 30 ms；并非固定 30 ms 帧周期。
  中心图像始终保留在屏幕 GRAM 中，主循环不修改、不重新提交。

## 缓冲与传输

- 静态像素缓冲：填充 2000 像素 + 中心图像 10000 像素，RGB565 共 24000 B，XRGB8888 共 48000 B。
  两个缓冲均为 64 字节对齐，无需整屏帧缓冲。
- 清屏和边框使用 `display_draw()`；纯色填充沿两个坐标轴分块，满足填充缓冲容量及 `max_draw_bytes` 限制。
- 中心图像仅上传一次，按 `max_draw_bytes` 和缓冲对齐要求拆分为完整行条带，通过 `display_draw_async()` 提交。
  每条带均等待完成事件并检查结果，再提交下一条带；一次图像上传可能包含多次提交。
- 要求 GRAM/B8 屏幕至少 160×160、区域对齐为 1×1，且缓冲地址满足对齐要求。
  `max_draw_bytes` 至少容纳一行 100 像素：RGB565 为 200 B，XRGB8888 为 400 B；不满足条件会记录错误并停止绘图。

## 接线

屏幕必须设置为 8080 x8 模式。连接 D0～D7、WR、RD、D/C、CS，并连接公共 GND；供电及 IO 电压按照屏幕模块和开发板规格。
默认接线以 `bl616dk` 为准：

| 信号 | GPIO |
| --- | --- |
| D0～D7（依次） | 8～15 |
| WR、RD、D/C 控制组 | 4、6、7，按板原理图或连接器信号标注逐一连接 |
| CS | GPIO5，软件控制 |
| 背光 | GPIO17，高有效 |

控制组的 GPIO 顺序不是信号顺序。其他板型须按实际接线调整 CS、背光及板级 DBI 引脚，不能直接套用这些引脚。

### Reset

默认 `CONFIG_DISPLAY_DEFAULT_RESET_USED=n`，即 `DISPLAY_PIN_NONE`，程序不驱动任何 reset GPIO。
**不要把屏幕 RESET 接到 GPIO12**：默认接线中它是 DBI 数据脚。
应保证模块 RESET 通过模块自身上拉/复位电路保持非复位状态，不能悬空；屏幕初始化序列含软件复位。

如需硬件复位，在 Kconfig 中启用 `DISPLAY_DEFAULT_RESET_USED`，并将 `DISPLAY_DEFAULT_RESET_PIN` 设置为实际接线的独立空闲 GPIO（低有效）。
该 GPIO 不得与 DBI、CS、背光、串口或板上其他外设冲突；本示例不假定额外的 reset 接线。

### 背光

默认使用 GPIO17 控制背光，高有效。
可通过 `DISPLAY_DEFAULT_BACKLIGHT_MODE`、`DISPLAY_DEFAULT_BACKLIGHT_PIN` 和 `DISPLAY_DEFAULT_BACKLIGHT_ACTIVE_HIGH` 修改。
绘制初始中心图像和边框后开启背光。

背光引脚不得与 DBI 数据或控制信号冲突。若背光由模块常亮电路控制，可选择 `DISPLAY_DEFAULT_BACKLIGHT_NONE`。
不要将背光负载直接接到不满足驱动能力的 GPIO。

## 配置

工程使用 `defconfig` 和 SDK Kconfig：

| 配置 | 默认值/用途 |
| --- | --- |
| `BSP_DISPLAY`、`DISPLAY_DEFAULT` | 启用显示框架和默认屏 |
| `DISPLAY_SIMPLE_FONT`、`DISPLAY_SIMPLE_FONT_ASCII_16X8` | 启用内存绘字和 8×16 ASCII 字体 |
| `DISPLAY_DEFAULT_TYPE_GRAM`、`DISPLAY_DEFAULT_IF_DBI_B8` | GRAM 屏，DBI Type B x8 |
| `DISPLAY_DEFAULT_MODULE_ST7796_320X480` | ST7796，320×480 |
| `DISPLAY_DEFAULT_FORMAT_RGB565` | 默认像素格式；也支持 `DISPLAY_DEFAULT_FORMAT_XRGB8888`，不支持 packed RGB888 |
| `DISPLAY_DEFAULT_DMA` | `dma0_ch3`；设为空字符串可使用 CPU 发送 |
| `DISPLAY_DEFAULT_LLI_COUNT` | 20 个 DMA 描述符；同步绘图自动分块 |
| `DISPLAY_DEFAULT_FREQ_HZ` | 0，采用接口默认频率 |
| `DISPLAY_DEFAULT_CS_GPIO`、`DISPLAY_DEFAULT_CS_PIN` | GPIO5 软件 CS |
| `DISPLAY_DEFAULT_RESET_USED`、`DISPLAY_DEFAULT_POWER_USED` | 默认均关闭，实际接线确认后可开启 |

字体覆盖可打印 ASCII；用法及字体许可见公开的 [`simple_font` 文档](../../../../bsp/common/display/simple_font/README_CN.md)。
可用 `make menuconfig CHIP=bl616 BOARD=bl616dk BUILD_DIR=build_bl616` 调整配置。
同一 `BUILD_DIR` 中保存的 menuconfig 配置优先于 `defconfig`；不同芯片和核心请使用不同构建目录。

### 更换屏幕

在 `defconfig` 或 menuconfig 中取消 ST7796 模块，选择新屏幕对应的 `DISPLAY_DEFAULT_MODULE_*` 和 RGB565/XRGB8888 格式；保持 GRAM、DBI B8 接口，并满足上述尺寸和传输要求。
按实际接线调整 CS、复位和背光。`main.c` 中的 `adjust_default_module()` 仅在选择 ST7796 时启用，设置该屏的 BGR、镜像及反色/列反转参数；更换玻璃型号时也应核对这些参数。
使用新的 `BUILD_DIR` 或更新已保存的 menuconfig，避免旧配置覆盖修改。

## 构建

在本示例目录执行，配置好 SDK 工具链及环境。

默认接线的构建命令：`make CHIP=bl616 BOARD=bl616dk BUILD_DIR=build_bl616`。

烧录时沿用同一组芯片、板型、核心及构建目录参数，加上 `flash COMX=/dev/ttyUSB0`，串口替换为实际设备。