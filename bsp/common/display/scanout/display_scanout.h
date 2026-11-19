/**
 * @file display_scanout.h
 * @brief Video scanout: configure while disabled, then submit frames or NULL.
 *
 * A non-NULL flip enables a disabled screen layer or replaces its frame. A NULL
 * flip disables that layer, not necessarily the video output. FIRST / RETIRED /
 * LAST confirm the operation; an ordinary boundary reports VSYNC instead.
 * Events carry no address: the caller keeps track of its submitted frames.
 *
 * One owner serializes each scanout's lifecycle/configuration. Frame submission,
 * queries and callback changes may run in a task or ISR, including event callbacks.
 * There is no multi-task arbitration; an interrupting submission may make the
 * interrupted preparation return -EBUSY without replacing the accepted frame.
 * Frames are tightly packed and owned by the hardware after a successful flip.
 * RETIRED releases the previous frame; LAST releases the final frame. A failed
 * deinit does not release frames or resources and may be retried.
 * No frame, including the first, requires static storage; its memory must remain
 * valid and unchanged until released by RETIRED / LAST or successful deinit.
 *
 * Include a concrete backend header to define a scanout object. Backend operations,
 * the complete base object and event reporting are in display_scanout_driver.h.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_SCANOUT_H
#define DISPLAY_SCANOUT_H

#include "display_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct display_scanout display_scanout_t;

typedef enum {
    DISPLAY_SCANOUT_STOPPED = 0, /**< Screen layer disabled; video output may still run */
    DISPLAY_SCANOUT_RUNNING,     /**< Screen layer shows a frame */
    DISPLAY_SCANOUT_ERROR        /**< Release failed; frames and resources remain owned */
} display_scanout_state_t;

#define DISPLAY_SCANOUT_CAP_FLIP   (1u << 0)
#define DISPLAY_SCANOUT_CAP_VSYNC  (1u << 1)
#define DISPLAY_SCANOUT_CAP_ROTATE (1u << 2)

typedef struct {
    uint32_t flags;
    uint32_t format_mask;
    uint32_t vlink_mask;
    uint16_t addr_align;
    uint16_t size_align;
} display_scanout_caps_t;

/** @brief Configuration independent of any frame address. */
typedef struct {
    display_format_t format;
    bool dcache_dis; /**< Default false: the core cleans the frame (both NV12 planes) before submission */
} display_scanout_cfg_t;

typedef struct {
    display_scanout_state_t state;
    const void *current;
    const void *pending;
    display_frame_op_t pending_op; /**< LAST is pending even though pending is NULL */
} display_scanout_status_t;

/** @brief ISR listener: may submit the next frame; never block waiting for a boundary. */
typedef void (*display_scanout_cb_t)(display_scanout_t *scanout, const display_event_t *event, void *ctx);

/* Lifecycle/config context depends on backend waits and board hooks; serialize them.
 * Queries, callback changes and flip do not reject ISR context. */
int display_scanout_init(display_scanout_t *scanout);
int display_scanout_deinit(display_scanout_t *scanout);

/** @brief Single listener; remove with NULL before installing another one. */
int display_scanout_set_callback(display_scanout_t *scanout, display_scanout_cb_t cb, void *ctx);

/** @brief Only while disabled and no operation is pending. The backend must
 * reject timing changes that would disturb an already running video output. */
int display_scanout_config(display_scanout_t *scanout, const display_video_mode_t *mode,
                           const display_scanout_cfg_t *cfg);

/** @brief Nonblocking task/ISR submission. NULL disables, a non-NULL fb must
 * have data and match cfg.format. One operation at a time (-EBUSY).
 * Cache cleaning and backend preparation can be lengthy: budget ISR latency,
 * or clean in advance and set cfg.dcache_dis.
 * Disabled + NULL is an immediate no-op without a completion event, even before config.
 * A successful submission may complete before this call returns. */
int display_scanout_flip(display_scanout_t *scanout, const display_fb_t *fb);

int display_scanout_get_caps(display_scanout_t *scanout, display_scanout_caps_t *caps);
int display_scanout_get_status(display_scanout_t *scanout, display_scanout_status_t *status);

#ifdef __cplusplus
}
#endif
#endif /* DISPLAY_SCANOUT_H */
