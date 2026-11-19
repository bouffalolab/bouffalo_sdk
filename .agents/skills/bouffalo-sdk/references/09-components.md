# SDK 组件目录

下表是组件用途和查找入口，路径相对 `bouffalo_sdk/`。实际接入条件见各组件的 `Kconfig`、`CMakeLists.txt`、工程 `defconfig` 和 `components/CMakeLists.txt`；目录存在不代表目标芯片已启用或验证。外部子仓是否齐全由当前 SDK checkout 决定。

| 用途 | 源码 / 文档入口 | 示例或专题 |
| --- | --- | --- |
| Wi-Fi、蓝牙和其他无线协议 | `components/wireless/` | [无线协议](05-wireless.md) |
| TCP/IP、应用协议、NetHub、Netbus | `components/net/`、`components/net/Kconfig` | [网络](17-tcpip-network.md)、[应用协议](25-app-protocols-cloud.md)、[NetHub](26-nethub.md)、[Netbus](27-netbus-at.md) |
| 网络吞吐测试 | `components/iperf/README_cn.md`、`components/iperf/` | [网络测试](17-tcpip-network.md)；Classic iPerf2，不能与 iPerf3 对端互测 |
| FreeRTOS 和电源管理 | `components/os/` | `examples/freertos/`、[低功耗](12-power-management.md)；NuttX 属于外部系统集成 |
| C 运行时和系统初始化 | `components/libc/`、`components/sysinit/`、`docs/standard_riscv_toolchain.md` | `examples/cxx_runtime/`、[运行时工具](34-runtime-utilities.md) |
| 堆、内存管理 | `components/mm/` | [内存与分区](10-memory-management.md) |
| 核间通信、OpenAMP 和 RPMsg | `components/ipc/` | `examples/openamp/`、[IPC 与 Offload](18-ipc-offload.md) |
| 文件系统、KV、MTD | `components/fs/` | `examples/fatfs/`、[文件系统与存储](28-filesystems-storage.md) |
| SPI NAND 和 Dhara FTL | `components/spi_nand/`、`components/spi_nand/Kconfig` | `examples/peripherals/spi/spi_nand/README.md`、[文件系统与存储](28-filesystems-storage.md) |
| 固件升级 | `components/fota/` | [OTA 与 FOTA](29-ota-fota.md) |
| XZ/LZMA2 解码 | `components/lzma/Kconfig`、`components/lzma/xz/` | `examples/boot2_isp/defconfig`、`examples/eflash_loader/defconfig`，见[运行时工具](34-runtime-utilities.md) |
| 加密与 TLS | `components/crypto/` | `examples/mbedtls/`、[安全和加密](11-security.md) |
| USB Device/Host | `components/usb/` | `examples/cherryusb/`、[USB](30-usb-cherryusb.md) |
| 图形和显示 | `components/graphics/` | `examples/display/`、[图形和显示](31-graphics-display-touch.md) |
| 音视频与媒体框架 | `components/multimedia/`、`components/bflb_gmf/` | `examples/audio/`、[音视频](32-audio-video-media.md)、[AI 与语音](33-ai-inference-speech.md)；按专题区分两套 GMF |
| 音视频渲染 facade | `components/av_render/README.md`、`components/av_render/` | [音视频](32-audio-video-media.md) |
| 媒体数据流与方案辅助 | `components/soln_utils/README_zh.md`、`components/soln_utils/` | `examples/solution/solution_bl616/image_transmission/`、[音视频](32-audio-video-media.md) |
| AI 推理与语音 | `components/ai/` | [AI 与语音](33-ai-inference-speech.md)；相关源码是否齐全以当前 checkout 为准 |
| WebAssembly 运行时 | `components/wamr/` | [WAMR](41-wasm-wamr.md) |
| Shell、常用数据结构与工具 | `components/shell/`、`components/utils/` | `examples/shell/`、[运行时工具](34-runtime-utilities.md) |
| 调试和日志 | `components/debug/` | `examples/crash/`、[调试与 Coredump](19-debugging-coredump.md) |

查找组件接入与依赖时，先看其 `Kconfig` 和 CMake 文件，再看目标工程配置；新增组件的接入方式见 [构建系统](06-build-system.md)。线程局部存储（TLS）与网络传输层安全（TLS）是不同概念。
