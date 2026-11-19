#include "bflb_mtimer.h"
#include "bflb_i2c.h"
#include "bflb_cam.h"
#include "bflb_gpio.h"
#include "bflb_l1c.h"
#if defined(BL618DG)
#include "hardware/cam_reg.h"
#endif

#include "stdlib.h"
#include "malloc.h"
#include <string.h>

#include "board.h"
#include "image_sensor.h"
#include "display.h"
#include "default/display_default.h"
#include "display_os.h"
#include "simple_font/display_simple_font.h"

#if !defined(CONFIG_DISPLAY_DEFAULT_TYPE_GRAM)
#error "cam_lcd requires a default GRAM display"
#endif
#if !defined(CONFIG_DISPLAY_DEFAULT_FORMAT_RGB565) && !defined(CONFIG_DISPLAY_DEFAULT_FORMAT_XRGB8888)
#error "cam_lcd supports RGB565 or XRGB8888 CPU output"
#endif

#define CAM_LCD_W           DISPLAY_DEFAULT_WIDTH
#define CAM_LCD_H           DISPLAY_DEFAULT_HEIGHT
#define CAM_LCD_COLOR_DEPTH (DISPLAY_DEFAULT_BPP * 8)
#define CAM_LCD_RGB565(r, g, b) ((((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3))

#define src_b_l1(x)           (src[(x)])
#define src_b_l2(x)           (src[(640 * 2 + (x))])
#define YUYV_RGB565_TILE_SIZE 16
#define FPS_REPORT_INTERVAL_US 1000000ULL
#define CAM_STOP_TIMEOUT_US     1000000ULL

static struct bflb_device_s *i2c;
static struct bflb_device_s *cam0;
static display_t *screen;
static display_info_t screen_info;
static uint16_t draw_rows;
static struct {
    const void *expected;
    volatile bool armed;
    volatile bool done;
    volatile int result;
} draw_completion;

static short __s_r_1370705v[256] = { 0 };
static short __s_b_1732446u[256] = { 0 };
static short __s_g_337633u[256] = { 0 };
static short __s_g_698001v[256] = { 0 };

static uint16_t __s_src_pair_offset[CAM_LCD_H];
static uint8_t __s_src_y_offset[CAM_LCD_H];
static uint32_t __s_src_line_offset[CAM_LCD_W];
static uint16_t __s_rgb565_tile[YUYV_RGB565_TILE_SIZE][YUYV_RGB565_TILE_SIZE];
static uint16_t __s_map_src_w;
static uint16_t __s_map_src_h;
static uint16_t __s_map_dest_w;
static uint16_t __s_map_dest_h;

static ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(BFLB_CACHE_LINE_SIZE)))
    uint8_t display_buffer[CAM_LCD_W * CAM_LCD_H * DISPLAY_DEFAULT_BPP];

static void display_event(display_t *display, const display_event_t *event, void *ctx)
{
    (void)ctx;
    if ((display == screen) && (event->type == DISPLAY_EVT_DRAW_DONE) && draw_completion.armed &&
        (event->buf == draw_completion.expected)) {
        draw_completion.result = event->result;
        draw_completion.armed = false;
        draw_completion.done = true;
    }
}

/* Only one request is outstanding. Arm before submission: CPU-only buses and fast DMA
 * may call back before display_draw_async() returns. Never release on a fixed delay. */
static int display_frame(void)
{
    const uint32_t row_bytes = CAM_LCD_W * DISPLAY_DEFAULT_BPP;

    for (uint16_t y = 0; y < CAM_LCD_H;) {
        uint16_t rows = CAM_LCD_H - y;
        uintptr_t flags;
        int ret;

        if (rows > draw_rows) {
            rows = draw_rows;
        }
        display_area_t area = { .x = 0, .y = y, .w = CAM_LCD_W, .h = rows };
        const void *pixels = display_buffer + y * row_bytes;

        flags = display_os_irq_save();
        draw_completion.expected = pixels;
        draw_completion.done = false;
        draw_completion.result = -EINPROGRESS;
        draw_completion.armed = true;
        display_os_irq_restore(flags);

        /* The default endpoint cleans the CPU-written pixels before DMA reads them. */
        ret = display_draw_async(screen, &area, pixels, rows * row_bytes);
        if (ret < 0) {
            flags = display_os_irq_save();
            draw_completion.armed = false;
            display_os_irq_restore(flags);
            return ret;
        }
        while (!draw_completion.done) {}
        if (draw_completion.result != 0) {
            return draw_completion.result;
        }
        y += rows;
    }
    return 0;
}

