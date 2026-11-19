# wasm_cmd：从 C 生成 Wasm 和 AOT

本示例面向 BL616 和 BL616CL，提供 `wasm_cmd interp|aot` shell 命令。两种
模式运行 `app/hello.c` 的逻辑，输出 `Hello World from WAMR!`。固件通过
`app/wasm_cmd_modules.h` 内嵌 `app/hello.wasm` 和 `app/hello.aot`。

## 编译和运行固件

在本目录执行：

```sh
make CHIP=bl616 BOARD=bl616dk
```

BL616CL 使用：

```sh
make CHIP=bl616cl BOARD=bl616cldk
```

切换芯片时请使用 `make clean` 清理原构建目录，或分别指定不同的
`BUILD_DIR`，避免复用另一芯片的缓存配置。

烧录固件后，在设备 shell 中分别运行：

```text
wasm_cmd interp
wasm_cmd aot
```

CoreMark 比较同一套算法的多种执行路径：

```text
wasm_cmd coremark direct
wasm_cmd coremark interp
wasm_cmd coremark aot
wasm_cmd coremark aot-fast
wasm_cmd coremark aot-dynamic
wasm_cmd coremark aot-dynamic-fast
wasm_cmd coremark aot-xip
wasm_cmd coremark aot-xip-fast
```

`aot` 和 `aot-xip` 保留软件线性内存边界检查和栈检查；对应的 `-fast`
模式关闭这些检查。`-fast` 只适用于经过验证的可信 AOT 模块，不应作为不受信任
模块的默认执行模式。

`aot-dynamic` 和 `aot-dynamic-fast` 使用 `wamrc --enable-indirect-mode`，
不是 `--xip`。这样 text 仍然不复制、不重定位并从 Flash 执行，函数地址由
模块实例的 `func_ptrs` 表在运行时解析；同时保留 LLVM intrinsics。它们适合
运行时加载、卸载和替换 AOT 模块。动态模式目前仍使用 WAMR 的间接函数表，
并没有实现固定寄存器 GOT/PLT；后者需要定制 `wamrc` 代码生成。

`aot` 把代码复制到可执行 OCRAM 后运行；`aot-xip` 使用 `--xip` 生成的
XIP AOT，代码直接从可执行 Flash 执行。XIP 模块数组必须是 `const`，否则会
落入 RAM，失去 XIP 的意义。

注意：`wamrc --xip` 不只是“从 Flash 取指”，它还隐含启用 AOT indirect
mode 并关闭 LLVM intrinsics。因此即使 BL616CL 的硬件 Flash XIP 可以直接
取指，WAMR XIP AOT 仍会有额外调用开销。当前 CoreMark 实测中，Flash XIP
安全模式约为 direct 的 32.6%，关闭检查后约为 36.9%。

BL616CL 上以同一固件、`-O2`、2K performance、Static、约 10 秒采样测得：

| 模式 | CoreMark | 相对 direct |
| --- | ---: | ---: |
| `direct` | 875.575（三轮平均） | 100% |
| `aot` | 356.640 | 40.73% |
| `aot-fast` | 444.948 | 50.82% |
| `aot-dynamic` | 334.620（单次） | 38.22% |
| `aot-dynamic-fast` | 440.755（三轮平均） | 50.34% |
| `aot-xip-fast` | 323.247（三轮平均） | 36.92% |

`aot-dynamic-fast` 明显优于原 `aot-xip-fast`，同时保留运行时加载、卸载和
替换能力。`aot-fast` 把代码复制到 OCRAM，`aot-dynamic-fast` 从 Flash
执行；两者接近说明 Flash 取指不是当前主要瓶颈，剩余差距主要来自 WAMR AOT
的调用约定、辅助栈和 Wasm 语义处理。上述所有运行的 CRC 均为
`list=0xe714 matrix=0x1fd7 state=0x8e3a`、`error=0`。

每条命令先进行短时间校准，再自动选择约 10 秒的迭代次数。计时的范围只包含
CoreMark `execute` 阶段，不包含数据初始化和 Wasm 模块加载。输出包含迭代数、
耗时、CoreMark 分数以及 2K performance run 的 CRC；正常情况下
`list=0xe714 matrix=0x1fd7 state=0x8e3a` 且状态为 `PASS`。

