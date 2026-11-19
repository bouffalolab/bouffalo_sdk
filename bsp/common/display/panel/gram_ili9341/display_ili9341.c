/**
 * @file display_ili9341.c
 * @brief ILI9341 IC description and default modules.
 *
 * The initialization sequence does not contain MADCTL, COLMOD, INVON and DISPON: the generic DCS driver sends them.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "panel/gram_ili9341/display_ili9341.h"

static const display_dcs_cmd_t ili9341_init[] = {
    DISPLAY_DCS0(0x01, 20),
    DISPLAY_DCS0(0x11, 120), /* Exit sleep */
    DISPLAY_DCS(0xCF, 0, 0x00, 0xD9, 0x30),
    DISPLAY_DCS(0xED, 0, 0x64, 0x03, 0x12, 0x81),
    DISPLAY_DCS(0xE8, 0, 0x85, 0x01, 0x78),
    DISPLAY_DCS(0xCB, 0, 0x39, 0x2C, 0x00, 0x34, 0x02),
    DISPLAY_DCS(0xF7, 0, 0x20),
    DISPLAY_DCS(0xEA, 0, 0x00, 0x00),
    DISPLAY_DCS(0xC0, 0, 0x23), /* Power control */
    DISPLAY_DCS(0xC1, 0, 0x12), /* Power control */
    DISPLAY_DCS(0xC2, 0, 0x11),
    DISPLAY_DCS(0xC5, 0, 0x40, 0x30), /* VCOM control 1 */
    DISPLAY_DCS(0xC7, 0, 0xA9),       /* VCOM control 2 */
    DISPLAY_DCS(0xB1, 0, 0x00, 0x18), /* Frame Rate Control */
    DISPLAY_DCS(0xB6, 0, 0x0A, 0xA2), /* Display Function Control */
    DISPLAY_DCS(0x0C, 0, 0xD5),       /* display pixel format,RGB 16bits,MCU 16bits */
    DISPLAY_DCS(0xF2, 0, 0x00),       /* 3Gamma Function Disable */
    DISPLAY_DCS(0xF7, 0, 0x20),
    DISPLAY_DCS(0x26, 0, 0x01), /* Gamma curve selected */
    DISPLAY_DCS(0xE0, 0, 0x1F, 0x1A, 0x18, 0x0A, 0x0F, 0x06, 0x45, 0x87, 0x32, 0x0A, 0x07, 0x02, 0x07, 0x05,
                0x00), /* Set Gamma */
    DISPLAY_DCS(0xE1, 0, 0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3A, 0x78, 0x4D, 0x05, 0x18, 0x0D, 0x38, 0x3A,
                0x1F), /* Set Gamma */
    DISPLAY_DCS(0xB7, 0, 0x07),
    DISPLAY_DCS(0x35, 0, 0x00), /* Tearing Effect Line On */
};

/* Interfaces of the IC and its settings on each (colmod: 16 bit, 24 bit) */
static const display_link_t ili9341_links[] = {
    { .iface = DISPLAY_IFACE_DBI_B8, .colmod = { 0x55, 0x66 } },
    { .iface = DISPLAY_IFACE_DBI_C4, .colmod = { 0x55, 0x66 } },
    { .iface = DISPLAY_IFACE_DBI_C3, .colmod = { 0x55, 0x66 } },
};

const display_dcs_ic_t display_ic_ili9341 = {
    .name = "ili9341",
    .ram_width = 240,
    .ram_height = 320,
    .x_align = 1,
    .y_align = 1,
    DISPLAY_DCS_LINKS(ili9341_links),
    .reset_low_ms = 10,
    .reset_wait_ms = 120,
    DISPLAY_DCS_INIT(ili9341_init),
};

const display_dcs_module_t display_module_ili9341_240x320 = {
    .name = "ili9341_240x320",
    .ic = &display_ic_ili9341,
    .width = 240,
    .height = 320,
    .rotation_mask = 0x0F,
    .madctl = { 0x00, 0x60, 0xC0, 0xA0 },
};
