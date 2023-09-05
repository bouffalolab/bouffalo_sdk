#include "bflb_mtimer.h"
#include "bflb_spi.h"
#include "bflb_dma.h"
#include "board.h"

#define SPI_MASTER_CASE 0
#define SPI_SLAVE_CASE  1
#define SPI_CASE_SELECT SPI_MASTER_CASE

struct bflb_device_s *spi0;
struct bflb_device_s *dma0_ch0;
struct bflb_device_s *dma0_ch1;

static ATTR_NOCACHE_NOINIT_RAM_SECTION uint32_t tx_buffer[256];
static ATTR_NOCACHE_NOINIT_RAM_SECTION uint32_t rx_buffer[256];

static volatile uint8_t dma_tc_flag0 = 0;
static volatile uint8_t dma_tc_flag1 = 0;
struct bflb_dma_channel_lli_pool_s tx_llipool[1];
struct bflb_dma_channel_lli_pool_s rx_llipool[1];

void dma0_ch0_isr(void *arg)
{
    dma_tc_flag0++;
    printf("tc done\r\n");
}

void dma0_ch1_isr(void *arg)
{
    dma_tc_flag1++;
    printf("rx done\r\n");
}

void sram_init()
{
    uint32_t i;

    for (i = 0; i < 256; i++) {
        tx_buffer[i] = i * 0x1010101;
        rx_buffer[i] = 0;
    }
}

int main(void)
{
    struct bflb_dma_channel_lli_transfer_s tx_transfers[1];
    struct bflb_dma_channel_lli_transfer_s rx_transfers[1];

    struct bflb_spi_config_s spi_cfg = {
#if (SPI_CASE_SELECT == SPI_MASTER_CASE)
        .freq = 1 * 1000 * 1000,
        .role = SPI_ROLE_MASTER,
#else
        .freq = 32 * 1000 * 1000,
        .role = SPI_ROLE_SLAVE,
#endif
        .mode = SPI_MODE3,
        .data_width = SPI_DATA_WIDTH_32BIT,
        .bit_order = SPI_BIT_MSB,
        .byte_order = SPI_BYTE_LSB,
        .tx_fifo_threshold = 0,
        .rx_fifo_threshold = 0,
    };

    struct bflb_dma_channel_config_s tx_config = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_SPI0_TX,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_32BIT,
        .dst_width = DMA_DATA_WIDTH_32BIT,
    };

    struct bflb_dma_channel_config_s rx_config = {
        .direction = DMA_PERIPH_TO_MEMORY,
        .src_req = DMA_REQUEST_SPI0_RX,
        .dst_req = DMA_REQUEST_NONE,
        .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_32BIT,
        .dst_width = DMA_DATA_WIDTH_32BIT,
    };

    board_init();
    board_spi0_gpio_init();
    sram_init();

    spi0 = bflb_device_get_by_name("spi0");
    bflb_spi_init(spi0, &spi_cfg);
    bflb_spi_link_txdma(spi0, true);
    bflb_spi_link_rxdma(spi0, true);

    printf("\n\rspi dma test\n\r");

    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");
    dma0_ch1 = bflb_device_get_by_name("dma0_ch1");

    bflb_dma_channel_init(dma0_ch0, &tx_config);
    bflb_dma_channel_init(dma0_ch1, &rx_config);

    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);
    bflb_dma_channel_irq_attach(dma0_ch1, dma0_ch1_isr, NULL);

    tx_transfers[0].src_addr = (uint32_t)tx_buffer;
    tx_transfers[0].dst_addr = (uint32_t)DMA_ADDR_SPI0_TDR;
    tx_transfers[0].nbytes = 256 * 4;

    rx_transfers[0].src_addr = (uint32_t)DMA_ADDR_SPI0_RDR;
    rx_transfers[0].dst_addr = (uint32_t)rx_buffer;
    rx_transfers[0].nbytes = 256 * 4;

    bflb_dma_channel_lli_reload(dma0_ch0, tx_llipool, 1, tx_transfers, 1);
    bflb_dma_channel_lli_reload(dma0_ch1, rx_llipool, 1, rx_transfers, 1);
    bflb_dma_channel_start(dma0_ch0);
    bflb_dma_channel_start(dma0_ch1);

    while (dma_tc_flag0 != 1) {
        bflb_mtimer_delay_ms(1);
    }
    while (dma_tc_flag1 != 1) {
        bflb_mtimer_delay_ms(1);
    }

    for (uint32_t i = 0; i < 256; i++) {
        if (rx_buffer[i] != tx_buffer[i]) {
            printf("error! index %ld: send 0x%08lX but read 0x%08lX\n\r", i, tx_buffer[i], rx_buffer[i]);
        }
    }

    printf("\n\rtest end\n\r");

    while (1) {
    }
}
