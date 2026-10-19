#include "bflb_mtimer.h"
#include "bflb_i2c.h"
#include "bflb_cam.h"
#include "bflb_gpio.h"

#include "stdlib.h"
#include "malloc.h"

#include "board.h"
#include "image_sensor.h"
#include "lcd.h"

#define src_b_l1(x)           (src[(x)])
#define src_b_l2(x)           (src[(640 * 2 + (x))])
#define YUYV_RGB565_TILE_SIZE 16
#define FPS_REPORT_INTERVAL_US 1000000ULL

static struct bflb_device_s *i2c;
static struct bflb_device_s *cam0;

static short __s_r_1370705v[256] = { 0 };
static short __s_b_1732446u[256] = { 0 };
static short __s_g_337633u[256] = { 0 };
static short __s_g_698001v[256] = { 0 };

static uint16_t __s_src_pair_offset[LCD_H];
static uint8_t __s_src_y_offset[LCD_H];
static uint32_t __s_src_line_offset[LCD_W];
static uint16_t __s_rgb565_tile[YUYV_RGB565_TILE_SIZE][YUYV_RGB565_TILE_SIZE];
static uint16_t __s_map_src_w;
static uint16_t __s_map_src_h;
static uint16_t __s_map_dest_w;
static uint16_t __s_map_dest_h;

static ATTR_NOINIT_PSRAM_SECTION uint32_t display_buffer[LCD_W * LCD_H];

static void initYUV2RGBTabel()
{
    for (int i = 0; i < 256; i++) {
        __s_r_1370705v[i] = (1.370705 * (i - 128));
        __s_b_1732446u[i] = (1.732446 * (i - 128));
        __s_g_337633u[i] = (0.337633 * (i - 128));
        __s_g_698001v[i] = (0.698001 * (i - 128));
    }
}

static inline uint8_t BYTECLIP(int val)
{
    if (unlikely(val < 0)) {
        return 0;
    } else if (unlikely(val > 255)) {
        return 255;
    } else {
        return (uint8_t)val;
    }
}

#if 0
static ATTR_TCM_SECTION __attribute__((optimize("O3"))) void yuyv_to_nrgb(void *yuyv_src, void *nrgb_dest)
{
    uint8_t *src = yuyv_src;
    uint8_t *dest  = nrgb_dest;

    int16_t out_y;

    for (uint16_t i = 0; i < 320; i++) {
        for (uint16_t j = 0; j < 480; j++) {
            out_y = src[2 * (j % 2)];

            dest[0] = /*R*/ BYTECLIP(out_y + __s_r_1370705v[src[3]]);
            dest[1] = /*G*/ BYTECLIP(out_y - __s_g_337633u[src[1]] - __s_g_698001v[src[3]]);
            dest[2] = /*B*/ BYTECLIP(out_y + __s_b_1732446u[src[1]]);

            src += 4 * (j % 2);
            dest += 4;
        }
    }
}

static ATTR_TCM_SECTION __attribute__((optimize("O3"))) void yuyv_640x480_to_nrgb_480x320(uint8_t *yuyv_src, uint8_t *nrgb_dest)
{
    uint8_t *src;
    uint8_t *dest;

    int16_t out_y, out_u, out_v;

    for (uint16_t i = 0; i < 320; i++) {
        src = yuyv_src;
        dest = nrgb_dest;

        for (uint16_t j = 0; j < 480; j++) {
            /* output pixel yuv */
            if((i % 2 != 1) && (j % 3 != 2)){
                out_y = src[2 * (j % 2)];
                out_u = src[1];
                out_v = src[3];
            }else if((i % 2 != 1) && (j % 3 == 2)){
                out_y = (src[0] + src[2]) / 2;
                out_u = src[1];
                out_v = src[3];
            }else if((i % 2 == 1) && (j % 3 != 2)){
                out_y = (src_b_l1(2 * (j % 2)) + src_b_l2(2 * (j % 2))) / 2;
                out_u = (src_b_l1(1) + src_b_l2(1)) / 2;
                out_v = (src_b_l1(3) + src_b_l2(3)) / 2;
            }else{
                out_y = (src_b_l1(0) + src_b_l2(2)) / 2;;
                out_u = (src_b_l1(1) + src_b_l2(1)) / 2;
                out_v = (src_b_l1(3) + src_b_l2(3)) / 2;
            }

            dest[0] = /*R*/ BYTECLIP(out_y + __s_r_1370705v[out_v]);
            dest[1] = /*G*/ BYTECLIP(out_y - __s_g_337633u[out_u] - __s_g_698001v[out_v]);
            dest[2] = /*B*/ BYTECLIP(out_y + __s_b_1732446u[out_u]);

            src += 4 * (j % 2);
            dest += 4;
        }

        yuyv_src += 640 * 2 + 640 * 2 * (i % 2);
        nrgb_dest += 480 * 4;
    }
}
#endif

