# DBI Type B LCD

[English](README.md) | [简体中文](README_CN.md)

## Features

Uses the default GRAM display in `bsp/common/display`: ST7796, 320×480, DBI Type B (8080 x8), RGB565.
The panel model does not change with the chip; adapt the wiring to the actual board.

- A fixed 100×100 multicolor gradient is centered on a black screen at `((width - 100) / 2, (height - 100) / 2)`.
- “DBI Display Test” is arranged as three centered lines: `DBI`, `Display`, `Test`, using `display_simple_font_draw()` and the built-in 8×16 ASCII font.
	Each line is horizontally centered; top coordinates are y=22, 42, 62 within the image, with 4 px gaps and 56 px total text height.
	Foreground is opaque white ARGB `0xffffffff`; background `0x00000000` is transparent and preserves the gradient.
- Six animated rectangular rings are 2 px wide with 3 px gaps, at insets 0, 5, 10, 15, 20, 25 px from the screen edge.
	From inner to outer, `depth=0..5`: hue is `phase + 80 * depth`, brightness is `255 - 28 * depth`; hue wraps at 1536.
- Initialization clears the screen, renders and uploads the center image once, draws the initial rings, then enables the backlight.
	The loop advances phase by 6, updates only the rings synchronously, then delays 30 ms; this is not a fixed 30 ms frame period.
	The center image remains unchanged in panel GRAM and is not resubmitted in the loop.

## Buffers and transfers

- Static pixel buffers: 2000 pixels for fills + 10000 pixels for the center = 24000 B in RGB565 or 48000 B in XRGB8888.
	Both buffers are 64-byte aligned; no full-screen framebuffer is required.
- Clears and rings use `display_draw()`; uniform fills are tiled in both axes to fit the fill buffer and `max_draw_bytes`.
- The center is uploaded once as full-width row stripes using `display_draw_async()`, sized by `max_draw_bytes` and buffer alignment.
	Every stripe waits for its completion event and checks the result before the next submission; one upload may require multiple submissions.
- Requires a GRAM/B8 display of at least 160×160, area alignment 1×1, and compatible buffer alignment.
	`max_draw_bytes` must fit at least one 100-pixel row: 200 B for RGB565 or 400 B for XRGB8888; unsupported conditions stop drawing with an error.

## Wiring

Set the panel to 8080 x8 mode. Connect D0–D7, WR, RD, D/C, CS and common GND; follow panel and board specifications for power and IO voltages.
Default wiring targets `bl616dk`:

| Signal | GPIO |
| --- | --- |
| D0–D7 (in order) | 8–15 |
| WR, RD, D/C control group | 4, 6, 7; map individually using the board schematic or connector labels |
| CS | GPIO5, software-controlled |
| Backlight | GPIO17, active high |

The GPIO group order is not the signal order. For other boards, adapt CS, backlight and board-level DBI pins to the actual wiring; these pins are not universal defaults.

### Reset

By default, `CONFIG_DISPLAY_DEFAULT_RESET_USED=n` selects `DISPLAY_PIN_NONE`; the application drives no reset GPIO.
**Do not connect panel RESET to GPIO12**: it is a DBI data pin in the default wiring.
Keep RESET deasserted through the module's own pull-up/reset circuit; never leave it floating. The panel initialization includes software reset.

For hardware reset, enable `DISPLAY_DEFAULT_RESET_USED` in Kconfig and set `DISPLAY_DEFAULT_RESET_PIN` to a separately wired, free GPIO (active low).
It must not conflict with DBI, CS, backlight, UART or other board peripherals; this example assumes no additional reset wiring.

### Backlight

The default backlight control is GPIO17, active high.
Adjust `DISPLAY_DEFAULT_BACKLIGHT_MODE`, `DISPLAY_DEFAULT_BACKLIGHT_PIN` and `DISPLAY_DEFAULT_BACKLIGHT_ACTIVE_HIGH` as needed.
Backlight is enabled after the initial center image and rings have been drawn.

The backlight pin must not conflict with DBI data or control signals. Select `DISPLAY_DEFAULT_BACKLIGHT_NONE` if the module has an always-on backlight circuit.
Do not connect a backlight load directly to a GPIO without sufficient drive capability.

## Configuration

The project uses `defconfig` and SDK Kconfig:

| Configuration | Default / purpose |
| --- | --- |
| `BSP_DISPLAY`, `DISPLAY_DEFAULT` | Enable the display framework and default display |
| `DISPLAY_SIMPLE_FONT`, `DISPLAY_SIMPLE_FONT_ASCII_16X8` | Memory text rendering and 8×16 ASCII font |
| `DISPLAY_DEFAULT_TYPE_GRAM`, `DISPLAY_DEFAULT_IF_DBI_B8` | GRAM panel, DBI Type B x8 |
| `DISPLAY_DEFAULT_MODULE_ST7796_320X480` | ST7796, 320×480 |
| `DISPLAY_DEFAULT_FORMAT_RGB565` | Default format; `DISPLAY_DEFAULT_FORMAT_XRGB8888` also supported, not packed RGB888 |
| `DISPLAY_DEFAULT_DMA` | `dma0_ch3`; an empty string selects CPU transfers |
| `DISPLAY_DEFAULT_LLI_COUNT` | 20 DMA descriptors; synchronous drawing is automatically chunked |
| `DISPLAY_DEFAULT_FREQ_HZ` | 0, use the interface default frequency |
| `DISPLAY_DEFAULT_CS_GPIO`, `DISPLAY_DEFAULT_CS_PIN` | Software CS on GPIO5 |
| `DISPLAY_DEFAULT_RESET_USED`, `DISPLAY_DEFAULT_POWER_USED` | Both disabled by default; confirm actual wiring before enabling |

The font covers printable ASCII; see the public [`simple_font` usage and font license](../../../../bsp/common/display/simple_font/README.md).
Configure with `make menuconfig CHIP=bl616 BOARD=bl616dk BUILD_DIR=build_bl616`.
Saved menuconfig settings in a `BUILD_DIR` override `defconfig`; use separate build directories for different chips and cores.

### Changing panels

In `defconfig` or menuconfig, deselect ST7796 and select the matching `DISPLAY_DEFAULT_MODULE_*` and RGB565/XRGB8888 format. Retain GRAM and DBI B8, and meet the size/transfer requirements above.
Adapt CS, reset and backlight to the actual wiring. `adjust_default_module()` in `main.c` is enabled only for ST7796 and sets this glass's BGR order, mirroring, display inversion and column inversion; check these parameters when changing glass variants as well.
Use a new `BUILD_DIR` or update saved menuconfig settings so an old configuration does not override the changes.

## Build

Run from this example directory after setting up the SDK toolchain and environment.

For the default wiring: `make CHIP=bl616 BOARD=bl616dk BUILD_DIR=build_bl616`.

To flash, retain the same chip, board, core and build-directory arguments, append `flash COMX=/dev/ttyUSB0`, and replace the serial port with the actual device.
