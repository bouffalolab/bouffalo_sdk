/**
 * @file display_ili9881c.c
 * @brief ILI9881C IC description and default modules (KD050HDFIA020 and KD050HDFIA023-W4 panels).
 *
 * The two glasses use the same vendor script apart from the first two GIP registers of page 3 (0x01 and 0x02).
 * The pixel format is RGB565 because the DSI link carries RGB565.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "panel/video_ili9881c/display_ili9881c.h"

/* Vendor sequence; _gip_01 and _gip_02 are the first two registers of page 3, which differ between the glasses */
#define ILI9881C_INIT(_gip_01, _gip_02)                                                                   \
    /* Page 3 - GIP 1 */                                                                                  \
    DISPLAY_DCS(0xFF, 0, 0x98, 0x81, 0x03), DISPLAY_DCS(0x01, 0, _gip_01), DISPLAY_DCS(0x02, 0, _gip_02), \
        DISPLAY_DCS(0x03, 0, 0x72), DISPLAY_DCS(0x04, 0, 0x00), DISPLAY_DCS(0x05, 0, 0x00),               \
        DISPLAY_DCS(0x06, 0, 0x09), DISPLAY_DCS(0x07, 0, 0x00), DISPLAY_DCS(0x08, 0, 0x00),               \
        DISPLAY_DCS(0x09, 0, 0x01), DISPLAY_DCS(0x0A, 0, 0x00), DISPLAY_DCS(0x0B, 0, 0x00),               \
        DISPLAY_DCS(0x0C, 0, 0x01), DISPLAY_DCS(0x0D, 0, 0x00), DISPLAY_DCS(0x0E, 0, 0x00),               \
        DISPLAY_DCS(0x0F, 0, 0x00), DISPLAY_DCS(0x10, 0, 0x00), DISPLAY_DCS(0x11, 0, 0x00),               \
        DISPLAY_DCS(0x12, 0, 0x00), DISPLAY_DCS(0x13, 0, 0x00), DISPLAY_DCS(0x14, 0, 0x00),               \
        DISPLAY_DCS(0x15, 0, 0x00), DISPLAY_DCS(0x16, 0, 0x00), DISPLAY_DCS(0x17, 0, 0x00),               \
        DISPLAY_DCS(0x18, 0, 0x00), DISPLAY_DCS(0x19, 0, 0x00), DISPLAY_DCS(0x1A, 0, 0x00),               \
        DISPLAY_DCS(0x1B, 0, 0x00), DISPLAY_DCS(0x1C, 0, 0x00), DISPLAY_DCS(0x1D, 0, 0x00),               \
        DISPLAY_DCS(0x1E, 0, 0x40), DISPLAY_DCS(0x1F, 0, 0x80), DISPLAY_DCS(0x20, 0, 0x05),               \
        DISPLAY_DCS(0x21, 0, 0x02), DISPLAY_DCS(0x22, 0, 0x00), DISPLAY_DCS(0x23, 0, 0x00),               \
        DISPLAY_DCS(0x24, 0, 0x00), DISPLAY_DCS(0x25, 0, 0x00), DISPLAY_DCS(0x26, 0, 0x00),               \
        DISPLAY_DCS(0x27, 0, 0x00), DISPLAY_DCS(0x28, 0, 0x33), DISPLAY_DCS(0x29, 0, 0x02),               \
        DISPLAY_DCS(0x2A, 0, 0x00), DISPLAY_DCS(0x2B, 0, 0x00), DISPLAY_DCS(0x2C, 0, 0x00),               \
        DISPLAY_DCS(0x2D, 0, 0x00), DISPLAY_DCS(0x2E, 0, 0x00), DISPLAY_DCS(0x2F, 0, 0x00),               \
        DISPLAY_DCS(0x30, 0, 0x00), DISPLAY_DCS(0x31, 0, 0x00), DISPLAY_DCS(0x32, 0, 0x00),               \
        DISPLAY_DCS(0x33, 0, 0x00), DISPLAY_DCS(0x34, 0, 0x04), DISPLAY_DCS(0x35, 0, 0x00),               \
        DISPLAY_DCS(0x36, 0, 0x00), DISPLAY_DCS(0x37, 0, 0x00), DISPLAY_DCS(0x38, 0, 0x3C),               \
        DISPLAY_DCS(0x39, 0, 0x00), DISPLAY_DCS(0x3A, 0, 0x40), DISPLAY_DCS(0x3B, 0, 0x40),               \
        DISPLAY_DCS(0x3C, 0, 0x00), DISPLAY_DCS(0x3D, 0, 0x00), DISPLAY_DCS(0x3E, 0, 0x00),               \
        DISPLAY_DCS(0x3F, 0, 0x00), DISPLAY_DCS(0x40, 0, 0x00), DISPLAY_DCS(0x41, 0, 0x00),               \
        DISPLAY_DCS(0x42, 0, 0x00), DISPLAY_DCS(0x43, 0, 0x00), DISPLAY_DCS(0x44, 0, 0x00), /* GIP 2 */   \
        DISPLAY_DCS(0x50, 0, 0x01), DISPLAY_DCS(0x51, 0, 0x23), DISPLAY_DCS(0x52, 0, 0x45),               \
        DISPLAY_DCS(0x53, 0, 0x67), DISPLAY_DCS(0x54, 0, 0x89), DISPLAY_DCS(0x55, 0, 0xAB),               \
        DISPLAY_DCS(0x56, 0, 0x01), DISPLAY_DCS(0x57, 0, 0x23), DISPLAY_DCS(0x58, 0, 0x45),               \
        DISPLAY_DCS(0x59, 0, 0x67), DISPLAY_DCS(0x5A, 0, 0x89), DISPLAY_DCS(0x5B, 0, 0xAB),               \
        DISPLAY_DCS(0x5C, 0, 0xCD), DISPLAY_DCS(0x5D, 0, 0xEF), /* GIP 3 */                               \
        DISPLAY_DCS(0x5E, 0, 0x11), DISPLAY_DCS(0x5F, 0, 0x01), DISPLAY_DCS(0x60, 0, 0x00),               \
        DISPLAY_DCS(0x61, 0, 0x15), DISPLAY_DCS(0x62, 0, 0x14), DISPLAY_DCS(0x63, 0, 0x0E),               \
        DISPLAY_DCS(0x64, 0, 0x0F), DISPLAY_DCS(0x65, 0, 0x0C), DISPLAY_DCS(0x66, 0, 0x0D),               \
        DISPLAY_DCS(0x67, 0, 0x06), DISPLAY_DCS(0x68, 0, 0x02), DISPLAY_DCS(0x69, 0, 0x07),               \
        DISPLAY_DCS(0x6A, 0, 0x02), DISPLAY_DCS(0x6B, 0, 0x02), DISPLAY_DCS(0x6C, 0, 0x02),               \
        DISPLAY_DCS(0x6D, 0, 0x02), DISPLAY_DCS(0x6E, 0, 0x02), DISPLAY_DCS(0x6F, 0, 0x02),               \
        DISPLAY_DCS(0x70, 0, 0x02), DISPLAY_DCS(0x71, 0, 0x02), DISPLAY_DCS(0x72, 0, 0x02),               \
        DISPLAY_DCS(0x73, 0, 0x02), DISPLAY_DCS(0x74, 0, 0x02), DISPLAY_DCS(0x75, 0, 0x01),               \
        DISPLAY_DCS(0x76, 0, 0x00), DISPLAY_DCS(0x77, 0, 0x14), DISPLAY_DCS(0x78, 0, 0x15),               \
        DISPLAY_DCS(0x79, 0, 0x0E), DISPLAY_DCS(0x7A, 0, 0x0F), DISPLAY_DCS(0x7B, 0, 0x0C),               \
        DISPLAY_DCS(0x7C, 0, 0x0D), DISPLAY_DCS(0x7D, 0, 0x06), DISPLAY_DCS(0x7E, 0, 0x02),               \
        DISPLAY_DCS(0x7F, 0, 0x07), DISPLAY_DCS(0x80, 0, 0x02), DISPLAY_DCS(0x81, 0, 0x02),               \
        DISPLAY_DCS(0x83, 0, 0x02), DISPLAY_DCS(0x84, 0, 0x02), DISPLAY_DCS(0x85, 0, 0x02),               \
        DISPLAY_DCS(0x86, 0, 0x02), DISPLAY_DCS(0x87, 0, 0x02), DISPLAY_DCS(0x88, 0, 0x02),               \
        DISPLAY_DCS(0x89, 0, 0x02), DISPLAY_DCS(0x8A, 0, 0x02), /* Page 4 - power / ESD */                \
        DISPLAY_DCS(0xFF, 0, 0x98, 0x81, 0x04), DISPLAY_DCS(0x6C, 0, 0x15), DISPLAY_DCS(0x6E, 0, 0x2A),   \
        DISPLAY_DCS(0x6F, 0, 0x33), DISPLAY_DCS(0x3A, 0, 0x94), DISPLAY_DCS(0x8D, 0, 0x15),               \
        DISPLAY_DCS(0x87, 0, 0xBA), DISPLAY_DCS(0x26, 0, 0x76), DISPLAY_DCS(0xB2, 0, 0xD1),               \
        DISPLAY_DCS(0xB5, 0, 0x06), /* Page 1 - display timing + gamma */                                 \
        DISPLAY_DCS(0xFF, 0, 0x98, 0x81, 0x01), DISPLAY_DCS(0x22, 0, 0x0A), DISPLAY_DCS(0x31, 0, 0x00),   \
        DISPLAY_DCS(0x53, 0, 0xA5), DISPLAY_DCS(0x55, 0, 0xA2), DISPLAY_DCS(0x50, 0, 0xB7),               \
        DISPLAY_DCS(0x51, 0, 0xB7), DISPLAY_DCS(0x60, 0, 0x22), DISPLAY_DCS(0x61, 0, 0x00),               \
        DISPLAY_DCS(0x62, 0, 0x19), DISPLAY_DCS(0x63, 0, 0x10), /* Positive gamma */                      \
        DISPLAY_DCS(0xA0, 0, 0x08), DISPLAY_DCS(0xA1, 0, 0x17), DISPLAY_DCS(0xA2, 0, 0x1E),               \
        DISPLAY_DCS(0xA3, 0, 0x0E), DISPLAY_DCS(0xA4, 0, 0x13), DISPLAY_DCS(0xA5, 0, 0x24),               \
        DISPLAY_DCS(0xA6, 0, 0x1B), DISPLAY_DCS(0xA7, 0, 0x1B), DISPLAY_DCS(0xA8, 0, 0x53),               \
        DISPLAY_DCS(0xA9, 0, 0x1B), DISPLAY_DCS(0xAA, 0, 0x28), DISPLAY_DCS(0xAB, 0, 0x45),               \
        DISPLAY_DCS(0xAC, 0, 0x1A), DISPLAY_DCS(0xAD, 0, 0x1A), DISPLAY_DCS(0xAE, 0, 0x50),               \
        DISPLAY_DCS(0xAF, 0, 0x21), DISPLAY_DCS(0xB0, 0, 0x2C), DISPLAY_DCS(0xB1, 0, 0x3B),               \
        DISPLAY_DCS(0xB2, 0, 0x63), DISPLAY_DCS(0xB3, 0, 0x39), /* Negative gamma */                      \
        DISPLAY_DCS(0xC0, 0, 0x08), DISPLAY_DCS(0xC1, 0, 0x0C), DISPLAY_DCS(0xC2, 0, 0x17),               \
        DISPLAY_DCS(0xC3, 0, 0x0F), DISPLAY_DCS(0xC4, 0, 0x0B), DISPLAY_DCS(0xC5, 0, 0x1C),               \
        DISPLAY_DCS(0xC6, 0, 0x10), DISPLAY_DCS(0xC7, 0, 0x16), DISPLAY_DCS(0xC8, 0, 0x5B),               \
        DISPLAY_DCS(0xC9, 0, 0x1A), DISPLAY_DCS(0xCA, 0, 0x26), DISPLAY_DCS(0xCB, 0, 0x55),               \
        DISPLAY_DCS(0xCC, 0, 0x1D), DISPLAY_DCS(0xCD, 0, 0x1E), DISPLAY_DCS(0xCE, 0, 0x52),               \
        DISPLAY_DCS(0xCF, 0, 0x26), DISPLAY_DCS(0xD0, 0, 0x29), DISPLAY_DCS(0xD1, 0, 0x45),               \
        DISPLAY_DCS(0xD2, 0, 0x63), DISPLAY_DCS(0xD3, 0, 0x39),                                           \
        DISPLAY_DCS(0xFF, 0, 0x98, 0x81, 0x00), /* Page 0: normal commands */                             \
        DISPLAY_DCS0(0x35, 0),                  /* Tearing effect on (V-blank) */                         \
        DISPLAY_DCS(0x3A, 0, 0x05),             /* Pixel format: RGB565 (DBI[2:0] = 101) */               \
        DISPLAY_DCS0(0x11, 120),                /* Sleep out */

