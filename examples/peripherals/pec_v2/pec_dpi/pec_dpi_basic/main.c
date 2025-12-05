#include "board.h"
#include "bflb_pec_v2_instance.h"
#include "bflb_dma.h"

#define LCD_WIDTH                       800
#define LCD_HEIGHT                      480
#define LCD_PIXEL                       (LCD_WIDTH * LCD_HEIGHT)

struct bflb_device_s *pec_dpi;
struct bflb_device_s *dma0_ch0;

struct bflb_pec_dpi_s dpi_cfg = {
    .mem = 0,                                /*!< memory address of first instruction */
    .div = 0,                                /*!< divisor, N = div + 1 */
    .dma_enable = true,                      /*!< enable or disable dma */
    .fifo_threshold = 4 - 1,                 /*!< tx fifo threshold */
    .pin_data_start[0] = PEC_DPI_DATA0_PIN,  /*!< DPI data pin start index, data pin number is continuous in one group */
    .pin_data_count[0] = 16,                 /*!< DPI data group pin count */
    .pin_data_start[1] = 0,                  /*!< DPI data pin start index, data pin number is continuous in one group */
    .pin_data_count[1] = 0,                  /*!< DPI data group pin count */
    .pin_data_start[2] = 0,                  /*!< DPI data pin start index, data pin number is continuous in one group */
    .pin_data_count[2] = 0,                  /*!< DPI data group pin count */
    .pin_pclk = PEC_DPI_PCLK_PIN,            /*!< DPI pixel clock pin */
    .pin_vsync = PEC_DPI_VSYNC_PIN,          /*!< DPI vsync pin */
    .pin_hsync = PEC_DPI_HSYNC_PIN,          /*!< DPI hsync pin */
    .pin_de = PEC_DPI_DE_PIN,                /*!< DPI de pin */
    .level_vsync = 0,                        /*!< DPI vsync level in vertical synchronization segment */
    .level_hsync = 0,                        /*!< DPI vsync level in horizontal synchronization segment */
    .level_de = 1,                           /*!< DPI de level when data is enabled */
    .hsync = 45,                             /*!< DPI pixel clock count in horizontal synchronization segment, max is 1024 */
    .hbp = 45,                               /*!< DPI pixel clock count in horizontal back porch segment, max is 2048 */
    .hfp = 89,                               /*!< DPI pixel clock count in horizontal front porch segment, max is 2048 */
    .vsync = 7,                              /*!< DPI row count in vertical synchronization segment, max is 1024 */
    .vbp = 7,                                /*!< DPI row count in vertical back porch segment, max is 2048 */
    .vfp = 5,                                /*!< DPI row count in vertical front porch segment, max is 2048 */
    .width = LCD_WIDTH,                      /*!< DPI actual picture width, max is 65535 */
    .height = LCD_HEIGHT,                    /*!< DPI actual picture height, max is 65535 */
};

ATTR_NOINIT_PSRAM_SECTION __attribute((aligned(32))) uint16_t framebuffer[LCD_HEIGHT][LCD_WIDTH];
struct bflb_dma_channel_lli_transfer_s transfers[1];
struct bflb_dma_channel_lli_pool_s lli[200];
volatile uint32_t dma_tc_flag0 = 0;

int lcd_draw_circle(uint16_t *screen_buffer, uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
    int a = 0, b;
    int di;
    b = r;
    di = 1 - r;

    while (a <= b) {
        screen_buffer[(x - b) * LCD_WIDTH + (y - a)] = color;
        screen_buffer[(x + b) * LCD_WIDTH + (y - a)] = color;
        screen_buffer[(x - a) * LCD_WIDTH + (y + b)] = color;
        screen_buffer[(x - a) * LCD_WIDTH + (y - b)] = color;
        screen_buffer[(x + b) * LCD_WIDTH + (y + a)] = color;
        screen_buffer[(x + a) * LCD_WIDTH + (y - b)] = color;
        screen_buffer[(x + a) * LCD_WIDTH + (y + b)] = color;
        screen_buffer[(x - b) * LCD_WIDTH + (y + a)] = color;
        a++;

        if (di < 0) {
            di += (a << 1) + 3;
        } else {
            di += ((a - b) << 1) + 5;
            b--;
        }
    }
    return 0;
}

