/**
 * @file display_ili9341.h
 * @brief ILI9341 driver IC and default module.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_ILI9341_H
#define DISPLAY_ILI9341_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief ILI9341 IC description and default initialization sequence. */
extern const display_dcs_ic_t display_ic_ili9341;
/** @brief Default module: 2.4"/2.8" 240x320. */
extern const display_dcs_module_t display_module_ili9341_240x320;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_ILI9341_H */
