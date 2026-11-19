/**
 * @file display_nt35510.h
 * @brief NT35510 driver IC and default module.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * @note The command encoding depends on the interface and is part of the IC link: 16-bit addressing on
 *       8080 (DISPLAY_IFACE_DBI_B8); standard DCS on MIPI DSI once that interface exists.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_NT35510_H
#define DISPLAY_NT35510_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief NT35510 IC description and default initialization sequence. */
extern const display_dcs_ic_t display_ic_nt35510;
/** @brief Default module: 480x800. */
extern const display_dcs_module_t display_module_nt35510_480x800;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_NT35510_H */
