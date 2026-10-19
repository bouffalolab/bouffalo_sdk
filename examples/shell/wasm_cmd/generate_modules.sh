#!/bin/sh
set -eu

cd "$(dirname "$0")"
: "${WASI_SDK:?Set WASI_SDK to the WASI-SDK directory}"
: "${WAMRC:?Set WAMRC to the WAMR 2.4.4 wamrc executable}"

"$WASI_SDK/bin/clang" --target=wasm32-wasi -O2 -nostdlib \
    -Wl,--no-entry -Wl,--export=main \
    -o app/hello.wasm app/hello.c

"$WAMRC" --target=riscv32 --target-abi=ilp32f \
    --cpu=generic-rv32 --cpu-features=+m,+f,+c --xip \
    -o app/hello.aot app/hello.wasm

xxd -i -n hello_wasm -c 12 app/hello.wasm > app/wasm_cmd_modules.h
xxd -i -n hello_aot -c 12 app/hello.aot >> app/wasm_cmd_modules.h
xxd -p app/hello.wasm > app/hello.wasm.hex