static ATTR_TCM_SECTION __attribute__((optimize("O3"))) void yuyv_640x480_to_nrgb_320x240(uint8_t *yuyv_src,
                                                                                          uint8_t *nrgb_dest)
{
    uint8_t *src;
    uint8_t *dest;

    int16_t out_y, out_u, out_v;

    for (uint16_t i = 0; i < 240; i++) {
        src = yuyv_src;
        dest = nrgb_dest;

        for (uint16_t j = 0; j < 320; j++) {
            /* output pixel yuv */
            out_y = (src_b_l1(0) + src_b_l2(2)) / 2;
            out_u = (src_b_l1(3) + src_b_l2(3)) / 2;
            out_v = (src_b_l1(1) + src_b_l2(1)) / 2;

            dest[0] = /*R*/ BYTECLIP(out_y + __s_r_1370705v[out_v]);
            dest[1] = /*G*/ BYTECLIP(out_y - __s_g_337633u[out_u] - __s_g_698001v[out_v]);
            dest[2] = /*B*/ BYTECLIP(out_y + __s_b_1732446u[out_u]);

            src += 4;
            dest += 4;
        }

        yuyv_src += 640 * 2 * 2;
        nrgb_dest += 320 * 4;
    }
}

static ATTR_TCM_SECTION __attribute__((optimize("O3"))) void yuyv_640x480_to_rgb565_320x240(uint8_t *yuyv_src,
                                                                                            uint8_t *nrgb_dest)
{
    uint8_t *src;
    uint8_t *dest;

    int16_t out_y, out_u, out_v;
    uint8_t out_r, out_g, out_b;

    for (uint16_t i = 0; i < 240; i++) {
        src = yuyv_src;
        dest = nrgb_dest;

        for (uint16_t j = 0; j < 320; j++) {
            /* output pixel yuv */
            out_y = (src_b_l1(0) + src_b_l2(2)) / 2;
            out_u = (src_b_l1(3) + src_b_l2(3)) / 2;
            out_v = (src_b_l1(1) + src_b_l2(1)) / 2;

            out_r = /*R*/ BYTECLIP(out_y + __s_r_1370705v[out_v]);
            out_g = /*G*/ BYTECLIP(out_y - __s_g_337633u[out_u] - __s_g_698001v[out_v]);
            out_b = /*B*/ BYTECLIP(out_y + __s_b_1732446u[out_u]);

            *(uint16_t *)dest = LCD_COLOR_RGB565(out_r, out_g, out_b);

            src += 4;
            dest += 2;
        }

        yuyv_src += 640 * 2 * 2;
        nrgb_dest += 320 * 2;
    }
}

static ATTR_TCM_SECTION __attribute__((optimize("O3"))) void yuyv_640x480_to_rgb565_240x320(uint8_t *yuyv_src,
                                                                                            uint8_t *nrgb_dest)
{
    uint8_t *src_row1;
    uint8_t *src_row2;
    uint8_t *dest;

    int16_t out_y, out_u, out_v;
    uint8_t out_r, out_g, out_b;

    for (uint16_t i = 0; i < 320; i++) {
        for (uint16_t j = 0; j < 240; j++) {
            uint16_t src_x = i * 2;
            uint16_t src_y = j * 2;

            uint32_t offset = src_y * 640 * 2 + (src_x / 2) * 4;

            src_row1 = yuyv_src + offset;
            src_row2 = src_row1 + 640 * 2;

            out_y = (src_row1[0] + src_row2[2]) / 2;
            out_u = (src_row1[3] + src_row2[3]) / 2;
            out_v = (src_row1[1] + src_row2[1]) / 2;

            out_r = BYTECLIP(out_y + __s_r_1370705v[out_v]);
            out_g = BYTECLIP(out_y - __s_g_337633u[out_u] - __s_g_698001v[out_v]);
            out_b = BYTECLIP(out_y + __s_b_1732446u[out_u]);

            dest = nrgb_dest + (i * 240 + j) * 2;
            *(uint16_t *)dest = LCD_COLOR_RGB565(out_r, out_g, out_b);
        }
    }
}

