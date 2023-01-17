#include "bflb_mtimer.h"
#include "bflb_i2c.h"
#include "bflb_cam.h"
#include "bflb_csi.h"
#include "image_sensor.h"
#include "board.h"

#define CAM_FRAME_COUNT_USE 50

static struct bflb_device_s *i2c0;
static struct bflb_device_s *cam0;
static struct bflb_device_s *csi;

static struct bflb_cam_config_s cam_config;
static struct bflb_csi_config_s csi_config;

int main(void)
{
    uint32_t i, pic_size;
    uint8_t *pic;
    struct image_sensor_config_s *sensor_config;
    
    board_init();
    board_csi_gpio_init();
    
    i2c0 = bflb_device_get_by_name("i2c0");
    cam0 = bflb_device_get_by_name("cam0");
    csi = bflb_device_get_by_name("csi");

    csi_config.lane_number = CSI_LANE_NUMBER_2;
    csi_config.tx_clk_escape = 24000000;
    csi_config.data_rate = 520000000;
    bflb_csi_init(csi, &csi_config);
    bflb_csi_start(csi);

    if(image_sensor_scan(i2c0, &sensor_config)){
        printf("\r\nSensor name: %s\r\n", sensor_config->name);
    }else{
        printf("\r\nError! Can't identify sensor!\r\n");
        while(1){}
    }

    bflb_csi_set_line_threshold(csi, sensor_config->resolution_x, sensor_config->pixel_clock, 80000000);

    memcpy(&cam_config, sensor_config, IMAGE_SENSOR_INFO_COPY_SIZE);
    cam_config.with_mjpeg = false;
    cam_config.input_source = CAM_INPUT_SOURCE_CSI;
    cam_config.output_format = CAM_OUTPUT_FORMAT_AUTO;
    cam_config.output_bufaddr = BFLB_PSRAM_BASE;
    cam_config.output_bufsize = cam_config.resolution_x * cam_config.resolution_y * 8;
    bflb_cam_init(cam0, &cam_config);
    bflb_cam_start(cam0);
    
    for (i = 0; i < CAM_FRAME_COUNT_USE; i++) {
        while(bflb_cam_get_frame_count(cam0) == 0){}
        pic_size = bflb_cam_get_frame_info(cam0, &pic);
        bflb_cam_pop_one_frame(cam0);
        printf("pop picture %d: 0x%08x, len: %d\r\n", i, (uint32_t)pic, pic_size);
    }
    
    bflb_cam_stop(cam0);
    
    //for (i = 0; i < pic_size; i++) {
    //    printf("%02x", pic[i]);
    //}
    //printf("\r\n");
    
    printf("end\r\n");
    
    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
