/**
 * @file display_st7796.c
 * @brief ST7796 IC description and default modules.
 *
 * The initialization sequence does not contain MADCTL, COLMOD, INVON and DISPON: the generic DCS driver sends them.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "panel/gram_st7796/display_st7796.h"

static const display_dcs_cmd_t st7796_init[] = {
    DISPLAY_DCS0(0x01, 10),
    DISPLAY_DCS0(0x11, 120), /* Exit sleep */
    DISPLAY_DCS(0xF0, 0, 0xC3),
    DISPLAY_DCS(0xF0, 0, 0x96),
    DISPLAY_DCS(0xE6, 0, 0x0F, 0xF2, 0x3F, 0x4F, 0x4F, 0x28, 0x0E, 0x00),
    DISPLAY_DCS(0xC5, 0, 0x2A),
    DISPLAY_DCS(0xE0, 0, 0xF0, 0x03, 0x0A, 0x11, 0x14, 0x1C, 0x3B, 0x55, 0x4A, 0x0A, 0x13, 0x14, 0x1C,
                0x1F), /* Set Gamma */
    DISPLAY_DCS(0xE1, 0, 0xF0, 0x03, 0x0A, 0x0C, 0x0C, 0x09, 0x36, 0x54, 0x49, 0x0F, 0x1B, 0x18, 0x1B,
                0x1F), /* Set Gamma */
    DISPLAY_DCS(0xF0, 0, 0x3C),
    DISPLAY_DCS(0xF0, 0, 0x69),
    DISPLAY_DCS(0x35, 0, 0x00), /* Tearing Effect Line On */
};

/* 1-dot column inversion (vendor registers): set together with INVON */
static const display_dcs_cmd_t st7796_invert[] = {
    DISPLAY_DCS(0xF0, 0, 0xC3), DISPLAY_DCS(0xF0, 0, 0x96), DISPLAY_DCS(0xB4, 0, 0x01),
    DISPLAY_DCS(0xF0, 0, 0x3C), DISPLAY_DCS(0xF0, 0, 0x69),
};

/* Interfaces of the IC and its settings on each (colmod: 16 bit, 24 bit) */
static const display_link_t st7796_links[] = {
    { .iface = DISPLAY_IFACE_DBI_B8, .colmod = { 0x55, 0x66 } },
    { .iface = DISPLAY_IFACE_DBI_C4, .colmod = { 0x55, 0x66 } },
    { .iface = DISPLAY_IFACE_DBI_C3, .colmod = { 0x55, 0x66 } },
};

const display_dcs_ic_t display_ic_st7796 = {
    .name = "st7796",
    .ram_width = 320,
    .ram_height = 480,
    .x_align = 1,
    .y_align = 1,
    DISPLAY_DCS_LINKS(st7796_links),
    .reset_low_ms = 10,
    .reset_wait_ms = 120,
    DISPLAY_DCS_INIT(st7796_init),
    DISPLAY_DCS_INVERT_SEQ(st7796_invert),
};

const display_dcs_module_t display_module_st7796_320x480 = {
    .name = "st7796_320x480",
    .ic = &display_ic_st7796,
    .width = 320,
    .height = 480,
    .rotation_mask = 0x0F,
    .madctl = { 0x48, 0xE8, 0x88, 0x28 }, /* BGR glass, mirrored X at each rotation */
    .invert = true,
};
