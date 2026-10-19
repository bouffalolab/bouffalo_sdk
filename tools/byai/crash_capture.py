#!/usr/bin/env python3
"""
Split multiple BFLB coredumps from a single log file.

This rewrites the behavior of tools/crash_tools/crash_capture.pl in Python and
reuses the same encoding and fixed-prefix handling strategy as tools/byai/coredump.py.

Usage:
    python3 tools/byai/crash_capture.py <log_file> [-o output_dir]
"""

import argparse
import base64
import binascii
import re
import sys
import zlib
from dataclasses import dataclass
from pathlib import Path
from typing import List, Optional


HEADER_TEXT = "-+-+-+- BFLB COREDUMP v0.0.1 +-+-+-+"
HEADER_LINE = HEADER_TEXT + "\n\n"
BASE64_CHARS = set("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=")


def detect_and_read_file(log_file: Path) -> str:
    """Detect file encoding and return decoded text."""
    raw_data = log_file.read_bytes()
    utf16le_marker = "COREDUMP".encode("utf-16-le")
    utf16be_marker = "COREDUMP".encode("utf-16-be")
    candidates = []

    if raw_data.startswith(b"\xff\xfe"):
        candidates.append("utf-16-le")
    elif raw_data.startswith(b"\xfe\xff"):
        candidates.append("utf-16-be")

    if raw_data.find(utf16le_marker) != -1:
        candidates.append("utf-16-le")
    if raw_data.find(utf16be_marker) != -1:
        candidates.append("utf-16-be")

    candidates.extend(["utf-8", "utf-16", "latin-1"])

    seen = set()
    decoded_candidates = []
    for encoding in candidates:
        if encoding in seen:
            continue
        seen.add(encoding)
        try:
            if encoding == "utf-8":
                cleaned = re.sub(rb"\n\[[^\]]*\]\n", b"\n", raw_data)
                decoded = cleaned.decode("utf-8")
            else:
                decoded = raw_data.decode(encoding)
            decoded_candidates.append(decoded)
            if "COREDUMP" in decoded:
                return decoded
        except UnicodeDecodeError:
            continue

    if decoded_candidates:
        return decoded_candidates[0]
    return raw_data.decode("latin-1", errors="replace")


@dataclass
class CoredumpSection:
    index: int
    prefix_length: int
    content: str


@dataclass
class CoredumpBlock:
    addr: int
    length: int
    data: bytes
    name: str


def find_coredump_sections(content: str) -> List[CoredumpSection]:
    """Find all v0.0.1 coredump sections in the log."""
    pattern = re.compile(r"-\+-\+-\+-\s*BFLB\s+COREDUMP\s+v0\.0\.1\s*\+-\+-\+-\+")
    matches = list(pattern.finditer(content))
    sections: List[CoredumpSection] = []

    for idx, match in enumerate(matches):
        start = match.start()
        end = matches[idx + 1].start() if idx + 1 < len(matches) else len(content)
        line_start = content.rfind("\n", 0, start) + 1
        prefix_length = start - line_start
        sections.append(
            CoredumpSection(
                index=idx + 1,
                prefix_length=prefix_length,
                content=content[start:end],
            )
        )

    return sections


def strip_prefix(line: str, prefix_length: int) -> str:
    if prefix_length > 0 and len(line) > prefix_length:
        return line[prefix_length:]
    return line


def normalize_line(line: str, prefix_length: int) -> str:
    return strip_prefix(line, prefix_length).rstrip("\r")


def decode_base64_line(line: str) -> Optional[bytes]:
    if not line or len(line) <= 10 or not all(ch in BASE64_CHARS for ch in line):
        return None
    try:
        return base64.b64decode(line)
    except binascii.Error:
        return None


