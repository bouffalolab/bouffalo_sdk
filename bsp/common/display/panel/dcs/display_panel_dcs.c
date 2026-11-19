/**
 * @file display_panel_dcs.c
 * @brief Generic MIPI DCS panel driver.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "panel/dcs/display_panel_dcs.h"
#include "display_os.h"
#include <stddef.h>

/* The core hands out &panel->base and this driver casts it back: base has to stay the first member */
_Static_assert(offsetof(display_dcs_panel_t, base) == 0, "display_panel_t must be the first member");

#define DCS_SLPIN    0x10
#define DCS_INVON    0x21
#define DCS_DISPOFF  0x28
#define DCS_DISPON   0x29
#define DCS_CASET    0x2A
#define DCS_RASET    0x2B
#define DCS_RAMWR    0x2C
#define DCS_MADCTL   0x36
#define DCS_COLMOD   0x3A

#define MADCTL_MY    0x80 /**< Row address order */
#define MADCTL_MX    0x40 /**< Column address order */
#define MADCTL_MV    0x20 /**< Row/column exchange */
#define MADCTL_BGR   0x08 /**< BGR color order */
#define DCS_SLPIN_MS 120  /**< Wait after sleep in before power can be removed */

/** @brief Module description of a panel (the panel is the first member of display_dcs_panel_t). */
static const display_dcs_module_t *dcs_module(const display_panel_t *panel)
{
    return ((const display_dcs_panel_t *)panel)->module;
}

/**
 * @brief Find the settings of the IC on an interface, honoring the interfaces the module is strapped for.
 */
static const display_link_t *dcs_find_link(const display_dcs_module_t *m, display_iface_t iface)
{
    if (m->iface_mask && !(m->iface_mask & DISPLAY_IFACE_BIT(iface))) {
        return NULL;
    }
    for (uint8_t i = 0; i < m->ic->link_count; i++) {
        if (m->ic->links[i].iface == iface) {
            return &m->ic->links[i];
        }
    }
    return NULL;
}

/**
 * @brief Choose the video link of a module for the interface of the command endpoint.
 *
 * DSI_LP selects the DSI link, every other interface (and no endpoint at all) the first link that is not DSI.
 */
static const display_video_mode_t *dcs_find_video(const display_dcs_module_t *m, display_iface_t iface)
{
    for (uint8_t i = 0; i < m->video_count; i++) {
        if ((m->video[i].link == DISPLAY_VLINK_DSI) == (iface == DISPLAY_IFACE_DSI_LP)) {
            return &m->video[i];
        }
    }
    return NULL;
}

/**
 * @brief The default initialization sequence of the IC for an interface: the one of @ref display_dcs_ic_t::iface_init
 *        that names the interface, otherwise @ref display_dcs_ic_t::init.
 */
static const display_dcs_cmd_t *dcs_ic_init(const display_dcs_ic_t *ic, display_iface_t iface, uint16_t *count)
{
    for (uint8_t i = 0; i < ic->iface_init_count; i++) {
        if (ic->iface_init[i].iface_mask & DISPLAY_IFACE_BIT(iface)) {
            *count = ic->iface_init[i].init_count;
            return ic->iface_init[i].init;
        }
    }
    *count = ic->init_count;
    return ic->init;
}

/** @brief Index into display_link_t::colmod: 0 for 16 bit, 1 for 24 bit. */
static unsigned dcs_colmod_index(display_wire_t wire)
{
    return wire == DISPLAY_WIRE_16 ? 0u : 1u;
}

/**
 * @brief Send an initialization sequence.
 */
static int dcs_send_seq(display_panel_t *panel, const display_dcs_cmd_t *seq, uint16_t count)
{
    for (uint16_t i = 0; i < count; i++) {
        const display_io_msg_t msg = {
            .cmd = seq[i].cmd, .tx = seq[i].data, .len = seq[i].len, .flags = seq[i].io_flags
        };
        int ret = display_io_xfer(panel->io, &msg, 1);

        if (ret) {
            return ret;
        }
        if (seq[i].delay_ms) {
            display_os_delay_ms(seq[i].delay_ms);
        }
    }
    return 0;
}

void display_dcs_module_adjust(display_dcs_module_t *dst, const display_dcs_module_t *src, bool swap_rgb_bgr,
                               bool mirror, display_dcs_invert_t invert)
{
    *dst = *src;
    for (unsigned r = 0; r < 4; r++) {
        if (r && !(src->rotation_mask & (1u << r))) {
            continue;
        }
        if (swap_rgb_bgr) {
            dst->madctl[r] ^= MADCTL_BGR;
        }
        if (mirror) {
            dst->madctl[r] ^= (src->madctl[r] & MADCTL_MV) ? MADCTL_MY : MADCTL_MX;
        }
    }
    if (invert != DISPLAY_DCS_INVERT_KEEP) {
        dst->invert = (invert == DISPLAY_DCS_INVERT_ON);
    }
}

