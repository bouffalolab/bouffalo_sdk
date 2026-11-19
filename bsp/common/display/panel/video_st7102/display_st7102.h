/**
 * @file display_st7102.h
 * @brief ST7102 driver IC (MIPI DSI video panels) and default modules.
 *
 * The two glasses come with different vendor scripts, so each module carries its own initialization sequence;
 * the IC description has no common one.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_ST7102_H
#define DISPLAY_ST7102_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief ST7102 IC description. */
extern const display_dcs_ic_t display_ic_st7102;
/** @brief KD027HVFID009 panel, 320x320, 1 data lane, RGB565, 400 Mbit/s per lane, about 63 Hz. */
extern const display_dcs_module_t display_module_st7102_320x320_kd027hvf;
/** @brief YH-494BSAC002N1 panel, 480x960 portrait, 2 data lanes, RGB565, 850 Mbit/s per lane, about 45 Hz. */
extern const display_dcs_module_t display_module_st7102_480x960_yh494;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_ST7102_H */
