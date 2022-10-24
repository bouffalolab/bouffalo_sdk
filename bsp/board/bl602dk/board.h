#ifndef _BOARD_H
#define _BOARD_H

void board_init(void);

void board_uart1_gpio_init();
void board_i2c0_gpio_init();
void board_spi0_gpio_init();
void board_adc_gpio_init();
void board_dac_gpio_init();
void board_emac_gpio_init();
void board_pwm_gpio_init();
#endif