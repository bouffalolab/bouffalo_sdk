#include "bflb_hci_uart_common.h"
#include "bflb_hci_transport.h"
#include "hci_h4.h"
#include "bflb_gpio.h"
#include "bflb_uart.h"
#include "bflb_dma.h"
#include "bflb_clock.h"
#include "bt_log.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

#define UART_FLOW_CTRL_ENABLE       HCI_UART_FLOW_CTRL_ENABLE

static uint8_t ATTR_NOCACHE_NOINIT_RAM_SECTION uart_rx_dma_buf[HCI_UART_DMA_RX_BUF_SIZE];

static struct bflb_rx_cycle_dma g_uart_rx_dma;
static struct bflb_dma_channel_lli_pool_s rx_llipool[20];
static struct bflb_dma_channel_lli_pool_s tx_llipool[20];

static struct bflb_device_s *uart = NULL;
static struct bflb_device_s *dma_rx = NULL;
static struct bflb_device_s *dma_tx = NULL;

static uint8_t uart_initialized = 0;
static StreamBufferHandle_t xStreamBufferRX = NULL;
static h4_context_t h4_ctx;

/* Transport vtable */
static int32_t uart_dma_open(void);
static int32_t uart_dma_close(void);
static int32_t uart_dma_write(const uint8_t *buf, uint32_t nbytes);

static const struct bflb_hci_transport uart_dma_transport = {
    .name  = "uart_dma",
    .open  = uart_dma_open,
    .close = uart_dma_close,
    .write = uart_dma_write,
};

/* --- Read thread: StreamBuffer -> h4_recv --- */

static struct k_thread uart_read_thread_handle;

static void uart_dma_read_thread(void *arg)
{
    uint8_t tmp[64];
    const bflb_hci_trans_callbacks_t *cb = bflb_hci_transport_get_callbacks();

    h4_init(&h4_ctx);

    for (;;) {
        size_t n = xStreamBufferReceive(xStreamBufferRX, tmp, sizeof(tmp), portMAX_DELAY);
        if (n == 0)
            continue;

        /* Convert hci callbacks to h4 callbacks */
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

/* --- DMA RX callback: copy data to StreamBuffer --- */

static void dma_rx_copy(uint8_t *data, uint32_t len)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (xStreamBufferRX != NULL) {
        size_t space = xStreamBufferSpacesAvailable(xStreamBufferRX);
        if (space < len) {
            BT_ERR("Stream buffer overflow: avail %d, need %d", (int)space, (int)len);
        } else {
            int ret = xStreamBufferSendFromISR(xStreamBufferRX, data, len, &xHigherPriorityTaskWoken);
            if (ret != (int)len) {
                BT_ERR("Stream buffer send failed: ret %d, need %d", ret, (int)len);
                configASSERT(ret == len);
            }
        }
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* --- ISR handlers --- */

static void uart_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_uart_get_intstatus(uart);

    if (intstatus & UART_INTSTS_RTO) {
#if UART_FLOW_CTRL_ENABLE
        bflb_uart_feature_control(uart, UART_CMD_SET_RTS_VALUE, 0);
#endif
        bflb_uart_int_clear(uart, UART_INTCLR_RTO);
        bflb_rx_cycle_dma_process(&g_uart_rx_dma, 0);
    }
}

static void dma_rx_isr(void *arg)
{
#if UART_FLOW_CTRL_ENABLE
    bflb_uart_feature_control(uart, UART_CMD_SET_RTS_VALUE, 1);
#endif
    bflb_rx_cycle_dma_process(&g_uart_rx_dma, 1);
}

static void dma_tx_isr(void *arg)
{
}

/* --- Hardware init helpers --- */

static void gpio_init(void)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_uart_init(gpio, HCI_UART_TXD_PIN, GPIO_UART_FUNC_UART0_TX + 4 * uart->idx);
    bflb_gpio_uart_init(gpio, HCI_UART_RXD_PIN, GPIO_UART_FUNC_UART0_RX + 4 * uart->idx);

#if UART_FLOW_CTRL_ENABLE
    bflb_gpio_uart_init(gpio, HCI_UART_CTS_PIN, GPIO_UART_FUNC_UART0_CTS + 4 * uart->idx);
    bflb_gpio_uart_init(gpio, HCI_UART_RTS_PIN, GPIO_UART_FUNC_UART0_RTS + 4 * uart->idx);
#endif
}

static void uart_hw_init(void)
{
    struct bflb_uart_config_s cfg = {
        .baudrate = HCI_UART_BAUDRATE,
        .data_bits = UART_DATA_BITS_8,
        .stop_bits = UART_STOP_BITS_1,
        .parity = UART_PARITY_NONE,
        .bit_order = UART_LSB_FIRST,
#if UART_FLOW_CTRL_ENABLE
        .flow_ctrl = 1,
#else
        .flow_ctrl = 0,
#endif
        .tx_fifo_threshold = 0,
        .rx_fifo_threshold = 0,
    };

    bflb_uart_init(uart, &cfg);
    bflb_uart_feature_control(uart, UART_CMD_SET_RTO_VALUE, 0x80);
    bflb_uart_link_txdma(uart, true);
    bflb_uart_link_rxdma(uart, true);
    bflb_irq_attach(uart->irq_num, uart_isr, NULL);
    bflb_irq_enable(uart->irq_num);
}

static void dma_rx_init(void)
{
    struct bflb_dma_channel_config_s rx_cfg = {
        .direction = DMA_PERIPH_TO_MEMORY,
        .src_req = DMA_REQUEST_UART0_RX + 2 * uart->idx,
        .dst_req = DMA_REQUEST_NONE,
        .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_8BIT,
        .dst_width = DMA_DATA_WIDTH_8BIT,
    };

    bflb_dma_channel_init(dma_rx, &rx_cfg);
    bflb_dma_channel_irq_attach(dma_rx, dma_rx_isr, NULL);

    bflb_rx_cycle_dma_init(&g_uart_rx_dma,
                           dma_rx,
                           rx_llipool,
                           sizeof(rx_llipool) / sizeof(rx_llipool[0]),
                           uart->reg_base + 0x8C,
                           uart_rx_dma_buf,
                           HCI_UART_DMA_RX_BUF_SIZE,
                           dma_rx_copy);
}

static void dma_tx_init(void)
{
    struct bflb_dma_channel_config_s tx_cfg = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_UART0_TX + 2 * uart->idx,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_8BIT,
        .dst_width = DMA_DATA_WIDTH_8BIT,
    };

    bflb_dma_channel_init(dma_tx, &tx_cfg);
    bflb_dma_channel_irq_attach(dma_tx, dma_tx_isr, NULL);
}

