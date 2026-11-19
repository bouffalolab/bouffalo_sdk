# display：显示框架

[English](README.md) | 中文

display 是一套分层的显示框架。它支持带有片内显存（GRAM）、通过 SPI 或 DBI 外设（MIPI DBI Type B、Type C、QSPI）
连接的屏幕，也支持由帧缓冲持续扫描输出的视频屏（RGB 和 MIPI DSI）。屏幕在编译期静态定义，GRAM 屏的像素数据通过 DMA 发送（可选异步），
多块屏幕可以共享同一个控制器。视频屏目前只实现了 BL618DG（MIPI DSI 和 RGB 引脚），它的显示处理单元还有可以显示视频帧（含摄像头的 YUV 格式）的 Base 层和两个 OSD 层
（见 [`scanout/bl618dg_dpu`](scanout/bl618dg_dpu/README_CN.md)）。BL616 的并行 RGB（“SIM v1”）后端需要额外的 ROM 函数表库；BL616CL 的串行 RGB 后端尚未实现。见第 2 节。

在工程中设置 `CONFIG_BSP_DISPLAY=y` 即可引入框架（见 3.1 节）。LVGL 示例位于 [`examples/display`](../../../examples/display)。

## 1. 框架结构

```text
应用 / LVGL port
      |  display_draw()  display_draw_async()  display_flip() ...
+-----v------------------------------------------------------------------+
| Display   一块屏幕                                                      |
+------------------------------------------------------------------------+
| Panel     屏幕驱动 IC（命令、初始化、方向）                              |
+------------------------------------------------------------------------+
| IO        一块屏幕与总线的连接：CS、接口、时钟、像素格式                  |
+------------------------------------------------------------------------+
| Bus       一个控制器：仲裁、端点切换、DMA                                |
+------------------------------------------------------------------------+
                     LHAL（bflb_spi / bflb_dbi / bflb_dma / gpio）
```

| 层 | 作用 |
|---|---|
| Display（`display_t`） | 应用的入口。`display_init()` 把屏幕带到可用状态；之后绘制（GRAM 屏）或切换帧缓冲（视频屏）、旋转、设置背光；`display_deinit()` 关闭屏幕。 |
| Panel（`display_panel_t`） | 把“绘制一个区域”翻译成屏幕 IC 的命令。对遵循 MIPI DCS 命令集的 IC，提供通用驱动。 |
| IO（`display_io_t`） | 描述一块屏幕如何接到总线上：片选、接口类型、时钟和像素格式。 |
| Bus（`display_dbi_bus_t`、`display_spi_bus_t`、`display_soft_dbi_c_bus_t`、`display_dsi_host_t`） | 每个物理控制器一个对象，由挂在上面的所有屏幕共享。 |
| Scanout（`display_scanout_t`） | 只用于视频屏：像素引擎持续读取帧缓冲并送给 RGB 或 MIPI DSI 屏，在帧边界切换到新的帧缓冲。它代替 GRAM 屏的绘制通路，不依赖屏幕驱动。 |

设计原则：

- **静态定义。** 整个拓扑在编译期声明，框架不分配内存。运行时应用只需调用 `display_init()`，它会按需初始化各层，
  不必关心初始化顺序。
- **标准接口。** 接口采用 MIPI DBI 的名称：Type B（`DBI_B8`，8080 并口）、Type C 4 线（`DBI_C4`）、Type C 3 线（`DBI_C3`）
  和 `QSPI`。总线后端声明它能驱动的接口；屏幕 IC 声明它支持的接口以及在每个接口上的标准设置（命令编码、像素格式码、默认时钟、
  QSPI 协议）；端点写明屏幕接在哪个接口上。初始化时三者互相核对，不匹配返回 `-ENOTSUP`。
- **软件片选。** 共享总线的屏幕靠片选（CS）GPIO 区分。一个 DBI 外设可以同时驱动不同接口的屏幕，例如 Type B 和 Type C；
  切换屏幕时，框架调用板级提供的引脚复用函数。
- **两级屏幕数据。** *IC 描述*包含使用同一颗驱动 IC 的所有屏幕共有的内容：初始化序列、支持的接口、复位时序。
  *模组描述*包含某一块屏幕自己的差异：分辨率、方向、GRAM 偏移、反色、伽马。
- **DMA 传输。** 总线支持时，像素数据总是通过 DMA 发送；同步绘制只是等待传输完成。DMA 完成中断先释放 CS 和总线，再通知应用。
- **背光。** 背光属于屏幕的板级控制，亮度范围 0（关）到 1000。GPIO 背光对大于 0 的任何值都开；PWM 或扩展 IO 背光由工程实现。`display_init()` 之后背光是关的，第一帧画好之前看不到任何内容。

## 2. 支持的硬件

### 2.1 屏幕 IC

| IC | 默认模组 | 接口 |
|---|---|---|
| ST7789 | 240x320、240x240 | B8、C4、C3 |
| ST7796 | 320x480 | B8、C4、C3 |
| ILI9341 | 240x320 | B8、C4、C3 |
| ILI9488 | 320x480 | B8；C4、C3 只支持 24 位像素 |
| ILI9486 | 320x480 | 同 ILI9488 |
| GC9A01 | 240x240 | C4、C3 |
| GC9307、JD9853 | 240x320 | B8、C4、C3 |
| ST7735 | 240x320（其他尺寸需自己写模组） | C4、C3 |
| ST77922 / ST77926 | 320x480 | ST77922：B8、C4、C3；ST77926：QSPI |
| SPD2010 | 412x412 | QSPI |
| NT35510 | 480x800 | B8 |

### 2.2 视频屏

下表列出内置视频屏模组；可用链路取决于芯片的 scanout 后端（2.4 节）。

