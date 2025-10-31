#include "board.h"
#include "bflb_dma.h"
#include "bflb_pec_v2_instance.h"

#define UART_BAUDRATE (115200)
#define UART_DATABITS (8)
#define UART_STOPBITS (PEC_UART_STOPBITS_1P0)
#define UART_PARITY   (PEC_UART_PARITY_NONE)

#define UART_BUFFER_SIZE (1000)

struct bflb_device_s *pec_uart_tx;
struct bflb_device_s *pec_uart_rx;
ATTR_NOCACHE_NOINIT_RAM_SECTION __attribute__((aligned(32))) uint8_t uart_tx_buff[UART_BUFFER_SIZE];
ATTR_NOCACHE_NOINIT_RAM_SECTION __attribute__((aligned(32))) uint8_t uart_rx_buff[UART_BUFFER_SIZE];

struct bflb_device_s *dma0_ch0;
struct bflb_device_s *dma0_ch1;
struct bflb_dma_channel_lli_transfer_s transfers_tx[1];
struct bflb_dma_channel_lli_transfer_s transfers_rx[1];
struct bflb_dma_channel_lli_pool_s lli_tx[1]; /* max trasnfer size 4064 * 1 */
struct bflb_dma_channel_lli_pool_s lli_rx[1]; /* max trasnfer size 4064 * 1 */

struct bflb_pec_uart_tx_s uart_tx_cfg = {
    .mem = 0,                     /*!< memory address of first instruction */
    .baudrate = UART_BAUDRATE,    /*!< uart baudrate */
    .databits = UART_DATABITS,    /*!< uart data bits */
    .stopbits = UART_STOPBITS,    /*!< uart stop bits */
    .parity = UART_PARITY,        /*!< uart parity */
    .pin = PEC_UART_TX_PIN % 32,  /*!< pin index of uart */
    .fifo_threshold = 8 - 1,      /*!< uart fifo threshold */
    .dma_enable = 1,              /*!< enable or disable dma with uart */
    .pin_inverse = 0,             /*!< enable or disable uart_tx output level inverse */
};

struct bflb_pec_uart_rx_s uart_rx_cfg = {
#if defined(BL616L)
    .mem = 0,                        /*!< memory address of first instruction */
#else
    .mem = 128,                      /*!< memory address of first instruction */
#endif
    .baudrate = UART_BAUDRATE,       /*!< uart baudrate */
    .databits = UART_DATABITS,       /*!< uart data bits */
    .stopbits = UART_STOPBITS,       /*!< uart stop bits, note: hardware don't check stopbits length */
    .parity = UART_PARITY,           /*!< uart parity */
    .pin = PEC_UART_RX_PIN % 32,     /*!< pin index of uart */
    .fifo_threshold = 8 - 1,         /*!< uart fifo threshold */
    .dma_enable = 1,                 /*!< enable or disable dma with uart */
    .pin_inverse = 0,                /*!< enable or disable uart_rx input level inverse */
    .rto_bits = 35,                  /*!< bits of RTO(receive timeout) */
};

void dma0_ch0_isr(void *arg)
{
    printf("dma0_ch0_isr\r\n");
}

void dma0_ch1_isr(void *arg)
{
    printf("dma0_ch1_isr\r\n");
    for (uint32_t i = 0; i < UART_BUFFER_SIZE; i++) {
        if (uart_rx_buff[i] != uart_tx_buff[i]) {
            printf("check error, [%d] expert 0x%02X, but actual is 0x%02X\r\n", i, uart_tx_buff[i], uart_rx_buff[i]);
            return;
        }
    }
    printf("check pass\r\n");
}

int main(void)
{
    board_init();
    board_pec_uart_gpio_init();
    pec_uart_tx = bflb_device_get_by_name("pec_sm0");
    printf("pec_uart_tx = 0x%08lX\r\n", pec_uart_tx);
    pec_uart_rx = bflb_device_get_by_name("pec_sm1");
    printf("pec_uart_rx = 0x%08lX\r\n", pec_uart_rx);

    bflb_pec_uart_tx_init(pec_uart_tx, &uart_tx_cfg);
    bflb_pec_uart_rx_init(pec_uart_rx, &uart_rx_cfg);
    bflb_pec_uart_rx_start(pec_uart_rx);
    for (uint32_t i = 0; i < UART_BUFFER_SIZE; i++) {
        uart_tx_buff[i] = i % 256;
        uart_rx_buff[i] = 0;
    }

    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");
    printf("dma0_ch0 = 0x%08lX\r\n", dma0_ch0);
    struct bflb_dma_channel_config_s dma_config_tx;
    dma_config_tx.direction = DMA_MEMORY_TO_PERIPH;
    dma_config_tx.src_req = DMA_REQUEST_NONE;
    dma_config_tx.dst_req = DMA_REQUEST_PEC_SM0_TX;
    dma_config_tx.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    dma_config_tx.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    dma_config_tx.src_burst_count = DMA_BURST_INCR8;
    dma_config_tx.dst_burst_count = DMA_BURST_INCR8;
    dma_config_tx.src_width = DMA_DATA_WIDTH_8BIT;
    dma_config_tx.dst_width = DMA_DATA_WIDTH_8BIT;
    bflb_dma_channel_init(dma0_ch0, &dma_config_tx);
    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);
    transfers_tx[0].src_addr = (uint32_t)uart_tx_buff;
    transfers_tx[0].dst_addr = DMA_ADDR_PEC_SM0_TDR;
    transfers_tx[0].nbytes = sizeof(uart_tx_buff);
    bflb_dma_channel_lli_reload(dma0_ch0, lli_tx, 1, transfers_tx, 1);

    dma0_ch1 = bflb_device_get_by_name("dma0_ch1");
    printf("dma0_ch1 = 0x%08lX\r\n", dma0_ch1);
    struct bflb_dma_channel_config_s dma_config_rx;
    dma_config_rx.direction = DMA_PERIPH_TO_MEMORY;
    dma_config_rx.src_req = DMA_REQUEST_PEC_SM1_RX;
    dma_config_rx.dst_req = DMA_REQUEST_NONE;
    dma_config_rx.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    dma_config_rx.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    dma_config_rx.src_burst_count = DMA_BURST_INCR8;
    dma_config_rx.dst_burst_count = DMA_BURST_INCR8;
    dma_config_rx.src_width = DMA_DATA_WIDTH_8BIT;
    dma_config_rx.dst_width = DMA_DATA_WIDTH_8BIT;
    bflb_dma_channel_init(dma0_ch1, &dma_config_rx);
    bflb_dma_channel_irq_attach(dma0_ch1, dma0_ch1_isr, NULL);
    transfers_rx[0].src_addr = DMA_ADDR_PEC_SM1_RDR;
    transfers_rx[0].dst_addr = (uint32_t)uart_rx_buff;
    transfers_rx[0].nbytes = sizeof(uart_rx_buff);
    bflb_dma_channel_lli_reload(dma0_ch1, lli_rx, 1, transfers_rx, 1);

    bflb_dma_channel_start(dma0_ch1);
    bflb_dma_channel_start(dma0_ch0);

    return 0;
}
