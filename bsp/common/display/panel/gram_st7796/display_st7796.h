/**
 * @file display_st7796.h
 * @brief ST7796 driver IC and default module.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_ST7796_H
#define DISPLAY_ST7796_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief ST7796 IC description and default initialization sequence. */
extern const display_dcs_ic_t display_ic_st7796;
/** @brief Default module: 3.5" 320x480, BGR, X-mirrored, display inversion enabled. */
extern const display_dcs_module_t display_module_st7796_320x480;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_ST7796_H */
