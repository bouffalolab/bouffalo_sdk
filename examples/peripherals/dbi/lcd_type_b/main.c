#include "board.h"
#include "default/display_default.h"
#include "display_os.h"
#include "simple_font/display_simple_font.h"
#include <string.h>

#define DBG_TAG "MAIN"
#include "log.h"

#if !defined(CONFIG_DISPLAY_DEFAULT_TYPE_GRAM) || !defined(CONFIG_DISPLAY_DEFAULT_IF_DBI_B8)
#error "This demo requires the default GRAM display on DBI Type B x8"
#endif

#if defined(CONFIG_DISPLAY_DEFAULT_FORMAT_RGB565)
typedef uint16_t pixel_t;
#define PIXEL_RGB(r, g, b) ((pixel_t)((((r) & 0xf8) << 8) | (((g) & 0xfc) << 3) | ((b) >> 3)))
#elif defined(CONFIG_DISPLAY_DEFAULT_FORMAT_XRGB8888)
typedef uint32_t pixel_t;
#define PIXEL_RGB(r, g, b) ((pixel_t)(((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (b)))
#else
#error "This demo supports RGB565 or XRGB8888 only"
#endif

#define BLOCK_W           100u
#define BLOCK_H           100u
#define FILL_PIXELS       2000u
#define BORDER_COUNT      6u
#define BORDER_WIDTH      2u
#define BORDER_GAP        3u
#define MIN_SCREEN_SIZE   160u
#define HUE_PERIOD        1536u
#define ANIMATION_STEP    6u
#define FRAME_DELAY_MS    30u

/* All draws start at this aligned address, including narrow border strips.
 * Static storage also leaves a complete word accessible for odd RGB565 counts. */
static pixel_t pixels[FILL_PIXELS] __attribute__((aligned(64)));
static pixel_t center_pixels[BLOCK_W * BLOCK_H] __attribute__((aligned(64)));
static display_t *screen;
static display_info_t info;
static size_t chunk_pixels;
static volatile bool draw_pending;
static volatile int draw_result;
static const void *pending_pixels;

#if defined(CONFIG_DISPLAY_DEFAULT_MODULE_ST7796_320X480)
/* Match the ST7796 glass used by the BL616 LVGL v9 example. */
static const display_dcs_cmd_t st7796_inversion[] = {
    DISPLAY_DCS(0xF0, 0, 0xC3), /* Enable vendor commands. */
    DISPLAY_DCS(0xF0, 0, 0x96),
    DISPLAY_DCS(0xB4, 0, 0x01), /* 1-dot column inversion. */
    DISPLAY_DCS(0xF0, 0, 0x3C), /* Disable vendor commands. */
    DISPLAY_DCS(0xF0, 0, 0x69),
};

static void adjust_default_module(void)
{
    display_dcs_module_t *module = display_default_get_module();

    module->madctl[0] = 0x48;
    module->madctl[1] = 0xE8;
    module->madctl[2] = 0x88;
    module->madctl[3] = 0x28;
    module->invert = true;
    module->init_extra = st7796_inversion;
    module->init_extra_count = DISPLAY_DCS_COUNT(st7796_inversion);
}
#endif

/* Integer HSV wheel: hue wraps continuously through six 256-step sectors. */
static pixel_t gradient_color(uint16_t hue, uint8_t value)
{
    uint16_t h = hue % HUE_PERIOD;
    uint8_t rising = (uint32_t)(h & 255u) * value / 255u;
    uint8_t falling = value - rising;

    switch (h / 256u) {
        case 0: return PIXEL_RGB(value, rising, 0);
        case 1: return PIXEL_RGB(falling, value, 0);
        case 2: return PIXEL_RGB(0, value, rising);
        case 3: return PIXEL_RGB(0, falling, value);
        case 4: return PIXEL_RGB(rising, 0, value);
        default: return PIXEL_RGB(value, 0, falling);
    }
}

static void fill_pixels(pixel_t color, size_t count)
{
    for (size_t i = 0; i < count; i++) {
        pixels[i] = color;
    }
}

/* Uniform fills are tiled in both axes: neither a long scanline nor a full
 * screen needs to fit in the staging buffer or in one DMA descriptor list. */
