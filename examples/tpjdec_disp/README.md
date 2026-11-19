# Tiny JPEG DBI Display

[English](README.md) | [简体中文](README_CN.md)

## Requirements

**This example can only use the hardware DBI peripheral with YUV-to-RGB conversion. Hardware SPI, software-driven interfaces and video scanout are not supported, even when their panel wiring is described as DBI Type C.**

The example exclusively owns the DBI peripheral. Do not share it with another display, device, task or direct LHAL user while it runs.
Supported chips are BL616/BL618, BL616CL and BL618DG; the selected board must provide the interface pins and PSRAM required by the project.

## Operation

- Default panel: ILI9488, 320×480, driven by the DBI peripheral in Type C 4-wire mode at a requested 40 MHz, with hardware CS.
- The display is rotated 90° to 480×320. Startup shows two colored text lines and three RGB outlines on a dark background for 500 ms.
- Tiny JPEG decodes five built-in 320×240 images in a loop. Images are drawn at (0, 0), without scaling, followed by a 1 s delay after display completion.
- Normal display format is packed `RGB888`, with memory bytes `[B,G,R]`. JPEG output is packed YUV444 with bytes `[Y,U,V]`, also 3 bytes per pixel.
- Before each JPEG submission, a DCS NOP applies the display endpoint's normal configuration. The application then temporarily selects LHAL `DBI_PIXEL_INPUT_FORMAT_YUV444` and submits the image through `display_draw_async()`.
	The DBI peripheral converts YUV to RGB; the panel still receives RGB pixels. The completion callback restores `DBI_PIXEL_INPUT_FORMAT_RGB_888` and clears the FIFO before releasing the buffer. Failed submissions also restore RGB; display errors stop the example rather than retrying an unknown controller state.
- One 230400-byte PSRAM image buffer is reused only after completion, with 3 extra bytes for word-rounded DBI reads. JPEG input capacity is 32 KiB; decoder workspace is 16 KiB. Transfers exceeding `max_draw_bytes` are split into full-width, aligned row stripes; configurations that cannot fit an aligned stripe are rejected.

## Wiring

Set the module to DBI Type C 4-wire mode; connect SCL, SDA, D/C, CS, common GND and suitable power. Board defaults are:

| Signal | BL616DK GPIO | BL618DGDK GPIO |
| --- | --- | --- |
| SCL | 12 | 11 |
| SDA | 14 | 13 |
| D/C | 15 | 14 |
| Hardware CS | 13 | 12 |

Other boards require their own pin mapping. Reset, power switching and GPIO backlight control are disabled by default; provide the module's power, backlight and a deasserted RESET externally. RESET must not float.
To enable GPIO controls, set the corresponding `DISPLAY_DEFAULT_RESET_*`, `DISPLAY_DEFAULT_POWER_*` or `DISPLAY_DEFAULT_BACKLIGHT_*` options, using independently wired, non-conflicting GPIOs.

## Configuration

`defconfig` enables `BSP_DISPLAY`, `DISPLAY_DEFAULT`, GRAM, `DISPLAY_DEFAULT_IF_DBI_C4`, `DISPLAY_DEFAULT_MODULE_ILI9488_320X480`, `DISPLAY_DEFAULT_FORMAT_RGB888`, `TJPGDEC`, PSRAM and the 8×16 `simple_font` font.
Pixels use `dma0_ch3` and 40 DMA descriptors; an empty `DISPLAY_DEFAULT_DMA` selects CPU transfers.
On BL618DG, `RAM_FOR_AP_SIZE=0x48000` and `WRAM_FOR_AP_SIZE=0x28000` leave SRAM for either core; any accompanying firmware must use the same partition settings. The video/DSI backends are disabled.

Keep RGB888 and a hardware DBI interface when changing the panel. Select the matching module, interface and actual wiring; verify the panel supports the resulting 24-bit wire format and the 90° orientation.
The image must fit both the rotated screen and the 76800-pixel output capacity. The decoder uses full resolution and does not scale oversized JPEGs.
Use a fresh `BUILD_DIR` or update saved menuconfig settings when changing defaults; saved settings override `defconfig`.

### Optional SD-card input

Enable `FATFS`, `FATFS_SDH_SDCARD` and `BSP_SDH_SD`, and configure the SDH driver for the board. The loop then reads `/sd/img0.jpg` through `/sd/img4.jpg` instead of built-in data.
Files must be supported baseline JPEGs, no larger than 32 KiB, and meet the image dimensions/capacity above. Invalid or unreadable files are skipped.

**BL616DK's default SDH pins GPIO10–15 overlap its DBI-C4 pins. These default connections cannot be used together.** Provide a board-specific, non-conflicting pin mapping before enabling SD-card input. Check all SDH/display pins on other boards as well.

## Build and flash

Run in this example directory with the SDK toolchain configured:

- BL616/BL618: `make CHIP=bl616 BOARD=bl616dk BUILD_DIR=build_bl616`
- BL616CL: `make CHIP=bl616cl BOARD=bl616cldk BUILD_DIR=build_bl616cl`
- BL618DG AP: `make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap BUILD_DIR=build_ap`
- BL618DG NP: `make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=np BUILD_DIR=build_np`

For flashing, retain the same chip, board, core and build-directory arguments and append `flash COMX=/dev/ttyUSB0`, replacing the serial port with the actual device.
