#ifndef _DPI_MANAGER_H_
#define _DPI_MANAGER_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "app_display.h" /* main owns the panel selection, dimensions and scan-out */
#include "video_config.h" /* customer knobs: VIDEO_WIDTH/HEIGHT, VIDEO_TARGET_FPS */

#if defined(CONFIG_FREERTOS)
#include <FreeRTOS.h>
#endif

typedef struct {
    uint8_t *data;
    size_t size;
} jpg_buffer_t;

/* Panel resolution follows the display selected by main, not the old LCD BSP. */
#define LCD_WIDTH  APP_DISPLAY_WIDTH
#define LCD_HEIGHT APP_DISPLAY_HEIGHT
#define LCD_PIXELS (LCD_WIDTH * LCD_HEIGHT)

/* VIDEO_WIDTH / VIDEO_HEIGHT config live in video_config.h. */
#ifndef VIDEO_WIDTH
#define VIDEO_WIDTH    LCD_WIDTH
#endif
#ifndef VIDEO_HEIGHT
#define VIDEO_HEIGHT   LCD_HEIGHT
#endif

/* VIDEO_OFFSET_X must be 4-aligned: the DMA2D composite does 32-bit writes to
 * base+stride*Y+X. Round the centering offset down to a multiple of 4 (video
 * shifts <=3px into the border). NV12 also needs an even Y/chroma origin. */
#define VIDEO_OFFSET_X (((LCD_WIDTH - VIDEO_WIDTH) / 2) & ~3u)
#define VIDEO_OFFSET_Y (((LCD_HEIGHT - VIDEO_HEIGHT) / 2) & ~1u)

/* MJDEC decodes whole 16x16 MCUs: it WRITES ceil(dim/16)*16 px per plane, so every
 * decode buffer must be MCU-aligned or it overruns and corrupts PSRAM. Panel scans
 * only WIDTH x HEIGHT. A non-16-aligned width needs the DMA2D crop/repack path. */
#define MJDEC_MCU_ALIGN(v) (((v) + 15) & ~15u)
#define LCD_WIDTH_MCU      MJDEC_MCU_ALIGN(LCD_WIDTH)
#define LCD_HEIGHT_MCU     MJDEC_MCU_ALIGN(LCD_HEIGHT)
#define LCD_PIXELS_MCU     (LCD_WIDTH_MCU * LCD_HEIGHT_MCU)
#define VIDEO_WIDTH_MCU    MJDEC_MCU_ALIGN(VIDEO_WIDTH)
#define VIDEO_HEIGHT_MCU   MJDEC_MCU_ALIGN(VIDEO_HEIGHT)

/* Panel YUV buffer layout (retain the original DPI allocation headroom). */
#define YUV_Y_PLANE_SIZE   LCD_PIXELS_MCU
#define YUV_BUFFER_SIZE    (LCD_PIXELS_MCU * 2)

/* VIDEO_TARGET_FPS lives in video_config.h (customer-editable). Derived from it: */
#define VIDEO_FRAME_PERIOD_MS ((1000U + VIDEO_TARGET_FPS - 1U) / VIDEO_TARGET_FPS)

/* Full-screen fast path: decode straight into the framebuffer. Requires MCU-aligned
 * width == LCD_WIDTH: the scan-out uses visible-width stride, not MCU stride. */
#define VIDEO_FULLSCREEN ((VIDEO_WIDTH == LCD_WIDTH) && (VIDEO_HEIGHT == LCD_HEIGHT) && \
                          (VIDEO_WIDTH_MCU == VIDEO_WIDTH))

_Static_assert(VIDEO_TARGET_FPS > 0, "VIDEO_TARGET_FPS must be positive");
_Static_assert(VIDEO_WIDTH > 0 && VIDEO_HEIGHT > 0 &&
               VIDEO_WIDTH <= LCD_WIDTH && VIDEO_HEIGHT <= LCD_HEIGHT, "video exceeds panel");
_Static_assert(!(VIDEO_WIDTH & 1U) && !(VIDEO_HEIGHT & 1U) &&
               !(LCD_WIDTH & 1U) && !(LCD_HEIGHT & 1U), "NV12 requires even dimensions");
_Static_assert(LCD_PIXELS_MCU % 128U == 0, "each Y/UV slot must be cache64 aligned");

/* Bring up MJDEC + sub-panel DMA2D + YUV buffers; main owns the display side.
 * image_switch_task waits for app_display readiness before using scan-out. */
int dpi_manager_init(void);

#if defined(CONFIG_FREERTOS)
int frame_buffer_pool_init(jpg_buffer_t *buffers, uint8_t *data, uint32_t buffer_count, size_t buffer_size);
int frame_buffer_get(jpg_buffer_t **buffer, TickType_t timeout);
int frame_buffer_push(jpg_buffer_t *buffer, TickType_t timeout);
int frame_buffer_pop(jpg_buffer_t **buffer, TickType_t timeout);
int frame_buffer_release(jpg_buffer_t *buffer);

/* Consumer task: pull JPEG buffers from the output queue, MJDEC-decode
 * into ping-pong/triple YUV buffers, present each completed frame via app_display. */
void image_switch_task(void *param);

#if defined(CONFIG_LVGL_V9_WITH_OSD_OTA) && CONFIG_LVGL_V9_WITH_OSD_OTA
#define VIDEO_OTA_PRODUCER 1U
#define VIDEO_OTA_DECODER  2U
/* Compatibility status/requests. Prefer the cooperative APIs below: paused only
 * becomes true after all configured participants reach safe boundaries. */
extern volatile bool g_video_pause_req;
extern volatile bool g_video_paused;
extern volatile bool g_video_resume_req;
/* Startup only, with no pause in progress or participant tasks running. Defaults
 * to PRODUCER | DECODER. Configure 0 after decoder init while playback is absent,
 * then the required bits before creating its tasks; restore 0 on startup failure
 * after deleting any created tasks. Never exclude a running producer/decoder. */
void video_ota_configure_participants(uint32_t participants);
int video_ota_pause(void);
void video_ota_resume(void);
/* Safe task boundary: no JPEG decode or filesystem/USB operation in flight. */
void video_ota_checkpoint(uint32_t participant);
#endif
#endif

#endif /* _DPI_MANAGER_H_ */