static int fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, pixel_t color)
{
    if (!w || !h || (uint32_t)x + w > info.width || (uint32_t)y + h > info.height) {
        return -EINVAL;
    }
    fill_pixels(color, chunk_pixels);
    for (uint16_t dx = 0; dx < w;) {
        uint16_t cw = w - dx;

        if (cw > chunk_pixels) {
            cw = chunk_pixels;
        }
        for (uint16_t dy = 0; dy < h;) {
            uint16_t ch = h - dy;
            size_t rows = chunk_pixels / cw;
            int ret;

            if (ch > rows) {
                ch = rows;
            }
            display_area_t area = { .x = x + dx, .y = y + dy, .w = cw, .h = ch };

            ret = display_draw(screen, &area, pixels, (size_t)cw * ch * sizeof(pixel_t));
            if (ret < 0) {
                return ret;
            }
            dy += ch;
        }
        dx += cw;
    }
    return 0;
}

static int prepare_center(void)
{
    static const char *const lines[] = { "DBI", "Display", "Test" };
    const display_simple_font_cfg_t cfg = {
        .format = info.format,
        .width = BLOCK_W,
        .height = BLOCK_H,
        .font = DISPLAY_SIMPLE_FONT_ASCII_16X8,
        .fg_color = 0xffffffff, /* Opaque white glyphs. */
        .bg_color = 0x00000000, /* Preserve the gradient between glyph pixels. */
    };

    for (unsigned y = 0; y < BLOCK_H; y++) {
        for (unsigned x = 0; x < BLOCK_W; x++) {
            center_pixels[y * BLOCK_W + x] =
                gradient_color(x * (HUE_PERIOD - 1u) / (BLOCK_W - 1u) + y * 256u / (BLOCK_H - 1u),
                               128u + y * 127u / (BLOCK_H - 1u));
        }
    }
    /* A single 16-character line would be 128 px wide. Center each word in
     * three 8x16 lines, with 4 px between lines (56 px total height). */
    for (unsigned i = 0; i < sizeof(lines) / sizeof(lines[0]); i++) {
        uint16_t x = (BLOCK_W - strlen(lines[i]) * 8u) / 2u;
        int ret = display_simple_font_draw(&cfg, center_pixels, x, 22u + i * 20u, lines[i]);

        if (ret < 0) {
            return ret;
        }
    }
    return 0;
}

static int draw_border(uint16_t inset, pixel_t color)
{
    if ((uint32_t)inset * 2u + BORDER_WIDTH * 2u > info.width ||
        (uint32_t)inset * 2u + BORDER_WIDTH * 2u > info.height) {
        return -EINVAL;
    }
    uint16_t w = info.width - 2 * inset;
    uint16_t h = info.height - 2 * inset;
    int ret;

    ret = fill_rect(inset, inset, w, BORDER_WIDTH, color);
    if (ret < 0) {
        return ret;
    }
    ret = fill_rect(inset, info.height - inset - BORDER_WIDTH, w, BORDER_WIDTH, color);
    if (ret < 0) {
        return ret;
    }
    ret = fill_rect(inset, inset + BORDER_WIDTH, BORDER_WIDTH, h - 2 * BORDER_WIDTH, color);
    if (ret < 0) {
        return ret;
    }
    return fill_rect(info.width - inset - BORDER_WIDTH, inset + BORDER_WIDTH,
                     BORDER_WIDTH, h - 2 * BORDER_WIDTH, color);
}

static int draw_tunnel(uint16_t phase)
{
    /* Inner rings are brighter; hue shifts smoothly through the depth. */
    for (unsigned depth = 0; depth < BORDER_COUNT; depth++) {
        unsigned ring = BORDER_COUNT - 1u - depth;
        uint16_t inset = ring * (BORDER_WIDTH + BORDER_GAP);
        pixel_t color = gradient_color(phase + depth * 80u, 255u - depth * 28u);
        int ret = draw_border(inset, color);

        if (ret < 0) {
            return ret;
        }
    }
    return 0;
}

static void draw_done(display_t *display, const display_event_t *event, void *ctx)
{
    (void)display;
    (void)ctx;

    /* May run in the ISR, or inline during submit when DMA is disabled. */
    if (display == screen && event->type == DISPLAY_EVT_DRAW_DONE &&
        event->buf == pending_pixels && draw_pending) {
        draw_result = event->result;
        draw_pending = false;
    }
}