static const display_dcs_cmd_t ili9881c_init_kd050023w4[] = { ILI9881C_INIT(0x00, 0x00) };

static const display_dcs_cmd_t ili9881c_init_kd050020[] = { ILI9881C_INIT(0x01, 0x02) };

static const display_link_t ili9881c_links[] = {
    { .iface = DISPLAY_IFACE_DSI_LP },
};

const display_dcs_ic_t display_ic_ili9881c = {
    .name = "ili9881c",
    .reset_low_ms = 10,
    .reset_wait_ms = 120,
    DISPLAY_DCS_LINKS(ili9881c_links),
    DISPLAY_DCS_INIT(ili9881c_init_kd050023w4),
};

/* The 4-lane glass needs a shorter LPX and shorter turnaround times than the default of the host */
static const display_dsi_phy_t phy_kd050020 = {
    .clk_zero = 0x0F,
    .clk_trail = 3,
    .clk_exit = 5,
    .data_zero = 6,
    .data_trail = 3,
    .data_exit = 5,
    .data_prepare = 1,
    .ta_go = 0x0B,
    .ta_get = 0x0E,
    .req_ready = 0,
    .lpx = 2,
    .wakeup = 0x9C41,
};

/*
 * 240 MHz / 6 = 40 MHz: 793 x 1295 clocks per frame give about 39 Hz on the 4-lane glass, 787 x 1289 about 39.4 Hz
 * on the 2-lane glass. RGB565 at 40 MHz needs 640 / lanes Mbit/s per lane: 160 on 4 lanes, 320 on 2 lanes.
 */
