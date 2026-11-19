# cam_lcd

## 功能

DVP 摄像头预览示例：启动后显示黑底红字 `Cam test` 500 ms，随后由 CPU 将摄像头图像转换为屏幕像素并显示，每秒通过串口报告实际完成的整帧 FPS。

- 使用 `bsp/common/display` 的 default GRAM 显示，默认 **GC9307、DBI Type B 8-bit、240×320、RGB565**，保持该屏的 X 镜像方向。
- 保留 VGA 640×480 到 RGB565 320×240、240×320 的专用转换；其他尺寸使用居中裁剪、缩放和顺时针旋转路径，包括 320×480 分支。
- 支持 CPU XRGB8888 输出，内存排列为 B/G/R/X。VGA 到 320×240、240×320 均保留对应 RGB565 专用转换的原坐标、两行采样和 U/V 解释，仅像素打包不同；其他尺寸保留通用裁剪/缩放/旋转，固定尺寸转换仅在源、目标尺寸同时匹配时启用。
- 每帧按显示接口报告的 `max_draw_bytes` 分成整行条带，逐条异步提交并匹配完成事件的缓冲区和结果；整帧完成后才归还摄像头帧并计入 FPS。CPU-only 显示接口同样可用。
- 采集缓冲区和显示缓冲区按 `BFLB_CACHE_LINE_SIZE` 对齐（DG N900 为 64 字节）；CAM 初始化前对整个采集缓冲区执行 clean/invalidate，CPU 读取完成帧前 invalidate 源缓存；输出像素使用默认显示接口的自动 cache clean。
- BL618DG 在取得完成帧后停止 CAM、等待总线和 AXI 空闲，再读取和转换；保持停止直到整帧显示完成，随后 clear/start。停止超时不读取或归还未确认稳定的帧。其他芯片保留帧 FIFO 的 pop 流程。
- 启动文字使用 `display_simple_font_draw()` 和内置 8×16 ASCII 字体，在完整显示缓冲区的 `(10, 10)` 绘制黑底红字，再提交显示。字体用法及许可见 [`simple_font`](../../../../bsp/common/display/simple_font/README_CN.md)。

## 配置

`defconfig` 保留 `CONFIG_PSRAM=y` 和 `CONFIG_BSP_IMAGE_SENSOR=y`。屏幕尺寸和应用像素深度分别来自 `DISPLAY_DEFAULT_WIDTH/HEIGHT`、`DISPLAY_DEFAULT_BPP`，不单独填写一套屏幕尺寸。

在 `menuconfig` 的 BSP Display/default 菜单中配置模块、接口、DMA 通道、LLI 数量及控制 GPIO。示例只支持 **GRAM + RGB565/XRGB8888**，不使用 video/flip 或 packed RGB888。选择其他面板前，应核对实际模块、接口和引脚；默认不会自动换屏。

| 配置 | 默认值/用途 |
| --- | --- |
| `CONFIG_BSP_DISPLAY` / `CONFIG_DISPLAY_DEFAULT` | 启用显示及默认屏幕 |
| `CONFIG_DISPLAY_SIMPLE_FONT` / `CONFIG_DISPLAY_SIMPLE_FONT_ASCII_16X8` | 启用内存绘字及 8×16 ASCII 字体 |
| `CONFIG_DISPLAY_DEFAULT_MODULE_GC9307_240X320` | GC9307 240×320 |
| `CONFIG_DISPLAY_DEFAULT_IF_DBI_B8` | DBI 8080 x8 |
| `CONFIG_DISPLAY_DEFAULT_FORMAT_RGB565` | RGB565；可改选 `CONFIG_DISPLAY_DEFAULT_FORMAT_XRGB8888` |
| `CONFIG_DISPLAY_DEFAULT_DMA` | `"dma0_ch3"`；空字符串选择 CPU 发送 |
| `CONFIG_DISPLAY_DEFAULT_LLI_COUNT` | 40；改变后应用自动按实际容量分条 |
| `CONFIG_DISPLAY_DEFAULT_CS_GPIO` | 软件 CS；BL616/IFA GPIO5，BL616CL GPIO0，BL618DG GPIO18 |
| `CONFIG_DISPLAY_DEFAULT_RESET_USED` | n，不操作 LCD reset GPIO |
| `CONFIG_DISPLAY_DEFAULT_BACKLIGHT_NONE` | 不操作 LCD backlight GPIO |

