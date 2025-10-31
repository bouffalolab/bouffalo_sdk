#include "bflb_mtimer.h"
#include "bflb_dma.h"
#include "board.h"

#define DMA_BUFFER_MAX_LENGTH 128
#define DMA_TRANSFER_LENGTH   64

static ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t src_buffer[DMA_BUFFER_MAX_LENGTH];

static ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t dst_buffer[DMA_BUFFER_MAX_LENGTH];
static uint8_t dma_tc_flag0 = 0;

struct bflb_device_s *dma0_ch0;

struct bflb_dma_channel_lli_pool_s lli[20]; /* max trasnfer size 4064 * 20 */

void dma0_ch0_isr(void *arg)
{
    dma_tc_flag0++;
    printf("tc done\r\n");
}

void sram_init()
{
    uint32_t i;

    for (i = 0; i < DMA_BUFFER_MAX_LENGTH; i++) {
        src_buffer[i] = i & 0xff;
    }

    memset(dst_buffer, 0, DMA_BUFFER_MAX_LENGTH);
}

int main(void)
{
    uint32_t i;

    struct bflb_dma_channel_lli_transfer_s transfers[3];

    board_init();

    sram_init();

    printf("dma memory case:\r\n");
    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");

    struct bflb_dma_channel_config_s config;

    config.direction = DMA_MEMORY_TO_MEMORY;
    config.src_req = 0;
    config.dst_req = 0;
    config.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    config.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    config.src_burst_count = DMA_BURST_INCR1;
    config.dst_burst_count = DMA_BURST_INCR4;
    config.src_width = DMA_DATA_WIDTH_32BIT;
    config.dst_width = DMA_DATA_WIDTH_8BIT;
    bflb_dma_channel_init(dma0_ch0, &config);

    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);

    /* Disable reduce mode, transfer length = nbytes = 64 byte */
    transfers[0].src_addr = (uint32_t)src_buffer;
    transfers[0].dst_addr = (uint32_t)dst_buffer;
    transfers[0].nbytes = DMA_TRANSFER_LENGTH;
    bflb_dma_channel_lli_reload(dma0_ch0, lli, 20, transfers, 1);

    bflb_dma_channel_start(dma0_ch0);

    while (dma_tc_flag0 == 0) {
        bflb_mtimer_delay_ms(1);
    }

    /* Check 0~63 data */
    for (i = 0; i < DMA_TRANSFER_LENGTH; i++) {
        if (src_buffer[i] != dst_buffer[i]) {
            printf("Error! index: %ld, src: 0x%02x, dst: 0x%02x\r\n", i, src_buffer[i], dst_buffer[i]);
        }
    }
    /* Check 64~127 data, should be 0 */
    for (i = DMA_TRANSFER_LENGTH; i < DMA_BUFFER_MAX_LENGTH; i++) {
        if (dst_buffer[i] != 0) {
            printf("Error! Dst should be 0. index: %ld, src: 0x%02x, dst: 0x%02x\r\n", i, src_buffer[i], dst_buffer[i]);
        }
    }
    printf("Check over\r\n\r\n");

    memset(dst_buffer, 0, DMA_BUFFER_MAX_LENGTH);
    dma_tc_flag0 = 0;

    /* Enable reduce mode, src_width = 32bit, dst_width = 8bit, nbytes = 64, transfer length = (nbytes - 1 * dst_width) = 63 byte */
    bflb_dma_feature_control(dma0_ch0, DMA_CMD_SET_REDUCE_MODE, 1);
    bflb_dma_channel_lli_reload(dma0_ch0, lli, 20, transfers, 1);

    bflb_dma_channel_start(dma0_ch0);

    while (dma_tc_flag0 == 0) {
        bflb_mtimer_delay_ms(1);
    }

    /* Check 0~62 data */
    for (i = 0; i < DMA_TRANSFER_LENGTH - 1; i++) {
        if (src_buffer[i] != dst_buffer[i]) {
            printf("Error! index: %ld, src: 0x%02x, dst: 0x%02x\r\n", i, src_buffer[i], dst_buffer[i]);
        }
    }
    /* Check 63~127 data, should be 0 */
    for (i = DMA_TRANSFER_LENGTH - 1; i < DMA_BUFFER_MAX_LENGTH; i++) {
        if (dst_buffer[i] != 0) {
            printf("Error! Dst should be 0. index: %ld, src: 0x%02x, dst: 0x%02x\r\n", i, src_buffer[i], dst_buffer[i]);
        }
    }
    printf("Check over\r\n\r\n");

    memset(dst_buffer, 0, DMA_BUFFER_MAX_LENGTH);
    dma_tc_flag0 = 0;

    /* Disable reduce mode and enable add mode, src_width = 8bit, dst_width = 32bit, nbytes = 65, transfer length = nbytes + 3 = 68 byte, multiple of dst_width */
    bflb_dma_feature_control(dma0_ch0, DMA_CMD_SET_REDUCE_MODE, 0);
    bflb_dma_feature_control(dma0_ch0, DMA_CMD_SET_ADD_MODE, 1);

    config.src_burst_count = DMA_BURST_INCR4;
    config.dst_burst_count = DMA_BURST_INCR1;
    config.src_width = DMA_DATA_WIDTH_8BIT;
    config.dst_width = DMA_DATA_WIDTH_32BIT;
    bflb_dma_channel_init(dma0_ch0, &config);

    bflb_dma_channel_tcint_mask(dma0_ch0, false);

    transfers[0].nbytes = DMA_TRANSFER_LENGTH + 1;
    bflb_dma_channel_lli_reload(dma0_ch0, lli, 20, transfers, 1);

    bflb_dma_channel_start(dma0_ch0);

    while (dma_tc_flag0 == 0) {
        bflb_mtimer_delay_ms(1);
    }

    /* Check 0~64 data */
    for (i = 0; i < DMA_TRANSFER_LENGTH + 1; i++) {
        if (src_buffer[i] != dst_buffer[i]) {
            printf("Error! index: %ld, src: 0x%02x, dst: 0x%02x\r\n", i, src_buffer[i], dst_buffer[i]);
        }
    }
    /* Check 65~67 data */
    for (i = DMA_TRANSFER_LENGTH + 1; i < DMA_TRANSFER_LENGTH + 4; i++) {
        if (dst_buffer[i] == 0) {
            printf("Error! Dst should not be 0. index: %ld\r\n", i);
        } else {
            printf("Correct, meaningless data. index: %ld, src: 0x%02x, dst: 0x%02x\r\n", i, src_buffer[i], dst_buffer[i]);
        }
    }
    /* Check 68~127 data, should be 0 */
    for (i = DMA_TRANSFER_LENGTH + 4; i < DMA_BUFFER_MAX_LENGTH; i++) {
        if (dst_buffer[i] != 0) {
            printf("Error! Dst should be 0. index: %ld, src: 0x%02x, dst: 0x%02x\r\n", i, src_buffer[i], dst_buffer[i]);
        }
    }
    printf("Check over\r\n\r\n");

    memset(dst_buffer, 0, DMA_BUFFER_MAX_LENGTH);
    dma_tc_flag0 = 0;

    /* Disable add mode, src_width = 8bit, dst_width = 32bit, nbytes = 65, transfer length = nbytes - 1 = 64 byte, multiple of dst_width */
    bflb_dma_feature_control(dma0_ch0, DMA_CMD_SET_ADD_MODE, 0);
    bflb_dma_channel_lli_reload(dma0_ch0, lli, 20, transfers, 1);

    bflb_dma_channel_start(dma0_ch0);

    while (dma_tc_flag0 == 0) {
        bflb_mtimer_delay_ms(1);
    }

    /* Check 0~63 data */
    for (i = 0; i < DMA_TRANSFER_LENGTH; i++) {
        if (src_buffer[i] != dst_buffer[i]) {
            printf("Error! index: %ld, src: 0x%02x, dst: 0x%02x\r\n", i, src_buffer[i], dst_buffer[i]);
        }
    }
    /* Check 64~127 data, should be 0 */
    for (i = DMA_TRANSFER_LENGTH; i < DMA_BUFFER_MAX_LENGTH; i++) {
        if (dst_buffer[i] != 0) {
            printf("Error! Dst should be 0. index: %ld, src: 0x%02x, dst: 0x%02x\r\n", i, src_buffer[i], dst_buffer[i]);
        }
    }
    printf("Check over\r\n\r\n");

    printf("end\r\n");
    while (1) {
    }
}
