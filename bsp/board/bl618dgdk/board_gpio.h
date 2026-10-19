#ifndef _BOARD_GPIO_H_
#define _BOARD_GPIO_H_

#include <stdint.h>

/** @brief Board-specific KYS v2 matrix GPIO mapping. */
struct board_kys_v2_gpio_config_s {
    uint8_t row_count;       /*!< Number of KEY_SCAN_DRIVE rows */
    uint8_t col_count;       /*!< Number of KEY_SCAN_IN columns */
    const uint8_t *row_pins; /*!< KEY_SCAN_DRIVE GPIOs */
    const uint8_t *col_pins; /*!< KEY_SCAN_IN GPIOs */
};

/** @brief Return the board-specific KYS v2 matrix GPIO mapping. */
const struct board_kys_v2_gpio_config_s *board_kys_v2_gpio_config_get(void);

/** @brief Board-specific low-power GPIO wakeup configuration. */
struct board_lp_gpio_wakeup_config_s {
    uint64_t io_ie;             /*!< GPIO input-enable mask */
    uint64_t io_pu;             /*!< GPIO pull-up mask */
    uint64_t io_pd;             /*!< GPIO pull-down mask */
    uint64_t io_wakeup_unmask;  /*!< GPIO wakeup-enable mask */
    uint8_t io_0_7_trig_mode;   /*!< Shared trigger mode for GPIO0-GPIO7 */
    uint8_t io_8_15_trig_mode;  /*!< Shared trigger mode for GPIO8-GPIO15 */
    uint8_t io_16_23_trig_mode; /*!< Shared trigger mode for GPIO16-GPIO23 */
    uint8_t io_24_31_trig_mode; /*!< Shared trigger mode for GPIO24-GPIO31 */
    uint8_t io_32_39_trig_mode; /*!< Shared trigger mode for GPIO32-GPIO39 */
    uint8_t io_40_47_trig_mode; /*!< Shared trigger mode for GPIO40-GPIO47 */
    uint8_t io_48_52_trig_mode; /*!< Shared trigger mode for GPIO48-GPIO52 */
};

/** @brief Return the board-specific low-power GPIO wakeup configuration. */
const struct board_lp_gpio_wakeup_config_s *board_lp_gpio_wakeup_config_get(void);

#if defined(CONFIG_BSP_LED_MATRIX)
#include "bflb_led_matrix.h"

/** @brief Return the board-specific LED matrix wiring configuration. */
const led_matrix_gpio_config_t *board_led_matrix_gpio_config_get(void);
#endif

#if __has_include("board_gpio_overlay.h")
#include "board_gpio_overlay.h"
#else

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

#define BOARD_DVP_USE_I2C BFLB_NAME_I2C1

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

#endif
