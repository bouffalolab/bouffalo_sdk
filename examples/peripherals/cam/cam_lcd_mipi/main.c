#include "lcd_conf.h"
#include "bflb_cam.h"
#include "bflb_dma2d.h"
#include "bflb_i2c.h"
#include "bflb_irq.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"
#include "bl618dg_clock.h"
#include "board.h"
#include "image_sensor.h"
#include "lcd.h"
#include <stdbool.h>
#include <string.h>

#define CAMERA_FRAME_MAX_WIDTH  1280U
#define CAMERA_FRAME_MAX_HEIGHT 720U
#define CAMERA_FRAME_MAX_PIXELS (CAMERA_FRAME_MAX_WIDTH * CAMERA_FRAME_MAX_HEIGHT)
#define CAMERA_FRAME_COUNT      2
#define DISPLAY_FRAME_COUNT     3
#define DISPLAY_QUEUE_SIZE      (DISPLAY_FRAME_COUNT - 1)
#define RGB565_BYTES_PER_PIXEL  2
#define FPS_REPORT_INTERVAL_US  1000000ULL

#if !defined(BL618DG)
#error "cam_lcd_mipi requires BL618DG"
#endif

#if (LCD_INTERFACE_TYPE != LCD_INTERFACE_DSI)
#error "cam_lcd_mipi requires a MIPI DSI panel"
#endif

#if (LCD_COLOR_DEPTH != 16)
#error "cam_lcd_mipi requires an RGB565 panel"
#endif

#if !defined(LCD_DSI_CAMERA_RGB565_FRAMEBUFFER_MODE)
#error "Selected panel does not support cam_lcd_mipi RGB565 framebuffer mode"
#endif

static struct bflb_device_s *i2c;
static struct bflb_device_s *cam0;
static struct bflb_device_s *dma2d_ch0;

struct frame_geometry_s {
    uint16_t sensor_width;
    uint16_t sensor_height;
    uint16_t crop_x;
    uint16_t crop_y;
    uint16_t crop_width;
    uint16_t crop_height;
    uint16_t display_x;
    uint16_t display_y;
    uint8_t transform;
    bool direct;
};

static struct frame_geometry_s frame_geometry;

static volatile uint8_t camera_ready_index;
static volatile uint8_t camera_write_index;
static volatile uint8_t camera_frame_ready;
static volatile uint8_t camera_paused;
static volatile uint8_t dma2d_busy;
static volatile uint8_t dma2d_done;
static volatile uint8_t active_display_framebuffer;
static volatile uint8_t display_framebuffer_queue[DISPLAY_QUEUE_SIZE];
static volatile uint8_t display_queue_head;
static volatile uint8_t display_queue_tail;
static volatile uint8_t display_queue_count;
static volatile uint32_t displayed_frame_count;

ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(
    BFLB_CACHE_LINE_SIZE))) static uint16_t camera_framebuffer[CAMERA_FRAME_COUNT][CAMERA_FRAME_MAX_PIXELS];
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(
    BFLB_CACHE_LINE_SIZE))) static lcd_color_t display_framebuffer[DISPLAY_FRAME_COUNT][LCD_W * LCD_H];

static int display_framebuffer_acquire(void);
static bool display_framebuffer_enqueue(uint8_t display_index);
static void camera_resume_if_paused(void);

static void fail_stop(const char *message)
{
    printf("%s\r\n", message);
    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}

static void camera_devices_open(const char *i2c_name)
{
    i2c = bflb_device_get_by_name(i2c_name);
    cam0 = bflb_device_get_by_name(BFLB_NAME_CAM0);
    if (i2c == NULL || cam0 == NULL) {
        fail_stop("Error: get DVP device failed");
    }
}

static struct image_sensor_config_s *camera_sensor_detect(void)
{
    struct image_sensor_config_s *sensor_config;

    if (!image_sensor_scan(i2c, &sensor_config)) {
        fail_stop("Error: can't identify DVP image sensor");
    }

    printf("Sensor: %s, %ux%u, format %u\r\n", sensor_config->name, sensor_config->resolution_x,
           sensor_config->resolution_y, sensor_config->output_format);
    return sensor_config;
}

static void camera_config_from_sensor(struct bflb_cam_config_s *cam_config,
                                      const struct image_sensor_config_s *sensor_config, uint8_t output_format,
                                      uint32_t output_bufaddr, uint32_t output_bufsize)
{
    memcpy(cam_config, sensor_config, IMAGE_SENSOR_INFO_COPY_SIZE);
    cam_config->with_mjpeg = false;
    cam_config->input_source = CAM_INPUT_SOURCE_DVP;
    cam_config->output_format = output_format;
    cam_config->output_bufaddr = output_bufaddr;
    cam_config->output_bufsize = output_bufsize;
}

