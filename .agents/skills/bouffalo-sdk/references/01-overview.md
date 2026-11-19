# Bouffalo SDK 概览

Bouffalo SDK 是用于 Bouffalo 芯片 IoT 和 MCU 开发的 SDK，包含板级支持、驱动、组件、示例和工具。芯片、板子、核心及对应源码入口见 [04-chips-diff.md](04-chips-diff.md)；芯片能力以目标资料、配置和示例为准。

路径相对 SDK 根目录：

| 目录或文件 | 用途 |
| --- | --- |
| `bsp/board/`、`bsp/common/` | 目标板配置、初始化和公共板级功能 |
| `drivers/lhal/` | 外设 API、实现和各芯片设备表 |
| `drivers/soc/` | 各芯片启动、底层驱动和链接相关文件 |
| `drivers/sys/`、`drivers/rfparam/` | 芯片系统与低功耗恢复、RF 参数 |
| `components/` | 无线、网络、RTOS、存储、多媒体等软件组件，见[组件目录](09-components.md) |
| `examples/` | 依主题组织的示例，见[示例索引](13-examples.md) |
| `docs/zh/index.rst`、`docs/en/index.rst` | 中文、英文文档入口；`docs/doxygen/` 用于 API 文档生成 |
| `tools/`、`tests/` | 烧录、调试、配置等工具与测试 |
| `project.build`、`board.build`、`cmake/`、`Kconfig` | 工程构建、板级参数、CMake 和配置入口 |
| `README.md`、`AGENTS.md` | SDK 项目说明和 agent 项目指南 |

SDK 的依赖和第三方组件版本随 checkout 和配置变化，以组件源码与构建配置为准。构建使用 `riscv64-unknown-elf` GCC 14.3 和 Picolibc；具体环境见 `docs/standard_riscv_toolchain.md` 和[构建系统](06-build-system.md)。
