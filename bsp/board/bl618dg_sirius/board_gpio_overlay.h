#ifndef BOARD_BL618DG_SIRIUS_GPIO_OVERLAY_H
#define BOARD_BL618DG_SIRIUS_GPIO_OVERLAY_H

#include <stdint.h>
#include "bflb_gpio.h"

void board_ap_console_gpio_init(void);
void board_np_console_gpio_init(void);
void board_uartx_gpio_init(void);
void board_rs485_gpio_init(void);
void board_rs485_rxonly_gpio_init(void);
void board_i2c0_gpio_init(void);
void board_i2c2_gpio_init(void);
void board_ws2812_gpio_init(void);
void board_touch_reset_gpio_init(void);
void board_touch_int_gpio_init(void);
void board_adc_gpio_init(void);
void board_audac_gpio_init(void);
void board_lcd_backlight_gpio_init(void);
void board_dvp_gpio_init(void);
void board_audio_pll_config_for_rate(unsigned int samplerate_hz);
void board_i2s_codec_gpio_init(void);
void board_i2s_gpio_init(void);
void board_canfd_gpio_init(int idx);
void board_sdh_gpio_init(void);
void board_sdio_gpio_init(void);
void board_sdio_gpio_deinit(void);

#define BOARD_DVP_USE_I2C BFLB_NAME_I2C0
#define BOARD_USB_VIA_GPIO
void board_usb_gpio_init(void);

#endif
