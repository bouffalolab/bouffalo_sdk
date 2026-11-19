/**
 * @file display_ili9881c.h
 * @brief ILI9881C driver IC (MIPI DSI video panels) and default modules.
 *
 * The vendor command set is in pages: 0xFF followed by the signature 0x98 0x81 and the page number selects
 * the page.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * @note The module has to match the number of lanes wired on the board.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_ILI9881C_H
#define DISPLAY_ILI9881C_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief ILI9881C IC description; the default sequence is the one of the KD050HDFIA023-W4 glass. */
extern const display_dcs_ic_t display_ic_ili9881c;
/** @brief KD050HDFIA020-B002 panel, 720x1280, 4 data lanes, RGB565, 550 Mbit/s per lane, about 39 Hz. */
extern const display_dcs_module_t display_module_ili9881c_720x1280_kd050020;
/** @brief KD050HDFIA023-W4 panel, 720x1280, 2 data lanes, RGB565, 850 Mbit/s per lane, about 39 Hz. */
extern const display_dcs_module_t display_module_ili9881c_720x1280_kd050023w4;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_ILI9881C_H */
