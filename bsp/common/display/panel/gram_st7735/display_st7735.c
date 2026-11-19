/**
 * @file display_st7735.c
 * @brief ST7735 IC description and default module.
 *
 * The initialization sequence does not contain MADCTL, COLMOD, INVON and DISPON: the generic DCS driver sends them.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "panel/gram_st7735/display_st7735.h"

static const display_dcs_cmd_t st7735_init[] = {
    DISPLAY_DCS(0x30, 0, 0x00, 0x01, 0x00, 0xA0),
    DISPLAY_DCS(0xB1, 0, 0x05, 0x59, 0x59),
    DISPLAY_DCS(0xB2, 0, 0x05, 0x59, 0x59),
    DISPLAY_DCS(0xB3, 0, 0x05, 0x59, 0x59, 0x05, 0x59, 0x59),
    DISPLAY_DCS(0xC0, 0, 0x62, 0x02, 0x04),
    DISPLAY_DCS(0xC1, 0, 0xC0),
    DISPLAY_DCS(0xC4, 0, 0x8D, 0xEE),
    DISPLAY_DCS(0xC5, 0, 0x0E),
    DISPLAY_DCS(0xE0, 0, 0x10, 0x0E, 0x02, 0x03, 0x0E, 0x07, 0x02, 0x07, 0x0A, 0x12, 0x27, 0x37, 0x00, 0x0D, 0x0E,
                0x10),
    DISPLAY_DCS(0xE1, 0, 0x10, 0x0E, 0x03, 0x03, 0x0F, 0x06, 0x02, 0x08, 0x0A, 0x13, 0x26, 0x36, 0x00, 0x0D, 0x0E,
                0x10),
    DISPLAY_DCS0(0x11, 120),
    DISPLAY_DCS(0x35, 0, 0x00), /* Tearing Effect Line On */
};

/* Interfaces of the IC and its settings on each (colmod: 16 bit, 24 bit) */
static const display_link_t st7735_links[] = {
    { .iface = DISPLAY_IFACE_DBI_C4, .colmod = { 0x05, 0x06 } },
    { .iface = DISPLAY_IFACE_DBI_C3, .colmod = { 0x05, 0x06 } },
};

const display_dcs_ic_t display_ic_st7735 = {
    .name = "st7735",
    .ram_width = 240,
    .ram_height = 320,
    .x_align = 1,
    .y_align = 1,
    DISPLAY_DCS_LINKS(st7735_links),
    .reset_low_ms = 10,
    .reset_wait_ms = 120,
    DISPLAY_DCS_INIT(st7735_init),
};

const display_dcs_module_t display_module_st7735_240x320 = {
    .name = "st7735_240x320",
    .ic = &display_ic_st7735,
    .width = 240,
    .height = 320,
    .rotation_mask = 0x0F,
    .madctl = { 0x00, 0x60, 0xC0, 0xA0 },
};
