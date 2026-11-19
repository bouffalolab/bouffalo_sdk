# display: Display Framework

English | [中文](README_CN.md)

display is a layered display framework. It drives panels with on-chip graphics RAM (GRAM) that are connected over SPI
or the DBI peripheral (MIPI DBI Type B, Type C and QSPI), and video panels (RGB and MIPI DSI) that are scanned out from
frame buffers. Screens are described statically at build time, pixel data of GRAM panels is sent by DMA (optionally
asynchronously), and several panels can share one controller. Video panels are implemented for the BL618DG (MIPI DSI and
RGB pins), whose display processing unit also has a Base layer for video frames (including the YUV formats of a camera)
and two OSD layers ([`scanout/bl618dg_dpu`](scanout/bl618dg_dpu/README.md)). Parallel RGB on the BL616 ("SIM v1")
requires an additional ROM function-table library. A serial RGB backend for the BL616CL is not implemented; see section 2.

Enable the framework with `CONFIG_BSP_DISPLAY=y` (section 3.1). LVGL examples are in
[`examples/display`](../../../examples/display).

## 1. Architecture

```text
application / LVGL port
      |  display_draw()  display_draw_async()  display_flip() ...
+-----v------------------------------------------------------------------+
| Display   one screen                                                   |
+------------------------------------------------------------------------+
| Panel     panel IC driver (commands, initialization, orientation)      |
+------------------------------------------------------------------------+
| IO        connection of one panel to a bus: CS, interface, clock, format |
+------------------------------------------------------------------------+
| Bus       one controller: arbitration, endpoint switching, DMA         |
+------------------------------------------------------------------------+
                     LHAL (bflb_spi / bflb_dbi / bflb_dma / gpio)
```

| Layer | Role |
|---|---|
| Display (`display_t`) | The application entry point. `display_init()` brings the screen up and it is ready to use afterwards; draw (GRAM panels) or flip frame buffers (video panels), rotate and set the backlight; `display_deinit()` switches it off. |
| Panel (`display_panel_t`) | Translates "draw this area" into the commands of the panel IC. A generic driver for ICs that follow the MIPI DCS command set is provided. |
| IO (`display_io_t`) | Describes how one panel is attached to a bus: chip select, interface type, clock and pixel format. |
| Bus (`display_dbi_bus_t`, `display_spi_bus_t`, `display_soft_dbi_c_bus_t`, `display_dsi_host_t`) | One object per physical controller, shared by all panels attached to it. |
| Scanout (`display_scanout_t`) | Video panels only: a pixel engine that continuously reads a frame buffer and sends it to an RGB or MIPI DSI panel, and switches to a new frame buffer at a frame boundary. It takes the place of the draw path of a GRAM panel and does not depend on the panel driver. |

Design principles:

- **Static definition.** The whole topology is declared at build time; the framework does not allocate memory. At run time
  the application only calls `display_init()`, which brings up the layers on demand, so no initialization order has to be
  observed.
- **Standard interfaces.** Interfaces use MIPI DBI terminology: Type B (`DBI_B8`, 8080 parallel), Type C 4-wire
  (`DBI_C4`), Type C 3-wire (`DBI_C3`) and `QSPI`. A bus backend declares the interfaces it can drive, a panel IC declares
  the interfaces it supports together with its standard settings for each (command encoding, pixel format code, default
  clock, QSPI protocol), and each endpoint names the interface its panel is wired to. The three declarations are checked
  against each other during initialization; a mismatch is reported as `-ENOTSUP`.
- **Software chip select.** Panels that share a bus are distinguished by a chip select (CS) GPIO. One DBI peripheral can
  drive panels on different interfaces, for example Type B and Type C; when the active panel changes, the framework calls
  the pin multiplexing functions supplied by the board.
- **Two levels of panel data.** The *IC description* contains what all panels with the same driver IC share: the
  initialization sequence, the supported interfaces and the reset timing. The *module description* contains what one
  particular panel changes: resolution, orientation, GRAM offset, color inversion and gamma.
- **DMA transfers.** Pixel data is sent by DMA whenever the bus supports it. A synchronous draw call simply waits for the
  transfer to finish. The DMA completion interrupt releases CS and the bus before it notifies the application.
- **Backlight.** The backlight is part of the board control of a panel, with a level from 0 (off) to 1000. A GPIO
  backlight is on for any level above 0; a PWM or an I/O expander backlight is implemented by the project. It is off after
  `display_init()`, so that nothing is visible before the first picture is drawn.

## 2. Supported hardware

### 2.1 Panel ICs

| IC | Default modules | Interfaces |
|---|---|---|
| ST7789 | 240x320, 240x240 | B8, C4, C3 |
| ST7796 | 320x480 | B8, C4, C3 |
| ILI9341 | 240x320 | B8, C4, C3 |
| ILI9488 | 320x480 | B8; C4 and C3 with 24-bit pixels only |
| ILI9486 | 320x480 | same as ILI9488 |
| GC9A01 | 240x240 | C4, C3 |
| GC9307, JD9853 | 240x320 | B8, C4, C3 |
| ST7735 | 240x320 (other sizes need their own module) | C4, C3 |
| ST77922 / ST77926 | 320x480 | ST77922: B8, C4, C3; ST77926: QSPI |
| SPD2010 | 412x412 | QSPI |
| NT35510 | 480x800 | B8 |

### 2.2 Video panels

The table lists built-in video panel modules. Available links depend on the chip's scanout backend (section 2.4).

