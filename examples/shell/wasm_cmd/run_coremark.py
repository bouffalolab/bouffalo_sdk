#!/usr/bin/env python3
import argparse
import re
import sys
import time

import serial


PROMPT = b"bouffalolab />"
MODES = ("direct", "interp", "aot", "aot-fast", "aot-dynamic",
         "aot-dynamic-fast", "aot-xip", "aot-xip-fast")


def read_until(port, marker, timeout):
    data = bytearray()
    deadline = time.monotonic() + timeout

    while time.monotonic() < deadline:
        chunk = port.read(port.in_waiting or 1)
        if chunk:
            data.extend(chunk)
            print(chunk.decode(errors="replace"), end="", flush=True)
            if marker in data:
                return bytes(data)
    received = data.decode(errors="replace")
    raise TimeoutError(
        f"timed out waiting for {marker!r}; received {received!r}"
    )


def main():
    parser = argparse.ArgumentParser(description="Run wasm_cmd CoreMark modes")
    parser.add_argument("--port", default="/dev/ttyACM0")
    parser.add_argument("--baud", type=int, default=2000000)
    parser.add_argument("--timeout", type=float, default=90.0)
    parser.add_argument("--mode", action="append", choices=MODES)
    parser.add_argument("--interrupt", action="store_true",
                        help="send Ctrl+C before waiting for the shell prompt")
    args = parser.parse_args()

    modes = args.mode or list(MODES)
    port = serial.Serial()
    port.port = args.port
    port.baudrate = args.baud
    port.timeout = 0.2
    port.write_timeout = 2
    port.rtscts = False
    port.dsrdtr = False
    port.dtr = True
    port.rts = False

    results = {}
    with port:
        if args.interrupt:
            port.write(b"\x03")
            port.flush()
            time.sleep(0.5)
            port.reset_input_buffer()
        try:
            read_until(port, PROMPT, 20)
        except TimeoutError:
            port.write(b"\r\n")
            read_until(port, PROMPT, 10)

        for mode in modes:
            command = f"wasm_cmd coremark {mode}\r".encode()
            print(f"\n===== {mode}: {command.decode().strip()} =====", flush=True)
            port.reset_input_buffer()
            port.write(command)
            output = read_until(port, PROMPT, args.timeout)
            text = output.decode(errors="replace")

            score = re.search(
                rf"CoreMark 1\.0 \[{mode}\] : ([0-9]+\.[0-9]+)", text
            )
            stats = re.search(
                rf"CoreMark \[{mode}\]: iterations=(\d+) elapsed_us=(\d+)", text
            )
            crc = re.search(
                rf"CRC \[{mode}\]: final=0x([0-9a-fA-F]+).* error=0x([0-9a-fA-F]+) (PASS|FAIL)",
                text,
            )
            results[mode] = {
                "score": score.group(1) if score else None,
                "iterations": stats.group(1) if stats else None,
                "elapsed_us": stats.group(2) if stats else None,
                "crc": crc.group(1) if crc else None,
                "error": crc.group(2) if crc else None,
                "status": crc.group(3) if crc else "NO RESULT",
            }

    print("\n===== summary =====")
    for mode in modes:
        result = results[mode]
        print(
            f"{mode:6s} score={result['score'] or '-':>10s} "
            f"iterations={result['iterations'] or '-':>8s} "
            f"elapsed_us={result['elapsed_us'] or '-':>9s} "
            f"crc={result['crc'] or '-':>4s} "
            f"status={result['status']}"
        )

    return 0 if all(result["status"] == "PASS" for result in results.values()) else 1


if __name__ == "__main__":
    sys.exit(main())
