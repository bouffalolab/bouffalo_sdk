#ifndef _DPI_MANAGER_H_
#define _DPI_MANAGER_H_

#include <stddef.h>
#include <stdint.h>
#include "lcd.h" /* panel selected in lcd_conf_user.h -> LCD_W / LCD_H */
#include "video_config.h" /* customer knobs: VIDEO_WIDTH/HEIGHT, VIDEO_TARGET_FPS */

#if defined(CONFIG_FREERTOS)
#include <FreeRTOS.h>
#endif

typedef struct {
    uint8_t *data;
    size_t size;
} jpg_buffer_t;

/* Panel resolution follows whatever lcd_conf_user.h selected, so
 * switching panels is just a one-line change there (no edits in this project). */
#define LCD_WIDTH  LCD_W
#define LCD_HEIGHT LCD_H
#define LCD_PIXELS (LCD_WIDTH * LCD_HEIGHT)

/* VIDEO_WIDTH / VIDEO_HEIGHT config live in video_config.h. */
#ifndef VIDEO_WIDTH
#define VIDEO_WIDTH    LCD_WIDTH
#endif
#ifndef VIDEO_HEIGHT
#define VIDEO_HEIGHT   LCD_HEIGHT
#endif

/* VIDEO_OFFSET_X must be 4-aligned: the DMA2D composite does 32-bit writes to
 * base+stride*Y+X (stride=LCD_WIDTH, 4-aligned), so a non-4 X 
 * bus-faults. Round the centering offset down to a multiple of 4 (video shifts <=3px into the border). */
#define VIDEO_OFFSET_X (((LCD_WIDTH - VIDEO_WIDTH) / 2) & ~3u)
#define VIDEO_OFFSET_Y ((LCD_HEIGHT - VIDEO_HEIGHT) / 2)

/* MJDEC decodes whole 16x16 MCUs: it WRITES ceil(dim/16)*16 px per plane, so every decode
 * buffer must be MCU-aligned or it overruns and corrupts PSRAM (-> "parse header error"/timeouts;
 * e.g. 854->864, 480 stays). Panel scans only WIDTH x HEIGHT. When WIDTH isn't 16-aligned the
 * decode stride > scan stride and a direct decode shears -> VIDEO_FULLSCREEN routes it via DMA2D. */
#define MJDEC_MCU_ALIGN(v) (((v) + 15) & ~15u)
#define LCD_WIDTH_MCU      MJDEC_MCU_ALIGN(LCD_WIDTH)
#define LCD_HEIGHT_MCU     MJDEC_MCU_ALIGN(LCD_HEIGHT)
#define LCD_PIXELS_MCU     (LCD_WIDTH_MCU * LCD_HEIGHT_MCU)
#define VIDEO_WIDTH_MCU    MJDEC_MCU_ALIGN(VIDEO_WIDTH)
#define VIDEO_HEIGHT_MCU   MJDEC_MCU_ALIGN(VIDEO_HEIGHT)

/* Panel YUV buffer layout */
#define YUV_Y_PLANE_SIZE   LCD_PIXELS_MCU
#define YUV_BUFFER_SIZE    (LCD_PIXELS_MCU * 2)

/* VIDEO_TARGET_FPS lives in video_config.h (customer-editable). Derived from it: */
#define VIDEO_FRAME_PERIOD_MS ((1000U + VIDEO_TARGET_FPS - 1U) / VIDEO_TARGET_FPS)

/* Full-screen fast path: decode straight into the framebuffer. Requires MCU-aligned width
 * == LCD_WIDTH, because the DTSRC/DVP2AXI scanout has no pitch register (only visible width),
 * so a non-16-aligned width would shear -> fall back to DMA2D crop/repack. */
#define VIDEO_FULLSCREEN ((VIDEO_WIDTH == LCD_WIDTH) && (VIDEO_HEIGHT == LCD_HEIGHT) && \
                          (VIDEO_WIDTH_MCU == VIDEO_WIDTH))

/* Bring up the video pipeline (MJDEC + sub-panel DMA2D + ping-pong YUV buffers); the
 * display side is already up via lcd_init() -> the panel's _dsi_init(). Returns 0 on success. */
int dpi_manager_init(void);

#if defined(CONFIG_FREERTOS)
int frame_buffer_pool_init(jpg_buffer_t *buffers, uint8_t *data, uint32_t buffer_count, size_t buffer_size);
int frame_buffer_get(jpg_buffer_t **buffer, TickType_t timeout);
int frame_buffer_push(jpg_buffer_t *buffer, TickType_t timeout);
int frame_buffer_pop(jpg_buffer_t **buffer, TickType_t timeout);
int frame_buffer_release(jpg_buffer_t *buffer);

/* Consumer task: pull JPEG buffers from the output queue, MJDEC-decode
 * into ping-pong YUV buffers, switch the DPI background layer to each new frame. */
void image_switch_task(void *param);
#endif

#endif /* _DPI_MANAGER_H_ */
