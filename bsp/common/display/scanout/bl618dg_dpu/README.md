# BL618DG DPU

[中文](README_CN.md)

The display processing unit (DPU) of the BL618DG reads frames from memory, blends up to three layers and sends the
result to a video panel, over **MIPI DSI** or **RGB pins**. This directory has its driver: the part that connects it
to the display framework, and the hardware level that can also be used alone.

## Features

- Three layers, bottom to top:

  | Layer | What it shows |
  |---|---|
  | `DISPLAY_BL618DG_DPU_LAYER_BASE` | The whole screen: a constant color (default black, no memory access), or a frame buffer: RGB565, RGB888, XRGB8888, or the YUV formats of a camera: YUYV, UYVY, NV12 |
  | `DISPLAY_BL618DG_DPU_LAYER_OSD0` | A window (the whole screen by default) of RGB565, XRGB8888, ARGB8888 or AYUV8888 frames. This is the screen layer of a `display_t` |
  | `DISPLAY_BL618DG_DPU_LAYER_OSD1` | Like OSD0, over it |

- YUV frames (YUYV, UYVY, NV12) go to the Base layer only; the DPI converts them to RGB itself, so a camera frame can be shown
  without a conversion in software. The range of the YUV data is full (0..255) or limited (16..235, what most cameras
  give), set in the configuration. The OSD layers take RGB565, XRGB8888, ARGB8888 and AYUV8888 (`0xAAYYUUVV`).
- OSD windows have any size and position inside the screen. Each OSD layer blends over the layers below with a
  constant alpha, or with the alpha of every pixel (ARGB8888, not premultiplied).
- Frames are explicitly submitted after configuration; callbacks distinguish FIRST, RETIRED, LAST and ordinary VSYNC.
  The first IRQ after a hardware write is the implementation's retirement assumption, not proof of physical read completion.
- The window of an OSD layer can change while it runs: its frame IRQ writes the new window/address, and the next IRQ
  confirms them (two boundaries). Hardware-atomic latching is not guaranteed.
- `display_bl618dg_dpu_hw_layer_get_info()` returns current/pending Y and UV **address pointers**, plus running,
  format, window and pending_op. Only internal layer state stores complete display_fb_t objects.
- RGB pins (24, 18 or 16 bit layouts) or MIPI DSI (HS video; the panel commands go over the LP side of the same DSI host).
- The hardware level works without `display_t`: use the DPU with a panel driver of your own, or with none.

## Files

| File | Role |
|---|---|
| `display_bl618dg_dpu_hw.[ch]` | The hardware level: the output and the three layers. The only code here that calls the DPU drivers (LHAL). Does not depend on `display_t`. The header's adapter-only section declares `display_bl618dg_dpu_osd0_{claim,release,config,flip}` for the adapter to manage OSD0 ownership and submissions |
| `display_bl618dg_dpu_scanout.[ch]` | The adapter that connects the scanout core of the display framework to the OSD0 layer of the hardware level |
| `display_bl618dg_dpu_clock.[ch]` | BL618DG DPU display clock and DSI PLL steps, shared with the DSI host |

Enable it with `CONFIG_DISPLAY_SCANOUT_BL618DG_DPU` (on by default on BL618DG, selects the DSI bus). The default
display (`CONFIG_BSP_DISPLAY` with a video panel) uses it for you.

Include `display_bl618dg_dpu_hw.h` to use the hardware level directly, or `display_bl618dg_dpu_scanout.h` to define a
scanout object. Defining a MIPI DSI host additionally requires `bus/dsi/display_dsi.h`.

## Use with `display_t`