| 屏幕 | 链路 | 模组 |
|---|---|---|
| EK79007 | MIPI DSI，2 或 4 lane | 1024x600 |
| AXS15231B | MIPI DSI，1 lane | 172x640 |
| AXS15231E | MIPI DSI，1 lane | 258x960 |
| FL7707N | MIPI DSI，2 lane | 720x720 |
| ILI9806E | MIPI DSI，2 lane | 480x854 |
| ILI9881C | MIPI DSI，4 lane（KD050020）或 2 lane（KD050023W4） | 720x1280 |
| JD9365TX | MIPI DSI，2 lane | 720x1280 |
| ST7102 | MIPI DSI，1 lane（KD027HVF）或 2 lane（YH494） | 320x320、480x960 |
| ST7701S | MIPI DSI，2 lane；并行 RGB 加 3 线或 4 线命令口 | 480x480 |
| ST7701P | 并行 RGB 加 3 线或 4 线命令口 | 480x480 |
| GC9503V | 并行 RGB（DE 模式）加 3 线或 4 线命令口 | 480x480 |
| JD9165BA | 并行 RGB，3 线或 4 线命令口，或没有命令口 | 1024x600 |
| 标准 RGB 屏（无驱动 IC） | 并行 RGB | 800x480、1024x600（70 Hz） |

屏幕上电时序、复位和背光由板级配置管理；没有复位线的板子需要保证屏幕的上电等待时间。

标准 RGB 屏不需要初始化：它显示时序送来的内容，所以只用时序和时钟描述（`panel/video_standard`）。其他尺寸的模组，在应用工程里写几行时序数据即可。

### 2.3 总线后端和像素格式

| 后端 | 接口 | 内存像素格式 | 说明 |
|---|---|---|---|
| DBI 外设（`bus/dbi`） | B8、C4、C3、QSPI | RGB565、RGB888（紧凑）、XRGB8888 | 可在运行时切换接口；DMA 异步 |
| 硬件 SPI（`bus/spi`） | C4 | RGB565、XRGB8888 | DMA 异步 |
| Soft DBI C（`bus/soft_dbi_c`） | C3、C4 | RGB565、RGB888、XRGB8888 | GPIO 模拟，同步，适合命令口 |
| MIPI DSI Host（`bus/dsi`） | DSI 低功耗命令 | 无（只发命令） | BL618DG（B0）。视频运行期间命令被拒绝（`-EBUSY`）；不支持读 |

支持的芯片：BL616、BL616CL、BL618DG、BL702L（BL702L 只有硬件 SPI 后端；QSPI 仅限带该功能的芯片；DSI Host 和 DPU scanout 只在 BL618DG 的 B0 版本上）。
显示接口的引脚函数提供给 `bl616dk`、`bl616cldk`、`bl618dgdk`、`bl702ldk` 这几块板（4.3 节）。

### 2.4 Scanout 后端

| 后端 | 芯片 | 链路 | 内存像素格式 | 使用条件 |
|---|---|---|---|---|
| BL618DG DPU（`scanout/bl618dg_dpu`） | BL618DG | MIPI DSI，或 RGB 引脚（24、18、16 位接法） | 屏幕层（OSD0）：RGB565、XRGB8888、ARGB8888、AYUV8888 | B0 版本，AP 核 |
| SIM v1（`scanout/sim_v1`） | BL616 | 并行 RGB（16 位，或 8 位加外部锁存） | RGB565 | `DISPLAY_SCANOUT_SIM_V1` 在 BL616 上默认打开；需要额外提供包含 ROM 函数表符号 `__edata_etext_final` 的库。该符号是弱符号，没有库时仍能链接，但 `display_init()` 返回 `-ENOTSUP` |

BL618DG 只有一条 DPI 引擎，RGB 屏和 DSI 屏不能同时工作。BL618DG DPU 把 `display_t` 的屏幕显示在 OSD0 层上；下面是 Base 层（常量颜色，默认黑；或帧缓冲，格式 RGB565、RGB888、XRGB8888、YUYV、UYVY、NV12），
上面是第二个窗口层 OSD1。ARGB8888 或 AYUV8888 帧的逐像素 alpha 会叠在下面各层上。这些图层、它们的格式、窗口变化以及不经 `display_t` 使用 DPU 的方法见
[`scanout/bl618dg_dpu/README_CN.md`](scanout/bl618dg_dpu/README_CN.md)；Base 层和 OSD1 通过 scanout 里的硬件层对象（`dpu.hw`）使用，`display_t` 也会从它收到 `DISPLAY_EVT_VSYNC`。

### 2.5 像素格式

控制器到屏幕之间只有两种位宽：16 位（RGB565）和 24 位（RGB888，RGB666 屏接收同样的数据）。内存中的格式可以与线上的位宽不同：

| 内存格式 | 每像素字节数 | 线上位宽 | 使用者 |
|---|---|---|---|
| RGB565 | 2 | 16 位 | 全部 |
| RGB888（紧凑） | 3 | 24 位 | DBI、Soft DBI C；视频 scanout（BL618DG 的 Base 层） |
| XRGB8888 | 4（高字节被忽略） | 24 位 | 全部 |
| ARGB8888 | 4（高字节为非预乘的 alpha） | 24 位 | 视频 scanout |
| AYUV8888 | 4（`0xAAYYUUVV`，非预乘 alpha） | 24 位 | BL618DG 的 OSD 层 |
| YUYV、UYVY | 2（宽度为偶数） | 24 位 | BL618DG 的 Base 层 |
| NV12 | 1（Y 平面）加交织的 UV 平面，4:2:0 | 24 位 | BL618DG 的 Base 层 |

DBI 外设由硬件转换，内存格式和线上位宽可以任意组合。硬件 SPI 和 Soft DBI C 的位宽由格式决定（RGB565 以 16 位发送，
其他格式以 24 位发送）。屏幕 IC 在某个接口上不支持所需的位宽时，`display_init()` 返回 `-ENOTSUP`。

视频屏的内存格式由 `scanout.cfg.format` 配置，不含帧地址；层关闭且无 pending 后可重配（NULL → LAST → config），不必重启物理输出。帧是紧凑排列的（行长度为 `宽 × 每像素字节数`，没有行间距）。输出一侧由屏幕决定：
DSI 的数据类型写在屏幕模组里，RGB 数据线的根数取决于板子的接线。一个图层接受哪些格式由 scanout 决定（BL618DG 用 `display_bl618dg_dpu_hw_layer_formats()`）；
图层不能显示的格式返回 `-ENOTSUP`。

## 3. 使用

### 3.1 在工程中引入框架

