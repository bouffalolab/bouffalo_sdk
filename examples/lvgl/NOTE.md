# NOTE: Legacy LVGL Examples

This directory contains the legacy LVGL examples based on `BSP_LCD`.

The legacy `BSP_LCD` framework and the examples under `examples/lvgl` are kept temporarily for source compatibility with existing projects. This directory is scheduled for removal in the next major SDK release.

For new projects, use `CONFIG_BSP_DISPLAY` and the examples under [`examples/display`](../display). Do not enable `CONFIG_BSP_LCD` and `CONFIG_BSP_DISPLAY` at the same time.

## Migration

For a typical migration:

1. Replace `CONFIG_BSP_LCD=y` with `CONFIG_BSP_DISPLAY=y`.
2. Move the LCD settings from `lcd_conf_user.h` to the corresponding Kconfig
	options for the display bus, panel module, pixel format, pins, reset, and
	backlight.
3. Replace legacy board LCD GPIO hooks with the board hooks required by the new
	Display framework.
4. Update application code to use the new display APIs and
	`lv_port_disp_default.h`.
5. Use the migrated examples under [`examples/display`](../display) as references.

See the [Display framework documentation](../../bsp/common/display/README.md)
for details. For large or customized projects, an AI coding assistant can help
inspect the existing LCD configuration, map it to the new framework, and verify
the result with a clean build.