def extract_valid_block_entries(section: CoredumpSection) -> List[CoredumpBlock]:
    """Extract valid blocks, skipping ones that fail CRC."""
    begin_pattern = re.compile(r"-{6}\s*DATA BEGIN\s+([0-9A-Fa-f]+)@([0-9A-Fa-f]+)@([-\w. ]+)\s*-{6}")
    end_pattern = re.compile(r"-{6}\s*END\s+(.+?)\s*-{6}")

    blocks: List[CoredumpBlock] = []
    in_block = False
    current_addr = 0
    current_len = 0
    current_name = ""
    current_data = bytearray()

    for raw_line in section.content.split("\n"):
        line = normalize_line(raw_line, section.prefix_length)

        begin_match = begin_pattern.search(line)
        if begin_match and not in_block:
            in_block = True
            current_addr = int(begin_match.group(1), 16)
            current_len = int(begin_match.group(2), 16)
            current_name = begin_match.group(3)
            current_data = bytearray()
            continue

        end_match = end_pattern.search(line)
        if end_match and in_block:
            in_block = False
            original_crc = end_match.group(1).strip()
            end_crc = None
            try:
                end_crc = int.from_bytes(base64.b64decode(original_crc), "little")
            except (binascii.Error, ValueError):
                pass

            calc_crc = zlib.crc32(current_data) & 0xFFFFFFFF
            if end_crc is not None and calc_crc != end_crc:
                print(
                    f"crc check addr: {current_addr:08x}, length: {current_len:08x}, "
                    f"end_crc: {end_crc:08x}, crc: {calc_crc:08x} ",
                    file=sys.stderr,
                )
                continue

            blocks.append(
                CoredumpBlock(
                    addr=current_addr,
                    length=current_len,
                    data=bytes(current_data),
                    name=current_name,
                )
            )
            continue

        if in_block:
            decoded = decode_base64_line(line.strip())
            if decoded is None:
                continue
            current_data.extend(decoded)

    return blocks


def render_block(block: CoredumpBlock) -> str:
    encoded = base64.b64encode(block.data).decode("ascii")
    lines = [encoded[i:i + 76] for i in range(0, len(encoded), 76)]
    crc = base64.b64encode((zlib.crc32(block.data) & 0xFFFFFFFF).to_bytes(4, "little")).decode("ascii")
    body = "\n".join(lines)
    if body:
        body += "\n"
    return (
        f"------ DATA BEGIN {block.addr:08X}@{block.length:08X}@save_crash ------\n"
        f"{body}"
        f"------ END {crc} ------\n\n"
    )


def parse_coredump_sections(log_file: Path) -> List[List[CoredumpBlock]]:
    content = detect_and_read_file(log_file)
    sections = find_coredump_sections(content)
    return [extract_valid_block_entries(section) for section in sections]


def prepare_output_dir(output_dir: Path) -> None:
    output_dir.mkdir(parents=True, exist_ok=True)
    for path in output_dir.glob("coredump*"):
        if path.is_file():
            path.unlink()


def write_sections(sections: List[CoredumpSection], output_dir: Path) -> int:
    count = 0
    for section in sections:
        output_path = output_dir / f"coredump{section.index}"
        valid_blocks = extract_valid_block_entries(section)
        output_path.write_text(
            HEADER_LINE + "".join(render_block(block) for block in valid_blocks),
            encoding="utf-8",
            newline="",
        )
        count += 1
        print(f"[crash_capture] find {section.index} coredump ")
    return count


def main() -> int:
    parser = argparse.ArgumentParser(description="Capture multiple coredumps from a log file")
    parser.add_argument("log_file", help="Path to the input log file")
    parser.add_argument(
        "-o",
        "--output-dir",
        default=str(Path(__file__).resolve().parent / "output"),
        help="Directory to write split coredump files into",
    )
    args = parser.parse_args()

    log_file = Path(args.log_file)
    if not log_file.exists():
        print(f"[crash_capture] log file not found: {log_file}", file=sys.stderr)
        return 1

    content = detect_and_read_file(log_file)
    sections = find_coredump_sections(content)
    if not sections:
        print("[crash_capture] no valid coredump find ", file=sys.stderr)
        return 1

    prepare_output_dir(Path(args.output_dir))
    write_sections(sections, Path(args.output_dir))
    return 0


if __name__ == "__main__":
    sys.exit(main())