框架是板级支持包的一个组件：在工程的 `defconfig` 里使能即可。它的 Kconfig 选项（`DISPLAY_*`）出现在 menuconfig 里这个选项的下面，CMake 文件属于 `bsp/common`，
所以工程不需要 `add_subdirectory`，也不需要 `source` 行。

```text
CONFIG_BSP_DISPLAY=y
CONFIG_DISPLAY_BUS_SPI=n          # 可选：关掉不想要的后端
CONFIG_DISPLAY_PANEL_ILI9488=n    # 可选：不编译用不到的屏幕 IC
```

本芯片的所有后端默认都是打开的，所以只需要使能 `CONFIG_BSP_DISPLAY`；工程只关掉不想要的后端，例如不想让某个后端占用引脚或 DMA 通道。后端都是静态分配的，没有被引用的代码会被链接器丢掉，
所以不用的后端不占代码空间。

屏幕 IC 用同样的方式开关，每个 IC 一个选项（`DISPLAY_PANEL_<IC>`，默认全部打开，见 4.1 节）。关掉的屏幕不编译，所以工程如果仍然引用它的模组，链接会失败。

头文件按层包含，例如 `display.h`、`bus/dbi/display_dbi.h`、`panel/gram_st7796/display_st7796.h`。

### 3.2 默认屏

工程只有一块屏幕且没有特殊要求时，可以完全用 Kconfig 定义屏幕，应用一次调用即可取得。

```text
CONFIG_DISPLAY_DEFAULT=y
CONFIG_DISPLAY_DEFAULT_IF_DBI_B8=y
CONFIG_DISPLAY_DEFAULT_MODULE_ST7796_320X480=y
CONFIG_DISPLAY_DEFAULT_CS_GPIO=y
CONFIG_DISPLAY_DEFAULT_CS_PIN=5
CONFIG_DISPLAY_DEFAULT_DMA="dma0_ch3"
CONFIG_DISPLAY_DEFAULT_LLI_COUNT=20
CONFIG_DISPLAY_DEFAULT_BACKLIGHT_GPIO=y
CONFIG_DISPLAY_DEFAULT_BACKLIGHT_PIN=17
```

可配置的内容如下（全部选项见 [`default/Kconfig`](default/Kconfig)）：

- **接口和屏幕模组：**硬件 SPI、DBI Type C 4 线、DBI Type B 或 QSPI；模组从内置列表中选择；只有对应的屏幕 IC 选项（`DISPLAY_PANEL_<IC>`）打开时才能选它的模组。
- **内存像素格式：**RGB565、XRGB8888 或紧凑 RGB888（仅 DBI）。
- **片选：**软件 GPIO（默认）、DBI 控制器的硬件 CS，或不使用。CS 始终是 GPIO；它在 DMA 完成中断里释放，
  所以不支持由 I/O 扩展芯片提供的 CS。没有软件 CS 的屏幕必须独占总线，默认屏就是这种情况。
- **复位、电源和背光：**可选的 GPIO。它们只在任务上下文中操作，所以也可以改由工程实现（例如接在 I/O 扩展芯片或 PWM 上）。
- **方向、色序和屏幕命令：**默认屏没有 RGB/BGR 顺序、镜像、反色的 Kconfig 选项。默认屏始终使用所选模组的 RAM 副本，
  应用在 `display_init()` 之前修改这个副本（见下面的“调整屏幕模组”）。
- **自定义模组：**需要完全由工程定义的模组时，工程提供自己的模组描述（见 5.1 节），并选择 `DISPLAY_DEFAULT_MODULE_CUSTOM`；
  默认屏同样使用它的 RAM 副本。
- **视频屏（仅 BL618DG）：**`CONFIG_DISPLAY_DEFAULT_TYPE_VIDEO` 时默认屏是一块视频屏：MIPI DSI 的 EK79007（2 或 4 lane）或标准 RGB 屏（800x480 或 1024x600），
  板上接线的 DSI lane 数和顺序或 RGB 引脚接法，以及内存格式（RGB565、RGB888、XRGB8888 或 ARGB8888）。见 3.5 节。

应用代码：

```c
#include "default/display_default.h"

display_t *lcd = display_default_get();
display_init(lcd);                    /* 上电、复位、发送初始化序列、打开显示；背光是关的 */
/* 绘制第一帧，见 3.4 节 */
display_set_backlight(lcd, DISPLAY_BACKLIGHT_MAX);   /* 0 .. 1000 */
```

**调整屏幕模组。**默认屏不直接使用 `const` 的屏幕模组。第一次调用 `display_default_get()` 或 `display_default_get_module()` 时，
所选模组被复制到 RAM，应用可以在 `display_init()` 之前修改这个副本：

```c
#include "default/display_default.h"

static const display_dcs_cmd_t my_commands[] = {
    DISPLAY_DCS0(0x21, 0),            /* INVON：反色 */
};

display_dcs_module_t *module = display_default_get_module();
module->madctl[0] = 0x48;             /* GRAM 屏：方向和色序，每个旋转角度一个值 */
module->madctl[1] = 0xE8;
module->madctl[2] = 0x88;
module->madctl[3] = 0x28;
module->invert = true;
module->init_user = my_commands;      /* 屏幕准备阶段最后发送 */
module->init_user_count = DISPLAY_DCS_COUNT(my_commands);

display_t *lcd = display_default_get();
display_init(lcd);
```

可以修改的字段：

- `madctl[]`：GRAM 屏每个旋转角度的 MADCTL 值（方向、镜像和 RGB/BGR 色序）。每次旋转变化时框架会再发送一次。
- `invert`：反色。
- `gap_x[]`、`gap_y[]`：可见区域在控制器显存里的列、行偏移，每个旋转角度一个值。
- `init_extra`、`init_extra_count`：**替换**模组自带的额外命令（在初始化序列之后发送）。
- `init_user`、`init_user_count`：工程的命令，**追加**发送。它们在屏幕准备阶段**最后**发送：在初始化序列、模组的 `init_extra`、COLMOD 和
  MADCTL（GRAM 屏）以及模组自己的反色之后，所以与它们冲突时以工程的命令为准。

命令表（`init_extra`、`init_user`）必须是 `static`：显示只保存指针，每次准备屏幕时都会读取这张表。

