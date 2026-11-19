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

#if GIF_PLAYER_OUTPUT != GIF_PLAYER_LVGL
#include "lcd.h"
#include "bflb_dpi.h"
#include "bflb_osd.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"
#if !defined(BL618DG) || !defined(LCD_DSI_AXS15231E_JX371)
#error "GIF hardware backends currently require BL618DG and the JX371 DSI panel"
#endif
#endif

#define GIF_HARDWARE_OUTPUT_BUFFERS 2U

typedef struct {
    GIFIMAGE decoder;
    gif_frame_t frame;
    uint32_t palette[256];
    lv_timer_t * timer;
    lv_obj_t * image;
    lv_image_dsc_t image_dsc;
    bool open, first_line, failed, hardware_ready;
    uint32_t frame_number, decoded, presented, drawn_frame, log_start, decode_total, decode_max;
#if GIF_PLAYER_OUTPUT != GIF_PLAYER_LVGL
    void * output_raw[GIF_HARDWARE_OUTPUT_BUFFERS];
    uint16_t * output[GIF_HARDWARE_OUTPUT_BUFFERS];
    volatile int active, pending;
    volatile uint32_t scanned;
    struct bflb_device_s * dpi;
    struct bflb_device_s * osd;
#endif
} gif_player_t;

static gif_player_t player;

#if GIF_PLAYER_OUTPUT != GIF_PLAYER_LVGL
static void * aligned_alloc64(size_t size, void ** raw)
{
    *raw = lv_malloc(size + 63U);
    return *raw == NULL ? NULL : (void *)(((uintptr_t)*raw + 63U) & ~(uintptr_t)63U);
}
#endif

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

#if GIF_PLAYER_OUTPUT != GIF_PLAYER_LVGL
static void rgb_swap(void)
{
    if(player.pending >= 0) {
        player.active = player.pending;
        player.pending = -1;
        player.scanned++;
    }
}

#if GIF_PLAYER_OUTPUT == GIF_PLAYER_RGB555A1_OSD
static void osd_isr(int irq, void * arg)
{
    (void)irq;
    (void)arg;
    bflb_osd_int_clear(player.osd);
    rgb_swap();
}
#endif