/** @brief Send MADCTL for the current rotation. */
static int dcs_send_madctl(display_panel_t *panel)
{
    uint8_t madctl = dcs_module(panel)->madctl[panel->info.rotation];

    return display_io_write(panel->io, DCS_MADCTL, &madctl, 1);
}

/**
 * @brief Check every enabled rotation: the MV bit matches the width/height swap and the visible area
 *        plus offset fits into GRAM.
 */
static int dcs_check_geometry(const display_dcs_module_t *m, uint8_t mask)
{
    for (unsigned r = 0; r < 4; r++) {
        bool swap = (r & 1u) != 0;
        bool mv = (m->madctl[r] & MADCTL_MV) != 0;
        uint32_t w = swap ? m->height : m->width;
        uint32_t h = swap ? m->width : m->height;
        uint32_t ram_w = mv ? m->ic->ram_height : m->ic->ram_width;
        uint32_t ram_h = mv ? m->ic->ram_width : m->ic->ram_height;

        if (!(mask & (1u << r))) {
            continue;
        }
        if (mv != swap || w + m->gap_x[r] > ram_w || h + m->gap_y[r] > ram_h) {
            return -EINVAL;
        }
    }
    return 0;
}

/*****************************************************************************
 * Panel operations
 *****************************************************************************/

/** @brief Validate the module against the IC and the endpoint, load the link and fill info. */
static int dcs_init(display_panel_t *panel)
{
    const display_dcs_module_t *m = dcs_module(panel);
    display_panel_info_t *info = &panel->info;
    display_io_info_t io = { 0 };
    const display_link_t *link = NULL;
    const display_video_mode_t *video = NULL;
    bool gram;

    if (!m || !m->ic || !m->width || !m->height || (m->init_count && !m->init) ||
        (m->init_extra_count && !m->init_extra) || (m->init_user_count && !m->init_user) ||
        (m->ic->init_count && !m->ic->init) || (m->ic->link_count && !m->ic->links) ||
        (m->ic->iface_init_count && !m->ic->iface_init) || (m->video_count && !m->video) ||
        (!m->video_count && m->video)) {
        return -EINVAL;
    }
    if (panel->io) {
        if (display_io_get_info(panel->io, &io)) {
            return -EINVAL;
        }
        /* The endpoint tells how the panel is wired; the IC and the module must support that interface */
        link = dcs_find_link(m, io.iface);
        if (!link) {
            return -ENOTSUP;
        }
    }

    gram = m->video_count == 0;
    info->rotation_mask = (gram && m->rotation_mask) ? m->rotation_mask : 1u;
    if (gram) {
        /* A GRAM panel needs an endpoint that carries pixels, at a wire width the IC supports */
        if (!panel->io || !(io.caps.format_mask & DISPLAY_FMT_BIT(io.format))) {
            return -ENOTSUP;
        }
        if (!link->colmod[dcs_colmod_index(io.wire)]) {
            return -ENOTSUP;
        }
        if (dcs_check_geometry(m, info->rotation_mask)) {
            return -EINVAL;
        }
    } else {
        /* The command interface tells which input of the panel is used, and with it which video link */
        video = dcs_find_video(m, panel->io ? io.iface : DISPLAY_IFACE_NONE);
        if (!video || video->width != m->width || video->height != m->height) {
            return -ENOTSUP;
        }
    }
    if (link) {
        int ret = display_io_set_link(panel->io, link);

        if (ret) {
            return ret;
        }
    }
    if (video && panel->io) {
        /* A bus that also carries video (MIPI DSI) sets the link up before the first command */
        int ret = display_io_set_video(panel->io, video);

        if (ret) {
            return ret;
        }
    }

    info->width = m->width;
    info->height = m->height;
    info->format = io.format;
    info->wire = io.wire;
    info->rotation = DISPLAY_ROTATION_0;
    info->x_align = m->ic->x_align;
    info->y_align = m->ic->y_align;
    info->gram = gram;
    info->video_mode = video;
    info->video_start_delay_ms = m->video_start_delay_ms;
    return 0;
}

/** @brief The init sequence of the module, or of the IC for the interface of the endpoint, then the extras. */
static int dcs_send_init(display_panel_t *panel, const display_dcs_module_t *m)
{
    int ret;

    if (m->init) {
        ret = dcs_send_seq(panel, m->init, m->init_count);
    } else {
        uint16_t count;
        const display_dcs_cmd_t *seq = dcs_ic_init(m->ic, panel->io->link->iface, &count);

        ret = dcs_send_seq(panel, seq, count);
    }
    return ret ? ret : dcs_send_seq(panel, m->init_extra, m->init_extra_count);
}

/** @brief COLMOD and MADCTL of a GRAM panel: from the endpoint and the rotation, never from the sequences. */
static int dcs_send_colmod_madctl(display_panel_t *panel)
{
    uint8_t colmod = panel->io->link->colmod[dcs_colmod_index(panel->info.wire)];
    int ret = display_io_write(panel->io, DCS_COLMOD, &colmod, 1);

    return ret ? ret : dcs_send_madctl(panel);
}

/**
 * @brief Reset, then send: IC/module sequence, module extras, COLMOD and MADCTL (GRAM panels), INVON.
 */
