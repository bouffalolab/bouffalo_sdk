#ifndef _BOARD_GPIO_OVERLAY_H_
#define _BOARD_GPIO_OVERLAY_H_

#include <stdint.h>
#include "bflb_gpio.h"

/* Only interfaces wired on the BL616CL AVE schematic are exposed here. */
void board_i2c0_gpio_init(void);
void board_emac_rmii_gpio_init(uint8_t port);
void board_emac_mdio_gpio_init(uint8_t port);
void board_sdh_gpio_init(void);
void board_dvp_gpio_init(void);
void board_i2s_gpio_init(void);
void board_usb_gpio_init(void);

#define BOARD_DVP_USE_I2C BFLB_NAME_I2C0
#define BOARD_USB_VIA_GPIO

void board_audio_pll_config_for_rate(unsigned int samplerate_hz);
void board_i2s_codec_gpio_init(void);

#if defined(CONFIG_BSP_LCD)
void board_lcd_dbi_type_b_gpio_init(void);
#endif

#endif
