#ifndef __EK79007_WKS70WSV114_DSI_H__
#define __EK79007_WKS70WSV114_DSI_H__

#include "../lcd_conf.h"
#include "stdint.h"

#if defined(LCD_DSI_EK79007_WKS70WSV114)

#include "mipi_dsi_v2.h"

/* WKS70WSV114 panel: 1024x600 landscape, EK79007 driver IC, MIPI DSI (RGB888 link).
 *
 * EK79007 is a DSI-to-RGB bridge: it has no frame memory and no DCS sleep/display
 * on-off registers, only the 0x80..0x86 / 0xB1 / 0xB2 vendor configuration
 * registers written once after reset. The DPI controller then scans out
 * continuously in HS video mode, exactly like the other DSI v2 panels.
 *
 * Lane count (EK79007_WKS70WSV114_LANE_NUM) is a panel-wiring property, so it is
 * configured in lcd_conf_user.h (default in lcd_conf.h) rather than hard-coded
 * here. It selects both the DSI lane configuration and the matching vendor init
 * table: the 2-lane variant writes 0xB2 = 0x10, the 4-lane variant omits it.
 */

/* An lcd_conf_user.h that predates this panel leaves the lane count unset; fall
 * back to the 2-lane wiring rather than silently building a half-configured
 * link (the init table keys off the exact value, not just "not 4"). */
#ifndef EK79007_WKS70WSV114_LANE_NUM
#define EK79007_WKS70WSV114_LANE_NUM 2
#endif

#if (EK79007_WKS70WSV114_LANE_NUM != 2) && (EK79007_WKS70WSV114_LANE_NUM != 4)
#error "EK79007_WKS70WSV114_LANE_NUM must be 2 or 4"
#endif

#define EK79007_WKS70WSV114_FB_MODE_RGB565 0
#define EK79007_WKS70WSV114_FB_MODE_Y_UV_PLANAR 1

#ifndef EK79007_WKS70WSV114_FB_MODE
#define EK79007_WKS70WSV114_FB_MODE EK79007_WKS70WSV114_FB_MODE_Y_UV_PLANAR
#endif

#if (EK79007_WKS70WSV114_FB_MODE != EK79007_WKS70WSV114_FB_MODE_RGB565) && \
    (EK79007_WKS70WSV114_FB_MODE != EK79007_WKS70WSV114_FB_MODE_Y_UV_PLANAR)
#error "EK79007_WKS70WSV114_FB_MODE must be RGB565 or Y_UV_PLANAR"
#endif

/* OSD0 canvas format, independent of the DPI base format. */
#ifndef EK79007_WKS70WSV114_OSD0_FORMAT
#define EK79007_WKS70WSV114_OSD0_FORMAT MIPI_DSI_V2_OSD_FORMAT_ARGB8888
#endif

#if (EK79007_WKS70WSV114_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_RGB565) && \
    (EK79007_WKS70WSV114_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_ARGB8888) && \
    (EK79007_WKS70WSV114_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_NONE)
#error "EK79007_WKS70WSV114_OSD0_FORMAT must be RGB565, ARGB8888 or NONE"
#endif

/* Do not modify the following */

#define EK79007_WKS70WSV114_DSI_W 1024
#define EK79007_WKS70WSV114_DSI_H 600

#if (EK79007_WKS70WSV114_OSD0_FORMAT == MIPI_DSI_V2_OSD_FORMAT_RGB565) || \
    ((EK79007_WKS70WSV114_OSD0_FORMAT == MIPI_DSI_V2_OSD_FORMAT_NONE) && \
     (EK79007_WKS70WSV114_FB_MODE == EK79007_WKS70WSV114_FB_MODE_RGB565))
#define EK79007_WKS70WSV114_DSI_COLOR_DEPTH 16
typedef uint16_t ek79007_wks70wsv114_dsi_color_t;
#else
#define EK79007_WKS70WSV114_DSI_COLOR_DEPTH 32
typedef uint32_t ek79007_wks70wsv114_dsi_color_t;
#endif

/* Turn 24-bit RGB color to 16-bit */
#define RGB(r, g, b) (((r >> 3) << 3 | (g >> 5) | (g >> 2) << 13 | (b >> 3) << 8) & 0xffff)
/* Calculate 32-bit or 16-bit absolute value */
#define ABS32(value) ((value ^ (value >> 31)) - (value >> 31))
#define ABS16(value) ((value ^ (value >> 15)) - (value >> 15))

int ek79007_wks70wsv114_dsi_init(ek79007_wks70wsv114_dsi_color_t *screen_buffer);
int ek79007_wks70wsv114_dsi_screen_switch(ek79007_wks70wsv114_dsi_color_t *screen_buffer);
ek79007_wks70wsv114_dsi_color_t *ek79007_wks70wsv114_dsi_get_screen_using(void);
int ek79007_wks70wsv114_dsi_frame_callback_register(uint32_t callback_type, void (*callback)(void));
const mipi_dsi_v2_timing_t *ek79007_wks70wsv114_dsi_get_timing(void);
int display_prepare(void);
int display_enable(void);
int display_disable(void);
int display_unprepare(void);

#endif

#endif /* __EK79007_WKS70WSV114_DSI_H__ */