| Panel | Link | Modules |
|---|---|---|
| EK79007 | MIPI DSI, 2 or 4 lanes | 1024x600 |
| AXS15231B | MIPI DSI, 1 lane | 172x640 |
| AXS15231E | MIPI DSI, 1 lane | 258x960 |
| FL7707N | MIPI DSI, 2 lanes | 720x720 |
| ILI9806E | MIPI DSI, 2 lanes | 480x854 |
| ILI9881C | MIPI DSI, 4 lanes (KD050020) or 2 lanes (KD050023W4) | 720x1280 |
| JD9365TX | MIPI DSI, 2 lanes | 720x1280 |
| ST7102 | MIPI DSI, 1 lane (KD027HVF) or 2 lanes (YH494) | 320x320, 480x960 |
| ST7701S | MIPI DSI, 2 lanes; parallel RGB with a 3-wire or 4-wire command port | 480x480 |
| ST7701P | Parallel RGB with a 3-wire or 4-wire command port | 480x480 |
| GC9503V | Parallel RGB (DE mode) with a 3-wire or 4-wire command port | 480x480 |
| JD9165BA | Parallel RGB with a 3-wire or 4-wire command port, or without one | 1024x600 |
| Standard RGB panel (no driver IC) | Parallel RGB | 800x480, 1024x600 (70 Hz) |

Power-up timing, reset and backlight are managed by the board configuration. A board without a panel reset line must
provide the panel's power-up delay.

A standard RGB panel needs no initialization: it shows what the timing delivers, so it is described by its timing and
clock only (`panel/video_standard`). A module for another size is a few lines of timing data
in the application project.

### 2.3 Bus backends and pixel formats

| Backend | Interfaces | Memory pixel formats | Notes |
|---|---|---|---|
| DBI peripheral (`bus/dbi`) | B8, C4, C3, QSPI | RGB565, RGB888 (packed), XRGB8888 | Interfaces can be switched at run time; asynchronous DMA |
| Hardware SPI (`bus/spi`) | C4 | RGB565, XRGB8888 | Asynchronous DMA |
| Soft DBI C (`bus/soft_dbi_c`) | C3, C4 | RGB565, RGB888, XRGB8888 | GPIO bit-banging, synchronous, intended for command ports |
| MIPI DSI host (`bus/dsi`) | DSI low-power commands | none (commands only) | BL618DG (B0). Commands are rejected with `-EBUSY` while the video runs; no read |

Supported chips: BL616, BL616CL, BL618DG and BL702L (the BL702L has the hardware SPI backend only; QSPI only on chips
that provide it; the DSI host and the DPU scanout only on the BL618DG, revision B0). Pin functions of the display
interfaces are provided for the boards `bl616dk`, `bl616cldk`, `bl618dgdk` and `bl702ldk` (section 4.3).

### 2.4 Scanout backends

| Backend | Chip | Link | Memory pixel formats | Requirements |
|---|---|---|---|---|
| BL618DG DPU (`scanout/bl618dg_dpu`) | BL618DG | MIPI DSI, or RGB pins (24, 18 or 16 bit layouts) | Screen layer (OSD0): RGB565, XRGB8888, ARGB8888, AYUV8888 | Revision B0, AP core |
| SIM v1 (`scanout/sim_v1`) | BL616 | Parallel RGB (16 bit, or 8 bit with an external latch) | RGB565 | `DISPLAY_SCANOUT_SIM_V1` is on by default on BL616. An additional library must provide the ROM function-table symbol `__edata_etext_final`. The weak symbol allows linking without the library, but `display_init()` returns `-ENOTSUP` |

The BL618DG has one DPI engine, so an RGB panel and a DSI panel cannot run at the same time. The BL618DG DPU shows the
screen of a `display_t` on its OSD0 layer. Under it lies the Base layer (a constant color, black by default, or a frame
buffer in RGB565, RGB888, XRGB8888, YUYV, UYVY or NV12), over it a second window layer, OSD1. With ARGB8888 or AYUV8888
frames the alpha of every pixel is blended over what is below. These layers, their formats, the window changes and the use
of the DPU without a `display_t` are described in [`scanout/bl618dg_dpu/README.md`](scanout/bl618dg_dpu/README.md);
the Base layer and OSD1 are used through the hardware level object of the scanout (`dpu.hw`), and a `display_t` also
receives `DISPLAY_EVT_VSYNC` from it.

### 2.5 Pixel formats

Between the controller and the panel there are two widths: 16 bit (RGB565) and 24 bit (RGB888; RGB666 panels accept
the same data). The format in memory may differ from the width on the wire:

| Memory format | Bytes per pixel | Width on the wire | Used by |
|---|---|---|---|
| RGB565 | 2 | 16 bit | all |
| RGB888 (packed) | 3 | 24 bit | DBI, soft DBI C; video scanouts (Base layer of the BL618DG) |
| XRGB8888 | 4 (the upper byte is ignored) | 24 bit | all |
| ARGB8888 | 4 (non-premultiplied alpha in the upper byte) | 24 bit | video scanouts |
| AYUV8888 | 4 (`0xAAYYUUVV`, non-premultiplied alpha) | 24 bit | OSD layers of the BL618DG |
| YUYV, UYVY | 2 (width even) | 24 bit | Base layer of the BL618DG |
| NV12 | 1 (Y plane) + interleaved UV plane, 4:2:0 | 24 bit | Base layer of the BL618DG |

The DBI peripheral converts in hardware and accepts any combination. Hardware SPI and soft DBI C fix the width by the format
(RGB565 is sent as 16 bit, the other formats as 24 bit). If a panel IC does not support the required width on an interface,
`display_init()` returns `-ENOTSUP`.

