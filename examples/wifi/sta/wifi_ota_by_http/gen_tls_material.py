#!/usr/bin/env python3

import argparse
from pathlib import Path


PEM_FILES = (
    ("ca_1.crt", "https_ota_ca_pem"),
    ("client_1.crt", "https_ota_client_cert_pem"),
    ("client_1.key", "https_ota_client_key_pem"),
)


def pem_to_c_string(pem_text: str) -> str:
    lines = []
    for line in pem_text.splitlines(keepends=True):
        escaped = line.replace("\\", "\\\\").replace('"', '\\"')
        escaped = escaped.replace("\r", "\\r").replace("\n", "\\n")
        lines.append(f'    "{escaped}"')

    if not lines:
        return '    ""'

    return "\n".join(lines)


def read_pem(path: Path) -> str:
    pem_text = path.read_text(encoding="utf-8")
    if not pem_text.endswith("\n"):
        pem_text += "\n"
    return pem_text


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate embedded TLS PEM header")
    parser.add_argument("--input-dir", required=True)
    parser.add_argument("--output", required=True)
    args = parser.parse_args()

    input_dir = Path(args.input_dir)
    output = Path(args.output)
    output.parent.mkdir(parents=True, exist_ok=True)

    header_lines = [
        "#pragma once",
        "",
        "#include <stddef.h>",
        "",
    ]

    for filename, symbol in PEM_FILES:
        pem_path = input_dir / filename
        pem_text = read_pem(pem_path)
        header_lines.append(f"static const char {symbol}[] =")
        header_lines.append(pem_to_c_string(pem_text))
        header_lines.append(";")
        header_lines.append(f"static const size_t {symbol}_len = sizeof({symbol});")
        header_lines.append("")

    output.write_text("\n".join(header_lines), encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())