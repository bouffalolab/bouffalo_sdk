/**
 * @file display_gc9307.h
 * @brief GC9307 driver IC and default module.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_GC9307_H
#define DISPLAY_GC9307_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief GC9307 IC description and default initialization sequence. */
extern const display_dcs_ic_t display_ic_gc9307;
/** @brief Default module: GC9307 240x320. */
extern const display_dcs_module_t display_module_gc9307_240x320;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_GC9307_H */
