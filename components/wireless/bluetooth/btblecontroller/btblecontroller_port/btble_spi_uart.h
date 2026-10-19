#ifndef __BTBLE_SPI_TO_UART_H__
#define __BTBLE_SPI_TO_UART_H__

#include <stdint.h>

void    btble_spi_uart_init(void);
uint32_t btble_spi_uart_get_rx_count(void);
uint32_t btble_spi_uart_read(uint8_t *data, uint32_t len);
void btble_spi_uart_write(uint8_t *data, uint32_t len);
void btble_spi_rx_copy(uint8_t *data, uint32_t len);
#endif