static void initYUYVScaleMap(uint16_t src_w, uint16_t src_h, uint16_t dest_w, uint16_t dest_h)
{
    uint16_t crop_x, crop_y, crop_w, crop_h;
    uint16_t src_x, src_y, src_x_pair;
    uint32_t src_x_rem, src_y_start;
    int32_t src_y_rem;

    if ((uint32_t)src_w * dest_w > (uint32_t)src_h * dest_h) {
        crop_w = (uint32_t)src_h * dest_h / dest_w;
        crop_h = src_h;
        crop_x = (src_w - crop_w) / 2;
        crop_y = 0;
    } else {
        crop_w = src_w;
        crop_h = (uint32_t)src_w * dest_w / dest_h;
        crop_x = 0;
        crop_y = (src_h - crop_h) / 2;
    }

    src_x = crop_x;
    src_x_rem = dest_h / 2;
    src_y_start = (uint32_t)(dest_w - 1) * crop_h + dest_w / 2;

    for (uint16_t i = 0; i < dest_h; i++) {
        src_x_pair = src_x & ~1U;
        __s_src_pair_offset[i] = src_x_pair * 2;
        __s_src_y_offset[i] = (src_x & 1U) ? 2 : 0;

        if (i < dest_h - 1) {
            src_x_rem += crop_w;
            while (src_x_rem >= dest_h) {
                src_x_rem -= dest_h;
                src_x++;
            }
        }
    }

    src_y = crop_y + src_y_start / dest_w;
    src_y_rem = src_y_start % dest_w;

    for (uint16_t i = 0; i < dest_w; i++) {
        __s_src_line_offset[i] = src_y * src_w * 2;

        if (i < dest_w - 1) {
            src_y_rem -= crop_h;
            while (src_y_rem < 0) {
                src_y_rem += dest_w;
                src_y--;
            }
        }
    }

    __s_map_src_w = src_w;
    __s_map_src_h = src_h;
    __s_map_dest_w = dest_w;
    __s_map_dest_h = dest_h;
}

static ATTR_TCM_SECTION __attribute__((optimize("O3"))) void
yuyv_to_rgb565_crop_scale_rotate(uint8_t *yuyv_src, uint16_t src_w, uint16_t src_h, uint8_t *rgb565_dest,
                                 uint16_t dest_w, uint16_t dest_h)
{
    uint8_t *src_line;
    uint8_t *src;
    uint16_t *dest;

    uint16_t tile_w, tile_h;
    uint16_t src_pair_offset;
    uint8_t src_y_offset;
    int16_t out_y, out_u, out_v;
    uint8_t out_r, out_g, out_b;

    if ((src_w == 0) || (src_w & 1U) || (src_h == 0) || (dest_w == 0) || (dest_h == 0) || (dest_w > LCD_W) ||
        (dest_h > LCD_H)) {
        return;
    }

    if ((__s_map_src_w != src_w) || (__s_map_src_h != src_h) || (__s_map_dest_w != dest_w) ||
        (__s_map_dest_h != dest_h)) {
        initYUYVScaleMap(src_w, src_h, dest_w, dest_h);
    }

    /* The output image is rotated clockwise. */
    for (uint16_t y = 0; y < dest_h; y += YUYV_RGB565_TILE_SIZE) {
        tile_h = (dest_h - y > YUYV_RGB565_TILE_SIZE) ? YUYV_RGB565_TILE_SIZE : dest_h - y;

        for (uint16_t x = 0; x < dest_w; x += YUYV_RGB565_TILE_SIZE) {
            tile_w = (dest_w - x > YUYV_RGB565_TILE_SIZE) ? YUYV_RGB565_TILE_SIZE : dest_w - x;

            for (uint16_t j = 0; j < tile_w; j++) {
                src_line = yuyv_src + __s_src_line_offset[x + j];

                for (uint16_t i = 0; i < tile_h; i++) {
                    src_pair_offset = __s_src_pair_offset[y + i];
                    src_y_offset = __s_src_y_offset[y + i];
                    src = src_line + src_pair_offset;

                    out_y = src[src_y_offset];
                    out_u = src[1];
                    out_v = src[3];

                    out_r = /*R*/ BYTECLIP(out_y + __s_r_1370705v[out_v]);
                    out_g = /*G*/ BYTECLIP(out_y - __s_g_337633u[out_u] - __s_g_698001v[out_v]);
                    out_b = /*B*/ BYTECLIP(out_y + __s_b_1732446u[out_u]);

                    __s_rgb565_tile[i][j] = LCD_COLOR_RGB565(out_r, out_g, out_b);
                }
            }

            for (uint16_t i = 0; i < tile_h; i++) {
                dest = (uint16_t *)rgb565_dest + (y + i) * dest_w + x;

                for (uint16_t j = 0; j < tile_w; j++) {
                    dest[j] = __s_rgb565_tile[i][j];
                }
            }
        }
    }
}

