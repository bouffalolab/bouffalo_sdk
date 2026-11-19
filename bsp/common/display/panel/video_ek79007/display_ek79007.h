/**
 * @file display_ek79007.h
 * @brief EK79007 driver IC (MIPI DSI to parallel RGB bridge of 7" 1024x600 panels) and default modules.
 *
 * The IC takes a few vendor registers over LP and converts the DSI video stream to RGB. It has no memory,
 * no sleep and no display on/off command: the panel shows what the link delivers.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * @note The module has to match the number of lanes wired on the board.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_EK79007_H
#define DISPLAY_EK79007_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief EK79007 IC description and default initialization sequence (4 lanes, the default of the IC). */
extern const display_dcs_ic_t display_ic_ek79007;
/** @brief WKS70WSV114 panel, 1024x600, 2 data lanes, RGB888, 850 Mbit/s per lane, about 62.5 Hz. */
extern const display_dcs_module_t display_module_ek79007_1024x600_2lane;
/** @brief WKS70WSV114 panel, 1024x600, 4 data lanes, RGB888, 850 Mbit/s per lane, about 62.5 Hz. */
extern const display_dcs_module_t display_module_ek79007_1024x600_4lane;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_EK79007_H */
