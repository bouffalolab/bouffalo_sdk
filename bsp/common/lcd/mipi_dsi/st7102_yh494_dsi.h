#ifndef __ST7102_YH494_DSI_H__
#define __ST7102_YH494_DSI_H__

#include "../lcd_conf.h"
#include "stdint.h"

#if defined(LCD_DSI_ST7102_YH494)

#include "mipi_dsi_v2.h"

/* YH-494BSAC002N1 panel: ST7102_YH494 driver IC, 480x960 portrait, RGB565, 2-lane MIPI DSI */

/* Do not modify the following */

#define ST7102_YH494_DSI_W           480
#define ST7102_YH494_DSI_H           960
#define ST7102_YH494_DSI_COLOR_DEPTH 32

typedef uint32_t st7102_yh494_dsi_color_t;

/* Turn 24-bit RGB color to 16-bit */
#define RGB(r, g, b) (((r >> 3) << 3 | (g >> 5) | (g >> 2) << 13 | (b >> 3) << 8) & 0xffff)
/* Calculate 32-bit or 16-bit absolute value */
#define ABS32(value) ((value ^ (value >> 31)) - (value >> 31))
#define ABS16(value) ((value ^ (value >> 15)) - (value >> 15))

int st7102_yh494_dsi_init(st7102_yh494_dsi_color_t *screen_buffer);
int st7102_yh494_dsi_screen_switch(st7102_yh494_dsi_color_t *screen_buffer);
st7102_yh494_dsi_color_t *st7102_yh494_dsi_get_screen_using(void);
int st7102_yh494_dsi_frame_callback_register(uint32_t callback_type, void (*callback)(void));
const mipi_dsi_v2_timing_t *st7102_yh494_dsi_get_timing(void);
int display_prepare(void);
int display_enable(void);
int display_disable(void);
int display_unprepare(void);

#endif

#endif /* __ST7102_YH494_DSI_H__ */
