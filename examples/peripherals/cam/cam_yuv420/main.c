#include "bflb_mtimer.h"
#include "bflb_i2c.h"
#include "bflb_cam.h"
#include "image_sensor.h"
#include "board.h"

#define CAM_FRAME_COUNT_USE 50

static struct bflb_device_s *i2c0;
static struct bflb_device_s *cam0_y;
static struct bflb_device_s *cam1_uv;

int main(void)
{
    uint32_t i, pic_size_y, pic_size_uv;
    uint8_t *pic_y, *pic_uv;
    struct bflb_cam_config_s cam_config;
    struct image_sensor_config_s *sensor_config;

    board_init();
    board_dvp_gpio_init();

    i2c0 = bflb_device_get_by_name("i2c0");
    cam0_y = bflb_device_get_by_name("cam0");
    cam1_uv = bflb_device_get_by_name("cam1");

    if (image_sensor_scan(i2c0, &sensor_config)) {
        printf("\r\nSensor name: %s\r\n", sensor_config->name);
    } else {
        printf("\r\nError! Can't identify sensor!\r\n");
        while (1) {
        }
    }

    /* Init cam0 for Y */
    memcpy(&cam_config, sensor_config, IMAGE_SENSOR_INFO_COPY_SIZE);
    cam_config.with_mjpeg = false;
    cam_config.input_source = CAM_INPUT_SOURCE_DVP;
    cam_config.output_format = CAM_OUTPUT_FORMAT_GRAY;
    cam_config.output_bufaddr = BFLB_PSRAM_BASE;
    cam_config.output_bufsize = cam_config.resolution_x * cam_config.resolution_y * 6;
    bflb_cam_init(cam0_y, &cam_config);

    /* Init cam1 for UV */
    cam_config.output_format = CAM_OUTPUT_FORMAT_YUV420_UV;
    cam_config.output_bufaddr += cam_config.output_bufsize;
    cam_config.output_bufsize /= 2;
    bflb_cam_init(cam1_uv, &cam_config);

    bflb_cam_start(cam0_y);
    bflb_cam_start(cam1_uv);

    for (i = 0; i < CAM_FRAME_COUNT_USE; i++) {
        while (bflb_cam_get_frame_count(cam0_y) == 0 || bflb_cam_get_frame_count(cam1_uv) == 0) {
        }
        pic_size_y = bflb_cam_get_frame_info(cam0_y, &pic_y);
        pic_size_uv = bflb_cam_get_frame_info(cam1_uv, &pic_uv);
        bflb_cam_pop_one_frame(cam0_y);
        bflb_cam_pop_one_frame(cam1_uv);
        printf("pop picture_y  %d: 0x%08x, len: %d\r\n", i, (uint32_t)pic_y, pic_size_y);
        printf("pop picture_uv %d: 0x%08x, len: %d\r\n", i, (uint32_t)pic_uv, pic_size_uv);
    }

    bflb_cam_stop(cam0_y);
    bflb_cam_stop(cam1_uv);

    /* Printf Y */
    //for (i = 0; i < pic_size_y; i++) {
    //    printf("%02x", pic_y[i]);
    //}

    /* Printf UV */
    //for (i = 0; i < pic_size_uv; i++) {
    //    printf("%02x", pic_uv[i]);
    //}
    //printf("\r\n");

    printf("end\r\n");

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
