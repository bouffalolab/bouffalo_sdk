#!/usr/bin/env python3
"""Generate the user_kv LittleFS image from a manufacturing data CSV."""

import argparse
import base64
import csv
import platform
import re
import struct
import subprocess
import sys
import tempfile
from pathlib import Path


def factory_files(path, root):
    types = {"u8": 1, "i8": 2, "u16": 3, "i16": 4, "u32": 5, "i32": 6,
             "string": 7, "binary": 8, "hex2bin": 8, "base64": 8}
    namespaces = {}
    namespace = None
    with path.open(encoding="utf-8", newline="") as source:
        for row in csv.DictReader(source):
            key = row["key"]
            if not re.fullmatch(r"[A-Za-z0-9_][A-Za-z0-9_.]{0,14}", key):
                raise ValueError(f"Invalid factory name: {key}")
            if row["type"] == "namespace":
                namespace = namespaces.setdefault(key, bytearray())
                continue
            if row["type"] not in ("data", "file") or namespace is None:
                raise ValueError(f"Invalid entry or missing namespace: {key}")
            encoding, value = row["encoding"], row["value"]
            if row["type"] == "file":
                if encoding != "binary":
                    raise ValueError(f"File requires binary encoding: {key}")
                data = (path.parent / value).read_bytes()
            elif encoding == "string":
                data = value.encode("utf-8")
            elif encoding == "hex2bin":
                data = bytes.fromhex(value)
            elif encoding == "base64":
                data = base64.b64decode(value, validate=True)
            else:
                number = int(value, 10)
                bits = int(encoding[1:])
                minimum, maximum = 0, (1 << bits) - 1
                if encoding[0] == "i":
                    minimum, maximum = -(1 << (bits - 1)), (1 << (bits - 1)) - 1
                if not minimum <= number <= maximum:
                    raise ValueError(f"{key} is out of {encoding} range")
                data = str(number).encode("utf-8")
            namespace.extend(struct.pack("<16sII", key.encode(), types[encoding], len(data)))
            namespace.extend(data)
    for name, data in namespaces.items():
        (root / name).write_bytes(data)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sdk", required=True, type=Path)
    parser.add_argument("--primary-table", required=True, type=Path)
    parser.add_argument("--factory", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()
    sys.path.insert(0, str(args.sdk / "tools/pylib"))
    import toml

    entry = next(entry for entry in toml.load(args.primary_table)["pt_entry"] if entry["name"] == "user_kv")
    suffix = {"Linux": "-ubuntu", "Darwin": "-macos", "Windows": ".exe"}[platform.system()]
    mklfs = (args.sdk / "tools/genlfs" / ("mklfs" + suffix)).resolve()
    args.output.parent.mkdir(parents=True, exist_ok=True)

    with tempfile.TemporaryDirectory(dir=args.output.parent) as directory:
        work = Path(directory)
        factory = work / "factory"
        factory.mkdir()
        factory_files(args.factory, factory)
        subprocess.run([str(mklfs), "-c", factory.name, "-b", "4096", "-p", "256",
                        "-r", "256", "-s", str(entry["size0"]), "-i", str(args.output.resolve())],
                       cwd=work, check=True)


if __name__ == "__main__":
    main()
