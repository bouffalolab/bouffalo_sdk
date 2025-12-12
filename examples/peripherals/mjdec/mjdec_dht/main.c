#include "bflb_mtimer.h"
#include "bflb_mjdec.h"
#include "board.h"
#include "jpg.h"

uint16_t header_skip;
uint16_t X, Y;
uint8_t YUV_FORMAT;

struct bflb_mjdec_config_s config;
static struct bflb_device_s *mjdec;
volatile uint32_t pic_count = 0;

static __attribute__((aligned(32))) ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t yuv_buffer[160 * 160 * 2];

__UNUSED static uint8_t buffer_temp[160 * 160 / 2];
__UNUSED static void nv16_to_yuv422(void)
{
    for (uint32_t i = 0; i < X * Y / 2; i++) {
        buffer_temp[i] = yuv_buffer[X * Y + 2 * i + 1];
    }
    for (uint32_t i = 0; i < X * Y / 2; i++) {
        yuv_buffer[X * Y + i] = yuv_buffer[X * Y + 2 * i];
    }
    for (uint32_t i = 0; i < X * Y / 2; i++) {
        yuv_buffer[X * Y + X * Y / 2 + i] = buffer_temp[i];
    }
}

__UNUSED static void nv61_to_yuv422(void)
{
    for (uint32_t i = 0; i < X * Y / 2; i++) {
        buffer_temp[i] = yuv_buffer[X * Y + 2 * i];
    }
    for (uint32_t i = 0; i < X * Y / 2; i++) {
        yuv_buffer[X * Y + i] = yuv_buffer[X * Y + 2 * i + 1];
    }
    for (uint32_t i = 0; i < X * Y / 2; i++) {
        yuv_buffer[X * Y + X * Y / 2 + i] = buffer_temp[i];
    }
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
        pic_count = 1;
        bflb_mjdec_pop_one_frame(mjdec);
    }
}

int main(void)
{
    uint8_t ret = 0;
    board_init();

    mjdec = bflb_device_get_by_name("mjdec");
    ret = parse_sof0(jpeg_buffer, &X, &Y, &YUV_FORMAT, &header_skip);
    if (ret) {
        printf("parse header error!\r\n");
        return -1;
    }
    config.format = YUV_FORMAT;
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
    config.output_bufaddr0 = (uint32_t)(&yuv_buffer);
    config.output_bufaddr1 = (uint32_t)(&yuv_buffer[X * Y]);

    bflb_mjdec_init(mjdec, &config);
    ret = bflb_mjdec_set_dqt_from_header(mjdec, jpeg_buffer);
    if (ret) {
        printf("bflb_mjdec_set_dqt_from_header error, %d\r\n", ret);
    }
    ret = bflb_mjdec_set_dht_from_header(mjdec, jpeg_buffer);
    if (ret) {
        printf("bflb_mjdec_set_dht_from_header error, %d\r\n", ret);
    }
    bflb_mjdec_tcint_mask(mjdec, false);
    bflb_irq_attach(mjdec->irq_num, mjdec_isr, NULL);
    bflb_irq_enable(mjdec->irq_num);
    bflb_mjdec_start(mjdec);
#if defined(BL616D) && !defined(CPU_MODEL_A0)
    bflb_mjdec_push_jpeg(mjdec, jpeg_buffer);
#else
    if (header_skip & 0x7) {
        bflb_mjdec_push_jpeg(mjdec, jpeg_buffer);
    } else {
        bflb_mjdec_push_jpeg(mjdec, jpeg_buffer + header_skip);
    }
#endif

    while (pic_count != 1) {
        bflb_mtimer_delay_ms(200);
    }

    printf("picture size is %u x %u\r\n", X, Y);
    switch(YUV_FORMAT) {
        case MJDEC_FORMAT_YUV422SP_NV16:
            nv16_to_yuv422();
            printf("\r\nmode %s complete\r\n\r\n", "MJDEC_FORMAT_YUV422SP_NV16");
            printf("dump binary memory %s.bin 0x%08lX 0x%08lX\r\n", "YUV422SP_NV16", yuv_buffer, yuv_buffer + X * Y * 2);
            break;
        case MJDEC_FORMAT_YUV422SP_NV61:
            nv61_to_yuv422();
            printf("\r\nmode %s complete\r\n\r\n", "MJDEC_FORMAT_YUV422SP_NV61");
            printf("dump binary memory %s.bin 0x%08lX 0x%08lX\r\n", "YUV422SP_NV61", yuv_buffer, yuv_buffer + X * Y * 2);
            break;
        case MJDEC_FORMAT_YUV420SP_NV12:
            printf("\r\nmode %s complete\r\n\r\n", "MJDEC_FORMAT_YUV420SP_NV12");
            printf("dump binary memory %s.bin 0x%08lX 0x%08lX\r\n", "YUV420SP_NV12", yuv_buffer, yuv_buffer + X * Y / 2 * 3);
            break;
        case MJDEC_FORMAT_YUV420SP_NV21:
            printf("\r\nmode %s complete\r\n\r\n", "MJDEC_FORMAT_YUV420SP_NV21");
            printf("dump binary memory %s.bin 0x%08lX 0x%08lX\r\n", "YUV420SP_NV21", yuv_buffer, yuv_buffer + X * Y / 2 * 3);
            break;
        case MJDEC_FORMAT_GRAY:
            printf("\r\nmode %s complete\r\n\r\n", "MJDEC_FORMAT_GRAY");
            printf("dump binary memory %s.bin 0x%08lX 0x%08lX\r\n", "GRAY", yuv_buffer, yuv_buffer + X * Y);
            break;
    }

    return 0;
}