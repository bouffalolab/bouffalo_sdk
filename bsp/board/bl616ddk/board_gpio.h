#ifndef _BOARD_GPIO_H_
#define _BOARD_GPIO_H_

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
void board_dac_gpio_init(void);
void board_emac_gpio_init(void);
void board_emac1_gpio_init(void);
void board_sdh_gpio_init(void);
void board_sdio_gpio_init(void);
void board_ir_gpio_init(void);
void board_dvp_gpio_init(void);
void board_dpi_gpio_init(void);
void board_i2s_gpio_init(void);
void board_timer_gpio_init(void);

#define BOARD_USB_VIA_GPIO
void board_usb_gpio_init(void);

#define PEC_I2C_SCL_PIN  GPIO_PIN_6
#define PEC_I2C_SDA_PIN  GPIO_PIN_10
void board_pec_i2c_gpio_init(void);

#define PEC_IR_PIN       GPIO_PIN_10
void board_pec_ir_gpio_init(void);

#define PEC_PWM_CH0_PIN  GPIO_PIN_6
#define PEC_PWM_CH1_PIN  GPIO_PIN_10
#define PEC_PWM_CH2_PIN  GPIO_PIN_28
#define PEC_PWM_CH3_PIN  GPIO_PIN_31
void board_pec_pwm_gpio_init(void);

#define PEC_SPI_SCK_PIN  GPIO_PIN_6
#define PEC_SPI_MOSI_PIN GPIO_PIN_10
#define PEC_SPI_MISO_PIN GPIO_PIN_28
void board_pec_spi_gpio_init(void);

#define PEC_UART_TX_PIN  GPIO_PIN_6
#define PEC_UART_RX_PIN  GPIO_PIN_10
void board_pec_uart_gpio_init(void);

#define PEC_DPI_DATA0_PIN  GPIO_PIN_12
#define PEC_DPI_DATA1_PIN  GPIO_PIN_13
#define PEC_DPI_DATA2_PIN  GPIO_PIN_14
#define PEC_DPI_DATA3_PIN  GPIO_PIN_15
#define PEC_DPI_DATA4_PIN  GPIO_PIN_16
#define PEC_DPI_DATA5_PIN  GPIO_PIN_17
#define PEC_DPI_DATA6_PIN  GPIO_PIN_18
#define PEC_DPI_DATA7_PIN  GPIO_PIN_19
#define PEC_DPI_DATA8_PIN  GPIO_PIN_20
#define PEC_DPI_DATA9_PIN  GPIO_PIN_21
#define PEC_DPI_DATA10_PIN GPIO_PIN_22
#define PEC_DPI_DATA11_PIN GPIO_PIN_23
#define PEC_DPI_DATA12_PIN GPIO_PIN_24
#define PEC_DPI_DATA13_PIN GPIO_PIN_25
#define PEC_DPI_DATA14_PIN GPIO_PIN_26
#define PEC_DPI_DATA15_PIN GPIO_PIN_27
#define PEC_DPI_DATA16_PIN GPIO_PIN_28
#define PEC_DPI_DATA17_PIN GPIO_PIN_29
#define PEC_DPI_DATA18_PIN GPIO_PIN_30
#define PEC_DPI_DATA19_PIN GPIO_PIN_31
#define PEC_DPI_DATA20_PIN GPIO_PIN_32
#define PEC_DPI_DATA21_PIN GPIO_PIN_33
#define PEC_DPI_DATA22_PIN GPIO_PIN_43
#define PEC_DPI_DATA23_PIN GPIO_PIN_44
// #define PEC_DPI_PCLK_PIN   GPIO_PIN_45
// #define PEC_DPI_HSYNC_PIN  GPIO_PIN_46
// #define PEC_DPI_VSYNC_PIN  GPIO_PIN_47
// #define PEC_DPI_DE_PIN     GPIO_PIN_48
#define PEC_DPI_PCLK_PIN   GPIO_PIN_0
#define PEC_DPI_HSYNC_PIN  GPIO_PIN_1
#define PEC_DPI_VSYNC_PIN  GPIO_PIN_2
#define PEC_DPI_DE_PIN     GPIO_PIN_3
void board_pec_dpi_gpio_init(void);

void board_i2s_codec_gpio_init(void);

#endif

#endif