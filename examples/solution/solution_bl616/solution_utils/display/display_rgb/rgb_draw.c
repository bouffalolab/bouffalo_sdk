#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "lcd.h"

#include "frame_queue.h"

#include "rgb_disp.h"
#include "solution.h"

#include "rgb_draw.h"

void rgb888_640x480_to_rgb888_480x272(void *src, void *dst)
{
    uint16_t by;
    uint16_t bx;
    uint8_t *src_buf = (uint8_t *)src;
    uint8_t *dst_buf = (uint8_t *)dst;

    if (src == NULL || dst == NULL) {
        return;
    }

    /* 640x480 -> 480x272, point-sampling: 4x4 keep 3x3, drop extra bottom lines */
    for (by = 0; by < 90; by++) {
        uint8_t *src_line0 = src_buf + ((uint32_t)by * 4 * 640 * 3);
        uint8_t *src_line1 = src_line0 + (640 * 3);
        uint8_t *src_line2 = src_line1 + (640 * 3);
        uint8_t *dst_line0 = dst_buf + ((uint32_t)by * 3 * 480 * 3);
        uint8_t *dst_line1 = dst_line0 + (480 * 3);
        uint8_t *dst_line2 = dst_line1 + (480 * 3);

        for (bx = 0; bx < 160; bx++) {
            uint8_t *src_pix0 = src_line0 + ((uint32_t)bx * 4 * 3);
            uint8_t *src_pix1 = src_line1 + ((uint32_t)bx * 4 * 3);
            uint8_t *src_pix2 = src_line2 + ((uint32_t)bx * 4 * 3);
            uint8_t *dst_pix0 = dst_line0 + ((uint32_t)bx * 3 * 3);
            uint8_t *dst_pix1 = dst_line1 + ((uint32_t)bx * 3 * 3);
            uint8_t *dst_pix2 = dst_line2 + ((uint32_t)bx * 3 * 3);

            memcpy(dst_pix0, src_pix0, 9);
            memcpy(dst_pix1, src_pix1, 9);
            memcpy(dst_pix2, src_pix2, 9);
        }
    }

    /* remain 2 lines: clear to 0 */
    memset(dst_buf + ((uint32_t)270 * 480 * 3), 0, (uint32_t)(2 * 480 * 3));
}

void rgb888_640x480_to_nrgb8888_480x272(void *src, void *dst)
{
    uint16_t by;
    uint16_t bx;
    uint8_t *src_buf = (uint8_t *)src;
    uint32_t *dst_buf = (uint32_t *)dst;

    if (src == NULL || dst == NULL) {
        return;
    }

    /* 640x480 -> 480x272, point-sampling: 4x4 keep 3x3, drop extra bottom lines */
    for (by = 0; by < 90; by++) {
        uint8_t *src_line0 = src_buf + ((uint32_t)by * 4 * 640 * 3);
        uint8_t *src_line1 = src_line0 + (640 * 3);
        uint8_t *src_line2 = src_line1 + (640 * 3);
        uint32_t *dst_line0 = dst_buf + ((uint32_t)by * 3 * 480);
        uint32_t *dst_line1 = dst_line0 + 480;
        uint32_t *dst_line2 = dst_line1 + 480;

        for (bx = 0; bx < 160; bx++) {
            uint32_t dx = (uint32_t)bx * 3;
            uint8_t *src_pix0 = src_line0 + ((uint32_t)bx * 4 * 3);
            uint8_t *src_pix1 = src_line1 + ((uint32_t)bx * 4 * 3);
            uint8_t *src_pix2 = src_line2 + ((uint32_t)bx * 4 * 3);

            dst_line0[dx + 0] = ((uint32_t)src_pix0[0] << 16) | ((uint32_t)src_pix0[1] << 8) | src_pix0[2];
            dst_line0[dx + 1] = ((uint32_t)src_pix0[3] << 16) | ((uint32_t)src_pix0[4] << 8) | src_pix0[5];
            dst_line0[dx + 2] = ((uint32_t)src_pix0[6] << 16) | ((uint32_t)src_pix0[7] << 8) | src_pix0[8];

            dst_line1[dx + 0] = ((uint32_t)src_pix1[0] << 16) | ((uint32_t)src_pix1[1] << 8) | src_pix1[2];
            dst_line1[dx + 1] = ((uint32_t)src_pix1[3] << 16) | ((uint32_t)src_pix1[4] << 8) | src_pix1[5];
            dst_line1[dx + 2] = ((uint32_t)src_pix1[6] << 16) | ((uint32_t)src_pix1[7] << 8) | src_pix1[8];

            dst_line2[dx + 0] = ((uint32_t)src_pix2[0] << 16) | ((uint32_t)src_pix2[1] << 8) | src_pix2[2];
            dst_line2[dx + 1] = ((uint32_t)src_pix2[3] << 16) | ((uint32_t)src_pix2[4] << 8) | src_pix2[5];
            dst_line2[dx + 2] = ((uint32_t)src_pix2[6] << 16) | ((uint32_t)src_pix2[7] << 8) | src_pix2[8];
        }
    }

    /* remain 2 lines: clear to 0 */
    memset(dst_buf + ((uint32_t)270 * 480), 0, (uint32_t)(2 * 480 * sizeof(uint32_t)));
}

