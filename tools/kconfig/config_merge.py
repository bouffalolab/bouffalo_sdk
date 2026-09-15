#!/usr/bin/env python3
"""Print and append configuration changes between two Kconfig files."""

import argparse
import re
import sys


ASSIGN_RE = re.compile(r"^\s*(CONFIG_[A-Za-z0-9_]+)\s*=\s*(.*?)\s*$")
NOT_SET_RE = re.compile(r"^\s*#\s*(CONFIG_[A-Za-z0-9_]+) is not set\s*$")


def parse_config(path):
    values = {}
    with open(path, encoding="utf-8") as stream:
        for line in stream:
            line = line.rstrip("\n")
            match = NOT_SET_RE.match(line)
            if match:
                name = match.group(1)
                values[name] = ("n", "{}=n".format(name))
                continue
            match = ASSIGN_RE.match(line)
            if match:
                values[match.group(1)] = (match.group(2), line.strip())
    return values


def changed_entries(before_path, after_path):
    before = parse_config(before_path)
    after = parse_config(after_path)
    return [
        line
        for name, (value, line) in after.items()
        if name not in before or before[name][0] != value
    ]


def append_entries(path, entries):
    if not entries:
        return

    try:
        with open(path, "r", encoding="utf-8") as stream:
            contents = stream.read()
    except FileNotFoundError:
        contents = ""

    with open(path, "a", encoding="utf-8") as stream:
        if contents and not contents.endswith("\n"):
            stream.write("\n")
        for entry in entries:
            stream.write(entry + "\n")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--before", required=True, help="Pre-menuconfig autoconfig.def")
    parser.add_argument("--after", required=True, help="Post-menuconfig autoconfig.def")
    parser.add_argument("--defconfig", required=True, help="Project defconfig to append")
    args = parser.parse_args()

    try:
        entries = changed_entries(args.before, args.after)
    except OSError as error:
        print("Error: {}".format(error), file=sys.stderr)
        return 1

    print("New or changed configuration entries:")
    if entries:
        try:
            append_entries(args.defconfig, entries)
        except OSError as error:
            print("Error: {}".format(error), file=sys.stderr)
            return 1
        for entry in entries:
            print(entry)
    else:
        print("(none)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
