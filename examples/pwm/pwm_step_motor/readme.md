**board/bl706_iot/pinmux_config.h** 中, 以下宏设置成 **GPIO_FUN_PWM**

- **CONFIG_GPIO10_FUNC**
- **CONFIG_GPIO11_FUNC** 
- **CONFIG_GPIO12_FUNC**
- **CONFIG_GPIO3_FUNC**

**board/bl706_iot/clock_config.h** 中 

- **BSP_PWM_CLOCK_SOURCE** 选择 **ROOT_CLOCK_SOURCE_RC_32K**,
- **BSP_PWM_CLOCK_DIV** 设置成 **32**

```bash

$ make APP=pwm_it BOARD=bl706_iot

```