#include "bflb_mtimer.h"
#include "bflb_mjdec.h"
#include "board.h"
// #define YUV_FORMAT MJDEC_FORMAT_YUV422SP_NV16
// #define YUV_FORMAT MJDEC_FORMAT_YUV422SP_NV61
// #define YUV_FORMAT MJDEC_FORMAT_YUV420SP_NV12
// #define YUV_FORMAT MJDEC_FORMAT_YUV420SP_NV21
#define YUV_FORMAT MJDEC_FORMAT_GRAY
#include "jpg.h"

#define X             128
#define Y             128

struct bflb_mjdec_config_s config;
static struct bflb_device_s *mjdec;
volatile uint32_t pic_count = 0;

static __attribute__((aligned(32))) ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t yuv_buffer[X * Y * 2];

__UNUSED static uint8_t buffer_temp[X * Y / 2];
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
    board_init();

    mjdec = bflb_device_get_by_name("mjdec");
    config.format = YUV_FORMAT;
    config.swap_enable = true;
    config.resolution_x = X;
    config.resolution_y = Y;
    if (YUV_FORMAT == MJDEC_FORMAT_GRAY) {
        config.head_size = 595;
    } else {
        config.head_size = 605;
    }
    config.output_bufaddr0 = (uint32_t)(&yuv_buffer);
    config.output_bufaddr1 = (uint32_t)(&yuv_buffer[X * Y]);

    bflb_mjdec_init(mjdec, &config);
    bflb_mjdec_set_dqt_from_header(mjdec, jpeg_buffer);
    bflb_mjdec_tcint_mask(mjdec, false);
    bflb_irq_attach(mjdec->irq_num, mjdec_isr, NULL);
    bflb_irq_enable(mjdec->irq_num);
    bflb_mjdec_start(mjdec);
    bflb_mjdec_push_jpeg(mjdec, jpeg_buffer);

    while (pic_count != 1) {
        bflb_mtimer_delay_ms(200);
    }

    switch(YUV_FORMAT) {
        case MJDEC_FORMAT_YUV422SP_NV16:
            nv16_to_yuv422();
            printf("\r\nmode %s complete\r\n\r\n", "MJDEC_FORMAT_YUV422SP_NV16");
            printf("dump binary memory %s.bin 0x%08lX 0x%08lX\r\n", "YUV422SP_NV16", yuv_buffer, yuv_buffer + sizeof(yuv_buffer));
            break;
        case MJDEC_FORMAT_YUV422SP_NV61:
            nv61_to_yuv422();
            printf("\r\nmode %s complete\r\n\r\n", "MJDEC_FORMAT_YUV422SP_NV61");
            printf("dump binary memory %s.bin 0x%08lX 0x%08lX\r\n", "YUV422SP_NV61", yuv_buffer, yuv_buffer + sizeof(yuv_buffer));
            break;
        case MJDEC_FORMAT_YUV420SP_NV12:
            printf("\r\nmode %s complete\r\n\r\n", "MJDEC_FORMAT_YUV420SP_NV12");
            printf("dump binary memory %s.bin 0x%08lX 0x%08lX\r\n", "YUV420SP_NV12", yuv_buffer, yuv_buffer + sizeof(yuv_buffer) / 4 * 3);
            break;
        case MJDEC_FORMAT_YUV420SP_NV21:
            printf("\r\nmode %s complete\r\n\r\n", "MJDEC_FORMAT_YUV420SP_NV21");
            printf("dump binary memory %s.bin 0x%08lX 0x%08lX\r\n", "YUV420SP_NV21", yuv_buffer, yuv_buffer + sizeof(yuv_buffer) / 4 * 3);
            break;
        case MJDEC_FORMAT_GRAY:
            printf("\r\nmode %s complete\r\n\r\n", "MJDEC_FORMAT_GRAY");
            printf("dump binary memory %s.bin 0x%08lX 0x%08lX\r\n", "GRAY", yuv_buffer, yuv_buffer + sizeof(yuv_buffer) / 2);
            break;
    }

    return 0;
}