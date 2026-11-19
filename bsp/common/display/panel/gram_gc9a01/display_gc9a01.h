/**
 * @file display_gc9a01.h
 * @brief GC9A01 driver IC and default module.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_GC9A01_H
#define DISPLAY_GC9A01_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief GC9A01 IC description and default initialization sequence. */
extern const display_dcs_ic_t display_ic_gc9a01;
/** @brief Default module: 1.28" round 240x240. */
extern const display_dcs_module_t display_module_gc9a01_240x240;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_GC9A01_H */
