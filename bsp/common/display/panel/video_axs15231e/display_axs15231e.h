/**
 * @file display_axs15231e.h
 * @brief AXS15231E driver IC (MIPI DSI video panels) and default module.
 *
 * The IC takes its register table as MIPI DSI generic long writes.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_AXS15231E_H
#define DISPLAY_AXS15231E_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief AXS15231E IC description and its initialization sequence. */
extern const display_dcs_ic_t display_ic_axs15231e;
/** @brief JX 3.71 inch panel, 258x960 portrait, 1 data lane, RGB565, 400 Mbit/s per lane, about 50 Hz. */
extern const display_dcs_module_t display_module_axs15231e_258x960;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_AXS15231E_H */
