# display / lvgl_v8

[中文](README_CN.md)

LVGL v8 demos use the shared display framework's default endpoint (`bsp/common/display`).
The benchmark runs by default; change the demo call in `main.c` to select another demo.

## Configurations

| `DEFCONFIG` | Display | Supported chips / requirements |
|---|---|---|
| `defconfig` (default) | SPI ILI9341, 240x320, RGB565 | Framework supports `bl616`, `bl616cl`, `bl618dg`, `bl702l` |
| `defconfig.dbi_st7796` | DBI Type B B8 ST7796, 320x480, RGB565 | Only `bl616`, `bl616cl`, `bl618dg` |
| `defconfig.dsi_ek79007` | 2-lane DSI EK79007, 1024x600, RGB565 | Only `bl618dg` AP (`CPU_ID=ap`); PSRAM required |

For pin assignments, see the board implementation in `bsp/board/<board>` and the selected defconfig.

## Compile

Set up the SDK toolchain, then run one command in `examples/display/lvgl_v8`.
For other supported chips, change `CHIP` / `BOARD`; specify `CPU_ID=ap` for BL618DG.

```sh
# Default SPI ILI9341
make CHIP=bl616 BOARD=bl616dk DEFCONFIG=defconfig BUILD_DIR="$PWD/build/spi_ili9341"

# Optional DBI ST7796
make CHIP=bl616 BOARD=bl616dk DEFCONFIG=defconfig.dbi_st7796 BUILD_DIR="$PWD/build/dbi_st7796"

# Optional DSI EK79007
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap DEFCONFIG=defconfig.dsi_ek79007 BUILD_DIR="$PWD/build/dsi_ek79007"
```

Use a separate absolute `BUILD_DIR` when switching configurations, chips or boards: saved `generated/autoconfig.menuconfig` takes precedence over defconfig.

## Flash

After compiling, connect the programming UART and enter download mode; flash the default SPI BL616 build below, then reset the board.

```sh
make flash CHIP=bl616 BOARD=bl616dk DEFCONFIG=defconfig BUILD_DIR="$PWD/build/spi_ili9341" COMX=/dev/ttyACM0
```

For another configuration, reuse its build-time `DEFCONFIG`, `BUILD_DIR`, `CHIP`, `BOARD` and `CPU_ID` (if applicable), and adjust `COMX` to the actual port.

## Rotation

For GRAM displays, enable `CONFIG_LVGL8_DISP_DEFAULT_SW_ROTATE=y` for software rotation,
then call `lv_disp_set_rotation(NULL, LV_DISP_ROT_90)`.
VIDEO displays do not support rotation; LVGL v8 software rotation is incompatible with full refresh.

## Reference

See the [display framework documentation](../../../bsp/common/display/README.md) for further details.
