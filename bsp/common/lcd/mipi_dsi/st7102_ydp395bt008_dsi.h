#ifndef ST7102_YDP395BT008_DSI_DRIVER_H
#define ST7102_YDP395BT008_DSI_DRIVER_H

#include "../lcd_conf.h"
#include <stdint.h>

#if defined(LCD_DSI_ST7102_YDP395BT008)

#include "mipi_dsi_v2.h"

#define ST7102_YDP395BT008_DSI_W 480
#define ST7102_YDP395BT008_DSI_H 480
#define ST7102_YDP395BT008_FB_MODE_RGB565 0
#define ST7102_YDP395BT008_FB_MODE_ARGB8888 1

/* Retain the profile selector for existing lcd_conf_user.h files.
 * Only the YDP395BT008-V1 vendor profile is supported by this driver. */
#define ST7102_YDP395BT008_INIT_OSPTEK 2

#ifndef ST7102_YDP395BT008_INIT_PROFILE
#define ST7102_YDP395BT008_INIT_PROFILE ST7102_YDP395BT008_INIT_OSPTEK
#endif

#if ST7102_YDP395BT008_INIT_PROFILE != ST7102_YDP395BT008_INIT_OSPTEK
#error "Only the OSPTEK YDP395BT008-V1 initialization profile is supported"
#endif

/* Framebuffer format in memory; the two-lane DSI link always uses RGB888. */
#ifndef ST7102_YDP395BT008_FB_MODE
#define ST7102_YDP395BT008_FB_MODE ST7102_YDP395BT008_FB_MODE_ARGB8888
#endif

#if ST7102_YDP395BT008_FB_MODE == ST7102_YDP395BT008_FB_MODE_RGB565
#define ST7102_YDP395BT008_DSI_COLOR_DEPTH 16
typedef uint16_t st7102_ydp395bt008_dsi_color_t;
#elif ST7102_YDP395BT008_FB_MODE == ST7102_YDP395BT008_FB_MODE_ARGB8888
/* The OSD scan-out buffer is ARGB8888; the DSI link is always RGB888. */
#define ST7102_YDP395BT008_DSI_COLOR_DEPTH 32
typedef uint32_t st7102_ydp395bt008_dsi_color_t;
#else
#error "Invalid ST7102_YDP395BT008_FB_MODE"
#endif

/* Panel MADCTR color order: 0 = RGB (default), 1 = BGR. */
#ifndef ST7102_YDP395BT008_RGB_ORDER_MODE
#define ST7102_YDP395BT008_RGB_ORDER_MODE 0
#endif

#if (ST7102_YDP395BT008_RGB_ORDER_MODE != 0) && (ST7102_YDP395BT008_RGB_ORDER_MODE != 1)
#error "ST7102_YDP395BT008_RGB_ORDER_MODE must be 0 (RGB) or 1 (BGR)"
#endif

/* Define ST7102_YDP395BT008_TOUCH_RESET_PIN in lcd_conf_user.h if the
 * module's ST7123 TDDI reset must be released before LCD initialization.
 * LCD_RESET_* and LCD_BACKLIGHT_* remain managed by the LCD framework. */

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize a full 480x480 framebuffer. Returns 0 on success, < 0 on failure. */
int st7102_ydp395bt008_dsi_init(st7102_ydp395bt008_dsi_color_t *screen_buffer);

/* Queue a buffer swap; the common DSI helper handles cache clean and SEOF. */
int st7102_ydp395bt008_dsi_screen_switch(st7102_ydp395bt008_dsi_color_t *screen_buffer);

/* Buffer whose swap has completed at a frame boundary. */
st7102_ydp395bt008_dsi_color_t *st7102_ydp395bt008_dsi_get_screen_using(void);

/* callback_type is MIPI_DSI_V2_FRAME_INT_TYPE_CYCLE or _SWAP; runs in ISR. */
int st7102_ydp395bt008_dsi_frame_callback_register(uint32_t callback_type, void (*callback)(void));

const mipi_dsi_v2_timing_t *st7102_ydp395bt008_dsi_get_timing(void);

#ifdef __cplusplus
}
#endif

#endif /* LCD_DSI_ST7102_YDP395BT008 */
#endif /* ST7102_YDP395BT008_DSI_DRIVER_H */
