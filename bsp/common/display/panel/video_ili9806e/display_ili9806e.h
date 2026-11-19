/**
 * @file display_ili9806e.h
 * @brief ILI9806E driver IC (MIPI DSI video panels) and default module.
 *
 * The vendor command set is in pages: 0xFF followed by the signature 0xFF 0x98 0x06 0x04 and the page number
 * selects the page (not to be confused with the 0x98 0x81 signature of the ILI9881C).
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_ILI9806E_H
#define DISPLAY_ILI9806E_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief ILI9806E IC description and its initialization sequence (RGB565 link, 180 degree vendor script). */
extern const display_dcs_ic_t display_ic_ili9806e;
/** @brief KD050FWFIA019 panel, 480x854, 2 data lanes, RGB565, 550 Mbit/s per lane, about 62 Hz. */
extern const display_dcs_module_t display_module_ili9806e_480x854;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_ILI9806E_H */
