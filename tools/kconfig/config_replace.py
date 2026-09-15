#!/usr/bin/env python3
"""Apply the SDK's single CONFIG_* name mapping at build boundaries."""

import argparse
import re
from pathlib import Path


REPLACEMENTS = {
    "CHIP": "CONFIG_CHIP",
    "BOARD": "CONFIG_BOARD",
    "CPU_ID": "CONFIG_CPU_ID",
    "CPU_MODEL": "CONFIG_CPU_MODEL",
    "ARCH_RISCV": "CONFIG_ARCH_RISCV",
    "BOUFFALO_SDK": "CONFIG_BOUFFALO_SDK",
    "BFLB_USE_HAL_DRIVER": "CONFIG_BFLB_USE_HAL_DRIVER",
}
CONFIG_TO_OUTPUT = {value: key for key, value in REPLACEMENTS.items()}
SET_RE = re.compile(r"^(\s*set\(\s*)([A-Za-z_][A-Za-z0-9_]*)(\s+.*)$")
SET_VALUE_RE = re.compile(r"^\s*set\(\s*([A-Za-z_][A-Za-z0-9_]*)\s+(.*?)\s*\)\s*$")


def config_name(name):
    return REPLACEMENTS.get(name, name)


def output_name(name):
    return CONFIG_TO_OUTPUT.get(name, name)


def replace_cmake(input_path, output_path):
    lines = []
    with open(input_path, encoding="utf-8") as stream:
        for line in stream:
            match = SET_RE.match(line.rstrip("\n"))
            if match:
                line = "{}{}{}\n".format(match.group(1), output_name(match.group(2)), match.group(3))
            lines.append(line)
    Path(output_path).write_text("".join(lines), encoding="utf-8")


def parse_cmake_vars(path):
    values = {}
    with open(path, encoding="utf-8") as stream:
        for line in stream:
            match = SET_VALUE_RE.match(line)
            if match:
                values[match.group(1)] = match.group(2)
    return values


def c_string(value):
    return '"{}"'.format(value.strip('"').replace("\\", "\\\\").replace('"', '\\"'))


def define(lines, name, value):
    if value == "y":
        lines.append("#define {} 1\n".format(name))
    elif value == "n":
        lines.append("#undef {}\n".format(name))
    elif re.fullmatch(r"(?:0[xX])?[0-9a-fA-F]+", value):
        lines.append("#define {} {}\n".format(name, value))
    else:
        lines.append("#define {} {}\n".format(name, c_string(value)))


def replace_header(input_path, output_path):
    values = parse_cmake_vars(input_path)
    lines = [
        "#define _ZERO_WITH_COMMA_1 0,\n",
        "#define _second_arg(__ignored, val, ...) val\n",
        "#define _is_enabled(x)                __is_enabled(x)\n",
        "#define __is_enabled(val)             ___is_enabled(_ZERO_WITH_COMMA_##val)\n",
        "#define ___is_enabled(junk_or_comma)  _second_arg(junk_or_comma 1, 0)\n",
        "#define IS_ENABLED(config)            _is_enabled(config)\n",
    ]
    for name in sorted(values):
        if name.startswith("CONFIG_"):
            define(lines, name, values[name])
        elif name in REPLACEMENTS:
            lines.append("#define {} {}\n".format(name, c_string(values[name])))
    for value_name, macro in (("CHIP", "chip"), ("CPU_ID", "cpu"), ("CPU_MODEL", "model")):
        value = values.get(value_name, "").strip('"')
        if value:
            prefix = "CPU_" if macro == "cpu" else "CPU_MODEL_" if macro == "model" else ""
            lines.append("#define {}{} 1\n".format(prefix, value.upper()))
    Path(output_path).write_text("".join(lines), encoding="utf-8")


def main():
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(dest="command", required=True)
    cmake = subparsers.add_parser("cmake")
    cmake.add_argument("--input", required=True)
    cmake.add_argument("--output", required=True)
    header = subparsers.add_parser("header")
    header.add_argument("--input", required=True)
    header.add_argument("--output", required=True)
    args = parser.parse_args()
    if args.command == "cmake":
        replace_cmake(args.input, args.output)
    else:
        replace_header(args.input, args.output)


if __name__ == "__main__":
    main()
