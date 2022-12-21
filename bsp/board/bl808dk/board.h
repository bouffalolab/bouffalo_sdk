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
void board_iso11898_gpio_init(void);

#define DEFAULT_TEST_UART                "uart1"
#define DEFAULT_TEST_UART_DMA_TX_REQUEST DMA_REQUEST_UART1_TX
#define DEFAULT_TEST_UART_DMA_RX_REQUEST DMA_REQUEST_UART1_RX
#define DEFAULT_TEST_UART_DMA_TDR        DMA_ADDR_UART1_TDR
#define DEFAULT_TEST_UART_DMA_RDR        DMA_ADDR_UART1_RDR

#endif