`display_default_get_module()` 适用于 GRAM 默认屏，以及 MIPI DSI 的视频默认屏（有命令口）；标准 RGB 屏没有命令口，也没有这种模组，不能使用。
请在应用初始化里、从同一个任务、在 `display_init()` 之前调用：驱动在显示初始化时读取模组，复制副本的过程没有加锁。

视频默认屏不自带帧内存：init 不读帧，注册回调后显式提交首帧，等 FIRST 再开背光；之后替换等 RETIRED（3.5 节）：

```c
static uint16_t frame[2][DISPLAY_DEFAULT_VIDEO_FRAME_BYTES / 2] ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(64)));

display_t *lcd = display_default_get();
display_init(lcd); /* 不读帧 */
display_set_callback(lcd, on_event, ctx); /* ISR 完成回调可续交下一帧；flip 前登记期望事件 */
display_flip(lcd, &(display_fb_t){ .data = frame[0], .format = DISPLAY_FMT_RGB565 });
/* 检查返回值，等 FIRST 后开背光；超时不能当作帧已释放。 */
```

**LVGL。**`CONFIG_LVGL9_PORT_DISP=y`（或 `CONFIG_LVGL8_PORT_DISP=y`）且 `CONFIG_DISPLAY_DEFAULT=y` 时，LVGL 的显示 port 使用默认屏，应用只需调用 `lv_port_disp_init()`。
port 由屏幕类型决定：

- **带显存的屏（SPI / DBI / QSPI）：**局部渲染，一块或两块绘制缓冲区（`LVGL9_DISP_DEFAULT_BUF_COUNT`、`LVGL9_DISP_DEFAULT_BUF_SIZE`；v8 同名选项以 `LVGL8_` 开头），通过异步 DMA 发送
  （[`lv_port_disp_default_gram.c`](../../../components/graphics/lvgl_v9/port/lv_port_disp_default_gram.c)）。
- **视频屏（MIPI DSI / RGB）：**PSRAM 里两整帧，LVGL v8 使用整屏刷新（`full_refresh`），v9 使用 DIRECT 模式；port 用 `display_flip()` 显示一帧，旧帧归还后把缓冲区还给 LVGL
  （[`lv_port_disp_default_video.c`](../../../components/graphics/lvgl_v9/port/lv_port_disp_default_video.c)）。像素字节数由 `default/display_default.h` 的 `DISPLAY_DEFAULT_BPP` 定义。
  v9 显式设置显示格式，不依赖 `LV_COLOR_DEPTH`；v8 仍要求 `lv_color_t` 大小与屏幕像素匹配，支持 RGB565、XRGB8888，以及在 `LV_COLOR_DEPTH=32` 且 `LV_COLOR_SCREEN_TRANSP=1` 时使用 ARGB8888。
  初始化不提交帧，LVGL 完成首次绘制后由 flush 提交首帧，FIRST 确认完成；应用不要覆盖 port 的回调。
- **旋转：**LVGL v8 的 GRAM port 可用 `CONFIG_LVGL8_DISP_DEFAULT_SW_ROTATE=y` 启用软件旋转，
  通过 `lv_disp_set_rotation()` 设置角度，不叠加面板硬件旋转；LVGL v9 的 GRAM port 使用面板硬件旋转。两版视频 port 均不支持旋转。

完整示例：[`examples/display/lvgl_v8`](../../../examples/display/lvgl_v8)、[`lvgl_v9`](../../../examples/display/lvgl_v9)，以及叠加视频背景的
[`lvgl_v9_with_osd`](../../../examples/display/lvgl_v9_with_osd)。

### 3.3 手工定义屏幕

多块屏幕、特殊接线或自定义模组时，在 C 文件中声明 *总线 → 端点 → 屏幕 → 显示* 这条链：

```c
static display_dbi_bus_t dbi0 = DISPLAY_DBI_BUS(.dma = "dma0_ch3", .lli = lli, .lli_count = 20,
                                                .ifaces = DISPLAY_DBI_IFACES({ DISPLAY_IFACE_DBI_B8,
                                                                               board_display_dbi_b8_gpio_init,
                                                                               board_display_dbi_b8_gpio_deinit }));
static display_io_t io = DISPLAY_IO(&dbi0, .cs = DISPLAY_PIN_LOW(5), .iface = DISPLAY_IFACE_DBI_B8,
                                    .format = DISPLAY_FMT_RGB565);
static display_dcs_panel_t panel = DISPLAY_DCS_PANEL(&io, &display_module_st7796_320x480, DISPLAY_BOARD_NONE);
display_t lcd = DISPLAY_GRAM(&panel);
```

要在同一总线上再接一块屏幕，增加一个指向同一总线对象的端点和屏幕即可；每块屏幕有各自的 CS、接口、格式和时钟。

### 3.4 使用屏幕

`display_init()` 返回时屏幕已经可以使用，`display_deinit()` 关闭屏幕：

| 调用 | 作用 |
|---|---|
| `display_init()` | 检查配置、上电复位初始化；视频输出配置但不读帧。清理失败保留 ERROR 和资源以便 deinit 重试。背光仍关闭。 |
| `display_set_backlight()` | 设置背光，0 到 1000（`DISPLAY_BACKLIGHT_MAX`）；没有背光的板返回 `-ENOTSUP`。 |
| `display_deinit()` | 关背光、关显示、睡眠、断电、释放。 |

请在 `display_init()` 之后绘制第一帧，再打开背光。不希望自动打开显示的屏幕 IC（例如 AMOLED）在它的描述里有标志，此时由应用在
第一帧送出之后自己发命令打开显示。

`display_get_info()` 返回分辨率、方向、像素格式、对齐要求、单次绘制的最大长度以及是否使用 DMA，GUI 适配层据此确定缓冲区大小。

GRAM 屏的绘制使用 `display_draw()`（同步）或 `display_draw_async()`。异步调用立即返回，在 `display_set_callback()` 注册的回调收到
`DISPLAY_EVT_DRAW_DONE` 事件之前，不能修改像素缓冲区。方向用 `display_set_rotation()` 修改，宽高随之交换。
需要底层访问时，`display_get_panel()` 和 `display_panel_get_io()` 返回下层对象，端点提供发送命令、读取屏幕 ID 和写入像素数据的函数。

### 3.5 视频屏（RGB 和 MIPI DSI）

