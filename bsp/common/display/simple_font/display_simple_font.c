/**
 * @file display_simple_font.c
 * @brief Small fixed-size 1bpp ASCII renderer. No hardware or OS dependencies.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "simple_font/display_simple_font.h"

typedef struct {
    uint32_t alpha;
    uint32_t inverse;
    uint32_t source[3]; /* B,G,R premultiplied by alpha, plus rounding bias */
    uint8_t bytes[3];   /* Destination-format bytes for opaque writes */
} font_color_t;

typedef struct {
    size_t bpp;
    size_t glyph_stride;
    font_color_t colors[2]; /* Background=0, foreground=1 */
} font_render_t;

/* dst is the clipped cell's top-left pixel. stride is the destination row pitch,
 * independent of glyph_stride (bitmap row pitch). All sizes, including custom
 * non-byte-aligned widths, share these kernels. A NULL glyph is a blank cell. */
typedef void (*font_glyph_fn_t)(const font_render_t *render, uint8_t *dst, size_t stride,
                                const uint8_t *glyph, uint32_t width, uint32_t height);

/* Both colors have alpha 0 or 255: never read destination pixels. */
static void font_glyph_direct(const font_render_t *render, uint8_t *dst, size_t stride,
                              const uint8_t *glyph, uint32_t width, uint32_t height)
{
    for (uint32_t row = 0; row < height; row++) {
        uint8_t *pixel = dst;
        unsigned bits = 0;

        for (uint32_t col = 0; col < width; col++, pixel += render->bpp) {
            if (!(col & 7u)) {
                bits = glyph ? glyph[col / 8u] : 0u;
            }
            const font_color_t *color = &render->colors[(bits >> 7) & 1u];
            bits <<= 1;
            if (color->alpha) {
                pixel[0] = color->bytes[0];
                pixel[1] = color->bytes[1];
                if (render->bpp != 2u) {
                    pixel[2] = color->bytes[2];
                }
            }
        }
        /* Do not form pointers past the final destination/bitmap row. */
        if (row + 1u < height) {
            dst += stride;
            if (glyph) {
                glyph += render->glyph_stride;
            }
        }
    }
}

static void font_glyph_rgb565_alpha(const font_render_t *render, uint8_t *dst, size_t stride,
                                    const uint8_t *glyph, uint32_t width, uint32_t height)
{
    for (uint32_t row = 0; row < height; row++) {
        uint8_t *pixel = dst;
        unsigned bits = 0;

        for (uint32_t col = 0; col < width; col++, pixel += 2) {
            if (!(col & 7u)) {
                bits = glyph ? glyph[col / 8u] : 0u;
            }
            const font_color_t *color = &render->colors[(bits >> 7) & 1u];
            bits <<= 1;
            if (!color->alpha) {
                continue;
            }
            if (color->alpha == 255u) {
                pixel[0] = color->bytes[0];
                pixel[1] = color->bytes[1];
                continue;
            }
            const uint32_t old = (uint32_t)pixel[0] | ((uint32_t)pixel[1] << 8);
            uint32_t b = old & 31u;
            uint32_t g = (old >> 5) & 63u;
            uint32_t r = (old >> 11) & 31u;

            b = color->source[0] + ((b << 3) | (b >> 2)) * color->inverse;
            g = color->source[1] + ((g << 2) | (g >> 4)) * color->inverse;
            r = color->source[2] + ((r << 3) | (r >> 2)) * color->inverse;
            /* Exact rounded /255: source[] includes +128, each sum <= 65153. */
            b = (b + (b >> 8)) >> 8;
            g = (g + (g >> 8)) >> 8;
            r = (r + (r >> 8)) >> 8;
            const uint16_t packed = (uint16_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));

            pixel[0] = (uint8_t)packed;
            pixel[1] = (uint8_t)(packed >> 8);
        }
        if (row + 1u < height) {
            dst += stride;
            if (glyph) {
                glyph += render->glyph_stride;
            }
        }
    }
}

/* RGB888 and XRGB8888 share B,G,R bytes; bpp advances over X without touching it. */
static void font_glyph_rgb888_alpha(const font_render_t *render, uint8_t *dst, size_t stride,
                                    const uint8_t *glyph, uint32_t width, uint32_t height)
{
    for (uint32_t row = 0; row < height; row++) {
        uint8_t *pixel = dst;
        unsigned bits = 0;

        for (uint32_t col = 0; col < width; col++, pixel += render->bpp) {
            if (!(col & 7u)) {
                bits = glyph ? glyph[col / 8u] : 0u;
            }
            const font_color_t *color = &render->colors[(bits >> 7) & 1u];
            bits <<= 1;
            if (!color->alpha) {
                continue;
            }
            if (color->alpha == 255u) {
                pixel[0] = color->bytes[0];
                pixel[1] = color->bytes[1];
                pixel[2] = color->bytes[2];
                continue;
            }
            const uint32_t b = color->source[0] + (uint32_t)pixel[0] * color->inverse;
            const uint32_t g = color->source[1] + (uint32_t)pixel[1] * color->inverse;
            const uint32_t r = color->source[2] + (uint32_t)pixel[2] * color->inverse;

            pixel[0] = (uint8_t)((b + (b >> 8)) >> 8);
            pixel[1] = (uint8_t)((g + (g >> 8)) >> 8);
            pixel[2] = (uint8_t)((r + (r >> 8)) >> 8);
        }
        if (row + 1u < height) {
            dst += stride;
            if (glyph) {
                glyph += render->glyph_stride;
            }
        }
    }
}

