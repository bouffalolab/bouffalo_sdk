/**
 * @file display_gc9503v.h
 * @brief GC9503V driver IC (RGB video panels with a 3-wire or 4-wire command port) and default modules.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_GC9503V_H
#define DISPLAY_GC9503V_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief GC9503V IC description: command port on DISPLAY_IFACE_DBI_C3 or _C4, RGB pixels (DE mode). */
extern const display_dcs_ic_t display_ic_gc9503v;
/** @brief Default module: 480x480 RGB, R-G-B order, 60 Hz. */
extern const display_dcs_module_t display_module_gc9503v_480x480;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_GC9503V_H */
