#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifndef GIF_PLAYER_FRAME_TEST
#include "gif_player.h"
#endif

#if defined(GIF_PLAYER_FRAME_TEST) || LV_USE_GIF

typedef enum {
    GIF_FRAME_RGB565,
    GIF_FRAME_ARGB1555,
    GIF_FRAME_ARGB8888,
    GIF_FRAME_RGB565A8
} gif_frame_format_t;

typedef struct {
    void * pixels;
    void * restore;
    uint16_t width;
    uint16_t height;
    size_t stride;
    gif_frame_format_t format;
    uint16_t previous_x, previous_y, previous_w, previous_h;
    uint8_t previous_disposal;
    uint32_t previous_background;
} gif_frame_t;

static uint32_t gif_frame_color(gif_frame_format_t format, uint8_t r, uint8_t g, uint8_t b, bool opaque)
{
    if(format == GIF_FRAME_RGB565) return ((uint32_t)(r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    if(!opaque) return 0;
    if(format == GIF_FRAME_RGB565A8)
        return 0xff000000U | ((uint32_t)(r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    if(format == GIF_FRAME_ARGB1555) return 0x8000U | ((uint32_t)(r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
    return 0xff000000U | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

static size_t gif_frame_buffer_size(const gif_frame_t * frame)
{
    size_t color_size = frame->stride * frame->height;
    return frame->format == GIF_FRAME_RGB565A8 ? color_size + (frame->stride / 2) * frame->height : color_size;
}

static uint8_t * alpha_row(gif_frame_t * frame, unsigned y)
{
    /* LVGL stores the A8 plane after the entire RGB565 plane, at half its stride. */
    return (uint8_t *)frame->pixels + frame->stride * frame->height + y * (frame->stride / 2);
}

static void gif_frame_clear(gif_frame_t * frame)
{
    memset(frame->pixels, 0, gif_frame_buffer_size(frame));
    frame->previous_disposal = 0;
}

static void gif_frame_dispose(gif_frame_t * frame)
{
    if(frame->previous_disposal == 3 && frame->restore != NULL) {
        memcpy(frame->pixels, frame->restore, gif_frame_buffer_size(frame));
    }
    else if(frame->previous_disposal == 2) {
        for(unsigned y = frame->previous_y; y < (unsigned)frame->previous_y + frame->previous_h; y++) {
            uint8_t * row = (uint8_t *)frame->pixels + y * frame->stride;
            uint8_t * alpha = frame->format == GIF_FRAME_RGB565A8 ? alpha_row(frame, y) : NULL;
            for(unsigned x = frame->previous_x; x < (unsigned)frame->previous_x + frame->previous_w; x++) {
                if(frame->format == GIF_FRAME_ARGB8888) ((uint32_t *)row)[x] = frame->previous_background;
                else ((uint16_t *)row)[x] = (uint16_t)frame->previous_background;
                if(alpha != NULL) alpha[x] = frame->previous_background >> 24;
            }
        }
    }
    frame->previous_disposal = 0;
}

static bool gif_frame_draw_line(gif_frame_t * frame, int x, int y, int width,
                                const uint8_t * indices, const uint32_t * palette,
                                bool transparent, uint8_t transparent_index)
{
    if(x < 0 || y < 0 || width <= 0 || y >= frame->height || x > frame->width || width > frame->width - x)
        return false;
    uint8_t * row = (uint8_t *)frame->pixels + (size_t)y * frame->stride;
    uint8_t * alpha = frame->format == GIF_FRAME_RGB565A8 ? alpha_row(frame, (unsigned)y) : NULL;
    for(int i = 0; i < width; i++) {
        uint8_t index = indices[i];
        if(transparent && index == transparent_index) continue;
        if(frame->format == GIF_FRAME_ARGB8888) ((uint32_t *)row)[x + i] = palette[index];
        else ((uint16_t *)row)[x + i] = (uint16_t)palette[index];
        if(alpha != NULL) alpha[x + i] = palette[index] >> 24;
    }
    return true;
}

#ifndef GIF_PLAYER_FRAME_TEST

#include "libs/gif/AnimatedGIF.h"
#include <limits.h>
#include <stdio.h>

typedef struct {
    GIFIMAGE decoder;
    gif_frame_t frame;
    uint32_t palette[256];
    lv_timer_t * timer;
    lv_obj_t * image;
    lv_image_dsc_t image_dsc;
    bool open, first_line, failed;
    uint32_t frame_number, decoded, presented, drawn_frame, log_start, decode_total, decode_max;
} gif_player_t;

static gif_player_t player;

static void draw_line(GIFDRAW * draw)
{
    gif_player_t * p = draw->pUser;
    if(p->failed) return;
    if(p->first_line) {
        p->first_line = false;
        if(draw->iX < 0 || draw->iY < 0 || draw->iWidth <= 0 || draw->iHeight <= 0 ||
           draw->iX + draw->iWidth > p->frame.width || draw->iY + draw->iHeight > p->frame.height) {
            p->failed = true;
            return;
        }
        const uint8_t * pal = draw->pPalette24;
        for(unsigned i = 0; i < 256U; i++)
            p->palette[i] = gif_frame_color(p->frame.format, pal[i * 3U], pal[i * 3U + 1U], pal[i * 3U + 2U], true);
        if(draw->ucDisposalMethod == 3) {
            if(p->frame.restore == NULL) p->frame.restore = lv_malloc(gif_frame_buffer_size(&p->frame));
            if(p->frame.restore == NULL) {
                p->failed = true;
                return;
            }
            memcpy(p->frame.restore, p->frame.pixels, gif_frame_buffer_size(&p->frame));
        }
        p->frame.previous_x = draw->iX;
        p->frame.previous_y = draw->iY;
        p->frame.previous_w = draw->iWidth;
        p->frame.previous_h = draw->iHeight;
        p->frame.previous_disposal = draw->ucDisposalMethod;
        p->frame.previous_background = draw->ucHasTransparency && p->frame.format != GIF_FRAME_RGB565 ?
                                      0U : p->palette[draw->ucBackground];
    }
    if(!gif_frame_draw_line(&p->frame, draw->iX, draw->iY + draw->y, draw->iWidth,
                            draw->pPixels, p->palette, draw->ucHasTransparency != 0, draw->ucTransparent))
        p->failed = true;
}

static void image_drawn(lv_event_t * event)
{
    (void)event;
    if(player.drawn_frame != player.frame_number) {
        player.drawn_frame = player.frame_number;
        player.presented++;
    }
}

static void next_frame(lv_timer_t * timer)
{
    uint32_t start = lv_tick_get();
    if(player.decoder.GIFFile.iPos >= player.decoder.GIFFile.iSize - 1) {
        gif_frame_clear(&player.frame);
    }
    else gif_frame_dispose(&player.frame);
    player.first_line = true;
    int delay = 0;
    int more = GIF_playFrame(&player.decoder, &delay, &player);
    (void)more;
    if(player.failed || player.first_line || GIF_getLastError(&player.decoder) != GIF_SUCCESS) {
        printf("[GIF PLAYER] stopped: decode error=%d, bounds/allocation=%u\r\n",
               GIF_getLastError(&player.decoder), player.failed);
        lv_timer_pause(timer);
        return;
    }
    uint32_t decode = lv_tick_elaps(start);
    player.decode_total += decode;
    if(decode > player.decode_max) player.decode_max = decode;
    player.frame_number++;
    player.decoded++;
    lv_timer_set_period(timer, delay > 0 ? (uint32_t)delay : 10U);
    lv_obj_invalidate(player.image);
    uint32_t elapsed = lv_tick_elaps(player.log_start);
    if(elapsed >= 1000U) {
        uint32_t displayed = player.presented;
        uint32_t fps = player.decoded * 100000U / elapsed;
        printf("[GIF PLAYER FPS] decoded=%lu.%02lu (%lu/%lu ms), %s=%lu, decode=%lu/%lu ms, delay=%d ms\r\n",
               (unsigned long)(fps / 100U), (unsigned long)(fps % 100U),
               (unsigned long)player.decoded, (unsigned long)elapsed,
               "drawn", (unsigned long)displayed,
               (unsigned long)(player.decode_total / player.decoded), (unsigned long)player.decode_max, delay);
        player.decoded = player.decode_total = player.decode_max = 0;
        player.presented = 0;
        player.log_start = lv_tick_get();
    }
}

void gif_player_stop(void)
{
    if(player.timer != NULL) lv_timer_delete(player.timer);
    if(player.image != NULL) {
        lv_obj_delete(player.image);
    }
    if(player.open) GIF_close(&player.decoder);
    lv_free(player.frame.restore);
    lv_free(player.frame.pixels);
    memset(&player, 0, sizeof(player));
}

bool gif_player_start(lv_obj_t * parent, const lv_image_dsc_t * source, const char * name)
{
    gif_player_stop();
    if(source == NULL || source->data == NULL || source->data_size > INT_MAX) return false;
    GIF_begin(&player.decoder, GIF_PALETTE_RGB888);
    if(!GIF_openRAM(&player.decoder, (uint8_t *)source->data, (int)source->data_size, draw_line)) return false;
    player.open = true;
    player.frame.width = GIF_getCanvasWidth(&player.decoder);
    player.frame.height = GIF_getCanvasHeight(&player.decoder);
    if(player.frame.width == 0 || player.frame.width > MAX_WIDTH || player.frame.height == 0) goto fail;
#if GIF_PLAYER_LVGL_RGB565A8
    player.frame.format = GIF_FRAME_RGB565A8;
    player.frame.stride = player.frame.width * 2U;
#else
    player.frame.format = GIF_FRAME_ARGB8888;
    player.frame.stride = player.frame.width * 4U;
#endif
    player.frame.pixels = lv_malloc(gif_frame_buffer_size(&player.frame));
    if(player.frame.pixels == NULL) goto fail;
    /* Without pTurboBuffer, AnimatedGIF uses its built-in RAW line callback. */
    player.decoder.ucDrawType = GIF_DRAW_RAW;
    gif_frame_clear(&player.frame);
    if(parent == NULL || lv_display_get_default() == NULL) goto fail;
    player.image_dsc.header.magic = LV_IMAGE_HEADER_MAGIC;
    player.image_dsc.header.cf = GIF_PLAYER_LVGL_RGB565A8 ? LV_COLOR_FORMAT_RGB565A8 : LV_COLOR_FORMAT_ARGB8888;
    player.image_dsc.header.w = player.frame.width;
    player.image_dsc.header.h = player.frame.height;
    player.image_dsc.header.stride = player.frame.stride;
    player.image_dsc.data_size = gif_frame_buffer_size(&player.frame);
    player.image_dsc.data = player.frame.pixels;
    player.image = lv_image_create(parent);
    lv_image_set_src(player.image, &player.image_dsc);
    lv_obj_center(player.image);
    lv_obj_add_event_cb(player.image, image_drawn, LV_EVENT_DRAW_MAIN_END, NULL);
    player.timer = lv_timer_create(next_frame, 1, NULL);
    if(player.timer == NULL) goto fail;
    player.log_start = lv_tick_get();
    printf("[GIF PLAYER] source=%s size=%ux%u output=%s decoder=standard-raw\r\n", name,
           player.frame.width, player.frame.height,
           GIF_PLAYER_LVGL_RGB565A8 ? "LVGL-RGB565A8" : "LVGL-ARGB8888");
    printf("[GIF PLAYER] canvas_bytes=%lu\r\n", (unsigned long)gif_frame_buffer_size(&player.frame));
    next_frame(player.timer);
    if(player.failed || player.first_line) goto fail;
    return true;
fail:
    printf("[GIF PLAYER] initialization failed\r\n");
    gif_player_stop();
    return false;
}

#endif /* GIF_PLAYER_FRAME_TEST */
#endif /* GIF_PLAYER_FRAME_TEST || LV_USE_GIF */
