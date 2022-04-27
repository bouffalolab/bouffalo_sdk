**board/bl706_iot/pinmux_config.h** 中 **CONFIG_GPIO18_FUNC**  选择 **GPIO_FUN_UART1_TX**, **CONFIG_GPIO19_FUNC**  选择 **GPIO_FUN_UART1_RX**

```bash

$ make APP=dma_uart_p2p BOARD=bl706_iot

```