static const display_video_mode_t video_kd050020[] = {
    {
        .link = DISPLAY_VLINK_DSI,
        .width = 720,
        .height = 1280,
        .hsw = 6,
        .hbp = 20,
        .hfp = 47,
        .vsw = 6,
        .vbp = 8,
        .vfp = 1,
        .clock_hz = 40000000,
        .dsi = { .lanes = 4, .lane_mbps = 550, .format = DISPLAY_DSI_FMT_RGB565, .phy = &phy_kd050020 },
    },
};

static const display_video_mode_t video_kd050023w4[] = {
    {
        .link = DISPLAY_VLINK_DSI,
        .width = 720,
        .height = 1280,
        .hsw = 6,
        .hbp = 15,
        .hfp = 46,
        .vsw = 6,
        .vbp = 3,
        .vfp = 0,
        .clock_hz = 40000000,
        .dsi = { .lanes = 2, .lane_mbps = 850, .format = DISPLAY_DSI_FMT_RGB565 },
    },
};

const display_dcs_module_t display_module_ili9881c_720x1280_kd050020 = {
    .name = "ili9881c_720x1280_kd050020",
    .ic = &display_ic_ili9881c,
    .width = 720,
    .height = 1280,
    DISPLAY_DCS_INIT(ili9881c_init_kd050020),
    DISPLAY_DCS_VIDEO(video_kd050020),
};

const display_dcs_module_t display_module_ili9881c_720x1280_kd050023w4 = {
    .name = "ili9881c_720x1280_kd050023w4",
    .ic = &display_ic_ili9881c,
    .width = 720,
    .height = 1280,
    DISPLAY_DCS_VIDEO(video_kd050023w4),
};
