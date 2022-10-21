#ifndef _BOARD_H
#define _BOARD_H

void board_init(void);

void board_jtag_gpio_init(void);
void board_uart1_gpio_init(void);
void board_i2c0_gpio_init(void);
void board_spi0_gpio_init(void);
void board_pwm0_gpio_init(void);
void board_adc_gpio_init(void);
void board_dac_gpio_init(void);
void board_ir_gpio_init(void);
void board_emac_gpio_init(void);

#ifdef CONFIG_BSP_SDH_SDCARD
void board_sdh_gpio_init(void);
#endif

#endif