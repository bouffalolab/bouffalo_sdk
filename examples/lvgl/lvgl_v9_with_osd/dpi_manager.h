#ifndef _DPI_MANAGER_H_
#define _DPI_MANAGER_H_

#include <stdint.h>
#include "lcd.h" /* panel selected in lcd_conf_user.h -> LCD_W / LCD_H */

/* Panel resolution follows whatever lcd_conf_user.h selected, so
 * switching panels is just a one-line change there (no edits in this project). */
#define LCD_WIDTH  LCD_W
#define LCD_HEIGHT LCD_H
#define LCD_PIXELS (LCD_WIDTH * LCD_HEIGHT)

/* SW GPIO clock. */
#if defined(LCD_DPI_STANDARD)
#define DPI_PIXEL_CLOCK_USE_SW_GPIO 1
#else
#define DPI_PIXEL_CLOCK_USE_SW_GPIO 0
#endif

/* Background video size. When it equals the panel, the MJDEC decodes straight
 * into the DPI framebuffer (no centering, no intermediate copy). When smaller,
 * it is decoded to a scratch buffer and composited into the center with black
 * borders (the DPI background layer has no offset register). Defaults to the
 * panel size (full-screen); override here for a centered sub-panel video. 
 * (384x768 480x960) (576x1024 720x1280) (172x640) (700x420 800x480) (960x540 1024x600) */
#ifndef VIDEO_WIDTH
#define VIDEO_WIDTH    1024
#endif
#ifndef VIDEO_HEIGHT
#define VIDEO_HEIGHT   600
#endif

#define VIDEO_OFFSET_X ((LCD_WIDTH - VIDEO_WIDTH) / 2)
#define VIDEO_OFFSET_Y ((LCD_HEIGHT - VIDEO_HEIGHT) / 2)

/* Background video does not need to run flat-out; capping it leaves more CPU
 * time for the lower-priority LVGL overlay task. */
#define VIDEO_TARGET_FPS      100U
#define VIDEO_FRAME_PERIOD_MS ((1000U + VIDEO_TARGET_FPS - 1U) / VIDEO_TARGET_FPS)

/* Full-screen video -> direct decode into the framebuffer (the early, fast path).
 * Any smaller size -> decode-to-scratch + centered composite. */
#define VIDEO_FULLSCREEN ((VIDEO_WIDTH == LCD_WIDTH) && (VIDEO_HEIGHT == LCD_HEIGHT))

/* Bring up the video pipeline: MJDEC + (sub-panel) DMA2D + ping-pong YUV
 * background framebuffers. The DSI display side (panel link, DPI background
 * layer, OSD0 overlay, OSD interrupt) is brought up earlier by lcd_init() ->
 * the panel's _dsi_init() -> mipi_dsi_v2. Returns 0 on success. */
int dpi_manager_init(void);

#if defined(CONFIG_FREERTOS)
/* Consumer task: pull JPEG buffers from the filesystem full queue, MJDEC-decode
 * into ping-pong YUV buffers, switch the DPI background layer to each new frame. */
void image_switch_task(void *param);
#endif

#endif /* _DPI_MANAGER_H_ */