int main(void)
{
    uint8_t *pic;
    uint32_t displayed_frame_count = 0;
    uint32_t last_fps_frame_count = 0;
    uint64_t last_fps_time_us;
    struct bflb_cam_config_s cam_config;
    struct image_sensor_config_s *sensor_config;

    board_init();

    printf("cam_lcd_test\r\n");

    i2c = bflb_device_get_by_name(BOARD_DVP_USE_I2C);
    cam0 = bflb_device_get_by_name(BFLB_NAME_CAM0);

#ifdef LCD_DBI_ST77926
    board_lcd_dbi_type_b_gpio_init();
#endif
    lcd_init();
    // lcd_set_dir(3, 0);
    lcd_draw_area(0, 0, lcd_max_x, lcd_max_y, LCD_COLOR_RGB(0, 0, 0));
    printf("LCD init done, size: %d*%d\r\n", LCD_W, LCD_H);
    lcd_draw_str_ascii16(10, 10, LCD_COLOR_RGB(255, 0, 0), LCD_COLOR_RGB(0, 0, 0), (uint8_t *)"Cam test", 8);
    bflb_mtimer_delay_ms(500);

    initYUV2RGBTabel();
    board_dvp_gpio_init();
    if (image_sensor_scan(i2c, &sensor_config)) {
        printf("\r\nSensor name: %s, size: %d*%d\r\n", sensor_config->name, sensor_config->resolution_x,
               sensor_config->resolution_y);
    } else {
        printf("\r\nError! Can't identify sensor!\r\n");
        while (1) {}
    }

    memcpy(&cam_config, sensor_config, IMAGE_SENSOR_INFO_COPY_SIZE);
    cam_config.with_mjpeg = false;
    cam_config.input_source = CAM_INPUT_SOURCE_DVP;
    cam_config.output_format = CAM_OUTPUT_FORMAT_AUTO;
    cam_config.output_bufsize = cam_config.resolution_x * cam_config.resolution_y * 2 * 2;
    cam_config.output_bufaddr = (uint32_t)memalign(32, cam_config.output_bufsize);
    if (cam_config.output_bufaddr == 0) {
        printf("\r\nError! Can't allocate memory!\r\n");
        while (1) {}
    }

    bflb_cam_init(cam0, &cam_config);
#if defined(BL618DG)
    bflb_cam_int_mask(cam0, CAM_INTMASK_NORMAL, false);
#endif
    bflb_cam_start(cam0);
    last_fps_time_us = bflb_mtimer_get_time_us();

    while (1) {
        uint64_t now_us;

        while (bflb_cam_get_frame_count(cam0) == 0) {}
        bflb_cam_get_frame_info(cam0, &pic);

        if (LCD_COLOR_DEPTH == 32) {
            yuyv_640x480_to_nrgb_320x240(pic, (uint8_t *)display_buffer);
        } else {
            if ((LCD_W == 320) && (LCD_H == 240)) {
                yuyv_640x480_to_rgb565_320x240(pic, (uint8_t *)display_buffer);
            } else if ((LCD_W == 240) && (LCD_H == 320)) {
                yuyv_640x480_to_rgb565_240x320(pic, (uint8_t *)display_buffer);
            } else if ((LCD_W == 320) && (LCD_H == 480)) {
                yuyv_to_rgb565_crop_scale_rotate(pic, sensor_config->resolution_x, sensor_config->resolution_y,
                                                 (uint8_t *)display_buffer, LCD_W, LCD_H);
            } else {
                yuyv_to_rgb565_crop_scale_rotate(pic, sensor_config->resolution_x, sensor_config->resolution_y,
                                                 (uint8_t *)display_buffer, LCD_W, LCD_H);
            }
        }
        //yuyv_to_nrgb(pic, display_buffer);
        lcd_draw_picture_nonblocking(0, 0, lcd_max_x, lcd_max_y, (void *)display_buffer);
        while (lcd_draw_is_busy()) {}
        displayed_frame_count++;

#if defined(BL618DG)
        bflb_cam_int_clear(cam0, CAM_INTCLR_NORMAL);
#else
        bflb_cam_pop_one_frame(cam0);
#endif

        now_us = bflb_mtimer_get_time_us();
        if ((now_us - last_fps_time_us) >= FPS_REPORT_INTERVAL_US) {
            uint64_t elapsed_us = now_us - last_fps_time_us;
            uint32_t frame_delta = displayed_frame_count - last_fps_frame_count;
            uint32_t fps_x1000 = (uint32_t)((frame_delta * 1000000000ULL + elapsed_us / 2) / elapsed_us);

            printf("camera display fps: %lu.%03lu\r\n",
                   (unsigned long)(fps_x1000 / 1000), (unsigned long)(fps_x1000 % 1000));
            last_fps_frame_count = displayed_frame_count;
            last_fps_time_us = now_us;
        }
    }

    bflb_cam_stop(cam0);

    printf("end\r\n");

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
