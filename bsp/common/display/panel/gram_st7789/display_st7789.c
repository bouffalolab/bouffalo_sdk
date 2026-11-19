/**
 * @file display_st7789.c
 * @brief ST7789 IC description and default modules.
 *
 * The initialization sequence does not contain MADCTL, COLMOD, INVON and DISPON: the generic DCS driver sends them.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "panel/gram_st7789/display_st7789.h"

static const display_dcs_cmd_t st7789_init[] = {
    DISPLAY_DCS0(0x11, 120), /* Sleep Out */
    DISPLAY_DCS(0x44, 0, 0x00, 0x00),
    DISPLAY_DCS(0xB2, 0, 0x0C, 0x0C, 0x00, 0x33, 0x33), /* Porch Setting */
    DISPLAY_DCS(0xB7, 0, 0x35),                         /* Gate Control */
    DISPLAY_DCS(0xBB, 0, 0x19),                         /* VCOM Setting */
    DISPLAY_DCS(0xC0, 0, 0x2C),                         /* LCM Control */
    DISPLAY_DCS(0xC2, 0, 0x01),                         /* VDV and VRH Command Enable */
    DISPLAY_DCS(0xC3, 0, 0x12),                         /* VRH Set */
    DISPLAY_DCS(0xC4, 0, 0x20),                         /* VDV Set */
    DISPLAY_DCS(0xC6, 0, 0x10),                         /* Frame Rate Control in Normal Mode */
    DISPLAY_DCS(0xD0, 0, 0xA4, 0xA1),                   /* Power Control 1 */
    DISPLAY_DCS(0xE0, 0, 0xF0, 0x08, 0x0E, 0x09, 0x08, 0x16, 0x33, 0x43, 0x4A, 0x38, 0x14, 0x14, 0x2E,
                0x32), /* Positive Voltage Gamma Control */
    DISPLAY_DCS(0xE1, 0, 0xF0, 0x0A, 0x10, 0x0A, 0x09, 0x04, 0x33, 0x33, 0x49, 0x0A, 0x16, 0x15, 0x2C,
                0x30),          /* Negative Voltage Gamma Control */
    DISPLAY_DCS(0x35, 0, 0x00), /* Tearing Effect Line On */
};

/* Interfaces of the IC and its settings on each (colmod: 16 bit, 24 bit) */
static const display_link_t st7789_links[] = {
    { .iface = DISPLAY_IFACE_DBI_B8, .colmod = { 0x05, 0x66 } }, /* 18 bit as the 24-bit class */
    { .iface = DISPLAY_IFACE_DBI_C4, .colmod = { 0x05, 0x66 } },
    { .iface = DISPLAY_IFACE_DBI_C3, .colmod = { 0x05, 0x66 } },
};

const display_dcs_ic_t display_ic_st7789 = {
    .name = "st7789",
    .ram_width = 240,
    .ram_height = 320,
    .x_align = 1,
    .y_align = 1,
    DISPLAY_DCS_LINKS(st7789_links),
    .reset_low_ms = 10,
    .reset_wait_ms = 120,
    DISPLAY_DCS_INIT(st7789_init),
};

const display_dcs_module_t display_module_st7789_240x320 = {
    .name = "st7789_240x320",
    .ic = &display_ic_st7789,
    .width = 240,
    .height = 320,
    .rotation_mask = 0x0F,
    .madctl = { 0x00, 0x60, 0xC0, 0xA0 },
};

/* The 240x240 glass sits at one end of the 240x320 GRAM: 80 lines/columns offset at 180 and 270 degrees. */
const display_dcs_module_t display_module_st7789_240x240 = {
    .name = "st7789_240x240",
    .ic = &display_ic_st7789,
    .width = 240,
    .height = 240,
    .rotation_mask = 0x0F,
    .madctl = { 0x00, 0x60, 0xC0, 0xA0 },
    .gap_x = { 0, 0, 0, 80 },
    .gap_y = { 0, 0, 80, 0 },
    .invert = true,
};
