#include "bflb_mtimer.h"
#include "bflb_mjpeg.h"
#include "board.h"
#include "jpeg_head.h"
#include "test_64x64.h"

#define X 64
#define Y 64

uint8_t jpg_head_buf[800] = { 0 };
uint32_t jpg_head_len;

uint8_t MJPEG_QUALITY = 50;

static __attribute__((aligned(32))) ATTR_NOINIT_PSRAM_SECTION uint8_t mjpeg_buffer[50 * 1024];

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

static struct bflb_device_s *mjpeg;

volatile uint32_t pic_count = 0;
volatile uint8_t *pic_addr;
volatile uint32_t pic_len;

void mjpeg_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_mjpeg_get_intstatus(mjpeg);
    if (intstatus & MJPEG_INTSTS_ONE_FRAME) {
        bflb_mjpeg_int_clear(mjpeg, MJPEG_INTCLR_ONE_FRAME);
        pic_len = bflb_mjpeg_get_frame_info(mjpeg, &pic_addr);
        pic_count = 1;
        bflb_mjpeg_pop_one_frame(mjpeg);
    }
}

void mjpeg_init(uint32_t x, uint32_t y, uint8_t *yuv, uint8_t *jpeg, uint32_t size, uint8_t quality)
{
    struct bflb_mjpeg_config_s config;

    mjpeg = bflb_device_get_by_name("mjpeg");

    config.format = MJPEG_FORMAT_YUV422_YUYV;
    config.quality = quality;
    config.rows = y;
    config.resolution_x = x;
    config.resolution_y = y;
    config.input_bufaddr0 = (uint32_t)yuv;
    config.input_bufaddr1 = 0;
    config.output_bufaddr = (uint32_t)jpeg;
    config.output_bufsize = size;
    config.input_yy_table = NULL; /* use default table */
    config.input_uv_table = NULL; /* use default table */

    bflb_mjpeg_init(mjpeg, &config);
    jpg_head_len = JpegHeadCreate(YUV_MODE_422, quality, x, y, jpg_head_buf);
    bflb_mjpeg_fill_jpeg_header_tail(mjpeg, jpg_head_buf, jpg_head_len);
    bflb_mjpeg_tcint_mask(mjpeg, false);
    bflb_irq_attach(mjpeg->irq_num, mjpeg_isr, NULL);
    bflb_irq_enable(mjpeg->irq_num);
}

void mjpeg_compress(uint8_t *yuv, uint8_t *jpeg, uint32_t size)
{
    bflb_mjpeg_update_input_output_buff(mjpeg, yuv, NULL, jpeg, size);
    bflb_mjpeg_sw_run(mjpeg, 1);
}

int main(void)
{
    board_init();

    mjpeg_init(X, Y, NULL, NULL, 0, MJPEG_QUALITY);

    /* compress one pic */
    mjpeg_compress(test_64x64, mjpeg_buffer, 50 * 1024);

    while (pic_count != 1) {
        bflb_mtimer_delay_ms(200);
    }

    printf("jpg addr:%08x ,jpg size:%d\r\n", pic_addr, pic_len);
    bflb_mjpeg_dump_hex(pic_addr, pic_len);

    while (1) {
        bflb_mtimer_delay_ms(2000);
    }
}