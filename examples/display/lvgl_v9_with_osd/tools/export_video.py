#!/usr/bin/env python3
"""Offline SD exporter; JPEG framing/DHT normalization from the original streamer.

Requires Python 3 and ffmpeg only. Never opens a serial port or accesses a board.
"""
import argparse
from pathlib import Path
import struct
import subprocess

SOI = b"\xff\xd8"
EOI = b"\xff\xd9"


def iter_jpeg_frames(stream):
    """Yield complete SOI..EOI frames from ffmpeg's MJPEG stream."""
    buf = bytearray()
    while True:
        chunk = stream.read(65536)
        if not chunk:
            break
        buf += chunk
        while True:
            start = buf.find(SOI)
            if start < 0:
                if len(buf) > 1:
                    del buf[:-1]
                break
            end = buf.find(EOI, start + 2)
            if end < 0:
                if start > 0:
                    del buf[:start]
                break
            end += 2
            yield bytes(buf[start:end])
            del buf[:end]
    if buf:
        raise ValueError("Incomplete JPEG at end of ffmpeg output")


def split_multi_table_dht_segments(jpeg):
    """One FFC4 segment per Huffman table, as required by the LHAL parser.

    Preserve the original streamer's table splitting, not JPEG entropy coding.
    ffmpeg must also use -huffman default (not optimized table sizes).
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
    return bytes(out) if changed else jpeg


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("video", type=Path)
    parser.add_argument("sd_root", type=Path, help="staging directory or mounted SD root")
    parser.add_argument("--fps", type=int, default=100, help="match VIDEO_TARGET_FPS (default 100)")
    parser.add_argument("--quality", type=int, default=8, help="ffmpeg q:v, 2..31; larger means smaller")
    parser.add_argument("--frames", type=int, default=10000, help="maximum frames, 1..10000")
    args = parser.parse_args()
    if args.fps <= 0 or not 2 <= args.quality <= 31 or not 1 <= args.frames <= 10000:
        parser.error("fps must be positive, quality 2..31, frames 1..10000")
    if not args.video.is_file():
        parser.error("input video does not exist")
    target = args.sd_root / "1024x600"
    if target.exists():
        parser.error(f"{target} already exists; use a fresh directory to avoid stale frames")
    cmd = ["ffmpeg", "-nostdin", "-hide_banner", "-loglevel", "error",
           "-i", str(args.video), "-an", "-vf", "scale=1024:600", "-r", str(args.fps),
           "-frames:v", str(args.frames), "-f", "image2pipe", "-c:v", "mjpeg",
           "-huffman", "default", "-pix_fmt", "yuvj420p", "-q:v", str(args.quality), "pipe:1"]
    count = 0
    try:
        with subprocess.Popen(cmd, stdout=subprocess.PIPE) as proc:
            try:
                for count, frame in enumerate(iter_jpeg_frames(proc.stdout), 1):
                    frame = split_multi_table_dht_segments(frame)
                    if len(frame) > 128 * 1024:
                        raise ValueError(f"frame {count}: {len(frame)} > 128 KiB; increase --quality")
                    folder = target / f"{(count - 1) // 100:03d}"
                    folder.mkdir(parents=True, exist_ok=True)
                    (folder / f"p{count:04d}.jpg").write_bytes(frame)
            except BaseException:
                proc.terminate()
                raise
            finally:
                proc.stdout.close()
            if proc.wait():
                raise ValueError("ffmpeg failed")
        if not count:
            raise ValueError("no video frames exported")
    except (OSError, ValueError) as exc:
        parser.exit(1, f"Export failed: {exc}. Discard any partial 1024x600 directory.\n")
    print(f"Exported {count} baseline 4:2:0 frames to {target}")


if __name__ == "__main__":
    main()