void nrgb888_640x480_to_nrgb8888_480x272(void *src, void *dst)
{
    uint16_t by;
    uint16_t bx;
    uint32_t *src_buf = (uint32_t *)src;
    uint32_t *dst_buf = (uint32_t *)dst;

    if (src == NULL || dst == NULL) {
        return;
    }

    /* 640x480 -> 480x272, point-sampling: 4x4 keep 3x3, drop extra bottom lines */
    for (by = 0; by < 90; by++) {
        uint32_t *src_line0 = src_buf + ((uint32_t)by * 4 * 640);
        uint32_t *src_line1 = src_line0 + 640;
        uint32_t *src_line2 = src_line1 + 640;
        uint32_t *dst_line0 = dst_buf + ((uint32_t)by * 3 * 480);
        uint32_t *dst_line1 = dst_line0 + 480;
        uint32_t *dst_line2 = dst_line1 + 480;

        for (bx = 0; bx < 160; bx++) {
            uint32_t sx = (uint32_t)bx * 4;
            uint32_t dx = (uint32_t)bx * 3;

            dst_line0[dx + 0] = src_line0[sx + 0];
            dst_line0[dx + 1] = src_line0[sx + 1];
            dst_line0[dx + 2] = src_line0[sx + 2];

            dst_line1[dx + 0] = src_line1[sx + 0];
            dst_line1[dx + 1] = src_line1[sx + 1];
            dst_line1[dx + 2] = src_line1[sx + 2];

            dst_line2[dx + 0] = src_line2[sx + 0];
            dst_line2[dx + 1] = src_line2[sx + 1];
            dst_line2[dx + 2] = src_line2[sx + 2];
        }
    }

    /* remain 2 lines: clear to 0 */
    memset(dst_buf + ((uint32_t)270 * 480), 0, (uint32_t)(2 * 480 * sizeof(uint32_t)));
}



void rgb_draw_overlay(lcd_color_t *dst_buf)
{
#if LCD_INFO_DISP_ENABLE && FONT_ASCII_16X8
    int ret;
    char str_buff_total[128];
    uint16_t str_y;

    ret = solution_fps_str_get(str_buff_total, sizeof(str_buff_total));
    if (ret < 0) {
        snprintf(str_buff_total, sizeof(str_buff_total), "FPS: get failed");
        ret = strlen(str_buff_total);
    }

    if ((ret + 1) * 8 < LCD_W) {
        str_y = (LCD_H > 20) ? (LCD_H - 20) : 0;
    } else {
        str_y = (LCD_H > 40) ? (LCD_H - 40) : 0;
    }

    lcd_draw_str_ascii16(dst_buf, 4, str_y,
                         LCD_COLOR_RGB(0xff, 0x80, 0x80),
                         LCD_COLOR_RGB(0x40, 0x40, 0x40),
                         (void *)str_buff_total, (uint8_t)ret);
#else
    (void)dst_buf;
#endif
}