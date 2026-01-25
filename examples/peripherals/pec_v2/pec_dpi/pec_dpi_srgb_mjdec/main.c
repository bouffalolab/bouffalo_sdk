#include "board.h"
#include "bflb_pec_v2_instance.h"
#include "bflb_dma.h"
#include "bflb_mjdec.h"
#include "picture.h"

#define LCD_WIDTH  (480)
#define LCD_HEIGHT (272)
#define LCD_DEEPTH (3)
#define LCD_BYTES  (LCD_WIDTH * LCD_HEIGHT * LCD_DEEPTH)

struct bflb_device_s *pec_dpi;
struct bflb_device_s *dma0_ch0;
struct bflb_device_s *mjdec;

struct bflb_pec_dpi_s dpi_cfg = {
    .mem = 0,                               /*!< memory address of first instruction */
    .div = 3,                               /*!< divisor, N = div + 1 */
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

__attribute((aligned(32))) ATTR_NOINIT_PSRAM_SECTION uint8_t picture_a[LCD_BYTES];
__attribute((aligned(32))) ATTR_NOINIT_PSRAM_SECTION uint8_t picture_b[LCD_BYTES];
__attribute((aligned(32))) ATTR_NOINIT_PSRAM_SECTION uint8_t picture_yy[LCD_WIDTH * LCD_HEIGHT];
__attribute((aligned(32))) ATTR_NOINIT_PSRAM_SECTION uint8_t picture_uv[LCD_WIDTH * LCD_HEIGHT];
struct bflb_dma_channel_lli_transfer_s transfers[1];
struct bflb_dma_channel_lli_pool_s lli[200];
struct bflb_dma_channel_config_s config;
volatile int flag_ab_sel = 0;
volatile int flag_a_ready = 0;
volatile int flag_b_ready = 0;
volatile int flag_mjdec_done = 0;

void dma0_ch0_isr(void *arg)
{
    bflb_dma_channel_stop(dma0_ch0);
    if (flag_a_ready) {
        flag_a_ready = 0;
        flag_ab_sel = 0;
        transfers[0].src_addr = (uint32_t)picture_a;
    } else if (flag_b_ready) {
        flag_b_ready = 0;
        flag_ab_sel = 1;
        transfers[0].src_addr = (uint32_t)picture_b;
    } else {
        /* nothing */
    }
    bflb_dma_channel_lli_reload(dma0_ch0, lli, sizeof(lli) / sizeof(lli[0]), transfers, 1);
    bflb_dma_channel_start(dma0_ch0);
}

void pec_dpi_dma_init(void)
{
    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");
    printf("dma0_ch0 = 0x%08lX\r\n", dma0_ch0);

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
    transfers[0].src_addr = (uint32_t)picture_a;
    transfers[0].dst_addr = (uint32_t)DMA_ADDR_PEC_SM0_TDR;
    transfers[0].nbytes = LCD_BYTES;
    bflb_dma_channel_lli_reload(dma0_ch0, lli, sizeof(lli) / sizeof(lli[0]), transfers, 1);
    bflb_dma_channel_start(dma0_ch0);
}

uint8_t parse_sof0(uint8_t *header, uint16_t *x, uint16_t *y, uint8_t *format, uint16_t *skip)
{
    uint32_t len;
    uint8_t *p = header;

    if (p[0] != 0xFF || p[1] != 0xD8) {
        /* not jpeg header */
        return MJDEC_ERR_HEADER_SOI;
    }
    p += 2;
    while (1) {
        if (p[0] != 0xFF) {
            /* JPEG header format error */
            printf("error: header marker should be started by 0xFF.\r\n");
            return MJDEC_ERR_HEADER_MARKER;
        }
        p++;
        len = ((p[1] << 8) | p[2]);
        if (p[0] == 0xC0) {
            if (p[3] != 8) {
                printf("accuracy should be 8.\r\n");
                return 0xF1;
            }
            *y = (p[4] << 8) | p[5];
            *x = (p[6] << 8) | p[7];
            if (p[8] == 1) {
                if (p[9] == 1 && p[10] == 0x11 && p[11] == 0) {
                    *format = MJDEC_FORMAT_GRAY;
                } else {
                    printf("gray image error, id=0x%02X, sample=0x%02X, dqt=0x%02X.\r\n", p[9], p[10], p[11]);
                    return 0xF1;
                }
            } else if (p[8] == 3) {
                if (p[9] == 1 && p[11] == 0 && p[12] == 2 && p[13] == 0x11 && p[14] == 1 && p[15] == 3 && p[16] == 0x11 && p[17] == 1) {
                    if (p[10] == 0x21)  {
                        *format = MJDEC_FORMAT_YUV422SP_NV16;
                    } else if (p[10] == 0x22) {
                        *format = MJDEC_FORMAT_YUV420SP_NV12;
                    } else {
                        printf("component: %02X %02X %02X, %02X %02X %02X, %02X %02X %02X\r\n", p[9], p[10], p[11], p[12], p[13], p[14], p[15], p[16], p[17]);
                        printf("only support NV16 or NV12 format.\r\n");
                        return 0xF1;
                    }
                }
            } else {
                printf("only support gray or YCbCr format.\r\n");
                return 0xF2;
            }
        } else if (p[0] == 0xDA) {
            /* start of scan or EOI or other invalid marker */
            *skip = (uint16_t)(p + len + 1 - header);
            break;
        }
        p = p + len + 1;
    }
    return 0;
}

void mjdec_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_mjdec_get_intstatus(mjdec);
    if (intstatus & MJDEC_INTSTS_ONE_FRAME) {
        bflb_mjdec_int_clear(mjdec, MJDEC_INTCLR_ONE_FRAME);
        flag_mjdec_done = 1;
        bflb_mjdec_pop_one_frame(mjdec);
    }
}