For a video panel the memory format is `scanout.cfg.format`, configured without a frame address. Reconfiguration requires
the layer disabled and no pending operation (NULL → LAST → config); it need not stop the physical output. Frames are
tightly packed (the line length is `width * bytes per pixel`, there is no line pitch). The
output side is fixed by the panel: the DSI data type is part of the panel module, and the number of RGB data lines is a
property of the board wiring. Which formats a layer takes is decided by the scanout (for the BL618DG
`display_bl618dg_dpu_hw_layer_formats()`); a format that a layer cannot show is refused with `-ENOTSUP`.

## 3. Usage

### 3.1 Adding the framework to a project

The framework is a component of the board support package: enable it in the project `defconfig`. Its Kconfig options
(`DISPLAY_*`) appear in menuconfig below the option and its CMake files are part of `bsp/common`, so a project needs no
`add_subdirectory` and no `source` line.

```text
CONFIG_BSP_DISPLAY=y
CONFIG_DISPLAY_BUS_SPI=n          # optional: turn off a backend that is not wanted
CONFIG_DISPLAY_PANEL_ILI9488=n    # optional: do not compile a panel IC that is not used
```

Every backend of the chip is on by default, so enabling `CONFIG_BSP_DISPLAY` is enough; a project only turns off what it
does not want, for example to keep a backend from using pins or DMA channels. Backends are allocated statically and
code that is not referenced is dropped by the linker, so unused backends cost no code space.

The panel ICs are switched in the same way, one option per IC (`DISPLAY_PANEL_<IC>`, all on by default; see section 4.1).
A panel that is switched off is not compiled, so a project that still references one of its modules does not link.

Headers are included by layer, for example `display.h`, `bus/dbi/display_dbi.h` and `panel/gram_st7796/display_st7796.h`.

### 3.2 Default display

If a project uses a single panel and has no special requirements, the screen can be defined entirely in Kconfig. The
application then obtains it with one call.

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

The following can be configured (all options are listed in [`default/Kconfig`](default/Kconfig)):

- **Interface and panel module:** hardware SPI, DBI Type C 4-wire, DBI Type B or QSPI; the module is selected from the
  built-in list; a module is offered only while its panel IC is switched on (`DISPLAY_PANEL_<IC>`).
- **Pixel format in memory:** RGB565, XRGB8888 or packed RGB888 (DBI only).
- **Chip select:** a software GPIO (default), the DBI controller's hardware CS, or none. CS is always a GPIO; it is
  released in the DMA completion interrupt, so a CS provided by an I/O expander is not supported. A panel without software
  CS must be the only panel on its bus, which is always the case for the default display.
- **Reset, power and backlight:** optional GPIOs. They can instead be implemented by the project (for example on an
  I/O expander or a PWM), because they are only operated from task context.
- **Orientation, color order and commands of the panel:** the default display has no Kconfig options for RGB/BGR order,
  mirroring or color inversion. Instead, the display always uses a RAM copy of the selected panel module, and the
  application changes that copy before `display_init()` (see "Adjusting the panel module" below).
- **Custom module:** a project that needs a fully project-defined module supplies its own module description (see
  section 5.1) and selects `DISPLAY_DEFAULT_MODULE_CUSTOM`; the display uses a RAM copy of it as well.
- **Video panel (BL618DG only):** with `CONFIG_DISPLAY_DEFAULT_TYPE_VIDEO` the default display is a video panel: EK79007 on
  MIPI DSI (2 or 4 lanes) or a standard RGB panel (800x480 or 1024x600), the number of wired DSI lanes and their order or
  the RGB pin layout, and the memory format (RGB565, RGB888, XRGB8888 or ARGB8888). See section 3.5.

Application code:

```c
#include "default/display_default.h"

display_t *lcd = display_default_get();
display_init(lcd);                    /* power on, reset, initialization sequence, display on; the backlight is off */
/* draw the first frame, see 3.4 */
display_set_backlight(lcd, DISPLAY_BACKLIGHT_MAX);   /* 0 .. 1000 */
```

**Adjusting the panel module.** The default display does not use the `const` module of the panel directly. On the first
call of `display_default_get()` or `display_default_get_module()`, the selected module is copied into RAM, and the
application can change the copy before `display_init()`:

```c
#include "default/display_default.h"

static const display_dcs_cmd_t my_commands[] = {
    DISPLAY_DCS0(0x21, 0),            /* INVON: invert the colors */
};

display_dcs_module_t *module = display_default_get_module();
module->madctl[0] = 0x48;             /* GRAM panel: orientation and color order, one value per rotation */
module->madctl[1] = 0xE8;
module->madctl[2] = 0x88;
module->madctl[3] = 0x28;
module->invert = true;
module->init_user = my_commands;      /* sent last in the preparation of the panel */
module->init_user_count = DISPLAY_DCS_COUNT(my_commands);

display_t *lcd = display_default_get();
display_init(lcd);
```

The following fields are meant to be changed:

- `madctl[]`: the MADCTL value of each rotation of a GRAM panel (orientation, mirroring and RGB/BGR order). The
  framework sends it again on every rotation change.
- `invert`: color inversion.
- `gap_x[]` and `gap_y[]`: the column and row offset of the visible area in the frame memory of the controller, one
  value per rotation.
- `init_extra` and `init_extra_count`: **replace** the extra commands of the module, which are sent after its
  initialization sequence.
- `init_user` and `init_user_count`: commands of the application that are **appended**. They are sent **last** in the
  preparation of the panel, after the initialization sequence, the `init_extra` of the module, COLMOD and MADCTL (GRAM
  panels) and the inversion of the module itself, so where a command conflicts with one of those, the command of the
  project wins.

