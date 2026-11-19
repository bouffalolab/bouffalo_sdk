/**
 * @file display_fl7707n.h
 * @brief FL7707N driver IC (MIPI DSI video panels) and default module.
 *
 * The vendor register table declares the 2-lane wiring to the IC itself (0xBA parameter 1), so the table and the
 * module go together.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_FL7707N_H
#define DISPLAY_FL7707N_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief FL7707N IC description and its initialization sequence (2 lanes). */
extern const display_dcs_ic_t display_ic_fl7707n;
/** @brief HD395003C30-V2 panel, 720x720, 2 data lanes, RGB888, 550 Mbit/s per lane, about 62 Hz. */
extern const display_dcs_module_t display_module_fl7707n_720x720;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_FL7707N_H */
