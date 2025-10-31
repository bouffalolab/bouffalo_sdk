#include "bflb_gpio.h"
#include "bflb_uart.h"
#include "bflb_dma.h"
#include "bflb_clock.h"
#include "ring_buffer.h"
#if defined(BL616)
#include "bl616_l1c.h"
#endif

#if defined(BL616)
#define UART_NAME                  "uart1"
#define UART_BAUDRATE              2000000
#define UART_TXD_PIN               27
#define UART_RXD_PIN               28
#define UART_CTS_PIN               29
#define UART_RTS_PIN               30
#endif

#if defined(BL616D)
#define UART_NAME                  "uart1"
#define UART_BAUDRATE              2000000
#define UART_TXD_PIN               27
#define UART_RXD_PIN               28
#define UART_CTS_PIN               25
#define UART_RTS_PIN               26
#endif

#if defined(BL702L)
#define UART_NAME                  "uart0"
#define UART_BAUDRATE              115200
#define UART_TXD_PIN               14 //23
#define UART_RXD_PIN               15 //24
#define UART_CTS_PIN               25
#define UART_RTS_PIN               26
#endif

#define DMA_RX_NAME                "dma0_ch2"
#define DMA_TX_NAME                "dma0_ch3"

#if defined(BL702L)
#define UART_FLOW_CTRL_ENABLE      0
#else
#define UART_FLOW_CTRL_ENABLE      1
#endif

#define UART_RX_DMA_BUF_SIZE       2048


static uint8_t ATTR_NOCACHE_NOINIT_RAM_SECTION uart_rx_dma_buf[UART_RX_DMA_BUF_SIZE];

static Ring_Buffer_Type uartRB;
static uint8_t uartBuf[UART_RX_DMA_BUF_SIZE];

static struct bflb_rx_cycle_dma g_uart_rx_dma;
static struct bflb_dma_channel_lli_pool_s rx_llipool[20];
static struct bflb_dma_channel_lli_pool_s tx_llipool[20];

static struct bflb_device_s *uart = NULL;
static struct bflb_device_s *dma_rx = NULL;
static struct bflb_device_s *dma_tx = NULL;


__attribute__((__weak__)) void btble_dma_uart_rx_event(void)
{

}

__attribute__((__weak__)) void btble_dma_uart_tx_event(void)
{

}


static void uart_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_uart_get_intstatus(uart);

    if(intstatus & UART_INTSTS_RTO){
#if UART_FLOW_CTRL_ENABLE
        bflb_uart_feature_control(uart, UART_CMD_SET_RTS_VALUE, 0);
#endif

        bflb_uart_int_clear(uart, UART_INTCLR_RTO);
        bflb_rx_cycle_dma_process(&g_uart_rx_dma, 0);
        btble_dma_uart_rx_event();
    }
}

static void dma_rx_isr(void *arg)
{
#if UART_FLOW_CTRL_ENABLE
    bflb_uart_feature_control(uart, UART_CMD_SET_RTS_VALUE, 1);
#endif

    bflb_rx_cycle_dma_process(&g_uart_rx_dma, 1);
    btble_dma_uart_rx_event();
}

static void dma_tx_isr(void *arg)
{
    btble_dma_uart_tx_event();
}


static void dma_rx_copy(uint8_t *data, uint32_t len)
{
    Ring_Buffer_Write(&uartRB, data, len);
}


static void gpio_init(void)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_uart_init(gpio, UART_TXD_PIN, GPIO_UART_FUNC_UART0_TX + 4 * uart->idx);
    bflb_gpio_uart_init(gpio, UART_RXD_PIN, GPIO_UART_FUNC_UART0_RX + 4 * uart->idx);

#if UART_FLOW_CTRL_ENABLE
    bflb_gpio_uart_init(gpio, UART_CTS_PIN, GPIO_UART_FUNC_UART0_CTS + 4 * uart->idx);
    bflb_gpio_uart_init(gpio, UART_RTS_PIN, GPIO_UART_FUNC_UART0_RTS + 4 * uart->idx);
#endif
}

static void uart_init(void)
{
    struct bflb_uart_config_s cfg = {
        .baudrate = UART_BAUDRATE,
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
                           sizeof(rx_llipool)/sizeof(rx_llipool[0]),
                           uart->reg_base + 0x8C,
                           uart_rx_dma_buf,
                           UART_RX_DMA_BUF_SIZE,
                           dma_rx_copy);

    bflb_dma_channel_start(dma_rx);
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

static void dma_init(void)
{
    PERIPHERAL_CLOCK_DMA0_ENABLE();

    dma_rx_init();
    dma_tx_init();
}


void btble_dma_uart_init(void)
{
    uart = bflb_device_get_by_name(UART_NAME);
    dma_rx = bflb_device_get_by_name(DMA_RX_NAME);
    dma_tx = bflb_device_get_by_name(DMA_TX_NAME);

    Ring_Buffer_Init(&uartRB, uartBuf, sizeof(uartBuf), NULL, NULL);

    gpio_init();
    uart_init();
    dma_init();
}

uint32_t btble_dma_uart_get_rx_count(void)
{
    return Ring_Buffer_Get_Length(&uartRB);
}

uint32_t btble_dma_uart_read(uint8_t *data, uint32_t len)
{
    uint32_t cnt;

    cnt = Ring_Buffer_Get_Length(&uartRB);
    if(cnt < len){
        len = cnt;
    }

    Ring_Buffer_Read(&uartRB, data, len);
    return len;
}

void btble_dma_uart_write(uint8_t *data, uint32_t len)
{
    struct bflb_dma_channel_lli_transfer_s transfer = {
        .src_addr = (uint32_t)data,
        .dst_addr = uart->reg_base + 0x88,
        .nbytes = len,
    };

    while(bflb_dma_channel_isbusy(dma_tx));

#if defined(BL616) || defined(BL616D)
    bflb_l1c_dcache_clean_all();
#endif

    bflb_dma_channel_lli_reload(dma_tx, tx_llipool, sizeof(tx_llipool)/sizeof(tx_llipool[0]), &transfer, 1);

    bflb_dma_channel_start(dma_tx);
}