int mjpeg_to_yuv(uint8_t *jpg, uint8_t *yy, uint8_t *uv)
{
    int ret;
    uint16_t X, Y;
    uint8_t format;
    uint16_t header_skip;
    uint64_t time;
    struct bflb_mjdec_config_s config;

    ret = parse_sof0(jpg, &X, &Y, &format, &header_skip);
    if (ret) {
        printf("parse header error, ret = 0x%X\r\n", ret);
        return -1;
    }
    config.format = format;
    config.swap_enable = true;
    config.resolution_x = X;
    config.resolution_y = Y;
#if defined(BL616D) && !defined(CPU_MODEL_A0)
    config.head_size = header_skip;
#else
    if (header_skip & 0x7) {
        config.head_size = header_skip;
    } else {
        config.head_size = 0;
    }
#endif
    config.output_bufaddr0 = (uint32_t)yy;
    config.output_bufaddr1 = (uint32_t)uv;
    bflb_mjdec_init(mjdec, &config);
    ret = bflb_mjdec_set_dqt_from_header(mjdec, jpg);
    if (ret) {
        printf("bflb_mjdec_set_dqt_from_header error, %d\r\n", ret);
    }
    ret = bflb_mjdec_set_dht_from_header(mjdec, jpg);
    if (ret) {
        printf("bflb_mjdec_set_dht_from_header error, %d\r\n", ret);
    }
    bflb_mjdec_tcint_mask(mjdec, false);
    bflb_irq_attach(mjdec->irq_num, mjdec_isr, NULL);
    bflb_irq_enable(mjdec->irq_num);
    bflb_mjdec_start(mjdec);
#if defined(BL616D) && !defined(CPU_MODEL_A0)
    bflb_mjdec_push_jpeg(mjdec, jpg);
#else
    if (header_skip & 0x7) {
        bflb_mjdec_push_jpeg(mjdec, jpg);
    } else {
        bflb_mjdec_push_jpeg(mjdec, jpg + header_skip);
    }
#endif
    time = bflb_mtimer_get_time_ms();
    while ((bflb_mtimer_get_time_ms() - time) < 25) {
        if (flag_mjdec_done) {
            bflb_l1c_dcache_invalidate_range(picture_yy, sizeof(picture_yy));
            bflb_l1c_dcache_invalidate_range(picture_uv, sizeof(picture_uv));
            flag_mjdec_done = 0;
            return 0;
        }
    }
    printf("mjdec timeout error\r\n");
    return -1;
}

static inline uint8_t clamp(int val) {
    if (val < 0)  return 0;
    if (val > 255) return 255;
    return (uint8_t)val;
}

void nv12_to_rbg(uint8_t *yy, uint8_t *uv, uint8_t *rgb, uint16_t width, uint16_t height)
{
    if (yy == NULL || uv == NULL || rgb == NULL || width <= 0 || height <= 0) {
        return;
    }

    for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x += 2) {
            const int uv_row = y / 2;
            const int uv_col = x / 2;
            const int uv_offset = uv_row * width + uv_col * 2;
            
            const uint8_t U = uv[uv_offset];
            const uint8_t V = uv[uv_offset + 1];
            const int U128 = U - 128;
            const int V128 = V - 128;

            for (int dy = 0; dy < 2; dy++) {
                if (y + dy >= height) break;

                for (int dx = 0; dx < 2; dx++) {
                    if (x + dx >= width) break;

                    const int y_idx = (y + dy) * width + (x + dx);
                    const uint8_t Y = yy[y_idx];
                    const int Y16 = Y - 16;

                    int R = (298 * Y16 + 409 * V128) >> 8;
                    int G = (298 * Y16 - 100 * U128 - 207 * V128) >> 8;
                    int B = (298 * Y16 + 517 * U128) >> 8;

                    const int rgb_idx = ((y + dy) * width + (x + dx)) * 3;
                    rgb[rgb_idx]     = clamp(R);
                    rgb[rgb_idx + 1] = clamp(G);
                    rgb[rgb_idx + 2] = clamp(B);
                }
            }
        }
    }
}

int main(void)
{
    board_init();
    board_pec_dpi_gpio_init();
    pec_dpi = bflb_device_get_by_name(BFLB_NAME_PEC_SM0);
    printf("pec_dpi = 0x%08lX\r\n", pec_dpi);
    mjdec = bflb_device_get_by_name(BFLB_NAME_MJDEC);
    printf("mjdec = 0x%08lX\r\n", mjdec);

    bflb_pec_dpi_init(pec_dpi, &dpi_cfg);
    pec_dpi_dma_init();
    printf("pec_dpi_srgb show picture from jpg example.\r\n");
    while (1) {
        static uint32_t i = 0;
        mjpeg_to_yuv(image_data_raw + image_data_offset[i++], picture_yy, picture_uv);
        if (flag_ab_sel == 0) {
            nv12_to_rbg(picture_yy, picture_uv, picture_b, LCD_WIDTH, LCD_HEIGHT);
            flag_b_ready = 1;
        } else {
            nv12_to_rbg(picture_yy, picture_uv, picture_a, LCD_WIDTH, LCD_HEIGHT);
            flag_a_ready = 1;
        }
        if (i >= IMAGE_DATA_COUNT) {
            i = 0;
        }
    }
}
