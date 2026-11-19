/**
 * @file display_ili9486.h
 * @brief ILI9486 driver IC and default module.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * @note Like the ILI9488, the serial interfaces are described with the 18-bit class only (not verified).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_ILI9486_H
#define DISPLAY_ILI9486_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief ILI9486 IC description and default initialization sequence. */
extern const display_dcs_ic_t display_ic_ili9486;
/** @brief Default module: 3.5" 320x480. */
extern const display_dcs_module_t display_module_ili9486_320x480;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_ILI9486_H */
