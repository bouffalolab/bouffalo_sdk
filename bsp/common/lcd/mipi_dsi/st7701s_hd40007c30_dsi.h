#ifndef __ST7701S_HD40007C30_DSI_H__
#define __ST7701S_HD40007C30_DSI_H__

#include "../lcd_conf.h"
#include "stdint.h"

#if defined(LCD_DSI_ST7701S_HD40007C30)

#include "mipi_dsi_v2.h"

/* HD40007C30-V5 panel: ST7701S driver IC, 480x480, 2-lane MIPI DSI (RGB888 link).
 *
 * ST7701S is a DCS panel with a Command2 bank-switch scheme: register 0xFF takes
 * the magic 0x77 0x01 0x00 0x00 <bank> and selects which of BK0/BK1/BK3 the
 * following writes land in. The init table below therefore reads as a sequence of
 * bank switches, exactly like the vendor script it came from.
 *
 * Framebuffer format (ST7701S_HD40007C30_FB_MODE) is a board/application
 * property, so it is configured in lcd_conf_user.h (default in lcd_conf.h) rather
 * than hard-coded here -- same arrangement as EK79007_WKS70WSV114_FB_MODE.
 */

#define ST7701S_HD40007C30_FB_MODE_RGB565 0
#define ST7701S_HD40007C30_FB_MODE_Y_UV_PLANAR 1

/* RGB-BGR order control for DCS command 0x36:
 *   0: output R-G-B
 *   1: output B-G-R
 */
#ifndef ST7701S_HD40007C30_RGB_ORDER_MODE
#define ST7701S_HD40007C30_RGB_ORDER_MODE 0
#endif

#if (ST7701S_HD40007C30_RGB_ORDER_MODE != 0) && \
    (ST7701S_HD40007C30_RGB_ORDER_MODE != 1)
#error "ST7701S_HD40007C30_RGB_ORDER_MODE must be 0 (RGB) or 1 (BGR)"
#endif

/* An lcd_conf_user.h that predates this panel leaves the framebuffer mode unset;
 * fall back to the ARGB8888 OSD path, which is what the LVGL ports expect. */
#ifndef ST7701S_HD40007C30_FB_MODE
#define ST7701S_HD40007C30_FB_MODE ST7701S_HD40007C30_FB_MODE_Y_UV_PLANAR
#endif

#if (ST7701S_HD40007C30_FB_MODE != ST7701S_HD40007C30_FB_MODE_RGB565) && \
    (ST7701S_HD40007C30_FB_MODE != ST7701S_HD40007C30_FB_MODE_Y_UV_PLANAR)
#error "ST7701S_HD40007C30_FB_MODE must be RGB565 or Y_UV_PLANAR"
#endif

/* OSD0 canvas format, independent of the DPI base format. */
#ifndef ST7701S_HD40007C30_OSD0_FORMAT
#define ST7701S_HD40007C30_OSD0_FORMAT MIPI_DSI_V2_OSD_FORMAT_ARGB8888
#endif

#if (ST7701S_HD40007C30_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_RGB565) && \
    (ST7701S_HD40007C30_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_ARGB8888) && \
    (ST7701S_HD40007C30_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_NONE)
#error "ST7701S_HD40007C30_OSD0_FORMAT must be RGB565, ARGB8888 or NONE"
#endif

/* Do not modify the following */

#define ST7701S_HD40007C30_DSI_W 480
#define ST7701S_HD40007C30_DSI_H 480

#if (ST7701S_HD40007C30_OSD0_FORMAT == MIPI_DSI_V2_OSD_FORMAT_RGB565) || \
    ((ST7701S_HD40007C30_OSD0_FORMAT == MIPI_DSI_V2_OSD_FORMAT_NONE) && \
     (ST7701S_HD40007C30_FB_MODE == ST7701S_HD40007C30_FB_MODE_RGB565))
#define ST7701S_HD40007C30_DSI_COLOR_DEPTH 16
typedef uint16_t st7701s_hd40007c30_dsi_color_t;
#else
#define ST7701S_HD40007C30_DSI_COLOR_DEPTH 32
typedef uint32_t st7701s_hd40007c30_dsi_color_t;
#endif

/* Turn 24-bit RGB color to 16-bit */
#define RGB(r, g, b) (((r >> 3) << 3 | (g >> 5) | (g >> 2) << 13 | (b >> 3) << 8) & 0xffff)
/* Calculate 32-bit or 16-bit absolute value */
#define ABS32(value) ((value ^ (value >> 31)) - (value >> 31))
#define ABS16(value) ((value ^ (value >> 15)) - (value >> 15))

int st7701s_hd40007c30_dsi_init(st7701s_hd40007c30_dsi_color_t *screen_buffer);
int st7701s_hd40007c30_dsi_screen_switch(st7701s_hd40007c30_dsi_color_t *screen_buffer);
st7701s_hd40007c30_dsi_color_t *st7701s_hd40007c30_dsi_get_screen_using(void);
int st7701s_hd40007c30_dsi_frame_callback_register(uint32_t callback_type, void (*callback)(void));
const mipi_dsi_v2_timing_t *st7701s_hd40007c30_dsi_get_timing(void);
int display_prepare(void);
int display_enable(void);
int display_disable(void);
int display_unprepare(void);

#endif

#endif /* __ST7701S_HD40007C30_DSI_H__ */
