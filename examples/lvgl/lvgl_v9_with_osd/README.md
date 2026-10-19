# LVGL v9 With OSD

This example displays a JPEG video background with an LVGL v9 OSD overlay.

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

## WiFi + iperf (USB mode)

The device runs a WiFi station and bridges it to the host through the ECM
network interface. Connect WiFi from the device shell on `uart0` (115200):

```text
wifi_sta_connect -D <SSID> <PASSWORD>
```

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
