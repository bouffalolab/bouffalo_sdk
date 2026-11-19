#!/usr/bin/env python3
"""Run a short sequence of commands on the bouffalolab shell."""

import argparse
import sys
import time

import serial


PROMPT = b"bouffalolab />"


def read_until(port, marker, timeout):
    deadline = time.monotonic() + timeout
    data = bytearray()
    while time.monotonic() < deadline:
        chunk = port.read(port.in_waiting or 1)
        if not chunk:
            continue
        data.extend(chunk)
        if marker in data:
            return bytes(data)
    tail = bytes(data[-1024:])
    raise TimeoutError(
        f"timed out waiting for {marker!r}; bytes={len(data)} tail={tail!r}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", default="/dev/ttyACM0")
    parser.add_argument("--baud", type=int, default=2000000)
    parser.add_argument("--timeout", type=float, default=60.0)
    parser.add_argument("command", nargs="+")
    args = parser.parse_args()

    with serial.Serial(args.port, args.baud, timeout=0.2,
                       dsrdtr=False, rtscts=False, exclusive=True) as port:
        port.dtr = True
        port.rts = False
        port.write(b"\r")
        boot = read_until(port, PROMPT, 20.0)
        print(f"boot tail: {boot[-256:]!r}")
        for command in args.command:
            print(f"\n>>> {command}")
            time.sleep(0.2)
            port.reset_input_buffer()
            port.write(command.encode() + b"\r")
            port.flush()
            output = read_until(port, PROMPT, args.timeout)
            print(output.decode(errors="replace"))
            print()
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, TimeoutError, serial.SerialException) as exc:
        print(f"serial test failed: {exc}", file=sys.stderr)
        raise SystemExit(1)
