/**
 * @file display_bl618dg_dpu_scanout.h
 * @brief BL618DG display processing unit as a scanout of the display framework: an adapter that makes the OSD0 layer of
 *        the hardware level (display_bl618dg_dpu_hw.h) the screen of a display_t.
 *
 * - init claims disabled OSD0, config starts only the output and configures its
 *   format, flip(NULL) disables without releasing the claim. deinit synchronously
 *   stops all layers/output before releasing OSD0 and its callback.
 * - Base and OSD1 remain directly configurable through &dpu.hw. Public mutating
 *   OSD0 calls return -EBUSY while claimed, including while the layer is disabled.
 * - RGB pins: `.dsi = NULL`; MIPI DSI: `.dsi` points to the DSI host.
 * - The frame interrupt is OSD0 SEOF, owned by the hardware level. A new frame is written to OSD0 by the caller and
 *   takes effect at a frame boundary; the previous frame is reported as retired at the first frame interrupt after
 *   that. FIRST/RETIRED/LAST replace VSYNC; an ordinary boundary reports VSYNC.
 * - Frames are tightly packed and cache-line aligned; the scanout core cleans the data cache before submission,
 *   unless `base.cfg.dcache_dis` is set (the application cleans it).
 *
 * RGB backends of other chips (BL616, called SIM v1) implement the same scanout interface.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_BL618DG_DPU_SCANOUT_H
#define DISPLAY_BL618DG_DPU_SCANOUT_H

#include "scanout/bl618dg_dpu/display_bl618dg_dpu_hw.h"
#include "scanout/display_scanout_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY
 * @{
 */

/**
 * @defgroup DISPLAY_SCANOUT_BL618DG_DPU_SCANOUT BL618DG DPU scanout backend
 * @{
 */

/**
 * @brief DPU scanout object: the scanout core and the hardware level it drives.
 */
typedef struct {
    display_scanout_t base;      /**< Scanout core, must be first */
    display_bl618dg_dpu_hw_t hw; /**< The DPU hardware: configuration and runtime state */
} display_bl618dg_dpu_scanout_t;

/** @brief Backend operations, to be used as the `.ops` of the scanout base. */
extern const display_scanout_ops_t display_bl618dg_dpu_scanout_ops;

/**
 * @brief Static definition of a DPU scanout.
 *
 * @param _format memory format; configurable while disabled, no initial frame
 * @param ...     designated initializers of @ref display_bl618dg_dpu_cfg_t, e.g. `.dsi = &dsi0`
 */
#define DISPLAY_BL618DG_DPU_SCANOUT(_format, ...)                                            \
    {                                                                                        \
        .base = DISPLAY_SCANOUT_BASE(&display_bl618dg_dpu_scanout_ops, .format = (_format)), \
        .hw = { .cfg = { __VA_ARGS__ } }                                                     \
    }

/** @} */ /* DISPLAY_SCANOUT_BL618DG_DPU_SCANOUT */
/** @} */ /* DISPLAY */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_BL618DG_DPU_SCANOUT_H */