/* --- Transport vtable implementation --- */

static int32_t uart_dma_open(void)
{
    if (uart_initialized)
        return 0;
    uart_initialized = 1;

    uart = bflb_device_get_by_name(HCI_UART_NAME);
    dma_rx = bflb_device_get_by_name(HCI_UART_DMA_RX_CH);
    dma_tx = bflb_device_get_by_name(HCI_UART_DMA_TX_CH);

    xStreamBufferRX = xStreamBufferCreate(HCI_UART_STREAM_BUF_SIZE, 1);
    if (xStreamBufferRX == NULL) {
        BT_ERR("create stream buffer failed");
        return -1;
    }

    gpio_init();
    uart_hw_init();

    PERIPHERAL_CLOCK_DMA0_ENABLE();

    dma_rx_init();
    dma_tx_init();

    if (k_thread_create(&uart_read_thread_handle, "hci_uart_dma",
                        STKSIZE_HCI_PROCESS, uart_dma_read_thread,
                        PRI_HCI_PROCESS)) {
        BT_ERR("Fail to create uart_dma read thread");
        return -1;
    }

    bflb_dma_channel_start(dma_rx);
    return 0;
}

static int32_t uart_dma_close(void)
{
    if (!uart_initialized) {
        BT_ERR("UART is not open");
        return -1;
    }

    k_thread_delete(&uart_read_thread_handle);
    vStreamBufferDelete(xStreamBufferRX);
    xStreamBufferRX = NULL;
    uart_initialized = 0;

    return 0;
}

static int32_t uart_dma_write(const uint8_t *buf, uint32_t nbytes)
{
    struct bflb_dma_channel_lli_transfer_s transfer = {
        .src_addr = (uint32_t)buf,
        .dst_addr = uart->reg_base + 0x88,
        .nbytes = nbytes,
    };

    taskENTER_CRITICAL();

    while (bflb_dma_channel_isbusy(dma_tx))
        ;

#if defined(BL616) || defined(BL618DG)
    bflb_l1c_dcache_clean_all();
#endif

    bflb_dma_channel_lli_reload(dma_tx, tx_llipool,
                                sizeof(tx_llipool) / sizeof(tx_llipool[0]),
                                &transfer, 1);
    bflb_dma_channel_start(dma_tx);

    taskEXIT_CRITICAL();

    return nbytes;
}

/* --- Registration entry point --- */

int bflb_hci_uart_dma_register(const bflb_hci_trans_callbacks_t *cb)
{
    bflb_hci_transport_register_cb(cb);
    return bflb_hci_transport_register(BFLB_HCI_TRANSPORT_UART_DMA,
                                       &uart_dma_transport);
}
