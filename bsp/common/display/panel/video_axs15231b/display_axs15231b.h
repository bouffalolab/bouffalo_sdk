/**
 * @file display_axs15231b.h
 * @brief AXS15231B driver IC (MIPI DSI video panels with module firmware) and default module.
 *
 * The panel module initializes the registers of the IC with its own firmware. The host only wakes the panel
 * up and selects the pixel format, then sends video.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_AXS15231B_H
#define DISPLAY_AXS15231B_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief AXS15231B IC description and its initialization sequence. */
extern const display_dcs_ic_t display_ic_axs15231b;
/** @brief HS035VB35PGR panel, 172x640 portrait, 1 data lane, RGB565, 400 Mbit/s per lane, about 60 Hz. */
extern const display_dcs_module_t display_module_axs15231b_172x640;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_AXS15231B_H */
