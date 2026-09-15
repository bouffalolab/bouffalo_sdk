#!/usr/bin/env python3
"""Resolve a project defconfig and emit all build-system configuration files."""

import argparse
import os
import re
import sys
import tempfile


ASSIGN_RE = re.compile(
    r"^\s*(CONFIG_[A-Za-z0-9_]+|CHIP|BOARD|CPU_ID|CPU_MODEL)\s*=\s*(.*?)\s*$"
)
NOT_SET_RE = re.compile(r"^\s*#\s*(CONFIG_[A-Za-z0-9_]+) is not set\s*$")
LEGACY_RE = re.compile(
    r"^\s*(?:-?include\b|ifeq\b|ifneq\b|else\b|endif\b|override\b|\$\(error)"
)
OVERRIDE_NAME_RE = re.compile(r"^(?:CONFIG_[A-Za-z0-9_]+|CHIP|BOARD|CPU_ID|CPU_MODEL)$")


def config_name(name):
    from config_replace import config_name as replace_name

    return replace_name(name)


def parse_overrides(assignments):
    values = {}
    for assignment in assignments:
        raw_name, separator, value = assignment.partition("=")
        if not separator or not OVERRIDE_NAME_RE.fullmatch(raw_name):
            raise ValueError(
                "invalid command-line override {!r}; expected CONFIG_NAME=value "
                "or one of CHIP/BOARD/CPU_ID/CPU_MODEL=value".format(assignment)
            )
        name = config_name(raw_name)
        values[name] = value
    return values


def normalize_defconfig(path):
    values = {}
    with open(path, encoding="utf-8") as stream:
        for line_number, raw_line in enumerate(stream, 1):
            line = raw_line.rstrip("\n")
            match = NOT_SET_RE.match(line)
            if match:
                name = match.group(1)
                values[name] = "n"
                continue
            match = ASSIGN_RE.match(line)
            if match:
                name, value = match.groups()
                name = config_name(name)
                values[name] = value
                continue
            if LEGACY_RE.match(line):
                raise ValueError(
                    "{}:{}: Make syntax is not allowed in defconfig; move this "
                    "condition or include to Kconfig".format(path, line_number)
                )
            if line.strip() and not line.lstrip().startswith("#"):
                raise ValueError(
                    "{}:{}: invalid defconfig line: {}".format(
                        path, line_number, line.strip()
                    )
                )
    return values


def set_selector_environment(values):
    for config_key, environment_key in (
        ("CONFIG_CHIP", "CHIP"),
        ("CONFIG_BOARD", "BOARD"),
        ("CONFIG_CPU_ID", "CPU_ID"),
        ("CONFIG_CPU_MODEL", "CPU_MODEL"),
    ):
        if config_key in values:
            os.environ[environment_key] = values[config_key].strip("\"'")


def load_kconfig(kconfig_file):
    kconfig_dir = os.path.dirname(os.path.abspath(__file__))
    if kconfig_dir not in sys.path:
        sys.path.insert(0, kconfig_dir)
    from kconfiglib import Kconfig

    return Kconfig(kconfig_file, warn_to_stderr=False, suppress_traceback=True)


def write_fragment(lines):
    handle = tempfile.NamedTemporaryFile(
        "w", suffix=".conf", delete=False, encoding="utf-8"
    )
    try:
        handle.writelines(lines)
    finally:
        handle.close()
    return handle.name


def config_fragment(kconf, values):
    from kconfiglib import STRING

    lines = []
    for name, value in values.items():
        if value == "n":
            lines.append("# {} is not set\n".format(name))
        else:
            symbol = kconf.syms.get(name[7:]) if name.startswith("CONFIG_") else None
            if symbol is not None and symbol.type == STRING:
                value = value.strip("\"'")
                value = value.replace("\\", "\\\\").replace('"', '\\"')
                value = '"{}"'.format(value)
            lines.append("{}={}\n".format(name, value))
    return write_fragment(lines)


def resolved_values(kconf):
    from kconfiglib import BOOL, STRING, TRISTATE

    values = {}
    for symbol in sorted(kconf.unique_defined_syms, key=lambda item: item.name):
        if not symbol.config_string:
            continue
        name = "CONFIG_" + symbol.name
        if symbol.type in (BOOL, TRISTATE):
            values[name] = "y" if symbol.str_value == "y" else "n"
        elif symbol.type == STRING:
            escaped = symbol.str_value.replace("\\", "\\\\").replace('"', '\\"')
            values[name] = '"{}"'.format(escaped)
        else:
            values[name] = symbol.str_value
    return values


