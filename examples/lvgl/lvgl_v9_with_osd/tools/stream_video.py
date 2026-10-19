#!/usr/bin/env python3
"""
stream_video.py - Smart-lock USB video streamer (host side).

Decodes a video file with ffmpeg into a 24fps MJPEG stream, scales it to the
panel resolution, splits the stream into individual JPEG frames, wraps each
frame with the length-prefix header the BL618DG firmware expects, and writes it
to the DATA CDC-ACM device.

Frame protocol (little-endian), one per JPEG frame:

    magic (4) = 0x55AA55AA | length (4) = JPEG bytes | idx (2) | resv (2) | <JPEG ...>

Older firmware exposes one ttyACM node, used as the DATA channel for this
stream. NetHub composite firmware exposes multiple ttyACM nodes; the video DATA
ACM is interface 04 by default. The script auto-detects that node when
--data-dev is omitted.

Examples:
    # auto-pick the NetHub video DATA ACM when possible
    python3 stream_video.py --video ../video_ls.mp4

    # keep the old explicit form and optionally send START on CMD ACM first
    python3 stream_video.py --video ../video_ls.mp4 \
        --data-dev /dev/ttyACM1 --cmd-dev /dev/ttyACM0 --loop

    # probe which node is the command channel
    python3 stream_video.py --cmd-dev /dev/ttyACM0 --ping

Requires: ffmpeg on PATH, pyserial (`pip install pyserial`).
"""

import argparse
import glob
import os
import struct
import subprocess
import sys
import time

try:
    import serial  # pyserial
except ImportError:
    serial = None

FRAME_MAGIC = 0x55AA55AA
DEFAULT_DATA_INTERFACE = "04"
DEFAULT_CMD_INTERFACE = "02"
VCHAN_MAGIC = b"\xa5\x5a"
VCHAN_CHECKSUM = 0xA5A55A5A
VCHAN_MAX_PAYLOAD = 1500
SOI = b"\xff\xd8"  # JPEG start-of-image
EOI = b"\xff\xd9"  # JPEG end-of-image
RECONNECT_DELAY_S = 1.0


def build_ffmpeg_cmd(video, width, height, fps, quality, loop):
    """ffmpeg pipeline: (loop) -> decode -> scale -> force fps -> MJPEG to stdout.

    The sample clip is already rotated, so no transpose filter is needed. `-re`
    paces output at native rate; `-r <fps>` resamples to a constant frame rate.
    """
    cmd = ["ffmpeg", "-hide_banner", "-loglevel", "error"]
    if loop:
        cmd += ["-stream_loop", "-1"]
    cmd += [
        "-re",
        "-i", video,
        "-an",                       # drop audio
        "-vf", f"scale={width}:{height}",
        "-r", str(fps),
        "-f", "image2pipe",
        "-c:v", "mjpeg",
        # BL618DG MJDEC expects the standard Huffman table sizes. ffmpeg's default
        # may emit optimized tables, so force the standard tables first; the
        # host-side normalizer below then splits ffmpeg's combined DHT segment
        # into one table per segment for bflb_mjdec_set_dht_from_header().
        "-huffman", "default",
        "-pix_fmt", "yuvj420p",      # 4:2:0 -> NV12, matches the firmware decoder
        "-q:v", str(quality),
        "pipe:1",
    ]
    return cmd


def iter_jpeg_frames(stream):
    """Yield complete JPEG frames (SOI..EOI) from a raw MJPEG byte stream."""
    buf = bytearray()
    while True:
        chunk = stream.read(65536)
        if not chunk:
            break
        buf += chunk
        # extract every complete SOI..EOI frame currently in the buffer
        while True:
            start = buf.find(SOI)
            if start < 0:
                # no frame start yet; keep only a tail in case SOI straddles reads
                if len(buf) > 1:
                    del buf[:-1]
                break
            end = buf.find(EOI, start + 2)
            if end < 0:
                # incomplete frame; drop anything before the start and wait for more
                if start > 0:
                    del buf[:start]
                break
            end += 2  # include the EOI marker
            yield bytes(buf[start:end])
            del buf[:end]


