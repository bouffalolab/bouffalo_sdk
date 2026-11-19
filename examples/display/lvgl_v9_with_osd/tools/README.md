# NetHub USB Video Streamer

Streams a video as 24fps JPEG frames over USB CDC-ACM to the BL618DG firmware in
`examples/display/lvgl_v9_with_osd` when it is built with
`CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=y`. The firmware MJDEC-decodes the JPEG
stream into the display framework's NV12 Base layer under the ARGB8888 LVGL
OSD0 layer. This example supports BL618DG AP only. USB video is disabled by
default; build instructions and the independent OTA option are in
[the example README](../README.md).

Run the commands below from `examples/display/lvgl_v9_with_osd`, not from
the `tools` directory. The scripts do not select the firmware's panel or video
dimensions; match their output to `VIDEO_WIDTH/HEIGHT` in
[`config_file/video_config.h`](../config_file/video_config.h).

## Requirements
- `ffmpeg` on PATH
- `pip install pyserial`

## Frame protocol (data ACM)
Each JPEG frame is prefixed with a 12-byte little-endian header:

| field  | bytes | value                          |
|--------|-------|--------------------------------|
| magic  | 4     | `0x55AA55AA`                   |
| length | 4     | JPEG payload length            |
| idx    | 2     | frame sequence number          |
| resv   | 2     | 0                              |

Then `length` bytes of JPEG follow. If a header is rejected, the firmware drains
the declared payload length before receiving the next header. A USB disconnect
resets the receive state back to the next 12-byte header.

## JPEG Huffman table compatibility
The BL618DG MJDEC's Huffman-table RAM is fixed-size and must be loaded with the
**standard** full JPEG tables (Y-DC=12, Y-AC=162, UV-DC=12, UV-AC=162 bytes).
ffmpeg's mjpeg encoder defaults to `-huffman optimal`, which emits smaller
*optimized* tables that the hardware rejects:

```
bflb_mjdec_set_dht_from_header error, 25      # MJDEC_ERR_DHT_YY_DC_BYTES
******  mjdec reset!  ******
video: 0 fps (ok=0 timeout=6)
```

`stream_video.py` therefore always passes `-huffman default` (and
`-pix_fmt yuvj420p` for 4:2:0/NV12).

Frames must be complete baseline 8-bit YCbCr 4:2:0 JPEGs, match the configured
video dimensions exactly and fit the firmware's 128 KiB JPEG buffer. If a
frame is too large, increase `--quality` (larger values reduce JPEG quality
and usually reduce size).

There is one more BL618DG LHAL parser constraint: `bflb_mjdec_set_dht_from_header()`
expects each `FFC4` DHT segment to contain exactly one Huffman table. ffmpeg
usually emits one legal combined DHT segment containing all four tables, so the
script rewrites each JPEG frame before sending it and splits the combined DHT
segment into one segment per table. If you write your own encoder/converter (or
switch to the real camera path), keep both constraints in mind.

## Identify the ACM node
The NetHub composite device exposes multiple CDC-ACM channels. The video DATA
ACM is interface `04` by default, and `stream_video.py` auto-detects it when
`--data-dev` is omitted.

Confirm with:
```bash
python3 tools/stream_video.py --list-devs
```

## Run
```bash
# stream the sample clip to the DATA channel (480x960 video firmware)
python3 tools/stream_video.py --video video_ls.mp4 --loop

# match this example's default 1024x600 video configuration
python3 tools/stream_video.py --video video_ls.mp4 --width 1024 --height 600 --loop
```

Options: `--width/--height` (default 480x960), `--fps` (24), `--quality`
(MJPEG q:v, 2=best..31, default 4), `--loop`.
The streamer's 480x960 default is independent of the firmware's 1024x600
default; use explicit dimensions for the default panel/video configuration.

Additional channel and transport options:

- `--data-dev /dev/ttyACM1`: explicitly select DATA ACM; otherwise auto-detect
	by `--data-interface` (default `04`).
- `--cmd-dev /dev/ttyACM0`: optionally send a vchan-framed `START` on CMD ACM
	before streaming. Normal video reception does not require this command.
- `--cmd-interface` (default `02`): CMD ACM auto-detection for `--ping`.
- `--ping`: send `PING` on CMD ACM and check for `PONG`, then exit.
- `--baud` (default 2000000): nominal serial baud, ignored by CDC-ACM.
- `--no-pace`: disable Python write pacing; intended only for throughput use.

Explicit channel selection and a command-channel probe:

```bash
python3 tools/stream_video.py --video video_ls.mp4 --width 1024 --height 600 --data-dev /dev/ttyACM1 --cmd-dev /dev/ttyACM0 --loop
python3 tools/stream_video.py --cmd-dev /dev/ttyACM0 --ping
```

If the board disconnects or resets while streaming, the script keeps ffmpeg
running, waits for DATA ACM interface `04` to reappear, and resumes at the next
complete JPEG frame. The `/dev/ttyACM*` number is allowed to change after USB
re-enumeration.

## Offline SD exporter

`export_video.py` prepares files for FS mode (including FS + OTA). It requires
Python 3 and ffmpeg only, does not need pyserial and never opens a board or
serial port.

```bash
python3 tools/export_video.py input.mp4 osd-assets --frames 300 --quality 8
```

Copy `osd-assets/1024x600` to the root of a FAT32 SD card. The first frame is
`/1024x600/000/p0001.jpg` on the card, accessed as
`/sd/1024x600/000/p0001.jpg` by firmware; do not add an extra `sd` directory.
Frames are contiguous from 1, with 100 per chunk directory:
`000/p0001.jpg` through `000/p0100.jpg`, then `001/p0101.jpg`, and so on.

Options are `--fps` (default 100, match `VIDEO_TARGET_FPS`), `--quality`
(2..31, default 8) and `--frames` (1..10000, default 10000). The exporter
always scales to **1024x600** and uses 100 frames per chunk; it has no
width/height or chunk-size options. It emits standard-table 4:2:0 JPEGs,
splits combined DHT segments, rejects frames over 128 KiB and refuses an
existing output `1024x600` directory. Discard partial exports before retrying.
For other video dimensions or chunk sizes, prepare matching files separately
and keep [`config_file/video_config.h`](../config_file/video_config.h) and the card layout consistent.
