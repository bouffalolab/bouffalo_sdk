#include "app_display.h"
#include "default/display_default.h"
#include "scanout/bl618dg_dpu/display_bl618dg_dpu_scanout.h"
#include <FreeRTOS.h>
#include "task.h"
#include "semphr.h"
#include "log.h"
#include <errno.h>

static display_bl618dg_dpu_hw_t *hw;
static SemaphoreHandle_t done;
static const uint8_t *current_y;
static const uint8_t *pending_y;
static volatile bool outstanding;
static volatile display_event_type_t expected;
static volatile bool ready;
static volatile uint32_t scan_frame_count;
static app_display_wait_callback_t wait_callback;

static void base_event(display_event_type_t type, void *ctx)
{
    (void)ctx;
    if (type == DISPLAY_EVT_VSYNC || type == DISPLAY_EVT_FB_FIRST ||
        type == DISPLAY_EVT_FB_RETIRED || type == DISPLAY_EVT_FB_LAST) {
        scan_frame_count++;
    }
    /* VSYNC alone never returns a buffer. FIRST returns no previous frame;
     * RETIRED returns the old frame; LAST returns the final Y and UV planes. */
    if (outstanding && type == expected) {
        BaseType_t woken = pdFALSE;
        outstanding = false;
        xSemaphoreGiveFromISR(done, &woken);
        portYIELD_FROM_ISR(woken);
    }
}

int app_display_init(void)
{
    display_t *display = display_default_get();
    display_bl618dg_dpu_layer_info_t info;
    display_bl618dg_dpu_layer_cfg_t cfg = {
        .format = DISPLAY_FMT_NV12,
        /* CPU initialization is cleaned+invalidated by dpi_manager; all later
         * writes are MJDEC/DMA2D. Never clean stale CPU data over DMA output. */
        .dcache_dis = true,
    };
    int ret;

    if (!display || !display_get_scanout(display)) {
        return -ENODEV;
    }
    hw = &((display_bl618dg_dpu_scanout_t *)display_get_scanout(display))->hw;
    if (!hw->running || hw->width != APP_DISPLAY_WIDTH || hw->height != APP_DISPLAY_HEIGHT) {
        return -EINVAL;
    }
    ret = display_bl618dg_dpu_hw_layer_get_info(hw, DISPLAY_BL618DG_DPU_LAYER_BASE, &info);
    if (ret || info.running || info.pending_op != DISPLAY_FRAME_OP_NONE) {
        return ret ? ret : -EBUSY;
    }
    done = xSemaphoreCreateBinary();
    if (!done) {
        return -ENOMEM;
    }
    ret = display_bl618dg_dpu_hw_layer_config(hw, DISPLAY_BL618DG_DPU_LAYER_BASE, &cfg);
    if (!ret) {
        ret = display_bl618dg_dpu_hw_layer_set_callback(hw, DISPLAY_BL618DG_DPU_LAYER_BASE, base_event, NULL);
    }
    if (!ret) {
        ready = true;
    }
    return ret;
}

void app_display_wait_ready(void)
{
    while (!ready) {
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

uint32_t app_display_get_scan_frame_count(void)
{
    return scan_frame_count;
}

void app_display_set_wait_callback(app_display_wait_callback_t callback)
{
    wait_callback = callback;
}

void app_display_wait_writable(const uint8_t *y)
{
    while (y == current_y || y == pending_y) {
        /* No timeout can make a scanned/queued buffer writable. */
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

int app_display_present(uint8_t *y, uint8_t *uv)
{
    display_fb_t fb = {
        .data = y, .uv = uv, .format = DISPLAY_FMT_NV12,
        .stride = APP_DISPLAY_WIDTH, .uv_stride = APP_DISPLAY_WIDTH,
    };
    int ret;

    if (outstanding || (y && (!uv || y == current_y))) {
        return -EBUSY;
    }
    if (!y && !current_y) {
        return 0; /* Already disabled: the driver produces no LAST. */
    }
    xSemaphoreTake(done, 0);
    pending_y = y;
    expected = !y ? DISPLAY_EVT_FB_LAST :
               (!current_y ? DISPLAY_EVT_FB_FIRST : DISPLAY_EVT_FB_RETIRED);
    outstanding = true; /* An IRQ may complete before flip returns. */
    ret = display_bl618dg_dpu_hw_layer_flip(hw, DISPLAY_BL618DG_DPU_LAYER_BASE, y ? &fb : NULL, NULL);
    if (ret) {
        outstanding = false;
        pending_y = NULL; /* Rejected submission: current frame stays owned. */
        return ret;
    }
    /* The caller's MJDEC/DMA is stopped. A cooperative pause is safe even if
     * scan-out still owns both frames; only the matching IRQ can retire one. */
    if (wait_callback) {
        wait_callback();
    }
    while (outstanding) {
        if (xSemaphoreTake(done, pdMS_TO_TICKS(1000)) != pdTRUE && outstanding) {
            if (wait_callback) {
                wait_callback();
            }
            LOG_E("Base IRQ missing; keeping both NV12 planes owned, no reuse\r\n");
        }
    }
    current_y = y; /* Only the matching IRQ permits retiring the previous frame. */
    pending_y = NULL;
    return 0;
}