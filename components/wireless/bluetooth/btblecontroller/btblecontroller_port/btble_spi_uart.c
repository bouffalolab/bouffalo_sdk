#include "btble_spi_uart.h"
#include "ring_buffer.h"
#include "nxspi_hci.h"
#include "ring_buffer.h"


Ring_Buffer_Type spiRB;
static uint8_t spiBuf[1024];

__attribute__((__weak__)) void btble_spi_uart_rx_event(void)
{

}

__attribute__((__weak__)) void btble_spi_uart_tx_event(void)
{

}

void btble_spi_uart_init(void)
{
    Ring_Buffer_Init(&spiRB, spiBuf, sizeof(spiBuf), NULL, NULL);
}

uint32_t btble_spi_uart_get_rx_count(void)
{
    uint32_t len = Ring_Buffer_Get_Length(&spiRB);
    return len;
}

void btble_spi_rx_copy(uint8_t *data, uint32_t len)
{
    int ret = Ring_Buffer_Write(&spiRB, data, len);
    #if 0
    printf("Ring_Buffer_Write ret=%d\n", ret);
    printf("Ring buffer status:%d", 
           Ring_Buffer_Get_Status(&spiRB));
    #endif
}


uint32_t btble_spi_uart_read(uint8_t *data, uint32_t len)
{
    uint32_t cnt;

    cnt = Ring_Buffer_Get_Length(&spiRB);

    if(cnt < len){
        len = cnt;
    }

    Ring_Buffer_Read(&spiRB, data, len);
    return len;
}

void btble_spi_uart_write(uint8_t *data, uint32_t len)
{
    spi_hci_write(data,len);
    btble_spi_uart_tx_event();
}
