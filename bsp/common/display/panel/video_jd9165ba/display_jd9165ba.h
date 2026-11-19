/**
 * @file display_jd9165ba.h
 * @brief JD9165BA driver IC (1024x600 RGB video panel with a 3-wire or 4-wire command port) and default module.
 *
 * Use the default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c). A board without a command port defines the panel without an
 * endpoint with DISPLAY_DCS_PANEL_NO_IO(): then only the reset is done.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_JD9165BA_H
#define DISPLAY_JD9165BA_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief JD9165BA IC description: command port on DISPLAY_IFACE_DBI_C3 or DISPLAY_IFACE_DBI_C4, RGB pixels. */
extern const display_dcs_ic_t display_ic_jd9165ba;
/** @brief Default module: 1024x600 RGB, 60 Hz. */
extern const display_dcs_module_t display_module_jd9165ba_1024x600;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_JD9165BA_H */
