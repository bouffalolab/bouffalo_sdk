#ifndef _BOARD_H
#define _BOARD_H

#include "bflb_rtc.h"

/* Compatibility entry point; board startup is handled by SYSINIT. */
#if IS_ENABLED(CONFIG_COMPAT_V2)
void board_init(void) __attribute__((deprecated("handled by SYSINIT; will be removed in Bouffalo SDK v3")));
#endif /* IS_ENABLED(CONFIG_COMPAT_V2) */

/* Optional hardware hooks around the default board startup. */
void board_early_init(void);
void board_late_init(void);

enum bflb_rtc_32k_clk_type board_get_rtc_32k_clk_type(void);

void board_uartx_gpio_init();
void board_i2c0_gpio_init();
void board_spi0_gpio_init();
void board_adc_gpio_init();
void board_dac_gpio_init();
void board_dac_ref_gpio_init();
void board_emac_gpio_init();
void board_pwm_gpio_init();
void board_ir_gpio_init();
void board_sdio_gpio_init();
void board_sdio_gpio_deinit(void);

#define DEFAULT_TEST_UART                "uart1"
#define DEFAULT_TEST_UART_DMA_TX_REQUEST DMA_REQUEST_UART1_TX
#define DEFAULT_TEST_UART_DMA_RX_REQUEST DMA_REQUEST_UART1_RX
#define DEFAULT_TEST_UART_DMA_TDR        DMA_ADDR_UART1_TDR
#define DEFAULT_TEST_UART_DMA_RDR        DMA_ADDR_UART1_RDR

#endif
