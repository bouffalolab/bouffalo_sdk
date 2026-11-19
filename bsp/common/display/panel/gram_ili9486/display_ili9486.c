/**
 * @file display_ili9486.c
 * @brief ILI9486 IC description and default module.
 *
 * The initialization sequence does not contain MADCTL, COLMOD, INVON and DISPON: the generic DCS driver sends them.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "panel/gram_ili9486/display_ili9486.h"

static const display_dcs_cmd_t ili9486_init[] = {
    DISPLAY_DCS0(0x01, 10),
    DISPLAY_DCS0(0x11, 120),
    DISPLAY_DCS(0xF1, 0, 0x36, 0x04, 0x00, 0x3C, 0x0F, 0x8F),
    DISPLAY_DCS(0xF2, 0, 0x18, 0xA3, 0x12, 0x02, 0xB2, 0x12, 0xFF, 0x10, 0x00),
    DISPLAY_DCS(0xF8, 0, 0x21, 0x04),
    DISPLAY_DCS(0xF9, 0, 0x00, 0x08),
    DISPLAY_DCS(0xC0, 0, 0x13, 0x13),
    DISPLAY_DCS(0xC1, 0, 0x42),
    DISPLAY_DCS(0xC2, 0, 0x22),
    DISPLAY_DCS(0xC5, 0, 0x00, 0x01),
    DISPLAY_DCS(0xB1, 0, 0xC0, 0x11),
    DISPLAY_DCS(0xB4, 0, 0x00),
    DISPLAY_DCS(0xB6, 0, 0x00, 0x42, 0x3B),
    DISPLAY_DCS(0xB7, 0, 0xC6),
    DISPLAY_DCS(0xE0, 0, 0x00, 0x14, 0x13, 0x0E, 0x0E, 0x05, 0x49, 0x78, 0x3F, 0x0A, 0x16, 0x09, 0x10, 0x06, 0x00),
    DISPLAY_DCS(0xE1, 0, 0x0F, 0x38, 0x33, 0x0A, 0x0B, 0x03, 0x45, 0x42, 0x30, 0x04, 0x0F, 0x03, 0x19, 0x14, 0x0F),
    DISPLAY_DCS(0x35, 0, 0x00),
};

/* Interfaces of the IC and its settings on each (colmod: 16 bit, 24 bit) */
static const display_link_t ili9486_links[] = {
    { .iface = DISPLAY_IFACE_DBI_B8, .colmod = { 0x55, 0x66 } },
    { .iface = DISPLAY_IFACE_DBI_C4, .colmod = { 0x00, 0x66 } },
    { .iface = DISPLAY_IFACE_DBI_C3, .colmod = { 0x00, 0x66 } },
};

const display_dcs_ic_t display_ic_ili9486 = {
    .name = "ili9486",
    .ram_width = 320,
    .ram_height = 480,
    .x_align = 1,
    .y_align = 1,
    DISPLAY_DCS_LINKS(ili9486_links),
    .reset_low_ms = 10,
    .reset_wait_ms = 120,
    DISPLAY_DCS_INIT(ili9486_init),
};

const display_dcs_module_t display_module_ili9486_320x480 = {
    .name = "ili9486_320x480",
    .ic = &display_ic_ili9486,
    .width = 320,
    .height = 480,
    .rotation_mask = 0x0F,
    .madctl = { 0x00, 0x60, 0xC0, 0xA0 },
};