视频屏不是“画进去”的：scanout 引擎持续读取帧缓冲。帧缓冲归应用所有（通常在 PSRAM 里，BL618DG 上要按 cache line 对齐），由应用告诉显示该显示哪一块。
BL618DG 上的 MIPI DSI 屏这样定义：

```c
#include "bus/dsi/display_dsi.h"
#include "scanout/bl618dg_dpu/display_bl618dg_dpu_scanout.h"

static display_dsi_host_t dsi0 = DISPLAY_DSI_HOST(.lanes_wired = 2, .lane_order = DISPLAY_DSI_LANES_3210);
static display_dsi_io_t dsi_io = DISPLAY_DSI_IO(&dsi0, 0, .iface = DISPLAY_IFACE_DSI_LP, .cs = DISPLAY_PIN_NONE);
static display_board_gpio_t board = { .reset = DISPLAY_PIN_LOW(2) };
static display_dcs_panel_t panel =
    DISPLAY_DCS_PANEL(&dsi_io.base, &display_module_ek79007_1024x600_2lane, DISPLAY_BOARD_GPIO(&board));

static uint16_t frame[2][1024 * 600] ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(64)));
static display_bl618dg_dpu_scanout_t dpu = DISPLAY_BL618DG_DPU_SCANOUT(DISPLAY_FMT_RGB565, .dsi = &dsi0);
static display_t lcd = DISPLAY_FB(&panel, &dpu);
```

屏幕模组带着玻璃的固定链路参数（时序、像素时钟，DSI 另有 lane 数、lane 速率和数据类型），框架不规划也不交叉校验它们。RGB 引脚输出时，scanout 用 `.pins`（板上接线：
24、18 或 16 位接法）和复用引脚的板级函数代替 `.dsi`，屏幕用标准 RGB 屏，例如 `DISPLAY_VIDEO_PANEL(&display_module_rgb_800x480, DISPLAY_BOARD_NONE)`。BL616 的
SIM v1 scanout（`DISPLAY_SIM_V1_SCANOUT(...)`，`scanout/sim_v1`）只用于 BL616（`DISPLAY_SCANOUT_SIM_V1`，在 BL616 上默认打开）。

`display_video_mode_t` 的 `pclk_invert`、`hsync_invert`、`vsync_invert`、`de_invert` 用于反转 DPI/SRGB 信号，
默认均为 `false`，保持后端默认行为；不支持的选项由后端忽略。SIM v1 支持 HSYNC、VSYNC、DE 反转，忽略 PCLK 反转；
BL618DG DPU 的并行 RGB 输出支持 PCLK 反转（默认下降采样沿，反转为上升采样沿），忽略其余三项。DSI 链路忽略这四项。

| 调用 | 作用 |
|---|---|
| `display_init()` | 初始化面板/链路并配置 scanout，不读帧。READY 后注册回调。 |
| `display_flip(fb)` | 非 NULL 启用/替换且匹配 cfg.format；NULL 关闭；最多一个 pending（含 NULL，否则 -EBUSY）。关态 NULL 是无事件 no-op。 |
| `DISPLAY_EVT_FB_FIRST` | 首帧生效，没有旧缓冲归还；等此事件再开背光。 |
| `DISPLAY_EVT_FB_RETIRED` | 上一帧已经不再被读取，可以重写。事件不带地址：应用自己记着被替换的是哪一帧。 |
| `DISPLAY_EVT_FB_LAST` | 层已关闭，最后一帧归还。 |
| `display_deinit()` | 排空 pending、提交 LAST 并等 IRQ 确认，再停输出/DSI HS，之后才关显示、睡眠、断电。失败进入 ERROR，保留未确认帧/资源供重试；不以超时或固定延时假释放。 |

注意事项：

- **显式首帧**：init 不读缓冲。先注册回调，填好并 flip 首帧，等 FIRST 再开背光；以后画刚归还帧，替换后等 RETIRED。
  每层一个拥有者，负责串行操作（含任务/ISR 提交），不提供多任务仲裁；提交前登记期望事件，完成可能早于调用返回。超时不能当作帧释放。
- **调用上下文。**视频 flip、查询及回调设置允许 ISR；完成事件回调中可续交下一帧，不必先等另一个边界。
  调用耗时、cache clean 和 SIM LLI 构建由调用者预算，ISR 不得阻塞等待下一帧。
  初始化/配置能否在 ISR 调用取决于后端和板级 hook；IO/Bus、Display init/deinit 与真正执行停止的 DPU hw_stop
  因可能同步等待，仍限任务上下文，不能据此承诺所有 API 都可在 ISR 调用。
- **MIPI DSI 的命令和视频互斥。**视频运行期间，发给屏幕的命令返回 `-EBUSY`；显示会在启动视频之前把需要的命令都发完。
- **色序和反色就是初始化序列设定的。**框架不强制修改视频屏的色序和反色，也没有对应的模组变体：一块屏幕一个模组，由它的序列决定。
  需要别的设置时，自己提供模组（复制一份，改 `init` 或 `init_extra`）；MIPI DSI 屏的默认屏则用 `display_default_get_module()`
  修改模组 RAM 副本的 `init_extra` 或 `init_user`（3.2 节）。
- **背光。**`display_set_backlight()` 的用法与 GRAM 屏相同。BL618DG 开发板的背光常亮、没有控制引脚，这个调用在那里返回 `-ENOTSUP`。
- **缓存。**scanout 公共层每次提交只 clean 一次（NV12 两平面），cfg.dcache_dis 可关闭；任务中的长 clean 不屏蔽 IRQ，ISR 中保留调用者 IRQ 状态，只有短状态/MMIO 发布关 IRQ。core 与 DPU 在 clean 后、发布前再次短 IRQ 检查 pending、state/running、current；ISR 抢先提交后外层返回 `-EBUSY`，不覆盖其提交。适配器不重复 clean。
- **边界。**FIRST/RETIRED/LAST 在同层同 IRQ 替代 VSYNC，不同时报告；FPS 数四种总数，VSYNC 不能误唤醒完成等待。DPU 以硬件写后首个 IRQ 确认退休（含 LAST），该确认依赖帧中断与硬件读取完成的时序关系，不构成全部物理读访存已结束的保证。Base/OSD0 共享 IRQ，回调续交的普通帧不得在本 IRQ 确认；窗口覆盖用后续两个边界，第一次写入、第二次确认。短 IRQ 临界区只保护任务/ISR，不提供多任务锁。
- **BL618DG 的图层。**OSD0 从适配器 init 到成功 deinit 一直被 claim，包括关闭时。Base 层（常量颜色或帧缓冲，含 YUV 帧）和 OSD1 窗口层通过 scanout 里的硬件层对象 `dpu.hw` 使用：
  关态且无 pending 时 layer_config(format/window/alpha/cache/color_r/color_g)，随后 layer_flip(hw,layer,fb,win) 传完整 fb 或 NULL。
  Base 默认黑且无 AXI；OSD0 被占用时 config/flip/alpha/callback 修改均 -EBUSY（含关态），info 可读。
  见 [`scanout/bl618dg_dpu/README_CN.md`](scanout/bl618dg_dpu/README_CN.md)。ARGB8888 帧的像素 alpha 叠在下面各层上。
