#ifndef _BOARD_GPIO_H_
#define _BOARD_GPIO_H_

#include <stdint.h>

#include "bflb_gpio.h"

/** @brief Board-specific low-power GPIO wakeup configuration. */
struct board_lp_gpio_wakeup_config_s {
    uint64_t io_ie;                          /*!< GPIO input-enable mask */
    uint64_t io_pu;                          /*!< GPIO pull-up mask */
    uint64_t io_pd;                          /*!< GPIO pull-down mask */
    uint8_t io_0_36_trig_mode[GPIO_PIN_MAX]; /*!< Per-GPIO trigger mode */
    uint64_t io_wakeup_unmask;               /*!< GPIO wakeup-enable mask */
};

/** @brief Return the board-specific low-power GPIO wakeup configuration. */
const struct board_lp_gpio_wakeup_config_s *board_lp_gpio_wakeup_config_get(void);

#if __has_include("board_gpio_overlay.h")
#include "board_gpio_overlay.h"
#else

void board_uartx_gpio_init(void);
void board_i2c0_gpio_init(void);
void board_spi0_gpio_init(void);
void board_pwm_gpio_init(void);
void board_adc_gpio_init(void);
void board_dac_gpio_init(void);
void board_emac_rmii_gpio_init(uint8_t port);
void board_emac_mdio_gpio_init(uint8_t port);
void board_sdh_gpio_init(void);
void board_sdio_gpio_init(void);
void board_sdio_gpio_deinit(void);
void board_ir_gpio_init(void);
void board_dvp_gpio_init(void);
void board_i2s_gpio_init(void);
void board_timer_gpio_init(void);

#define BOARD_DVP_USE_I2C BFLB_NAME_I2C0

#define BOARD_USB_VIA_GPIO
void board_usb_gpio_init(void);

#define PEC_I2C_SCL_PIN  GPIO_PIN_8
#define PEC_I2C_SDA_PIN  GPIO_PIN_9
void board_pec_i2c_gpio_init(void);

#define PEC_IR_PIN       GPIO_PIN_9
void board_pec_ir_gpio_init(void);

#define PEC_IR_RX_PIN    GPIO_PIN_10
void board_pec_ir_rx_gpio_init(void);

#define PEC_PWM_CH0_PIN  GPIO_PIN_8
#define PEC_PWM_CH1_PIN  GPIO_PIN_9
void board_pec_pwm_gpio_init(void);

#define PEC_SPI_SCK_PIN  GPIO_PIN_8
#define PEC_SPI_MOSI_PIN GPIO_PIN_9
#define PEC_SPI_MISO_PIN GPIO_PIN_10
void board_pec_spi_gpio_init(void);

#define PEC_UART_TX_PIN  GPIO_PIN_8
#define PEC_UART_RX_PIN  GPIO_PIN_9
void board_pec_uart_gpio_init(void);

#define PEC_QSPI_CAM_D0_PIN   GPIO_PIN_24
#define PEC_QSPI_CAM_D1_PIN   GPIO_PIN_25
#define PEC_QSPI_CAM_D2_PIN   GPIO_PIN_26
#define PEC_QSPI_CAM_D3_PIN   GPIO_PIN_27
#define PEC_QSPI_CAM_PCLK_PIN GPIO_PIN_28
void board_pec_qspi_cam_gpio_init(void);

#define PEC_DVP_CAM_VSYNC_PIN GPIO_PIN_23
#define PEC_DVP_CAM_HSYNC_PIN GPIO_PIN_22
#define PEC_DVP_CAM_PCLK_PIN  GPIO_PIN_21
#define PEC_DVP_CAM_DATA0_PIN GPIO_PIN_26
void board_pec_dvp_cam_gpio_init(void);

#define PEC_DPI_DATA0_PIN  GPIO_PIN_16
#define PEC_DPI_DATA1_PIN  GPIO_PIN_17
#define PEC_DPI_DATA2_PIN  GPIO_PIN_18
#define PEC_DPI_DATA3_PIN  GPIO_PIN_19
#define PEC_DPI_DATA4_PIN  GPIO_PIN_20
#define PEC_DPI_DATA5_PIN  GPIO_PIN_21
#define PEC_DPI_DATA6_PIN  GPIO_PIN_22
#define PEC_DPI_DATA7_PIN  GPIO_PIN_23
#define PEC_DPI_PCLK_PIN   GPIO_PIN_12
#define PEC_DPI_HSYNC_PIN  GPIO_PIN_14
#define PEC_DPI_VSYNC_PIN  GPIO_PIN_15
#define PEC_DPI_DE_PIN     GPIO_PIN_13
void board_pec_dpi_gpio_init(void);

#define PEC_SRGB_DATA0_PIN GPIO_PIN_16
#define PEC_SRGB_DATA1_PIN GPIO_PIN_17
#define PEC_SRGB_DATA2_PIN GPIO_PIN_18
#define PEC_SRGB_DATA3_PIN GPIO_PIN_19
#define PEC_SRGB_DATA4_PIN GPIO_PIN_20
#define PEC_SRGB_DATA5_PIN GPIO_PIN_21
#define PEC_SRGB_DATA6_PIN GPIO_PIN_22
#define PEC_SRGB_DATA7_PIN GPIO_PIN_23
#define PEC_SRGB_CLK_PIN   GPIO_PIN_12
#define PEC_SRGB_HSYNC_PIN GPIO_PIN_14
#define PEC_SRGB_VSYNC_PIN GPIO_PIN_15
#define PEC_SRGB_DE_PIN    GPIO_PIN_13
void board_pec_srgb_gpio_init(void);

void board_audio_pll_config_for_rate(unsigned int samplerate_hz);
void board_audac_gpio_init(void);
void board_auadc_gpio_init(void);
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
