/**
 * @file display_simple_font.h
 * @brief Synchronous ASCII bitmap text rendering into caller-owned RGB memory.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_SIMPLE_FONT_H
#define DISPLAY_SIMPLE_FONT_H

#include "display_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fixed-size printable ASCII glyphs, with no hardware or mutable state.
 *
 * Each glyph contains height rows of ceil(width / 8) bytes, MSB leftmost.
 * Rows are byte-padded; bit 1 selects foreground and bit 0 background.
 * bitmap must contain char_count complete glyphs and remain readable during draw.
 */
typedef struct {
    uint8_t width;          /**< Character width in pixels, nonzero */
    uint8_t height;         /**< Character height in pixels, nonzero */
    uint8_t first_char;     /**< First printable ASCII code (0x20..0x7e) */
    uint8_t char_count;     /**< Nonzero count; last code must not exceed 0x7e */
    const uint8_t *bitmap;  /**< Contiguous, read-only glyphs */
} display_simple_font_t;

/** @brief Reusable immutable configuration; dimensions describe the destination image. */
typedef struct {
    display_format_t format;           /**< RGB565, RGB888 (B,G,R), or XRGB8888 */
    uint16_t width;                     /**< Image width in pixels, nonzero */
    uint16_t height;                    /**< Image height in pixels, nonzero */
    uint32_t stride;                    /**< Bytes between rows; 0 = width * BPP, otherwise >= width * BPP */
    const display_simple_font_t *font;  /**< Explicit font, e.g. DISPLAY_SIMPLE_FONT_ASCII_16X8 */
    uint32_t fg_color;                  /**< Foreground 0xAARRGGBB; A=0 leaves destination unchanged */
    uint32_t bg_color;                  /**< Background 0xAARRGGBB; A=255 overwrites destination RGB */
} display_simple_font_cfg_t;

/**
 * @name Built-in font pointers
 * Each object requires its corresponding CONFIG_DISPLAY_SIMPLE_FONT_* option.
 * Names use height x width. All three use SDK Code Bitmap, derived from the same code font.
 * @{
 */
extern const display_simple_font_t display_simple_font_ascii_16x8; /**< Width 8, height 16; default font */
extern const display_simple_font_t display_simple_font_ascii_32x16; /**< Width 16, height 32; printable ASCII */
extern const display_simple_font_t display_simple_font_ascii_64x32; /**< Width 32, height 64; digits 0..9 only */
#define DISPLAY_SIMPLE_FONT_ASCII_16X8  (&display_simple_font_ascii_16x8)
#define DISPLAY_SIMPLE_FONT_ASCII_32X16 (&display_simple_font_ascii_32x16)
#define DISPLAY_SIMPLE_FONT_ASCII_64X32 (&display_simple_font_ascii_64x32)
/** @} */

/**
 * @brief Render text at (x,y) relative to the start of the entire image buff.
 *
 * The caller supplies writable storage of at least (height-1)*effective_stride + width*BPP
 * bytes. No allocation, display submission, cache operation, OS call or locking is performed.
 * Do not draw into a frame being read by hardware. cfg, font, bitmap and text must not overlap
 * writable destination pixels; the caller serializes access to a shared buffer.
 *
 * Newline returns to x and advances one character height; carriage return is ignored.
 * Other ASCII controls are ignored. Missing glyphs (including non-ASCII bytes) use '?',
 * or a blank character cell when '?' is absent. No UTF-8 decoding is performed.
 * Text wraps before a character that does not fit at the right edge, except a character
 * wider than the space from x to the edge is clipped there. Bottom pixels are clipped.
 * Only visited character cells are painted, never the whole line or image.
 * Each pixel selects foreground OR background and blends once, with rounded /255 alpha.
 * RGB565 is unpacked to 8-bit channels before blending; XRGB8888's X byte is preserved.
 *
 * @return 0 on success (including empty text/outside origin), -EINVAL for invalid arguments
 * or geometry, -ENOTSUP for unsupported pixel format, -EOVERFLOW for address/size overflow.
 */
int display_simple_font_draw(const display_simple_font_cfg_t *cfg, void *buff,
                             uint16_t x, uint16_t y, const char *text);

#ifdef __cplusplus
}
#endif
#endif
