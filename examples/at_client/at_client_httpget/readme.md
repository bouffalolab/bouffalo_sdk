**board/bl706_iot/pinmux_config.h** 中 **CONFIG_GPIO18_FUNC**  选择 **GPIO_FUN_UART1_TX**, **CONFIG_GPIO19_FUNC**  选择 **GPIO_FUN_UART1_RX**

- UART1_TX 连接 BL602 GPIO3 (UART RX)
- UART1_RX 连接 BL602 GPIO4 (UART TX)

```bash

$ make APP=at_client BOARD=bl706_iot

```