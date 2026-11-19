/**
 * @file display_ili9806e.c
 * @brief ILI9806E IC description and default module (KD050FWFIA019 panel).
 *
 * The table is the complete vendor script (the 180 degree variant; the other script stops after the GIP table
 * and never sends tear-on, pixel format, sleep out and display on). The pixel format is RGB565 because the DSI
 * link carries RGB565; the vendor script has 24 bit. The panel supplies are VCI = 3.3 V and IOVCC = 1.8 V.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "panel/video_ili9806e/display_ili9806e.h"

static const display_dcs_cmd_t ili9806e_init[] = {
    /* Page 1 - power / VCOM / gamma */
    DISPLAY_DCS(0xFF, 0, 0xFF, 0x98, 0x06, 0x04, 0x01),
    DISPLAY_DCS(0x08, 0, 0x10), /* Output SDA */
    DISPLAY_DCS(0x20, 0, 0x00), /* set DE/VSYNC mode */
    DISPLAY_DCS(0x21, 0, 0x01), /* DE = 1 Active */
    DISPLAY_DCS(0x30, 0, 0x01), /* Resolution setting 480 x 854 */
    DISPLAY_DCS(0x31, 0, 0x00), /* Inversion setting 2-dot */
    DISPLAY_DCS(0x40, 0, 0x16), /* BT AVDD */
    DISPLAY_DCS(0x41, 0, 0x33),
    DISPLAY_DCS(0x42, 0, 0x03),
    DISPLAY_DCS(0x43, 0, 0x09),
    DISPLAY_DCS(0x44, 0, 0x06),
    DISPLAY_DCS(0x50, 0, 0x88), /* VREG1 */
    DISPLAY_DCS(0x51, 0, 0x88), /* VREG2 */
    DISPLAY_DCS(0x52, 0, 0x00), /* Flicker MSB */
    DISPLAY_DCS(0x53, 0, 0x49), /* Flicker LSB / VCOM */
    DISPLAY_DCS(0x55, 0, 0x49), /* Flicker */
    DISPLAY_DCS(0x60, 0, 0x07),
    DISPLAY_DCS(0x61, 0, 0x00),
    DISPLAY_DCS(0x62, 0, 0x07),
    DISPLAY_DCS(0x63, 0, 0x00),
    /* Positive gamma */
    DISPLAY_DCS(0xA0, 0, 0x00),
    DISPLAY_DCS(0xA1, 0, 0x09),
    DISPLAY_DCS(0xA2, 0, 0x11),
    DISPLAY_DCS(0xA3, 0, 0x0B),
    DISPLAY_DCS(0xA4, 0, 0x05),
    DISPLAY_DCS(0xA5, 0, 0x08),
    DISPLAY_DCS(0xA6, 0, 0x06),
    DISPLAY_DCS(0xA7, 0, 0x04),
    DISPLAY_DCS(0xA8, 0, 0x09),
    DISPLAY_DCS(0xA9, 0, 0x0C),
    DISPLAY_DCS(0xAA, 0, 0x15),
    DISPLAY_DCS(0xAB, 0, 0x08),
    DISPLAY_DCS(0xAC, 0, 0x0F),
    DISPLAY_DCS(0xAD, 0, 0x12),
    DISPLAY_DCS(0xAE, 0, 0x09),
    DISPLAY_DCS(0xAF, 0, 0x00),
    /* Negative gamma */
    DISPLAY_DCS(0xC0, 0, 0x00),
    DISPLAY_DCS(0xC1, 0, 0x09),
    DISPLAY_DCS(0xC2, 0, 0x10),
    DISPLAY_DCS(0xC3, 0, 0x0C),
    DISPLAY_DCS(0xC4, 0, 0x05),
    DISPLAY_DCS(0xC5, 0, 0x08),
    DISPLAY_DCS(0xC6, 0, 0x06),
    DISPLAY_DCS(0xC7, 0, 0x04),
    DISPLAY_DCS(0xC8, 0, 0x08),
    DISPLAY_DCS(0xC9, 0, 0x0C),
    DISPLAY_DCS(0xCA, 0, 0x14),
    DISPLAY_DCS(0xCB, 0, 0x08),
    DISPLAY_DCS(0xCC, 0, 0x0F),
    DISPLAY_DCS(0xCD, 0, 0x11),
    DISPLAY_DCS(0xCE, 0, 0x09),
    DISPLAY_DCS(0xCF, 0, 0x00),
    /* Page 6 - GIP timing 1 */
    DISPLAY_DCS(0xFF, 0, 0xFF, 0x98, 0x06, 0x04, 0x06),
    DISPLAY_DCS(0x00, 0, 0x20),
    DISPLAY_DCS(0x01, 0, 0x0A),
    DISPLAY_DCS(0x02, 0, 0x00),
    DISPLAY_DCS(0x03, 0, 0x00),
    DISPLAY_DCS(0x04, 0, 0x01),
    DISPLAY_DCS(0x05, 0, 0x01),
    DISPLAY_DCS(0x06, 0, 0x98),
    DISPLAY_DCS(0x07, 0, 0x06),
    DISPLAY_DCS(0x08, 0, 0x01),
    DISPLAY_DCS(0x09, 0, 0x80),
    DISPLAY_DCS(0x0A, 0, 0x00),
    DISPLAY_DCS(0x0B, 0, 0x00),
    DISPLAY_DCS(0x0C, 0, 0x01),
    DISPLAY_DCS(0x0D, 0, 0x01),
    DISPLAY_DCS(0x0E, 0, 0x05),
    DISPLAY_DCS(0x0F, 0, 0x00),
    DISPLAY_DCS(0x10, 0, 0xF0),
    DISPLAY_DCS(0x11, 0, 0xF4),
    DISPLAY_DCS(0x12, 0, 0x01),
    DISPLAY_DCS(0x13, 0, 0x00),
    DISPLAY_DCS(0x14, 0, 0x00),
    DISPLAY_DCS(0x15, 0, 0xC0),
    DISPLAY_DCS(0x16, 0, 0x08),
    DISPLAY_DCS(0x17, 0, 0x00),
    DISPLAY_DCS(0x18, 0, 0x00),
    DISPLAY_DCS(0x19, 0, 0x00),
    DISPLAY_DCS(0x1A, 0, 0x00),
    DISPLAY_DCS(0x1B, 0, 0x00),
    DISPLAY_DCS(0x1C, 0, 0x00),
    DISPLAY_DCS(0x1D, 0, 0x00),
    /* GIP timing 2 */
    DISPLAY_DCS(0x20, 0, 0x01),
    DISPLAY_DCS(0x21, 0, 0x23),
    DISPLAY_DCS(0x22, 0, 0x45),
    DISPLAY_DCS(0x23, 0, 0x67),
    DISPLAY_DCS(0x24, 0, 0x01),
    DISPLAY_DCS(0x25, 0, 0x23),
    DISPLAY_DCS(0x26, 0, 0x45),
    DISPLAY_DCS(0x27, 0, 0x67),
    /* GIP timing 3 */
    DISPLAY_DCS(0x30, 0, 0x11),
    DISPLAY_DCS(0x31, 0, 0x11),
    DISPLAY_DCS(0x32, 0, 0x00),
    DISPLAY_DCS(0x33, 0, 0xEE),
    DISPLAY_DCS(0x34, 0, 0xFF),
    DISPLAY_DCS(0x35, 0, 0xBB),
    DISPLAY_DCS(0x36, 0, 0xAA),
    DISPLAY_DCS(0x37, 0, 0xDD),
    DISPLAY_DCS(0x38, 0, 0xCC),
    DISPLAY_DCS(0x39, 0, 0x66),
    DISPLAY_DCS(0x3A, 0, 0x77),
    DISPLAY_DCS(0x3B, 0, 0x22),
    DISPLAY_DCS(0x3C, 0, 0x22),
    DISPLAY_DCS(0x3D, 0, 0x22),
    DISPLAY_DCS(0x3E, 0, 0x22),
    DISPLAY_DCS(0x3F, 0, 0x22),
    DISPLAY_DCS(0x40, 0, 0x22),
    /* Page 7 - GIP timing 4 */
    DISPLAY_DCS(0xFF, 0, 0xFF, 0x98, 0x06, 0x04, 0x07),
    DISPLAY_DCS(0x17, 0, 0x22),
    DISPLAY_DCS(0x02, 0, 0x77),
    DISPLAY_DCS(0x26, 0, 0xB2),
    DISPLAY_DCS(0xFF, 0, 0xFF, 0x98, 0x06, 0x04, 0x00), /* Page 0: normal commands */
    DISPLAY_DCS0(0x35, 0),                              /* Tearing effect on (V-blank) */
    DISPLAY_DCS(0x3A, 0, 0x05),                         /* Pixel format: RGB565 (DBI[2:0] = 101) */
    DISPLAY_DCS0(0x11, 120),                            /* Sleep out */
};