static int draw_startup_title(void)
{
    static const display_simple_font_cfg_t cfg = {
        .format = CAM_LCD_COLOR_DEPTH == 16 ? DISPLAY_FMT_RGB565 : DISPLAY_FMT_XRGB8888,
        .width = CAM_LCD_W,
        .height = CAM_LCD_H,
        .font = DISPLAY_SIMPLE_FONT_ASCII_16X8,
        .fg_color = 0xffff0000,
        .bg_color = 0xff000000,
    };

    memset(display_buffer, 0, sizeof(display_buffer));
    return display_simple_font_draw(&cfg, display_buffer, 10, 10, "Cam test");
}

static int init_display(void)
{
    const uint32_t row_bytes = CAM_LCD_W * DISPLAY_DEFAULT_BPP;
    uint32_t rows;
    int ret;

#if defined(CONFIG_DISPLAY_DEFAULT_MODULE_GC9307_240X320)
    /* Keep this module's X mirror (MADCTL MX), without changing the shared module. */
    display_default_get_module()->madctl[DISPLAY_ROTATION_0] = 0x40;
#endif
    screen = display_default_get();
    ret = display_init(screen);
    if (ret < 0) {
        return ret;
    }
    ret = display_get_info(screen, &screen_info);
    if (ret < 0) {
        return ret;
    }
    if ((screen_info.kind != DISPLAY_KIND_GRAM) || (screen_info.width != CAM_LCD_W) ||
        (screen_info.height != CAM_LCD_H) ||
        (screen_info.format != ((CAM_LCD_COLOR_DEPTH == 16) ? DISPLAY_FMT_RGB565 : DISPLAY_FMT_XRGB8888)) ||
        !screen_info.x_align || !screen_info.y_align || !screen_info.buf_align ||
        (CAM_LCD_W % screen_info.x_align) || (CAM_LCD_H % screen_info.y_align) ||
        ((uintptr_t)display_buffer % screen_info.buf_align)) {
        return -EINVAL;
    }
    rows = screen_info.max_draw_bytes / row_bytes;
    if (rows > CAM_LCD_H) {
        rows = CAM_LCD_H;
    }
    /* Every stripe starts on an aligned row/address; the last stripe may be shorter. */
    while (rows && ((rows % screen_info.y_align) || ((rows * row_bytes) % screen_info.buf_align))) {
        rows--;
    }
    if (!rows) {
        return -EINVAL;
    }
    draw_rows = rows;
    ret = display_set_callback(screen, display_event, NULL);
    if (ret < 0) {
        return ret;
    }
    ret = draw_startup_title();
    if (ret < 0) {
        return ret;
    }
    ret = display_frame();
    if (ret < 0) {
        return ret;
    }
    ret = display_set_backlight(screen, DISPLAY_BACKLIGHT_MAX);
    if ((ret < 0) && (ret != -ENOTSUP)) {
        return ret;
    }
    printf("LCD init done, size: %d*%d, depth: %d\r\n", CAM_LCD_W, CAM_LCD_H, CAM_LCD_COLOR_DEPTH);
    bflb_mtimer_delay_ms(500);
    return 0;
}

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

            dest[2] = /*R*/ BYTECLIP(out_y + __s_r_1370705v[src[3]]);
            dest[1] = /*G*/ BYTECLIP(out_y - __s_g_337633u[src[1]] - __s_g_698001v[src[3]]);
            dest[0] = /*B*/ BYTECLIP(out_y + __s_b_1732446u[src[1]]);
            dest[3] = 0;

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

            dest[2] = /*R*/ BYTECLIP(out_y + __s_r_1370705v[out_v]);
            dest[1] = /*G*/ BYTECLIP(out_y - __s_g_337633u[out_u] - __s_g_698001v[out_v]);
            dest[0] = /*B*/ BYTECLIP(out_y + __s_b_1732446u[out_u]);
            dest[3] = 0;

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

            /* XRGB8888 in little-endian memory is B, G, R, X. Keep the original sampling. */
            dest[2] = /*R*/ BYTECLIP(out_y + __s_r_1370705v[out_v]);
            dest[1] = /*G*/ BYTECLIP(out_y - __s_g_337633u[out_u] - __s_g_698001v[out_v]);
            dest[0] = /*B*/ BYTECLIP(out_y + __s_b_1732446u[out_u]);
            dest[3] = 0;

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

            *(uint16_t *)dest = CAM_LCD_RGB565(out_r, out_g, out_b);

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
            *(uint16_t *)dest = CAM_LCD_RGB565(out_r, out_g, out_b);
        }
    }
}