static uint32_t frame_overlap_area(uint16_t width, uint16_t height)
{
    uint16_t display_width = width < LCD_W ? width : LCD_W;
    uint16_t display_height = height < LCD_H ? height : LCD_H;

    return display_width * display_height;
}

static void frame_geometry_init(uint16_t sensor_width, uint16_t sensor_height)
{
    uint32_t unrotated_area = frame_overlap_area(sensor_width, sensor_height);
    uint32_t rotated_area = frame_overlap_area(sensor_height, sensor_width);
    bool rotate = rotated_area == unrotated_area
                      ? (sensor_width > sensor_height) != (LCD_W > LCD_H)
                      : rotated_area > unrotated_area;
    uint16_t crop_width = sensor_width < (rotate ? LCD_H : LCD_W) ? sensor_width : (rotate ? LCD_H : LCD_W);
    uint16_t crop_height = sensor_height < (rotate ? LCD_W : LCD_H) ? sensor_height : (rotate ? LCD_W : LCD_H);
    uint16_t output_width = rotate ? crop_height : crop_width;
    uint16_t output_height = rotate ? crop_width : crop_height;

    frame_geometry = (struct frame_geometry_s){
        .sensor_width = sensor_width,
        .sensor_height = sensor_height,
        .crop_x = (sensor_width - crop_width) / 2,
        .crop_y = (sensor_height - crop_height) / 2,
        .crop_width = crop_width,
        .crop_height = crop_height,
        .display_x = (LCD_W - output_width) / 2,
        .display_y = (LCD_H - output_height) / 2,
        .transform = rotate ? DMA2D_IMAGE_ROTATE_DEGREE_90 : DMA2D_IMAGE_TRANSLATE,
        .direct = !rotate && sensor_width == LCD_W && sensor_height == LCD_H,
    };

    printf("Frame geometry: crop %ux%u at (%u,%u), %s at (%u,%u)\r\n", frame_geometry.crop_width,
           frame_geometry.crop_height, frame_geometry.crop_x, frame_geometry.crop_y,
           frame_geometry.direct ? "direct" : (rotate ? "rotate 90" : "copy"), frame_geometry.display_x,
           frame_geometry.display_y);
}

static void camera_int_cb(int irq, void *arg)
{
    int next_display_index;

    (void)irq;
    (void)arg;

    if (frame_geometry.direct) {
        if (!display_framebuffer_enqueue(camera_write_index)) {
            bflb_cam_stop(cam0);
            camera_paused = 1;
        } else {
            next_display_index = display_framebuffer_acquire();
            if (next_display_index < 0) {
                bflb_cam_stop(cam0);
                camera_paused = 1;
            } else {
                camera_write_index = next_display_index;
                bflb_cam_feature_control(cam0, CAM_CMD_SET_OUTPUT_ADDR,
                                         (size_t)(uintptr_t)display_framebuffer[camera_write_index]);
            }
        }
        bflb_cam_int_clear(cam0, CAM_INTCLR_NORMAL);
        return;
    }

    camera_ready_index = camera_write_index;
    camera_frame_ready = 1;

    if (dma2d_busy) {
        bflb_cam_stop(cam0);
        camera_paused = 1;
    } else {
        camera_write_index ^= 1;
        bflb_cam_feature_control(cam0, CAM_CMD_SET_OUTPUT_ADDR,
                                 (size_t)(uintptr_t)camera_framebuffer[camera_write_index]);
    }

    bflb_cam_int_clear(cam0, CAM_INTCLR_NORMAL);
}

static void dma2d_int_cb(void *arg)
{
    (void)arg;
    dma2d_done = 1;
}

static void display_frame_commit(void)
{
    uint8_t display_index;

    if (display_queue_count == 0) {
        return;
    }

    display_index = display_framebuffer_queue[display_queue_head];
    if (lcd_screen_switch(display_framebuffer[display_index]) != 0) {
        return;
    }

    display_queue_head = (display_queue_head + 1) % DISPLAY_QUEUE_SIZE;
    display_queue_count--;
    active_display_framebuffer = display_index;
    displayed_frame_count++;
    if (frame_geometry.direct) {
        camera_resume_if_paused();
    }
}