- **对齐。**帧紧凑排列。地址要对齐到 cache line（BL618DG）或 4 字节（SIM v1），SIM v1 的整帧大小必须是 4 字节的倍数。
- **RGB 引脚的板级函数。**板子要提供 `board_display_dpu_gpio_init()` / `board_display_dpu_gpio_deinit()`（4.3 节）；`bl618dgdk` 已提供这对函数。
- **芯片和版本。**BL618DG 的 DSI Host 和 DPU 需要 B0 版本；A0 版本和低功耗核不支持，也不检查。

### 3.6 注意事项

- **片选。**除非另有说明，CS 是软件 GPIO；板级引脚函数不会把 CS 引脚复用给控制器。定义时不写 CS 的屏幕始终处于选中状态，
  必须独占总线，否则挂接返回 `-EBUSY`。
- **接口兼容。**端点的接口必须同时被总线、屏幕 IC 和模组支持，否则 `display_init()` 返回 `-ENOTSUP`。QSPI 的协议细节来自 IC 描述。
- **缓冲区。**像素缓冲区地址要按 `info.buf_align` 对齐（DBI 和 SPI 为 4 字节），绘制区域要按 `x_align` / `y_align` 对齐。
  单次绘制不能超过 `max_draw_bytes`，它由 DMA 链表节点数（`LLI_COUNT`）决定：DBI 外设每个节点约 16 KB，SPI（RGB565）每个节点约 8 KB。
- **回调。**GRAM 的 DRAW_DONE 在 DMA 完成中断中运行（总线没有 DMA 时在调用者任务中运行），仍以通知为主，ISR 内不得续交 IO/Bus 传输。
  视频回调在帧中断中运行，完成事件回调中可查询、设置视频回调或续交下一帧，遵守 3.5 节约束，不得阻塞等待。
- **并发。**同一块屏幕的生命周期调用和方向修改必须由调用者串行化；共享总线的不同屏幕的绘制由总线仲裁，可以来自不同任务。
  总线上有触摸芯片、Flash 等其他设备时，需要在 `display_bus_acquire()` 和 `display_bus_release()` 之间访问。
- **静态定义中省略的字段。**省略的字段为零，各有明确含义：没有软件 CS、默认时钟、RGB565、位宽由格式推导。省略接口会被拒绝；
  指定了 DMA 通道却没有 DMA 链表，或 DBI 总线没有接口列表，返回 `-EINVAL`。

### 3.7 简单内存绘字

可选的 [`simple_font`](simple_font/README_CN.md) 将 ASCII 字符串同步绘制到 RGB565、RGB888 或 XRGB8888 缓冲。
通过只读配置指定图像尺寸、stride、字体和 ARGB 前景/背景色，调用时只传缓冲起始地址、坐标和字符串。
启用 `DISPLAY_SIMPLE_FONT` 后默认提供 8×16 字体；其他字库可独立选择。该工具不提交显示、不做 cache 操作。

## 4. 配置

### 4.1 Kconfig

每一层（`bus/`、`panel/`、`scanout/`、`default/`）及可选工具 `simple_font/` 各有一个 `Kconfig` 和一个 `CMakeLists.txt`。
顶层 [`Kconfig`](Kconfig) 包含这些配置及 `DISPLAY_BUS_LOCK_TIMEOUT_MS`，`bsp/Kconfig` 在 `BSP_DISPLAY` 下包含它。

| 选项 | 含义 |
|---|---|
| `BSP_DISPLAY` | 使能框架（在 `bsp/Kconfig` 里；BL616、BL616CL、BL618DG、BL702L）。使能后本芯片的所有后端默认打开，工程只关掉不想要的。后端静态分配、链接器丢弃未引用的代码，所以不用的后端不占代码空间。 |
| `DISPLAY_PANEL_<IC>` | 每个屏幕 IC 一个选项，默认全部打开（`ST7789`、`ST7796`、`ILI9341`、`ILI9488`、`GC9A01`、`GC9307`、`ILI9486`、`JD9853`、`ST7735`、`ST77922`（也包含 ST77926）、`SPD2010`、`NT35510`、`ST7701S`、`ST7701P`、`GC9503V`、`JD9165BA`、`EK79007`、`AXS15231B`、`AXS15231E`、`FL7707N`、`ILI9806E`、`ILI9881C`、`JD9365TX`、`ST7102`）。关掉的屏幕不编译，所以引用它的模组的工程会链接失败。通用 DCS 驱动和 `panel/video_standard` 始终编译。默认屏的模组选项 `depends on` 对应的屏幕选项。选项在 [`panel/Kconfig`](panel/Kconfig)。 |
| `DISPLAY_BUS_SPI`、`_SOFT_DBI_C` | 总线后端，默认打开。 |
| `DISPLAY_BUS_DBI` | DBI 外设总线后端，在 BL616、BL616CL、BL618DG 上默认打开。 |
| `DISPLAY_BUS_DSI` | MIPI DSI Host，只在 BL618DG 上默认打开、也只能在 BL618DG 上选。 |
| `DISPLAY_SCANOUT_BL618DG_DPU` | BL618DG DPU scanout 后端（会选中 `DISPLAY_BUS_DSI`），只在 BL618DG 上默认打开、也只能在 BL618DG 上选。 |
| `DISPLAY_SCANOUT_SIM_V1` | BL616 的 SIM v1 scanout 后端，只在 BL616 上默认打开、也只能在 BL616 上选。它需要提供 ROM 函数表符号 `__edata_etext_final` 的库；该符号是弱符号，没有库时仍能链接，但 RGB 屏在该 scanout 上 `display_init()` 返回 `-ENOTSUP`。 |
| `DISPLAY_DBI_CLK_MODE`、`DISPLAY_SPI_MODE` | 后端的时钟极性和相位，对该后端的所有端点相同（Soft DBI C 固定为模式 0）。 |
| `DISPLAY_BUS_LOCK_TIMEOUT_MS` | 等待总线的最长时间。 |
| `DISPLAY_SIMPLE_FONT`、`DISPLAY_SIMPLE_FONT_*` | 可选的纯内存 ASCII 绘字及各字库；见 [simple_font](simple_font/README_CN.md)。 |
| `DISPLAY_DEFAULT`、`DISPLAY_DEFAULT_*` | 默认屏（3.2 节和 3.5 节）。 |
| `LVGL8_PORT_DISP`、`LVGL9_PORT_DISP`、`LVGL*_DISP_DEFAULT_*` | 默认屏的 LVGL 显示 port；选项在 `components/graphics/lvgl_v8/port/Kconfig` 和 `lvgl_v9/port/Kconfig`（3.2 节）。 |

