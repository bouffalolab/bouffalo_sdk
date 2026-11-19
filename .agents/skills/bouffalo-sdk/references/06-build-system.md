# 构建系统

本页以当前 SDK 中 `project.build`、`project.build.linux`、`board.build`、`cmake/bouffalo_sdk-config.cmake` 和 `examples/helloworld/` 为依据。修改构建逻辑或排查具体示例时，以这些文件和该示例的 `Makefile`、`CMakeLists.txt`、`Kconfig`、`defconfig` 为准。

## 按需求查找

路径相对 SDK 根目录。先选目标示例，再查看其构建文件和配置。

| 需求 | 入口 |
| --- | --- |
| 示例怎样接入 SDK | `examples/helloworld/Makefile`、`examples/helloworld/CMakeLists.txt`；通用入口 `project.build`、`board.build` |
| 菜单、配置来源和生成文件 | `project.build`、`project.build.linux`、`tools/kconfig/config_seed.py`；中文指南 `docs/zh/get_started/menuconfig_guide.rst` |
| 配置定义与组件依赖 | `Kconfig`、`bsp/Kconfig`、`components/Kconfig` 和目标组件的 `Kconfig` |
| 比较两份配置 | `tools/kconfig/diffconfig.py` |
| 编译器、ISA 和 ABI | `cmake/toolchain.cmake`、`docs/standard_riscv_toolchain.md` |

菜单指南可帮助理解界面，但其中部分描述与当前构建入口不一致：示例 `examples/wifi/defconfig` 包含 Make 条件语句，`project.build` 先通过 Make 解析；直接交给 `config_seed.py` 的输入则不接受这些语句。另外无线选项应查 `components/wireless/Kconfig` 的实际定义和依赖，不能笼统认为全部不在菜单中。以当前源码与生成配置为准。

## 构建入口

普通单工程示例的 `Makefile` 设置 `SDK_DEMO_PATH`、`BL_SDK_BASE`，并包含 SDK 的 `project.build`。后者包含 `board.build`，生成配置并调用 CMake；CMake 再生成 Make 或 Ninja 构建文件。双核父工程可能另用专门的 Makefile 组合多个子工程。

```bash
cd examples/helloworld
riscv64-unknown-elf-gcc --version  # 应为 GCC 14.3.0
make CHIP=bl616 BOARD=bl616dk
# 或：make ninja CHIP=bl616 BOARD=bl616dk
```

使用 GCC 14.3 的 `riscv64-unknown-elf-` 工具链，默认 C 库为 Picolibc。`board.build` 默认 `CHIP=bl616`、`BOARD=bl616dk`；即使有默认值也要显式传入，防止误编目标。BL618DG 的 `CPU_ID` 默认为 `ap`，切换到 NP 或 LP 时须显式指定并选择相应示例：

```bash
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=np
# LP 核参见 examples/pmu/bl618dg/pds_lp_wakeup/lp/
```

每个命令在对应示例目录执行；并非每个示例都支持所有芯片、板子或核心。单独指定 `BUILD_DIR` 可以隔离构建产物。`examples/wifi/sta/dualcore/` 父工程使用 `build/np` 和 `build/ap`；直接进入 `wifi_np/` 时默认是该目录下的 `build/`。父工程构建还会向父目录 defconfig 追加配置，先检查其 Makefile。

隔离构建目录不代表所有辅助目标都完全支持该目录：`tools/bflb_tools/project_flash.build` 的 ELF 自动发现仍固定查找 `build/build_out/`，而 `ram`、`puncover` 等命令又引用 `BUILD_DIR`。使用非默认目录时，先检查目标实现和实际文件，不能假定辅助目标会找到对应产物。

## 配置来源和生成文件

`project.build` 默认读取示例目录的 `defconfig`，`DEFCONFIG` 可指定其他文件；若示例有 `Kconfig` 就用它，否则使用 SDK 根目录的 `Kconfig`。Make 解析 defconfig、命令行 `CONFIG_*` 与芯片选择变量后，通过 `tools/kconfig/config_seed.py` 生成配置：

```text
defconfig + CHIP/BOARD/CPU_ID + 命令行 CONFIG_*
    -> build/generated/autoconfig.input
    -> Kconfig/config_seed.py
    -> autoconfig.def、autoconfig.cmake、autoconfig.h、autoconfig.mk
    -> CMake / 编译
```

