**board/bl706_iot/pinmux_config.h** 中 **CONFIG_GPIO22_FUNC** 选择 **GPIO_FUN_PWM**

**board/bl706_iot/clock_config.h** 中 

- **BSP_PWM_CLOCK_SOURCE** 选择 **ROOT_CLOCK_SOURCE_XCLK**,
- **BSP_PWM_CLOCK_DIV** 设置成 **1**

```bash

$ make APP=pwm_breath_led BOARD=bl706_iot

```