A table of commands (`init_extra`, `init_user`) must be `static`: the display keeps the pointer and reads the table
every time the panel is prepared.

`display_default_get_module()` is available for a GRAM default display and for a video default display that is MIPI DSI
(it has a command port). It is not available for a standard RGB panel, which has no command port and no module of this
kind. Call it from the application initialization, from one task, before `display_init()`: the driver reads the module
when the display is initialized, and the copy is made without locking.

A video default display owns no frame memory. Initialize, register a callback, then explicitly submit the first frame
and wait FIRST before backlight; later replacements release the previous frame through RETIRED (section 3.5):

```c
static uint16_t frame[2][DISPLAY_DEFAULT_VIDEO_FRAME_BYTES / 2] ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(64)));

display_t *lcd = display_default_get();
display_init(lcd); /* no frame reads */
display_set_callback(lcd, on_event, ctx); /* ISR callback may submit the next frame; arm before flip */
display_flip(lcd, &(display_fb_t){ .data = frame[0], .format = DISPLAY_FMT_RGB565 });
/* Check return values and wait FIRST, then enable backlight. Never treat a timeout as frame release. */
```

**LVGL.** With `CONFIG_LVGL9_PORT_DISP=y` (or `CONFIG_LVGL8_PORT_DISP=y`) and `CONFIG_DISPLAY_DEFAULT=y`, the LVGL
display port uses the default display; the application only calls `lv_port_disp_init()`. The port is chosen by the
type of the screen:

- **Screen with frame memory (SPI / DBI / QSPI):** partial rendering with one or two draw buffers
  (`LVGL9_DISP_DEFAULT_BUF_COUNT`, `LVGL9_DISP_DEFAULT_BUF_SIZE`; the same with `LVGL8_`), sent by asynchronous DMA
  ([`lv_port_disp_default_gram.c`](../../../components/graphics/lvgl_v9/port/lv_port_disp_default_gram.c)).
- **Video screen (MIPI DSI / RGB):** two whole frames in PSRAM, using full-screen refresh (`full_refresh`) in v8
  and DIRECT mode in v9; the port
  shows a frame with `display_flip()` and gives LVGL the buffer back when the old frame is retired
  ([`lv_port_disp_default_video.c`](../../../components/graphics/lvgl_v9/port/lv_port_disp_default_video.c)). The
  pixel size is defined by `DISPLAY_DEFAULT_BPP` in `default/display_default.h` (bytes per pixel).
  LVGL v9 sets the display format independently of `LV_COLOR_DEPTH`; v8 requires a matching `lv_color_t` size.
  LVGL v8 takes RGB565 and XRGB8888, plus ARGB8888 with
  `LV_COLOR_DEPTH=32` and `LV_COLOR_SCREEN_TRANSP=1`. Initialization submits no frame; the first completed LVGL
  refresh submits the first frame through flush, with FIRST confirming completion. Do not replace the port callback.
- **Rotation:** the LVGL v8 GRAM port supports software rotation with `CONFIG_LVGL8_DISP_DEFAULT_SW_ROTATE=y`;
  set the angle with `lv_disp_set_rotation()`. It does not also rotate the panel in hardware.
  The LVGL v9 GRAM port uses panel hardware rotation. Neither video port supports rotation.

Complete examples: [`examples/display/lvgl_v8`](../../../examples/display/lvgl_v8),
[`lvgl_v9`](../../../examples/display/lvgl_v9), and
[`lvgl_v9_with_osd`](../../../examples/display/lvgl_v9_with_osd) with a video background.

### 3.3 Defining screens manually

For several panels, special wiring or a custom module, the chain *bus → endpoint → panel → display* is declared in a C
file:

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

Another panel on the same bus is added with one more endpoint and panel that refer to the same bus object. Each panel has
its own CS, interface, format and clock.

### 3.4 Using a screen

A screen is ready to use as soon as `display_init()` returns, and `display_deinit()` switches it off:

| Call | Effect |
|---|---|
| `display_init()` | Checks configuration, powers on, resets and initializes the panel; video output is configured without a frame. Failed cleanup retains resources in ERROR for deinit retry. Backlight remains off. |
| `display_set_backlight()` | Sets the backlight, 0 to 1000 (`DISPLAY_BACKLIGHT_MAX`); `-ENOTSUP` on a board without one. |
| `display_deinit()` | Backlight off, display off, sleep in, power off, release. |

Draw the first frame after `display_init()` and switch the backlight on afterwards. A panel IC that should not switch its
display on by itself (for example an AMOLED panel) is flagged in its description; the application then sends its own
command after the first frame.

`display_get_info()` returns the resolution, rotation, pixel format, alignment requirements, the largest single draw size
and whether DMA is used; GUI adapters use it to size their buffers.

Drawing a GRAM panel is done with `display_draw()` (synchronous) or `display_draw_async()`. The asynchronous call returns
immediately; the pixel buffer must not be modified until the `DISPLAY_EVT_DRAW_DONE` event arrives through the callback
registered with `display_set_callback()`. The rotation is changed with `display_set_rotation()`; width and height are
exchanged accordingly. For low-level access, `display_get_panel()` and `display_panel_get_io()` return the lower layers,
and the endpoint offers functions to send commands, read the panel ID and write pixel data.

### 3.5 Video panels (RGB and MIPI DSI)

