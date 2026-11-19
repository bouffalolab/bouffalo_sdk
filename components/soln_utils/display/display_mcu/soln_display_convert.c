#include "soln_display_convert.h"

#include <stddef.h>

/* Alias-safe aligned word access. Supported targets are little-endian.
 * Alignment is checked before calling these helpers. */
static inline uint32_t load_word(const uint8_t *src)
{
    uint32_t value;
    __builtin_memcpy(&value, __builtin_assume_aligned(src, 4), sizeof(value));
    return value;
}

static inline void store_word(uint8_t *dst, uint32_t value)
{
    __builtin_memcpy(__builtin_assume_aligned(dst, 4), &value, sizeof(value));
}

static inline void expand_pair(uint8_t *restrict dst, const uint8_t *restrict src)
{
    const uint8_t y0 = src[0], u = src[1], y1 = src[2], v = src[3];
    dst[0] = y0; dst[1] = u; dst[2] = v;
    dst[3] = y1; dst[4] = u; dst[5] = v;
}

void soln_yuyv422_to_yuv444_band(uint8_t *restrict dst, const uint8_t *restrict src,
                               uint16_t width, uint16_t y, uint16_t rows)
{
    const size_t source_stride = (size_t)width * 2u;
    const size_t dest_stride = (size_t)width * 3u;
    const uint8_t *srow = src + (size_t)y * source_stride;

    for (unsigned row = 0; row < rows; row++) {
        const uint8_t *s = srow;
        uint8_t *d = dst;
        unsigned x = 0;

        if (!(((uintptr_t)s | (uintptr_t)d) & 3u)) {
            /* Two YUYV words -> four YUV444 pixels -> three aligned words. */
            for (; x + 4u <= width; x += 4u, s += 8, d += 12) {
                uint32_t a = load_word(s), b = load_word(s + 4);
                store_word(d, (a & 0xFFFFu) | ((a >> 8) & 0xFF0000u) | ((a & 0xFF0000u) << 8));
                store_word(d + 4, ((a >> 8) & 0xFFu) | ((a >> 16) & 0xFF00u) | (b << 16));
                store_word(d + 8, (b >> 24) | ((b >> 8) & 0xFF00u) | ((b & 0xFF00u) << 8) |
                                   (b & 0xFF000000u));
            }
        }
        for (; x < width; x += 2u, s += 4, d += 6) {
            expand_pair(d, s);
        }
        srow += source_stride;
        dst += dest_stride;
    }
}

void soln_yuyv422_640x480_to_yuv444_480x320_band(uint8_t *restrict dst,
                                             const uint8_t *restrict src,
                                             uint16_t y, uint16_t rows)
{
    const uint8_t *source = src + (size_t)(y / 2u) * 3840u;

    for (unsigned row = 0; row < rows; row += 2u) {
        const uint8_t *s0 = source, *s2 = source + 2560u;
        uint8_t *d0 = dst, *d1 = dst + 1440u;

        /* Source 4 columns x 3 rows -> output 3 columns x 2 rows.
         * Select rows 0/2 and columns 0/2/3; keep each Y's original pair U/V.
         * Process both output rows before moving right to the next block. */
        for (unsigned block = 0; block < 160u; block++) {
            d0[0] = s0[0]; d0[1] = s0[1]; d0[2] = s0[3];
            d0[3] = s0[4]; d0[4] = s0[5]; d0[5] = s0[7];
            d0[6] = s0[6]; d0[7] = s0[5]; d0[8] = s0[7];
            d1[0] = s2[0]; d1[1] = s2[1]; d1[2] = s2[3];
            d1[3] = s2[4]; d1[4] = s2[5]; d1[5] = s2[7];
            d1[6] = s2[6]; d1[7] = s2[5]; d1[8] = s2[7];
            s0 += 8;
            s2 += 8;
            d0 += 9;
            d1 += 9;
        }
        source += 3840u;
        dst += 2880u;
    }
}