/**
 * @file lv_port_disp_default_gram.c
 * @brief LVGL v8 display port for the default display of the display framework when it is a screen with frame
 *        memory (SPI / DBI / QSPI), that is CONFIG_DISPLAY_DEFAULT_TYPE_GRAM.
 *        The video screens are in lv_port_disp_default_video.c.
 *
 * The screen is defined by Kconfig (bsp/common/display/default/Kconfig) and taken with display_default_get(). The
 * application calls lv_port_disp_init() and nothing else. To change the screen (for example the module of the
 * default display, see display_default_get_module()), do it before lv_port_disp_init(): the port calls
 * display_init(), which does nothing when the application has already initialized the display.
 *
 * - Partial rendering into CONFIG_LVGL8_DISP_DEFAULT_BUF_COUNT draw buffers of CONFIG_LVGL8_DISP_DEFAULT_BUF_SIZE
 *   bytes each; pixels are sent by display_draw_async() and the completion event releases LVGL.
 * - Pixel format: the one of the default display, RGB565 or XRGB8888, which must match LV_COLOR_DEPTH (16 or 32).
 *   LVGL v8 has no packed RGB888.
 * - CONFIG_LVGL8_DISP_DEFAULT_SW_ROTATE enables LVGL software rotation via lv_disp_set_rotation().
 *   The port does not rotate the panel in hardware.
 *
 * A project with several screens, or with special needs, writes its own port instead.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "lv_port_disp_default.h"

#if defined(CONFIG_DISPLAY_DEFAULT_TYPE_GRAM)

#include "bflb_core.h"
#include "default/display_default.h"
#include <stdbool.h>
#include <string.h>

#if defined(CONFIG_FREERTOS)
#include "FreeRTOS.h"
#include "semphr.h"
#endif

#define DBG_TAG "LV_DISP"
#include "log.h"

#define BYTE_PER_PIXEL DISPLAY_DEFAULT_BPP
#define BUF_COUNT      CONFIG_LVGL8_DISP_DEFAULT_BUF_COUNT
#define CACHE_LINE     64
#define ALIGN_UP(x)    (((x) + CACHE_LINE - 1) & ~(size_t)(CACHE_LINE - 1))
#define DRAW_BUFF_SIZE ALIGN_UP((size_t)CONFIG_LVGL8_DISP_DEFAULT_BUF_SIZE)

_Static_assert(sizeof(lv_color_t) == BYTE_PER_PIXEL,
               "lv_color_t does not match the format of the default display; check LV_COLOR_DEPTH");

/* Cached memory: the display framework cleans the data cache before the DMA reads it. With PSRAM the buffers
 * are placed there, which allows larger buffers. */
#ifdef CONFIG_PSRAM
#define DRAW_BUFF_ATTR __attribute__((aligned(CACHE_LINE))) ATTR_NOINIT_PSRAM_SECTION
#else
#define DRAW_BUFF_ATTR __attribute__((aligned(CACHE_LINE)))
#endif

static DRAW_BUFF_ATTR uint8_t draw_buf_1[DRAW_BUFF_SIZE];
#if BUF_COUNT > 1
static DRAW_BUFF_ATTR uint8_t draw_buf_2[DRAW_BUFF_SIZE];
#endif
static uint8_t x_align = 1;
static uint8_t y_align = 1;

static display_t *lcd;
static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;
static volatile bool flush_busy;

#if defined(CONFIG_FREERTOS)
static SemaphoreHandle_t flush_sem;
#endif

/* The display event callback: runs in the interrupt of the DMA completion */
static void disp_event_cb(display_t *display, const display_event_t *event, void *ctx)
{
    (void)display;
    (void)ctx;
    if (event->type != DISPLAY_EVT_DRAW_DONE) {
        return;
    }
    flush_busy = false;
#if defined(CONFIG_FREERTOS)
    BaseType_t woken = pdFALSE;

    xSemaphoreGiveFromISR(flush_sem, &woken);
    portYIELD_FROM_ISR(woken);
#endif
}

/* Flush the rendered area: asynchronous, lv_disp_flush_ready() follows in disp_wait_cb() */
static void disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p)
{
    display_area_t a = {
        .x = (uint16_t)area->x1,
        .y = (uint16_t)area->y1,
        .w = (uint16_t)lv_area_get_width(area),
        .h = (uint16_t)lv_area_get_height(area),
    };
    size_t len = (size_t)a.w * a.h * BYTE_PER_PIXEL;
    int ret;

    flush_busy = true;
    ret = display_draw_async(lcd, &a, color_p, len);
    if (ret) {
        LOG_E("display_draw_async failed: %d\r\n", ret);
        flush_busy = false;
        lv_disp_flush_ready(drv);
    }
}

/* LVGL calls this in a loop until the flush is ready: wait for the end of the transfer */
static void disp_wait_cb(lv_disp_drv_t *drv)
{
#if defined(CONFIG_FREERTOS)
    if (flush_busy) {
        xSemaphoreTake(flush_sem, portMAX_DELAY);
    }
    xSemaphoreTake(flush_sem, 0); /* drop a token left by a flush that finished before we looked */
#else
    while (flush_busy) {}
#endif
    lv_disp_flush_ready(drv);
}

