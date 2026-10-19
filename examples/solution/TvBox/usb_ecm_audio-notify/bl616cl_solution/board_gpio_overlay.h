#ifndef USB_ECM_AUDIO_BL616CL_SOLUTION_BOARD_GPIO_OVERLAY_H
#define USB_ECM_AUDIO_BL616CL_SOLUTION_BOARD_GPIO_OVERLAY_H

#include <stdint.h>

#include "bflb_gpio.h"

#define BOARD_USB_VIA_GPIO

void board_i2c0_gpio_init(void);
void board_emac_rmii_gpio_init(uint8_t port);
void board_emac_mdio_gpio_init(uint8_t port);
void board_usb_gpio_init(void);
void board_audio_pll_config_for_rate(unsigned int samplerate_hz);
void board_i2s_codec_gpio_init(void);

#endif
