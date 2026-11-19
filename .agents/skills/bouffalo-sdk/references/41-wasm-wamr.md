# WebAssembly / WAMR

## SDK 通用运行时

`components/wamr/` 包含 WAMR 2.4.4 的运行时子集及 Bouffalo FreeRTOS/heap/cache 适配。`CONFIG_WAMR_INTERP` 和 `CONFIG_WAMR_AOT` 默认关闭、均依赖 FreeRTOS；当前 AOT 另限制 `CHIP=bl616`，目标为 RV32 ILP32F，代码需要可执行 OCRAM，不能将它当成可在不可执行 PSRAM 运行。解释器接受架构无关 Wasm 字节码，但 Kconfig 无芯片限制不等于每颗芯片都已验证。

当前通用构建启用 builtin libc，不启用 WASI、JIT、SIMD、shared memory、线程管理、reference types 或 bulk memory。模块导入和 ABI 必须匹配此配置；不能将普通依赖 `wasi_snapshot_preview1` 的 WASI 应用直接当作可用模块。

`examples/shell/wasm_cmd/` 展示初始化 runtime、加载、实例化、查找并调用 `main`、按相反顺序清理。`wasm_cmd interp|aot` 使用 `app/wasm_cmd_modules.h` 中预生成的数组；普通 make 不会从 `hello.c` 重新生成。README 提及的独立 `hello.wasm`、`hello.aot` 文件在当前 checkout 未见，数组存在。重新生成需匹配 WASI-SDK Clang、WAMR 2.4.4 `wamrc` 和 `xxd`；生成脚本会覆盖数组头，先保护用户改动。生成后的导入、导出及目标 ABI 应重新核对。

## MicroPixel 的另一套运行时

`components/micropixel/` 包含自己的 WAMR 2.4.3 副本，Bouffalo port 通过其 `firmware/bouffalo/sdk.cmake` 引入，AOT-only，针对 BL616 ILP32F 或 BL618DG AP ILP32D，配置与通用组件不同。其 AOT loader 还有面向特定版本的源码复制和修补；升级需重新核对。不要将两套编译器、运行时、模块 ABI 或测试结果混同。

构建示例只能证明编译与链接；解释器和 AOT 的加载、执行、错误处理、内存使用及设备输出仍须对应硬件验证。MicroPixel 的 Linux AOT ownership 测试针对其独立副本，不能证明 SDK 通用运行时的板端行为。