A video panel is not drawn into: a scanout engine reads a frame buffer continuously. The application owns the frame buffers
(usually in PSRAM, aligned to a cache line on the BL618DG) and tells the display which one to show. A MIPI DSI panel on
the BL618DG is defined like this:

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

The panel module carries the fixed link parameters of the glass (timing, pixel clock, and for DSI the lanes, the lane rate
and the data type); the framework does not plan or cross-check them. For RGB pins, the scanout is defined with `.pins`
(the board wiring: 24, 18 or 16 bit layout) and the board functions that mux the pins instead of `.dsi`, and the panel is a
standard RGB panel, for example `DISPLAY_VIDEO_PANEL(&display_module_rgb_800x480, DISPLAY_BOARD_NONE)`. The
SIM v1 scanout for the BL616 (`DISPLAY_SIM_V1_SCANOUT(...)`, `scanout/sim_v1`) is available only on the BL616 (`DISPLAY_SCANOUT_SIM_V1`, on by default there).

The `pclk_invert`, `hsync_invert`, `vsync_invert` and `de_invert` booleans of `display_video_mode_t` reverse DPI/SRGB
signals. All default to `false`, preserving backend defaults; unsupported flags are ignored. SIM v1 supports HSYNC,
VSYNC and DE inversion and ignores PCLK inversion. BL618DG DPU parallel RGB supports PCLK inversion (falling sample
edge by default, rising when inverted) and ignores the other three flags. DSI links ignore all four flags.

| Call | Effect |
|---|---|
| `display_init()` | Initializes panel/link and configures scanout; reads no frame. Register the callback after READY. |
| `display_flip(fb)` | Non-NULL enables/replaces, matching cfg.format; NULL disables. One pending operation (`-EBUSY`), including pending NULL. Already-disabled NULL is an event-free no-op. |
| `DISPLAY_EVT_FB_FIRST` | First frame active, no previous buffer released. Wait before backlight. |
| `DISPLAY_EVT_FB_RETIRED` | The previous frame is no longer read and may be rewritten. The event does not carry an address: the application keeps track of the frame it replaced. |
| `DISPLAY_EVT_FB_LAST` | Layer disabled, final frame released. |
| `display_deinit()` | Drains pending operations, submits LAST and awaits IRQ confirmation, then stops output/DSI HS before panel-off, sleep and power-off. On failure ERROR retains unconfirmed frames/resources for retry; no timeout or fixed-delay release. |

Observe the following:

- **Explicit first frame**: init reads neither buffer. Register the callback, fill and submit the first frame, wait FIRST,
  then enable backlight. The loop draws into the retired buffer and waits RETIRED after replacement. Each layer has one
  owner that serializes its operations, including task/ISR submissions; no multi-task arbitration is provided.
  Arm notifications before flip because completion may precede return; a timeout never releases a frame.
- **Calling context.** Video flip, queries and callback registration allow ISR use; a completion callback may submit the
  next frame without waiting for another boundary. Budget the call time, cache clean and SIM LLI construction yourself;
  never block waiting for the next frame in an ISR. Initialization/configuration depend on the backend and board hooks.
  IO/Bus, Display init/deinit and the actual
  DPU hw_stop remain task-only because they may wait synchronously; not every API is ISR-safe.
- **MIPI DSI commands and video exclude each other.** While the video runs, commands to the panel return `-EBUSY`; the
  display sends everything it needs before it starts the video.
- **Color order and inversion are those of the initialization sequence.** The framework does not force the color order or
  the inversion of a video panel, and has no module variants for them: there is one module per panel, and its sequence
  decides. A project that needs something else changes the sequence by supplying its own module (a copy with `init` or
  `init_extra`), or, for the default display of a MIPI DSI panel, changes `init_extra` or `init_user` of the RAM copy of
  the module with `display_default_get_module()` (section 3.2).
- **Backlight.** `display_set_backlight()` works as for GRAM panels. The backlight of the BL618DG development board is always
  on and has no control pin, so the call returns `-ENOTSUP` there.
- **Cache.** Scanout core cleans each submitted frame once (both NV12 planes), unless cfg.dcache_dis is set. Long clean
  does not mask IRQs in task context and preserves the caller's IRQ state in an ISR. Only short state/MMIO publication
  masks IRQs. After clean, before publication, both core and DPU recheck pending, state/running and current under a short
  IRQ guard; if an ISR submitted first, the outer call returns `-EBUSY` without overwriting it. The adapter does not clean again.
- **Boundaries.** FIRST/RETIRED/LAST replace VSYNC in that layer's IRQ, not accompany it. Count all four kinds for FPS;
  VSYNC must not wake a completion waiter. DPU retirement, including LAST, is confirmed at the first IRQ after the
  hardware write; this relies on the timing relationship between the frame interrupt and hardware reads, and is not a
  guarantee that all physical reads have completed. Base/OSD0 share an IRQ: a plain frame submitted by a callback cannot be
  confirmed in that same IRQ. A window override uses two subsequent boundaries, first to write, second to confirm.
  IRQ critical sections protect task/ISR state, not multiple calling tasks.
- **Layers of the BL618DG.** The screen is OSD0, claimed from adapter init through successful deinit, even while disabled. The
  Base layer (a constant color or a frame buffer, including YUV frames) and the OSD1 window layer are used through the
  hardware level object `dpu.hw`: layer_config while disabled with no pending operation (format/window/alpha/cache/color_r/color_g), then
  layer_flip(hw,layer,fb,win) with a full display_fb_t or NULL. Base defaults black without AXI frame reads. Claimed OSD0
  rejects public config/flip/alpha/callback mutations with -EBUSY, even while disabled; info stays readable. See
  [`scanout/bl618dg_dpu/README.md`](scanout/bl618dg_dpu/README.md). With ARGB8888 frames the pixel alpha is blended
  over what lies below.