static int draw_center(void)
{
    const uint16_t x = (info.width - BLOCK_W) / 2u;
    const uint16_t y = (info.height - BLOCK_H) / 2u;
    const size_t row_bytes = BLOCK_W * sizeof(pixel_t);
    size_t max_rows = info.max_draw_bytes / row_bytes;

    if (!max_rows) {
        return -ENOTSUP;
    }
    if (max_rows > BLOCK_H) {
        max_rows = BLOCK_H;
    }
    /* Keep every stripe start aligned, even on a backend with stricter alignment. */
    while (max_rows && info.buf_align && (max_rows * row_bytes) % info.buf_align) {
        max_rows--;
    }
    if (!max_rows) {
        return -ENOTSUP;
    }
    for (unsigned row = 0; row < BLOCK_H;) {
        unsigned rows = BLOCK_H - row;
        if (rows > max_rows) {
            rows = max_rows;
        }
        display_area_t area = { .x = x, .y = y + row, .w = BLOCK_W, .h = rows };

        /* Arm BEFORE submit; completion may precede return, even without DMA. */
        pending_pixels = center_pixels + row * BLOCK_W;
        draw_result = -EINPROGRESS;
        draw_pending = true;
        int ret = display_draw_async(screen, &area, pending_pixels, rows * row_bytes);

        if (ret < 0) {
            draw_pending = false;
            return ret;
        }
        while (draw_pending) {
            display_os_delay_ms(1);
        }
        if (draw_result != 0) {
            return draw_result;
        }
        row += rows;
    }
    return 0;
}

int main(void)
{
    uint16_t phase = 0;
    int ret;

    board_init();
    LOG_I("DBI Type B display demo\r\n");

#if defined(CONFIG_DISPLAY_DEFAULT_MODULE_ST7796_320X480)
    adjust_default_module();
#endif
    screen = display_default_get();
    ret = display_init(screen);
    if (ret < 0) {
        goto fail;
    }
    ret = display_get_info(screen, &info);
    if (ret < 0) {
        goto fail;
    }
    /* Six rings consume 27 px per side; leave at least 3 px before the block. */
    if (info.kind != DISPLAY_KIND_GRAM || info.width < MIN_SCREEN_SIZE || info.height < MIN_SCREEN_SIZE ||
        display_format_bytes(info.format) != sizeof(pixel_t) || info.x_align > 1 || info.y_align > 1 ||
        (info.buf_align && ((uintptr_t)pixels % info.buf_align || (uintptr_t)center_pixels % info.buf_align)) ||
        info.max_draw_bytes < BLOCK_W * sizeof(pixel_t)) {
        ret = -ENOTSUP;
        goto fail;
    }
    chunk_pixels = info.max_draw_bytes / sizeof(pixel_t);
    if (chunk_pixels > FILL_PIXELS) {
        chunk_pixels = FILL_PIXELS;
    }
    ret = display_set_callback(screen, draw_done, NULL);
    if (ret < 0) {
        goto fail;
    }
    ret = fill_rect(0, 0, info.width, info.height, PIXEL_RGB(0, 0, 0));
    if (ret < 0) {
        goto fail;
    }
    ret = prepare_center();
    if (ret < 0) {
        goto fail;
    }
    ret = draw_center();
    if (ret < 0) {
        goto fail;
    }
    ret = draw_tunnel(phase);
    if (ret < 0) {
        goto fail;
    }
    ret = display_set_backlight(screen, DISPLAY_BACKLIGHT_MAX);
    if (ret < 0 && ret != -ENOTSUP) {
        goto fail;
    }

    while (1) {
        phase = (phase + ANIMATION_STEP) % HUE_PERIOD;
        ret = draw_tunnel(phase);
        if (ret < 0) {
            goto fail;
        }
        /* Only the rings change; the center image stays fixed in panel GRAM. */
        display_os_delay_ms(FRAME_DELAY_MS);
    }

fail:
    LOG_E("display demo failed: %d\r\n", ret);
    ret = display_deinit(screen);
    if (ret < 0) {
        LOG_E("display deinit failed: %d\r\n", ret);
    }
    while (1) {
        display_os_delay_ms(1000);
    }
}
