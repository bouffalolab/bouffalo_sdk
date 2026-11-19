#ifndef __RGB_DRAW_H__
#define __RGB_DRAW_H__

#include <stdint.h>

void soln_rgb888_640x480_to_rgb888_480x272(void *src, void *dst);
void soln_rgb888_640x480_to_nrgb8888_480x272(void *src, void *dst);
void soln_nrgb888_640x480_to_nrgb8888_480x272(void *src, void *dst);

int soln_rgb_draw_overlay(uint32_t *dst_buf, uint16_t width, uint16_t height);

#endif /* __RGB_DRAW_H__ */