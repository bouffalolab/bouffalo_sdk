#!/bin/sh

set -eu

docs_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

missing=0
for tool in make sphinx-build; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        printf 'Missing required tool: %s\n' "$tool" >&2
        missing=1
    fi
done
if [ "$missing" -ne 0 ]; then
    printf '%s\n' \
        "Install Python dependencies: python3 -m pip install -r \"$docs_dir/requirements.txt\"" \
        'On Debian/Ubuntu: sudo apt install make' >&2
    exit 1
fi

for language in en zh; do
    make -C "$docs_dir" html LANGUAGE="$language"
done
