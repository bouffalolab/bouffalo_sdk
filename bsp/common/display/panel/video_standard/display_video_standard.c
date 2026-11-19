/**
 * @file display_video_standard.c
 * @brief Minimal panel driver for standard RGB panels, and the standard modules.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "panel/video_standard/display_video_standard.h"
#include <stddef.h>

/* The core hands out &panel->base and this driver casts it back: base has to stay the first member */
_Static_assert(offsetof(display_video_panel_t, base) == 0, "display_panel_t must be the first member");

#define VIDEO_RESET_LOW_MS  10 /**< Reset pulse of a panel that has a reset line */
#define VIDEO_RESET_WAIT_MS 50 /**< Wait after reset */

/** @brief Validate the module and fill the panel information. */
static int video_init(display_panel_t *panel)
{
    const display_video_module_t *m = ((const display_video_panel_t *)panel)->module;
    display_panel_info_t *info = &panel->info;

    if (!m || !m->video || !m->video->width || !m->video->height || m->video->link == DISPLAY_VLINK_NONE ||
        m->video->link == DISPLAY_VLINK_DSI) {
        return -EINVAL;
    }
    info->width = m->video->width;
    info->height = m->video->height;
    info->rotation = DISPLAY_ROTATION_0;
    info->rotation_mask = 1u;
    info->x_align = 1;
    info->y_align = 1;
    info->gram = false;
    info->video_mode = m->video;
    return 0;
}

/** @brief Reset pulse; power is already on. */
static int video_prepare(display_panel_t *panel)
{
    return display_panel_hw_reset(panel, VIDEO_RESET_LOW_MS, VIDEO_RESET_WAIT_MS);
}

const display_panel_ops_t display_video_panel_ops = {
    .init = video_init,
    .prepare = video_prepare,
};

/*****************************************************************************
 * Standard modules
 *****************************************************************************/

/* 70 Hz: (800+100) x (480+50) x 70 = 33.39 MHz */
static const display_video_mode_t rgb_800x480 = {
    .link = DISPLAY_VLINK_DPI,
    .width = 800,
    .height = 480,
    .hsw = 4,
    .hbp = 82,
    .hfp = 14,
    .vsw = 5,
    .vbp = 6,
    .vfp = 39,
    .clock_hz = 33390000,
};

/* 70 Hz: (1024+320) x (600+38) x 70 ≈ 60.02 MHz */
static const display_video_mode_t rgb_1024x600 = {
    .link = DISPLAY_VLINK_DPI,
    .width = 1024,
    .height = 600,
    .hsw = 20,
    .hbp = 140,
    .hfp = 160,
    .vsw = 3,
    .vbp = 23,
    .vfp = 12,
    .clock_hz = 60023040,
};

/*
 * Serial RGB: one sub-pixel per clock, so a line is 480 x 3 + hsw + hbp + hfp = 1486 clocks and a frame 272 + vsw +
 * vbp + vfp = 286 lines. At the 27 MHz sub-pixel clock that is 27 MHz / (1486 x 286) = 63.5 Hz. The width is in
 * pixels; hsw / hbp / hfp are counted in sub-pixel clocks.
 */
static const display_video_mode_t srgb_480x272 = {
    .link = DISPLAY_VLINK_SRGB,
    .width = 480,
    .height = 272,
    .hsw = 1,
    .hbp = 43,
    .hfp = 2,
    .vsw = 1,
    .vbp = 12,
    .vfp = 1,
    .clock_hz = 27000000,
};

const display_video_module_t display_module_rgb_800x480 = { .name = "rgb_800x480", .video = &rgb_800x480 };
const display_video_module_t display_module_rgb_1024x600 = { .name = "rgb_1024x600", .video = &rgb_1024x600 };
const display_video_module_t display_module_srgb_480x272 = { .name = "srgb_480x272", .video = &srgb_480x272 };