- **Alignment.** Frames are tightly packed. The address must be aligned to a cache line (BL618DG) or 4 bytes (SIM v1), and for
  SIM v1 the size of a frame must be a multiple of 4 bytes.
- **Pin functions for RGB pins.** The board supplies `board_display_dpu_gpio_init()` / `board_display_dpu_gpio_deinit()`
  (section 4.3); `bl618dgdk` provides both functions.
- **Chip and revision.** The BL618DG DSI host and DPU need revision B0; revision A0 and the low-power core are not
  supported and not checked.

### 3.6 Points to observe

- **Chip select.** CS is a software GPIO unless stated otherwise; the board pin functions never route a CS pin to the
  controller. A panel defined without CS is always selected and must be the only panel on its bus; otherwise attaching it
  returns `-EBUSY`.
- **Interface compatibility.** The interface of an endpoint must be supported by the bus, by the panel IC and by the module.
  Otherwise `display_init()` returns `-ENOTSUP`. QSPI protocol details come from the IC description.
- **Buffers.** Pixel buffers must be aligned to `info.buf_align` (4 bytes for DBI and SPI), and draw areas to
  `x_align` / `y_align`. A single draw cannot be larger than `max_draw_bytes`, which depends on the number of DMA list nodes
  (`LLI_COUNT`): about 16 KB per node on the DBI peripheral and about 8 KB per node on SPI with RGB565.
- **Callbacks.** GRAM DRAW_DONE runs in the DMA completion interrupt (in the calling task if the bus has no DMA);
  keep it notification-only and do not resubmit IO/Bus transfers from an ISR. Video callbacks run in the frame interrupt
  and may query state, set the video callback or submit the next frame after completion, subject to section 3.5; never block.
- **Concurrency.** Life-cycle calls and rotation changes of one screen must be serialized by the caller. Draws to
  different screens on the same bus are arbitrated by the bus and may come from different tasks. If another device, such
  as a touch controller or a flash, shares the bus, it must be accessed between `display_bus_acquire()` and
  `display_bus_release()`.
- **Fields left out of a static definition.** Omitted fields are zero and have defined meanings: no software CS, default
  clock, RGB565, and a wire width derived from the format. An omitted interface is rejected. A DMA channel without a list
  of DMA nodes, or a DBI bus without an interface list, returns `-EINVAL`.

### 3.7 Simple memory text rendering

The optional [`simple_font`](simple_font/README.md) synchronously renders ASCII strings into RGB565, RGB888 or
XRGB8888 memory. A read-only configuration selects image dimensions, stride, font and ARGB foreground/background
colors; each call supplies only the buffer base address, coordinates and text. Enabling `DISPLAY_SIMPLE_FONT`
includes the 8×16 font by default; other fonts are individually selectable. It neither submits pixels nor manages cache.

## 4. Configuration

### 4.1 Kconfig

There is one `Kconfig` and one `CMakeLists.txt` per layer/tool: `bus/`, `panel/`, `scanout/`, `default/` and the optional
`simple_font/`. The top-level [`Kconfig`](Kconfig) includes them and `DISPLAY_BUS_LOCK_TIMEOUT_MS`, and `bsp/Kconfig`
sources it below `BSP_DISPLAY`.

| Option | Meaning |
|---|---|
| `BSP_DISPLAY` | Enables the framework (in `bsp/Kconfig`; BL616, BL616CL, BL618DG, BL702L). All backends of the chip are then on by default; a project turns off the ones it does not want. Unused backends cost no code space, because backends are statically allocated and the linker drops unreferenced code. |
| `DISPLAY_PANEL_<IC>` | One option per panel IC, all on by default (`ST7789`, `ST7796`, `ILI9341`, `ILI9488`, `GC9A01`, `GC9307`, `ILI9486`, `JD9853`, `ST7735`, `ST77922` (also ST77926), `SPD2010`, `NT35510`, `ST7701S`, `ST7701P`, `GC9503V`, `JD9165BA`, `EK79007`, `AXS15231B`, `AXS15231E`, `FL7707N`, `ILI9806E`, `ILI9881C`, `JD9365TX`, `ST7102`). A panel that is switched off is not compiled, so a project that references one of its modules does not link. The generic DCS driver and `panel/video_standard` are always built. The module choices of the default display depend on their panel option. |
| `DISPLAY_BUS_SPI`, `_SOFT_DBI_C` | Bus backends; on by default. |
| `DISPLAY_BUS_DBI` | DBI peripheral bus backend; on by default on the BL616, BL616CL and BL618DG. |
| `DISPLAY_BUS_DSI` | MIPI DSI host; on by default, and selectable, on the BL618DG only. |
| `DISPLAY_SCANOUT_BL618DG_DPU` | BL618DG DPU scanout backend (selects `DISPLAY_BUS_DSI`); on by default, and selectable, on the BL618DG only. |
| `DISPLAY_SCANOUT_SIM_V1` | BL616 SIM v1 scanout backend; on by default, and selectable, on the BL616 only. It needs the library that provides the ROM function table symbol `__edata_etext_final`. The symbol is weak, so the link succeeds without the library, but then `display_init()` of an RGB panel on this scanout returns `-ENOTSUP`. |
| `DISPLAY_DBI_CLK_MODE`, `DISPLAY_SPI_MODE` | Clock polarity and phase of the backend, common to all its endpoints (soft DBI C is fixed to mode 0). |
| `DISPLAY_BUS_LOCK_TIMEOUT_MS` | Maximum time to wait for the bus. |
| `DISPLAY_SIMPLE_FONT`, `DISPLAY_SIMPLE_FONT_*` | Optional memory-only ASCII renderer and individual fonts; see [simple_font](simple_font/README.md). |
| `DISPLAY_DEFAULT`, `DISPLAY_DEFAULT_*` | The default display (sections 3.2 and 3.5). |
| `LVGL8_PORT_DISP`, `LVGL9_PORT_DISP`, `LVGL*_DISP_DEFAULT_*` | The LVGL display ports of the default display; options in `components/graphics/lvgl_v8/port/Kconfig` and `lvgl_v9/port/Kconfig` (section 3.2). |

