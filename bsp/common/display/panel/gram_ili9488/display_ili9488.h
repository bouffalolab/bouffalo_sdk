/**
 * @file display_ili9488.h
 * @brief ILI9488 driver IC and default module.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * @note ILI9488 accepts 18-bit pixels only on its serial interfaces: use `.format = DISPLAY_FMT_XRGB8888`
 *       (24-bit class on the wire) on C3/C4; RGB565 works on 8080 (DISPLAY_IFACE_DBI_B8).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_ILI9488_H
#define DISPLAY_ILI9488_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief ILI9488 IC description and default initialization sequence. */
extern const display_dcs_ic_t display_ic_ili9488;
/** @brief Default module: 3.5" 320x480. */
extern const display_dcs_module_t display_module_ili9488_320x480;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_ILI9488_H */