int display_simple_font_draw(const display_simple_font_cfg_t *cfg, void *buff,
                             uint16_t x, uint16_t y, const char *text)
{
    size_t bpp, row_bytes, stride, span;
    uint32_t cx = x, cy = y;

    if (!cfg || !buff || !text || !cfg->width || !cfg->height || !cfg->font) {
        return -EINVAL;
    }
    const display_simple_font_t *font = cfg->font;

    if (!font->width || !font->height || !font->bitmap || !font->char_count ||
        font->first_char < 0x20u || (unsigned)font->first_char + font->char_count > 0x7fu) {
        return -EINVAL;
    }
    switch (cfg->format) {
        case DISPLAY_FMT_RGB565: bpp = 2; break;
        case DISPLAY_FMT_RGB888: bpp = 3; break;
        case DISPLAY_FMT_XRGB8888: bpp = 4; break;
        default: return -ENOTSUP;
    }
    row_bytes = (size_t)cfg->width * bpp;
    stride = cfg->stride ? cfg->stride : row_bytes;
    if (stride < row_bytes) {
        return -EINVAL;
    }
    if ((size_t)(cfg->height - 1u) > (SIZE_MAX - row_bytes) / stride) {
        return -EOVERFLOW;
    }
    span = (size_t)(cfg->height - 1u) * stride + row_bytes;
    if (span - 1u > UINTPTR_MAX - (uintptr_t)buff) {
        return -EOVERFLOW;
    }
    if (x >= cfg->width || y >= cfg->height) {
        return 0;
    }
    const size_t glyph_stride = ((size_t)font->width + 7u) / 8u;
    const size_t glyph_bytes = glyph_stride * font->height;
    const bool rgb565 = cfg->format == DISPLAY_FMT_RGB565;
    font_render_t render = { .bpp = bpp, .glyph_stride = glyph_stride };
    font_color_t *colors = render.colors;

    /* Background=0, foreground=1. No format conversion or source multiplication per pixel. */
    for (unsigned i = 0; i < 2u; i++) {
        const uint32_t color = i ? cfg->fg_color : cfg->bg_color;

        colors[i].alpha = color >> 24;
        colors[i].inverse = 255u - colors[i].alpha;
        for (unsigned channel = 0; channel < 3u; channel++) {
            uint8_t value = (uint8_t)(color >> (channel * 8u));

            colors[i].bytes[channel] = value;
            colors[i].source[channel] = (uint32_t)value * colors[i].alpha + 128u;
        }
        if (rgb565) {
            const uint16_t pixel = (uint16_t)(((color >> 8) & 0xf800u) |
                                             ((color >> 5) & 0x07e0u) | ((color >> 3) & 0x001fu));

            colors[i].bytes[0] = (uint8_t)pixel;
            colors[i].bytes[1] = (uint8_t)(pixel >> 8);
        }
    }

    const bool direct = (colors[0].alpha == 0u || colors[0].alpha == 255u) &&
                        (colors[1].alpha == 0u || colors[1].alpha == 255u);
    const font_glyph_fn_t draw_glyph = direct ? font_glyph_direct :
                                               (rgb565 ? font_glyph_rgb565_alpha : font_glyph_rgb888_alpha);

    for (const unsigned char *s = (const unsigned char *)text; *s && cy < cfg->height; s++) {
        unsigned code = *s;

        if (code == '\n') {
            cx = x;
            cy += font->height;
            continue;
        }
        if (code < 0x20u || code == 0x7fu) {
            continue;
        }
        if (cx != x && cx + font->width > cfg->width) {
            cx = x;
            cy += font->height;
            if (cy >= cfg->height) {
                break;
            }
        }
        if (code < font->first_char || code >= (unsigned)font->first_char + font->char_count) {
            code = '?';
        }
        const uint8_t *glyph = NULL;

        if (code >= font->first_char && code < (unsigned)font->first_char + font->char_count) {
            glyph = font->bitmap + (code - font->first_char) * glyph_bytes;
        }
        const uint32_t width = font->width < cfg->width - cx ? font->width : cfg->width - cx;
        const uint32_t height = font->height < cfg->height - cy ? font->height : cfg->height - cy;

        uint8_t *dst = (uint8_t *)buff + (size_t)cy * stride + (size_t)cx * bpp;

        draw_glyph(&render, dst, stride, glyph, width, height);
        cx += font->width;
    }
    return 0;
}
