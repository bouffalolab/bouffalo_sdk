#ifndef __ILI9806E_KD050FWFIA019_DSI_H__
#define __ILI9806E_KD050FWFIA019_DSI_H__

#include "../lcd_conf.h"
#include "stdint.h"

#if defined(LCD_DSI_ILI9806E_KD050FWFIA019)

#include "mipi_dsi_v2.h"

/* KD050FWFIA019 panel: 480x854, ILI9806E driver IC, 2-lane MIPI DSI (RGB565) */

/* Panel orientation (ILI9806E_KD050FWFIA019_ROTATE_180) and framebuffer format
 * (ILI9806E_KD050FWFIA019_FB_MODE) are board/application properties, so they are
 * configured in lcd_conf_user.h (defaults in lcd_conf.h) rather than hard-coded
 * here -- same arrangement as EK79007_WKS70WSV114_FB_MODE. Consumed by
 * ili9806e_kd050fwfia019_dsi.c via #if. */

#define ILI9806E_KD050FWFIA019_FB_MODE_RGB565 0
#define ILI9806E_KD050FWFIA019_FB_MODE_Y_UV_PLANAR 1

/* An lcd_conf_user.h that predates this option leaves the framebuffer mode unset;
 * fall back to RGB565, the panel's default scanout format. */
#ifndef ILI9806E_KD050FWFIA019_FB_MODE
#define ILI9806E_KD050FWFIA019_FB_MODE ILI9806E_KD050FWFIA019_FB_MODE_RGB565
#endif

#if (ILI9806E_KD050FWFIA019_FB_MODE != ILI9806E_KD050FWFIA019_FB_MODE_RGB565) && \
    (ILI9806E_KD050FWFIA019_FB_MODE != ILI9806E_KD050FWFIA019_FB_MODE_Y_UV_PLANAR)
#error "ILI9806E_KD050FWFIA019_FB_MODE must be RGB565 or Y_UV_PLANAR"
#endif

/* OSD0 canvas format, independent of the DPI base format. */
#ifndef ILI9806E_KD050FWFIA019_OSD0_FORMAT
#define ILI9806E_KD050FWFIA019_OSD0_FORMAT MIPI_DSI_V2_OSD_FORMAT_ARGB8888
#endif

#if (ILI9806E_KD050FWFIA019_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_RGB565) && \
    (ILI9806E_KD050FWFIA019_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_ARGB8888) && \
    (ILI9806E_KD050FWFIA019_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_NONE)
#error "ILI9806E_KD050FWFIA019_OSD0_FORMAT must be RGB565, ARGB8888 or NONE"
#endif

/* Do not modify the following */

#define ILI9806E_KD050FWFIA019_DSI_W 480
#define ILI9806E_KD050FWFIA019_DSI_H 854

#if (ILI9806E_KD050FWFIA019_OSD0_FORMAT == MIPI_DSI_V2_OSD_FORMAT_RGB565) || \
    ((ILI9806E_KD050FWFIA019_OSD0_FORMAT == MIPI_DSI_V2_OSD_FORMAT_NONE) && \
     (ILI9806E_KD050FWFIA019_FB_MODE == ILI9806E_KD050FWFIA019_FB_MODE_RGB565))
#define ILI9806E_KD050FWFIA019_DSI_COLOR_DEPTH 16
typedef uint16_t ili9806e_kd050fwfia019_dsi_color_t;
#else
#define ILI9806E_KD050FWFIA019_DSI_COLOR_DEPTH 32
typedef uint32_t ili9806e_kd050fwfia019_dsi_color_t;
#endif

/* Turn 24-bit RGB color to 16-bit */
#define RGB(r, g, b) (((r >> 3) << 3 | (g >> 5) | (g >> 2) << 13 | (b >> 3) << 8) & 0xffff)
/* Calculate 32-bit or 16-bit absolute value */
#define ABS32(value) ((value ^ (value >> 31)) - (value >> 31))
#define ABS16(value) ((value ^ (value >> 15)) - (value >> 15))

int ili9806e_kd050fwfia019_dsi_init(ili9806e_kd050fwfia019_dsi_color_t *screen_buffer);
int ili9806e_kd050fwfia019_dsi_screen_switch(ili9806e_kd050fwfia019_dsi_color_t *screen_buffer);
ili9806e_kd050fwfia019_dsi_color_t *ili9806e_kd050fwfia019_dsi_get_screen_using(void);
int ili9806e_kd050fwfia019_dsi_frame_callback_register(uint32_t callback_type, void (*callback)(void));
const mipi_dsi_v2_timing_t *ili9806e_kd050fwfia019_dsi_get_timing(void);
int display_prepare(void);
int display_enable(void);
int display_disable(void);
int display_unprepare(void);

#endif

#endif /* __ILI9806E_KD050FWFIA019_DSI_H__ */
