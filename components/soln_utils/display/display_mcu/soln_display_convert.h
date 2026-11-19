#ifndef SOLN_DISPLAY_CONVERT_H
#define SOLN_DISPLAY_CONVERT_H

#include <stdint.h>

/* Pure memory conversion of non-overlapping buffers. src is the complete packed
 * YUYV frame; y/rows select output rows, dst starts at band row zero.
 * Caller validates even width, dimensions and capacity. Neither function touches DBI. */
void soln_yuyv422_to_yuv444_band(uint8_t *dst, const uint8_t *src, uint16_t width,
                               uint16_t y, uint16_t rows);

/* Source 4 columns x 3 rows -> output 3 columns x 2 rows (width 480).
 * Block-local source (x,y): top (0,0),(2,0),(3,0), bottom (0,2),(2,2),(3,2).
 * Each pixel keeps its source YUYV pair's U/V; no interpolation is performed.
 * Caller supplies even y and rows, with y + rows <= 320 (normally 16-row bands).
 * y is the global output row; partial two-row blocks are not supported. */
void soln_yuyv422_640x480_to_yuv444_480x320_band(uint8_t *dst, const uint8_t *src,
                                             uint16_t y, uint16_t rows);

#endif