def split_multi_table_dht_segments(jpeg):
    """Split JPEG DHT segments so each FFC4 segment contains exactly one table.

    ffmpeg commonly emits one legal DHT segment containing all four tables. The
    BL618DG LHAL bflb_mjdec_set_dht_from_header() parser expects one table per
    DHT segment and otherwise computes the first table length from the whole
    segment, causing MJDEC_ERR_DHT_YY_DC_BYTES (25).
    """
    if len(jpeg) < 4 or not jpeg.startswith(SOI):
        return jpeg

    out = bytearray(jpeg[:2])
    pos = 2
    changed = False

    while pos < len(jpeg):
        marker_start = pos
        if jpeg[pos] != 0xFF:
            out += jpeg[pos:]
            break

        while pos < len(jpeg) and jpeg[pos] == 0xFF:
            pos += 1
        if pos >= len(jpeg):
            out += jpeg[marker_start:]
            break

        marker = jpeg[pos]
        pos += 1

        if marker == 0x00:
            out += jpeg[marker_start:]
            break

        if marker == 0xD9 or marker == 0x01 or 0xD0 <= marker <= 0xD7:
            out += jpeg[marker_start:pos]
            if marker == 0xD9:
                out += jpeg[pos:]
                break
            continue

        if pos + 2 > len(jpeg):
            out += jpeg[marker_start:]
            break

        seg_len = (jpeg[pos] << 8) | jpeg[pos + 1]
        seg_end = pos + seg_len
        if seg_len < 2 or seg_end > len(jpeg):
            out += jpeg[marker_start:]
            break

        if marker == 0xDA:
            out += jpeg[marker_start:]
            break

        if marker != 0xC4:
            out += jpeg[marker_start:seg_end]
            pos = seg_end
            continue

        payload = jpeg[pos + 2:seg_end]
        entries = []
        entry_pos = 0
        ok = True

        while entry_pos < len(payload):
            table_start = entry_pos
            if entry_pos + 17 > len(payload):
                ok = False
                break

            entry_pos += 1
            counts = payload[entry_pos:entry_pos + 16]
            entry_pos += 16
            value_count = sum(counts)
            if value_count == 0 or entry_pos + value_count > len(payload):
                ok = False
                break

            entry_pos += value_count
            entries.append(payload[table_start:entry_pos])

        if ok and len(entries) > 1:
            for entry in entries:
                out += b"\xff\xc4" + struct.pack(">H", len(entry) + 2) + entry
            changed = True
        else:
            out += jpeg[marker_start:seg_end]

        pos = seg_end

    if not changed:
        return jpeg
    return bytes(out)


def open_serial(dev, baud):
    if serial is None:
        sys.exit("pyserial not installed: pip install pyserial")
    # CDC-ACM ignores baud/flow settings, but pyserial needs a value.
    return serial.Serial(dev, baudrate=baud, timeout=1, write_timeout=5)


def tty_interface_num(dev):
    """Return the USB interface number for /dev/ttyACMx, e.g. '04'."""
    name = os.path.basename(dev)
    sysfs_path = os.path.join("/sys/class/tty", name, "device", "bInterfaceNumber")
    try:
        with open(sysfs_path, "r", encoding="ascii") as fp:
            return fp.read().strip().lower()
    except OSError:
        pass

    try:
        result = subprocess.run(
            ["udevadm", "info", "-q", "property", "-n", dev],
            check=False,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            text=True,
        )
    except OSError:
        return None

    for line in result.stdout.splitlines():
        if line.startswith("ID_USB_INTERFACE_NUM="):
            return line.split("=", 1)[1].strip().lower()
    return None


def list_acm_devices():
    return sorted(glob.glob("/dev/ttyACM*"))


def print_acm_devices(stream=sys.stdout):
    devs = list_acm_devices()
    if not devs:
        print("no /dev/ttyACM* devices found", file=stream)
        return

    for dev in devs:
        iface = tty_interface_num(dev) or "unknown"
        print(f"{dev}: interface {iface}", file=stream)


def select_data_dev(explicit_dev, data_interface):
    if explicit_dev:
        return explicit_dev

    devs = list_acm_devices()
    if not devs:
        sys.exit("no /dev/ttyACM* devices found; connect/reset the board first")

    target = data_interface.lower()
    matches = [dev for dev in devs if tty_interface_num(dev) == target]
    if len(matches) == 1:
        return matches[0]

    if len(devs) == 1:
        print(f"auto-selected only ACM device {devs[0]}", file=sys.stderr)
        return devs[0]

    print("could not uniquely choose the video DATA ACM.", file=sys.stderr)
    print_acm_devices(stream=sys.stderr)
    sys.exit(f"pass --data-dev explicitly, or adjust --data-interface {data_interface}")


