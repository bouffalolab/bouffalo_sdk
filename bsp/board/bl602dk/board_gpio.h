#ifndef _BOARD_GPIO_H_
#define _BOARD_GPIO_H_

#if __has_include("board_gpio_overlay.h")
#include "board_gpio_overlay.h"
#else

void board_uartx_gpio_init(void);

#endif
#if defined(CONFIG_BSP_LCD)

void board_lcd_spi_0_hard_4_gpio_init(void);
#if defined(GPIO_FUNC_SPI1)
void board_lcd_spi_1_hard_4_gpio_init(void);
#endif
#endif
#endif