开发机可以通过串口脚本自动依次执行上述模式并汇总结果：

```sh
python3 run_coremark.py --port /dev/ttyACM0
```

只测试一种模式时使用 `--mode direct`、`--mode interp` 或
`--mode aot`，可以重复指定多个 `--mode`。

## XZ 压缩 AOT：下载到 LittleFS 后直接运行

`aot_store` 支持把 AOT 以 XZ 压缩形式下载到 LittleFS（`media` 分区），运行
时按 AOT 类型自动选择解压目的地：

- `e_type = XIP`（`wamrc --xip` 生成）：解压到原始 Flash `AOT` 分区，
  从 Flash 原地执行，代码不占用 RAM。
- `e_type = REL`（普通 `wamrc` 生成）：解压到 OCRAM 缓冲区，再由 WAMR
  把 text 拷贝/重定位到可执行 OCRAM 执行。

压缩必须使用 CRC32，且 LZMA2 字典不超过 8 KiB（解码器工作区约 40 KiB，
优先从 PSRAM 申请，否则从 OCRAM 申请，解码结束后释放）：

```sh
xz --check=crc32 --lzma2=preset=9,dict=8KiB -k app/coremark_xip.aot
xz --check=crc32 --lzma2=preset=9,dict=8KiB -k app/coremark.aot
```

`xz` 默认的 CRC64 和大于 8 KiB 的字典会在加载时报错。确需更大的字典时，
可提高 `app/aot_xz.h` 中的 `AOT_XZ_DICT_MAX`，工作区会相应增大。

把 `.aot.xz` 放到 HTTP(S) 服务器后，在设备 shell 中：

```text
aot_wifi_connect <ssid> <password>   # wifi_mgmr 连接，自动启动 DHCP
aot_download http://<host>/coremark_xip.aot.xz
aot_list
aot_run_lfs coremark_xip.aot.xz coremark_execute
```

`aot_download` 支持 `http://` 和 `https://`（HTTPS 走 SDK 的 TLS 客户端）。

`aot_run_lfs` 也可以直接运行未压缩的 `.aot` 文件；默认入口为 `main`。
`aot_install` 仍可用，并且现在同时接受 `.aot` 和 `.aot.xz`，安装到原始
slot 后再用 `aot_run` 运行。

注意：

- `aot_download` 只校验下载内容是否为 XZ/AOT 头；XZ 字典、CRC 类型和
  AOT 类型在 `aot_run_lfs`/`aot_install` 解压时才检查。
- XIP 类型每次 `aot_run_lfs` 都会重写 raw slot，slot 中原有镜像会被覆盖。
- REL 类型解压出的 staging 缓冲区必须存活到模块卸载（WAMR 的 AOT loader
  会保留对输入 buffer 的引用），因此运行期间 RAM 需要同时容纳整个镜像和
  WAMR 的可执行代码副本。RAM 紧张时可以改用
  `aot_install <file.aot.xz>` + `aot_run`：镜像解压到 Flash slot 后，
  WAMR 只把 text 拷贝到 OCRAM，不在 RAM 中保留整个镜像。
- 解压工作区或 REL staging 申请失败时会报错退出；打开 `CONFIG_PSRAM=y`
  可以让工作区和 staging 优先使用 PSRAM。
- SDK 的 XZ 解码器（`components/lzma`）使用全局解码器状态和 bump 分配器，
  同一时刻只能有一个解码流；本示例只在 shell 命令中串行使用。

仓库已附带预生成的 Wasm/AOT 数组头文件，可以直接构建固件。普通 `make` 不会
从 C 源码重新生成模块；原始 `.wasm`、`.aot` 和 `.hex` 是本地中间产物，已通过
`.gitignore` 排除。

## 从 C 重新生成 Wasm 和 AOT

转换在开发机上运行，需要 WASI-SDK 的 Clang、WAMR 2.4.4 的 `wamrc` 和
`xxd`。工具获取地址：

