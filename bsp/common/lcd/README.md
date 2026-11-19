> [!WARNING]
> `BSP_LCD` and this legacy LCD framework are retained temporarily for source compatibility only.
> Please migrate to `CONFIG_BSP_DISPLAY` and the new Display framework as soon as possible.
> This legacy framework and directory will be removed in the next major SDK release.
> Do not enable `CONFIG_BSP_LCD` and `CONFIG_BSP_DISPLAY` at the same time.

# Migrating to the Display Framework

The new Display framework is provided by `bsp/common/display`. New projects should
enable `CONFIG_BSP_DISPLAY` and use the display bus, panel, and default-display
configuration provided by that framework. Do not enable `CONFIG_BSP_LCD` and
`CONFIG_BSP_DISPLAY` together.

For a typical migration:

1. Replace `CONFIG_BSP_LCD=y` with `CONFIG_BSP_DISPLAY=y`.
2. Move LCD selections from `lcd_conf_user.h` to the corresponding Kconfig
	options, including the bus, panel module, pixel format, pins, reset, and
	backlight settings.
3. Replace legacy board LCD GPIO hooks with the board hooks required by the new
	Display framework.
4. Update application code to use the new display APIs and the default LVGL
	headers, such as `lv_port_disp_default.h`.
5. Use the migrated examples under [`examples/display`](../../../examples/display)
	as configuration and application references.

The framework documentation is available in
[`bsp/common/display/README.md`](../display/README.md). Existing projects may
continue using this legacy framework temporarily, but migration should be done
as soon as possible because it will be removed in the next major SDK release.

For large or customized projects, consider using an AI coding assistant to
inspect the existing LCD configuration and application code, map it to the new
Display framework, and verify the migration with a clean build.