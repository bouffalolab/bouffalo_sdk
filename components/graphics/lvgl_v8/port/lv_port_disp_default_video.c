/**
 * @file lv_port_disp_default_video.c
 * @brief LVGL v8 full-refresh double-buffered port for the default video display (MIPI DSI / RGB).
 *
 * Kconfig selects the screen. Configure any display module override before calling lv_port_disp_init().
 * This port initializes and exclusively owns the default screen layer and two static frames in PSRAM.
 * Initialization clears the buffers but does not submit a frame; the first completed LVGL refresh does.
 *
 * LVGL redraws the whole off-screen frame on each refresh, without synchronizing dirty areas.
 * Only the last flush of a refresh submits that frame. FIRST confirms the first frame; RETIRED releases
 * the replaced frame. VSYNC alone does not complete a flip.
 *
 * The last flush only submits the frame. LVGL calls wait_cb before rendering the next frame;
 * it waits for flip completion before the old frame becomes writable. DIRECT mode is not used.
 * LVGL v8's built-in software rotation does not support full_refresh.
 *
 * - The size of the buffers is the size of a frame; CONFIG_LVGL8_DISP_DEFAULT_BUF_SIZE does not apply.
 * - Rotation is not supported by this port.
 * - Pixel format: RGB565, XRGB8888 or ARGB8888 matching LV_COLOR_DEPTH (16 or 32).
 *   ARGB8888 requires LV_COLOR_SCREEN_TRANSP; packed RGB888 is not supported by v8.
 *
 * A project with several screens, or with special needs, writes its own port instead.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "lv_port_disp_default.h"

#if defined(CONFIG_DISPLAY_DEFAULT_TYPE_VIDEO)

#include "bflb_core.h"
#include "default/display_default.h"
#include "display_os.h"
#include <stdbool.h>
#include <string.h>

#if defined(CONFIG_FREERTOS)
#include "FreeRTOS.h"
#include "semphr.h"
#endif

#define DBG_TAG "LV_DISP"
#include "log.h"

#define BYTE_PER_PIXEL DISPLAY_DEFAULT_BPP
#define CACHE_LINE     64
#define FRAME_BYTES    DISPLAY_DEFAULT_VIDEO_FRAME_BYTES

_Static_assert(sizeof(lv_color_t) == BYTE_PER_PIXEL,
               "lv_color_t does not match the format of the default display; check LV_COLOR_DEPTH");

static uint8_t frames[2][FRAME_BYTES] ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(CACHE_LINE)));

static display_t *lcd;
static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;
static bool port_initialized;
static display_format_t fb_format; /* format of the frames, from the display */
static volatile bool outstanding;  /* a flip is awaiting FIRST / RETIRED */

#if defined(CONFIG_FREERTOS)
static SemaphoreHandle_t flip_sem;
#endif

/* ISR callback: FIRST / RETIRED completes a flip; VSYNC does not. */
static void disp_event_cb(display_t *display, const display_event_t *event, void *ctx)
{
    (void)display;
    (void)ctx;
    if (!outstanding || (event->type != DISPLAY_EVT_FB_FIRST && event->type != DISPLAY_EVT_FB_RETIRED)) {
        return;
    }
    outstanding = false;
#if defined(CONFIG_FREERTOS)
    BaseType_t woken = pdFALSE;

    xSemaphoreGiveFromISR(flip_sem, &woken);
    portYIELD_FROM_ISR(woken);
#endif
}

/* Wait for flip completion. A timeout never makes a scanned frame writable. */
static void wait_flip_done(void)
{
#if defined(CONFIG_FREERTOS)
    while (outstanding) { /* a token left by an earlier completion only makes the loop run once more */
        if (xSemaphoreTake(flip_sem, pdMS_TO_TICKS(500)) != pdTRUE) {
            LOG_E("frame flip did not complete\r\n");
        }
    }
#else
    unsigned elapsed = 0;

    while (outstanding) {
        display_os_delay_ms(1);
        if (++elapsed == 500) {
            LOG_E("frame flip did not complete\r\n");
            elapsed = 0;
        }
    }
#endif
}

