# BL618DG DPU

[English](README.md)

BL618DG 的显示处理单元（DPU）从内存读取帧，混合最多三个图层，再经 **MIPI DSI** 或 **RGB 引脚**输出到视频屏。
本目录是它的驱动：一部分把它接到显示框架，另一部分是可以单独使用的硬件层。

## 功能特性

- 三个图层，从下到上：

  | 图层 | 显示内容 |
  |---|---|
  | `DISPLAY_BL618DG_DPU_LAYER_BASE` | 整个屏幕：纯色（默认黑，不访问内存），或帧缓冲：RGB565、RGB888、XRGB8888，以及摄像头常用的 YUV 格式 YUYV、UYVY、NV12 |
  | `DISPLAY_BL618DG_DPU_LAYER_OSD0` | 一个窗口（默认整屏），帧为 RGB565、XRGB8888、ARGB8888 或 AYUV8888。`display_t` 的屏幕层就是它 |
  | `DISPLAY_BL618DG_DPU_LAYER_OSD1` | 与 OSD0 相同，叠在它上面 |

- YUV 帧（YUYV、UYVY、NV12）只能给 Base 层；由 DPI 自己转换成 RGB，所以摄像头的帧不用在软件里转换就能显示。
  YUV 数据的范围可以在配置里选全范围（0..255）或有限范围（16..235，多数摄像头给的）。OSD 层使用 RGB565、XRGB8888、ARGB8888
  和 AYUV8888（`0xAAYYUUVV`）。
- OSD 窗口可以是屏幕内任意大小和位置。每个 OSD 层叠在下面各层上，可以用整层常数 alpha，或每个像素自带的 alpha
  （ARGB8888，非预乘）。
- config 不读帧，显式 flip 提交；事件区分 FIRST/RETIRED/LAST 和普通 VSYNC。
  写入后首个 IRQ 确认退休是实现假设，不是所有物理读访存已经结束的证明。
- OSD 层的窗口可以在运行中改变：本层帧 IRQ 写新窗口/地址，下一 IRQ 确认，共两个边界；不保证硬件原子锁存。
- `display_bl618dg_dpu_hw_layer_get_info()` 返回 current/pending/uv_current/uv_pending **地址指针**，
  以及 running、format、window、pending_op；内部 layer 的 current/pending 才是完整 display_fb_t。
- RGB 引脚（24、18、16 位接法）或 MIPI DSI（HS 视频；屏的命令走同一个 DSI Host 的 LP 通道）。
- 硬件层可以不经 `display_t` 单独使用：自己写面板驱动，甚至没有面板驱动都可以。

## 文件

| 文件 | 作用 |
|---|---|
| `display_bl618dg_dpu_hw.[ch]` | 硬件层：输出和三个图层。这里唯一调用 DPU 驱动（LHAL）的代码，不依赖 `display_t`。头文件 adapter-only 区段声明 `display_bl618dg_dpu_osd0_{claim,release,config,flip}`，供适配器管理 OSD0 占用和提交 |
| `display_bl618dg_dpu_scanout.[ch]` | 适配器：把显示框架的 scanout 核心接到硬件层的 OSD0 层 |
| `display_bl618dg_dpu_clock.[ch]` | BL618DG DPU 显示时钟和 DSI PLL 步骤，与 DSI Host 共用 |

用 `CONFIG_DISPLAY_SCANOUT_BL618DG_DPU` 打开（BL618DG 上默认打开，并选中 DSI 总线）。默认显示（`CONFIG_BSP_DISPLAY`
选视频屏）会替你使用它。

直接使用硬件层只需包含 `display_bl618dg_dpu_hw.h`；创建 Scanout 对象则包含
`display_bl618dg_dpu_scanout.h`。定义 MIPI DSI Host 时，另行包含 `bus/dsi/display_dsi.h`。

## 与 `display_t` 一起使用

