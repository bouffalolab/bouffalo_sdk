#ifndef __RGB_DRAW_H__
#define __RGB_DRAW_H__

#include "lcd.h"
#include "frame_queue.h"

void rgb888_640x480_to_rgb888_480x272(void *src, void *dst);
void rgb888_640x480_to_nrgb8888_480x272(void *src, void *dst);
void nrgb888_640x480_to_nrgb8888_480x272(void *src, void *dst);

void rgb_draw_overlay(lcd_color_t *dst_buf);

#endif /* __RGB_DRAW_H__ */