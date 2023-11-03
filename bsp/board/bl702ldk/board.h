#ifndef _BOARD_H
#define _BOARD_H

void board_init(void);

void board_uartx_gpio_init();
void board_i2c0_gpio_init();
void board_spi0_gpio_init();
void board_adc_gpio_init();
void board_pwm_gpio_init();
void board_ir_gpio_init(void);
void board_keyscan_gpio_init(void);

#define DEFAULT_TEST_UART "uart0"
#define DEFAULT_TEST_UART_DMA_TX_REQUEST DMA_REQUEST_UART0_TX
#define DEFAULT_TEST_UART_DMA_RX_REQUEST DMA_REQUEST_UART0_RX
#define DEFAULT_TEST_UART_DMA_TDR DMA_ADDR_UART0_TDR
#define DEFAULT_TEST_UART_DMA_RDR DMA_ADDR_UART0_RDR

#endif