屏幕就是 OSD0 层。定义只含格式，不含首帧；init 不读帧，READY 后注册回调，再显式提交首帧。
提交前登记期望事件：完成可能早于调用返回。以下片段省略应用等待和错误处理，实际必须检查每次返回值。
`panel` 和 `dsi0` 的定义见[框架使用说明第 3.5 节](../../README_CN.md#35-视频屏rgb-和-mipi-dsi)。

```c
#include "display.h"
#include "scanout/bl618dg_dpu/display_bl618dg_dpu_scanout.h"

static uint16_t frame[2][1024 * 600] ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(64)));

static display_bl618dg_dpu_scanout_t dpu = DISPLAY_BL618DG_DPU_SCANOUT(DISPLAY_FMT_RGB565, .dsi = &dsi0);
static display_t lcd = DISPLAY_FB(&panel, &dpu);

display_init(&lcd);                                /* 建立链路、初始化面板、配置；不读帧 */
display_set_callback(&lcd, on_display_event, ctx); /* ISR 完成回调可续交下一帧 */
display_flip(&lcd, &(display_fb_t){ .data = frame[0], .format = DISPLAY_FMT_RGB565 });
/* 等 FIRST 后再开背光；FIRST 不归还任何旧帧。 */
display_flip(&lcd, &(display_fb_t){ .data = frame[1], .format = DISPLAY_FMT_RGB565 });
/* DISPLAY_EVT_FB_RETIRED：旧帧可以重写 */
display_flip(&lcd, NULL); /* 等 LAST 后最后一帧可复用 */
display_deinit(&lcd); /* 返回 int；失败保留帧和资源，可重试 */
```

RGB 引脚输出不写 `.dsi`，改写 `.pins`（引脚接法）、`.pins_init` 和 `.pins_deinit`（板级的引脚复用函数）。

Base 层和 OSD1 通过 scanout 里的硬件层对象 `dpu.hw` 使用：

```c
display_bl618dg_dpu_layer_cfg_t base = { .format = DISPLAY_FMT_RGB565, .color_r = 255 };
display_bl618dg_dpu_hw_layer_config(&dpu.hw, DISPLAY_BL618DG_DPU_LAYER_BASE, &base); /* 关态 Base 红，无 AXI */

display_bl618dg_dpu_layer_cfg_t win = { .format = DISPLAY_FMT_ARGB8888, .window = { 100, 80, 320, 200 } };
display_bl618dg_dpu_hw_layer_set_callback(&dpu.hw, DISPLAY_BL618DG_DPU_LAYER_OSD1, on_osd1, NULL);
display_bl618dg_dpu_hw_layer_config(&dpu.hw, DISPLAY_BL618DG_DPU_LAYER_OSD1, &win);
display_fb_t overlay = { .data = win_frame[0], .format = DISPLAY_FMT_ARGB8888 };
display_bl618dg_dpu_hw_layer_flip(&dpu.hw, DISPLAY_BL618DG_DPU_LAYER_OSD1, &overlay, NULL); /* 等 FIRST */
display_bl618dg_dpu_hw_layer_flip(&dpu.hw, DISPLAY_BL618DG_DPU_LAYER_OSD1, NULL, NULL); /* 等 LAST */
```

Base 默认黑，不读帧。输出运行后，层关闭且无 pending 时 layer_config 选择 format/color_r/color_g。
统一 layer_flip 提交完整 fb；NULL → LAST → config 新格式/纯色 → fb → FIRST，可保持同一输出且不影响 OSD0/1。

摄像头的帧就填它的格式：`DISPLAY_FMT_YUYV`（字节顺序 Y0 U Y1 V）、`DISPLAY_FMT_UYVY`（U Y0 V Y1）或
`DISPLAY_FMT_NV12`（一个 Y 平面，后面是 UV 交错平面，U 在前，4:2:0）。NV12 还要给出 UV 平面，翻页时两个平面一起切换：

```c
display_bl618dg_dpu_layer_cfg_t nv12_cfg = { .format = DISPLAY_FMT_NV12 };
display_bl618dg_dpu_hw_layer_config(&dpu.hw, DISPLAY_BL618DG_DPU_LAYER_BASE, &nv12_cfg);
display_fb_t nv12 = { .data = y_plane[0], .uv = uv_plane[0], .format = DISPLAY_FMT_NV12 };
display_bl618dg_dpu_hw_layer_flip(&dpu.hw, DISPLAY_BL618DG_DPU_LAYER_BASE, &nv12, NULL);
```

窗口运行中改变（OSD 层；`display_t` 使用 OSD0 时不能用，它是整屏大小）：

```c
display_bl618dg_dpu_window_t win = { .x = 400, .y = 100, .width = 500, .height = 300 };
display_fb_t bigger = { .data = big_frame, .format = DISPLAY_FMT_ARGB8888 };
display_bl618dg_dpu_hw_layer_flip(&dpu.hw, DISPLAY_BL618DG_DPU_LAYER_OSD1, &bigger, &win);
```

YUV 帧的范围用配置里的 `.yuv_range = DISPLAY_BL618DG_DPU_YUV_LIMITED` 选择（默认 `DISPLAY_BL618DG_DPU_YUV_FULL`）。

回调形式 `void cb(display_event_type_t type,void *ctx)`，不带地址。FIRST 首帧生效但无旧帧归还，
RETIRED 归还上一帧，LAST 归还最后一帧；普通边界才报 VSYNC。同层同 IRQ 只一种事件，FPS 数四种总数。
VSYNC 不能唤醒 FIRST/RETIRED/LAST 等待。关态 NULL 无事件；pending LAST 通过 pending_op 区分。

## 不经 `display_t` 使用

自己拉起面板（DSI 屏用面板驱动发 LP 命令），然后：

```c
static display_bl618dg_dpu_hw_t hw = { .cfg = { .dsi = &dsi0 } };

display_bl618dg_dpu_hw_open(&hw);
display_bl618dg_dpu_hw_layer_set_callback(&hw, DISPLAY_BL618DG_DPU_LAYER_BASE, on_base, NULL);
display_bl618dg_dpu_hw_start(&hw, info.video_mode); /* 先等 video_start_delay_ms；黑 Base、OSD 关 */
display_bl618dg_dpu_hw_layer_config(&hw, DISPLAY_BL618DG_DPU_LAYER_OSD1, &win);
display_bl618dg_dpu_hw_layer_flip(&hw, DISPLAY_BL618DG_DPU_LAYER_OSD1, &overlay, NULL);
/* ... 翻页 ... */
display_bl618dg_dpu_hw_stop(&hw);                                /* 依次停 OSD1、OSD0、输出；然后再关面板 */
```

## 注意事项

- **每层一个拥有者，串行操作（含任务/ISR）。**不提供多任务仲裁，无整机 owner。适配器占用 OSD0（包括关态），公开 config/flip/alpha/callback 修改均 -EBUSY，
  info 仍可读；不通过内部函数或 LHAL 绕过。Base/OSD1 独立。已运行输出只接受逐字段一致的 mode。
- **帧。**行紧密排列（`宽 × 每像素字节数`，没有行填充），地址按 cache line 对齐。正在显示的帧归硬件所有：
  RETIRED 归还上一帧，LAST 归还最后一帧，或成功 deinit 后才可重写；FIRST 不释放旧帧。每块缓冲提交前填好，超时不释放。
- **数据 Cache。**一次提交只 clean 一次（NV12 两平面）：Display 由 scanout 公共层，直接层由硬件层，
  cfg.dcache_dis 可交给调用者管理。任务中的长 clean 不屏蔽 IRQ，ISR 中保留调用者 IRQ 状态，只有短状态/MMIO 发布关 IRQ。
  core 与 DPU 在 clean 后、发布前再次短 IRQ 检查 pending、state/running、current；ISR 抢交后外层返回 `-EBUSY`，不覆盖其提交。
  适配器不重复 clean，临界区不提供多任务互斥。
- **翻页。**`layer_flip()` 不阻塞，查询及视频回调设置同样允许任务或 ISR 调用。同一层上一帧还没报告就再翻返回 `-EBUSY`。
  没有多层的原子翻页：每一层在自己的帧边界切换。
- **报告回调在帧中断里运行。**先更新状态再通知，完成事件回调中可续交下一帧。调用耗时/cache clean 由调用者预算，不得阻塞等待下一帧。
  Scanout 及 DPU open/start/层访问能否在 ISR 调用取决于后端和板级 hook。
  IO/Bus、Display init/deinit 与真正执行停止的 DPU hw_stop 因同步等待仍限任务上下文，并非所有 API 都可在 ISR 调用。
- **Base。**纯色红绿、蓝为零；关态 config 的 color_r/color_g 即时生效。NULL 关闭 AXI，LAST 归还帧。
  Base 和 OSD0 共用 OSD0 SEOF。
- **每一层的格式。**每个层有自己支持的帧格式集合，`display_bl618dg_dpu_hw_layer_formats(layer)` 以掩码返回。不在集合里的格式在
  配置层时返回 `-ENOTSUP`，例如 OSD1 用 YUYV、Base 用 ARGB8888。`display_t` 的 scanout 提供的是 OSD0 的集合。
- **改变窗口。**`layer_flip(fb,win)` 不阻塞、一次一个（帧或窗口还没生效时返回 `-EBUSY`），回调报告
  它之前那一帧什么时候可以重写。新窗口和新帧由这一层的帧中断写入，不是调用本身写入。之后的普通 `layer_flip()` 是新窗口的大小。
  `display_bl618dg_dpu_hw_layer_get_info()` 返回最近写入硬件的窗口，此时操作可能仍 pending；ISR 写入后下一 IRQ 才完成，共后续两个边界。
  通过 FIRST/RETIRED 或 `pending_op` 判断完成，不要仅凭窗口字段判断。
- **YUV 帧。**层关闭且无 pending 可改格式。宽度必须是偶数，NV12 的高度也必须是偶数。转换成 RGB 由 DPI
  完成，按全范围 BT.601（Y、U、V 取 0..255）。NV12 的 Y 平面和 UV 平面（`宽 × 高 / 2` 字节，每行 `宽` 字节）都要
  紧密排列、按 cache line 对齐，一起提交归还；回调无地址。YUYV 和 NV12 帧不能给 OSD 层，OSD 层可以用 AYUV8888（`0xAAYYUUVV`，像素 alpha
  的用法与 ARGB8888 相同）。yuv_range 影响整个输出，改变需成功停/重启输出，不能当作逐层配置。
- **OSD 层。**格式 RGB565、XRGB8888、ARGB8888（RGB565 和 XRGB8888 不透明，除非设了常数 alpha；ARGB8888 使用像素自带的
  alpha）。cfg.alpha=0 对有 alpha 格式使用像素 alpha，否则不透明；1..255 是常数。fb+win 可运行中换窗口。
  层的上下顺序固定：Base < OSD0 < OSD1。
- **帧中断。**Base/OSD0 共用 OSD0 SEOF，OSD1 独立 SEOF 且 LAST 后可停 IRQ，关态不要求 VSYNC。
  共享 IRQ 的任一回调续交普通帧都不得在本 IRQ 确认；窗口覆盖用后续两个边界，第一次写入、第二次确认。
- **停止。**在任务上下文排空 pending、提交 LAST 并等硬件写后首 IRQ 确认，再停输出；该确认依赖帧中断与硬件读取完成的时序关系，不保证全部物理读访存已经结束。
  不用固定帧延时假释放；失败保留未确认帧/资源与 OSD0 claim 可重试。
  DSI HS 必须先于 LP 关屏/睡眠/断电停止，关闭某个图层不等于停 HS。使用 Display 时生命周期只经 Display 调用。
- **一条 DPI。**RGB 屏和 DSI 屏不能同时运行。
- **时序限制。**同步脉宽和前肩最大 255，行总数和帧总行数最大 65535，否则返回 `-EINVAL`。
