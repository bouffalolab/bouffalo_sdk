/**
 * @file display_module_template.c
 * @brief Template of a panel module description. Copy it into your project; no framework change is needed.
 *
 * Modules built on the same driver IC usually differ in size and GRAM offset, orientation (MADCTL),
 * inversion, gamma and voltages. Choose the style by the amount of difference:
 * - style 1: keep the IC default sequence and append a few registers (e.g. gamma) with `init_extra`;
 * - style 2: the vendor provides a complete table: replace the sequence with `init`.
 * Then reference the module in the static definition: `DISPLAY_DCS_PANEL(&io, &my_st7789_240x280, board)`.
 *
 * The interface the panel is wired to is not part of the module: it comes from the endpoint
 * (`.iface`), and the IC description lists the interfaces the IC supports with its settings on each.
 * Only if the module is strapped for a subset of them, restrict it with
 * `.iface_mask = DISPLAY_IFACE_BIT(DISPLAY_IFACE_DBI_C4)`.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "panel/gram_st7789/display_st7789.h"

/*****************************************************************************
 * Style 1: IC default sequence + module gamma
 *****************************************************************************/

/* Example values only; use the parameters from the module vendor. */
static const display_dcs_cmd_t my_gamma[] = {
    DISPLAY_DCS(0xE0, 0, 0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23),
    DISPLAY_DCS(0xE1, 0, 0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23),
};

/** 1.69" 240x280 rounded panel: the glass is centered in the 240x320 GRAM, 20 lines offset. */
const display_dcs_module_t my_st7789_240x280 = {
    .name = "my_st7789_240x280",
    .ic = &display_ic_st7789, /* reuse the IC description and its default sequence */
    .width = 240,
    .height = 280,
    .rotation_mask = 0x0F,
    .madctl = { 0x00, 0x60, 0xC0, 0xA0 },
    .gap_x = { 0, 20, 0, 20 },
    .gap_y = { 20, 0, 20, 0 },
    .invert = true,
    DISPLAY_DCS_INIT_EXTRA(my_gamma), /* sent after the default sequence */
};

/*****************************************************************************
 * Style 2: replace the whole sequence
 *****************************************************************************/
#if 0
static const display_dcs_cmd_t vendor_init[] = {
    DISPLAY_DCS0(0x01, 10),
    DISPLAY_DCS0(0x11, 120),
    /* ... vendor sequence, without 0x29; GRAM panels without 0x36/0x3A ... */
};

const display_dcs_module_t my_vendor_module = {
    .name = "my_vendor_module",
    .ic = &display_ic_st7789,
    .width = 240,
    .height = 320,
    .rotation_mask = 0x0F,
    .madctl = { 0x00, 0x60, 0xC0, 0xA0 },
    DISPLAY_DCS_INIT(vendor_init), /* replaces the IC default sequence */
};
#endif
