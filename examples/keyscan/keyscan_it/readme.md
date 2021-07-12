**board/bl706_iot/pinmux_config.h** 中, 以下宏设置成 **GPIO_FUN_KEY_SCAN_ROW**

- **CONFIG_GPIO16_FUNC**
- **CONFIG_GPIO17_FUNC**
- **CONFIG_GPIO18_FUNC**
- **CONFIG_GPIO19_FUNC**

**board/bl706_iot/pinmux_config.h** 中, 以下宏设置成 **GPIO_FUN_KEY_SCAN_COL**

- **CONFIG_GPIO0_FUNC**
- **CONFIG_GPIO1_FUNC**
- **CONFIG_GPIO2_FUNC**
- **CONFIG_GPIO3_FUNC**

```bash

$ make APP=keyscan_it BOARD=bl706_iot

```