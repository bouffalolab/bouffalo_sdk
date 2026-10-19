#!/usr/bin/env python3
"""Find RISC-V tools used by the BYAI tools.

The SDK may be used with either an ``riscv64-unknown-*`` or an
``riscv64-zephyr-*`` toolchain.  Each requested tool is searched separately
in PATH, so different tools may come from different toolchain installations.
"""

import argparse
import os
import re
import shutil
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, Iterable


_TOOL_COMPONENTS = frozenset({
    "ar", "as", "addr2line", "cpp", "gcc", "g++", "gdb", "gcov",
    "ld", "nm", "objcopy", "objdump", "ranlib", "readelf", "size", "strip",
})


@dataclass(frozen=True)
class Toolchain:
    """A discovered RISC-V toolchain.

    ``prefix`` includes the toolchain directory and the trailing dash, for
    example ``/opt/toolchains/riscv64-unknown-elf-``.
    """

    prefix: str

    def tool(self, name: str) -> str:
        """Return an executable from this specific toolchain prefix."""
        executable = self.prefix + name
        if os.name == "nt":
            executable += ".exe"

        resolved = shutil.which(executable)
        if resolved is None:
            raise FileNotFoundError(
                f"Tool '{name}' was not found for toolchain prefix '{self.prefix}'"
            )
        return resolved


def _path_entries() -> Iterable[Path]:
    for entry in os.environ.get("PATH", "").split(os.pathsep):
        yield Path(entry or os.curdir)


def find_tool(name: str) -> str:
    """Find one tool independently in PATH.

    The search follows PATH order and accepts both supported RISC-V prefix
    families.  A tool is not required to be installed beside the other tools.
    """
    if not name or os.path.basename(name) != name:
        raise ValueError(f"Invalid tool name: {name!r}")

    pattern = re.compile(
        rf"^riscv64-(?:unknown|zephyr)-.+-{re.escape(name)}(?:\.exe)?$"
    )
    for directory in _path_entries():
        try:
            entries = sorted(directory.iterdir(), key=lambda path: path.name)
        except OSError:
            continue

        for entry in entries:
            if not entry.is_file() and not entry.is_symlink():
                continue
            if not pattern.match(entry.name):
                continue

            # Do not treat wrappers such as riscv64-unknown-elf-gcc-nm as
            # the real riscv64-unknown-elf-nm executable.
            prefix_without_tool = entry.name[:-(len(name) + 1)]
            if prefix_without_tool.rsplit("-", 1)[-1] in _TOOL_COMPONENTS:
                continue

            resolved = shutil.which(str(entry))
            if resolved is not None:
                return resolved

    raise FileNotFoundError(
        f"No RISC-V tool '{name}' found in PATH. Expected a tool matching "
        f"riscv64-unknown-*-{name} or riscv64-zephyr-*-{name}."
    )


def find_toolchain(tool: str = "gcc") -> Toolchain:
    """Return the prefix associated with the first matching ``tool``."""
    executable = find_tool(tool)
    suffix = f"{tool}.exe" if executable.endswith(".exe") else tool
    return Toolchain(executable[:-len(suffix)])


def get_tool(name: str) -> str:
    """Find a named executable independently in PATH."""
    return find_tool(name)


def _main() -> int:
    parser = argparse.ArgumentParser(description="Find the RISC-V toolchain in PATH")
    parser.add_argument(
        "tool",
        nargs="*",
        default=["gcc", "nm", "objdump", "gdb"],
        help="tool names to resolve (default: gcc nm objdump gdb)",
    )
    args = parser.parse_args()

    resolved: Dict[str, str] = {}
    for name in args.tool:
        try:
            resolved[name] = get_tool(name)
        except FileNotFoundError as error:
            print(f"{name}: unavailable ({error})")
        else:
            print(f"{name}: {resolved[name]}")
    return 0


if __name__ == "__main__":
    raise SystemExit(_main())
