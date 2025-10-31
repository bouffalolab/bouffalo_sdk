#ifndef _BOARD_GPIO_OVERLAY_H_
#define _BOARD_GPIO_OVERLAY_H_

#include "bflb_gpio.h"

void board_uartx_gpio_init(void);
void board_i2c0_gpio_init(void);
void board_spi0_gpio_init(void);
void board_pwm_gpio_init(void);
void board_adc_gpio_init(void);
void board_dac_gpio_init(void);
void board_emac_gpio_init(void);
void board_sdh_gpio_init(void);
void board_sdio_gpio_init(void);
void board_ir_gpio_init(void);
void board_dvp_gpio_init(void);
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

#endif