/* Keep the RGB565 240x320 path's coordinates, two-row averaging and U/V
 * interpretation unchanged; only the output pixel packing differs. */
static ATTR_TCM_SECTION __attribute__((optimize("O3"))) void
yuyv_640x480_to_xrgb8888_240x320(uint8_t *yuyv_src, uint8_t *xrgb_dest)
{
    for (uint16_t i = 0; i < 320; i++) {
        for (uint16_t j = 0; j < 240; j++) {
            uint16_t src_x = i * 2;
            uint16_t src_y = j * 2;
            uint32_t offset = src_y * 640 * 2 + (src_x / 2) * 4;
            const uint8_t *src_row1 = yuyv_src + offset;
            const uint8_t *src_row2 = src_row1 + 640 * 2;
            uint8_t *dest = xrgb_dest + (i * 240 + j) * 4;
            int16_t out_y = (src_row1[0] + src_row2[2]) / 2;
            int16_t out_u = (src_row1[3] + src_row2[3]) / 2;
            int16_t out_v = (src_row1[1] + src_row2[1]) / 2;

            dest[2] = BYTECLIP(out_y + __s_r_1370705v[out_v]);
            dest[1] = BYTECLIP(out_y - __s_g_337633u[out_u] - __s_g_698001v[out_v]);
            dest[0] = BYTECLIP(out_y + __s_b_1732446u[out_u]);
            dest[3] = 0;
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
        if (crop_w == 0) {
            crop_w = 1;
        }
        crop_h = src_h;
        crop_x = (src_w - crop_w) / 2;
        crop_y = 0;
    } else {
        crop_w = src_w;
        crop_h = (uint32_t)src_w * dest_w / dest_h;
        if (crop_h == 0) {
            crop_h = 1;
        }
        crop_x = 0;
        crop_y = (src_h - crop_h) / 2;
    }

    src_x = crop_x;
    src_x_rem = dest_h / 2;
    src_y_start = (uint32_t)(dest_w - 1) * crop_h + dest_w / 2;

    for (uint16_t i = 0; i < dest_h; i++) {
        /* Clamp only the stored sample; retain the original integer recurrence. */
        uint16_t map_x = (src_x < crop_x + crop_w) ? src_x : crop_x + crop_w - 1;

        src_x_pair = map_x & ~1U;
        __s_src_pair_offset[i] = src_x_pair * 2;
        __s_src_y_offset[i] = (map_x & 1U) ? 2 : 0;

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
        uint16_t map_y = (src_y < crop_y + crop_h) ? src_y : crop_y + crop_h - 1;

        __s_src_line_offset[i] = map_y * src_w * 2;

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

    if ((src_w == 0) || (src_w & 1U) || (src_h == 0) || (dest_w == 0) || (dest_h == 0) || (dest_w > CAM_LCD_W) ||
        (dest_h > CAM_LCD_H)) {
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

                    __s_rgb565_tile[i][j] = CAM_LCD_RGB565(out_r, out_g, out_b);
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

/* Same center crop, nearest-neighbor samples and clockwise rotation as the generic
 * RGB565 path. The fixed VGA -> 320x240/240x320 paths keep their two-row averaging. */
static ATTR_TCM_SECTION __attribute__((optimize("O3"))) void
yuyv_to_xrgb8888_crop_scale_rotate(uint8_t *yuyv_src, uint16_t src_w, uint16_t src_h, uint8_t *dest,
                                  uint16_t dest_w, uint16_t dest_h)
{
    if ((src_w == 0) || (src_w & 1U) || (src_h == 0) || (dest_w == 0) || (dest_h == 0) ||
        (dest_w > CAM_LCD_W) || (dest_h > CAM_LCD_H)) {
        return;
    }
    if ((__s_map_src_w != src_w) || (__s_map_src_h != src_h) || (__s_map_dest_w != dest_w) ||
        (__s_map_dest_h != dest_h)) {
        initYUYVScaleMap(src_w, src_h, dest_w, dest_h);
    }
    for (uint16_t y = 0; y < dest_h; y++) {
        for (uint16_t x = 0; x < dest_w; x++) {
            const uint8_t *src = yuyv_src + __s_src_line_offset[x] + __s_src_pair_offset[y];
            uint8_t *pixel = dest + ((uint32_t)y * dest_w + x) * 4;
            int16_t out_y = src[__s_src_y_offset[y]];
            int16_t out_u = src[1];
            int16_t out_v = src[3];

            pixel[0] = BYTECLIP(out_y + __s_b_1732446u[out_u]);
            pixel[1] = BYTECLIP(out_y - __s_g_337633u[out_u] - __s_g_698001v[out_v]);
            pixel[2] = BYTECLIP(out_y + __s_r_1370705v[out_v]);
            pixel[3] = 0;
        }
    }
}

#if defined(BL618DG)
static int stop_camera_frame(void)
{
    uint64_t start_us = bflb_mtimer_get_time_us();
    const uint32_t idle = CAM_ST_BUS_IDLE | CAM_AXI_IDLE;

    /* DG returns the fixed capture base, not a popped FIFO frame. Stop capture and
     * drain outstanding AXI writes before reading that memory on the CPU. */
    bflb_cam_stop(cam0);
    while ((bflb_cam_get_intstatus(cam0) & idle) != idle) {
        if ((bflb_mtimer_get_time_us() - start_us) >= CAM_STOP_TIMEOUT_US) {
            return -ETIMEDOUT;
        }
    }
    return 0;
}
#endif

static void release_camera_frame(void)
{
#if defined(BL618DG)
    bflb_cam_int_clear(cam0, CAM_INTCLR_NORMAL);
    bflb_cam_start(cam0);
#else
    bflb_cam_pop_one_frame(cam0);
#endif
}

int main(void)
{
    uint8_t *pic;
    int ret;
    uint32_t frame_bytes;
    uint64_t frame_bytes64;
    uint32_t displayed_frame_count = 0;
    uint32_t last_fps_frame_count = 0;
    uint64_t last_fps_time_us;
    struct bflb_cam_config_s cam_config;
    struct image_sensor_config_s *sensor_config;

    board_init();

    printf("cam_lcd_test\r\n");

    i2c = bflb_device_get_by_name(BOARD_DVP_USE_I2C);
    cam0 = bflb_device_get_by_name(BFLB_NAME_CAM0);

    if (!i2c || !cam0) {
        printf("Error! Camera/I2C device unavailable\r\n");
        return -ENODEV;
    }
    ret = init_display();
    if (ret < 0) {
        printf("Error! Display init failed: %d\r\n", ret);
        display_deinit(screen);
        return ret;
    }

    initYUV2RGBTabel();
    board_dvp_gpio_init();
    if (image_sensor_scan(i2c, &sensor_config)) {
        printf("\r\nSensor name: %s, size: %d*%d\r\n", sensor_config->name, sensor_config->resolution_x,
               sensor_config->resolution_y);
    } else {
        printf("\r\nError! Can't identify sensor!\r\n");
        while (1) {}
    }

    if (!sensor_config->resolution_x || (sensor_config->resolution_x & 1U) || !sensor_config->resolution_y ||
        ((sensor_config->output_format != IMAGE_SENSOR_FORMAT_YUV422_YUYV) &&
         (sensor_config->output_format != IMAGE_SENSOR_FORMAT_YUV422_YVYU))) {
        printf("Error! CPU conversion requires an even-width YUYV/YVYU sensor\r\n");
        display_deinit(screen);
        return -ENOTSUP;
    }
    frame_bytes64 = (uint64_t)sensor_config->resolution_x * sensor_config->resolution_y * 2U;
    if ((frame_bytes64 > UINT32_MAX / 2U) || (frame_bytes64 > SIZE_MAX / 2U)) {
        printf("Error! Camera double-frame buffer size overflows\r\n");
        display_deinit(screen);
        return -EINVAL;
    }
    /* CAM packs frames without padding. Require each frame boundary to be a
     * cache-line boundary so invalidation cannot discard adjacent heap metadata
     * or share a line with another frame. Do not round up the frame stride. */
    if (frame_bytes64 % BFLB_CACHE_LINE_SIZE) {
        printf("Error! Camera frame size must be a multiple of %u cache-line bytes\r\n",
               (unsigned int)BFLB_CACHE_LINE_SIZE);
        display_deinit(screen);
        return -ENOTSUP;
    }
    frame_bytes = (uint32_t)frame_bytes64;
    memcpy(&cam_config, sensor_config, IMAGE_SENSOR_INFO_COPY_SIZE);
    cam_config.with_mjpeg = false;
    cam_config.input_source = CAM_INPUT_SOURCE_DVP;
    cam_config.output_format = CAM_OUTPUT_FORMAT_AUTO;
    cam_config.output_bufsize = frame_bytes * 2U;
    cam_config.output_bufaddr = (uint32_t)memalign(BFLB_CACHE_LINE_SIZE, cam_config.output_bufsize);
    if (cam_config.output_bufaddr == 0) {
        printf("\r\nError! Can't allocate memory!\r\n");
        while (1) {}
    }

    /* Transfer the entire allocation to DMA before CAM initialization: dirty
     * cache lines left by the allocator must not later overwrite captured data. */
    bflb_l1c_dcache_clean_invalidate_range((void *)(uintptr_t)cam_config.output_bufaddr, cam_config.output_bufsize);
    bflb_cam_init(cam0, &cam_config);
#if defined(BL618DG)
    bflb_cam_int_mask(cam0, CAM_INTMASK_NORMAL, false);
#endif
    bflb_cam_start(cam0);
    last_fps_time_us = bflb_mtimer_get_time_us();

    while (1) {
        uint64_t now_us;

        while (bflb_cam_get_frame_count(cam0) == 0) {}
#if defined(BL618DG)
        ret = stop_camera_frame();
        if (ret < 0) {
            /* Do not touch or recycle a capture whose writes did not stop. */
            printf("Error! CAM stop failed: %d\r\n", ret);
            while (1) {}
        }
#endif
        if ((bflb_cam_get_frame_info(cam0, &pic) < frame_bytes) || !pic) {
            printf("Error! Incomplete camera frame\r\n");
            release_camera_frame();
            continue;
        }
        bflb_l1c_dcache_invalidate_range(pic, frame_bytes);

        if (CAM_LCD_COLOR_DEPTH == 32) {
            if ((sensor_config->resolution_x == 640) && (sensor_config->resolution_y == 480) &&
                (CAM_LCD_W == 320) && (CAM_LCD_H == 240)) {
                yuyv_640x480_to_nrgb_320x240(pic, (uint8_t *)display_buffer);
            } else if ((sensor_config->resolution_x == 640) && (sensor_config->resolution_y == 480) &&
                       (CAM_LCD_W == 240) && (CAM_LCD_H == 320)) {
                yuyv_640x480_to_xrgb8888_240x320(pic, (uint8_t *)display_buffer);
            } else {
                yuyv_to_xrgb8888_crop_scale_rotate(pic, sensor_config->resolution_x, sensor_config->resolution_y,
                                                  (uint8_t *)display_buffer, CAM_LCD_W, CAM_LCD_H);
            }
        } else {
            if ((CAM_LCD_W == 320) && (CAM_LCD_H == 240) &&
                (sensor_config->resolution_x == 640) && (sensor_config->resolution_y == 480)) {
                yuyv_640x480_to_rgb565_320x240(pic, (uint8_t *)display_buffer);
            } else if ((CAM_LCD_W == 240) && (CAM_LCD_H == 320) &&
                       (sensor_config->resolution_x == 640) && (sensor_config->resolution_y == 480)) {
                yuyv_640x480_to_rgb565_240x320(pic, (uint8_t *)display_buffer);
            } else if ((CAM_LCD_W == 320) && (CAM_LCD_H == 480)) {
                yuyv_to_rgb565_crop_scale_rotate(pic, sensor_config->resolution_x, sensor_config->resolution_y,
                                                 (uint8_t *)display_buffer, CAM_LCD_W, CAM_LCD_H);
            } else {
                yuyv_to_rgb565_crop_scale_rotate(pic, sensor_config->resolution_x, sensor_config->resolution_y,
                                                 (uint8_t *)display_buffer, CAM_LCD_W, CAM_LCD_H);
            }
        }
        //yuyv_to_nrgb(pic, display_buffer);
        ret = display_frame();
        /* Return the source only after every submitted stripe has completed. */
        if (ret < 0) {
            printf("Error! Display frame failed: %d\r\n", ret);
            break;
        }
        displayed_frame_count++;
        release_camera_frame();

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
    display_deinit(screen);

    printf("end\r\n");

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
