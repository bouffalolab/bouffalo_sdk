#include <FreeRTOS.h>
#include <ring_buffer.h>
#include <bflb_uart.h>
#include <mem.h>

#include <openthread_port.h>
#include <utils/uart.h>

#ifndef OT_SERIAL_BUFFER_SIZE
#define OT_SERIAL_BUFFER_SIZE 512
#endif

#ifndef OT_SERIAL_LINE_SIZE
#define OT_SERIAL_LINE_SIZE 256
#endif

static struct bflb_device_s *uart_handle = NULL;
static Ring_Buffer_Type uart_rb;
static uint8_t uart_buffer[OT_SERIAL_BUFFER_SIZE];
static uint8_t uart_line[OT_SERIAL_LINE_SIZE];

void uart_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_uart_get_intstatus(uart_handle);
    if (intstatus & UART_INTSTS_RX_FIFO) {
        while (bflb_uart_rxavailable(uart_handle)) {
            Ring_Buffer_Write_Byte(&uart_rb, bflb_uart_getchar(uart_handle));
        }
    }
    if (intstatus & UART_INTSTS_RTO) {
        while (bflb_uart_rxavailable(uart_handle)) {
            Ring_Buffer_Write_Byte(&uart_rb, bflb_uart_getchar(uart_handle));
        }
        bflb_uart_int_clear(uart_handle, UART_INTCLR_RTO);
    }

    if ((intstatus & UART_INTSTS_RTO) || Ring_Buffer_Get_Empty_Length(&uart_rb) == 0) {
        otrNotifyEvent(OT_SYSTEM_EVENT_SERIAL_RX_DONE);
    }
}

void ot_uart_init(void *uart)
{
    uart_handle = (struct bflb_device_s *)uart;
    bflb_uart_rxint_mask(uart_handle, false);
    bflb_irq_attach(uart_handle->irq_num, uart_isr, NULL);
    bflb_irq_enable(uart_handle->irq_num);

    Ring_Buffer_Init(&uart_rb, uart_buffer, sizeof(uart_buffer), NULL, NULL);
}

void ot_serialProcess(ot_system_event_t sevent) 
{
    int available_len = 0;

    if (OT_SYSTEM_EVENT_SERIAL_RX_DONE & sevent) {
        available_len = Ring_Buffer_Get_Length(&uart_rb);
        for (int i = 0; i < available_len; i ++) {
            Ring_Buffer_Read_Byte(&uart_rb, &uart_line[i]);
        }

        if (available_len) {
            otPlatUartReceived(uart_line, available_len);
        }
    }
}

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
    bflb_uart_put(uart_handle, (uint8_t *)aBuf, aBufLength);

    otPlatUartSendDone();
    return OT_ERROR_NONE;
}

otError otPlatUartFlush(void)
{
    return OT_ERROR_NONE;
}
