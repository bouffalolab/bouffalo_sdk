#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "bflb_mjpeg.h"
#include "../jpeg_head.h"
#include "board.h"

#define X             (1920)
#define Y             (1080)
#define KICK_CNT      (1)
#define MJPEG_QUALITY (60)

struct bflb_mjpeg_config_s config;
uint8_t jpg_head_buf[800] = { 0 };
uint32_t jpg_head_len;
struct bflb_device_s *mjpeg;
volatile uint32_t pic_count = 0;
uint8_t *pic_addr;
uint32_t pic_len;
extern struct bflb_device_s *console;

__attribute__((aligned(32))) ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t yuv_buffer[2][X * 2 * 8 * KICK_CNT];
__attribute__((aligned(32))) ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t mjpeg_buffer[250 * 1024];

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

void mjpeg_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_mjpeg_get_intstatus(mjpeg);
    if (intstatus & MJPEG_INTSTS_ONE_FRAME) {
        bflb_mjpeg_int_clear(mjpeg, MJPEG_INTCLR_ONE_FRAME);
        pic_len = bflb_mjpeg_get_frame_info(mjpeg, &pic_addr);
        pic_count = 1;
        printf("pic_count:%d, jpg addr:%08x ,jpg size:%d\r\n", pic_count, pic_addr, pic_len);
        bflb_mjpeg_pop_one_frame(mjpeg);
    }
    if (intstatus & MJPEG_INTSTS_KICK_DONE) {
        bflb_mjpeg_int_clear(mjpeg, MJPEG_INTCLR_KICK_DONE);
        printf("kick\r\n");
    }
}

int main(void)
{
    board_init();

    mjpeg = bflb_device_get_by_name("mjpeg");
    config.format = MJPEG_FORMAT_YUV422_YUYV;
    config.quality = MJPEG_QUALITY;
    config.rows = Y;
    config.resolution_x = X;
    config.resolution_y = Y;
    config.input_bufaddr0 = 0;
    config.input_bufaddr1 = 0;
    config.output_bufaddr = (uint32_t)mjpeg_buffer;
    config.output_bufsize = sizeof(mjpeg_buffer);
    config.input_yy_table = NULL; /* use default table */
    config.input_uv_table = NULL; /* use default table */

    bflb_mjpeg_init(mjpeg, &config);
    jpg_head_len = JpegHeadCreate(YUV_MODE_422, MJPEG_QUALITY, X, Y, jpg_head_buf);
    bflb_mjpeg_fill_jpeg_header_tail(mjpeg, jpg_head_buf, jpg_head_len);
    bflb_mjpeg_kick_run(mjpeg, KICK_CNT);
    bflb_mjpeg_tcint_mask(mjpeg, false);
    // bflb_mjpeg_kickint_mask(mjpeg, false);
    bflb_irq_attach(mjpeg->irq_num, mjpeg_isr, NULL);
    bflb_irq_enable(mjpeg->irq_num);

    while (1) {
        static uint32_t idx = 0;
        static int flag = 0;
        int ch = bflb_uart_getchar(console);
        if (ch >= 0) {
            ((uint8_t *)yuv_buffer)[idx % sizeof(yuv_buffer)] = (uint8_t)ch;
            idx++;
            if (idx % sizeof(yuv_buffer[0]) == 0) {
                bflb_mjpeg_feature_control(mjpeg, MJPEG_CMD_SET_INPUTADDR0, (size_t)yuv_buffer + flag * (sizeof(yuv_buffer[0])));
                bflb_mjpeg_feature_control(mjpeg, MJPEG_CMD_UPDATE_KICK_ADDR, NULL);
                bflb_mjpeg_kick(mjpeg);
                flag ^= 1;
            }
            if (idx >= X * Y * 2) {
                printf("receive %d bytes\r\n", idx);
                break;
            }
        }
    }

    while (pic_count != 1) {

    }
    bflb_mjpeg_dump_hex(pic_addr, pic_len);
    printf("\r\nkick mode %s complete\r\n", "MJPEG_FORMAT_YUV422_YUYV");

    return 0;
}