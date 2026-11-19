/**
 * @file display_st7701p.h
 * @brief ST7701P driver IC (RGB video panels with a 3-wire or 4-wire command port) and default modules.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * @note 0xFF is a real bank-switch command of this IC; delays are a separate field, so there is no ambiguity.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_ST7701P_H
#define DISPLAY_ST7701P_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief ST7701P IC description: command port on DISPLAY_IFACE_DBI_C3 or DISPLAY_IFACE_DBI_C4, RGB pixels. */
extern const display_dcs_ic_t display_ic_st7701p;
/** @brief Default module: 480x480 RGB, R-G-B order, 60 Hz. */
extern const display_dcs_module_t display_module_st7701p_480x480;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_ST7701P_H */
