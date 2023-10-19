#include "bflb_mtimer.h"
#include "bflb_i2c.h"
#include "bflb_cam.h"
#include "bflb_mjpeg.h"
#include "image_sensor.h"
#include "board.h"
#include "jpeg_head.h"

#define BLOCK_NUM           2
#define ROW_NUM             (8 * BLOCK_NUM)
#define CAM_FRAME_COUNT_USE 5

static struct bflb_device_s *i2c0;
static struct bflb_device_s *cam0;

static struct bflb_device_s *mjpeg;

volatile uint32_t pic_count = 0;
volatile uint32_t pic_addr[CAM_FRAME_COUNT_USE] = { 0 };
volatile uint32_t pic_len[CAM_FRAME_COUNT_USE] = { 0 };

static __attribute__((aligned(32))) ATTR_NOINIT_PSRAM_SECTION uint8_t dvp_buffer[640 * 2 * ROW_NUM];
static __attribute__((aligned(32))) ATTR_NOINIT_PSRAM_SECTION uint8_t mjpeg_buffer[50 * 1024 * CAM_FRAME_COUNT_USE];

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
        if (pic_count == CAM_FRAME_COUNT_USE) {
            bflb_cam_stop(cam0);
            bflb_mjpeg_stop(mjpeg);
        }
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
    struct bflb_cam_config_s cam_config;
    struct image_sensor_config_s *sensor_config;

    board_init();
    board_dvp_gpio_init();

    i2c0 = bflb_device_get_by_name("i2c0");
    cam0 = bflb_device_get_by_name("cam0");

    if (image_sensor_scan(i2c0, &sensor_config)) {
        printf("\r\nSensor name: %s\r\n", sensor_config->name);
    } else {
        printf("\r\nError! Can't identify sensor!\r\n");
        while (1) {
        }
    }

    memcpy(&cam_config, sensor_config, IMAGE_SENSOR_INFO_COPY_SIZE);
    cam_config.with_mjpeg = true;
    cam_config.input_source = CAM_INPUT_SOURCE_DVP;
    cam_config.output_format = CAM_OUTPUT_FORMAT_AUTO;
    cam_config.output_bufaddr = (uint32_t)dvp_buffer;
    cam_config.output_bufsize = cam_config.resolution_x * 2 * ROW_NUM;

    bflb_cam_init(cam0, &cam_config);
    bflb_cam_start(cam0);

    mjpeg = bflb_device_get_by_name("mjpeg");

    struct bflb_mjpeg_config_s config;

    config.format = MJPEG_FORMAT_YUV422_YUYV;
    config.quality = MJPEG_QUALITY;
    config.rows = ROW_NUM;
    config.resolution_x = cam_config.resolution_x;
    config.resolution_y = cam_config.resolution_y;
    config.input_bufaddr0 = (uint32_t)dvp_buffer;
    config.input_bufaddr1 = 0;
    config.output_bufaddr = (uint32_t)mjpeg_buffer;
    config.output_bufsize = sizeof(mjpeg_buffer);
    config.input_yy_table = NULL; /* use default table */
    config.input_uv_table = NULL; /* use default table */

    bflb_mjpeg_init(mjpeg, &config);

    jpg_head_len = JpegHeadCreate(YUV_MODE_422, MJPEG_QUALITY, cam_config.resolution_x, cam_config.resolution_y, jpg_head_buf);
    bflb_mjpeg_fill_jpeg_header_tail(mjpeg, jpg_head_buf, jpg_head_len);

    bflb_mjpeg_tcint_mask(mjpeg, false);
    bflb_irq_attach(mjpeg->irq_num, mjpeg_isr, NULL);
    bflb_irq_enable(mjpeg->irq_num);

    bflb_mjpeg_start(mjpeg);

    while (pic_count < CAM_FRAME_COUNT_USE) {
        printf("pic count:%d\r\n", pic_count);
        bflb_mtimer_delay_ms(200);
    }

    for (uint8_t i = 0; i < CAM_FRAME_COUNT_USE; i++) {
        if ((pic_addr[i] + pic_len[i]) > ((uint32_t)mjpeg_buffer + sizeof(mjpeg_buffer))) {
            printf("drop invalid pic\r\n");
            continue;
        }
        printf("jpg addr:%08x ,jpg size:%d\r\n", pic_addr[i], pic_len[i]);
        //bflb_mjpeg_dump_hex((uint8_t *)pic_addr[i], pic_len[i]);
    }

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
