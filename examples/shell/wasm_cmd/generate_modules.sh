#!/bin/sh
set -eu

cd "$(dirname "$0")"
. ./wasm_env.sh

if [ ! -x "$WASI_SDK/bin/clang" ]; then
    echo "WASI-SDK clang not found: $WASI_SDK/bin/clang" >&2
    echo "Set WASI_SDK to the WASI-SDK directory." >&2
    exit 1
fi

if [ -z "${WAMRC:-}" ] || [ ! -x "$WAMRC" ]; then
    echo "WAMRC not found. Add wamrc to PATH or set WAMRC to its path." >&2
    exit 1
fi

if ! command -v xxd >/dev/null 2>&1; then
    echo "xxd not found. Install the xxd package." >&2
    exit 1
fi

"$WASI_SDK/bin/clang" --target=wasm32-wasip1 -O2 -nostdlib \
    -Wl,--no-entry -Wl,-z,stack-size=8192 \
    -o app/hello.wasm app/hello.c

"$WAMRC" --target=riscv32 --target-abi=ilp32f \
    --cpu=generic-rv32 --cpu-features=+m,+f,+c --xip \
    -o app/hello.aot app/hello.wasm

COREMARK_DIR="$(cd ../../coremark_v1.01 && pwd)"
"$WASI_SDK/bin/clang" --target=wasm32-wasip1 -O2 -nostdlib -fno-builtin \
    -include app/coremark_portable.h -I "$COREMARK_DIR" \
    -Wl,--no-entry -Wl,-z,stack-size=32768 \
    -o app/coremark.wasm \
    app/coremark_bench.c \
    "$COREMARK_DIR/core_list_join.c" \
    "$COREMARK_DIR/core_matrix.c" \
    "$COREMARK_DIR/core_state.c" \
    "$COREMARK_DIR/core_util.c"

"$WAMRC" --target=riscv32 --target-abi=ilp32f \
    --cpu=generic-rv32 --cpu-features=+m,+f,+c \
    --opt-level=3 --size-level=3 \
    -o app/coremark.aot app/coremark.wasm

"$WAMRC" --target=riscv32 --target-abi=ilp32f \
    --cpu=generic-rv32 --cpu-features=+m,+f,+c \
    --opt-level=3 --size-level=3 \
    --bounds-checks=0 --stack-bounds-checks=0 --disable-aux-stack-check \
    -o app/coremark_fast.aot app/coremark.wasm

# Runtime-loadable indirect AOT baseline. Do not use --xip here: --xip also
# disables LLVM intrinsics, while these modes keep them enabled.
"$WAMRC" --target=riscv32 --target-abi=ilp32f \
    --cpu=generic-rv32 --cpu-features=+m,+f,+c \
    --opt-level=3 --size-level=3 --enable-indirect-mode \
    -o app/coremark_dynamic.aot app/coremark.wasm

"$WAMRC" --target=riscv32 --target-abi=ilp32f \
    --cpu=generic-rv32 --cpu-features=+m,+f,+c \
    --opt-level=3 --size-level=3 --enable-indirect-mode \
    --bounds-checks=0 --stack-bounds-checks=0 --disable-aux-stack-check \
    -o app/coremark_dynamic_fast.aot app/coremark.wasm

"$WAMRC" --target=riscv32 --target-abi=ilp32f \
    --cpu=generic-rv32 --cpu-features=+m,+f,+c \
    --opt-level=3 --size-level=3 --xip \
    -o app/coremark_xip.aot app/coremark.wasm

"$WAMRC" --target=riscv32 --target-abi=ilp32f \
    --cpu=generic-rv32 --cpu-features=+m,+f,+c \
    --opt-level=3 --size-level=3 --xip \
    --bounds-checks=0 --stack-bounds-checks=0 --disable-aux-stack-check \
    -o app/coremark_xip_fast.aot app/coremark.wasm

xxd -i -n hello_wasm -c 12 app/hello.wasm > app/wasm_cmd_modules.h
xxd -i -n hello_aot -c 12 app/hello.aot >> app/wasm_cmd_modules.h
xxd -i -n coremark_wasm -c 12 app/coremark.wasm >> app/wasm_cmd_modules.h
xxd -i -n coremark_aot -c 12 app/coremark.aot >> app/wasm_cmd_modules.h
xxd -i -n coremark_fast_aot -c 12 app/coremark_fast.aot \
    | sed -E 's/^unsigned (char|int) /const unsigned \1 /' >> app/wasm_cmd_modules.h
xxd -i -n coremark_dynamic_aot -c 12 app/coremark_dynamic.aot \
    | sed -E 's/^unsigned (char|int) /const unsigned \1 /' >> app/wasm_cmd_modules.h
xxd -i -n coremark_dynamic_fast_aot -c 12 app/coremark_dynamic_fast.aot \
    | sed -E 's/^unsigned (char|int) /const unsigned \1 /' >> app/wasm_cmd_modules.h
xxd -i -n coremark_xip_aot -c 12 app/coremark_xip.aot \
    | sed -E 's/^unsigned (char|int) /const unsigned \1 /' >> app/wasm_cmd_modules.h
xxd -i -n coremark_xip_fast_aot -c 12 app/coremark_xip_fast.aot \
    | sed -E 's/^unsigned (char|int) /const unsigned \1 /' >> app/wasm_cmd_modules.h
xxd -p app/hello.wasm > app/hello.wasm.hex
xxd -p app/coremark.wasm > app/coremark.wasm.hex

# Optional: XZ-compressed AOT images for the LittleFS download/run flow
# (aot_download + aot_run_lfs). The SDK XZ decoder supports CRC32 checks and
# an LZMA2 dictionary of at most 8 KiB, see app/aot_xz.h.
if command -v xz >/dev/null 2>&1; then
    for aot in app/hello.aot app/coremark.aot app/coremark_fast.aot \
               app/coremark_dynamic.aot app/coremark_dynamic_fast.aot \
               app/coremark_xip.aot app/coremark_xip_fast.aot; do
        xz --check=crc32 --lzma2=preset=9,dict=8KiB -k -f "$aot"
    done
fi
