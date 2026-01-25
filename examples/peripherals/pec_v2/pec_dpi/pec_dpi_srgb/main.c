#include "board.h"
#include "bflb_pec_v2_instance.h"
#include "bflb_dma.h"

#define LCD_WIDTH  (480)
#define LCD_HEIGHT (272)
#define LCD_DEEPTH (3)
#define LCD_BYTES  (LCD_WIDTH * LCD_HEIGHT * LCD_DEEPTH)

struct bflb_device_s *pec_dpi;
struct bflb_device_s *dma0_ch0;

struct bflb_pec_dpi_s dpi_cfg = {
    .mem = 0,                               /*!< memory address of first instruction */
    .div = 1,                               /*!< divisor, N = div + 1 */
    .dma_enable = true,                     /*!< enable or disable dma */
    .fifo_threshold = 4 - 1,                /*!< tx fifo threshold */
    .pin_data_start[0] = PEC_DPI_DATA0_PIN, /*!< DPI data pin start index, data pin number is continuous in one group */
    .pin_data_count[0] = 8,                 /*!< DPI data group pin count */
    .pin_data_start[1] = 0,                 /*!< DPI data pin start index, data pin number is continuous in one group */
    .pin_data_count[1] = 0,                 /*!< DPI data group pin count */
    .pin_data_start[2] = 0,                 /*!< DPI data pin start index, data pin number is continuous in one group */
    .pin_data_count[2] = 0,                 /*!< DPI data group pin count */
    .pin_pclk = PEC_DPI_PCLK_PIN,           /*!< DPI pixel clock pin */
    .pin_vsync = PEC_DPI_VSYNC_PIN,         /*!< DPI vsync pin */
    .pin_hsync = PEC_DPI_HSYNC_PIN,         /*!< DPI hsync pin */
    .pin_de = PEC_DPI_DE_PIN,               /*!< DPI de pin */
    .level_pclk = 0,                        /*!< DPI pixel clock level, 0: sample on falling edge, others: sample on rising edge */
    .level_vsync = 0,                       /*!< DPI vsync level in vertical synchronization segment */
    .level_hsync = 0,                       /*!< DPI vsync level in horizontal synchronization segment */
    .level_de = 1,                          /*!< DPI de level when data is enabled */
    .hsync = 1,                             /*!< DPI pixel clock count in horizontal synchronization segment, max is 1024 */
    .hbp = 43,                              /*!< DPI pixel clock count in horizontal back porch segment, max is 2048 */
    .hfp = 2,                               /*!< DPI pixel clock count in horizontal front porch segment, max is 2048 */
    .vsync = 1,                             /*!< DPI row count in vertical synchronization segment, max is 1024 */
    .vbp = 12,                              /*!< DPI row count in vertical back porch segment, max is 2048 */
    .vfp = 1,                               /*!< DPI row count in vertical front porch segment, max is 2048 */
    .width = LCD_WIDTH * LCD_DEEPTH,        /*!< DPI actual picture width, max is 65535 */
    .height = LCD_HEIGHT,                   /*!< DPI actual picture height, max is 65535 */
};

__attribute((aligned(32))) ATTR_NOINIT_PSRAM_SECTION uint8_t framebuffer[LCD_BYTES];
struct bflb_dma_channel_lli_transfer_s transfers[1];
struct bflb_dma_channel_lli_pool_s lli[200];
volatile uint32_t dma_tc_flag0 = 0;

void pec_dpi_pic_init(void)
{
    for (uint32_t i = 0; i < LCD_HEIGHT; i++) {
        for (uint32_t j = 0; j < LCD_WIDTH / 6; j++) {
            framebuffer[(i * LCD_WIDTH + j) * LCD_DEEPTH + 0] = 0x00;
            framebuffer[(i * LCD_WIDTH + j) * LCD_DEEPTH + 1] = 0x00;
            framebuffer[(i * LCD_WIDTH + j) * LCD_DEEPTH + 2] = 0xFF;
        }
    }
    for (uint32_t i = 0; i < LCD_HEIGHT; i++) {
        for (uint32_t j = LCD_WIDTH / 6; j < LCD_WIDTH / 2; j++) {
            framebuffer[(i * LCD_WIDTH + j) * LCD_DEEPTH + 0] = 0x00;
            framebuffer[(i * LCD_WIDTH + j) * LCD_DEEPTH + 1] = 0xFF;
            framebuffer[(i * LCD_WIDTH + j) * LCD_DEEPTH + 2] = 0x00;
        }
    }
    for (uint32_t i = 0; i < LCD_HEIGHT; i++) {
        for (uint32_t j = LCD_WIDTH / 2; j < LCD_WIDTH; j++) {
            framebuffer[(i * LCD_WIDTH + j) * LCD_DEEPTH + 0] = 0xFF;
            framebuffer[(i * LCD_WIDTH + j) * LCD_DEEPTH + 1] = 0x00;
            framebuffer[(i * LCD_WIDTH + j) * LCD_DEEPTH + 2] = 0x00;
        }
    }
    for (uint32_t i = 0; i < 10; i++) {
        for (uint32_t j = 0; j < 20; j++) {
            framebuffer[(i * LCD_WIDTH + j) * LCD_DEEPTH + 0] = 0xFF;
            framebuffer[(i * LCD_WIDTH + j) * LCD_DEEPTH + 1] = 0xFF;
            framebuffer[(i * LCD_WIDTH + j) * LCD_DEEPTH + 2] = 0xFF;
        }
    }
    bflb_l1c_dcache_clean_range(framebuffer, sizeof(framebuffer));
}

void dma0_ch0_isr(void *arg)
{
    dma_tc_flag0++;
    printf("tc done, flag = %u\r\n", dma_tc_flag0);
}

void pec_dpi_dma_init(void)
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

    transfers[0].src_addr = (uint32_t)framebuffer;
    transfers[0].dst_addr = (uint32_t)DMA_ADDR_PEC_SM0_TDR;
    transfers[0].nbytes = LCD_BYTES;
    int used_count = bflb_dma_channel_lli_reload(dma0_ch0, lli, sizeof(lli) / sizeof(lli[0]), transfers, 1);
    bflb_dma_channel_lli_link_head(dma0_ch0, lli, used_count);
    bflb_dma_channel_start(dma0_ch0);
}

int main(void)
{
    board_init();
    board_pec_dpi_gpio_init();
    pec_dpi = bflb_device_get_by_name("pec_sm0");
    printf("pec_dpi = 0x%08lX\r\n", pec_dpi);

    bflb_pec_dpi_init(pec_dpi, &dpi_cfg);
    pec_dpi_pic_init();
    pec_dpi_dma_init();
    printf("pec_dpi example\r\n");
    while (1);
}
