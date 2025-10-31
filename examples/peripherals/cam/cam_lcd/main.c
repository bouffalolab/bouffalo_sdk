#include "bflb_mtimer.h"
#include "bflb_i2c.h"
#include "bflb_cam.h"
#include "bflb_gpio.h"

#include "stdlib.h"

#include "board.h"
#include "image_sensor.h"
#include "lcd.h"

#define CAM_FRAME_COUNT_USE 50

#define src_b_l1(x)         (src[(x)])
#define src_b_l2(x)         (src[(640 * 2 + (x))])

static struct bflb_device_s *i2c0;
static struct bflb_device_s *cam0;
static struct bflb_device_s *gpio;

static short __s_r_1370705v[256] = { 0 };
static short __s_b_1732446u[256] = { 0 };
static short __s_g_337633u[256] = { 0 };
static short __s_g_698001v[256] = { 0 };

static ATTR_NOINIT_PSRAM_SECTION uint32_t display_buffer[480 * 320];

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

static ATTR_TCM_SECTION __attribute__((optimize("O3"))) void yuyv_640x480_to_nrgb_320x240(uint8_t *yuyv_src, uint8_t *nrgb_dest)
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

static ATTR_TCM_SECTION __attribute__((optimize("O3"))) void yuyv_640x480_to_rgb565_320x240(uint8_t *yuyv_src, uint8_t *nrgb_dest)
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

void cam_gpio_init(void)
{
    gpio = bflb_device_get_by_name("gpio");

    /* DVP0 GPIO init */
    /* I2C GPIO */
    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

    /* Reset GPIO */
    bflb_gpio_init(gpio, GPIO_PIN_3, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_reset(gpio, GPIO_PIN_3);
    bflb_mtimer_delay_ms(10);
    bflb_gpio_set(gpio, GPIO_PIN_3);
    bflb_mtimer_delay_ms(10);

    /* MCLK GPIO */
    bflb_gpio_init(gpio, GPIO_PIN_23, GPIO_FUNC_CLKOUT | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

    /* DVP0 GPIO */
    bflb_gpio_init(gpio, GPIO_PIN_24, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_25, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_26, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_27, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_28, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_29, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_30, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_31, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_32, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_33, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_34, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

int main(void)
{
    uint8_t *pic;
    struct bflb_cam_config_s cam_config;
    struct image_sensor_config_s *sensor_config;

    board_init();

    printf("cam_lcd_test\r\n");

    i2c0 = bflb_device_get_by_name("i2c0");
    cam0 = bflb_device_get_by_name("cam0");

    lcd_init();
    lcd_set_dir(3, 0);
    lcd_draw_area(0, 0, lcd_max_x, lcd_max_y, LCD_COLOR_RGB(0, 0, 0));
    printf("LCD init done, size: %d*%d\r\n", LCD_W, LCD_H);
    lcd_draw_str_ascii16(10, 10, LCD_COLOR_RGB(255, 0, 0), LCD_COLOR_RGB(0, 0, 0), (uint8_t *)"Cam test", 8);
    bflb_mtimer_delay_ms(500);

    initYUV2RGBTabel();
    cam_gpio_init();
    if (image_sensor_scan(i2c0, &sensor_config)) {
        printf("\r\nSensor name: %s, size: %d*%d\r\n", sensor_config->name, sensor_config->resolution_x, sensor_config->resolution_y);
    } else {
        printf("\r\nError! Can't identify sensor!\r\n");
        while (1) {
        }
    }

    memcpy(&cam_config, sensor_config, IMAGE_SENSOR_INFO_COPY_SIZE);
    cam_config.with_mjpeg = false;
    cam_config.input_source = CAM_INPUT_SOURCE_DVP;
    cam_config.output_format = CAM_OUTPUT_FORMAT_AUTO;
    cam_config.output_bufsize = cam_config.resolution_x * cam_config.resolution_y * 2 * 2;
    cam_config.output_bufaddr = (uint32_t)memalign(32, cam_config.output_bufsize);
    if (cam_config.output_bufaddr == 0) {
        printf("\r\nError! Can't allocate memory!\r\n");
        while (1) {
        }
    }

    bflb_cam_init(cam0, &cam_config);
#if defined(BL616D)
    bflb_cam_int_mask(cam0, CAM_INTMASK_NORMAL, false);
#endif
    bflb_cam_start(cam0);

    while (1) {
        while (bflb_cam_get_frame_count(cam0) == 0) {
        }
        bflb_cam_get_frame_info(cam0, &pic);

        if (LCD_COLOR_DEPTH == 32) {
            yuyv_640x480_to_nrgb_320x240(pic, (uint8_t *)display_buffer);
        } else {
            yuyv_640x480_to_rgb565_320x240(pic, (uint8_t *)display_buffer);
        }
        //yuyv_to_nrgb(pic, display_buffer);
        lcd_draw_picture_nonblocking(0, 0, lcd_max_x, lcd_max_y, (void *)display_buffer);
        while (lcd_draw_is_busy()) {
        }

#if defined(BL616D)
        bflb_cam_int_clear(cam0, CAM_INTCLR_NORMAL);
#else
        bflb_cam_pop_one_frame(cam0);
#endif
    }

    bflb_cam_stop(cam0);

    printf("end\r\n");

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
