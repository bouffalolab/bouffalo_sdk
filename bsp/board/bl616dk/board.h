#ifndef _BOARD_H
#define _BOARD_H

#if __has_include("board_overlay.h")
#include "board_overlay.h"
#else

#include "board_gpio.h"

void board_init(void);

void board_acomp_init(void);
void board_bldc_pre_init(void);

#define DEFAULT_ACOMP_VREF_1V65          33

#define DEFAULT_TEST_UART                "uart1"
#define DEFAULT_TEST_UART_DMA_TX_REQUEST DMA_REQUEST_UART1_TX
#define DEFAULT_TEST_UART_DMA_RX_REQUEST DMA_REQUEST_UART1_RX
#define DEFAULT_TEST_UART_DMA_TDR        DMA_ADDR_UART1_TDR
#define DEFAULT_TEST_UART_DMA_RDR        DMA_ADDR_UART1_RDR
#ifdef LP_APP
void board_recovery(void);
void cmd_io_test(char *buf, int len, int argc, char **argv);
void cmd_acomp_test(char *buf, int len, int argc, char **argv);
#endif
#endif

#endif
