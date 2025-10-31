#ifndef _BOARD_H
#define _BOARD_H

void board_init(void);

void board_jtag_gpio_init(void);
void board_uartx_gpio_init(void);
void board_i2c0_gpio_init(void);
void board_spi0_gpio_init(void);
void board_pwm_gpio_init(void);
void board_adc_gpio_init(void);
void board_dac_gpio_init(void);
void board_ir_gpio_init(void);
void board_emac_gpio_init(void);
void board_sdh_gpio_init(void);
void board_dvp_gpio_init(void);
void board_csi_gpio_init(void);
void board_timer_gpio_init(void);
void board_touch_gpio_init(void);

#define APP_PWM_TRI_CH_U  (PWM_CH0)
#define APP_PWM_TRI_CH_V  (PWM_CH1)
#define APP_PWM_TRI_CH_W  (PWM_CH2)
#define APP_PWM_PIN_U_H   (GPIO_PIN_16)
#define APP_PWM_PIN_U_L   (GPIO_PIN_17)
#define APP_PWM_PIN_V_H   (GPIO_PIN_18)
#define APP_PWM_PIN_V_L   (GPIO_PIN_19)
#define APP_PWM_PIN_W_H   (GPIO_PIN_20)
#define APP_PWM_PIN_W_L   (GPIO_PIN_21)
#define APP_HALL_PIN_U    (GPIO_PIN_0)
#define APP_HALL_PIN_V    (GPIO_PIN_1)
#define APP_HALL_PIN_W    (GPIO_PIN_2)
#define APP_KEY_0         (GPIO_PIN_35)
#define APP_KEY_1         (GPIO_PIN_36)
#define APP_KEY_2         (GPIO_PIN_37)
#define APP_CURRENT_CH_U  (ADC_CHANNEL_1)
#define APP_CURRENT_CH_V  (ADC_CHANNEL_2)
#define APP_CURRENT_CH_W  (ADC_CHANNEL_3)
#define APP_CURRENT_PIN_U (GPIO_PIN_5)
#define APP_CURRENT_PIN_V (GPIO_PIN_4)
#define APP_CURRENT_PIN_W (GPIO_PIN_11)
#define APP_TEST_PIN      (GPIO_PIN_38)

#define DEFAULT_TEST_UART                "uart1"
#define DEFAULT_TEST_UART_DMA_TX_REQUEST DMA_REQUEST_UART1_TX
#define DEFAULT_TEST_UART_DMA_RX_REQUEST DMA_REQUEST_UART1_RX
#define DEFAULT_TEST_UART_DMA_TDR        DMA_ADDR_UART1_TDR
#define DEFAULT_TEST_UART_DMA_RDR        DMA_ADDR_UART1_RDR

#define CONFIG_D0_FLASH_ADDR             0x100000
#define CONFIG_LP_FLASH_ADDR             0x20000

#endif