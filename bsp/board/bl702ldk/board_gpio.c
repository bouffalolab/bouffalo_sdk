/* SPDX-License-Identifier: Apache-2.0 */
#if __has_include("board_gpio_overlay.h")
/* Use board_gpio_overlay.c instead of this file */
#else

#include "board_gpio.h"
#include "bflb_gpio.h"

#if defined(CONFIG_BSP_DISPLAY)
/* BL702L has no DBI controller or display processing unit. Only hardware SPI
 * is provided; D/C and CS remain software-controlled GPIOs. */
void board_display_spi_gpio_init(void)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    /* SPI_DAT (MOSI) */
    bflb_gpio_init(gpio, GPIO_PIN_15, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    /* SPI_CLK */
    bflb_gpio_init(gpio, GPIO_PIN_13, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_display_spi_gpio_deinit(void)
{
    /* Nothing to do when the display is released. */
}
#endif

#endif /* __has_include("board_gpio_overlay.h") */