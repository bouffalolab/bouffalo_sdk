
#include <openthread_port.h>
#include <utils/uart.h>

#include <nxspi.h>

otError otPlatUartEnable(void)
{
    return OT_ERROR_NONE;
}

otError otPlatUartDisable(void)
{
    return OT_ERROR_NONE;
}

otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    nxspi_write(NXSPI_TYPE_OT, (uint8_t *)aBuf, aBufLength, portMAX_DELAY);
    otPlatUartSendDone();
    return OT_ERROR_NONE;
}

otError otPlatUartFlush(void)
{
    return OT_ERROR_NONE;
}

void ot_nxspi_notify_rxd(void)
{
    otrNotifyEvent(OT_SYSTEM_EVENT_UART_RXD);
}

void ot_serialProcess(ot_system_event_t sevent) 
{
    uint8_t rxd_buf[256];

    if (OT_SYSTEM_EVENT_UART_RXD & sevent) {
        int rxd_len = nxspi_read(NXSPI_TYPE_OT, rxd_buf, sizeof(rxd_buf)/sizeof(rxd_buf[0]), 0);
        if (rxd_len > 0) {
            otPlatUartReceived(rxd_buf, rxd_len);

            /** try one more time to exhaust nxspi readbuf */
            otrNotifyEvent(OT_SYSTEM_EVENT_UART_RXD);
        }
    }
}
