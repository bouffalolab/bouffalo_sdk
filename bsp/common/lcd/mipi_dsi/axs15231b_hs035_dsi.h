#ifndef __AXS15231B_HS035_DSI_H__
#define __AXS15231B_HS035_DSI_H__

#include "../lcd_conf.h"
#include "stdint.h"

#if defined(LCD_DSI_AXS15231B_HS035)

#include "mipi_dsi_v2.h"

/* HS035VB35PGR panel: AXS15231B, firmware-initialized, RGB565, 1-lane MIPI DSI */

#define AXS15231B_HS035_FB_MODE_RGB565 0
#define AXS15231B_HS035_FB_MODE_Y_UV_PLANAR 1

#ifndef AXS15231B_HS035_FB_MODE
#define AXS15231B_HS035_FB_MODE AXS15231B_HS035_FB_MODE_Y_UV_PLANAR
#endif

#if (AXS15231B_HS035_FB_MODE != AXS15231B_HS035_FB_MODE_RGB565) && \
    (AXS15231B_HS035_FB_MODE != AXS15231B_HS035_FB_MODE_Y_UV_PLANAR)
#error "AXS15231B_HS035_FB_MODE must be RGB565 or Y_UV_PLANAR"
#endif

/* OSD0 canvas format, independent of the DPI base format. */
#ifndef AXS15231B_HS035_OSD0_FORMAT
#define AXS15231B_HS035_OSD0_FORMAT MIPI_DSI_V2_OSD_FORMAT_ARGB8888
#endif

#if (AXS15231B_HS035_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_RGB565) && \
    (AXS15231B_HS035_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_ARGB8888) && \
    (AXS15231B_HS035_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_NONE)
#error "AXS15231B_HS035_OSD0_FORMAT must be RGB565, ARGB8888 or NONE"
#endif

/* Do not modify the following */


#define AXS15231B_HS035_DSI_W           172
#define AXS15231B_HS035_DSI_H           640
#if (AXS15231B_HS035_OSD0_FORMAT == MIPI_DSI_V2_OSD_FORMAT_RGB565) || \
    ((AXS15231B_HS035_OSD0_FORMAT == MIPI_DSI_V2_OSD_FORMAT_NONE) && \
     (AXS15231B_HS035_FB_MODE == AXS15231B_HS035_FB_MODE_RGB565))
#define AXS15231B_HS035_DSI_COLOR_DEPTH 16
typedef uint16_t axs15231b_hs035_dsi_color_t;
#else
#define AXS15231B_HS035_DSI_COLOR_DEPTH 32
typedef uint32_t axs15231b_hs035_dsi_color_t;
#endif

/* Turn 24-bit RGB color to 16-bit */
#define RGB(r, g, b) (((r >> 3) << 3 | (g >> 5) | (g >> 2) << 13 | (b >> 3) << 8) & 0xffff)
/* Calculate 32-bit or 16-bit absolute value */
#define ABS32(value) ((value ^ (value >> 31)) - (value >> 31))
#define ABS16(value) ((value ^ (value >> 15)) - (value >> 15))

int axs15231b_hs035_dsi_init(axs15231b_hs035_dsi_color_t *screen_buffer);
int axs15231b_hs035_dsi_screen_switch(axs15231b_hs035_dsi_color_t *screen_buffer);
axs15231b_hs035_dsi_color_t *axs15231b_hs035_dsi_get_screen_using(void);
int axs15231b_hs035_dsi_frame_callback_register(uint32_t callback_type, void (*callback)(void));
const mipi_dsi_v2_timing_t *axs15231b_hs035_dsi_get_timing(void);
int display_prepare(void);
int display_enable(void);
int display_disable(void);
int display_unprepare(void);

#endif

#endif /* __AXS15231B_HS035_DSI_H__ */
