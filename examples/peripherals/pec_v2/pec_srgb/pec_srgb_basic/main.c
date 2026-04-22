#include "board.h"
#include "bflb_pec_v2_instance.h"
#include "bflb_dma.h"
#include "pic_rgb888.h"

#define LCD_WIDTH  (480)
#define LCD_HEIGHT (272)
#define PIC_FORMAT (PEC_SRGB_FORMAT_NRGB888)

struct bflb_device_s *pec_srgb;
struct bflb_device_s *dma0_ch0;

struct bflb_pec_srgb_s srgb_cfg = {
    .mem = 0,                             /*!< memory address of first instruction */
    .format = PIC_FORMAT,                 /*!< sRGB data format, PEC_SRGB_FORMAT_xxx */
    .clk_freq = PEC_SRGB_CLOCK_20MHZ,     /*!< sRGB clock frequency, PEC_SRGB_CLOCK_xxx */
    .dma_enable = true,                   /*!< enable or disable dma */
    .fifo_threshold = 4 - 1,              /*!< tx fifo threshold */
    .pin_data_start = PEC_SRGB_DATA0_PIN, /*!< sRGB data pin start index, data pin number must be continuous */
    .pin_clk = PEC_SRGB_CLK_PIN,          /*!< sRGB clock pin */
    .pin_vsync = PEC_SRGB_VSYNC_PIN,      /*!< sRGB vsync pin */
    .pin_hsync = PEC_SRGB_HSYNC_PIN,      /*!< sRGB hsync pin */
    .pin_de = PEC_SRGB_DE_PIN,            /*!< sRGB de pin */
    .level_clk = 0,                       /*!< sRGB clock level, 0: sample on falling edge, others: sample on rising edge */
    .level_vsync = 0,                     /*!< sRGB vsync level in vertical synchronization segment */
    .level_hsync = 0,                     /*!< sRGB hsync level in horizontal synchronization segment */
    .level_de = 1,                        /*!< sRGB de level when data is enabled */
    .hsync = 1,                           /*!< sRGB clock count in horizontal synchronization segment, max is 1024 */
    .hbp = 43,                            /*!< sRGB clock count in horizontal back porch segment, max is 2048 */
    .hfp = 2,                             /*!< sRGB clock count in horizontal front porch segment, max is 2048 */
    .vsync = 1,                           /*!< sRGB row count in vertical synchronization segment, max is 1024 */
    .vbp = 12,                            /*!< sRGB row count in vertical back porch segment, max is 2048 */
    .vfp = 1,                             /*!< sRGB row count in vertical front porch segment, max is 2048 */
    .width = LCD_WIDTH,                   /*!< sRGB actual picture width, max is 65536 */
    .height = LCD_HEIGHT,                 /*!< sRGB actual picture height, max is 65536 */
};

ATTR_NOINIT_PSRAM_SECTION __attribute((aligned(BFLB_CACHE_LINE_SIZE))) uint8_t frame_buffer[LCD_HEIGHT * LCD_WIDTH * 4];
struct bflb_dma_channel_lli_transfer_s transfers[1];
struct bflb_dma_channel_lli_pool_s lli[200];
volatile uint32_t dma_tc_flag0 = 0;

