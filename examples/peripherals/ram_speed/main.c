#include "bflb_platform_dma.h"
#include "bflb_dma.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"
#include "board.h"

#define K_NUM             32
#define DMA_BUFFER_LENGTH (K_NUM * 1024)

__attribute((aligned(32))) uint8_t src1_buffer[DMA_BUFFER_LENGTH];
ATTR_NOINIT_PSRAM_SECTION __attribute((aligned(32))) uint8_t src2_buffer[DMA_BUFFER_LENGTH];

__attribute((aligned(32))) uint8_t dst1_buffer[DMA_BUFFER_LENGTH];
ATTR_NOINIT_PSRAM_SECTION __attribute((aligned(32))) uint8_t dst2_buffer[DMA_BUFFER_LENGTH];

ATTR_WIFI_RAM_SECTION struct bflb_platform_dma_desc dmslist;

struct bflb_dma_channel_lli_pool_s lli[20];

void memcpy_test(void)
{
    uint64_t start_time = 0;

    printf("memcpy speed test\r\n");

    start_time = bflb_mtimer_get_time_us();
    memcpy(dst1_buffer, src1_buffer, DMA_BUFFER_LENGTH);
    printf("ocram2ocram speed:%d MB/s\r\n", K_NUM * 1000000 / 1024 / ((uint32_t)(bflb_mtimer_get_time_us() - start_time)));

    start_time = bflb_mtimer_get_time_us();
    memcpy(dst2_buffer, src1_buffer, DMA_BUFFER_LENGTH);
    printf("ocram2psram speed:%d MB/s\r\n", K_NUM * 1000000 / 1024 / ((uint32_t)(bflb_mtimer_get_time_us() - start_time)));

    start_time = bflb_mtimer_get_time_us();
    memcpy(dst2_buffer, src2_buffer, DMA_BUFFER_LENGTH);
    printf("psram2psram speed:%d MB/s\r\n", K_NUM * 1000000 / 1024 / ((uint32_t)(bflb_mtimer_get_time_us() - start_time)));
}

void platform_dma_test()
{
    struct bflb_device_s *dma_chx;

    uint64_t start_time = 0;

    printf("platform dma speed test\r\n");

    dma_chx = bflb_device_get_by_name("plfm_dma_ch2");
    bflb_platform_dma_init(dma_chx);

    dmslist.src = (uint32_t)src1_buffer;
    dmslist.dest = (uint32_t)dst1_buffer;
    dmslist.length = DMA_BUFFER_LENGTH;
    dmslist.ctrl = 0;
    dmslist.next = 0;

    start_time = bflb_mtimer_get_time_us();
    bflb_platform_dma_push(dma_chx, &dmslist, &dmslist);
    bflb_platform_dma_wait_eot(dma_chx);
    bflb_platform_dma_clear_eot(dma_chx);
    printf("ocram2ocram speed:%d MB/s\r\n", K_NUM * 1000000 / 1024 / ((uint32_t)(bflb_mtimer_get_time_us() - start_time)));

    dmslist.src = (uint32_t)src1_buffer;
    dmslist.dest = (uint32_t)dst2_buffer;
    dmslist.length = DMA_BUFFER_LENGTH;
    dmslist.ctrl = 0;
    dmslist.next = 0;

    start_time = bflb_mtimer_get_time_us();
    bflb_platform_dma_push(dma_chx, &dmslist, &dmslist);
    bflb_platform_dma_wait_eot(dma_chx);
    bflb_platform_dma_clear_eot(dma_chx);
    printf("ocram2psram speed:%d MB/s\r\n", K_NUM * 1000000 / 1024 / ((uint32_t)(bflb_mtimer_get_time_us() - start_time)));

    dmslist.src = (uint32_t)src2_buffer;
    dmslist.dest = (uint32_t)dst2_buffer;
    dmslist.length = DMA_BUFFER_LENGTH;
    dmslist.ctrl = 0;
    dmslist.next = 0;

    start_time = bflb_mtimer_get_time_us();
    bflb_platform_dma_push(dma_chx, &dmslist, &dmslist);
    bflb_platform_dma_wait_eot(dma_chx);
    bflb_platform_dma_clear_eot(dma_chx);
    printf("psram2psram speed:%d MB/s\r\n", K_NUM * 1000000 / 1024 / ((uint32_t)(bflb_mtimer_get_time_us() - start_time)));
}

void dma_test()
{
    struct bflb_device_s *dma_chx;

    uint64_t start_time = 0;

    printf("dma speed test\r\n");

    dma_chx = bflb_device_get_by_name("dma0_ch0");

    struct bflb_dma_channel_config_s config;

    config.direction = DMA_MEMORY_TO_MEMORY;
    config.src_req = 0;
    config.dst_req = 0;
    config.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    config.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    config.src_burst_count = DMA_BURST_INCR4;
    config.dst_burst_count = DMA_BURST_INCR4;
    config.src_width = DMA_DATA_WIDTH_32BIT;
    config.dst_width = DMA_DATA_WIDTH_32BIT;
    bflb_dma_channel_init(dma_chx, &config);

    struct bflb_dma_channel_lli_transfer_s transfers;

    transfers.src_addr = (uint32_t)src1_buffer;
    transfers.dst_addr = (uint32_t)dst1_buffer;
    transfers.nbytes = DMA_BUFFER_LENGTH;

    start_time = bflb_mtimer_get_time_us();
    bflb_dma_channel_lli_reload(dma_chx, lli, 20, &transfers, 1);
    bflb_dma_channel_start(dma_chx);
    while (bflb_dma_channel_isbusy(dma_chx)) {}
    printf("ocram2ocram speed:%d MB/s\r\n", K_NUM * 1000000 / 1024 / ((uint32_t)(bflb_mtimer_get_time_us() - start_time)));

    transfers.src_addr = (uint32_t)src1_buffer;
    transfers.dst_addr = (uint32_t)dst2_buffer;
    transfers.nbytes = DMA_BUFFER_LENGTH;

    start_time = bflb_mtimer_get_time_us();
    bflb_dma_channel_lli_reload(dma_chx, lli, 20, &transfers, 1);
    bflb_dma_channel_start(dma_chx);
    while (bflb_dma_channel_isbusy(dma_chx)) {}
    printf("ocram2psram speed:%d MB/s\r\n", K_NUM * 1000000 / 1024 / ((uint32_t)(bflb_mtimer_get_time_us() - start_time)));

    transfers.src_addr = (uint32_t)src2_buffer;
    transfers.dst_addr = (uint32_t)dst2_buffer;
    transfers.nbytes = DMA_BUFFER_LENGTH;

    start_time = bflb_mtimer_get_time_us();
    bflb_dma_channel_lli_reload(dma_chx, lli, 20, &transfers, 1);
    bflb_dma_channel_start(dma_chx);
    while (bflb_dma_channel_isbusy(dma_chx)) {}
    printf("psram2psram speed:%d MB/s\r\n", K_NUM * 1000000 / 1024 / ((uint32_t)(bflb_mtimer_get_time_us() - start_time)));
}

int main(void)
{
    board_init();

    memset(src1_buffer, 0x55, DMA_BUFFER_LENGTH);
    memset(src2_buffer, 0x55, DMA_BUFFER_LENGTH);
    memset(dst1_buffer, 0xaa, DMA_BUFFER_LENGTH);
    memset(dst2_buffer, 0xaa, DMA_BUFFER_LENGTH);

    memcpy_test();
#if defined(BL616)
    platform_dma_test();
#endif
    dma_test();

    while (1) {
    }
}
