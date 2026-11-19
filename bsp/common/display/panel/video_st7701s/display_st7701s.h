/**
 * @file display_st7701s.h
 * @brief ST7701S driver IC (RGB video panels with a command port, and MIPI DSI video panels) and default modules.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * @note 0xFF is a real bank-switch command of this IC; delays are a separate field, so there is no ambiguity.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_ST7701S_H
#define DISPLAY_ST7701S_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief ST7701S IC description: the default initialization sequence is the RGB one, MIPI DSI has its own. */
extern const display_dcs_ic_t display_ic_st7701s;
/** @brief Default module: 480x480 RGB, B-G-R order, command port 3-wire 9-bit or 4-wire SPI, 60 Hz. */
extern const display_dcs_module_t display_module_st7701s_rgb_480x480;
/** @brief HD40007C30-V5 panel on MIPI DSI, 480x480, 2 lanes, RGB888, 400 Mbit/s per lane, about 61 Hz, B-G-R. */
extern const display_dcs_module_t display_module_st7701s_dsi_480x480;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_ST7701S_H */