void pec_srgb_pic_buffer_init(struct bflb_pec_srgb_s *srgb)
{
    for (uint16_t y = 0; y < LCD_HEIGHT; y++) {
        for (uint16_t x = 0; x < LCD_WIDTH; x++) {
#if (PIC_FORMAT == PEC_SRGB_FORMAT_RGB565)
            uint8_t r, g, b;
            r = (pic_src_rgb888[(y * LCD_WIDTH + x) * 3 + 0] >> 3) & 0x1F;
            g = (pic_src_rgb888[(y * LCD_WIDTH + x) * 3 + 1] >> 2) & 0x3F;
            b = (pic_src_rgb888[(y * LCD_WIDTH + x) * 3 + 2] >> 3) & 0x1F;
            frame_buffer[(y * LCD_WIDTH + x) * 2 + 1] = (r << 3) | (g >> 3);
            frame_buffer[(y * LCD_WIDTH + x) * 2 + 0] = (g << 5) | b;
#elif (PIC_FORMAT == PEC_SRGB_FORMAT_BGR565)
            uint16_t r, g, b;
            r = (pic_src_rgb888[(y * LCD_WIDTH + x) * 3 + 0] >> 3) & 0x1F;
            g = (pic_src_rgb888[(y * LCD_WIDTH + x) * 3 + 1] >> 2) & 0x3F;
            b = (pic_src_rgb888[(y * LCD_WIDTH + x) * 3 + 2] >> 3) & 0x1F;
            frame_buffer[(y * LCD_WIDTH + x) * 2 + 0] = (r << 3) | (g >> 3);
            frame_buffer[(y * LCD_WIDTH + x) * 2 + 1] = (g << 5) | b;
#elif (PIC_FORMAT == PEC_SRGB_FORMAT_RGB888)
            frame_buffer[(y * LCD_WIDTH + x) * 3 + 0] = pic_src_rgb888[(y * LCD_WIDTH + x) * 3 + 2];
            frame_buffer[(y * LCD_WIDTH + x) * 3 + 1] = pic_src_rgb888[(y * LCD_WIDTH + x) * 3 + 1];
            frame_buffer[(y * LCD_WIDTH + x) * 3 + 2] = pic_src_rgb888[(y * LCD_WIDTH + x) * 3 + 0];
#elif (PIC_FORMAT == PEC_SRGB_FORMAT_BGR888)
            frame_buffer[(y * LCD_WIDTH + x) * 3 + 0] = pic_src_rgb888[(y * LCD_WIDTH + x) * 3 + 0];
            frame_buffer[(y * LCD_WIDTH + x) * 3 + 1] = pic_src_rgb888[(y * LCD_WIDTH + x) * 3 + 1];
            frame_buffer[(y * LCD_WIDTH + x) * 3 + 2] = pic_src_rgb888[(y * LCD_WIDTH + x) * 3 + 2];
#elif (PIC_FORMAT == PEC_SRGB_FORMAT_NRGB888)
            frame_buffer[(y * LCD_WIDTH + x) * 4 + 0] = pic_src_rgb888[(y * LCD_WIDTH + x) * 3 + 2];
            frame_buffer[(y * LCD_WIDTH + x) * 4 + 1] = pic_src_rgb888[(y * LCD_WIDTH + x) * 3 + 1];
            frame_buffer[(y * LCD_WIDTH + x) * 4 + 2] = pic_src_rgb888[(y * LCD_WIDTH + x) * 3 + 0];
            frame_buffer[(y * LCD_WIDTH + x) * 4 + 3] = 0;
#elif (PIC_FORMAT == PEC_SRGB_FORMAT_NBGR888)
            frame_buffer[(y * LCD_WIDTH + x) * 4 + 0] = pic_src_rgb888[(y * LCD_WIDTH + x) * 3 + 0];
            frame_buffer[(y * LCD_WIDTH + x) * 4 + 1] = pic_src_rgb888[(y * LCD_WIDTH + x) * 3 + 1];
            frame_buffer[(y * LCD_WIDTH + x) * 4 + 2] = pic_src_rgb888[(y * LCD_WIDTH + x) * 3 + 2];
            frame_buffer[(y * LCD_WIDTH + x) * 4 + 3] = 0;
#else
    #error "please select correct PIC_FORMAT"
#endif
        }
    }
    bflb_l1c_dcache_clean_range(frame_buffer, sizeof(frame_buffer));
}

void dma0_ch0_isr(void *arg)
{
    dma_tc_flag0++;
    printf("tc done, flag = %u\r\n", dma_tc_flag0);
}

void pec_srgb_dma_init(void)
{
    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");
    printf("dma0_ch0 = 0x%08lX\r\n", dma0_ch0);

    struct bflb_dma_channel_config_s config;
    config.direction = DMA_MEMORY_TO_PERIPH;
    config.src_req = DMA_REQUEST_NONE;
    config.dst_req = DMA_REQUEST_PEC_SM0_TX;
    config.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    config.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    config.src_burst_count = DMA_BURST_INCR4;
    config.dst_burst_count = DMA_BURST_INCR4;
    config.src_width = DMA_DATA_WIDTH_32BIT;
    config.dst_width = DMA_DATA_WIDTH_32BIT;
    bflb_dma_channel_init(dma0_ch0, &config);

    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);

    transfers[0].src_addr = (uint32_t)frame_buffer;
    transfers[0].dst_addr = (uint32_t)DMA_ADDR_PEC_SM0_TDR;
#if ((PIC_FORMAT == PEC_SRGB_FORMAT_RGB565) || (PIC_FORMAT == PEC_SRGB_FORMAT_BGR565))
    transfers[0].nbytes = LCD_WIDTH * LCD_HEIGHT * 2;
#elif ((PIC_FORMAT == PEC_SRGB_FORMAT_RGB888) || (PIC_FORMAT == PEC_SRGB_FORMAT_BGR888))
    transfers[0].nbytes = LCD_WIDTH * LCD_HEIGHT * 3;
#elif ((PIC_FORMAT == PEC_SRGB_FORMAT_NRGB888) || (PIC_FORMAT == PEC_SRGB_FORMAT_NBGR888))
    transfers[0].nbytes = LCD_WIDTH * LCD_HEIGHT * 4;
#else
    #error "please select correct PIC_FORMAT"
#endif
    int used_count = bflb_dma_channel_lli_reload(dma0_ch0, lli, sizeof(lli) / sizeof(lli[0]), transfers, 1);
    bflb_dma_channel_lli_link_head(dma0_ch0, lli, used_count);
    bflb_dma_channel_start(dma0_ch0);
}

int main(void)
{
    board_init();
    board_pec_srgb_gpio_init();
    pec_srgb = bflb_device_get_by_name("pec_sm0");
    printf("pec_srgb = 0x%08lX\r\n", pec_srgb);

    bflb_pec_srgb_init(pec_srgb, &srgb_cfg);
    pec_srgb_pic_buffer_init(&srgb_cfg);
    pec_srgb_dma_init();
    printf("pec_srgb example\r\n");
    while (1);
}
