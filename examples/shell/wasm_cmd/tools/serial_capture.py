#!/usr/bin/env python3
"""Capture a short, readable boot trace from the bouffalolab console."""

import argparse
import re
import sys
import time

import serial


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", default="/dev/ttyACM0")
    parser.add_argument("--baud", type=int, default=2000000)
    parser.add_argument("--seconds", type=float, default=12.0)
    args = parser.parse_args()

    with serial.Serial(args.port, args.baud, timeout=0.2,
                       dsrdtr=False, rtscts=False, exclusive=True) as port:
        port.dtr = True
        port.rts = False
        time.sleep(0.5)
        port.reset_input_buffer()
        port.write(b"\r")
        deadline = time.monotonic() + args.seconds
        data = bytearray()
        stop_markers = (b"BFLB COREDUMP", b"------ DATA BEGIN")
        marker_pos = -1
        while time.monotonic() < deadline:
            chunk = port.read(port.in_waiting or 1)
            if chunk:
                data.extend(chunk)
                positions = [data.find(item) for item in stop_markers]
                positions = [pos for pos in positions if pos >= 0]
                if positions:
                    marker_pos = min(positions)
                    break
        pre = bytes(data if marker_pos < 0 else data[:marker_pos])
        text = pre.decode(errors="replace")
        lines = text.replace("\r", "").splitlines()
        short_lines = [line for line in lines if len(line) <= 240]
        print(f"captured={len(data)} bytes, lines={len(lines)}, coredump={marker_pos >= 0}")
        for line in short_lines[-120:]:
            print(line)

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, TimeoutError, serial.SerialException) as exc:
        print(f"serial capture failed: {exc}", file=sys.stderr)
        raise SystemExit(1)