static void display_framebuffers_init(void)
{
    memset(display_framebuffer, 0, sizeof(display_framebuffer));
    bflb_l1c_dcache_clean_range(display_framebuffer, sizeof(display_framebuffer));

    active_display_framebuffer = 0;
    display_queue_head = 0;
    display_queue_tail = 0;
    display_queue_count = 0;
}

static int display_framebuffer_acquire(void)
{
    uintptr_t irq_flags;
    uint8_t display_index;

    irq_flags = bflb_irq_save();
    for (display_index = 0; display_index < DISPLAY_FRAME_COUNT; display_index++) {
        uint8_t queue_index;
        bool queued = false;

        if (display_index == active_display_framebuffer) {
            continue;
        }

        for (queue_index = 0; queue_index < display_queue_count; queue_index++) {
            uint8_t index = (display_queue_head + queue_index) % DISPLAY_QUEUE_SIZE;

            if (display_framebuffer_queue[index] == display_index) {
                queued = true;
                break;
            }
        }

        if (!queued) {
            bflb_irq_restore(irq_flags);
            return display_index;
        }
    }
    bflb_irq_restore(irq_flags);

    return -1;
}

static bool display_framebuffer_enqueue(uint8_t display_index)
{
    uintptr_t irq_flags = bflb_irq_save();

    if (display_queue_count == DISPLAY_QUEUE_SIZE) {
        bflb_irq_restore(irq_flags);
        return false;
    }

    display_framebuffer_queue[display_queue_tail] = display_index;
    display_queue_tail = (display_queue_tail + 1) % DISPLAY_QUEUE_SIZE;
    display_queue_count++;
    bflb_irq_restore(irq_flags);
    return true;
}

static void camera_resume_if_paused(void)
{
    int next_display_index;

    if (camera_paused) {
        if (frame_geometry.direct) {
            next_display_index = display_framebuffer_acquire();
            if (next_display_index < 0) {
                return;
            }
            camera_write_index = next_display_index;
            bflb_cam_feature_control(cam0, CAM_CMD_SET_OUTPUT_ADDR,
                                     (size_t)(uintptr_t)display_framebuffer[camera_write_index]);
        } else {
            camera_write_index ^= 1;
            bflb_cam_feature_control(cam0, CAM_CMD_SET_OUTPUT_ADDR,
                                     (size_t)(uintptr_t)camera_framebuffer[camera_write_index]);
        }
        camera_paused = 0;
        bflb_cam_start(cam0);
    }
}

static uint8_t dma2d_burst_size(uint16_t width)
{
    uint32_t row_bytes = width * RGB565_BYTES_PER_PIXEL;

    if (row_bytes % 16 == 0) {
        return DMA2D_BURST_INCR16;
    }
    if (row_bytes % 4 == 0) {
        return DMA2D_BURST_INCR4;
    }
    return DMA2D_BURST_INCR1;
}

static void dma2d_blit_to_display(const uint16_t *src, lcd_color_t *dst)
{
    uint8_t burst = dma2d_burst_size(frame_geometry.crop_width);
    struct bflb_dma2d_channel_config_s dma2d_config = {
        .src_addr = 0,
        .dst_addr = 0,
        .next_lli_addr = 0,
        .control = {
            .bits = {
                .transfer_size = 0,
                .src_burst = burst,
                .dst_burst = burst,
                .src_data_width = DMA2D_DATA_WIDTH_16BIT,
                .dst_data_width = DMA2D_DATA_WIDTH_16BIT,
                .src_incr = 0,
                .dst_incr = 0,
                .int_enable = 1,
            },
        },
    };
    struct bflb_dma2d_image_s image_config = {
        .transfer_data_width = DMA2D_DATA_WIDTH_16BIT,
        .pixel_data_width = RGB565_BYTES_PER_PIXEL,
        .src_image_addr = (uint32_t)(uintptr_t)src,
        .src_image_width = frame_geometry.sensor_width,
        .src_x_start = frame_geometry.crop_x,
        .src_y_start = frame_geometry.crop_y,
        .src_x_end = frame_geometry.crop_x + frame_geometry.crop_width,
        .src_y_end = frame_geometry.crop_y + frame_geometry.crop_height,
        .dst_image_addr = (uint32_t)(uintptr_t)dst,
        .dst_image_width = LCD_W,
        .dst_x_start = frame_geometry.display_x,
        .dst_y_start = frame_geometry.display_y,
    };

    dma2d_busy = 1;
    camera_resume_if_paused();
    dma2d_done = 0;

    bflb_dma2d_image_geometric_transfor_calculate(dma2d_ch0, &dma2d_config, &image_config,
                                                  frame_geometry.transform);
    bflb_dma2d_channel_init(dma2d_ch0, &dma2d_config);
    bflb_dma2d_channel_start(dma2d_ch0);

    while (!dma2d_done) {
        bflb_mtimer_delay_us(1);
    }

    bflb_dma2d_channel_stop(dma2d_ch0);
    dma2d_busy = 0;
}