/* Flush: LVGL has rendered into a frame. Only the last area of a refresh shows the frame. */
static void disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *px_map)
{
    display_fb_t fb = { .data = px_map, .format = fb_format };
    int ret;

    (void)area;
    if (!lv_disp_flush_is_last(disp)) {
        lv_disp_flush_ready(disp);
        return;
    }
    do {
        outstanding = true; /* completion can precede display_flip's return */
        ret = display_flip(lcd, &fb);
        if (ret) {
            outstanding = false;
            LOG_E("display_flip failed: %d; keeping LVGL's buffers busy\r\n", ret);
            display_os_delay_ms(500);
        }
    } while (ret); /* never let LVGL reuse the old scanned frame after a rejected submission */

    /* Full refresh has no dirty-area synchronization before the rendering wait point. */
}

/* LVGL waits here before rendering into the old frame. */
static void disp_wait_cb(lv_disp_drv_t *disp)
{
    wait_flip_done();
    lv_disp_flush_ready(disp);
}

lv_disp_drv_t *lv_port_disp_init(void)
{
    display_info_t info;
    unsigned pixel_size;
    int ret;

    if (port_initialized) {
        return &disp_drv;
    }
#if defined(CONFIG_FREERTOS)
    flip_sem = xSemaphoreCreateBinary();
    if (!flip_sem) {
        LOG_E("frame semaphore allocation failed\r\n");
        return NULL;
    }
#endif
    outstanding = false;
    lcd = display_default_get();
    ret = display_init(lcd);
    ret = ret ? ret : display_get_info(lcd, &info);
    if (ret) {
        LOG_E("default display initialization failed: %d\r\n", ret);
        goto fail;
    }
    if (info.state != DISPLAY_STATE_READY || info.kind != DISPLAY_KIND_FB ||
        info.width != DISPLAY_DEFAULT_VIDEO_WIDTH || info.height != DISPLAY_DEFAULT_VIDEO_HEIGHT) {
        LOG_E("default display is not a ready framebuffer of %ux%u\r\n",
              (unsigned)DISPLAY_DEFAULT_VIDEO_WIDTH, (unsigned)DISPLAY_DEFAULT_VIDEO_HEIGHT);
        goto fail;
    }

    /* Match LVGL's pixel format to the default display. */
    if (info.format == DISPLAY_FMT_RGB565) {
        pixel_size = 2;
    } else if (info.format == DISPLAY_FMT_XRGB8888 || info.format == DISPLAY_FMT_ARGB8888) {
        pixel_size = 4;
        if (info.format == DISPLAY_FMT_ARGB8888 && !LV_COLOR_SCREEN_TRANSP) {
            LOG_E("ARGB8888 requires LV_COLOR_SCREEN_TRANSP\r\n");
            goto fail;
        }
    } else {
        LOG_E("unsupported display format %d\r\n", info.format);
        goto fail;
    }
    if (pixel_size != sizeof(lv_color_t)) {
        LOG_E("lv_color_t is %u bytes but the display uses %u bytes per pixel\r\n",
              (unsigned)sizeof(lv_color_t), pixel_size);
        goto fail;
    }
    fb_format = info.format;
    memset(frames, 0, sizeof(frames));

    lv_disp_draw_buf_init(&draw_buf, frames[0], frames[1], (uint32_t)info.width * info.height);
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = (lv_coord_t)info.width;
    disp_drv.ver_res = (lv_coord_t)info.height;
    disp_drv.flush_cb = disp_flush;
    disp_drv.wait_cb = disp_wait_cb;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.full_refresh = 1;
#if defined(CONFIG_LVGL8_DISP_DEFAULT_SW_ROTATE)
    disp_drv.sw_rotate = 1;
#endif
#if LV_COLOR_SCREEN_TRANSP
    disp_drv.screen_transp = (info.format == DISPLAY_FMT_ARGB8888);
#endif
    if (!lv_disp_drv_register(&disp_drv)) {
        LOG_E("LVGL display allocation failed\r\n");
        goto fail;
    }
    display_set_callback(lcd, disp_event_cb, NULL);
    display_set_backlight(lcd, DISPLAY_BACKLIGHT_MAX);
    port_initialized = true;
    LOG_I("default video display %ux%u, %d bit, 2 frame buffers of %u bytes\r\n", info.width, info.height,
          BYTE_PER_PIXEL * 8, (unsigned)FRAME_BYTES);
    return &disp_drv;

fail:
#if defined(CONFIG_FREERTOS)
    vSemaphoreDelete(flip_sem);
    flip_sem = NULL;
#endif
    return NULL;
}

#else /* port of the other kind of default display, or no default display */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;

#endif
