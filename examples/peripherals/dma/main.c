#include "bflb_mtimer.h"
#include "bflb_dma.h"

extern void board_init(void);

static ATTR_NOCACHE_RAM_SECTION uint8_t src_buffer[4100];
static ATTR_NOCACHE_RAM_SECTION uint8_t src2_buffer[4100];
static ATTR_NOCACHE_RAM_SECTION uint8_t src3_buffer[4100];

static ATTR_NOCACHE_RAM_SECTION uint8_t dst_buffer[4100];
static ATTR_NOCACHE_RAM_SECTION uint8_t dst2_buffer[4100];
static ATTR_NOCACHE_RAM_SECTION uint8_t dst3_buffer[4100];
static uint8_t dma_tc_flag0 = 0;

struct bflb_device_s *dma0_ch0;

void dma0_ch0_isr(void *arg)
{
    dma_tc_flag0++;
    printf("tc done\r\n");
}

void sram_init()
{
    memset(src_buffer, 'a', 4100);
    src_buffer[3999] = 'B';
    src_buffer[4095] = 'A';
    src_buffer[4096] = 'B';
    src_buffer[4097] = 'C';
    src_buffer[4098] = 'D';
    src_buffer[4099] = 'E';

    memset(src2_buffer, 'c', 512);
    memset(src3_buffer, 'd', 256);
}

int main(void)
{
    board_init();

    sram_init();

    printf("dma memory test\r\n");
    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");

    struct bflb_dma_channel_config_s config;

    config.direction = DMA_MEMORY_TO_MEMORY;
    config.src_req = 0;
    config.dst_req = 0;
    config.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    config.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    config.src_burst_count = DMA_BURST_INCR1;
    config.dst_burst_count = DMA_BURST_INCR1;
    config.src_width = DMA_DATA_WIDTH_32BIT;
    config.dst_width = DMA_DATA_WIDTH_32BIT;
    bflb_dma_channel_init(dma0_ch0, &config);

    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);

    struct bflb_dma_channel_lli_pool_s lli[20]; /* max trasnfer size 4064 * 20 */
    struct bflb_dma_channel_lli_transfer_s transfers[3];

    transfers[0].src_addr = (uint32_t)src_buffer;
    transfers[0].dst_addr = (uint32_t)dst_buffer;
    transfers[0].nbytes = 4100;

    transfers[1].src_addr = (uint32_t)src2_buffer;
    transfers[1].dst_addr = (uint32_t)dst2_buffer;
    transfers[1].nbytes = 4100;

    transfers[2].src_addr = (uint32_t)src3_buffer;
    transfers[2].dst_addr = (uint32_t)dst3_buffer;
    transfers[2].nbytes = 4100;

    bflb_dma_channel_lli_reload(dma0_ch0, lli, 20, transfers, 3);
    bflb_dma_channel_start(dma0_ch0);
    while (dma_tc_flag0 != 3) {
        bflb_mtimer_delay_ms(1);
    }

    while (1) {
    }
}