static int dcs_prepare(display_panel_t *panel)
{
    const display_dcs_module_t *m = dcs_module(panel);
    int ret;

    ret = display_panel_hw_reset(panel, m->ic->reset_low_ms, m->ic->reset_wait_ms);
    if (ret || !panel->io) {
        return ret;
    }
    ret = dcs_send_init(panel, m);
    if (ret) {
        return ret;
    }
    if (panel->info.gram) {
        ret = dcs_send_colmod_madctl(panel);
        if (ret) {
            return ret;
        }
    }
    if (m->invert) {
        /* IC specific preparation (if any) must precede INVON */
        ret = dcs_send_seq(panel, m->ic->invert_seq, m->ic->invert_seq_count);
        if (ret) {
            return ret;
        }
        ret = display_io_write(panel->io, DCS_INVON, NULL, 0);
        if (ret) {
            return ret;
        }
    }
    return dcs_send_seq(panel, m->init_user, m->init_user_count);
}

/** @brief Send a command without parameters; a panel without an endpoint has nothing to send. */
static int dcs_write_cmd(display_panel_t *panel, uint8_t cmd)
{
    return panel->io ? display_io_write(panel->io, cmd, NULL, 0) : 0;
}

/** @brief DISPON, unless the IC is left to the application. */
static int dcs_enable(display_panel_t *panel)
{
    if (dcs_module(panel)->ic->flags & (DISPLAY_DCS_IC_NO_AUTO_DISPLAY_ON | DISPLAY_DCS_IC_NO_DISPLAY_CMDS)) {
        return 0;
    }
    return dcs_write_cmd(panel, DCS_DISPON);
}

/** @brief DISPOFF, unless the IC has no display commands. */
static int dcs_disable(display_panel_t *panel)
{
    if (dcs_module(panel)->ic->flags & DISPLAY_DCS_IC_NO_DISPLAY_CMDS) {
        return 0;
    }
    return dcs_write_cmd(panel, DCS_DISPOFF);
}

/** @brief SLPIN, then wait so that power can be removed safely. */
static int dcs_unprepare(display_panel_t *panel)
{
    int ret;

    if (dcs_module(panel)->ic->flags & DISPLAY_DCS_IC_NO_DISPLAY_CMDS) {
        return 0;
    }
    ret = dcs_write_cmd(panel, DCS_SLPIN);

    display_os_delay_ms(DCS_SLPIN_MS);
    return ret;
}

/** @brief Update rotation and size; MADCTL is sent only when the panel is powered. */
static int dcs_set_rotation(display_panel_t *panel, display_rotation_t rotation)
{
    const display_dcs_module_t *m = dcs_module(panel);
    display_rotation_t old = panel->info.rotation;
    bool swap = (rotation & 1u) != 0;

    panel->info.rotation = rotation;
    if (panel->state != DISPLAY_PANEL_OFF) {
        int ret = dcs_send_madctl(panel);

        if (ret) {
            panel->info.rotation = old;
            return ret;
        }
    }
    panel->info.width = swap ? m->height : m->width;
    panel->info.height = swap ? m->width : m->height;
    return 0;
}

/** @brief Set the CASET/RASET window (with the module GRAM offset) and write the pixels with RAMWR. */
static int dcs_draw(display_panel_t *panel, const display_area_t *area, const void *pixels, size_t len,
                    display_done_cb_t cb, void *ctx)
{
    const display_dcs_module_t *m = dcs_module(panel);
    display_rotation_t r = panel->info.rotation;
    uint32_t x0 = (uint32_t)area->x + m->gap_x[r];
    uint32_t x1 = x0 + area->w - 1u;
    uint32_t y0 = (uint32_t)area->y + m->gap_y[r];
    uint32_t y1 = y0 + area->h - 1u;
    /* The window parameters are sent before display_io_xfer[_async]() returns: stack is fine */
    uint8_t caset[4] = { (uint8_t)(x0 >> 8), (uint8_t)x0, (uint8_t)(x1 >> 8), (uint8_t)x1 };
    uint8_t raset[4] = { (uint8_t)(y0 >> 8), (uint8_t)y0, (uint8_t)(y1 >> 8), (uint8_t)y1 };
    display_io_msg_t msgs[3] = {
        { .cmd = DCS_CASET, .tx = caset, .len = 4 },
        { .cmd = DCS_RASET, .tx = raset, .len = 4 },
        { .cmd = DCS_RAMWR, .tx = pixels, .len = (uint32_t)len, .flags = DISPLAY_IO_MSG_COLOR },
    };

    return cb ? display_io_xfer_async(panel->io, msgs, 3, cb, ctx) : display_io_xfer(panel->io, msgs, 3);
}

/** @brief Driver operations of the generic DCS panel. */
const display_panel_ops_t display_dcs_panel_ops = {
    .init = dcs_init,
    .prepare = dcs_prepare,
    .enable = dcs_enable,
    .disable = dcs_disable,
    .unprepare = dcs_unprepare,
    .set_rotation = dcs_set_rotation,
    .draw = dcs_draw,
};
