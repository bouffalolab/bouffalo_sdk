#ifndef __JD9365TX_7KF82_DSI_H__
#define __JD9365TX_7KF82_DSI_H__

#include "../lcd_conf.h"
#include <stdint.h>

#if defined(LCD_DSI_JD9365TX_7KF82)

#include "mipi_dsi_v2.h"

#define JD9365TX_7KF82_FB_MODE_RGB565 0
#define JD9365TX_7KF82_FB_MODE_Y_UV_PLANAR 1

#ifndef JD9365TX_7KF82_FB_MODE
#define JD9365TX_7KF82_FB_MODE JD9365TX_7KF82_FB_MODE_RGB565
#endif

#if (JD9365TX_7KF82_FB_MODE != JD9365TX_7KF82_FB_MODE_RGB565) && \
    (JD9365TX_7KF82_FB_MODE != JD9365TX_7KF82_FB_MODE_Y_UV_PLANAR)
#error "JD9365TX_7KF82_FB_MODE must be RGB565 or Y_UV_PLANAR"
#endif

/* OSD0 canvas format, independent of the DPI base format. */
#ifndef JD9365TX_7KF82_OSD0_FORMAT
#define JD9365TX_7KF82_OSD0_FORMAT MIPI_DSI_V2_OSD_FORMAT_ARGB8888
#endif

#if (JD9365TX_7KF82_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_RGB565) && \
    (JD9365TX_7KF82_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_ARGB8888) && \
    (JD9365TX_7KF82_OSD0_FORMAT != MIPI_DSI_V2_OSD_FORMAT_NONE)
#error "JD9365TX_7KF82_OSD0_FORMAT must be RGB565, ARGB8888 or NONE"
#endif


/* JD9365TX 7KF82: 720x1280, 2-lane MIPI DSI. */
#define JD9365TX_7KF82_DSI_W           720
#define JD9365TX_7KF82_DSI_H           1280
#if (JD9365TX_7KF82_FB_MODE == JD9365TX_7KF82_FB_MODE_RGB565)
/* The panel exposes RGB565 scanout and OSD0 SEOF frame callbacks through the
 * public lcd_* API, which is required by cam_lcd_mipi. */
#define LCD_DSI_CAMERA_RGB565_FRAMEBUFFER_MODE 1
#endif

#if (JD9365TX_7KF82_OSD0_FORMAT == MIPI_DSI_V2_OSD_FORMAT_RGB565) || \
    ((JD9365TX_7KF82_OSD0_FORMAT == MIPI_DSI_V2_OSD_FORMAT_NONE) && \
     (JD9365TX_7KF82_FB_MODE == JD9365TX_7KF82_FB_MODE_RGB565))
#define JD9365TX_7KF82_DSI_COLOR_DEPTH 16
typedef uint16_t jd9365tx_7kf82_dsi_color_t;
#else
#define JD9365TX_7KF82_DSI_COLOR_DEPTH 32
typedef uint32_t jd9365tx_7kf82_dsi_color_t;
#endif

int jd9365tx_7kf82_dsi_init(jd9365tx_7kf82_dsi_color_t *screen_buffer);
int jd9365tx_7kf82_dsi_screen_switch(jd9365tx_7kf82_dsi_color_t *screen_buffer);
jd9365tx_7kf82_dsi_color_t *jd9365tx_7kf82_dsi_get_screen_using(void);
int jd9365tx_7kf82_dsi_frame_callback_register(uint32_t callback_type, void (*callback)(void));
const mipi_dsi_v2_timing_t *jd9365tx_7kf82_dsi_get_timing(void);

#endif

#endif /* __JD9365TX_7KF82_DSI_H__ */
