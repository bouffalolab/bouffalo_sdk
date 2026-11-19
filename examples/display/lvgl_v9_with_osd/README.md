# display / lvgl_v9_with_osd

[中文](README_CN.md)

JPEG video playback with an animated LVGL v9 OSD. The display framework
(`bsp/common/display`) combines an **NV12 Base** video layer with an
**ARGB8888 OSD0** UI layer. JPEG input comes from either an SD card (FS) or USB
DATA ACM. Optional WiFi HTTP OTA is available with either source.

The adaptive UI in `demos/ui/` shares seven inline C icons, playback controls,
a volume slider and a scrolling subtitle. It reads the LVGL display resolution,
selects fonts by the shorter side and relayouts on resolution changes. Portrait
framebuffers use individually transformed widgets rotated 270 degrees for
landscape viewing; square and landscape framebuffers use native orientation.
The screen background remains transparent. No touch input or audio playback is
configured; the controls are UI animations, not audio transport controls.

## Platform and operating modes

Only **BL618DG AP** is supported. Other chips and the NP core are rejected.
The supplied configuration uses `BOARD=bl618dgdk`, 16 MiB AP PSRAM and, by
default, the EK79007 1024x600 MIPI DSI 2-lane module with reset on GPIO2.

| Mode | `CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO` | `CONFIG_LVGL_V9_WITH_OSD_OTA` | JPEG source / network services |
|---|---|---|---|
| FS (default) | `n` | `n` | SD card only; no USB video or WiFi OTA |
| USB | `y` | `n` | USB DATA ACM, NetHub ECM/CMD ACM, WiFi and iperf |
| FS + OTA | `n` | `y` | SD card and WiFi HTTP OTA |
| USB + OTA | `y` | `y` | USB DATA ACM, NetHub services and WiFi HTTP OTA |

Both options default to **`n`**. OTA does not select USB; enabling USB replaces
the SD reader rather than enabling runtime source switching.

`config_file/` contains the FreeRTOS, FatFS, LVGL, lwIP, USB and video
configuration headers, plus the reference-only LCD configuration header.

## Build and flash

Set up the SDK toolchain and run all commands below from
`examples/display/lvgl_v9_with_osd`. Use a separate build directory per mode
and panel configuration; the commands provide explicit overrides and do not
require `menuconfig`.

```sh
# FS only
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build/fs CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=n CONFIG_LVGL_V9_WITH_OSD_OTA=n

# USB video
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build/usb CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=y CONFIG_LVGL_V9_WITH_OSD_OTA=n

# FS + WiFi HTTP OTA
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build/ota CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=n CONFIG_LVGL_V9_WITH_OSD_OTA=y

# USB video + WiFi HTTP OTA
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build/usb_ota CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=y CONFIG_LVGL_V9_WITH_OSD_OTA=y
```

Output is `<BUILD_DIR>/build_out/display_lvgl_v9_with_osd_bl618dg_ap.{elf,bin,map}`.
OTA builds also generate the RAW `.bin.ota` package.

Connect the programming UART and enter download mode according to the board
instructions. Compile first, then flash the corresponding existing output:

```sh
make flash CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build/fs COMX=xxx CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=n CONFIG_LVGL_V9_WITH_OSD_OTA=n
make flash CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build/usb COMX=xxx CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=y CONFIG_LVGL_V9_WITH_OSD_OTA=n
make flash CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build/ota COMX=xxx CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=n CONFIG_LVGL_V9_WITH_OSD_OTA=y
make flash CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build/usb_ota COMX=xxx CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=y CONFIG_LVGL_V9_WITH_OSD_OTA=y
```

Run only the flash command for the selected mode. Replace `xxx` with the actual
serial port. The default programming baud rate is 2000000; override `BAUDRATE`
as needed. Keep the same `BUILD_DIR`, mode and panel overrides for building
and flashing. After flashing, leave download mode and reset the board.
The application UART console runs at **2000000 baud**.

## Panel selection and wiring

Select a video module with `CONFIG_DISPLAY_DEFAULT_VIDEO_*=y` on the build
command line. Panel selection is defined in
`bsp/common/display/default/Kconfig`.