- [WASI-SDK 发布页](https://github.com/WebAssembly/wasi-sdk/releases)：下载适合开发机系统的压缩包，解压后使用 `bin/clang`。
- [WAMR 2.4.4 源码](https://github.com/wasm-micro-runtime/wasm-micro-runtime/tree/WAMR-2.4.4)：`wamrc` 的源码与构建说明在 [wamr-compiler 目录](https://github.com/wasm-micro-runtime/wasm-micro-runtime/tree/WAMR-2.4.4/wamr-compiler)。本 SDK 的 `components/wamr/upstream` 只有运行时源码，不含 `wamrc` 编译器。
- [WABT 发布页](https://github.com/WebAssembly/wabt/releases)：可选，提供检查 Wasm 导入和导出的 `wasm-objdump`。

`xxd` 可通过开发机的软件包管理器安装，通常由 Vim 的 `xxd` 软件包提供。
本示例的 `wasm_env.sh` 默认使用 `~/.local/opt/wasi-sdk` 和 `PATH` 中的
`wamrc`；如果工具安装在这些位置，直接重新生成模块即可：

```sh
sh generate_modules.sh
```

脚本会自动加载 `wasm_env.sh`，并检查 `clang`、`wamrc` 和 `xxd`。使用其他
安装位置时，可以通过环境变量覆盖默认值；也可以在当前 shell 中执行：

```sh
export WASI_SDK=/path/to/wasi-sdk
export WAMRC=/path/to/wamrc
sh generate_modules.sh
```

脚本依次执行以下命令：

```sh
"$WASI_SDK/bin/clang" --target=wasm32-wasip1 -O2 -nostdlib \
    -Wl,--no-entry \
    -o app/hello.wasm app/hello.c

"$WAMRC" --target=riscv32 --target-abi=ilp32f \
    --cpu=generic-rv32 --cpu-features=+m,+f,+c --xip \
    -o app/hello.aot app/hello.wasm

xxd -i -n hello_wasm -c 12 app/hello.wasm > app/wasm_cmd_modules.h
xxd -i -n hello_aot -c 12 app/hello.aot >> app/wasm_cmd_modules.h
xxd -p app/hello.wasm > app/hello.wasm.hex
```

脚本还会用同一 WASI-SDK 和 `wamrc` 编译
`app/coremark_bench.c` 及 `examples/coremark_v1.01` 中的 CoreMark 算法源码，
生成 `coremark.wasm`、带安全检查的 `coremark.aot`、关闭边界和栈检查的
`coremark_fast.aot`、两个 `--xip` 版本，以及两个使用
`--enable-indirect-mode` 的运行时 AOT 版本，然后一并写入
`app/wasm_cmd_modules.h`。

如果开发机装有 `xz`，脚本最后还会为这些 `.aot` 文件生成
`--check=crc32 --lzma2=preset=9,dict=8KiB` 的 `.aot.xz`，用于
`aot_download`/`aot_run_lfs`/`aot_install` 的 LittleFS 流程测试。

普通的 `coremark.aot`/`coremark_fast.aot` 不使用 `--xip`。BL616CL 会把
这些 AOT 代码加载到可执行 OCRAM；`aot-xip*` 仅用于 Flash 执行对比。

`-nostdlib` 避免链接 WASI 启动代码和 libc；`--no-entry` 不要求 `_start`。
`app/hello.c` 使用 `export_name("main")` 将无参数的 `wasm_entry` 导出为
`main`，避免 WASI-SDK 34 把 C 的 `main` 自动包装成接收 `argc`/`argv`
两个参数的入口，而固件按零参数调用该函数。`app/hello.c` 还显式导入
`env.puts`，该函数由 WAMR 内置 libc 提供。当前 SDK 未启用 WASI API，因此
导入 `wasi_snapshot_preview1` 的普通 WASI 程序不能在此固件中加载。

安装 WABT 后可用 `wasm-objdump -x app/hello.wasm` 检查导入为 `env.puts`、
导出为 `main`。重新编译的模块不一定与仓库里预生成的二进制逐字节一致，
应检查接口及设备上的输出。

AOT 命令针对 BL616 和 BL616CL 的 RV32/ILP32F ABI。AOT 加载后需要可执行
SRAM，不能放入不可执行的 PSRAM，也不能直接给其他 ABI 的芯片使用。
