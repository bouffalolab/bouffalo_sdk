#ifndef BOARD_BL618DG_MSTAR_GPIO_OVERLAY_H
#define BOARD_BL618DG_MSTAR_GPIO_OVERLAY_H

#include <stdint.h>

#include "bflb_gpio.h"

void board_ap_console_gpio_init(void);
void board_np_console_gpio_init(void);
void board_lp_console_gpio_init(void);

void board_i2c0_gpio_init(void);
void board_audio_pll_config_for_rate(unsigned int samplerate_hz);
void board_i2s_codec_gpio_init(void);
void board_audac_gpio_init(void);
void board_auadc_gpio_init(void);

#define BOARD_USB_VIA_GPIO
void board_usb_gpio_init(void);

#endif
