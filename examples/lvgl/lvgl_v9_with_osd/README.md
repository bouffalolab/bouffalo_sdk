# LVGL v9 With OSD

This example displays a JPEG video background with an LVGL v9 OSD overlay.

The overlay uses one implementation in `demos/ui/` and one shared set of icons.
It reads the LVGL display resolution at runtime to size and position the icons,
volume slider, playback controls, and scrolling subtitle. Fonts are selected
according to the shorter display side. Resolution changes also relayout the UI.

As in the original overlays, portrait framebuffers are viewed in landscape:
when the framebuffer height exceeds its width, widget coordinates and content
are rotated 270 degrees. Square and landscape framebuffers use their native
orientation. Widgets are transformed individually to avoid a full-screen
intermediate rotation buffer. The screen background remains transparent for
the JPEG video layer; icon cycling and the slider sweep are retained.

Select the panel in `lcd_conf_user.h`. Adding a supported panel or changing its
resolution no longer requires a separate UI directory or a CMake UI mapping.

The display and OSD pipeline is shared. The JPEG input source is selected at
build time:

- default: read JPEG frames from SD card through `filesystem_reader`
- USB mode: receive JPEG frames from USB DATA ACM through NetHub stream and
  `usb_reader`

## Build

Default SD-card mode:

```bash
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0
```

USB video mode:

```bash
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=y
```

In USB mode this example owns the USB composite device descriptor and registers:

| Function | Interface | Endpoint |
|----------|-----------|----------|
| ECM network | 0/1 | IN `0x81`, OUT `0x02`, INT `0x85` |
| CMD ACM / vchan | 2/3 | IN `0x83`, OUT `0x04`, INT `0x86` |
| Video DATA ACM | 4/5 | IN `0x87`, OUT `0x08`, INT `0x07` |

NetHub does not own USB descriptor or interface/endpoint registration in this
mode. The example owns enumeration and notifies NetHub from USB callbacks;
NetHub keeps endpoint read/write, routing, ECM, CMD ACM, and generic stream
handling.

## WiFi OTA (SD and USB modes)

OTA works with any supported panel selected in `lcd_conf_user.h`; first install
the OTA-enabled application and partition table over UART (the current layout
requires at least 8 MiB Flash).

Build and serve the RAW `.bin.ota` package from this directory:

```bash
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build_ota
python3 -m http.server 8000 --directory build_ota/build_out
```

In the UART shell (2000000 baud), connect WiFi and use the computer's LAN IP:

```text
wifi_sta_connect <SSID> <PASSWORD>
# Wait for CODE_WIFI_ON_GOT_IP.
https_ota_start http://<COMPUTER_LAN_IP>:8000/lvgl_v9_with_osd_bl618dg_ap.bin.ota
```

The board verifies SHA256 and reboots automatically; use `ota_status` to check
progress.

## WiFi + iperf (USB mode)

The device runs a WiFi station and bridges it to the host through the ECM
network interface. Connect WiFi from the device shell on `uart0` (2000000 on this BL618DG configuration):

```text
wifi_sta_connect -D <SSID> <PASSWORD>
```

`-D` disables DHCP on the device for this bridge setup. To use device-local
OTA, connect with `wifi_sta_connect <SSID> <PASSWORD>` instead and wait for
`CODE_WIFI_ON_GOT_IP` before starting an upgrade.

On the host, find the USB-enumerated ECM interface (name like `enx<mac>`) and
run iperf over it:

```bash
ifconfig -a                       # find the enx... interface the device created
IF=enxb4e8423cb200
iperf3 -c <server_ip> -B <enx_iface_ip>   # -B binds traffic to the ECM interface

```

## USB Host Check

After plugging in USB on Linux:

```bash
ifconfig -a
ls /dev/ttyACM*
python3 tools/stream_video.py --list-devs
```

Expected result:

- one ECM network interface, for example `enxb4e8423cb200`
- two CDC ACM tty nodes, usually `/dev/ttyACM0` and `/dev/ttyACM1`
- Video DATA ACM is normally interface `04`; the host script tries to detect it

Stream video:

```bash
python3 stream_video.py --video video_ls.mp4 --loop
```

## Data Flow

SD mode:

```text
SD card JPEG files -> filesystem_reader -> JPEG queue -> dpi_manager -> MJDEC -> LCD background
```

USB mode:

```text
Linux video file
    -> ffmpeg MJPEG
    -> tools/stream_video.py
    -> Video DATA ACM
    -> NetHub stream ACM
    -> usb_reader
    -> JPEG queue
    -> dpi_manager
    -> MJDEC
    -> LCD background
```

LVGL OSD rendering is common to both modes.