The screen is OSD0. Define only its format, compose it with the panel, initialize, register a callback and explicitly
submit the first frame. Init reads no frame. Capture/arm event notifications before flip: completion can precede return.
For the definitions of `panel` and `dsi0`, see [section 3.5 of the framework guide](../../README.md#35-video-panels-rgb-and-mipi-dsi).

```c
#include "display.h"
#include "scanout/bl618dg_dpu/display_bl618dg_dpu_scanout.h"

static uint16_t frame[2][1024 * 600] ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(64)));

static display_bl618dg_dpu_scanout_t dpu = DISPLAY_BL618DG_DPU_SCANOUT(DISPLAY_FMT_RGB565, .dsi = &dsi0);
static display_t lcd = DISPLAY_FB(&panel, &dpu);

display_init(&lcd);                                /* DSI link, panel, configuration; no frame reads */
display_set_callback(&lcd, on_display_event, ctx); /* ISR completion callback may submit the next frame */
display_flip(&lcd, &(display_fb_t){ .data = frame[0], .format = DISPLAY_FMT_RGB565 });
/* Wait FIRST before backlight. FIRST releases no previous buffer. Check all return values. */
display_flip(&lcd, &(display_fb_t){ .data = frame[1], .format = DISPLAY_FMT_RGB565 });
/* DISPLAY_EVT_FB_RETIRED: the old frame may be rewritten */
display_flip(&lcd, NULL); /* Wait LAST before reusing the final frame. */
display_deinit(&lcd);     /* int: failure retains frames/resources for retry */
```

For RGB pins leave `.dsi` out and give `.pins` (the layout), `.pins_init` and `.pins_deinit` (board functions that mux
the pins).

The Base layer and OSD1 are used through the hardware level object inside the scanout, `dpu.hw`:

```c
display_bl618dg_dpu_layer_cfg_t base = { .format = DISPLAY_FMT_RGB565, .color_r = 255 };
display_bl618dg_dpu_hw_layer_config(&dpu.hw, DISPLAY_BL618DG_DPU_LAYER_BASE, &base); /* disabled Base: red, no AXI */

display_bl618dg_dpu_layer_cfg_t win = { .format = DISPLAY_FMT_ARGB8888, .window = { 100, 80, 320, 200 } };
display_bl618dg_dpu_hw_layer_set_callback(&dpu.hw, DISPLAY_BL618DG_DPU_LAYER_OSD1, on_osd1, NULL);
display_bl618dg_dpu_hw_layer_config(&dpu.hw, DISPLAY_BL618DG_DPU_LAYER_OSD1, &win);
display_fb_t overlay = { .data = win_frame[0], .format = DISPLAY_FMT_ARGB8888 };
display_bl618dg_dpu_hw_layer_flip(&dpu.hw, DISPLAY_BL618DG_DPU_LAYER_OSD1, &overlay, NULL); /* wait FIRST */
display_bl618dg_dpu_hw_layer_flip(&dpu.hw, DISPLAY_BL618DG_DPU_LAYER_OSD1, NULL, NULL); /* wait LAST */
```

Base defaults to black without AXI frame reads. Configure its format/color only while disabled and no operation pending.
Use the same nullable layer_flip for Base. To change format: NULL → LAST → config → fb → FIRST, without stopping output
or disturbing OSD0/1. All snippets omit task waits/error checks for brevity; a timeout must never be treated as release.

For a camera, give the format of its frames instead: `DISPLAY_FMT_YUYV` (bytes Y0 U Y1 V), `DISPLAY_FMT_UYVY` (U Y0 V Y1)
or `DISPLAY_FMT_NV12` (a Y plane, then an interleaved UV plane with U first, 4:2:0). NV12 also names the UV plane, and
its frames are switched with both planes:

```c
display_bl618dg_dpu_layer_cfg_t nv12_cfg = { .format = DISPLAY_FMT_NV12 };
display_bl618dg_dpu_hw_layer_config(&dpu.hw, DISPLAY_BL618DG_DPU_LAYER_BASE, &nv12_cfg);
display_fb_t nv12 = { .data = y_plane[0], .uv = uv_plane[0], .format = DISPLAY_FMT_NV12 };
display_bl618dg_dpu_hw_layer_flip(&dpu.hw, DISPLAY_BL618DG_DPU_LAYER_BASE, &nv12, NULL);
```

A window that changes (OSD layers; not OSD0 while a `display_t` uses it, which has the size of the screen):

```c
display_bl618dg_dpu_window_t win = { .x = 400, .y = 100, .width = 500, .height = 300 };
display_fb_t bigger = { .data = big_frame, .format = DISPLAY_FMT_ARGB8888 };
display_bl618dg_dpu_hw_layer_flip(&dpu.hw, DISPLAY_BL618DG_DPU_LAYER_OSD1, &bigger, &win);
```

The range of YUV frames is chosen with `.yuv_range = DISPLAY_BL618DG_DPU_YUV_LIMITED` in the configuration (default
`DISPLAY_BL618DG_DPU_YUV_FULL`).

The callback is `void cb(display_event_type_t type, void *ctx)`, with no address. FIRST confirms activation without
release, RETIRED releases the previous frame, LAST releases the final frame; ordinary boundaries report VSYNC instead.
Exactly one kind per layer per IRQ; FPS counts all four. VSYNC must not wake a completion waiter. Already-disabled
NULL is an event-free no-op; pending LAST is distinguished by pending_op even though pending is NULL.

## Use without `display_t`

Bring up the panel yourself (for a DSI panel by its LP commands, with the panel driver), then:

```c
static display_bl618dg_dpu_hw_t hw = { .cfg = { .dsi = &dsi0 } };

display_bl618dg_dpu_hw_open(&hw);
display_bl618dg_dpu_hw_layer_set_callback(&hw, DISPLAY_BL618DG_DPU_LAYER_BASE, on_base, NULL);
display_bl618dg_dpu_hw_start(&hw, info.video_mode); /* honor video_start_delay_ms before this; black Base, OSDs off */
display_bl618dg_dpu_hw_layer_config(&hw, DISPLAY_BL618DG_DPU_LAYER_OSD1, &win);
display_bl618dg_dpu_hw_layer_flip(&hw, DISPLAY_BL618DG_DPU_LAYER_OSD1, &overlay, NULL);
/* ... flips ... */
display_bl618dg_dpu_hw_stop(&hw);                                /* stops OSD1, OSD0, the output; then the panel */
```

## Things to know

- **One serialized owner per layer, including task/ISR operations.** No multi-task arbitration or whole-DPU owner.
  The adapter claims OSD0 even while disabled; public config, flip, alpha
  and callback mutations return `-EBUSY`, while get_info remains allowed. Never bypass the claim with LHAL/internal calls.
  Base and OSD1 stay independent. Running output accepts only field-wise identical mode.
- **Frames.** Lines are tightly packed (`width * bytes per pixel`, no padding), and the address is cache-line aligned.
  Accepted memory is the hardware's until RETIRED releases the previous frame, LAST releases the final frame, or deinit
  succeeds. FIRST releases no previous frame. Fill each buffer before submission; a timeout never releases it.
- **Data cache.** One clean per submission, both NV12 planes: scanout core for Display, hardware layer for direct use.
  Use cfg.dcache_dis if the caller manages cache. Long clean does not mask IRQs in task context; in an ISR it preserves
  the caller's IRQ state. Short state/MMIO publication masks IRQs. After clean and before publication, core and DPU
  recheck pending, state/running and current under a short IRQ guard; if an ISR submitted first, the outer call returns
  `-EBUSY` without overwriting it. The adapter does not clean again; these guards do not serialize multiple tasks.
- **Flip.** `layer_flip()`, queries and video callback registration allow task or ISR context. A second flip before the first one
  is reported returns `-EBUSY`. There is no atomic flip of several layers: each layer switches at its own frame boundary.
- **Report callbacks run in the frame interrupt.** State is updated before notification, so a completion callback may
  submit the next frame. Budget call time/cache clean yourself; never block waiting for the next frame.
  ISR use of scanout and DPU open/start/layer access depends on the backend and board hooks.
  IO/Bus, Display init/deinit and an actual DPU hw_stop remain task-only (synchronous waits).
- **Base.** Constant color is R/G only, B zero. Set color_r/color_g and format with layer_config while disabled.
  NULL disables AXI reads, LAST releases the final frame. Base and OSD0 share the OSD0 SEOF boundary.
- **Formats per layer.** Each layer has its own set of frame formats, `display_bl618dg_dpu_hw_layer_formats(layer)` returns it
  as a mask. A format outside it is refused with `-ENOTSUP` when the layer is configured, for
  example YUYV for OSD1 or ARGB8888 for Base. The scanout of `display_t` offers the set of OSD0.
- **Window changes.** `layer_flip(fb, win)` is non-blocking, one at a time (`-EBUSY` while a frame or
  a window is pending), and the callback reports when the frame before it can be rewritten. The new window and frame are
  written by the frame interrupt of the layer, not by the call. A plain `layer_flip()` after it has the size of the new
  window. `display_bl618dg_dpu_hw_layer_get_info()` returns the latest programmed window, which may still be pending.
  ISR writes the window/address, and the next IRQ completes: two subsequent boundaries total. Use FIRST/RETIRED or
  `pending_op` to establish completion, not the window fields alone.
- **YUV frames.** The format is configurable while that layer is disabled/no pending. The width must be even (two pixels share a U / V pair), and
  for NV12 the height too. The conversion to RGB is done by the DPI and assumes full-range BT.601 (Y, U and V use 0..255). For NV12 the Y plane and the UV plane (`width * height / 2` bytes, lines of `width` bytes) are
  both tightly packed and cache-line aligned, transferred and released together. Callbacks carry no address. YUYV and NV12 frames
  cannot be given to the OSD layers; they can have AYUV8888 (`0xAAYYUUVV`, the alpha of the pixels works as in ARGB8888).
  `yuv_range` limited changes the conversions of the whole display path, so RGB frames and the OSD layers keep their
  colors; it is output-wide and changes only across a successful output stop/restart.
- **OSD layers.** Formats RGB565, XRGB8888, ARGB8888 (RGB565 and XRGB8888 are opaque unless a constant alpha is
  set; ARGB8888 uses the alpha of its pixels). cfg.alpha zero selects pixel alpha where available, otherwise opaque;
  1..255 selects constant alpha. Use fb + win to resize while running.
  The z-order is fixed: Base < OSD0 < OSD1.
- **Frame interrupt.** Base/OSD0 share OSD0 SEOF. OSD1 has its own SEOF and can stop IRQs after LAST; disabled OSD1
  need not produce VSYNC. A plain frame submitted by either shared-IRQ callback must not be confirmed in that same IRQ;
  a window override uses two subsequent boundaries, first to write and second to confirm.
- **Stop.** In task context, drain pending operations, submit LAST, await the first IRQ after its hardware write, then stop output;
  this confirmation relies on the timing relationship between the frame interrupt and hardware reads, and does not
  guarantee that all physical reads have completed. No fixed-frame-delay release.
  Failure preserves unconfirmed frames/resources and the OSD0 claim for retry. With DSI, stop HS before LP
  display-off/sleep/power-off; disabling a layer alone does not stop HS.
- **One DPI.** An RGB panel and a DSI panel cannot run at the same time.
- **Timing limits.** Sync pulse and front porch are at most 255, line and frame totals at most 65535 (`-EINVAL`
  otherwise).
