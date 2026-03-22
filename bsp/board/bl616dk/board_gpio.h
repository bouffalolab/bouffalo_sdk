#ifndef _BOARD_GPIO_H_
#define _BOARD_GPIO_H_

#if __has_include("board_gpio_overlay.h")
#include "board_gpio_overlay.h"
#else

void board_uartx_gpio_init(void);
void board_i2c0_gpio_init(void);
void board_spi0_gpio_init(void);
void board_pwm_gpio_init(void);
void board_adc_gpio_init(void);
void board_dac_gpio_init(void);
void board_auadc_gpio_init(void);
void board_audac_gpio_init(void);
void board_emac_gpio_init(void);
void board_sdh_gpio_init(void);
void board_sdio_gpio_init(void);
void board_ir_gpio_init(void);
void board_dvp_gpio_init(void);
void board_i2s_gpio_init(void);
void board_timer_gpio_init(void);

void board_i2s_codec_gpio_init(void);
void board_audio_pll_config_for_rate(unsigned int samplerate_hz);
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

#endif
