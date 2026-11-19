/**
 * @file display_jd9853.h
 * @brief JD9853 driver IC and default module.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_JD9853_H
#define DISPLAY_JD9853_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief JD9853 IC description and default initialization sequence. */
extern const display_dcs_ic_t display_ic_jd9853;
/** @brief Default module: JD9853 240x320. */
extern const display_dcs_module_t display_module_jd9853_240x320;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_JD9853_H */
