/**
 * @file display_nt35510.c
 * @brief NT35510 IC description and default modules.
 *
 * The initialization sequence does not contain MADCTL, COLMOD, INVON and DISPON: the generic DCS driver sends them.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "panel/gram_nt35510/display_nt35510.h"

/* Gamma curve shared by D1..D6, 52 bytes each */
#define NT35510_GAMMA                                                                                                 \
    0x00, 0x01, 0x00, 0x1C, 0x00, 0x4E, 0x00, 0x6A, 0x00, 0x85, 0x00, 0xAB, 0x00, 0xC4, 0x00, 0xFC, 0x01, 0x23, 0x01, \
        0x61, 0x01, 0x94, 0x01, 0xE4, 0x02, 0x27, 0x02, 0x29, 0x02, 0x65, 0x02, 0xA6, 0x02, 0xCA, 0x02, 0xFD, 0x03,   \
        0x1D, 0x03, 0x4D, 0x03, 0x6A, 0x03, 0x95, 0x03, 0xAC, 0x03, 0xCB, 0x03, 0xEA, 0x03, 0xEF

static const display_dcs_cmd_t nt35510_init[] = {
    DISPLAY_DCS0(0x01, 10),  /* software reset */
    DISPLAY_DCS0(0x11, 120), /* Sleep Out */
    DISPLAY_DCS(0xFF, 0, 0xAA, 0x55, 0xA5, 0x80),
    DISPLAY_DCS(0xF2, 0, 0x00, 0x84, 0x08),
    DISPLAY_DCS(0x6F, 0, 0x0A),
    DISPLAY_DCS(0xF4, 0, 0x13),
    DISPLAY_DCS(0xFF, 0, 0xAA, 0x55, 0xA5, 0x00),
    DISPLAY_DCS(0xFF, 0, 0x55, 0xAA, 0x52, 0x08, 0x01), /* page 1: 0xFF as in the tested driver */
    DISPLAY_DCS(0xB0, 0, 0x0C, 0x0C, 0x0C),             /* AVDD Set AVDD 5.2V */
    DISPLAY_DCS(0xB6, 0, 0x46, 0x46, 0x46),             /* AVDD ratio */
    DISPLAY_DCS(0xB1, 0, 0x0C, 0x0C, 0x0C),             /* AVEE  -5.2V */
    DISPLAY_DCS(0xB7, 0, 0x26, 0x26, 0x26),             /* AVEE ratio */
    DISPLAY_DCS(0xB2, 0, 0x00, 0x00, 0x00),             /* VCL -2.5V */
    DISPLAY_DCS(0xB8, 0, 0x34, 0x34, 0x34),             /* VCL -2.5V */
    DISPLAY_DCS(0xBF, 0, 0x01),
    DISPLAY_DCS(0xB3, 0, 0x08, 0x08, 0x08), /* VGH 15V (Free pump) */
    DISPLAY_DCS(0xB9, 0, 0x26, 0x26, 0x26), /* VGH ratio */
    DISPLAY_DCS(0xB5, 0, 0x08, 0x08, 0x08), /* VGL_REG -10V */
    DISPLAY_DCS(0xBA, 0, 0x36, 0x36, 0x36), /* VGLX ratio */
    DISPLAY_DCS(0xBC, 0, 0x00, 0x80, 0x00), /* VGMP/VGSP 4.5V/0V */
    DISPLAY_DCS(0xBD, 0, 0x00, 0x80, 0x00), /* VGMP/VGSP 4.5V/0V */
    DISPLAY_DCS(0xBE, 0, 0x00, 0x55),       /* VCOM */
    DISPLAY_DCS(0xD1, 0, NT35510_GAMMA),
    DISPLAY_DCS(0xD2, 0, NT35510_GAMMA),
    DISPLAY_DCS(0xD3, 0, NT35510_GAMMA),
    DISPLAY_DCS(0xD4, 0, NT35510_GAMMA),
    DISPLAY_DCS(0xD5, 0, NT35510_GAMMA),
    DISPLAY_DCS(0xD6, 0, NT35510_GAMMA),
    DISPLAY_DCS(0xF0, 0, 0x55, 0xAA, 0x52, 0x08, 0x00), /* enable page 0 */
    DISPLAY_DCS(0xB1, 0, 0xCC, 0x00), /* Display control, MCU I/F & mipi cmd mode: CC, RGB I/F  & mipi video mode: FC */
    DISPLAY_DCS(0xB4, 0, 0x10),
    DISPLAY_DCS(0xB6, 0, 0x05),                         /* Source hold time */
    DISPLAY_DCS(0xB7, 0, 0x70, 0x70),                   /* Gate EQ control */
    DISPLAY_DCS(0xB8, 0, 0x01, 0x05, 0x05, 0x05),       /* Source EQ control (Mode 2) */
    DISPLAY_DCS(0xBC, 0, 0x02, 0x00, 0x00),             /* Inversion mode */
    DISPLAY_DCS(0xCC, 0, 0x03, 0x50, 0x50),             /* BOE's Setting (default) */
    DISPLAY_DCS(0xBD, 0, 0x01, 0x84, 0x07, 0x31, 0x00), /* Porch Adjust */
    DISPLAY_DCS(0xBE, 0, 0x01, 0x84, 0x07, 0x31, 0x00),
    DISPLAY_DCS(0xBF, 0, 0x01, 0x84, 0x07, 0x31, 0x00),
    DISPLAY_DCS(0x35, 0, 0x00),
};

/* Interfaces of the IC and its settings on each (colmod: 16 bit, 24 bit) */
static const display_link_t nt35510_links[] = {
    /* 8080: 16-bit addressing, every parameter is addressed separately */
    { .iface = DISPLAY_IFACE_DBI_B8, .cmd_mode = DISPLAY_CMD_16_INDEXED, .colmod = { 0x55, 0x77 } },
};

const display_dcs_ic_t display_ic_nt35510 = {
    .name = "nt35510",
    .ram_width = 480,
    .ram_height = 800,
    .x_align = 1,
    .y_align = 1,
    DISPLAY_DCS_LINKS(nt35510_links),
    .reset_low_ms = 10,
    .reset_wait_ms = 120,
    DISPLAY_DCS_INIT(nt35510_init),
};

const display_dcs_module_t display_module_nt35510_480x800 = {
    .name = "nt35510_480x800",
    .ic = &display_ic_nt35510,
    .width = 480,
    .height = 800,
    .rotation_mask = 0x0F,
    .madctl = { 0x00, 0x60, 0xC0, 0xA0 },
};
