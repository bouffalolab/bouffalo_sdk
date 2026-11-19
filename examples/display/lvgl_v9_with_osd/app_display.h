#ifndef APP_DISPLAY_H
#define APP_DISPLAY_H

#include <stdint.h>
#include "default/display_default.h"

#define APP_DISPLAY_WIDTH  DISPLAY_DEFAULT_VIDEO_WIDTH
#define APP_DISPLAY_HEIGHT DISPLAY_DEFAULT_VIDEO_HEIGHT

/* Call after lv_port_disp_init(). The decoder never accesses DPU registers. */
int app_display_init(void);
void app_display_wait_ready(void);
uint32_t app_display_get_scan_frame_count(void);
void app_display_wait_writable(const uint8_t *y);
typedef void (*app_display_wait_callback_t)(void);
/* Register before the submitting task starts; NULL disables the hook. Called
 * only in that task, after an accepted flip and every 1000ms while awaiting IRQ.
 * The submitter must stop MJDEC/DMA before present. The hook may block, but must
 * not submit another flip, write/release either owned frame, or change ownership. */
void app_display_set_wait_callback(app_display_wait_callback_t callback);
/* One submitting task. Blocks until FIRST/RETIRED; NULL submits LAST and waits.
 * Buffers must be DMA-produced, 64-byte aligned, with no dirty CPU cache lines. */
int app_display_present(uint8_t *y, uint8_t *uv);

#endif