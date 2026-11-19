/**
 * @file display_ili9488.c
 * @brief ILI9488 IC description and default modules.
 *
 * The initialization sequence does not contain MADCTL, COLMOD, INVON and DISPON: the generic DCS driver sends them.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "panel/gram_ili9488/display_ili9488.h"

static const display_dcs_cmd_t ili9488_init[] = {
    DISPLAY_DCS0(0x01, 10),  /* software reset */
    DISPLAY_DCS0(0x11, 120), /* Sleep Out */
    DISPLAY_DCS(0xE0, 0, 0x00, 0x07, 0x0F, 0x0D, 0x1B, 0x0A, 0x3C, 0x78, 0x4A, 0x07, 0x0E, 0x09, 0x1B, 0x1E,
                0x0F), /* PGAMCTRL (Positive Gamma Control) */
    DISPLAY_DCS(0xE1, 0, 0x00, 0x22, 0x24, 0x26, 0x12, 0x07, 0x36, 0x47, 0x47, 0x06, 0x0A, 0x07, 0x30, 0x37,
                0x0F),                            /* NGAMCTRL (Negative Gamma Control) */
    DISPLAY_DCS(0xC0, 0, 0x10, 0x10),             /* Power Control 1 */
    DISPLAY_DCS(0xC1, 0, 0x41),                   /* Power Control 2 */
    DISPLAY_DCS(0xC5, 0, 0x00, 0x2C, 0x80),       /* VCOM Control */
    DISPLAY_DCS(0x35, 0, 0x00),                   /* enable Tearing Effect Output line */
    DISPLAY_DCS(0xB0, 0, 0x00),                   /* Interface Mode Control */
    DISPLAY_DCS(0xB1, 0, 0xB0),                   /* Frame rate 70Hz */
    DISPLAY_DCS(0xB4, 0, 0x02),                   /* Display Inversion Control */
    DISPLAY_DCS(0xB6, 0, 0x02, 0x22),             /* Display Function Control */
    DISPLAY_DCS(0xE9, 0, 0x00),                   /* Set Image Function */
    DISPLAY_DCS(0xF7, 0, 0xA9, 0x51, 0x2C, 0x82), /* Adjust Control 3 */
};

/* Interfaces of the IC and its settings on each (colmod: 16 bit, 24 bit) */
static const display_link_t ili9488_links[] = {
    { .iface = DISPLAY_IFACE_DBI_B8, .colmod = { 0x55, 0x66 } },
    { .iface = DISPLAY_IFACE_DBI_C4, .colmod = { 0, 0x66 } }, /* serial: 18-bit pixels only */
    { .iface = DISPLAY_IFACE_DBI_C3, .colmod = { 0, 0x66 } },
};

const display_dcs_ic_t display_ic_ili9488 = {
    .name = "ili9488",
    .ram_width = 320,
    .ram_height = 480,
    .x_align = 1,
    .y_align = 1,
    DISPLAY_DCS_LINKS(ili9488_links),
    .reset_low_ms = 10,
    .reset_wait_ms = 120,
    DISPLAY_DCS_INIT(ili9488_init),
};

const display_dcs_module_t display_module_ili9488_320x480 = {
    .name = "ili9488_320x480",
    .ic = &display_ic_ili9488,
    .width = 320,
    .height = 480,
    .rotation_mask = 0x0F,
    .madctl = { 0x00, 0x60, 0xC0, 0xA0 },
};
