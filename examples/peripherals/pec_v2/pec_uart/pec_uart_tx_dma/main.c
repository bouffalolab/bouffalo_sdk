#include "board.h"
#include "bflb_dma.h"
#include "bflb_pec_v2_instance.h"

#define UART_BAUDRATE (115200)
#define UART_DATABITS (8)
#define UART_STOPBITS (PEC_UART_STOPBITS_2P0)
#define UART_PARITY   (PEC_UART_PARITY_EVEN)

struct bflb_device_s *pec_uart;
ATTR_NOCACHE_NOINIT_RAM_SECTION __attribute__((aligned(32))) uint8_t uart_buff[512];

struct bflb_device_s *dma0_ch0;
struct bflb_dma_channel_lli_transfer_s transfers[1];
struct bflb_dma_channel_lli_pool_s lli[1]; /* max trasnfer size 4064 * 1 */

struct bflb_pec_uart_tx_s uart_cfg = {
    .mem = 0,                     /*!< memory address of first instruction */
    .baudrate = UART_BAUDRATE,    /*!< uart baudrate */
    .databits = UART_DATABITS,    /*!< uart data bits */
    .stopbits = UART_STOPBITS,    /*!< uart stop bits */
    .parity = UART_PARITY,        /*!< uart parity */
    .pin = PEC_UART_TX_PIN % 32,  /*!< pin index of uart */
    .fifo_threshold = 16 - 1,     /*!< uart fifo threshold */
    .dma_enable = 1,              /*!< enable or disable dma with uart */
    .pin_inverse = 0,             /*!< enable or disable uart_tx output level inverse */
};

void dma0_ch0_isr(void *arg)
{
    printf("dma0_ch0_isr\r\n");
}

int main(void)
{
    board_init();
    board_pec_uart_gpio_init();
    pec_uart = bflb_device_get_by_name("pec_sm1");
    printf("pec_uart = 0x%08lX\r\n", pec_uart);

    bflb_pec_uart_tx_init(pec_uart, &uart_cfg);
    for (uint32_t i = 0; i < sizeof(uart_buff); i++) {
        uart_buff[i] = i % 256;
    }

    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");
    printf("dma0_ch0 = 0x%08lX\r\n", dma0_ch0);
    struct bflb_dma_channel_config_s dma_config;
    dma_config.direction = DMA_MEMORY_TO_PERIPH;
    dma_config.src_req = DMA_REQUEST_NONE;
    dma_config.dst_req = DMA_REQUEST_PEC_SM1_TX;
    dma_config.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    dma_config.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    dma_config.src_burst_count = DMA_BURST_INCR16;
    dma_config.dst_burst_count = DMA_BURST_INCR16;
    dma_config.src_width = DMA_DATA_WIDTH_8BIT;
    dma_config.dst_width = DMA_DATA_WIDTH_8BIT;
    bflb_dma_channel_init(dma0_ch0, &dma_config);
    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);
    transfers[0].src_addr = (uint32_t)uart_buff;
    transfers[0].dst_addr = DMA_ADDR_PEC_SM1_TDR;
    transfers[0].nbytes = sizeof(uart_buff);
    bflb_dma_channel_lli_reload(dma0_ch0, lli, 1, transfers, 1);
    bflb_dma_channel_start(dma0_ch0);

    return 0;
}