static bool hardware_init(void)
{
    player.dpi = bflb_device_get_by_name("dpi");
    player.osd = bflb_device_get_by_name("osd1");
    if(player.dpi == NULL || player.osd == NULL) return false;
    struct bflb_device_s * gpio = bflb_device_get_by_name("gpio");
    if(gpio == NULL) return false;
#if defined(LCD_BACKLIGHT_EN) && LCD_BACKLIGHT_EN
    bflb_gpio_init(gpio, LCD_BACKLIGHT_PIN, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    lcd_backlight_toggle(false);
#endif
#if defined(LCD_RESET_EN) && LCD_RESET_EN
    bflb_gpio_init(gpio, LCD_RESET_PIN, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
#if LCD_RESET_ACTIVE_LEVEL
    bflb_gpio_set(gpio, LCD_RESET_PIN);
#else
    bflb_gpio_reset(gpio, LCD_RESET_PIN);
#endif
    bflb_mtimer_delay_ms(LCD_RESET_HOLD_MS);
#if LCD_RESET_ACTIVE_LEVEL
    bflb_gpio_reset(gpio, LCD_RESET_PIN);
#else
    bflb_gpio_set(gpio, LCD_RESET_PIN);
#endif
    bflb_mtimer_delay_ms(LCD_RESET_DELAY);
#endif
    if(display_prepare() != 0) return false;
    const mipi_dsi_v2_timing_t * timing = axs15231e_jx371_dsi_get_timing();
    for(unsigned i = 0; i < GIF_HARDWARE_OUTPUT_BUFFERS; i++) {
        player.output[i] = aligned_alloc64((size_t)LCD_W * LCD_H * 2U, &player.output_raw[i]);
        if(player.output[i] == NULL) return false;
        memset(player.output[i], 0, (size_t)LCD_W * LCD_H * 2U);
        bflb_l1c_dcache_clean_range(player.output[i], LCD_W * LCD_H * 2U);
    }
    player.active = 0;
    player.pending = -1;
#if GIF_PLAYER_OUTPUT == GIF_PLAYER_RGB565_DIRECT
    static uint32_t sync_pixel __ALIGNED(64);
    sync_pixel = 0;
    bflb_l1c_dcache_clean_range(&sync_pixel, sizeof(sync_pixel));
    if(mipi_dsi_v2_rgb565_display_init(timing, (uint32_t)(uintptr_t)player.output[0],
                                      (uint32_t)(uintptr_t)&sync_pixel) != 0) return false;
    mipi_dsi_v2_rgb565_frame_callback_register(MIPI_DSI_V2_FRAME_INT_TYPE_SWAP, rgb_swap);
#else
    struct bflb_dpi_config_s config = {
        .width = timing->width, .height = timing->height,
        .hsw = timing->hsw, .hbp = timing->hbp, .hfp = timing->hfp,
        .vsw = timing->vsw, .vbp = timing->vbp, .vfp = timing->vfp,
        .interface = DPI_INTERFACE_24_PIN,
        .input_sel = DPI_INPUT_SEL_TEST_PATTERN_WITH_OSD,
        .test_pattern = DPI_TEST_PATTERN_BLACK,
        .data_format = DPI_DATA_FORMAT_TEST_PATTERN,
    };
    bflb_dpi_init(player.dpi, &config);
    struct bflb_osd_blend_config_s overlay = {
        .blend_format = OSD_BLEND_FORMAT_ARGB1555,
        .order_a = 3, .order_rv = 2, .order_gy = 1, .order_bu = 0,
        .coor = { .start_x = 0, .start_y = 0, .end_x = LCD_W, .end_y = LCD_H },
        .layer_buffer_addr = (uint32_t)(uintptr_t)player.output[0],
    };
    bflb_osd_blend_init(player.osd, &overlay);
    bflb_osd_blend_set_global_a(player.osd, false, 255);
    bflb_osd_blend_enable(player.osd);
    bflb_osd_int_clear(player.osd);
    bflb_irq_attach(player.osd->irq_num, osd_isr, NULL);
    bflb_osd_int_mask(player.osd, false);
    bflb_irq_enable(player.osd->irq_num);
#endif
    player.hardware_ready = true;
    return true;
}

static void hardware_present(void)
{
    /* next_frame waits for the previous swap before reusing the other buffer. */
    int next = 1 - player.active;
    uint16_t * dst = player.output[next];
    unsigned x = (LCD_W - player.frame.width) / 2U;
    unsigned y = (LCD_H - player.frame.height) / 2U;
    for(unsigned row = 0; row < player.frame.height; row++)
        memcpy(dst + (y + row) * LCD_W + x,
               (uint8_t *)player.frame.pixels + row * player.frame.stride, player.frame.width * 2U);
#if GIF_PLAYER_OUTPUT == GIF_PLAYER_RGB565_DIRECT
    /* The existing API cleans cache and queues the buffer. Protect pending
     * publication so its ISR cannot acknowledge a half-published frame. */
    uintptr_t flags = bflb_irq_save();
    player.pending = next;
    mipi_dsi_v2_rgb565_screen_switch(dst);
    bflb_irq_restore(flags);
#else
    bflb_l1c_dcache_clean_range(dst, LCD_W * LCD_H * 2U);
    uintptr_t flags = bflb_irq_save();
    player.pending = next;
    bflb_osd_blend_set_layer_buffer(player.osd, (uint32_t)(uintptr_t)dst);
    bflb_irq_restore(flags);
#endif
}
#else
static void image_drawn(lv_event_t * event)
{
    (void)event;
    if(player.drawn_frame != player.frame_number) {
        player.drawn_frame = player.frame_number;
        player.presented++;
    }
}
#endif

static void next_frame(lv_timer_t * timer)
{
    uint32_t start = lv_tick_get();
#if GIF_PLAYER_OUTPUT != GIF_PLAYER_LVGL
    if(player.pending >= 0) {
        lv_timer_set_period(timer, 1);
        return;
    }
#endif
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
#if GIF_PLAYER_OUTPUT == GIF_PLAYER_LVGL
    lv_obj_invalidate(player.image);
#else
    hardware_present();
#endif
    uint32_t elapsed = lv_tick_elaps(player.log_start);
    if(elapsed >= 1000U) {
#if GIF_PLAYER_OUTPUT == GIF_PLAYER_LVGL
        uint32_t displayed = player.presented;
#else
        uint32_t displayed = player.scanned;
#endif
        uint32_t fps = player.decoded * 100000U / elapsed;
        printf("[GIF PLAYER FPS] decoded=%lu.%02lu (%lu/%lu ms), %s=%lu, decode=%lu/%lu ms, delay=%d ms\r\n",
               (unsigned long)(fps / 100U), (unsigned long)(fps % 100U),
               (unsigned long)player.decoded, (unsigned long)elapsed,
               GIF_PLAYER_OUTPUT == GIF_PLAYER_LVGL ? "drawn" : "presented", (unsigned long)displayed,
               (unsigned long)(player.decode_total / player.decoded), (unsigned long)player.decode_max, delay);
        player.decoded = player.decode_total = player.decode_max = 0;
        player.presented = 0;
#if GIF_PLAYER_OUTPUT != GIF_PLAYER_LVGL
        uintptr_t flags = bflb_irq_save();
        player.scanned = 0;
        bflb_irq_restore(flags);
#endif
        player.log_start = lv_tick_get();
    }
}

void gif_player_stop(void)
{
    if(player.timer != NULL) lv_timer_delete(player.timer);
    if(player.image != NULL) {
        lv_obj_delete(player.image);
    }
#if GIF_PLAYER_OUTPUT != GIF_PLAYER_LVGL
    if(player.hardware_ready) {
#if GIF_PLAYER_OUTPUT == GIF_PLAYER_RGB565_DIRECT
        mipi_dsi_v2_rgb565_frame_callback_register(MIPI_DSI_V2_FRAME_INT_TYPE_SWAP, NULL);
#endif
        bflb_irq_disable(player.osd->irq_num);
        bflb_osd_int_mask(player.osd, true);
        bflb_dpi_disable(player.dpi);
        bflb_osd_blend_disable(player.osd);
        mipi_dsi_v2_hs_mode_stop();
    }
    for(unsigned i = 0; i < GIF_HARDWARE_OUTPUT_BUFFERS; i++) lv_free(player.output_raw[i]);
#endif
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
#if GIF_PLAYER_OUTPUT == GIF_PLAYER_LVGL
#if GIF_PLAYER_LVGL_RGB565A8
    player.frame.format = GIF_FRAME_RGB565A8;
    player.frame.stride = player.frame.width * 2U;
#else
    player.frame.format = GIF_FRAME_ARGB8888;
    player.frame.stride = player.frame.width * 4U;
#endif
#else
    if(player.frame.width > LCD_W || player.frame.height > LCD_H) goto fail;
    player.frame.format = GIF_PLAYER_OUTPUT == GIF_PLAYER_RGB565_DIRECT ? GIF_FRAME_RGB565 : GIF_FRAME_ARGB1555;
    player.frame.stride = player.frame.width * 2U;
#endif
    player.frame.pixels = lv_malloc(gif_frame_buffer_size(&player.frame));
    if(player.frame.pixels == NULL) goto fail;
    /* Without pTurboBuffer, AnimatedGIF uses its built-in RAW line callback. */
    player.decoder.ucDrawType = GIF_DRAW_RAW;
    gif_frame_clear(&player.frame);
#if GIF_PLAYER_OUTPUT == GIF_PLAYER_LVGL
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
#else
    (void)parent;
    if(!hardware_init()) goto fail;
#endif
    player.timer = lv_timer_create(next_frame, 1, NULL);
    if(player.timer == NULL) goto fail;
    player.log_start = lv_tick_get();
    printf("[GIF PLAYER] source=%s size=%ux%u output=%s decoder=standard-raw\r\n", name,
           player.frame.width, player.frame.height,
           GIF_PLAYER_OUTPUT == GIF_PLAYER_RGB555A1_OSD ? "RGB555A1-OSD (A1R5G5B5)" :
           GIF_PLAYER_OUTPUT == GIF_PLAYER_RGB565_DIRECT ? "RGB565-DIRECT" :
           GIF_PLAYER_LVGL_RGB565A8 ? "LVGL-RGB565A8" : "LVGL-ARGB8888");
    printf("[GIF PLAYER] canvas_bytes=%lu\r\n", (unsigned long)gif_frame_buffer_size(&player.frame));
#if GIF_PLAYER_OUTPUT != GIF_PLAYER_LVGL
    printf("[GIF PLAYER] output_buffers=%u output_bytes=%lu\r\n",
           (unsigned)GIF_HARDWARE_OUTPUT_BUFFERS,
           (unsigned long)(GIF_HARDWARE_OUTPUT_BUFFERS * ((size_t)LCD_W * LCD_H * 2U + 63U)));
#endif
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
