/**
 * @file display_ek79007.c
 * @brief EK79007 IC description and default modules (WKS70WSV114 panel).
 *
 * The registers 0x80..0x86 are the internal power / timing registers, 0xB1 selects the RGB output.
 * 0xB2 selects the 2-lane MIPI input; the IC defaults to 4 lanes.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "panel/video_ek79007/display_ek79007.h"

static const display_dcs_cmd_t ek79007_init[] = {
    DISPLAY_DCS(0x80, 0, 0x8B), DISPLAY_DCS(0x81, 0, 0x78), DISPLAY_DCS(0x82, 0, 0x84), DISPLAY_DCS(0x83, 0, 0x88),
    DISPLAY_DCS(0x84, 0, 0xA8), DISPLAY_DCS(0x85, 0, 0xE3), DISPLAY_DCS(0x86, 0, 0x88), DISPLAY_DCS(0xB1, 0, 0x04),
};

static const display_link_t ek79007_links[] = {
    { .iface = DISPLAY_IFACE_DSI_LP },
};

/* Vendor sequence: reset high 5 ms, low 10 ms, high 120 ms */
const display_dcs_ic_t display_ic_ek79007 = {
    .name = "ek79007",
    .reset_low_ms = 10,
    .reset_wait_ms = 120,
    .flags = DISPLAY_DCS_IC_NO_DISPLAY_CMDS,
    DISPLAY_DCS_LINKS(ek79007_links),
    DISPLAY_DCS_INIT(ek79007_init),
};

/* The IC needs its PLL and regulators settled before the first video line; the vendor has no handshake */
#define EK79007_VIDEO_START_DELAY_MS 20

/*
 * 160 MHz / 3 = 53.33 MHz: 1344 x 635 clocks per frame give about 62.5 Hz (the data sheet asks for 51.2 MHz, which
 * the display clock cannot produce). The data sheet folds the sync pulse into its "blanking" figures: hsw + hbp = 160
 * and vsw + vbp = 23. RGB888 over 2 lanes at 53.33 MHz needs 640 Mbit/s per lane, so 850 Mbit/s leaves headroom.
 */
#define EK79007_1024X600_VIDEO(_lanes)                                                    \
    {                                                                                     \
        .link = DISPLAY_VLINK_DSI,                                                        \
        .width = 1024,                                                                    \
        .height = 600,                                                                    \
        .hsw = 20,                                                                        \
        .hbp = 140,                                                                       \
        .hfp = 160,                                                                       \
        .vsw = 2,                                                                         \
        .vbp = 21,                                                                        \
        .vfp = 12,                                                                        \
        .clock_hz = 53333333,                                                             \
        .dsi = { .lanes = (_lanes), .lane_mbps = 850, .format = DISPLAY_DSI_FMT_RGB888 }, \
    }

static const display_video_mode_t video_2lane[] = { EK79007_1024X600_VIDEO(2) };
static const display_video_mode_t video_4lane[] = { EK79007_1024X600_VIDEO(4) };

/* 2-lane MIPI input */
static const display_dcs_cmd_t ek79007_2lane[] = {
    DISPLAY_DCS(0xB2, 0, 0x10),
};

const display_dcs_module_t display_module_ek79007_1024x600_2lane = {
    .name = "ek79007_1024x600_2lane",
    .ic = &display_ic_ek79007,
    .width = 1024,
    .height = 600,
    .video_start_delay_ms = EK79007_VIDEO_START_DELAY_MS,
    DISPLAY_DCS_VIDEO(video_2lane),
    DISPLAY_DCS_INIT_EXTRA(ek79007_2lane),
};

const display_dcs_module_t display_module_ek79007_1024x600_4lane = {
    .name = "ek79007_1024x600_4lane",
    .ic = &display_ic_ek79007,
    .width = 1024,
    .height = 600,
    .video_start_delay_ms = EK79007_VIDEO_START_DELAY_MS,
    DISPLAY_DCS_VIDEO(video_4lane),
};