void draw_rectangle_border(void *fb, uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                           uint8_t thickness, uint16_t color)
{
    uint16_t i, j;
    uint16_t *fb_rgb565 = (uint16_t *)fb;

    /* Boundary checks */
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT) {
        return;
    }
    if (x + width > LCD_WIDTH) {
        width = LCD_WIDTH - x;
    }
    if (y + height > LCD_HEIGHT) {
        height = LCD_HEIGHT - y;
    }
    if (thickness == 0 || width < 2 * thickness || height < 2 * thickness) {
        return;
    }

    /* Draw top border */
    for (j = 0; j < thickness && (y + j) < LCD_HEIGHT; j++) {
        for (i = x; i < x + width && i < LCD_WIDTH; i++) {
            fb_rgb565[(y + j) * LCD_WIDTH + i] = color;
        }
    }

    /* Draw bottom border */
    for (j = height - thickness; j < height && (y + j) < LCD_HEIGHT; j++) {
        for (i = x; i < x + width && i < LCD_WIDTH; i++) {
            fb_rgb565[(y + j) * LCD_WIDTH + i] = color;
        }
    }

    /* Draw left border */
    for (j = thickness; j < height - thickness && (y + j) < LCD_HEIGHT; j++) {
        for (i = 0; i < thickness && (x + i) < LCD_WIDTH; i++) {
            fb_rgb565[(y + j) * LCD_WIDTH + (x + i)] = color;
        }
    }

    /* Draw right border */
    for (j = thickness; j < height - thickness && (y + j) < LCD_HEIGHT; j++) {
        for (i = width - thickness; i < width && (x + i) < LCD_WIDTH; i++) {
            fb_rgb565[(y + j) * LCD_WIDTH + (x + i)] = color;
        }
    }
}

void pec_dpi_pic_init(void)
{
    for (uint32_t y = 0; y < LCD_HEIGHT; y++) {
        for (uint32_t x = 0; x < LCD_WIDTH; x++) {
            framebuffer[y][x] = 0x00;
        }
    }
    for (uint32_t y = 0; y < LCD_HEIGHT; y++) {
        framebuffer[y][LCD_WIDTH / 2] = 0xFF00;
    }
    for (uint32_t x = 0; x < LCD_WIDTH; x++) {
        framebuffer[240][x] = 0xFF00;
    }
    draw_rectangle_border(framebuffer, LCD_WIDTH / 2, 0, LCD_WIDTH / 2 - 1, LCD_HEIGHT - 1, 5, 0x00FF);
    draw_rectangle_border(framebuffer, 0, LCD_HEIGHT / 2, LCD_WIDTH - 1, LCD_HEIGHT / 2 - 1, 15, 0xFF00);
    lcd_draw_circle((void *)framebuffer, LCD_HEIGHT / 2, LCD_WIDTH / 2, 200, 0xFF00);
    lcd_draw_circle((void *)framebuffer, LCD_HEIGHT / 2, LCD_WIDTH / 2, 100, 0x00FF);
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
    config.src_width = DMA_DATA_WIDTH_16BIT;
    config.dst_width = DMA_DATA_WIDTH_16BIT;
    bflb_dma_channel_init(dma0_ch0, &config);

    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);

    transfers[0].src_addr = (uint32_t)framebuffer;
    transfers[0].dst_addr = (uint32_t)DMA_ADDR_PEC_SM0_TDR;
    transfers[0].nbytes = LCD_WIDTH * LCD_HEIGHT * sizeof(uint16_t);
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
