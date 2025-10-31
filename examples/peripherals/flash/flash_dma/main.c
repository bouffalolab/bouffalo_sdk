#include "bflb_flash.h"
#include "bflb_dma.h"
#include "board.h"


#define FLASH_RW_START_ADDR   0x10000
#define DMA_FLASH_ADDR_OFFSET 28  /* 0 or 28 for unaligned case */
#define DMA_BUFFER_LENGTH     260

static ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t dst_buffer[DMA_BUFFER_LENGTH];
static uint8_t write_buf[DMA_BUFFER_LENGTH];
static uint8_t dma_tc_flag0 = 0;
struct bflb_device_s *dma0_ch0;
static uint64_t start_time;

struct bflb_dma_channel_lli_pool_s lli[1]; /* max trasnfer size 4064 * 1 */

static uint8_t src_burst[] = {
    DMA_BURST_INCR1,
    DMA_BURST_INCR1,
    DMA_BURST_INCR1,
    DMA_BURST_INCR4,
    DMA_BURST_INCR4,
    DMA_BURST_INCR4,
    DMA_BURST_INCR8,
    DMA_BURST_INCR8,
    DMA_BURST_INCR8,
    DMA_BURST_INCR16,
    DMA_BURST_INCR16,
    DMA_BURST_INCR16,
};

static uint8_t dst_burst[] = {
    DMA_BURST_INCR1,
    DMA_BURST_INCR1,
    DMA_BURST_INCR1,
    DMA_BURST_INCR4,
    DMA_BURST_INCR4,
    DMA_BURST_INCR4,
    DMA_BURST_INCR8,
    DMA_BURST_INCR8,
    DMA_BURST_INCR8,
    DMA_BURST_INCR16,
    DMA_BURST_INCR16,
    DMA_BURST_INCR16,
};

static uint8_t src_width[] = {
    DMA_DATA_WIDTH_8BIT,
    DMA_DATA_WIDTH_16BIT,
    DMA_DATA_WIDTH_32BIT,
    DMA_DATA_WIDTH_8BIT,
    DMA_DATA_WIDTH_16BIT,
    DMA_DATA_WIDTH_32BIT,
    DMA_DATA_WIDTH_8BIT,
    DMA_DATA_WIDTH_16BIT,
    DMA_DATA_WIDTH_32BIT,
    DMA_DATA_WIDTH_8BIT,
    DMA_DATA_WIDTH_16BIT,
    DMA_DATA_WIDTH_32BIT,
};

static uint8_t dst_width[] = {
    DMA_DATA_WIDTH_8BIT,
    DMA_DATA_WIDTH_16BIT,
    DMA_DATA_WIDTH_32BIT,
    DMA_DATA_WIDTH_8BIT,
    DMA_DATA_WIDTH_16BIT,
    DMA_DATA_WIDTH_32BIT,
    DMA_DATA_WIDTH_8BIT,
    DMA_DATA_WIDTH_16BIT,
    DMA_DATA_WIDTH_32BIT,
    DMA_DATA_WIDTH_8BIT,
    DMA_DATA_WIDTH_16BIT,
    DMA_DATA_WIDTH_32BIT,
};

void dma0_ch0_isr(void *arg)
{
    printf("cost time:%d us\r\n", (uint32_t)(bflb_mtimer_get_time_us() - start_time));
    dma_tc_flag0++;
    printf("tc done\r\n");
}

int main(void)
{
    board_init();
    for (uint16_t i = 0; i < DMA_BUFFER_LENGTH; i++) {
        write_buf[i] = (i & 0xff) + i / 256;
    }

    /* erase flash */
    bflb_flash_erase(FLASH_RW_START_ADDR, 4096);

    /* write flash data */
    bflb_flash_write(FLASH_RW_START_ADDR, write_buf, sizeof(write_buf)); /* FLASH_XIP_BASE - 0x2000 + 0x00010000 */

    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");

    struct bflb_dma_channel_config_s config;

    for (uint8_t i = 0; i < sizeof(src_burst); i++) {
        dma_tc_flag0 = 0;
        memset(dst_buffer, 0, DMA_BUFFER_LENGTH);

        config.direction = DMA_MEMORY_TO_MEMORY;
        config.src_req = 0;
        config.dst_req = 0;
        config.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        config.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        config.src_burst_count = src_burst[i];
        config.dst_burst_count = dst_burst[i];
        config.src_width = src_width[i];
        config.dst_width = dst_width[i];
        bflb_dma_channel_init(dma0_ch0, &config);

        bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);

        struct bflb_dma_channel_lli_transfer_s transfers[1];

        transfers[0].src_addr = (uint32_t)(FLASH_XIP_BASE - bflb_flash_get_image_offset() + FLASH_RW_START_ADDR + DMA_FLASH_ADDR_OFFSET);
        transfers[0].dst_addr = (uint32_t)dst_buffer;
        transfers[0].nbytes = DMA_BUFFER_LENGTH - DMA_FLASH_ADDR_OFFSET;

        bflb_dma_channel_lli_reload(dma0_ch0, lli, 1, transfers, 1);

        start_time = bflb_mtimer_get_time_us();
        bflb_dma_channel_start(dma0_ch0);
        while (dma_tc_flag0 != 1) {
            bflb_mtimer_delay_ms(1);
        }

        for (uint16_t j = 0; j < (DMA_BUFFER_LENGTH - DMA_FLASH_ADDR_OFFSET); j++) {
            if (dst_buffer[j] != write_buf[DMA_FLASH_ADDR_OFFSET + j]) {
                printf("flash test fail at %d, expect:%d but with %d\r\n", i, write_buf[DMA_FLASH_ADDR_OFFSET + j], dst_buffer[j]);
                while (1) {
                }
            }
        }
    }

    printf("flash dam case success\r\n");
    while (1) {
    }
}