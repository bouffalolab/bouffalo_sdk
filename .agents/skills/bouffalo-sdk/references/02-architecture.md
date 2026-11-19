# SDK 架构与板级支持

路径相对 SDK 根目录。各部分协作，具体调用关系取决于芯片和配置，不应把设备表或 SoC 驱动画成所有 LHAL 调用的固定中间层。

| 部分 | 职责与入口 |
| --- | --- |
| 应用和示例 | `examples/`；读取目标工程的 `main.c`、`Makefile`、`CMakeLists.txt`、`Kconfig` 和 `defconfig` |
| 软件组件 | `components/`，按目标配置接入无线、网络、RTOS、文件系统、媒体等；见[组件目录](09-components.md) |
| 板级支持 | `bsp/board/<BOARD>/` 和 `bsp/common/`；初始化引脚、时钟、存储和板上器件 |
| LHAL | `drivers/lhal/include/`、`drivers/lhal/src/`；`drivers/lhal/config/<chip>/device_table.c` 定义目标设备，见 [03-lhal-drivers.md](03-lhal-drivers.md) |
| 芯片实现 | `drivers/soc/<chip>/`、`drivers/sys/<chip>/`；包含芯片特定底层、启动和电源管理代码 |
| 构建和配置 | `project.build`、`board.build`、`cmake/`、`Kconfig`；见[构建系统](06-build-system.md) |

## 板级选择和启动

`bsp/board/CMakeLists.txt` 先查 SDK 的 `bsp/board/<BOARD>/CMakeLists.txt`，再查工程目录中的同名板。板级代码可复用 DK 并覆盖部分实现，因此还需查看该板的 CMake、`Kconfig.defconfig`、GPIO 配置和实际硬件。BL618DG Orion 的复用示例见 [43-board-integration.md](43-board-integration.md)。

启动代码及链接脚本受芯片、核心、boot2 和配置影响；例如 BL616、BL616CL 和 BL618DG 的启动路径会调用 `bsp/common/start_load.c`。`board_init()` 的调用由应用决定，可看 `examples/helloworld/main.c`。Picolibc 的线程局部存储及自定义链接脚本约束见 `docs/standard_riscv_toolchain.md`。

## 重要协作关系

- **无线和网络**：Wi-Fi 6 可选择 FHOST 或 wl80211，MACSW、supplicant、lwIP 各有职责，见 [Wi-Fi 6](14-wifi6-wl80211.md)；蓝牙 controller 和 host 按目标配置选择，见 [BLE](15-ble-basics.md)。
- **多核**：BL618DG 的 AP、NP、LP 使用各自工程和配置。硬件 IPC、OpenAMP/RPMsg 和 Socket Offload 的示例分工不代表所有工程固定分工，见 [IPC 与 Offload](18-ipc-offload.md)。
- **内存**：`components/mm/` 的堆区域由板级初始化和链接配置决定；Flash 与可选 PSRAM 的使用见[内存与分区](10-memory-management.md)。
- **低功耗与安全**：芯片 PDS/HBN、系统恢复、FreeRTOS tickless 以及安全启动、加密和 TLS 分别受配置约束，见[低功耗](12-power-management.md)和[安全](11-security.md)。
- **调试**：探针、GDB、ELF 和可选 coredump 的选择见[调试与 Coredump](19-debugging-coredump.md)。
