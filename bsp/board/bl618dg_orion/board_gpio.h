#ifndef BOARD_BL618DG_ORION_GPIO_H
#define BOARD_BL618DG_ORION_GPIO_H

#include <stdbool.h>
#include "bflb_gpio.h"
#include "board_config.h"

#ifdef CONFIG_FREERTOS
int board_shared_i2c_init(void);
int board_shared_i2c_acquire(bool hardware);
void board_shared_i2c_release(void);
#endif

#ifdef BL618DG
void board_ap_console_gpio_init(void);
void board_i2s_gpio_init(void);
void board_i2s_codec_gpio_init(void);
#endif

void board_audio_pll_config_for_rate(unsigned int samplerate_hz);

#endif
