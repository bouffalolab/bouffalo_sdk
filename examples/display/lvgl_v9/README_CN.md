# display / lvgl_v9

[English](README.md)

LVGL v9 演示使用公共 display 框架（`bsp/common/display`）的默认端点。
默认运行 benchmark；修改 `main.c` 中的演示调用即可切换演示。

## 配置

| `DEFCONFIG` | 显示屏 | 支持芯片 / 要求 |
|---|---|---|
| `defconfig`（默认） | SPI ILI9341，240x320，RGB565 | 框架支持 `bl616`、`bl616cl`、`bl618dg`、`bl702l` |
| `defconfig.dbi_st7796` | DBI Type B B8 ST7796，320x480，RGB565 | 仅 `bl616`、`bl616cl`、`bl618dg` |
| `defconfig.dsi_ek79007` | DSI 2 lane EK79007，1024x600，RGB565 | 仅 `bl618dg` AP（`CPU_ID=ap`）；需要 PSRAM |

引脚见 `bsp/board/<board>` 中对应的板级实现和所选 defconfig。

## 编译

配置好 SDK 工具链后，在 `examples/display/lvgl_v9` 目录选择一条命令执行。
其他支持的芯片修改 `CHIP` / `BOARD`；BL618DG 指定 `CPU_ID=ap`。

```sh
# 默认 SPI ILI9341
make CHIP=bl616 BOARD=bl616dk DEFCONFIG=defconfig BUILD_DIR="$PWD/build/spi_ili9341"

# 可选 DBI ST7796
make CHIP=bl616 BOARD=bl616dk DEFCONFIG=defconfig.dbi_st7796 BUILD_DIR="$PWD/build/dbi_st7796"

# 可选 DSI EK79007
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap DEFCONFIG=defconfig.dsi_ek79007 BUILD_DIR="$PWD/build/dsi_ek79007"
```

切换配置、芯片或板时使用独立的绝对 `BUILD_DIR`：已有的 `generated/autoconfig.menuconfig` 优先于 defconfig。

## 烧录

完成编译后，连接烧录串口并进入下载模式；以下烧录默认 SPI BL616 构建，完成后复位开发板。

```sh
make flash CHIP=bl616 BOARD=bl616dk DEFCONFIG=defconfig BUILD_DIR="$PWD/build/spi_ili9341" COMX=/dev/ttyACM0
```

其他配置沿用编译时的 `DEFCONFIG`、`BUILD_DIR`、`CHIP`、`BOARD` 和 `CPU_ID`（如适用），并按实际串口修改 `COMX`。

## 旋转

GRAM 屏的 `lv_display_set_rotation()` 通过显示框架的
`display_set_rotation()` 实现面板硬件旋转。
VIDEO 屏不支持旋转。

## 参考

更多细节见 [display 框架文档](../../../bsp/common/display/README_CN.md)。