The available panel modules are listed below. Set exactly one applicable
Kconfig module to `y`.

| Reference selection | Active Kconfig selection (`=y`) | Resolution / interface |
|---|---|---|
| `LCD_DSI_ILI9881C_KD050020` | `CONFIG_DISPLAY_DEFAULT_VIDEO_ILI9881C_KD050020` | 720x1280, DSI 4 lanes |
| `LCD_DSI_ILI9881C_KD050023W4` | `CONFIG_DISPLAY_DEFAULT_VIDEO_ILI9881C_KD050023W4` | 720x1280, DSI 2 lanes |
| `LCD_DSI_ILI9806E_KD050FWFIA019` | `CONFIG_DISPLAY_DEFAULT_VIDEO_ILI9806E_480X854` | 480x854, DSI 2 lanes |
| `LCD_DSI_ST7102_YH494` | `CONFIG_DISPLAY_DEFAULT_VIDEO_ST7102_YH494` | 480x960, DSI 2 lanes |
| `LCD_DSI_ST7102_KD027HVF` | `CONFIG_DISPLAY_DEFAULT_VIDEO_ST7102_KD027HVF` | 320x320, DSI 1 lane |
| `LCD_DSI_AXS15231B_HS035` | `CONFIG_DISPLAY_DEFAULT_VIDEO_AXS15231B_172X640` | 172x640, DSI 1 lane |
| `LCD_DSI_AXS15231E_JX371` | `CONFIG_DISPLAY_DEFAULT_VIDEO_AXS15231E_258X960` | 258x960, DSI 1 lane |
| `LCD_DSI_EK79007_WKS70WSV114` | `CONFIG_DISPLAY_DEFAULT_VIDEO_EK79007_2LANE` (default) or `CONFIG_DISPLAY_DEFAULT_VIDEO_EK79007_4LANE` | 1024x600, DSI 2 or 4 lanes |
| `LCD_DSI_ST7701S_HD40007C30` | `CONFIG_DISPLAY_DEFAULT_VIDEO_ST7701S_DSI_480X480` | 480x480, DSI 2 lanes |
| `LCD_DSI_FL7707N_HD395003C30` | `CONFIG_DISPLAY_DEFAULT_VIDEO_FL7707N_720X720` | 720x720, DSI 2 lanes |
| `LCD_DSI_JD9365TX_7KF82` | `CONFIG_DISPLAY_DEFAULT_VIDEO_JD9365TX_720X1280` | 720x1280, DSI 2 lanes |
| `LCD_DPI_GC9503V` | `CONFIG_DISPLAY_DEFAULT_VIDEO_GC9503V_480X480` | 480x480, RGB + command port |
| `LCD_DPI_ST7701P` | `CONFIG_DISPLAY_DEFAULT_VIDEO_ST7701P_480X480` | 480x480, RGB + command port |
| `LCD_DPI_ST7701S` | `CONFIG_DISPLAY_DEFAULT_VIDEO_ST7701S_RGB_480X480` | 480x480, RGB + command port |
| `LCD_DPI_JD9165BA` | `CONFIG_DISPLAY_DEFAULT_VIDEO_JD9165BA_1024X600` | 1024x600, RGB, optional command port |
| `LCD_DPI_STANDARD` | `CONFIG_DISPLAY_DEFAULT_VIDEO_RGB_800X480` or `CONFIG_DISPLAY_DEFAULT_VIDEO_RGB_1024X600` | Standard RGB, no command port |

- Panel dimensions come from the selected default module through
  `DISPLAY_DEFAULT_VIDEO_WIDTH/HEIGHT`; LVGL uses that display resolution.
  JPEG dimensions and `FRAME_DIR` remain separately editable in `config_file/video_config.h`
  and do **not** automatically change with the panel selection. Their defaults
  are **1024x600** and **`/sd/1024x600`** for both DSI and RGB.
- Keep `CONFIG_DISPLAY_DEFAULT_VIDEO_FORMAT_ARGB8888=y` and
  `CONFIG_LVGL9_PORT_DISP=y`. The default-video LVGL port owns OSD0; RGB565 and
  XRGB8888 cannot provide the per-pixel alpha required here. Base remains NV12.
