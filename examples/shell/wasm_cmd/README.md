# wasm_cmd：从 C 生成 Wasm 和 AOT

本示例面向 BL616，提供 `wasm_cmd interp|aot` shell 命令。两种模式运行
`app/hello.c` 的逻辑，输出 `Hello World from WAMR!`。固件通过
`app/wasm_cmd_modules.h` 内嵌 `app/hello.wasm` 和 `app/hello.aot`。

## 编译和运行固件

在本目录执行：

```sh
make CHIP=bl616 BOARD=bl616dk
```

烧录固件后，在设备 shell 中分别运行：

```text
wasm_cmd interp
wasm_cmd aot
```

仓库已附带预生成模块及数组头文件，可以直接构建固件。普通 `make` 不会
从 C 源码重新生成模块。

## 从 C 重新生成 Wasm 和 AOT

转换在开发机上运行，需要 WASI-SDK 的 Clang、WAMR 2.4.4 的 `wamrc` 和
`xxd`。工具获取地址：

- [WASI-SDK 发布页](https://github.com/WebAssembly/wasi-sdk/releases)：下载适合开发机系统的压缩包，解压后使用 `bin/clang`。
- [WAMR 2.4.4 源码](https://github.com/wasm-micro-runtime/wasm-micro-runtime/tree/WAMR-2.4.4)：`wamrc` 的源码与构建说明在 [wamr-compiler 目录](https://github.com/wasm-micro-runtime/wasm-micro-runtime/tree/WAMR-2.4.4/wamr-compiler)。本 SDK 的 `components/wamr/upstream` 只有运行时源码，不含 `wamrc` 编译器。
- [WABT 发布页](https://github.com/WebAssembly/wabt/releases)：可选，提供检查 Wasm 导入和导出的 `wasm-objdump`。

`xxd` 可通过开发机的软件包管理器安装，通常由 Vim 的 `xxd` 软件包提供。
把下面两个路径改成实际安装位置，然后在本目录执行：

```sh
export WASI_SDK=/path/to/wasi-sdk
export WAMRC=/path/to/wamrc
sh generate_modules.sh
```

脚本依次执行以下命令：

```sh
"$WASI_SDK/bin/clang" --target=wasm32-wasi -O2 -nostdlib \
    -Wl,--no-entry -Wl,--export=main \
    -o app/hello.wasm app/hello.c

"$WAMRC" --target=riscv32 --target-abi=ilp32f \
    --cpu=generic-rv32 --cpu-features=+m,+f,+c --xip \
    -o app/hello.aot app/hello.wasm

xxd -i -n hello_wasm -c 12 app/hello.wasm > app/wasm_cmd_modules.h
xxd -i -n hello_aot -c 12 app/hello.aot >> app/wasm_cmd_modules.h
xxd -p app/hello.wasm > app/hello.wasm.hex
```

`-nostdlib` 避免链接 WASI 启动代码和 libc；`--no-entry` 不要求 `_start`；
`--export=main` 导出固件要调用的函数。`app/hello.c` 显式导入 `env.puts`，
该函数由 WAMR 内置 libc 提供。当前 SDK 未启用 WASI API，因此导入
`wasi_snapshot_preview1` 的普通 WASI 程序不能在此固件中加载。

安装 WABT 后可用 `wasm-objdump -x app/hello.wasm` 检查导入为 `env.puts`、
导出为 `main`。重新编译的模块不一定与仓库里预生成的二进制逐字节一致，
应检查接口及设备上的输出。

AOT 命令针对 BL616 的 RV32/ILP32F ABI。AOT 加载后需要可执行 SRAM，
不能放入不可执行的 PSRAM，也不能直接给其他 ABI 的芯片使用。
