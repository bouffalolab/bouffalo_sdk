# 开发工作流入口

路径相对 SDK 根目录。客户环境和目标板可能不同，具体要求以 SDK 文档、目标示例及实际设备为准。

| 任务 | 从哪里查 |
| --- | --- |
| 安装环境、工具链和入门 | `docs/zh/index.rst`、`docs/en/index.rst`、`docs/standard_riscv_toolchain.md`；确认 `riscv64-unknown-elf-gcc --version` 为 SDK 要求的 GCC 14.3 |
| 寻找或创建工程 | [示例索引](13-examples.md)；先读目标示例的 README、Makefile、CMakeLists.txt、Kconfig 和 defconfig。复制示例时按新位置设置 `BL_SDK_BASE`，并避免复用旧的 `build/` |
| 构建与配置 | [构建系统](06-build-system.md)；显式选择 `CHIP`、`BOARD`，BL618DG NP/LP 工程指定相应 `CPU_ID` |
| 修改组件 | 先查组件 Kconfig、CMake 和[组件目录](09-components.md)，再核对目标工程实际配置 |
| 固件打包与下载 | [固件打包与烧录](35-firmware-pack-flash.md)；烧录前确认目标板、串口、镜像及擦除范围。`make flash` 使用现有构建目录，不自动重新编译；烧录后若停在下载模式，按串口 RTS/DTR（CHIP_EN/BOOT）说明恢复 |
| 调试与日志 | [调试与 Coredump](19-debugging-coredump.md)；按目标探针选择 `tools/openocd/` 配置，GDB 使用匹配的 ELF |
| 外设和无线 | [外设入口](08-peripherals.md)、[无线协议](05-wireless.md)；先确认板级资源和目标配置 |
| 测试 | [测试与 CI](20-testing-ci.md)；区分构建、烧录、运行和实际功能验证 |

`autoconfig.menuconfig` 可能影响后续构建选择；在切换目标或清理前先查看 [构建系统](06-build-system.md) 并保留需要的配置。构建成功本身不代表固件已在目标板上运行。
