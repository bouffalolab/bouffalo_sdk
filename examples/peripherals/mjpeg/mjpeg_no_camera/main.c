#include "bflb_mtimer.h"
#include "bflb_mjpeg.h"
#include "board.h"
#include "jpeg_head.h"
#include "test_64x64.h"

static struct bflb_device_s *mjpeg;

#define X 64
#define Y 64

volatile uint32_t pic_count = 0;
volatile uint32_t pic_addr[MJPEG_MAX_FRAME_COUNT] = { 0 };
volatile uint32_t pic_len[MJPEG_MAX_FRAME_COUNT] = { 0 };

uint8_t *pic;
volatile uint32_t jpeg_len;

void mjpeg_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_mjpeg_get_intstatus(mjpeg);
    if (intstatus & MJPEG_INTSTS_ONE_FRAME) {
        jpeg_len = bflb_mjpeg_get_frame_info(mjpeg, &pic);
        pic_addr[pic_count] = (uint32_t)pic;
        pic_len[pic_count] = jpeg_len;
        pic_count++;
        bflb_mjpeg_pop_one_frame(mjpeg);
        bflb_mjpeg_int_clear(mjpeg, MJPEG_INTCLR_ONE_FRAME);
    }
}

static uint16_t Q_Table_50_Y[64] = {
    16, 11, 10, 16, 24, 40, 51, 61,
    12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56,
    14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68, 109, 103, 77,
    24, 35, 55, 64, 81, 104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101,
    72, 92, 95, 98, 112, 100, 103, 99
};

static uint16_t Q_Table_50_UV[64] = {
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99
};

uint8_t jpgHeadBuf[800] = { 0 };
uint32_t jpgHeadLength;

uint8_t MJPEG_QUALITY = 50;

#define BUFFER_YUV  0xA8000000
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
    uint16_t tmpTableY[64] = { 0 };
    uint16_t tmpTableUV[64] = { 0 };

    board_init();

    mjpeg = bflb_device_get_by_name("mjpeg");

    struct bflb_mjpeg_config_s config;

    config.format = MJPEG_FORMAT_YUV422_YUYV;
    config.resolution_x = X;
    config.resolution_y = Y;
    config.input_bufaddr0 = (uint32_t)test_64x64;
    config.input_bufaddr1 = 0;
    config.output_bufaddr = (uint32_t)BUFFER_YUV + MJPEG_MAX_FRAME_COUNT * X * Y * 2;
    config.output_bufsize = SIZE_BUFFER - MJPEG_MAX_FRAME_COUNT * X * Y * 2;

    bflb_mjpeg_init(mjpeg, &config);

    bflb_mjpeg_calculate_quantize_table(MJPEG_QUALITY, Q_Table_50_Y, tmpTableY);
    bflb_mjpeg_calculate_quantize_table(MJPEG_QUALITY, Q_Table_50_UV, tmpTableUV);
    bflb_mjpeg_fill_quantize_table(mjpeg, tmpTableY, tmpTableUV);
    jpgHeadLength = JpegHeadCreate(YUV_MODE_422, MJPEG_QUALITY, X, Y, jpgHeadBuf);
    bflb_mjpeg_fill_jpeg_header_tail(mjpeg, jpgHeadBuf, jpgHeadLength);

    bflb_mjpeg_tcint_mask(mjpeg, false);
    bflb_irq_attach(mjpeg->irq_num, mjpeg_isr, NULL);
    bflb_irq_enable(mjpeg->irq_num);

    bflb_mjpeg_sw_run(mjpeg, MJPEG_MAX_FRAME_COUNT);

    while (pic_count < MJPEG_MAX_FRAME_COUNT) {
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