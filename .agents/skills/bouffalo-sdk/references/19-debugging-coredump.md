# 日志、调试与 Coredump

## 日志与断言

`components/utils/log/log.h` 定义 `LOG_F/E/W/I/D/T` 和 raw 变体 `LOG_RF/RE/RW/RI/RD/RT`；`DBG_TAG` 指定标签，`CONFIG_LOG_LEVEL` 控制编译期级别。`CONFIG_BFLB_LOG` 默认关闭，此时普通宏走 `printf`；开启时走 BFLB recorder，实际输出还受 recorder、输出端和 tag 的过滤条件影响。`log_start()` 创建同步 recorder 和 console stream；目标板是否调用它应查板级初始化。FreeRTOS 的 `log_restart()` 可注册锁并启用异步 flush，是否实际调用取决于应用；`examples/bflog/freertos_async` 是示例，不能由此推断 ISR 安全。

`CONFIG_LOG_DISABLE` 会关闭日志宏，构建系统还对非 app 目标禁用部分标准输出。`CONFIG_ASSERT_DISABLE` 下 `_ASSERT_PARAM` 类的表达式不求值，而 `_ASSERT_FUNC` 类仍求值，避免依赖前者的副作用。默认 weak `error_handler()` 是死循环，可由应用覆盖；例子见 `examples/log_dbg_assert`。

## 调试器和符号

`CONFIG_DEBUG` 默认启用调试符号，但优化级别另由 `CONFIG_GCC_OPTIMISE_LEVEL` 决定。使用与设备镜像完全对应的 ELF、芯片配置和工具链；部分优化可能影响变量和调用栈。OpenOCD 包装器在 `tools/openocd/bin/openocd`，接口和 target 分别在 `tools/openocd/interface/`、`tools/openocd/target/`。选择与硬件实际匹配的配置，仓库还提供 `tools/gdbinit/bl616.init` 等初始化脚本。BL618DG `target/bl618dg.cfg` 默认 AP，NP 可设 `DEBUG_CPU np`，并需先由 AP 释放；此处仅说明仓库配置，连接与复位行为须在目标板验证。

## Coredump 输出

异常入口、寄存器日志及处理随芯片不同；以 BL616 为例，`CONFIG_COREDUMP` 的异常路径调用 `coredump_run()`。dump 范围由链接脚本 `_dump_sections` 指定，未必覆盖全部内存；输出含 build ID，可用 `riscv64-unknown-elf-readelf -n firmware.elf` 核对。检查日志是否完整、区域是否包含所需数据，避免仅凭工具启动就判断故障原因。

串口使用 `BFLB COREDUMP v0.0.1` 和 Base64/CRC 块。可选 Flash BIN 保存须由应用调用 `core_partition_init(addr, size)` 并预留至少各 dump 区域之和加 4096 字节；符合条件时会**先擦除该分区**，不满足条件仍可输出串口。不要套用 `examples/crash` 的分区地址到其他项目。结束后设备端等待 UART0 的 `@` 请求重发；当前 encoder 只在首次输出版本头，因此重发能否被当作独立 dump 拆分需实际核实。

## 离线分析

```bash
# 从 SDK 根目录运行；输出目录需是专为此次分析新建的空目录
python3 tools/byai/crash_capture.py serial.log -o /path/to/new-capture-directory
python3 tools/byai/coredump.py serial.log /path/to/matching-firmware.elf
# 也接受 BIN：python3 tools/byai/coredump.py crash.bin /path/to/matching-firmware.elf
```

`crash_capture.py` 写入前会删除输出目录已有的 `coredump*` 普通文件；务必使用新的专用目录。文本分析选最后一个版本头对应的段，若该段无有效块会失败，不会自动回退到前面的有效段；文档中“最后一个有效 dump”的描述不应当作保证。解析器会跳过 CRC 不符的块，但当前没有完整验证声明长度，且无法解读 CRC 时不会拒绝该块，因此仍需检查输出完整性。

`coredump.py` 加载 ELF 的 PT_LOAD 数据、用 dump 覆盖对应内存，在本机启动 GDB RSP 并从 PATH 查找 RISC-V GDB。脚本的远程寄存器初值为零，不能将其直接当作捕获的真实异常寄存器；结合串口异常寄存器日志、匹配 ELF 和 `tools/bouffalo_sdk.gdb` 的 FreeRTOS 任务回溯分析。`CONFIG_BACKTRACE` 及 `docs/zh/debug/backtrace.rst` 还提供运行时任务回溯方法。