摄像头需输出非零、偶数宽的 YUYV/YVYU 422 数据，使用现有传感器配置及采样约定，不进行硬件 RGB 转换。CAM 帧紧密打包，单帧字节数（源宽 × 源高 × 2）必须为 `BFLB_CACHE_LINE_SIZE` 的整数倍；不满足时明确拒绝，不添加帧间 padding 或改变帧跨度。采集缓冲区保留精确双帧大小，并在分配前检查长度溢出；显示缓冲区使用 PSRAM，容量为屏宽 × 屏高 × 每像素字节数。PSRAM 型号/容量和传感器选项应与实际板卡一致。

### 接线与板级配置

BL616DK、BL616CLDK、BL618DGDK 使用所选板卡的 `board_display_*` 和 DVP 引脚实现；IFA 板使用本工程 `bl616_ifa_board/board_gpio_overlay.c`。显示数据引脚 hook 不复用 CS；只有选择硬件 CS 时，才调用独立的 CS pinmux hook。选择 no-CS 表示模组已常选中，而不是由数据 hook 隐式驱动片选。

IFA overlay 保留现有引脚：

| 接口 | 引脚 |
| --- | --- |
| DBI B8 | GPIO4、6～15；CS GPIO5 单独处理 |
| DBI C4 | GPIO12、14、15；硬件 CS GPIO13 单独处理 |
| DBI C3 | GPIO12、15；硬件 CS GPIO13 单独处理 |
| QSPI | GPIO10、12～15；硬件 CS GPIO11 单独处理 |
| SPI0 | SCK GPIO15、MOSI GPIO13；软件 CS/DC 另配 |
| 摄像头 | I2C GPIO0/3、reset GPIO6、MCLK GPIO23、DVP GPIO24～34 |
| 板级固定电平 | 保留 GPIO16 输出高，不将它作为可调 LCD 背光 |

**必须按板卡原理图解决共享引脚，不能仅凭编译参数认为所有接口能同时连接：**

- 不启用通用 LCD reset GPIO19 和 backlight GPIO16：它们可能与摄像头或板级固定控制信号冲突（BL618DG 的摄像头 reset 是 GPIO19，I2C 包含 GPIO16）。默认依赖屏幕模组的上电复位和独立背光供电。若硬件要求 GPIO 控制，请先确认空闲且实际接出的引脚，再配置 `RESET_USED/RESET_PIN` 或 `BACKLIGHT_GPIO/BACKLIGHT_PIN`，也可通过项目自定义 board ops 控制。
- IFA 原有摄像头 reset GPIO6 与 DBI B8 引脚组重叠，摄像头初始化会重新配置它。**该接线不能直接视为可同时使用的完整方案**；应在项目板级 overlay 中依据实际复位电路处理共享关系，或明确选择已按硬件连接的其他接口，本示例不猜测替代引脚。
- BL618DGDK 默认 B8 引脚包含 GPIO19，与摄像头 reset 重叠；`CONFIG_DBI_DISPLAY_BOARD_MODE` 的另一组引脚含 GPIO13，又与摄像头 I2C 重叠，且其硬件 CS 是 GPIO12。切换布局时须同步核对片选和摄像头接线，不能只改该选项。本工程默认软件 CS18 对应未启用该布局选项的 B8 引脚组。

## 使用

在本目录执行以下命令，选择与实际硬件一致的板卡。独立构建目录可避免已有菜单配置覆盖修改后的默认配置。

```sh
make CHIP=bl616 BOARD=bl616dk BUILD_DIR=build_bl616
make CHIP=bl616 BOARD=bl616_ifa_board BUILD_DIR=build_ifa
make CHIP=bl616cl BOARD=bl616cldk BUILD_DIR=build_bl616cl
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap BUILD_DIR=build_bl618dg_ap
```

BL618DG 应选择拥有摄像头、DBI 和 PSRAM 资源的核心，并在 `menuconfig` 中保留相应 PSRAM 容量设置。

```sh
make menuconfig CHIP=bl616 BOARD=bl616dk BUILD_DIR=build_bl616
make flash CHIP=bl616 BOARD=bl616dk BUILD_DIR=build_bl616 COMX=/dev/ttyUSB0
```

连接串口后可看到传感器名称、源尺寸、屏幕尺寸及 `camera display fps: ...`。FPS 包含转换、全部条带发送与等待时间，不是传感器标称帧率。
