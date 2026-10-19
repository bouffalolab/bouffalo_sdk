#ifndef __BTBLE_DMA_UART_H__
#define __BTBLE_DMA_UART_H__

#include <stdint.h>

void btble_dma_uart_init(void);
uint32_t btble_dma_uart_get_rx_count(void);
uint32_t btble_dma_uart_read(uint8_t *data, uint32_t len);
void btble_dma_uart_write(uint8_t *data, uint32_t len);

#endif