- DSI lane count defaults to the selected module. Match the physical wiring
  with `CONFIG_DISPLAY_DEFAULT_DSI_LANES` and the lane-order choices if needed.
- Backlight defaults to **`CONFIG_DISPLAY_DEFAULT_BACKLIGHT_NONE=y`**. Supply
  panel and backlight power externally. BL618DG USB uses **GPIO40/41**; never
  configure GPIO40 as backlight when using USB. The GPIO40 setting in the
  reference header must not be copied into the active configuration.
- RGB/DPI needs **`CONFIG_DPI_DISPLAY_BOARD_MODE=y`** so the control signals use
  GPIO0..3 rather than SDH GPIO45..48. The example's Kconfig automatically
  selects board mode for RGB video. For FS (including FS + OTA), CMake also
  forces SDH **1-line** operation and the reader leaves GPIO43/44 available to
  RGB, because these pins otherwise carry SDH DAT3/DAT2. Both board mode and
  1-line negotiation are required; GPIO remuxing alone is insufficient.
- **GPIO2 is an RGB control signal.** For RGB, override the DSI reset default
  with **`CONFIG_DISPLAY_DEFAULT_RESET_USED=n`**, unless the panel actually has
  a reset wired to another free GPIO; in that case set both
  `CONFIG_DISPLAY_DEFAULT_RESET_USED=y` and `CONFIG_DISPLAY_DEFAULT_RESET_PIN`
  to the real reset pin. CMake rejects reset on any RGB control pin GPIO0..3.
- RGB command-port defaults are
  **CS=GPIO34, SCK=GPIO35, data=GPIO36** (3-wire).
  Adjust `CONFIG_DISPLAY_DEFAULT_RGB_CMD_CS_PIN`, `_SCK_PIN`, `_MOSI_PIN` to
  the hardware. Select `CONFIG_DISPLAY_DEFAULT_RGB_CMD_C4=y` for a 4-wire port
  and configure `_DC_PIN` (default GPIO37). JD9165BA may instead select
  `CONFIG_DISPLAY_DEFAULT_RGB_CMD_NONE=y` if no initialization port is wired.
  Check these optional command pins against all other board connections.

For example, standard RGB 1024x600 with FS and no GPIO reset:

```sh
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build/fs_rgb CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=n CONFIG_LVGL_V9_WITH_OSD_OTA=n CONFIG_DISPLAY_DEFAULT_VIDEO_RGB_1024X600=y CONFIG_DISPLAY_DEFAULT_RESET_USED=n
```

## SD video assets (FS and FS + OTA)

Insert a FAT32 SD card into the board's SDH socket. The existing **9468-frame
1024x600** asset sequence can be used with the default configuration. The
reader determines the contiguous frame count; 9468 is not a hard-coded limit.
To export another clip, the host needs Python 3 and ffmpeg:

```sh
python3 tools/export_video.py input.mp4 osd-assets --frames 300 --quality 8
```

Copy the generated `1024x600` directory to the card root:

```text
/1024x600/000/p0001.jpg ... p0100.jpg
/1024x600/001/p0101.jpg ... p0200.jpg
/1024x600/002/p0201.jpg ...
```

The first frame on the card is `/1024x600/000/p0001.jpg`; firmware accesses it
as `/sd/1024x600/000/p0001.jpg`. **Do not create an extra `sd` directory on the
card**. Frames must be contiguous from 1, with 100 frames per directory by
default and at most 10000 frames. Playback loops at the detected end.
`FRAMES_PER_CHUNK` and the card layout must agree.

The exporter defaults to 100 fps, quality 8 and at most 10000 frames. It writes
1024x600 frames only and refuses an existing `1024x600` output directory to
avoid stale tail frames; discard partial exports before retrying. For another
panel/video size, prepare matching assets and update `VIDEO_WIDTH`,
`VIDEO_HEIGHT` and `FRAME_DIR` together in `config_file/video_config.h`; the exporter has
no width/height options.