### 4.2 时钟

端点的时钟按以下顺序取值：端点自己的设置、屏幕 IC 在该接口上的默认值、接口的默认值（Type B 为 27 MHz，其余为 40 MHz）。
最终结果受限于控制器能产生的频率。Soft DBI C 的时钟不可配置。

### 4.3 板级引脚函数

总线后端不知道引脚号。板级为每个接口提供一对函数，分别用于复用和释放引脚，例如 `board_display_dbi_b8_gpio_init()` / `_deinit()`、`board_display_spi_gpio_init()`、
`board_display_qspi_gpio_init()`，BL618DG 的 RGB 引脚用 `board_display_dpu_gpio_init()`。约定见 [`default/display_default_gpio_ext.h`](default/display_default_gpio_ext.h)；
每块板的函数在 `bsp/board/<board>/board_gpio.c`（overlay 板在 `board_gpio_overlay.c`），在 `board_gpio.h` 中声明，随 `CONFIG_BSP_DISPLAY` 编译。芯片支持的接口，函数必须在板级目录里（缺一个就是链接错误；还没定下来的函数是带 `TODO` 的空函数），
芯片没有的接口在 Kconfig 里是隐藏的。`bl616dk`、`bl616cldk`、`bl618dgdk`、`bl702ldk` 的表取自芯片的 pinmux 表；`bl616cl_ave_v10` 只有空函数。新板子可以参考
[`bl616dk`](../../board/bl616dk/board_gpio.c)。

数据接口的函数不会复用 CS 引脚；CS 由 DBI 硬件驱动的屏幕使用另外的 `..._cs_gpio_init()` 函数。自己定义屏幕的工程把自己的函数交给总线的静态定义
（`DISPLAY_DBI_IFACES()`，SPI 的 `func_init`、`func_deinit` 字段），不需要这些函数。SIM v1 后端的引脚号写在它的配置里。

## 5. 增加屏幕

### 5.1 为已有的 IC 增加屏幕模组

使用同一颗驱动 IC 的屏幕通常只在分辨率、GRAM 偏移、方向、反色和伽马上不同。这类屏幕只需在**应用工程**里写一份模组描述，框架本身不用修改。
模板见 [`panel/template/display_module_template.c`](panel/template/display_module_template.c)。

模组描述包含：

| 项目 | 含义 |
|---|---|
| IC | 要复用的 IC 描述（初始化序列和接口）。 |
| 尺寸 | 0° 时的可见宽度和高度。 |
| 方向 | 支持的方向，以及每个方向的内存访问控制（MADCTL）值，包括 RGB/BGR 顺序和镜像。 |
| GRAM 偏移 | 屏幕玻璃在 GRAM 中的位置，用于比 IC 的 GRAM 小的屏幕。 |
| 反色 | 是否开启反色。 |
| 附加命令 | 追加在 IC 序列之后的命令（例如伽马、电压），或屏幕厂商提供的完整替换序列。 |
| 接口 | 可选，限定模组实际接成的接口。 |

初始化时会校验方向表：90° 和 270° 必须交换行列，分辨率加偏移必须落在 GRAM 内，否则 `display_init()` 返回 `-EINVAL`。
模组随后用于 `DISPLAY_DCS_PANEL()`，或选作默认屏的自定义模组。

`display_dcs_module_adjust()` 是 DCS 驱动的通用函数，复制一份 GRAM 模组并修改其 RGB/BGR 位、镜像和反色。默认屏不使用它；应用代码需要时可以自己调用。

视频屏一块屏幕一个模组，色序和反色就是它的初始化序列设定的；没有 `_bgr` 变体，也没有强制修改的选项。

### 5.2 增加新的驱动 IC

遵循标准 MIPI DCS 命令集的 IC 只需要数据，不需要代码。在 `panel/gram_<ic>/` 下创建 `display_<ic>.h` 和 `display_<ic>.c`，
可参考 [`panel/gram_st7796`](panel/gram_st7796)。其中包含：

- 厂商的初始化序列，每一项是命令、延时和参数；
- 支持的接口，每个接口带有标准设置：16 位和 24 位数据的像素格式码、默认时钟、命令编码，QSPI 还有协议；
- IC 描述（GRAM 尺寸、窗口对齐、复位时序）和至少一个默认模组。

初始化序列的规则：

- 不要包含显示开关命令（`0x29`/`0x28`）。对 GRAM 屏，也不要包含内存访问控制（`0x36`）、像素格式（`0x3A`）和反色命令，
  驱动会在序列之后按方向、接口和模组发送。
- 软件复位和退出睡眠（`0x11`）及其延时要写在序列里。
- 只在开启反色时才需要的设置，单独写在 IC 描述里。

