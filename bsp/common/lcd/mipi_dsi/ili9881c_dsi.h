#ifndef __ILI9881C_DSI_H__
#define __ILI9881C_DSI_H__

#include "../lcd_conf.h"
#include "stdint.h"

#if defined(LCD_DSI_ILI9881C)

/* Do not modify the following */

#define ILI9881C_DSI_COLOR_DEPTH 16

typedef uint16_t ili9881c_dsi_color_t;

#define DSI_LANE_NUMBER_USE    2
#define DSI_ILI9881C_BSIT_MODE 0
/* Turn 24-bit RGB color to 16-bit */
#define RGB(r, g, b) (((r >> 3) << 3 | (g >> 5) | (g >> 2) << 13 | (b >> 3) << 8) & 0xffff)
/* Calculate 32-bit or 16-bit absolute value */
#define ABS32(value) ((value ^ (value >> 31)) - (value >> 31))
#define ABS16(value) ((value ^ (value >> 15)) - (value >> 15))

int ili9881c_dsi_init(ili9881c_dsi_color_t *screen_buffer);
int ili9881c_dsi_screen_switch(ili9881c_dsi_color_t *screen_buffer);
ili9881c_dsi_color_t *ili9881c_dsi_get_screen_using(void);
int ili9881c_dsi_frame_callback_register(uint32_t callback_type, void (*callback)(void));
int display_prepare(void);
int display_enable(void);
int display_disable(void);
int display_unprepare(void);

#endif

#endif /* __ILI9881C_DSI_H__ */
