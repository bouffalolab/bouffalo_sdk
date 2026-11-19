# 芯片系列差异

具体芯片、封装、板子和 SDK 配置决定可用外设、内存与协议。不要根据其他芯片的结果推断支持情况；以目标芯片的 `drivers/soc/<chip>/`、`drivers/lhal/config/`、`bsp/board/<board>/`、示例 `defconfig` 及芯片资料为准。

## 芯片和无线入口

| 系列 | 当前 SDK 入口 | 查证方向 |
| --- | --- | --- |
| BL602/BL604 | `drivers/soc/bl602/`、`bsp/board/bl602dk/` | WiFi 4 与 BLE 示例及其配置 |
| BL702/BL704/BL706 | `drivers/soc/bl702/`、`bsp/board/bl702dk/` | 以所选芯片和示例确认无线协议及外设 |
| BL702L | `drivers/soc/bl702l/`、`bsp/board/bl702ldk/` | 查看独立的芯片驱动、设备表和示例配置 |
| BL616/BL618 | `drivers/soc/bl616/`、`bsp/board/bl616dk/` | WiFi 6、BLE 及所选板级配置 |
| BL616CL | `drivers/soc/bl616cl/`、`bsp/board/bl616cldk/` | WiFi、BLE、板级配置；WiFi/BLE 共存示例含 BL616CL 分支 |
| BL618DG | `drivers/soc/bl618dg/`、`bsp/board/bl618dgdk/` | 多核配置及相应核心的示例 |

BL616CL 不是仅支持 BLE；`examples/wifi/coex/wifi_ble/defconfig` 和 `examples/wifi/nethub/defconfig` 含 BL616CL 分支。这是源码配置证据，不代表所有示例或组合均已在硬件上验证。

查询可用芯片、板子和配置的示例：

```bash
ls drivers/soc
ls bsp/board
rg -n 'BL616CL|bl616cl' examples/wifi
```

上表不提供未经核实的外设测试状态、容量或性能数值。Flash/PSRAM 的实际容量还与具体物料、板级配置和分区有关，须结合目标硬件确认。

## 构建选择

在对应示例目录中显式指定芯片与板子：

```bash
make CHIP=bl616 BOARD=bl616dk
make CHIP=bl616cl BOARD=bl616cldk
```

BL618DG 的 `CPU_ID` 默认是 `ap`；编译其他核心时显式传入，且应使用目标核心对应的工程：

```bash
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=np
# LP 核示例：examples/pmu/bl618dg/pds_lp_wakeup/lp/
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=lp
```

AP/NP 示例可参考 `examples/wifi/sta/dualcore/`。LP 示例需要在其自身目录执行；这些命令不表示同一个示例一定支持三个核心。

其他入口包括 `examples/bl618dg_dualcore/helloworld_ap/` 和 `helloworld_np/`，以及 `examples/pmu/bl618dg/pds_lp_wakeup/lp/`。双核及 LP 唤醒工程的父 Makefile 还负责组合镜像，不能用单核构建成功推断组合镜像或运行成功。

BL618DG 板级 `CPU_MODEL` 默认 `b0`。`drivers/soc/bl618dg/std/CMakeLists.txt` 在非 LP 核按 `CPU_MODEL=a0` 选择 `startup/mx/`、其他型号选择 `startup/mn/`，LP 选择 `startup/lp/`；NP 和 LP 构建会强制关闭 `CONFIG_ROMAPI`。默认链接脚本还取决于板级配置和 `CONFIG_CUSTOM_LINKSCRIPT`，其中 DK 的 LP b0 使用专用脚本，LP 非 b0 分支未在该处设置默认脚本。排查时读取实际 CMake 配置和链接命令，而不是仅根据芯片名推断。

## 迁移检查

1. 确定目标 `CHIP`、`BOARD`、核心及相应示例。
2. 比较目标芯片的设备表、引脚、时钟、中断和外设实现；相同 LHAL API 不保证所有能力相同。
3. 检查无线、内存和低功耗配置及目标板硬件。MACSW coex 中的芯片差异遵循项目的 HAL 平台隔离规则。
4. 分别验证编译、烧录与运行结果；性能、功耗和协议组合需在目标硬件上测量。
