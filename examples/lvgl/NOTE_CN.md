# NOTE：旧版 LVGL 示例

本目录包含基于 `BSP_LCD` 的旧版 LVGL 示例。

旧版 `BSP_LCD` 框架及 `examples/lvgl` 下的示例目前仅为兼容现有项目而暂时保留。本目录计划在下一个大版本中删除。

新项目请使用 `CONFIG_BSP_DISPLAY` 和 [`examples/display`](../display)。请勿同时启用 `CONFIG_BSP_LCD` 和 `CONFIG_BSP_DISPLAY`。

## 迁移说明

典型迁移步骤如下：

1. 将 `CONFIG_BSP_LCD=y` 替换为 `CONFIG_BSP_DISPLAY=y`。
2. 将 `lcd_conf_user.h` 中的 LCD 配置迁移到对应的 Kconfig 选项，包括显示总线、
	屏幕模组、像素格式、引脚、复位和背光配置。
3. 将旧版板级 LCD GPIO hook 替换为新 Display 框架要求的板级 hook。
4. 将应用代码更新为新的显示 API，并使用 `lv_port_disp_default.h`。
5. 参考已迁移的 [`examples/display`](../display) 示例。

详细信息请参阅 [Display 框架文档](../../bsp/common/display/README_CN.md)。对于规模较大
或定制较多的项目，可以使用 AI 编程助手检查现有 LCD 配置、映射到新框架，并通过独立
构建验证迁移结果。
