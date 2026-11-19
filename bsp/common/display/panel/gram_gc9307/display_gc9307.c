/**
 * @file display_gc9307.c
 * @brief GC9307 IC description and default module.
 *
 * The initialization sequence does not contain MADCTL, COLMOD, INVON and DISPON: the generic DCS driver sends them.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "panel/gram_gc9307/display_gc9307.h"

static const display_dcs_cmd_t gc9307_init[] = {
    DISPLAY_DCS0(0xFE, 0),
    DISPLAY_DCS0(0xEF, 0),
    DISPLAY_DCS(0x86, 0, 0x98),
    DISPLAY_DCS(0x89, 0, 0x03),
    DISPLAY_DCS(0x8B, 0, 0x80),
    DISPLAY_DCS(0x8D, 0, 0x22),
    DISPLAY_DCS(0x8E, 0, 0x0F),
    DISPLAY_DCS(0xE8, 0, 0x12, 0x00),
    DISPLAY_DCS(0xC3, 0, 0x47),
    DISPLAY_DCS(0xC4, 0, 0x28),
    DISPLAY_DCS(0xC9, 0, 0x08),
    DISPLAY_DCS(0xFF, 0, 0x62),
    DISPLAY_DCS(0x99, 0, 0x3E),
    DISPLAY_DCS(0x9D, 0, 0x4B),
    DISPLAY_DCS(0xF0, 0, 0x07, 0x0B, 0x0C, 0x0A, 0x06, 0x31),
    DISPLAY_DCS(0xF2, 0, 0x07, 0x07, 0x04, 0x06, 0x06, 0x21),
    DISPLAY_DCS(0xF1, 0, 0x4A, 0x78, 0x76, 0x33, 0x2F, 0xAF),
    DISPLAY_DCS(0xF3, 0, 0x38, 0x74, 0x72, 0x22, 0x28, 0x6F),
    DISPLAY_DCS(0x35, 0, 0x00),
    DISPLAY_DCS(0x44, 0, 0x00, 0x0A),
    DISPLAY_DCS0(0x11, 120),
};

/* Interfaces of the IC and its settings on each (colmod: 16 bit, 24 bit) */
static const display_link_t gc9307_links[] = {
    { .iface = DISPLAY_IFACE_DBI_B8, .colmod = { 0x55, 0x66 } },
    { .iface = DISPLAY_IFACE_DBI_C4, .colmod = { 0x55, 0x66 } },
    { .iface = DISPLAY_IFACE_DBI_C3, .colmod = { 0x55, 0x66 } },
};

const display_dcs_ic_t display_ic_gc9307 = {
    .name = "gc9307",
    .ram_width = 240,
    .ram_height = 320,
    .x_align = 1,
    .y_align = 1,
    DISPLAY_DCS_LINKS(gc9307_links),
    .reset_low_ms = 10,
    .reset_wait_ms = 120,
    DISPLAY_DCS_INIT(gc9307_init),
};

const display_dcs_module_t display_module_gc9307_240x320 = {
    .name = "gc9307_240x320",
    .ic = &display_ic_gc9307,
    .width = 240,
    .height = 320,
    .rotation_mask = 0x0F,
    .madctl = { 0x00, 0x60, 0xC0, 0xA0 },
};