### 4.2 Clock

The clock of an endpoint is taken from, in this order: the endpoint setting, the default of the panel IC for the interface,
and the default of the interface (27 MHz for Type B, 40 MHz for the others). The result is limited to what the controller can
generate. The soft DBI C clock is not configurable.

### 4.3 Board pin functions

The bus backends do not know pin numbers. For every interface the board provides a pair of functions that multiplex and
release the pins, for example `board_display_dbi_b8_gpio_init()` / `_deinit()`, `board_display_spi_gpio_init()`,
`board_display_qspi_gpio_init()` and `board_display_dpu_gpio_init()` for the RGB pins of the BL618DG. The contract is
[`default/display_default_gpio_ext.h`](default/display_default_gpio_ext.h); the functions of a board are in
`bsp/board/<board>/board_gpio.c` (or `board_gpio_overlay.c` for an overlay board), declared in `board_gpio.h`,
and compiled with `CONFIG_BSP_DISPLAY`. A chip that supports an interface
has its functions in the board directory (a missing one is a link error; a function that is not decided yet is an empty
one with a `TODO`), and an interface the chip does not have is hidden in Kconfig. The tables of `bl616dk`, `bl616cldk`,
`bl618dgdk` and `bl702ldk` come from the pinmux tables of the chips; `bl616cl_ave_v10` only has empty functions. For a new
board, use [`bl616dk`](../../board/bl616dk/board_gpio.c) as a reference.

The functions of the data interface never route a CS pin; separate `..._cs_gpio_init()` functions do that for panels whose CS is
driven by the DBI hardware. A project that defines its own displays gives its own functions to the static definition of
its buses (`DISPLAY_DBI_IFACES()`, the `func_init` and `func_deinit` fields of SPI) and does not need these. The SIM v1 backend
takes its pin numbers in its configuration.

## 5. Adding a screen

### 5.1 A new panel module for an existing IC

Panels with the same driver IC usually differ in resolution, GRAM offset, orientation, color inversion and gamma. Such a
panel is added by writing a module description **in the application project**; the framework itself does not change.
A template is provided in [`panel/template/display_module_template.c`](panel/template/display_module_template.c).

A module description specifies:

| Item | Meaning |
|---|---|
| IC | The IC description to reuse (initialization sequence and interfaces). |
| Size | Visible width and height at 0 degrees. |
| Rotations | The supported rotations and the memory access control (MADCTL) value for each, including RGB/BGR order and mirroring. |
| GRAM offset | The position of the glass within GRAM, for panels smaller than the IC's GRAM. |
| Inversion | Whether color inversion is enabled. |
| Additional commands | Commands appended after the IC's sequence (for example gamma or voltages), or a complete replacement sequence supplied by the panel vendor. |
| Interfaces | Optionally, the interfaces the module is actually wired for. |

The orientation table is validated during initialization: rotations of 90 and 270 degrees must exchange rows and columns,
and the resolution plus offset must fit into GRAM; otherwise `display_init()` returns `-EINVAL`. The module is then
used in `DISPLAY_DCS_PANEL()`, or selected as the custom module of the default display.

`display_dcs_module_adjust()` is a generic function of the DCS driver that copies a module and changes the RGB/BGR bit,
the mirroring and the inversion of a GRAM module. The default display does not use it; call it from application code if
it is useful there.

A video panel has one module per panel. Its color order and inversion are whatever its initialization sequence says; there
is no `_bgr` variant and no option to force them.

### 5.2 A new driver IC

An IC that follows the standard MIPI DCS command set needs data only, no code. Create `display_<ic>.h` and
`display_<ic>.c` in `panel/gram_<ic>/`, following [`panel/gram_st7796`](panel/gram_st7796). They contain:

- the vendor initialization sequence, in which every entry is a command, a delay and its parameters;
- the supported interfaces, each with its standard settings: the pixel format code for 16-bit and 24-bit data, the default
  clock, the command encoding and, for QSPI, the protocol;
- the IC description (GRAM size, window alignment, reset timing) and at least one default module.

Rules for the initialization sequence:

- Do not include display on/off (`0x29`/`0x28`). For GRAM panels, also leave out memory access control (`0x36`), pixel format
  (`0x3A`) and the inversion command; the driver sends them after the sequence according to the rotation, the interface and
  the module.
- Include the software reset and sleep out (`0x11`) with their delays.
- Settings that only make sense together with inversion are given separately in the IC description.

Then add the source file to [`panel/CMakeLists.txt`](panel/CMakeLists.txt) under a new `DISPLAY_PANEL_<IC>` option in
[`panel/Kconfig`](panel/Kconfig) (default y) and, to make the module selectable in the default display, add an option to
[`default/Kconfig`](default/Kconfig) that `depends on` that panel option, and the matching case to
`default/display_default_gram.c`.

