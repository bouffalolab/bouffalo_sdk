/**
 * @file display_axs15231b.c
 * @brief AXS15231B IC description and default module (HS035VB35PGR panel).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "panel/video_axs15231b/display_axs15231b.h"

/* The module firmware owns the IC registers, so there is no vendor register table */
static const display_dcs_cmd_t axs15231b_init[] = {
    DISPLAY_DCS0(0x11, 120),    /* Sleep out */
    DISPLAY_DCS(0x3A, 0, 0x05), /* Pixel format: RGB565 */
};

static const display_link_t axs15231b_links[] = {
    { .iface = DISPLAY_IFACE_DSI_LP },
};

const display_dcs_ic_t display_ic_axs15231b = {
    .name = "axs15231b",
    .reset_low_ms = 10,
    .reset_wait_ms = 120,
    DISPLAY_DCS_LINKS(axs15231b_links),
    DISPLAY_DCS_INIT(axs15231b_init),
};

/*
 * 160 MHz / 11 = 14.55 MHz: 302 x 804 clocks per frame give about 60 Hz. RGB565 over one lane needs about
 * 233 Mbit/s, so 400 Mbit/s leaves headroom.
 */
static const display_video_mode_t video_172x640[] = {
    {
        .link = DISPLAY_VLINK_DSI,
        .width = 172,
        .height = 640,
        .hsw = 10,
        .hbp = 60,
        .hfp = 60,
        .vsw = 4,
        .vbp = 120,
        .vfp = 40,
        .clock_hz = 14545454,
        .dsi = { .lanes = 1, .lane_mbps = 400, .format = DISPLAY_DSI_FMT_RGB565 },
    },
};

const display_dcs_module_t display_module_axs15231b_172x640 = {
    .name = "axs15231b_172x640",
    .ic = &display_ic_axs15231b,
    .width = 172,
    .height = 640,
    .video_start_delay_ms = 20, /* the display is on, the module needs a moment before the first line */
    DISPLAY_DCS_VIDEO(video_172x640),
};
