# NetHub USB Video Streamer

Streams a video as 24fps JPEG frames over USB CDC-ACM to the BL618DG firmware in
`examples/lvgl/lvgl_v9_with_osd` when it is built with
`CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=y`. The firmware MJDEC-decodes the JPEG
stream and displays it under the LVGL OSD layer.

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
# stream the sample clip to the DATA channel
python3 tools/stream_video.py --video video_ls.mp4 --loop
```

Options: `--width/--height` (default 480x960), `--fps` (24), `--quality`
(MJPEG q:v, 2=best..31), `--loop`.

If the board disconnects or resets while streaming, the script keeps ffmpeg
running, waits for DATA ACM interface `04` to reappear, and resumes at the next
complete JPEG frame. The `/dev/ttyACM*` number is allowed to change after USB
re-enumeration.
