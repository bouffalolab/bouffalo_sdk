/**
 * @file lv_port_disp_default_gram.c
 * @brief LVGL v9 display port for the default display of the display framework when it is a screen with frame
 *        memory (SPI / DBI / QSPI), that is CONFIG_DISPLAY_DEFAULT_TYPE_GRAM.
 *        The video screens are in lv_port_disp_default_video.c.
 *
 * The screen is defined by Kconfig (bsp/common/display/default/Kconfig) and taken with display_default_get(). The
 * application calls lv_port_disp_init() and nothing else. To change the screen (for example the module of the
 * default display, see display_default_get_module()), do it before lv_port_disp_init(): the port calls
 * display_init(), which does nothing when the application has already initialized the display.
 *
 * - Partial rendering into CONFIG_LVGL9_DISP_DEFAULT_BUF_COUNT draw buffers of CONFIG_LVGL9_DISP_DEFAULT_BUF_SIZE
 *   bytes each; pixels are sent by display_draw_async() and the completion event releases LVGL.
 * - LV_EVENT_RESOLUTION_CHANGED rotates the panel (MADCTL).
 * - Pixel format: the one of the default display (RGB565, RGB888 or XRGB8888), independent of LV_COLOR_DEPTH.
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
#define BUF_COUNT      CONFIG_LVGL9_DISP_DEFAULT_BUF_COUNT
#define CACHE_LINE     64
#define ALIGN_UP(x)    (((x) + CACHE_LINE - 1) & ~(size_t)(CACHE_LINE - 1))

#define DRAW_BUFF_SIZE ALIGN_UP((size_t)CONFIG_LVGL9_DISP_DEFAULT_BUF_SIZE)

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
static lv_display_t *lv_disp;
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

/* Flush the rendered area: asynchronous, lv_display_flush_ready() follows in disp_flush_wait_cb() */
static void disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
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
    ret = display_draw_async(lcd, &a, px_map, len);
    if (ret) {
        LOG_E("display_draw_async failed: %d\r\n", ret);
        flush_busy = false;
        lv_display_flush_ready(disp);
    }
}

/* LVGL needs the flush to be finished before it reuses the buffer */
static void disp_flush_wait_cb(lv_display_t *disp)
{
#if defined(CONFIG_FREERTOS)
    if (flush_busy) {
        xSemaphoreTake(flush_sem, portMAX_DELAY);
    }
    xSemaphoreTake(flush_sem, 0); /* drop a token left by a flush that finished before we looked */
#else
    while (flush_busy) {}
#endif
    lv_display_flush_ready(disp);
}

/* LV_EVENT_RESOLUTION_CHANGED: LVGL rotated the display, rotate the panel as well */
static void disp_resolution_cb(lv_event_t *e)
{
    lv_display_t *disp = (lv_display_t *)lv_event_get_target(e);
    display_info_t info;

    while (flush_busy) {}
    if (display_set_rotation(lcd, (display_rotation_t)lv_display_get_rotation(disp))) {
        LOG_E("display_set_rotation failed\r\n");
        return;
    }
    display_get_info(lcd, &info);
    x_align = info.x_align ? info.x_align : 1;
    y_align = info.y_align ? info.y_align : 1;
}

/* LV_EVENT_INVALIDATE_AREA: round the area to what the panel can draw */
static void disp_rounder_cb(lv_event_t *e)
{
    lv_area_t *a = lv_event_get_param(e);

    if (x_align > 1) {
        a->x1 = a->x1 & ~(int32_t)(x_align - 1);
        a->x2 = a->x2 | (int32_t)(x_align - 1);
    }
    if (y_align > 1) {
        a->y1 = a->y1 & ~(int32_t)(y_align - 1);
        a->y2 = a->y2 | (int32_t)(y_align - 1);
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

lv_display_t *lv_port_disp_init(void)
{
    display_info_t info;
    lv_color_format_t format;
    int ret;

    lcd = display_default_get();
    ret = display_init(lcd);
    ret = ret ? ret : display_get_info(lcd, &info);
    if (ret) {
        LOG_E("default display bring-up failed: %d\r\n", ret);
        return NULL;
    }

    /* The LVGL pixel format follows the display (the Kconfig format of the default display) */
    if (info.format == DISPLAY_FMT_RGB565) {
        format = LV_COLOR_FORMAT_RGB565;
    } else if (info.format == DISPLAY_FMT_RGB888) {
        format = LV_COLOR_FORMAT_RGB888;
    } else if (info.format == DISPLAY_FMT_XRGB8888) {
        format = LV_COLOR_FORMAT_XRGB8888;
    } else {
        LOG_E("unsupported display format %d\r\n", info.format);
        return NULL;
    }
    if (lv_color_format_get_size(format) != BYTE_PER_PIXEL) {
        LOG_E("display format does not match DISPLAY_DEFAULT_BPP\r\n");
        return NULL;
    }
    if ((size_t)info.width * BYTE_PER_PIXEL > DRAW_BUFF_SIZE) {
        LOG_E("draw buffer too small for %u pixels per line (CONFIG_LVGL9_DISP_DEFAULT_BUF_SIZE)\r\n", info.width);
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

    lv_disp = lv_display_create(info.width, info.height);
    lv_display_set_color_format(lv_disp, format);
    lv_display_set_flush_cb(lv_disp, disp_flush);
#if BUF_COUNT > 1
    lv_display_set_buffers(lv_disp, draw_buf_1, draw_buf_2, DRAW_BUFF_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);
#else
    lv_display_set_buffers(lv_disp, draw_buf_1, NULL, DRAW_BUFF_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif
    lv_display_set_flush_wait_cb(lv_disp, disp_flush_wait_cb);
    lv_display_add_event_cb(lv_disp, disp_resolution_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);
    lv_display_add_event_cb(lv_disp, disp_rounder_cb, LV_EVENT_INVALIDATE_AREA, NULL);
    LOG_I("default display %ux%u, %d bit, %d draw buffer(s) of %u bytes\r\n", info.width, info.height,
          BYTE_PER_PIXEL * 8, BUF_COUNT, (unsigned)DRAW_BUFF_SIZE);
    return lv_disp;
}

#else /* port of the other kind of default display, or no default display */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;

#endif
