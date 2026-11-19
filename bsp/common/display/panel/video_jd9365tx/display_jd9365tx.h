/**
 * @file display_jd9365tx.h
 * @brief JD9365TX driver IC (MIPI DSI video panels) and default module.
 *
 * The vendor commands go as MIPI DSI generic long writes, including those with a single parameter. The panel of
 * the default module has a fixed external reset circuit.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_JD9365TX_H
#define DISPLAY_JD9365TX_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief JD9365TX IC description and its initialization sequence. */
extern const display_dcs_ic_t display_ic_jd9365tx;
/** @brief 7KF82 panel, 720x1280, 2 data lanes, RGB888, 850 Mbit/s per lane, about 50 Hz. */
extern const display_dcs_module_t display_module_jd9365tx_720x1280;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_JD9365TX_H */
