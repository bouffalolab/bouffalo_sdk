#include "bflb_hci_uart_common.h"
#include "bflb_hci_transport.h"
#include "hci_h4.h"
#include "bflb_uart.h"
#include "bflb_gpio.h"
#include "bt_log.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

static uint8_t uart_initialized = 0;
static struct bflb_device_s *uartx = NULL;
static StreamBufferHandle_t xStreamBufferRX = NULL;
static h4_context_t h4_ctx;

/* Transport vtable */
static int32_t uart_int_open(void);
static int32_t uart_int_close(void);
static int32_t uart_int_write(const uint8_t *buf, uint32_t nbytes);

static const struct bflb_hci_transport uart_int_transport = {
    .name  = "uart_int",
    .open  = uart_int_open,
    .close = uart_int_close,
    .write = uart_int_write,
};

/* --- Read thread: StreamBuffer -> h4_recv --- */

static struct k_thread uart_read_thread_handle;

static void uart_int_read_thread(void *arg)
{
    uint8_t tmp[64];
    const bflb_hci_trans_callbacks_t *cb = bflb_hci_transport_get_callbacks();

    h4_init(&h4_ctx);

    for (;;) {
        size_t n = xStreamBufferReceive(xStreamBufferRX, tmp, sizeof(tmp), portMAX_DELAY);
        if (n == 0)
            continue;

        h4_callbacks_t h4_cb = {
            .rx_handler = cb->rx_handler,
            .alloc_buf  = cb->alloc_buf,
        };

        size_t off = 0;
        while (off < n) {
            uint16_t consumed = h4_recv(&h4_ctx, tmp + off, n - off, &h4_cb);
            if (consumed == 0) {
                BT_ERR("H4 parser stuck, resetting");
                h4_init(&h4_ctx);
                break;
            }
            off += consumed;
        }
    }
}

/* --- ISR: UART RX FIFO -> StreamBuffer --- */

static void uart_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_uart_get_intstatus(uartx);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (intstatus & UART_INTSTS_RX_FIFO) {
        while (bflb_uart_rxavailable(uartx) && !xStreamBufferIsFull(xStreamBufferRX)) {
            uint8_t data = bflb_uart_getchar(uartx);
            xStreamBufferSendFromISR(xStreamBufferRX, &data, 1, &xHigherPriorityTaskWoken);
        }
    }

    if (intstatus & UART_INTSTS_RTO) {
        bflb_uart_int_clear(uartx, UART_INTCLR_RTO);
        while (bflb_uart_rxavailable(uartx) && !xStreamBufferIsFull(xStreamBufferRX)) {
            uint8_t data = bflb_uart_getchar(uartx);
            xStreamBufferSendFromISR(xStreamBufferRX, &data, 1, &xHigherPriorityTaskWoken);
        }
    }

    if (intstatus & UART_INTSTS_TX_END) {
        bflb_uart_int_clear(uartx, UART_INTCLR_TX_END);
    }

    if (intstatus & UART_INTSTS_RX_END) {
        bflb_uart_int_clear(uartx, UART_INTCLR_RX_END);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* --- Transport vtable implementation --- */

static int32_t uart_int_open(void)
{
    if (uart_initialized)
        return 0;
    uart_initialized = 1;

    xStreamBufferRX = xStreamBufferCreate(HCI_UART_STREAM_BUF_SIZE, 1);
    if (xStreamBufferRX == NULL) {
        BT_ERR("create stream buffer failed");
        return -1;
    }

    uartx = bflb_device_get_by_name(HCI_UART_NAME);

    struct bflb_uart_config_s cfg = {
        .baudrate = HCI_UART_BAUDRATE,
        .data_bits = UART_DATA_BITS_8,
        .stop_bits = UART_STOP_BITS_1,
        .parity = UART_PARITY_NONE,
        .flow_ctrl = 0,
        .tx_fifo_threshold = 7,
        .rx_fifo_threshold = 7,
        .bit_order = UART_LSB_FIRST,
    };
    bflb_uart_init(uartx, &cfg);
    bflb_uart_rxint_mask(uartx, false);
    bflb_irq_attach(uartx->irq_num, uart_isr, NULL);
    bflb_irq_enable(uartx->irq_num);

    if (k_thread_create(&uart_read_thread_handle, "hci_uart_int",
                        STKSIZE_HCI_PROCESS, uart_int_read_thread,
                        PRI_HCI_PROCESS)) {
        BT_ERR("Fail to create uart_int read thread");
        return -1;
    }

    return 0;
}

static int32_t uart_int_close(void)
{
    if (!uart_initialized) {
        BT_ERR("UART is not open");
        return -1;
    }

    vStreamBufferDelete(xStreamBufferRX);
    xStreamBufferRX = NULL;
    k_thread_delete(&uart_read_thread_handle);
    uart_initialized = 0;

    return 0;
}

static int32_t uart_int_write(const uint8_t *buf, uint32_t nbytes)
{
    if (!uart_initialized || !buf)
        return -1;

    if (bflb_uart_put_block(uartx, (uint8_t *)buf, nbytes))
        return -1;

    return nbytes;
}

/* --- Registration entry point --- */

int bflb_hci_uart_int_register(const bflb_hci_trans_callbacks_t *cb)
{
    bflb_hci_transport_register_cb(cb);
    return bflb_hci_transport_register(BFLB_HCI_TRANSPORT_UART_INT,
                                       &uart_int_transport);
}