/* Round the area to the panel's pixel alignment. */
static void disp_rounder_cb(lv_disp_drv_t *drv, lv_area_t *a)
{
    uint8_t ax = x_align;
    uint8_t ay = y_align;

    if (drv->sw_rotate && (drv->rotated == LV_DISP_ROT_90 || drv->rotated == LV_DISP_ROT_270)) {
        ax = y_align;
        ay = x_align;
    }
    if (ax > 1) {
        a->x1 = a->x1 & ~(lv_coord_t)(ax - 1);
        a->x2 = a->x2 | (lv_coord_t)(ax - 1);
    }
    if (ay > 1) {
        a->y1 = a->y1 & ~(lv_coord_t)(ay - 1);
        a->y2 = a->y2 | (lv_coord_t)(ay - 1);
    }
}

/* Fill the screen with black before the backlight goes on, using a draw buffer as the source */
static int clear_screen(const display_info_t *info)
{
    uint16_t lines = (uint16_t)(DRAW_BUFF_SIZE / ((size_t)info->width * BYTE_PER_PIXEL));
    int ret = 0;

    if (lines > info->height) {
        lines = info->height;
    }
    memset(draw_buf_1, 0, (size_t)info->width * lines * BYTE_PER_PIXEL);
    for (uint16_t y = 0; y < info->height && !ret; y = (uint16_t)(y + lines)) {
        uint16_t n = (uint16_t)((info->height - y) < lines ? (info->height - y) : lines);
        display_area_t area = { .x = 0, .y = y, .w = info->width, .h = n };

        ret = display_draw(lcd, &area, draw_buf_1, (size_t)info->width * n * BYTE_PER_PIXEL);
    }
    return ret;
}

lv_disp_drv_t *lv_port_disp_init(void)
{
    display_info_t info;
    unsigned pixel_size;
    int ret;

    lcd = display_default_get();
    ret = display_init(lcd);
    ret = ret ? ret : display_get_info(lcd, &info);
    if (ret) {
        LOG_E("default display bring-up failed: %d\r\n", ret);
        return NULL;
    }

    /* Match LVGL's pixel type to the default display. */
    if (info.format == DISPLAY_FMT_RGB565) {
        pixel_size = 2;
    } else if (info.format == DISPLAY_FMT_XRGB8888) {
        pixel_size = 4;
    } else {
        LOG_E("unsupported display format %d (LVGL v8 has RGB565 and XRGB8888)\r\n", info.format);
        return NULL;
    }
    if (pixel_size != sizeof(lv_color_t)) {
        LOG_E("lv_color_t is %u bytes but the display uses %u bytes per pixel\r\n",
              (unsigned)sizeof(lv_color_t), pixel_size);
        return NULL;
    }
    if ((size_t)info.width * BYTE_PER_PIXEL > DRAW_BUFF_SIZE) {
        LOG_E("draw buffer too small for %u pixels per line (CONFIG_LVGL8_DISP_DEFAULT_BUF_SIZE)\r\n", info.width);
        return NULL;
    }
    if (!info.max_draw_bytes || DRAW_BUFF_SIZE > info.max_draw_bytes) {
        LOG_E("draw buffer %u exceeds display single-transfer limit %lu\r\n", (unsigned)DRAW_BUFF_SIZE,
              (unsigned long)info.max_draw_bytes);
        return NULL;
    }
    if (info.buf_align > CACHE_LINE) {
        LOG_E("draw buffer alignment %u not supported\r\n", info.buf_align);
        return NULL;
    }
    x_align = info.x_align ? info.x_align : 1;
    y_align = info.y_align ? info.y_align : 1;

#if defined(CONFIG_FREERTOS)
    flush_sem = xSemaphoreCreateBinary();
#endif
    ret = display_set_callback(lcd, disp_event_cb, NULL);
    ret = ret ? ret : clear_screen(&info); /* first frame, then the backlight */
    if (ret) {
        LOG_E("default display start failed: %d\r\n", ret);
        return NULL;
    }
    display_set_backlight(lcd, DISPLAY_BACKLIGHT_MAX); /* -ENOTSUP without a backlight is fine */

#if BUF_COUNT > 1
    lv_disp_draw_buf_init(&draw_buf, draw_buf_1, draw_buf_2, DRAW_BUFF_SIZE / BYTE_PER_PIXEL);
#else
    lv_disp_draw_buf_init(&draw_buf, draw_buf_1, NULL, DRAW_BUFF_SIZE / BYTE_PER_PIXEL);
#endif
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = (lv_coord_t)info.width;
    disp_drv.ver_res = (lv_coord_t)info.height;
    disp_drv.flush_cb = disp_flush;
    disp_drv.wait_cb = disp_wait_cb;
    disp_drv.rounder_cb = disp_rounder_cb;
    disp_drv.draw_buf = &draw_buf;
#if defined(CONFIG_LVGL8_DISP_DEFAULT_SW_ROTATE)
    disp_drv.sw_rotate = 1;
#endif
    disp_drv.rotated = LV_DISP_ROT_NONE; /* initial angle; lv_disp_set_rotation() may change it */
    lv_disp_drv_register(&disp_drv);
    LOG_I("default display %ux%u, %d bit, %d draw buffer(s) of %u bytes\r\n", info.width, info.height,
          BYTE_PER_PIXEL * 8, BUF_COUNT, (unsigned)DRAW_BUFF_SIZE);
    return &disp_drv;
}

#else /* port of the other kind of default display, or no default display */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;

#endif