def find_reconnected_data_dev(previous_dev, data_interface):
    """Find the DATA ACM again after USB re-enumeration.

    Linux may assign a different ttyACM number after reconnecting, so prefer
    the USB interface number over the old device path.
    """
    devs = list_acm_devices()
    target = data_interface.lower()
    matches = [dev for dev in devs if tty_interface_num(dev) == target]

    if previous_dev in matches:
        return previous_dev
    if len(matches) == 1:
        return matches[0]
    return None


def wait_for_data_serial(previous_dev, data_interface, baud):
    print(f"waiting for DATA ACM interface {data_interface} to reconnect...", file=sys.stderr)
    while True:
        data_dev = find_reconnected_data_dev(previous_dev, data_interface)
        if data_dev is not None:
            try:
                data_ser = open_serial(data_dev, baud)
                print(f"DATA ACM reconnected: {data_dev}", file=sys.stderr)
                return data_ser, data_dev
            except (serial.SerialException, OSError):
                pass
        time.sleep(RECONNECT_DELAY_S)


def select_cmd_dev(explicit_dev, cmd_interface):
    if explicit_dev:
        return explicit_dev

    devs = list_acm_devices()
    if not devs:
        sys.exit("no /dev/ttyACM* devices found; connect/reset the board first")

    target = cmd_interface.lower()
    matches = [dev for dev in devs if tty_interface_num(dev) == target]
    if len(matches) == 1:
        return matches[0]

    print("could not uniquely choose the CMD ACM.", file=sys.stderr)
    print_acm_devices(stream=sys.stderr)
    sys.exit(f"pass --cmd-dev explicitly, or adjust --cmd-interface {cmd_interface}")


def vchan_frame(payload):
    if not payload or len(payload) > VCHAN_MAX_PAYLOAD:
        raise ValueError("invalid vchan payload length")
    return VCHAN_MAGIC + struct.pack("<H", len(payload)) + payload + struct.pack("<I", VCHAN_CHECKSUM)


def vchan_payloads_from(buf):
    out = []
    i = 0
    while i + 8 <= len(buf):
        magic = buf.find(VCHAN_MAGIC, i)
        if magic < 0:
            break
        if magic + 4 > len(buf):
            break
        payload_len = struct.unpack_from("<H", buf, magic + 2)[0]
        if payload_len == 0 or payload_len > VCHAN_MAX_PAYLOAD:
            i = magic + 1
            continue
        frame_len = 4 + payload_len + 4
        if magic + frame_len > len(buf):
            break
        checksum = struct.unpack_from("<I", buf, magic + 4 + payload_len)[0]
        if checksum != VCHAN_CHECKSUM:
            i = magic + 1
            continue
        out.append(buf[magic + 4:magic + 4 + payload_len])
        i = magic + frame_len
    return out


def do_ping(cmd_dev, baud):
    ser = open_serial(cmd_dev, baud)
    ser.reset_input_buffer()
    ser.write(vchan_frame(b"PING"))
    ser.flush()
    time.sleep(0.2)
    resp = ser.read(256)
    ser.close()
    payloads = vchan_payloads_from(resp)
    if b"PONG" in payloads:
        print(f"{cmd_dev}: vchan PONG -> CMD ACM ok")
        return True
    print(f"{cmd_dev}: no vchan PONG (raw={resp!r}, payloads={payloads!r})")
    return False