static void dvp_camera_init(void)
{
    struct bflb_cam_config_s cam_config;
    struct image_sensor_config_s *sensor_config;

    board_dvp_gpio_init();
    GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_BCLK, 3);

    /* board_dvp_gpio_init() configures the selected board DVP bus on I2C0. */
    camera_devices_open(BFLB_NAME_I2C0);
    sensor_config = camera_sensor_detect();

    if ((uint32_t)sensor_config->resolution_x * sensor_config->resolution_y > CAMERA_FRAME_MAX_PIXELS) {
        fail_stop("Error: camera frame exceeds 1280x720 RGB565 buffer capacity");
    }
    if (sensor_config->output_format != IMAGE_SENSOR_FORMAT_YUV422_YUYV) {
        fail_stop("Error: this case requires YUYV camera output");
    }

    frame_geometry_init(sensor_config->resolution_x, sensor_config->resolution_y);
    camera_write_index = frame_geometry.direct ? display_framebuffer_acquire() : 0;
    if (camera_write_index >= (frame_geometry.direct ? DISPLAY_FRAME_COUNT : CAMERA_FRAME_COUNT)) {
        fail_stop("Error: no initial camera framebuffer available");
    }

    camera_config_from_sensor(&cam_config, sensor_config, CAM_OUTPUT_FORMAT_RGB888_TO_RGB565,
                              (uint32_t)(uintptr_t)(frame_geometry.direct ? display_framebuffer[camera_write_index]
                                                                           : camera_framebuffer[camera_write_index]),
                              sensor_config->resolution_x * sensor_config->resolution_y * RGB565_BYTES_PER_PIXEL);

    bflb_cam_init(cam0, &cam_config);
    bflb_cam_int_mask(cam0, CAM_INTMASK_NORMAL, false);
    bflb_irq_attach(cam0->irq_num, camera_int_cb, NULL);
    bflb_irq_enable(cam0->irq_num);
}

int main(void)
{
    uint32_t last_fps_frame_count = 0;
    uint64_t last_fps_time_us;

    board_init();
    display_framebuffers_init();
    if (lcd_init(display_framebuffer[0]) != 0) {
        fail_stop("Error: LCD init failed");
    }
    if (lcd_frame_callback_register(FRAME_INT_TYPE_CYCLE, display_frame_commit) != 0) {
        fail_stop("Error: LCD frame callback registration failed");
    }

    dma2d_ch0 = bflb_device_get_by_name(BFLB_NAME_DMA2D_CH0);
    if (dma2d_ch0 == NULL) {
        fail_stop("Error: get DMA2D channel failed");
    }
    bflb_dma2d_channel_irq_attach(dma2d_ch0, dma2d_int_cb, NULL);

    dvp_camera_init();
    bflb_cam_start(cam0);
    last_fps_time_us = bflb_mtimer_get_time_us();

    while (1) {
        uint8_t camera_index;
        int display_index;
        uint64_t now_us;

        if (camera_frame_ready) {
            display_index = display_framebuffer_acquire();
            if (display_index >= 0) {
                camera_frame_ready = 0;
                camera_index = camera_ready_index;
                dma2d_blit_to_display(camera_framebuffer[camera_index], display_framebuffer[display_index]);

                while (!display_framebuffer_enqueue(display_index)) {
                    bflb_mtimer_delay_us(1);
                }
            }
        }

        now_us = bflb_mtimer_get_time_us();
        if ((now_us - last_fps_time_us) >= FPS_REPORT_INTERVAL_US) {
            uint64_t elapsed_us = now_us - last_fps_time_us;
            uint32_t frame_count = displayed_frame_count;
            uint32_t frame_delta = frame_count - last_fps_frame_count;
            uint32_t fps_x1000 = (uint32_t)((frame_delta * 1000000000ULL + elapsed_us / 2) / elapsed_us);

            printf("camera display fps: %lu.%03lu\r\n",
                   (unsigned long)(fps_x1000 / 1000), (unsigned long)(fps_x1000 % 1000));
            last_fps_frame_count = frame_count;
            last_fps_time_us = now_us;
        }
    }
}
