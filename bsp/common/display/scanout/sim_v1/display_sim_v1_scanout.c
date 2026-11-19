/**
 * @file display_sim_v1_scanout.c
 * @brief SIM v1 scanout: two ping-pong frame lists, flip and frame-end events.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "scanout/sim_v1/display_sim_v1_hw.h"
#include "display_os.h"
#include <stddef.h>

_Static_assert(offsetof(display_sim_v1_scanout_t, base) == 0, "display_scanout_t must be the first member");

static display_sim_v1_scanout_t *to_sim(display_scanout_t *scanout)
{
    return (display_sim_v1_scanout_t *)scanout;
}

/** @brief Size of an RGB565 frame in 32-bit words. */
static uint32_t sim_frame_words(const display_video_mode_t *mode)
{
    return (uint32_t)mode->width * mode->height / 2u;
}

static struct bflb_dma_channel_lli_pool_s *sim_list(display_sim_v1_scanout_t *sim, int index)
{
    return &sim->cfg.lli[(uint32_t)index * sim->nodes_per_frame];
}

static int sim_init(display_scanout_t *scanout)
{
    display_sim_v1_scanout_t *sim = to_sim(scanout);

    if (!sim->cfg.dma || !sim->cfg.lli || !sim->cfg.lli_count || (uintptr_t)sim->cfg.lli % 4u) {
        return -EINVAL;
    }
    scanout->caps = (display_scanout_caps_t){
        .flags = DISPLAY_SCANOUT_CAP_FLIP | DISPLAY_SCANOUT_CAP_VSYNC,
        .format_mask = DISPLAY_FMT_BIT(DISPLAY_FMT_RGB565),
        .vlink_mask = DISPLAY_VLINK_BIT(DISPLAY_VLINK_DPI),
        .addr_align = 4,
        .size_align = 4,
    };
    sim->active_lli = -1;
    sim->opened = false;
    sim->submitted_op = DISPLAY_FRAME_OP_NONE;
    return display_sim_v1_hw_init(sim);
}

static uint16_t sim_frame_rate(const display_video_mode_t *mode)
{
    uint64_t htotal = (uint64_t)mode->width + mode->hsw + mode->hbp + mode->hfp;
    uint64_t vtotal = (uint64_t)mode->height + mode->vsw + mode->vbp + mode->vfp;
    uint64_t rate = (mode->clock_hz + htotal * vtotal / 2u) / (htotal * vtotal);

    return rate <= UINT16_MAX ? (uint16_t)rate : 0;
}

static int sim_config(display_scanout_t *scanout, const display_video_mode_t *mode, const display_scanout_cfg_t *cfg)
{
    display_sim_v1_scanout_t *sim = to_sim(scanout);
    uint32_t words = sim_frame_words(mode);
    uint16_t rate = sim_frame_rate(mode);
    uint32_t nodes = (words + DISPLAY_SIM_V1_NODE_BYTES / 4u - 1u) / (DISPLAY_SIM_V1_NODE_BYTES / 4u);
    int ret;

    if (cfg->format != DISPLAY_FMT_RGB565 || !mode->clock_hz || !rate || !mode->vfp ||
        ((uint32_t)mode->width * mode->height & 1u) ||
        mode->vsw + mode->vbp + mode->vfp > DISPLAY_SIM_V1_INVALID_ROWS_MAX || !nodes || nodes > UINT16_MAX) {
        return -EINVAL;
    }
    if (sim->cfg.lli_count / nodes < 2u) {
        return -ENOMEM;
    }
    if (sim->opened) {
        display_sim_v1_hw_close(sim); /* STOPPED: no list can be running */
        sim->opened = false;
    }
    ret = display_sim_v1_hw_open(sim, mode, rate); /* no frame list, no DMA start */
    if (ret) {
        return ret;
    }
    sim->opened = true;
    sim->nodes_per_frame = (uint16_t)nodes;
    sim->active_lli = -1;
    sim->submitted_op = DISPLAY_FRAME_OP_NONE;
    return 0;
}

/** @brief Build the idle group before publishing a ready operation under a short IRQ guard. */
static int sim_flip(display_scanout_t *scanout, const display_fb_t *fb)
{
    display_sim_v1_scanout_t *sim = to_sim(scanout);
    display_frame_op_t op = scanout->pending_op;
    uintptr_t flags;
    int index = 0;
    int ret;

    if (fb) {
        if (!sim->opened) {
            ret = display_sim_v1_hw_open(sim, &scanout->mode, sim_frame_rate(&scanout->mode));
            if (ret) {
                return ret;
            }
            sim->opened = true;
        }
        index = op == DISPLAY_FRAME_OP_FIRST ? 0 : sim->active_lli ^ 1;
        /* Core pending excludes another submission. Until publication an IRQ only repeats the active list. */
        display_sim_v1_hw_build_list(sim, sim_list(sim, index), sim->nodes_per_frame,
                                     fb->data, sim_frame_words(&scanout->mode));
    }
    flags = display_os_irq_save();
    sim->submitted_op = op; /* The descriptor clean has completed; never use core pending as a ready flag. */
    if (op == DISPLAY_FRAME_OP_FIRST) {
        sim->active_lli = (int8_t)index;
        display_sim_v1_hw_run(sim, sim_list(sim, index));
    }
    display_os_irq_restore(flags);
    return 0;
}

static int sim_deinit(display_scanout_t *scanout)
{
    display_sim_v1_scanout_t *sim = to_sim(scanout);
    uintptr_t flags = display_os_irq_save();

    sim->active_lli = -1;
    sim->submitted_op = DISPLAY_FRAME_OP_NONE;
    display_os_irq_restore(flags);
    if (sim->opened) {
        display_sim_v1_hw_close(sim);
        sim->opened = false;
    }
    return 0;
}

void display_sim_v1_frame_end(display_sim_v1_scanout_t *sim)
{
    display_scanout_t *scanout = &sim->base;
    display_frame_op_t op;
    display_event_type_t event;
    uintptr_t flags = display_os_irq_save();

    if (!sim->opened || sim->active_lli < 0) {
        display_os_irq_restore(flags);
        return; /* no first frame, or stale interrupt after close */
    }
    op = sim->submitted_op;
    if (op == DISPLAY_FRAME_OP_LAST) {
        sim->active_lli = -1;
        sim->submitted_op = DISPLAY_FRAME_OP_NONE;
        sim->opened = false;
        display_os_irq_restore(flags);
        display_sim_v1_hw_close(sim);
        display_scanout_on_event(scanout, DISPLAY_EVT_FB_LAST);
        return;
    }
    if (op == DISPLAY_FRAME_OP_FLIP) {
        sim->active_lli ^= 1;
    }
    display_sim_v1_hw_run(sim, sim_list(sim, sim->active_lli));
    sim->submitted_op = DISPLAY_FRAME_OP_NONE;
    event = op == DISPLAY_FRAME_OP_FIRST ? DISPLAY_EVT_FB_FIRST :
            op == DISPLAY_FRAME_OP_FLIP  ? DISPLAY_EVT_FB_RETIRED :
                                           DISPLAY_EVT_VSYNC;
    display_os_irq_restore(flags);
    /* State is ready before notification, so callbacks may prepare the next idle list. */
    display_scanout_on_event(scanout, event);
}

const display_scanout_ops_t display_sim_v1_scanout_ops = {
    .init = sim_init,
    .config = sim_config,
    .flip = sim_flip,
    .deinit = sim_deinit,
};