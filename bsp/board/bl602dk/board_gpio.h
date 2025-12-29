#ifndef _BOARD_GPIO_H_
#define _BOARD_GPIO_H_

#if __has_include("board_gpio_overlay.h")
#include "board_gpio_overlay.h"
#else

void board_uartx_gpio_init(void);

#endif

#endif