没有菜单配置时，`project.build` 先写 `CHIP`、`BOARD`、`CPU_ID`、`CPU_MODEL`，再写 `CONFIG_*`。`config_seed.py` 会把这两类选择器映射到同一 Kconfig 名称，输入归并时后出现的赋值覆盖前值，最终结果还需经过 Kconfig 解析。例如同时传入 `CHIP=bl616 CONFIG_CHIP=bl616cl`，生成配置的芯片值是 `bl616cl`。避免同时传入相互矛盾的写法；排查时应比较 `autoconfig.input` 和 `autoconfig.def`，不能只看 Make 命令。

Make 自身的 `CHIP`、`CPU_ID` 不会因此自动改写。若 Make 变量与生成配置不一致，构建配置与 `flash` 等仍使用 Make 变量的辅助目标可能指向不同芯片或核心。烧录前须核对固件配置和目标设备。

`DEFCONFIG` 默认相对于 `SDK_DEMO_PATH` 解析；需要指定完整路径时使用 `DEFCONFIG_FILE`。`config_seed.py` 先将解析后的芯片、板子和核心选择器写入环境，再加载 Kconfig；板级配置通过 `$BOARD` 选择。Make 条件语句和 `include` 应由 Make 入口解析，不能直接作为 `config_seed.py` 的纯 defconfig 输入。

如果存在 `build/generated/autoconfig.menuconfig`，它会作为后续配置源。此时命令行选择器和 `CONFIG_*` 仍写入 `autoconfig.input`，但实际加载的是菜单配置，不能假定命令行选项覆盖了它。切换芯片、板子或核心前，检查 `make config-info` 中的配置源和菜单配置内容；需要保留原配置时使用独立 `BUILD_DIR`。`make menuconfig`（Linux 的 `project.build.linux` 目标）先运行 `config`，再从 `autoconfig.def` 复制菜单配置并启动交互界面。该文件位于构建目录内；`make clean` 会删除整个构建目录，包括菜单配置。需要持久保存的选项应记录到示例 defconfig；`make savedefconfig` 当前只打印提示，不会自动保存。

```bash
make config-info CHIP=bl616 BOARD=bl616dk  # 配置入口和文件路径
make config-show CHIP=bl616 BOARD=bl616dk # Linux：显示项目 defconfig
make menuconfig CHIP=bl616 BOARD=bl616dk  # 交互终端
```

`config-info` 只显示配置来源和路径，不验证最终选择器。执行过配置生成后，应检查所示 `autoconfig.def` 中的 `CONFIG_CHIP`、`CONFIG_BOARD`、`CONFIG_CPU_ID` 和 `CONFIG_CPU_MODEL`。需要保留旧目标时，选用尚不存在或已确认无旧菜单配置的独立目录：

```bash
make config CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build-bl618dg-ap-b0
make config-info CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0 BUILD_DIR=build-bl618dg-ap-b0
```

在支持该目标的示例中执行；后续构建继续使用同一 `BUILD_DIR`。旧芯片菜单配置不应直接当成新芯片配置复用。

修改 `CONFIG_*` 后需全新编译时，先确认并保存需要保留的菜单配置，再运行 `make clean`；如需菜单配置，应在清理后重新配置，再按目标构建。不要把 `build/generated/` 下生成文件当作持久配置手工维护。当前没有 `distclean` 目标。

再次运行 `menuconfig` 时，现有 `autoconfig.menuconfig` 先作为配置源解析，再由生成的 `autoconfig.def` 重新填充菜单文件。普通构建每次执行 `config` 并重新生成 CMake 缓存；这不等于所有源码文件都会重新编译。

### 配置输出

`autoconfig.input` 是 Make 输入，`autoconfig.def` 是 Kconfig 解析结果；`autoconfig.cmake`、`autoconfig.h`、`autoconfig.mk` 分别供构建工具消费。关闭的布尔选项可能不出现在 `.mk` 中。配置异常时对照输入、最终结果与目标消费者；直接调用配置工具时，语法和行为见 `tools/kconfig/config_seed.py`、`tools/kconfig/config_replace.py`。

## 工具链、ISA 与 ABI