def main():
    ap = argparse.ArgumentParser(description="Smart-lock USB video streamer")
    ap.add_argument("--video", help="input video file (e.g. ../video_ls.mp4)")
    ap.add_argument("--data-dev", help="DATA CDC-ACM device, e.g. /dev/ttyACM1")
    ap.add_argument("--data-interface", default=DEFAULT_DATA_INTERFACE,
                    help=f"auto-detect DATA ACM by USB interface number (default {DEFAULT_DATA_INTERFACE})")
    ap.add_argument("--list-devs", action="store_true", help="list /dev/ttyACM* interface numbers and exit")
    ap.add_argument("--cmd-dev", help="CMD CDC-ACM device, e.g. /dev/ttyACM0")
    ap.add_argument("--cmd-interface", default=DEFAULT_CMD_INTERFACE,
                    help=f"auto-detect CMD ACM by USB interface number (default {DEFAULT_CMD_INTERFACE})")
    ap.add_argument("--width", type=int, default=480, help="scaled width (default 480)")
    ap.add_argument("--height", type=int, default=960, help="scaled height (default 960)")
    ap.add_argument("--fps", type=int, default=24, help="frames per second (default 24)")
    ap.add_argument("--quality", type=int, default=4, help="MJPEG q:v 2(best)-31 (default 4)")
    ap.add_argument("--baud", type=int, default=2000000, help="nominal baud (ignored by CDC-ACM)")
    ap.add_argument("--no-pace", action="store_true",
                    help="do not throttle Python writes; useful only for throughput tests")
    ap.add_argument("--loop", action="store_true", help="loop the video forever")
    ap.add_argument("--ping", action="store_true", help="probe --cmd-dev for PONG and exit")
    args = ap.parse_args()

    if args.list_devs:
        print_acm_devices()
        return

    if args.ping:
        cmd_dev = select_cmd_dev(args.cmd_dev, args.cmd_interface)
        sys.exit(0 if do_ping(cmd_dev, args.baud) else 1)

    if not args.video:
        sys.exit("need --video (or use --list-devs/--ping)")

    data_dev = select_data_dev(args.data_dev, args.data_interface)

    try:
        data_ser = open_serial(data_dev, args.baud)
    except serial.SerialException as exc:
        sys.exit(f"open {data_dev} failed: {exc}")

    if args.cmd_dev:
        try:
            cmd_ser = open_serial(args.cmd_dev, args.baud)
            cmd_ser.write(vchan_frame(b"START"))
            cmd_ser.flush()
            cmd_ser.close()
            print(f"sent START on {args.cmd_dev}")
        except serial.SerialException as exc:
            data_ser.close()
            sys.exit(f"open/write {args.cmd_dev} failed: {exc}")

    cmd = build_ffmpeg_cmd(args.video, args.width, args.height, args.fps,
                           args.quality, args.loop)
    print(f"data ACM: {data_dev} (interface {tty_interface_num(data_dev) or 'unknown'})")
    print("ffmpeg:", " ".join(cmd))
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)

    frame_no = 0
    idx = 0
    t0 = time.time()
    next_send = time.monotonic()
    frame_period = 1.0 / args.fps if args.fps > 0 else 0.0
    bytes_sent = 0
    exit_code = 0
    try:
        for jpeg in iter_jpeg_frames(proc.stdout):
            jpeg = split_multi_table_dht_segments(jpeg)
            frame_no += 1
            idx = (idx + 1) & 0xFFFF

            if not args.no_pace and frame_period > 0.0:
                now = time.monotonic()
                if next_send > now:
                    time.sleep(next_send - now)
                elif now - next_send > frame_period:
                    next_send = now

            header = struct.pack("<IIHH", FRAME_MAGIC, len(jpeg), idx, 0)
            try:
                data_ser.write(header)
                data_ser.write(jpeg)
            except (serial.SerialException, OSError) as exc:
                print(f"\nDATA ACM write failed on {data_dev}: {exc}", file=sys.stderr)
                try:
                    data_ser.close()
                except (serial.SerialException, OSError):
                    pass
                data_ser, data_dev = wait_for_data_serial(data_dev, args.data_interface, args.baud)
                next_send = time.monotonic()
                continue
            bytes_sent += len(header) + len(jpeg)

            if not args.no_pace and frame_period > 0.0:
                next_send += frame_period

            if frame_no % args.fps == 0:
                dt = time.time() - t0
                print(f"\rframe {frame_no}  {frame_no/max(dt,1e-3):.1f} fps  {len(jpeg)//1024}KB/f  "
                      f"{bytes_sent/1024/max(dt,1e-3):.0f} KB/s", end="", flush=True)
    except KeyboardInterrupt:
        print("\ninterrupted")
    except OSError as exc:
        print(f"\nvideo stream failed: {exc}", file=sys.stderr)
        exit_code = 2
    finally:
        proc.terminate()
        try:
            proc.wait(timeout=2)
        except subprocess.TimeoutExpired:
            proc.kill()
        try:
            data_ser.close()
        except (serial.SerialException, OSError):
            pass
        print(f"\ndone: {frame_no} frames, {bytes_sent/1024/1024:.1f} MB")
    sys.exit(exit_code)


if __name__ == "__main__":
    main()
