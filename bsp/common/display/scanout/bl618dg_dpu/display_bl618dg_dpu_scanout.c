/**
 * @file display_bl618dg_dpu_scanout.c
 * @brief BL618DG display processing unit as a scanout: connects the scanout core of the display framework to the OSD0
 *        layer of the hardware level (display_bl618dg_dpu_hw.h).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "scanout/bl618dg_dpu/display_bl618dg_dpu_scanout.h"
#include "bflb_core.h"
#include <stddef.h>

/* The core hands out &scanout->base and this backend casts it back: base has to stay the first member */
_Static_assert(offsetof(display_bl618dg_dpu_scanout_t, base) == 0, "display_scanout_t must be the first member");

/** @brief Get the DPU scanout from the core object (base is the first member). */
static display_bl618dg_dpu_scanout_t *to_dpu(display_scanout_t *scanout)
{
    return (display_bl618dg_dpu_scanout_t *)scanout;
}

static void dpu_event(display_event_type_t type, void *ctx)
{
    display_bl618dg_dpu_scanout_t *dpu = ctx;

    display_scanout_on_event(&dpu->base, type);
}

static int dpu_init(display_scanout_t *scanout)
{
    display_bl618dg_dpu_scanout_t *dpu = to_dpu(scanout);
    int ret = display_bl618dg_dpu_hw_open(&dpu->hw);

    if (ret) {
        return ret;
    }
    ret = display_bl618dg_dpu_osd0_claim(&dpu->hw, dpu_event, dpu);
    if (ret) {
        return ret;
    }
    scanout->caps = (display_scanout_caps_t){
        .flags = DISPLAY_SCANOUT_CAP_FLIP | DISPLAY_SCANOUT_CAP_VSYNC,
        .format_mask = display_bl618dg_dpu_hw_layer_formats(DISPLAY_BL618DG_DPU_LAYER_OSD0),
        .vlink_mask = DISPLAY_VLINK_BIT(dpu->hw.cfg.dsi ? DISPLAY_VLINK_DSI : DISPLAY_VLINK_DPI),
        .addr_align = BFLB_CACHE_LINE_SIZE,
    };
    return 0;
}

static int dpu_config(display_scanout_t *scanout, const display_video_mode_t *mode, const display_scanout_cfg_t *cfg)
{
    display_bl618dg_dpu_scanout_t *dpu = to_dpu(scanout);
    const display_bl618dg_dpu_layer_cfg_t osd0 = {
        .format = cfg->format,
        .window = { .width = mode->width, .height = mode->height },
        .dcache_dis = true, /* the scanout core manages the data cache of the frames */
    };
    bool was_running = dpu->hw.running;
    int ret = display_bl618dg_dpu_hw_start(&dpu->hw, mode);

    if (ret) {
        return ret;
    }
    ret = display_bl618dg_dpu_osd0_config(&dpu->hw, &osd0);
    if (ret && !was_running) {
        int stop_ret = display_bl618dg_dpu_hw_stop(&dpu->hw);

        if (stop_ret) {
            return stop_ret; /* preserve stopping/claim; deinit retries cleanup */
        }
    }
    return ret;
}

static int dpu_flip(display_scanout_t *scanout, const display_fb_t *fb)
{
    return display_bl618dg_dpu_osd0_flip(&to_dpu(scanout)->hw, fb);
}

static int dpu_deinit(display_scanout_t *scanout)
{
    display_bl618dg_dpu_scanout_t *dpu = to_dpu(scanout);
    int ret = display_bl618dg_dpu_hw_stop(&dpu->hw);

    return ret ? ret : display_bl618dg_dpu_osd0_release(&dpu->hw);
}

const display_scanout_ops_t display_bl618dg_dpu_scanout_ops = {
    .init = dpu_init,
    .config = dpu_config,
    .flip = dpu_flip,
    .deinit = dpu_deinit,
};