Every JPEG must be complete baseline 8-bit YCbCr 4:2:0, exactly matching
`VIDEO_WIDTH/HEIGHT`, and no larger than 128 KiB. Use standard Huffman tables
with one table per FFC4 DHT segment; ordinary ffmpeg defaults or optimized
tables are insufficient. Both host tools normalize the DHT layout.

Missing SD/filesystem or first frame leaves the reader idle with an error.
Later missing frames restart the sequence; invalid sizes and read failures
are logged and skipped. Error/retry paths yield for 10 ms to avoid busy loops.
The decoder drops invalid format/dimension/table
inputs. See [tools/README.md](tools/README.md) for JPEG compatibility details.

## USB video and host connection

USB mode owns the composite descriptor, interfaces, endpoints and enumeration.
NetHub handles ECM/CMD ACM transfers and network routing; it receives USB
notifications from the example. Video DATA ACM callbacks feed `usb_reader`
directly, without a NetHub stream.

| Function | Interface | Endpoint |
|---|---|---|
| ECM network | 0/1 | IN `0x81`, OUT `0x02`, INT `0x85` |
| CMD ACM / vchan | 2/3 | IN `0x83`, OUT `0x04`, INT `0x86` |
| Video DATA ACM | 4/5 | IN `0x87`, OUT `0x08`, INT `0x07` |

The host needs ffmpeg on PATH and pyserial (`pip install pyserial`). After
connecting the USB data port on Linux:

```sh
ifconfig -a
ls /dev/ttyACM*
python3 tools/stream_video.py --list-devs
```

Expect one ECM network interface (such as `enxb4e8423cb200`) and two CDC ACM tty
nodes (usually `/dev/ttyACM0` and `/dev/ttyACM1`). Video DATA ACM normally uses
interface **`04`**; the script auto-detects it when `--data-dev` is omitted.

For the default 1024x600 firmware, stream with explicit dimensions:

```sh
python3 tools/stream_video.py --video video_ls.mp4 --width 1024 --height 600 --loop
```

The basic invocation `python3 tools/stream_video.py --video video_ls.mp4 --loop`
is also available, but the script's unchanged defaults are **480x960**, 24 fps,
quality 4. It requires firmware configured for 480x960 video, not the default
1024x600 firmware. Always match `--width/--height` to `VIDEO_WIDTH/HEIGHT`.
Other options include `--fps`, `--quality` (2=best..31), `--data-dev`,
`--data-interface`, `--cmd-dev`, `--cmd-interface`, `--baud`, `--no-pace` and
`--ping`; see [tools/README.md](tools/README.md).

If USB disconnects or the board resets while streaming, the script keeps
ffmpeg running, waits for DATA interface `04` to reappear and resumes at the
next complete JPEG. The `/dev/ttyACM*` number may change after re-enumeration.
Firmware resets reception to the next 12-byte frame header on disconnect.

### WiFi and iperf (USB mode)

The device runs a WiFi station and bridges traffic to the host through ECM.
From the `uart0` shell at 2000000 baud:

```text
wifi_sta_connect -D <SSID> <PASSWORD>
```

`-D` disables device DHCP for the bridge setup. For device-local OTA, use
`wifi_sta_connect <SSID> <PASSWORD>` **without `-D`** and wait for
`CODE_WIFI_ON_GOT_IP` instead.

Find the USB ECM interface on the host and bind iperf traffic to its IP:

```sh
ifconfig -a
IF=enxb4e8423cb200
iperf3 -c <server_ip> -B <enx_iface_ip>
```

Replace the example interface and addresses with the actual ECM interface,
its configured IP and the iperf server IP.

## WiFi HTTP OTA (FS + OTA and USB + OTA)

First install the OTA-enabled application **and `partition_ota_8M.toml`
partition table over UART** using the corresponding build/flash commands.
At least **8 MiB Flash** is required. The layout provides two non-overlapping
2 MiB firmware slots; the firmware payload must fit the inactive slot.

Serve the generated RAW `.bin.ota` package from the selected OTA build:

```sh
# FS + OTA
python3 -m http.server 8000 --directory build/ota/build_out
# For USB + OTA, serve build/usb_ota/build_out instead.
```