Naming: the IC description is `display_ic_<ic>`, the built-in module `display_module_<ic>_<width>x<height>`. Modules defined
in an application project must not use the `display_module_` prefix.

A video panel IC (RGB or MIPI DSI) is described the same way in `panel/video_<ic>/`. The module additionally carries the
video link (`display_video_mode_t`: link kind, timing, clock, and the lanes, rate and data type for DSI) and the wait
between the last command and the first pixel. An IC with both an RGB and a DSI input is one IC description; the interface of
the command endpoint selects the video link, and a DSI-specific initialization sequence is given per interface. An IC
that takes no display on/off commands (EK79007) is flagged accordingly. Use a built-in panel module as a reference, for
example [`panel/video_ek79007`](panel/video_ek79007).

### 5.3 ICs with a non-standard protocol

Implement a panel driver instead: provide the panel operations (initialize, power, enable, rotate, draw), make the panel
core object the first member of the driver structure, and send commands through the IO layer. The generic DCS driver in
[`panel/dcs`](panel/dcs) is a reference implementation.

## 6. Directory layout

```text
display.h / display.c        Display: the application entry point
display_types.h              Common types: pixel formats, pins, areas, command interfaces, video links, events
bus/                         Bus core and endpoints; backends spi/, dbi/, soft_dbi_c/, dsi/ (MIPI DSI host, BL618DG); each layer has its own Kconfig and CMakeLists.txt
panel/                       Panel drivers: dcs/ (generic driver), gram_<ic>/ (GRAM panels: IC description and modules), video_<ic>/ (RGB / MIPI DSI panels), video_standard/ (standard RGB panels without driver IC), template/; Kconfig has one DISPLAY_PANEL_<IC> option per IC
scanout/                     Scanout core and backends for video panels: bl618dg_dpu/ (RGB pins and MIPI DSI; has its own README), sim_v1/ (BL616; requires an additional ROM function-table library)
default/                     The default display: Kconfig and implementation (GRAM panels and video panels), and the contract of the board pin functions
```

Each layer can be used independently. Include the header for the operations you need:

| Purpose | Header |
| --- | --- |
| Use an existing bus: borrow the controller or query statistics | `bus/display_bus.h` |
| Use IO: configure an endpoint, send commands and pixels | `bus/display_io.h` |
| Use an existing scanout: configure, submit frames, query status | `scanout/display_scanout.h` |
| Define a concrete bus or scanout object | The backend header, e.g. `bus/spi/display_spi.h` or `scanout/bl618dg_dpu/display_bl618dg_dpu_scanout.h` |
| Control DPU layers directly | `scanout/bl618dg_dpu/display_bl618dg_dpu_hw.h` |
| Implement a bus or scanout backend | `bus/display_bus_driver.h` or `scanout/display_scanout_driver.h` |

Concrete backend headers include the generic APIs and complete base definitions needed to define objects statically
with the existing initializer macros. The `*_driver.h` headers collect operation tables, base objects and implementation
helpers. Using a lower layer does not require a `display_t`. Once a panel or scanout belongs to a Display, its lifecycle
is managed by that Display.

The pin functions of the boards are in `bsp/board/<board>/board_gpio.c` (or `board_gpio_overlay.c` for an overlay board).

This directory contains the display framework. Application examples are in [`examples/display`](../../../examples/display).

## 7. Current limitations

Video panels:

- SIM v1 (BL616, parallel RGB) needs an additional library that provides the ROM function-table symbol `__edata_etext_final`.
  The symbol is weak, so a BL616 build with `DISPLAY_SCANOUT_SIM_V1` (on by default there) still links, but
  `display_init()` of an RGB panel on this scanout returns `-ENOTSUP` without the library.
  A serial RGB backend for the BL616CL is not implemented.
- The DPU of the BL618DG: YUYV and NV12 frames go to the Base layer only (the OSD layers have no such formats); the
  window of an OSD layer cannot be resized without a frame of that size (`display_bl618dg_dpu_hw_layer_flip(hw,layer,fb,win)`);
  rotation is not supported on video panels.
- The DSI host supports 40 MHz crystals only and the lane rates 400, 500, 550, 650, 750 and 850 Mbit/s, one panel per
  host, no reads and no command-mode panels. A communication error leaves the host in a failed state (every call returns
  `-EIO`) because the controller cannot be reset on its own. The panel must not be sent commands while the video runs.
- Signal inversion takes effect only where supported by the backend (section 3.5); unsupported flags are ignored. The DPU
  pixel clock is mapped to the nearest clock the chip can generate (for example 34.29 MHz for the 33.39 MHz of the standard
  800x480 module).
- A DSI link pixel clock of 60 MHz and above adds 4 MHz to the line buffer calculation.
- The color order and the inversion of a video panel cannot be forced by the framework; they are those of its
  initialization sequence. For the default display of a MIPI DSI panel, the application can change the commands of the RAM
  copy of the module (`display_default_get_module()`, section 3.2). A standard RGB panel has no command port, so
  `display_default_get_module()` is not available for it.
- The standard serial RGB module has timing data only; no scanout backend drives a serial RGB link.
- Revision A0 of the BL618DG and its low-power core are not supported. The backlight of the BL618DG development board is
  always on.
- There is no event for a hardware error of a scanout; a failure while running is reported by the call that stops it.

General:

- An LVGL port for several screens (the LVGL v8 and v9 ports of the default display exist, for GRAM and for video
  panels);
- Board pin assignments must match the actual wiring, especially DBI hardware CS. The display pin functions of
  `bl616cl_ave_v10` are empty and must be implemented by the board.
