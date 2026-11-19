/**
 * @file display_spd2010.h
 * @brief SPD2010 driver IC and default module.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * The register pages are selected with 0xFF 0x20 0x10 <page>; 0x36 and 0x3A inside a page are page registers, not
 * MADCTL / COLMOD.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_SPD2010_H
#define DISPLAY_SPD2010_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief SPD2010 IC description and default initialization sequence. */
extern const display_dcs_ic_t display_ic_spd2010;
/** @brief Default module: SPD2010 412x412 on QSPI (opcode 0x32, address on 1 wire, data on 4 wires). */
extern const display_dcs_module_t display_module_spd2010_412x412;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_SPD2010_H */