In the UART shell at 2000000 baud, connect WiFi and use the computer's LAN IP:

```text
wifi_sta_connect <SSID> <PASSWORD>
# Wait for CODE_WIFI_ON_GOT_IP.
https_ota_start http://<COMPUTER_LAN_IP>:8000/display_lvgl_v9_with_osd_bl618dg_ap.bin.ota
ota_status
```

The command name remains `https_ota_start`, but this application accepts
**HTTP only**. It pauses the JPEG producer and decoder at safe boundaries,
downloads to the inactive slot, verifies SHA256 and reboots automatically on
success. The OSD shows OTA phase/progress. To stage without automatic reboot,
append `0` to `https_ota_start <URL>`, then use `ota_reboot` after success.
Failures abort the upgrade and resume the video pipeline.

WiFi startup and USB ECM/CMD ACM enumeration do not wait for successful display
initialization; only the video producer and decoder are gated by display readiness.
This preserves network and OTA command access on display initialization failure,
but does not guarantee an upgrade: OTA requires all active video pause participants
to acknowledge, and aborts if any active participant is missing. Before video tasks
start, no video participants are registered. A decoder waiting for an accepted
display submission can acknowledge a pause without releasing its frame buffers.

## Pipeline and buffer ownership

```text
FS:  SD JPEG files -> filesystem_reader -> JPEG queue -> dpi_manager -> MJDEC
USB: host video -> ffmpeg -> tools/stream_video.py -> DATA ACM callback
     -> usb_reader -> JPEG queue -> dpi_manager -> MJDEC

MJDEC -> NV12 buffers -> app_display -> DPU Base
LVGL demos/ui -> default-video LVGL port -> ARGB8888 OSD0
Base + OSD0 -> selected DSI/RGB panel
```

- The selected reader feeds 128 KiB JPEG buffers (two for FS, three for USB);
  `dpi_manager` handles MJDEC, NV12 frame buffers (triple-buffered for DSI,
  double-buffered for RGB), rate limiting and decoder recovery.
- Full-screen 1024x600 decodes directly to NV12. Smaller video regions or
  non-MCU-aligned widths use the DMA2D centered crop/repack path. Video must
  fit within the panel and both video/panel dimensions must be even for NV12.
- `app_display` attaches the Base layer after default-video port initialization.
  Panel dimensions come from the selected default module; the LVGL port owns
  the two full-screen ARGB8888 OSD buffers and UI updates.
- DMA slots are 64-byte aligned. NV12 planes are cleaned/invalidated at
  initialization, then written only by MJDEC/DMA2D. Base cache cleaning is
  disabled to avoid stale CPU cache overwriting DMA output; CPU code must not
  write these planes afterward. JPEG input is cleaned before MJDEC reads it.
- FIRST/RETIRED/LAST display completion events govern buffer ownership.
  VSYNC or a timeout never permits premature reuse. A missing completion
  interrupt logs an error and continues waiting.

## Configuration limits

- The panel table describes available module selections, not a guarantee of
  plug-and-play operation on every board. Panel timing, lane wiring, power,
  reset, backlight and command-port wiring must match the actual hardware.
- Panel selection does not resize JPEG assets. The default 1024x600 video
  cannot fit smaller panels without changing `config_file/video_config.h` and the assets
  or USB stream size. Available PSRAM also constrains larger configurations.
- `VIDEO_TARGET_FPS` defaults to **100 fps** and is a cap, not a guaranteed
  playback rate. SD playback does not preserve source timestamps. Match the
  exporter's `--fps` to the intended cap; the USB tool defaults to 24 fps.
- OTA supports RAW `.bin.ota` over HTTP with status 200 and fixed valid
  `Content-Length`. HTTPS trust, redirects, chunked responses and XZ packages
  are not supported by this application. Use a trusted local network;
  SHA256 integrity checking is not server authentication.
- The complete bundled `demos/` sources and assets are present, but CMake
  compiles only `demos/ui/*.c` recursively. Other bundled demos are not enabled
  by their presence. No touch or audio subsystem is configured.