static const display_link_t ili9806e_links[] = {
    { .iface = DISPLAY_IFACE_DSI_LP },
};

const display_dcs_ic_t display_ic_ili9806e = {
    .name = "ili9806e",
    .reset_low_ms = 10,
    .reset_wait_ms = 120,
    DISPLAY_DCS_LINKS(ili9806e_links),
    DISPLAY_DCS_INIT(ili9806e_init),
};

/*
 * The panel asks for 29 MHz, the closest tap of the display clock is 240 MHz / 8 = 30 MHz: 532 x 908 clocks per
 * frame give about 62 Hz. The porches are the specified ones. The link needs only about 240 Mbit/s per lane
 * (480x854 RGB565): the IC does not take the 850 Mbit/s profile of the ILI9881C panels (the panel stays black), so
 * the 550 Mbit/s step is used.
 */
static const display_video_mode_t video_480x854[] = {
    {
        .link = DISPLAY_VLINK_DSI,
        .width = 480,
        .height = 854,
        .hsw = 4,
        .hbp = 30,
        .hfp = 18,
        .vsw = 4,
        .vbp = 30,
        .vfp = 20,
        .clock_hz = 30000000,
        .dsi = { .lanes = 2, .lane_mbps = 550, .format = DISPLAY_DSI_FMT_RGB565 },
    },
};

const display_dcs_module_t display_module_ili9806e_480x854 = {
    .name = "ili9806e_480x854",
    .ic = &display_ic_ili9806e,
    .width = 480,
    .height = 854,
    .video_start_delay_ms = 25, /* the display is on, the panel needs this time before the first line */
    DISPLAY_DCS_VIDEO(video_480x854),
};
