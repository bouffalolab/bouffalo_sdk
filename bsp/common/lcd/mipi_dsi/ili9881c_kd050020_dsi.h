#ifndef __ILI9881C_KD050020_DSI_H__
#define __ILI9881C_KD050020_DSI_H__

#include "../lcd_conf.h"
#include "stdint.h"

#if defined(LCD_DSI_ILI9881C_KD050020)

#include "mipi_dsi_v2.h"

/* KD050HDFIA020-B002 panel: 720x1280, ILI9881C, 4-lane MIPI DSI (RGB565) */

#define ILI9881C_KD050020_FB_MODE_RGB565 0
#define ILI9881C_KD050020_FB_MODE_Y_UV_PLANAR 1

#ifndef ILI9881C_KD050020_FB_MODE
#define ILI9881C_KD050020_FB_MODE ILI9881C_KD050020_FB_MODE_Y_UV_PLANAR
#endif

#if (ILI9881C_KD050020_FB_MODE != ILI9881C_KD050020_FB_MODE_RGB565) && \
    (ILI9881C_KD050020_FB_MODE != ILI9881C_KD050020_FB_MODE_Y_UV_PLANAR)
#error "ILI9881C_KD050020_FB_MODE must be RGB565 or Y_UV_PLANAR"
#endif

/* OSD0 canvas format, independent of the DPI base format. */
#ifndef ILI9881C_KD050020_OSD0_FORMAT
#define ILI9881C_KD050020_OSD0_FORMAT MIPI_DSI_V2_OSD_FORMAT_ARGB8888
#endif

#if (ILI9881C_KD050020_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_RGB565) && \
    (ILI9881C_KD050020_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_ARGB8888) && \
    (ILI9881C_KD050020_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_NONE)
#error "ILI9881C_KD050020_OSD0_FORMAT must be RGB565, ARGB8888 or NONE"
#endif

/* Do not modify the following */


#define ILI9881C_KD050020_DSI_W           720
#define ILI9881C_KD050020_DSI_H           1280
#if (ILI9881C_KD050020_OSD0_FORMAT == MIPI_DSI_V2_OSD_FORMAT_RGB565) || \
    ((ILI9881C_KD050020_OSD0_FORMAT == MIPI_DSI_V2_OSD_FORMAT_NONE) && \
     (ILI9881C_KD050020_FB_MODE == ILI9881C_KD050020_FB_MODE_RGB565))
#define ILI9881C_KD050020_DSI_COLOR_DEPTH 16
typedef uint16_t ili9881c_kd050020_dsi_color_t;
#else
#define ILI9881C_KD050020_DSI_COLOR_DEPTH 32
typedef uint32_t ili9881c_kd050020_dsi_color_t;
#endif

/* Turn 24-bit RGB color to 16-bit */
#define RGB(r, g, b) (((r >> 3) << 3 | (g >> 5) | (g >> 2) << 13 | (b >> 3) << 8) & 0xffff)
/* Calculate 32-bit or 16-bit absolute value */
#define ABS32(value) ((value ^ (value >> 31)) - (value >> 31))
#define ABS16(value) ((value ^ (value >> 15)) - (value >> 15))

int ili9881c_kd050020_dsi_init(ili9881c_kd050020_dsi_color_t *screen_buffer);
int ili9881c_kd050020_dsi_screen_switch(ili9881c_kd050020_dsi_color_t *screen_buffer);
ili9881c_kd050020_dsi_color_t *ili9881c_kd050020_dsi_get_screen_using(void);
int ili9881c_kd050020_dsi_frame_callback_register(uint32_t callback_type, void (*callback)(void));
const mipi_dsi_v2_timing_t *ili9881c_kd050020_dsi_get_timing(void);
int display_prepare(void);
int display_enable(void);
int display_disable(void);
int display_unprepare(void);

#endif

#endif /* __ILI9881C_KD050020_DSI_H__ */
