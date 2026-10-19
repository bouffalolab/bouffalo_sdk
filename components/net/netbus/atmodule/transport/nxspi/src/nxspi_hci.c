#include <stdio.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "bflb_mtimer.h"
#include <bflb_core.h>
#include "board.h"
#include <shell.h>
#include <assert.h>
#include "utils.h"
#include <nxspi.h>
#include "nxspi_hci.h"
#include "btble_spi_uart.h"
#include "btble_lib_api.h"

typedef struct {
    uint8_t *data;  
    uint32_t length;     
} hci_message_t;

#define NX_SPI_HCI_DBG  printf

int spihci_rx_task(void)
{
    int ret;
    hci_message_t msg;
    msg.data = pvPortMalloc(512);
    if (!msg.data) {
        NX_SPI_HCI_DBG("Memory allocation error.\r\n");
        return -1;
    }
    while (1) 
    {
        memset(msg.data, 0, 512);
        ret = nxspi_read(NXSPI_TYPE_HCI, msg.data, (512), -1);
        if (0 > ret) {
            continue;
        }else
        {
            msg.length = ret;
            NX_SPI_HCI_DBG("spi read\r\n");
            for (int i = 0; i < msg.length; i++) {
                NX_SPI_HCI_DBG("0x%02x ", ((char *)msg.data)[i]);
            }
            NX_SPI_HCI_DBG("\r\n");
            btble_spi_rx_copy(msg.data, msg.length);
            btble_spi_uart_rx_event();
        }
    }
    vPortFree(msg.data); 
}

int spi_hci_write(uint8_t *buf, uint32_t len)
{
    int res;
    res =nxspi_write(NXSPI_TYPE_HCI, buf,len, -1);
    NX_SPI_HCI_DBG("spi write\r\n");
    for (int i = 0; i < len; i++) {
        NX_SPI_HCI_DBG("0x%02x ", buf[i]);
    }
    NX_SPI_HCI_DBG("\r\n");
    return res;
}

void spihci_init(void)
{
    btble_spi_uart_init();
    btble_controller_init(configMAX_PRIORITIES - 1);
    xTaskCreate(spihci_rx_task, (char *)"nxspi_hci_rx", 1024, NULL, 25, NULL);
    return;
}
