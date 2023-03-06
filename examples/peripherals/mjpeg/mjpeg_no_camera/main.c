#include "bflb_mtimer.h"
#include "bflb_mjpeg.h"
#include "board.h"
#include "jpeg_head.h"
#include "test_64x64.h"

static struct bflb_device_s *mjpeg;

#define X       64
#define Y       64

#define ROW_NUM (Y * MJPEG_MAX_FRAME_COUNT)

volatile uint32_t pic_count = 0;
volatile uint32_t pic_addr[MJPEG_MAX_FRAME_COUNT] = { 0 };
volatile uint32_t pic_len[MJPEG_MAX_FRAME_COUNT] = { 0 };

void mjpeg_isr(int irq, void *arg)
{
    uint8_t *pic;
    uint32_t jpeg_len;

    uint32_t intstatus = bflb_mjpeg_get_intstatus(mjpeg);
    if (intstatus & MJPEG_INTSTS_ONE_FRAME) {
        bflb_mjpeg_int_clear(mjpeg, MJPEG_INTCLR_ONE_FRAME);
        jpeg_len = bflb_mjpeg_get_frame_info(mjpeg, &pic);
        pic_addr[pic_count] = (uint32_t)pic;
        pic_len[pic_count] = jpeg_len;
        pic_count++;
        bflb_mjpeg_pop_one_frame(mjpeg);
    }
}

uint8_t jpg_head_buf[800] = { 0 };
uint32_t jpg_head_len;

uint8_t MJPEG_QUALITY = 50;

#define SIZE_BUFFER (4 * 1024 * 1024)

void bflb_mjpeg_dump_hex(uint8_t *data, uint32_t len)
{
    uint32_t i = 0;

    for (i = 0; i < len; i++) {
        if (i % 16 == 0) {
            printf("\r\n");
        }

        printf("%02x ", data[i]);
    }

    printf("\r\n");
}

int main(void)
{
    board_init();

    mjpeg = bflb_device_get_by_name("mjpeg");

    struct bflb_mjpeg_config_s config;

    config.format = MJPEG_FORMAT_YUV422_YUYV;
    config.quality = MJPEG_QUALITY;
    config.rows = ROW_NUM;
    config.resolution_x = X;
    config.resolution_y = Y;
    config.input_bufaddr0 = (uint32_t)test_64x64;
    config.input_bufaddr1 = 0;
    config.output_bufaddr = (uint32_t)BFLB_PSRAM_BASE + X * 2 * ROW_NUM;
    config.output_bufsize = SIZE_BUFFER - X * 2 * ROW_NUM;
    config.input_yy_table = NULL; /* use default table */
    config.input_uv_table = NULL; /* use default table */

    bflb_mjpeg_init(mjpeg, &config);

    jpg_head_len = JpegHeadCreate(YUV_MODE_422, MJPEG_QUALITY, X, Y, jpg_head_buf);
    bflb_mjpeg_fill_jpeg_header_tail(mjpeg, jpg_head_buf, jpg_head_len);

    bflb_mjpeg_tcint_mask(mjpeg, false);
    bflb_irq_attach(mjpeg->irq_num, mjpeg_isr, NULL);
    bflb_irq_enable(mjpeg->irq_num);

    bflb_mjpeg_sw_run(mjpeg, MJPEG_MAX_FRAME_COUNT);

    while (pic_count < MJPEG_MAX_FRAME_COUNT) {
        printf("pic count:%d\r\n", pic_count);
        bflb_mtimer_delay_ms(200);
    }

    for (uint8_t i = 0; i < MJPEG_MAX_FRAME_COUNT; i++) {
        printf("jpg addr:%08x ,jpg size:%d\r\n", pic_addr[i], pic_len[i]);
        bflb_mjpeg_dump_hex((uint8_t *)pic_addr[i], pic_len[i]);
    }

    while (1) {
        bflb_mtimer_delay_ms(2000);
    }
}