/**
 * @file display_scanout_driver.h
 * @brief Scanout backend operations, base object and event reporting.
 *
 * Concrete backend headers include this contract to embed a scanout base and
 * support static allocation. Applications using an existing scanout need only
 * display_scanout.h.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_SCANOUT_DRIVER_H
#define DISPLAY_SCANOUT_DRIVER_H

#include "scanout/display_scanout.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Operations implemented by a scanout backend. */
typedef struct {
    int (*init)(display_scanout_t *scanout); /**< Discover devices, fill caps, claim the screen layer */
    int (*config)(display_scanout_t *scanout, const display_video_mode_t *mode,
                  const display_scanout_cfg_t *cfg); /**< No frame is read on success or failure */
    int (*flip)(display_scanout_t *scanout, const display_fb_t *fb);
    /**< Accept a frame or NULL from task or ISR. On error nothing was accepted.
     * Expensive preparation preserves the caller's IRQ state; only hardware
     * writes and shared state briefly mask IRQs. Report only accepted ops. */
    int (*deinit)(display_scanout_t *scanout); /**< Synchronous release, including output stop; retry on error */
} display_scanout_ops_t;

/** @brief Scanout base object, embedded first by a concrete backend. */
struct display_scanout {
    const display_scanout_ops_t *ops;
    display_scanout_cfg_t cfg;
    display_scanout_caps_t caps;
    display_scanout_state_t state;
    display_video_mode_t mode;
    display_fb_t current;
    display_fb_t pending;
    volatile display_frame_op_t pending_op;
    bool initialized;
    bool configured;
    display_scanout_cb_t cb;
    void *cb_ctx;
};

/** @brief Static config, e.g. .format = DISPLAY_FMT_RGB565; no frame memory. */
#define DISPLAY_SCANOUT_BASE(_ops, ...)       \
    {                                         \
        .ops = (_ops), .cfg = { __VA_ARGS__ } \
    }

/** @brief Backend ISR: FIRST / RETIRED / LAST or an ordinary VSYNC. Updates
 * state before notifying the listener; ignores unexpected completion kinds. */
void display_scanout_on_event(display_scanout_t *scanout, display_event_type_t type);

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_SCANOUT_DRIVER_H */
