/**
 * @file display_st7735.h
 * @brief ST7735 driver IC and default module.
 *
 * Use a default module directly, or write your own module description in your project
 * (see panel/template/display_module_template.c).
 *
 * @note The default module is 240x320; a 128x160 panel needs its own module (size and GRAM offsets).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_ST7735_H
#define DISPLAY_ST7735_H

#include "panel/dcs/display_panel_dcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY_PANEL_DCS
 * @{
 */

/** @brief ST7735 IC description and default initialization sequence. */
extern const display_dcs_ic_t display_ic_st7735;
/** @brief Default module: ST7735, 240x320. */
extern const display_dcs_module_t display_module_st7735_240x320;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_ST7735_H */
