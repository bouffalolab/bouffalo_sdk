#ifndef PM_DEMO_CLI_HELPER_H
#define PM_DEMO_CLI_HELPER_H

#include <stdint.h>

#include "board_gpio.h"

#if defined(CONFIG_EXAMPLE_PM_GPIO_WAKEUP)
int pm_demo_cli_gpio_init(
    const struct board_lp_gpio_wakeup_config_s *board_config,
    const struct board_kys_v2_gpio_config_s *keyscan_config);
int pm_demo_cli_gpio_apply(void);
const struct board_lp_gpio_wakeup_config_s *pm_demo_cli_gpio_get(void);
#endif

#if defined(CONFIG_EXAMPLE_PM_BOD_WAKEUP)
int pm_demo_cli_bod_init(void);
#endif

#endif
