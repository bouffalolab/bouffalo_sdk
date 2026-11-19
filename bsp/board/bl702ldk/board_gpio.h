#ifndef _BOARD_GPIO_H_
#define _BOARD_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_BSP_DISPLAY)
void board_display_spi_gpio_init(void);
void board_display_spi_gpio_deinit(void);
#endif

#ifdef __cplusplus
}
#endif

#endif