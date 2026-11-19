/**
 * @file display_st77922.h
 * @brief ST77922 driver IC and default modules.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * ST77922 and ST77926 share the register set: one IC description, two default modules that differ in the interface.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_ST77922_H
#define DISPLAY_ST77922_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief ST77922 IC description and default initialization sequence. */
extern const display_dcs_ic_t display_ic_st77922;
/** @brief Default module: ST77922 320x480 on a DBI interface. */
extern const display_dcs_module_t display_module_st77922_320x480;
/** @brief Default module: ST77926 320x480 on QSPI (opcode 0x38, address and data on 4 wires). */
extern const display_dcs_module_t display_module_st77926_320x480;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_ST77922_H */
