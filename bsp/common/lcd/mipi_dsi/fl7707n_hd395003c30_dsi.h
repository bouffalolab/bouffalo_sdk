#ifndef __FL7707N_HD395003C30_DSI_H__
#define __FL7707N_HD395003C30_DSI_H__

#include "../lcd_conf.h"
#include "stdint.h"

#if defined(LCD_DSI_FL7707N_HD395003C30)

#include "mipi_dsi_v2.h"

/* HD395003C30-V2 panel: FL7707N driver IC, 720x720, 2-lane MIPI DSI (RGB888 link).
 *
 * FL7707N is a DCS panel driven entirely through vendor registers (0xB0..0xEF)
 * written once after reset, followed by the standard 0x11 exit-sleep / 0x29
 * display-on pair. The 2-lane wiring is also declared to the panel itself: the
 * first parameter of 0xBA is 0x31, which is the IC's 2-lane setting (0x32 is
 * 3-lane, 0x33 is 4-lane), so the lane count is not configurable here the way it
 * is for EK79007.
 *
 * Framebuffer format (FL7707N_HD395003C30_FB_MODE) is a board/application
 * property, so it is configured in lcd_conf_user.h (default in lcd_conf.h) rather
 * than hard-coded here -- same arrangement as EK79007_WKS70WSV114_FB_MODE.
 */

#define FL7707N_HD395003C30_FB_MODE_RGB565 0
#define FL7707N_HD395003C30_FB_MODE_Y_UV_PLANAR 1

/* An lcd_conf_user.h that predates this panel leaves the framebuffer mode unset;
 * fall back to the ARGB8888 OSD path, which is what the LVGL ports expect. */
#ifndef FL7707N_HD395003C30_FB_MODE
#define FL7707N_HD395003C30_FB_MODE FL7707N_HD395003C30_FB_MODE_Y_UV_PLANAR
#endif

#if (FL7707N_HD395003C30_FB_MODE != FL7707N_HD395003C30_FB_MODE_RGB565) && \
    (FL7707N_HD395003C30_FB_MODE != FL7707N_HD395003C30_FB_MODE_Y_UV_PLANAR)
#error "FL7707N_HD395003C30_FB_MODE must be RGB565 or Y_UV_PLANAR"
#endif

/* OSD0 canvas format, independent of the DPI base format. */
#ifndef FL7707N_HD395003C30_OSD0_FORMAT
#define FL7707N_HD395003C30_OSD0_FORMAT MIPI_DSI_V2_OSD_FORMAT_ARGB8888
#endif

#if (FL7707N_HD395003C30_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_RGB565) && \
    (FL7707N_HD395003C30_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_ARGB8888) && \
    (FL7707N_HD395003C30_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_NONE)
#error "FL7707N_HD395003C30_OSD0_FORMAT must be RGB565, ARGB8888 or NONE"
#endif

/* Do not modify the following */

#define FL7707N_HD395003C30_DSI_W 720
#define FL7707N_HD395003C30_DSI_H 720

#if (FL7707N_HD395003C30_OSD0_FORMAT == MIPI_DSI_V2_OSD_FORMAT_RGB565) || \
    ((FL7707N_HD395003C30_OSD0_FORMAT == MIPI_DSI_V2_OSD_FORMAT_NONE) && \
     (FL7707N_HD395003C30_FB_MODE == FL7707N_HD395003C30_FB_MODE_RGB565))
#define FL7707N_HD395003C30_DSI_COLOR_DEPTH 16
typedef uint16_t fl7707n_hd395003c30_dsi_color_t;
#else
#define FL7707N_HD395003C30_DSI_COLOR_DEPTH 32
typedef uint32_t fl7707n_hd395003c30_dsi_color_t;
#endif

/* Turn 24-bit RGB color to 16-bit */
#define RGB(r, g, b) (((r >> 3) << 3 | (g >> 5) | (g >> 2) << 13 | (b >> 3) << 8) & 0xffff)
/* Calculate 32-bit or 16-bit absolute value */
#define ABS32(value) ((value ^ (value >> 31)) - (value >> 31))
#define ABS16(value) ((value ^ (value >> 15)) - (value >> 15))

int fl7707n_hd395003c30_dsi_init(fl7707n_hd395003c30_dsi_color_t *screen_buffer);
int fl7707n_hd395003c30_dsi_screen_switch(fl7707n_hd395003c30_dsi_color_t *screen_buffer);
fl7707n_hd395003c30_dsi_color_t *fl7707n_hd395003c30_dsi_get_screen_using(void);
int fl7707n_hd395003c30_dsi_frame_callback_register(uint32_t callback_type, void (*callback)(void));
const mipi_dsi_v2_timing_t *fl7707n_hd395003c30_dsi_get_timing(void);
int display_prepare(void);
int display_enable(void);
int display_disable(void);
int display_unprepare(void);

#endif

#endif /* __FL7707N_HD395003C30_DSI_H__ */
