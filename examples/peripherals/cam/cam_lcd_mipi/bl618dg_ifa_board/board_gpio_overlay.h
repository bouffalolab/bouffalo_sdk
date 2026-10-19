#ifndef _BOARD_GPIO_OVERLAY_H_
#define _BOARD_GPIO_OVERLAY_H_

#include <stdint.h>

#include "bflb_gpio.h"

void board_uartx_gpio_init(void);
void board_ap_console_gpio_init(void);
void board_np_console_gpio_init(void);
void board_lp_console_gpio_init(void);
void board_i2c0_gpio_init(void);
void board_spi0_gpio_init(void);
void board_pwm_gpio_init(void);
void board_adc_gpio_init(void);
void board_audac_gpio_init(void);
void board_auadc_gpio_init(void);
void board_audio_pll_config_for_rate(unsigned int samplerate_hz);
void board_emac_rmii_gpio_init(uint8_t port);
void board_emac_mdio_gpio_init(uint8_t port);
void board_sdh_gpio_init(void);
void board_sdio_gpio_init(void);
void board_sdio_gpio_deinit(void);
void board_ir_gpio_init(void);
void board_dvp_gpio_init(void);
void board_dpi_gpio_init(void);
void board_i2s_gpio_init(void);
void board_timer_gpio_init(void);
void board_canfd_gpio_init(int idx);

#define BOARD_DVP_USE_I2C BFLB_NAME_I2C0

#define BOARD_USB_VIA_GPIO
void board_usb_gpio_init(void);

void board_i2s_codec_gpio_init(void);

#if defined(CONFIG_BSP_LCD)
void board_lcd_dbi_type_c_4_wire_gpio_init(void);
void board_lcd_dbi_type_c_3_wire_gpio_init(void);
void board_lcd_dbi_type_b_gpio_init(void);
void board_lcd_dbi_ex_qspi_gpio_init(void);

void board_lcd_spi_0_hard_4_gpio_init(void);
#if defined(GPIO_FUNC_SPI1)
void board_lcd_spi_1_hard_4_gpio_init(void);
#endif
#endif

#endif
