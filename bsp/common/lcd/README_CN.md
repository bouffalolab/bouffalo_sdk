> [!WARNING]
> `BSP_LCD` 及本旧版 LCD 框架目前仅为兼容现有项目而暂时保留。
> 请尽快迁移到 `CONFIG_BSP_DISPLAY` 和新的 Display 框架。
> 该旧版框架和目录将在下一个大版本中删除。
> 请勿同时启用 `CONFIG_BSP_LCD` 和 `CONFIG_BSP_DISPLAY`。

# 迁移到新的 Display 框架

新的 Display 框架位于 `bsp/common/display`。新项目应启用
`CONFIG_BSP_DISPLAY`，并使用该框架提供的显示总线、面板和默认显示配置。
请勿同时启用 `CONFIG_BSP_LCD` 和 `CONFIG_BSP_DISPLAY`。

典型迁移步骤如下：

1. 将 `CONFIG_BSP_LCD=y` 替换为 `CONFIG_BSP_DISPLAY=y`。
2. 将 `lcd_conf_user.h` 中的 LCD 配置迁移到对应的 Kconfig 选项，包括显示总线、
	屏幕模组、像素格式、引脚、复位和背光配置。
3. 将旧版板级 LCD GPIO hook 替换为新 Display 框架要求的板级 hook。
4. 将应用代码更新为新的显示 API，并使用新的 LVGL 默认头文件，例如
	`lv_port_disp_default.h`。
5. 参考已迁移的 [`examples/display`](../../../examples/display) 示例，完成配置和应用代码迁移。

框架详细文档请参阅
[`bsp/common/display/README.md`](../display/README.md)。现有项目可以暂时继续使用旧框架，
但应尽快完成迁移，因为旧框架将在下一个大版本中删除。

对于规模较大或定制较多的项目，可以使用 AI 编程助手检查现有 LCD 配置和应用代码，
将其映射到新的 Display 框架，并通过独立构建验证迁移结果。