/**
 * @file display_st7789.h
 * @brief ST7789 driver IC and default modules.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_ST7789_H
#define DISPLAY_ST7789_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief ST7789 IC description and default initialization sequence. */
extern const display_dcs_ic_t display_ic_st7789;
/** @brief Default module: 2.0"/2.4" 240x320. */
extern const display_dcs_module_t display_module_st7789_240x320;
/** @brief Default module: 1.3"/1.54" 240x240, the glass sits at one end of the 240x320 GRAM. */
extern const display_dcs_module_t display_module_st7789_240x240;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_ST7789_H */
