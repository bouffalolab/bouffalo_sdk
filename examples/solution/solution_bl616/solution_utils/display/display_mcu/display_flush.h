#ifndef _DISPLAY_FLUSH_H_
#define _DISPLAY_FLUSH_H_

#include <stdint.h>

/* dbi init */
void disp_dbi_init(void);

/* cpu, partition */
void yuyv422_640x480_to_yuv444_480x320_to_dbi_partition_fast(void *src, uint16_t y_start, uint16_t y_end);
void yuyv422_to_yuv444_dbi_to_dbi_partition_fast(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, void *src);

/* cpu */
void yuyv422_640x480_to_yuv444_480x320_to_dbi_fast(void *src, uint16_t y_start, uint16_t y_end);
void yuyv422_to_yuv444_to_dbi_fast(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, void *src);

/* dma */
void disp_flush_dma_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, void *picture, uint32_t pixel_format);

/* TE */
void disp_te_init(uint8_t pin);
void disp_te_delay(void);

#endif
