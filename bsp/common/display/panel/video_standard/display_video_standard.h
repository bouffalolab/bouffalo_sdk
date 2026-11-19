/**
 * @file display_video_standard.h
 * @brief Minimal panel driver for standard RGB panels without driver IC command port, and their modules.
 *
 * A standard panel needs no initialization: it shows what the timing delivers. The driver therefore
 * only powers the panel, pulses the reset line (if wired) and reports the geometry. There is no
 * command endpoint, no rotation and no DCS sequence.
 *
 * Use a default module directly, or describe your own panel in your project:
 *
 * @code
 * static const display_video_mode_t my_video = {
 *     .link = DISPLAY_VLINK_DPI, .width = 800, .height = 480,
 *     .hsw = 4, .hbp = 82, .hfp = 14, .vsw = 5, .vbp = 6, .vfp = 39, .clock_hz = 33390000,
 * };
 * static const display_video_module_t my_module = { .name = "my_800x480", .video = &my_video };
 * static display_video_panel_t panel = DISPLAY_VIDEO_PANEL(&my_module, DISPLAY_BOARD_NONE);
 * @endcode
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_VIDEO_STANDARD_H
#define DISPLAY_VIDEO_STANDARD_H

#include "panel/display_panel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL
 * @{
 */

/**
 * @defgroup DISPLAY_PANEL_VIDEO_STANDARD Standard video panel
 * @{
 */

/**
 * @brief Module description of a standard panel: the size follows from the video link.
 */
typedef struct {
    const char *name;                  /**< Name */
    const display_video_mode_t *video; /**< Video link, @ref display_video_mode_t::width / height is the panel size */
} display_video_module_t;

/**
 * @brief Panel object of the standard video panel driver.
 */
typedef struct {
    display_panel_t base;                 /**< Panel core, must be first */
    const display_video_module_t *module; /**< Module description */
} display_video_panel_t;

/** @brief Driver operations of the standard video panel. */
extern const display_panel_ops_t display_video_panel_ops;

/**
 * @brief Static definition of a standard video panel.
 *
 * @param _module pointer to a @ref display_video_module_t
 * @param _board  board power / reset / backlight, e.g. DISPLAY_BOARD_NONE
 */
#define DISPLAY_VIDEO_PANEL(_module, _board) \
    { .base = { .ops = &display_video_panel_ops, .io = NULL, .board = _board }, .module = (_module) }

/** @brief Standard parallel RGB panel, 800x480, 70 Hz. */
extern const display_video_module_t display_module_rgb_800x480;
/** @brief Standard parallel RGB panel, 1024x600, 70 Hz. */
extern const display_video_module_t display_module_rgb_1024x600;
/**
 * @brief Standard serial RGB panel, 480x272, RGB888 on 8 data lines (3 sub-pixel clocks per pixel), about 63.5 Hz at
 *        the 27 MHz sub-pixel clock. No scanout backend drives a serial RGB link yet.
 */
extern const display_video_module_t display_module_srgb_480x272;

/** @} */ /* DISPLAY_PANEL_VIDEO_STANDARD */
/** @} */ /* DISPLAY_PANEL */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_VIDEO_STANDARD_H */