然后在 [`panel/Kconfig`](panel/Kconfig) 增加 `DISPLAY_PANEL_<IC>` 选项（默认 y），并把源文件加入 [`panel/CMakeLists.txt`](panel/CMakeLists.txt)
（放在该选项的条件下）；要让模组能在默认屏中选择，再在 [`default/Kconfig`](default/Kconfig) 增加 `depends on` 该屏幕选项的选项，并在 `default/display_default_gram.c` 增加对应分支。

命名：IC 描述为 `display_ic_<ic>`，内置模组为 `display_module_<ic>_<宽>x<高>`；应用工程自己的模组不要使用 `display_module_` 前缀。

视频屏的 IC（RGB 或 MIPI DSI）也这样描述，放在 `panel/video_<ic>/`。模组另外带有视频链路（`display_video_mode_t`：链路类型、时序、时钟，DSI 还有 lane 数、速率和数据类型）和
最后一条命令到第一个像素之间的等待时间。同时有 RGB 和 DSI 输入的 IC 是同一份 IC 描述，命令端点的接口决定用哪条视频链路，DSI 专用的初始化序列按接口给出。不接受显示开关命令的 IC（EK79007）要在描述里加标志。
可以参考内置屏幕模组，例如 [`panel/video_ek79007`](panel/video_ek79007)。

### 5.3 非标准协议的 IC

需要另写屏幕驱动：提供屏幕操作（初始化、电源、使能、旋转、绘制），把屏幕核心对象作为驱动结构体的第一个成员，通过 IO 层发送命令。
[`panel/dcs`](panel/dcs) 里的通用 DCS 驱动可作参考。

## 6. 目录结构

```text
display.h / display.c        Display：应用入口
display_types.h              公共类型：像素格式、引脚、区域、命令接口、视频链路、事件
bus/                         总线核心和端点；后端 spi/、dbi/、soft_dbi_c/、dsi/（MIPI DSI Host，BL618DG）；每层各有 Kconfig 和 CMakeLists.txt
panel/                       屏幕驱动：dcs/（通用驱动）、gram_<ic>/（GRAM 屏：IC 描述和模组）、video_<ic>/（RGB / MIPI DSI 屏）、video_standard/（无驱动 IC 的标准 RGB 屏）、template/；Kconfig 里每个 IC 一个 DISPLAY_PANEL_<IC> 选项
scanout/                     视频屏的 Scanout 核心和后端：bl618dg_dpu/（RGB 引脚和 MIPI DSI，有自己的 README）、sim_v1/（BL616，需要额外的 ROM 函数表库）
default/                     默认屏：Kconfig 和实现（GRAM 屏和视频屏），以及板级引脚函数的约定
```

各层可以独立使用，按需要包含对应头文件：

| 用途 | 头文件 |
| --- | --- |
| 使用已有 Bus：借用总线、查询统计 | `bus/display_bus.h` |
| 使用 IO：配置端点、发送命令和像素 | `bus/display_io.h` |
| 使用已有 Scanout：配置、提交帧、查询状态 | `scanout/display_scanout.h` |
| 定义具体 Bus 或 Scanout 对象 | 对应后端头文件，例如 `bus/spi/display_spi.h`、`scanout/bl618dg_dpu/display_bl618dg_dpu_scanout.h` |
| 直接控制 DPU 图层 | `scanout/bl618dg_dpu/display_bl618dg_dpu_hw.h` |
| 实现 Bus 或 Scanout 后端 | `bus/display_bus_driver.h`、`scanout/display_scanout_driver.h` |

具体后端头文件会包含所需的通用接口和完整基类定义，可直接使用现有初始化宏静态定义对象。
`*_driver.h` 集中放置操作表、基类对象和实现辅助接口；使用中间层无需创建 `display_t`。
已交给 Display 管理的 Panel 或 Scanout，其生命周期仍由 Display 管理。

各板的引脚函数在 `bsp/board/<board>/board_gpio.c`（overlay 板在 `board_gpio_overlay.c`）。

本目录包含显示框架；应用示例位于 [`examples/display`](../../../examples/display)。

## 7. 当前限制

视频屏：

- SIM v1（BL616，并行 RGB）需要额外提供 ROM 函数表符号 `__edata_etext_final` 的库。该符号是弱符号，所以 BL616 上带 `DISPLAY_SCANOUT_SIM_V1`（在 BL616 上默认打开）的构建仍能链接，但没有这个库时，RGB 屏在该 scanout 上 `display_init()` 返回 `-ENOTSUP`。BL616CL 的串行 RGB 后端尚未实现。
- BL618DG 的 DPU：YUYV 和 NV12 帧只能给 Base 层；OSD 换窗口必须同时提交相应大小的帧（layer_flip 的 win 参数）。视频屏不支持旋转。
- DSI Host 只支持 40 MHz 晶振和 400、500、550、650、750、850 Mbit/s 这几档 lane 速率，一个 Host 一块屏，不支持读，不支持命令模式的屏。通信出错后 Host 进入失败状态（之后每个调用都返回 `-EIO`），
  因为控制器不能单独复位。视频运行期间不能向屏幕发命令。
- 信号反转选项只在后端支持时生效（3.5 节），不支持的项忽略。DPU 像素时钟映射到芯片能产生的最接近的时钟（例如标准 800x480 模组的 33.39 MHz 实际是 34.29 MHz）。
- DSI 链路的像素时钟达到 60 MHz 及以上时，行缓冲计算会加 4 MHz。
- 框架不能强制修改视频屏的色序和反色，它们由初始化序列决定；MIPI DSI 屏的默认屏可以用 `display_default_get_module()` 修改模组 RAM
  副本里的命令（3.2 节）。标准 RGB 屏没有命令口，不能使用 `display_default_get_module()`。
- 标准串行 RGB 模组只有时序数据，还没有 scanout 后端驱动串行 RGB 链路。
- 不支持 BL618DG 的 A0 版本和它的低功耗核。BL618DG 开发板的背光常亮。
- scanout 没有硬件错误事件；运行中的故障由停止它的调用返回。

其他：

- 多屏幕的 LVGL port（默认屏的 LVGL v8 和 v9 port 已提供，GRAM 屏和视频屏都有）；
- 板级引脚需要与实际接线匹配，特别是 DBI 硬件 CS；`bl616cl_ave_v10` 的显示引脚函数为空，需要板级自行实现。
