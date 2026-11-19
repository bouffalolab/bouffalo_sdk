/**
 * @file display_jd9165ba.c
 * @brief JD9165BA IC description and default module: 1024x600 RGB panel with a 3-wire or 4-wire command port.
 *
 * The register pages are selected with R30h: page 0 is the standard command set (sleep out, display on), page 1 has
 * power, interface and timing, page 2 the GOA mapping, page 6 the gamma, pages A and D the source and VCOM tuning.
 *
 * A board without a command port (the panel is configured by its own circuit) defines the panel without an
 * endpoint: DISPLAY_DCS_PANEL_NO_IO(&display_module_jd9165ba_1024x600, board). Then only the reset is done.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "panel/video_jd9165ba/display_jd9165ba.h"

static const display_dcs_cmd_t jd9165ba_init[] = {
    DISPLAY_DCS(0x30, 0, 0x00), /* page 0 */
    DISPLAY_DCS(0xF7, 0, 0x49, 0x61, 0x02, 0x00),
    DISPLAY_DCS(0x30, 0, 0x01), /* page 1: power / interface / timing */
    DISPLAY_DCS(0x04, 0, 0x0C),
    DISPLAY_DCS(0x05, 0, 0x00),
    DISPLAY_DCS(0x06, 0, 0x00),
    DISPLAY_DCS(0x0B, 0, 0x11),
    DISPLAY_DCS(0x17, 0, 0x00),
    DISPLAY_DCS(0x20, 0, 0x04),
    DISPLAY_DCS(0x1F, 0, 0x05),
    DISPLAY_DCS(0x23, 0, 0x00),
    DISPLAY_DCS(0x25, 0, 0x19),
    DISPLAY_DCS(0x28, 0, 0x18),
    DISPLAY_DCS(0x29, 0, 0x04),
    DISPLAY_DCS(0x2A, 0, 0x01),
    DISPLAY_DCS(0x2B, 0, 0x04),
    DISPLAY_DCS(0x2C, 0, 0x01),
    DISPLAY_DCS(0x30, 0, 0x02), /* page 2: GOA mapping */
    DISPLAY_DCS(0x01, 0, 0x22),
    DISPLAY_DCS(0x03, 0, 0x12),
    DISPLAY_DCS(0x04, 0, 0x00),
    DISPLAY_DCS(0x05, 0, 0x64),
    DISPLAY_DCS(0x0A, 0, 0x08),
    DISPLAY_DCS(0x0B, 0, 0x0A, 0x1A, 0x0B, 0x0D, 0x0D, 0x11, 0x10, 0x06, 0x08, 0x1F, 0x1D),
    DISPLAY_DCS(0x0C, 0, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D),
    DISPLAY_DCS(0x0D, 0, 0x16, 0x1B, 0x0B, 0x0D, 0x0D, 0x11, 0x10, 0x07, 0x09, 0x1E, 0x1C),
    DISPLAY_DCS(0x0E, 0, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D),
    DISPLAY_DCS(0x0F, 0, 0x16, 0x1B, 0x0D, 0x0B, 0x0D, 0x11, 0x10, 0x1C, 0x1E, 0x09, 0x07),
    DISPLAY_DCS(0x10, 0, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D),
    DISPLAY_DCS(0x11, 0, 0x0A, 0x1A, 0x0D, 0x0B, 0x0D, 0x11, 0x10, 0x1D, 0x1F, 0x08, 0x06),
    DISPLAY_DCS(0x12, 0, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D),
    DISPLAY_DCS(0x14, 0, 0x00, 0x00, 0x11, 0x11),
    DISPLAY_DCS(0x18, 0, 0x99),
    DISPLAY_DCS(0x30, 0, 0x06), /* page 6: gamma */
    DISPLAY_DCS(0x12, 0, 0x36, 0x2C, 0x2E, 0x3C, 0x38, 0x35, 0x35, 0x32, 0x2E, 0x1D, 0x2B, 0x21, 0x16, 0x29),
    DISPLAY_DCS(0x13, 0, 0x36, 0x2C, 0x2E, 0x3C, 0x38, 0x35, 0x35, 0x32, 0x2E, 0x1D, 0x2B, 0x21, 0x16, 0x29),
    DISPLAY_DCS(0x30, 0, 0x0A), /* page A: source */
    DISPLAY_DCS(0x02, 0, 0x4F),
    DISPLAY_DCS(0x0B, 0, 0x40),
    DISPLAY_DCS(0x12, 0, 0x3E),
    DISPLAY_DCS(0x13, 0, 0x78),
    DISPLAY_DCS(0x30, 0, 0x0D), /* page D: VCOM */
    DISPLAY_DCS(0x0D, 0, 0x04),
    DISPLAY_DCS(0x10, 0, 0x0C),
    DISPLAY_DCS(0x11, 0, 0x0C),
    DISPLAY_DCS(0x12, 0, 0x0C),
    DISPLAY_DCS(0x13, 0, 0x0C),
    DISPLAY_DCS(0x30, 0, 0x00), /* page 0 */
    DISPLAY_DCS0(0x11, 120),    /* Sleep Out */
};

/* Command port of the RGB panel: 3-wire 9-bit or 4-wire SPI */
static const display_link_t jd9165ba_links[] = {
    { .iface = DISPLAY_IFACE_DBI_C3 },
    { .iface = DISPLAY_IFACE_DBI_C4 },
};

/* The RAM size is the size of the panel: the RGB interface does not use the GRAM windows */
const display_dcs_ic_t display_ic_jd9165ba = {
    .name = "jd9165ba",
    .ram_width = 1024,
    .ram_height = 600,
    .reset_low_ms = 10,
    .reset_wait_ms = 100,
    DISPLAY_DCS_LINKS(jd9165ba_links),
    DISPLAY_DCS_INIT(jd9165ba_init),
};

/* 60 Hz: (1024+344) x (600+37) x 60 = 52.28 MHz */
static const display_video_mode_t video_1024x600[] = {
    {
        .link = DISPLAY_VLINK_DPI,
        .width = 1024,
        .height = 600,
        .hsw = 24,
        .hbp = 160,
        .hfp = 160,
        .vsw = 2,
        .vbp = 23,
        .vfp = 12,
        .clock_hz = 52284960,
    },
};

const display_dcs_module_t display_module_jd9165ba_1024x600 = {
    .name = "jd9165ba_1024x600",
    .ic = &display_ic_jd9165ba,
    .width = 1024,
    .height = 600,
    .video_start_delay_ms = 20, /* the display is on, the panel needs this time before the first line */
    DISPLAY_DCS_VIDEO(video_1024x600),
};