SDK 要求 Bouffalo `riscv64-unknown-elf` GCC 14.3 和默认 Picolibc；T-Head GCC 与 Zephyr SDK 不在支持范围。详细背景和编译矩阵见 `docs/standard_riscv_toolchain.md`。`cmake/toolchain.cmake` 按目标固定选择 ISA/ABI，并同时用于编译与链接：

| 目标 | `-march` | `-mabi` |
| --- | --- | --- |
| 非 BL618DG 芯片 | `rv32imafc_zicsr_zifencei` | `ilp32f` |
| BL618DG AP/NP，`CPU_MODEL=b0` | `rv32imafdc_zicsr_zifencei` | `ilp32d` |
| BL618DG AP/NP，`CPU_MODEL=a0` 分支 | `rv32imafc_zicsr_zifencei` | `ilp32f` |
| BL618DG LP | `rv32emc_zicsr_zifencei` | `ilp32e` |

BL618DG 的 Kconfig 默认 `CPU_MODEL=b0`；`a0` 行描述当前源码分支，不表示所有发布配置都支持 A0。预编译库必须匹配对应 ABI。b0 的 `libbl618dg_phyrf_b0.a` 和 `libbl618dg_phyrf_lpfw_b0.a` 均使用 `ilp32d`；后者用于 AP 侧低功耗固件，不能据名称推断 LP 核也是 `ilp32d`。实际 PHY 库选择由 `drivers/soc/bl618dg/phyrf/CMakeLists.txt` 和 `CONFIG_PHY_LPFW` 决定。

Picolibc 使用 TLS errno：FreeRTOS 为任务准备 TLS，启动代码也在 `main()` 前初始化 boot TLS。SDK 常规构建依赖工具链默认 Picolibc，不自行添加 `--specs=picolibc.specs`。自定义链接脚本须提供 SDK 使用的 TLS 模板和 boot TLS 符号；不应通过假定存在 Newlib 回退来解决链接问题。编译/链接标志还可能由板级 CMake 追加，应结合 `cmake/compiler_flags.cmake`、目标板代码、生成配置和实际编译命令检查：正常 Kconfig 的优化级默认为 `-Os`，CMake 变量未定义时才回退 `-O2`；`CONFIG_NO_SHORT_ENUMS` 控制是否取消默认 `-fshort-enums`。

```bash
riscv64-unknown-elf-gcc -march=rv32imafdc_zicsr_zifencei -mabi=ilp32d -print-multi-directory
# -print-multi-directory 只检查工具链，检查实际归档的成员 ABI：
riscv64-unknown-elf-readelf -h path/to/selected_phyrf.a
python3 tests/toolchain/test_e907.py -v  # 在 SDK 根目录，先确认 GCC 版本
```

工具链测试可能因未找到 Picolibc specs 跳过相关用例，需查看测试输出；它验证编码和 ELF/TLS，不代替目标板运行。

## 示例 CMake 接入

`examples/helloworld/CMakeLists.txt` 使用 SDK 的 CMake 包和入口：

```cmake
cmake_minimum_required(VERSION 3.15)
find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})
sdk_set_main_file(main.c)
project(helloworld)
```

SDK 的 CMake 配置加载 `generated/autoconfig.cmake`、工具链和扩展，再添加 SDK 子目录。创建新项目时，从目标示例复制并按位置调整 `BL_SDK_BASE`，参照实际示例的组件接口；不要把通用 CMake 草稿当作 SDK 项目模板。

## 常用诊断

- 编译器找不到或版本不对：检查 `PATH`、`riscv64-unknown-elf-gcc --version` 和已有 `build/CMakeCache.txt` 的编译器路径。
- 芯片或板子不对：检查每次命令的 `CHIP`、`BOARD`，以及 BL618DG 的 `CPU_ID`。
- 配置与预期不符：先看 `make config-info`、defconfig、命令行选项和是否存在 `autoconfig.menuconfig`；需要重建前保存菜单配置。
- 链接或组件问题：检查目标示例的 `CMakeLists.txt`、`Kconfig`、`defconfig` 与构建日志。
- ABI 或 TLS 链接问题：对照 `CPU_ID`、`CPU_MODEL`、`-march`、`-mabi`、预编译库及链接脚本的 TLS 符号。
- `make size` 和 `make puncover` 分别调用项目中的大小报告与分析目标；使用前先完成构建。