def write_if_changed(output, content):
    try:
        with open(output, encoding="utf-8") as stream:
            if stream.read() == content:
                return
    except FileNotFoundError:
        pass

    with open(output, "w", encoding="utf-8") as stream:
        stream.write(content)


def write_cmake(output, values):
    from config_replace import output_name

    lines = ["# Auto-generated Kconfig configuration", "# Do not edit manually", ""]
    lines.extend(
        "set({} {})".format(output_name(name), value)
        for name, value in values.items()
    )
    write_if_changed(output, "\n".join(lines) + "\n")


def write_defconfig(output, values):
    lines = ["# Auto-generated Kconfig configuration", "# Do not edit manually", ""]
    for name, value in values.items():
        if value == "n":
            lines.append("# {} is not set".format(name))
        else:
            lines.append("{}={}".format(name, value))
    write_if_changed(output, "\n".join(lines) + "\n")


def write_make(output, values):
    lines = ["# Auto-generated Kconfig configuration", "# Do not edit manually", ""]
    for name, value in values.items():
        if value in ("n", "", '""'):
            continue
        make_value = value.strip('"').replace("$", "$$").replace("#", "\\#")
        lines.append("{} := {}".format(name, make_value))
    write_if_changed(output, "\n".join(lines) + "\n")


def write_header(output, values):
    from config_replace import define, output_name

    lines = [
        "#define _ZERO_WITH_COMMA_1 0,\n",
        "#define _second_arg(__ignored, val, ...) val\n",
        "#define _is_enabled(x)                __is_enabled(x)\n",
        "#define __is_enabled(val)             ___is_enabled(_ZERO_WITH_COMMA_##val)\n",
        "#define ___is_enabled(junk_or_comma)  _second_arg(junk_or_comma 1, 0)\n",
        "#define IS_ENABLED(config)            _is_enabled(config)\n",
    ]
    for name, value in values.items():
        define(lines, output_name(name), value)
    for value_name, prefix in (
        ("CONFIG_CHIP", ""),
        ("CONFIG_CPU_ID", "CPU_"),
        ("CONFIG_CPU_MODEL", "CPU_MODEL_"),
    ):
        value = values.get(value_name, "").strip('"')
        if value:
            lines.append("#define {}{} 1\n".format(prefix, value.upper()))
    write_if_changed(output, "".join(lines))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--defconfig", required=True)
    parser.add_argument("--kconfig", required=True)
    parser.add_argument("--def-out", required=True)
    parser.add_argument("--cmake-out", required=True)
    parser.add_argument("--header-out", required=True)
    parser.add_argument("--make-out", required=True)
    parser.add_argument("overrides", nargs="*", metavar="NAME=VALUE")
    args = parser.parse_args()

    try:
        defconfig_values = normalize_defconfig(args.defconfig)
        command_values = parse_overrides(args.overrides)
    except (OSError, ValueError) as error:
        print("Error: {}".format(error), file=sys.stderr)
        return 1

    selectors = dict(defconfig_values)
    selectors.update(command_values)
    set_selector_environment(selectors)

    kconf = load_kconfig(args.kconfig)
    kconf.warn_assign_override = False
    kconf.warn_assign_redun = False
    kconf.warn_assign_undef = True

    base_fragment = config_fragment(kconf, defconfig_values)
    override_fragment = config_fragment(kconf, command_values)
    try:
        kconf.load_config(base_fragment, replace=True)
        kconf.load_config(override_fragment, replace=False)
        kconf.write_config(os.devnull, save_old=False)
        if kconf.warnings:
            for warning in kconf.warnings:
                print(warning, file=sys.stderr)
            return 1
        values = resolved_values(kconf)
        write_defconfig(args.def_out, values)
        write_cmake(args.cmake_out, values)
        write_header(args.header_out, values)
        write_make(args.make_out, values)
    finally:
        os.unlink(base_fragment)
        os.unlink(override_fragment)

    print("Generated {}, {}, {} and {}".format(
        args.def_out, args.cmake_out, args.header_out, args.make_out
    ))
    return 0


if __name__ == "__main__":
    sys.exit(main())
