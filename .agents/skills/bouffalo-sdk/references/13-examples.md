# SDK 示例

从目标示例的 `README`、`Makefile`、`CMakeLists.txt`、`defconfig` 和 `main.c` 获取实际构建参数和 API 用法。示例支持的芯片、板子与核心可能不同；下面是检索入口，不代表所有组合都已验证。

| 主题 | SDK 目录 |
| --- | --- |
| 基础 | `examples/helloworld/` |
| 外设 | `examples/peripherals/`，如 `gpio/`、`uart/`、`spi/`、`i2c/`、`adc/`、`pwm_v2/` |
| WiFi Station | `examples/wifi/sta/` |
| WiFi AP + STA | `examples/wifi/ap_sta_bridge/` |
| HTTP | `examples/wifi/sta/wifi_http/`、`examples/wifi/sta/http_restful_api/` |
| WiFi/BLE 共存 | `examples/wifi/coex/wifi_ble/`、`examples/coex/` |
| BLE | `examples/btble/peripheral/`、`examples/btble/central/`、`examples/btble/btble_cli/` |
| 文件系统 | `examples/fatfs/`、`examples/littlefs/` |
| SPI NAND | `examples/peripherals/spi/spi_nand/` |
| USB | `examples/cherryusb/` |
| 显示与图形 | `examples/display/` |
| 音频 | `examples/audio/` |
| 安全与加密 | `examples/mbedtls/` |
| 分区 | `examples/partition/` |
| Shell 与崩溃调试 | `examples/shell/`、`examples/crash/` |
| RTOS | `examples/freertos/` |
| 电源管理 | `examples/pmu/`；BL618DG LP 核见 `examples/pmu/bl618dg/pds_lp_wakeup/lp/` |
| 核间通信 | `examples/openamp/`、`examples/bl618dg_dualcore/` |

查找具体示例时，可从 SDK 根目录运行：

```bash
rg --files examples/peripherals/uart examples/wifi/sta examples/btble/peripheral
```

## helloworld 构建

```bash
cd examples/helloworld
riscv64-unknown-elf-gcc --version  # 应为 GCC 14.3.0
make CHIP=bl616 BOARD=bl616dk
```

该示例的 `Makefile` 引入 `project.build`，`CMakeLists.txt` 通过 `find_package(bouffalo_sdk ...)` 和 `sdk_set_main_file(main.c)` 引入 SDK。配置流程、菜单配置保留与清理说明见 [06-build-system.md](06-build-system.md)。编译成功只证明构建完成；烧录和运行需分别验证。

使用其他示例前，查看其 README 和 `defconfig`。选择 BL618DG NP 或 LP 示例时，显式设置对应的 `CPU_ID`。烧录前确认目标板与串口，并遵守当前项目的设备和授权规则。

## LVGL

- [lvgl_v8](../../../../examples/display/lvgl_v8/README.md)：LVGL v8。
- [lvgl_v9](../../../../examples/display/lvgl_v9/README.md)：LVGL v9。
- [lvgl_v9_with_osd](../../../../examples/display/lvgl_v9_with_osd/README_CN.md)：LVGL v9 OSD 与视频显示。

这些示例使用公共 display 框架的默认 GRAM 或 VIDEO 显示端口；可选触摸输入默认关闭，启用对应 `CONFIG_LVGL8_PORT_INDEV` 或 `CONFIG_LVGL9_PORT_INDEV` 时，还需配置 `CONFIG_BSP_TOUCH` 和实际触摸控制器。先调用 `lv_init()`，再初始化并检查 `lv_port_disp_init()`，最后初始化可选输入设备；v9 还需设置 tick 回调。具体初始化、芯片支持和接线以各示例的 `main.c`、`defconfig` 和 README 为准。
