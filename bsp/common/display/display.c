/**
 * @file display.c
 * @brief Display: composition, lifecycle (init brings the screen up, deinit switches it off), event bridging,
 *        ownership.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "display.h"
#include "bus/display_bus_driver.h"
#include "scanout/display_scanout_driver.h"

/**
 * @brief Deliver an event to the display callback, if any.
 */
static void display_emit(display_t *display, display_event_type_t type, int result, const void *buf)
{
    display_event_t event = { .type = type, .result = result, .buf = buf };

    if (display->cb) {
        display->cb(display, &event, display->cb_ctx);
    }
}

/**
 * @brief Panel completion callback: report DRAW_DONE; @p ctx is the display.
 */
static void display_draw_done(int result, const void *buf, void *ctx)
{
    display_emit(ctx, DISPLAY_EVT_DRAW_DONE, result, buf);
}

/**
 * @brief Scanout listener: forward FIRST / RETIRED / LAST / VSYNC; @p ctx is the display.
 */
static void display_scanout_event(display_scanout_t *scanout, const display_event_t *event, void *ctx)
{
    display_t *display = ctx;
    display_event_cb_t cb;
    void *cb_ctx;
    uintptr_t flags;

    (void)scanout;
    /* Read the pair atomically: display_set_callback() may change it at any time */
    flags = display_os_irq_save();
    cb = display->cb;
    cb_ctx = display->cb_ctx;
    display_os_irq_restore(flags);
    if (cb) {
        cb(display, event, cb_ctx);
    }
}

/**
 * @brief Check that a display is wired and initialized.
 */
static bool display_ready(const display_t *display)
{
    return display && display->panel && display->state != DISPLAY_STATE_UNINIT;
}

/**
 * @brief The panel is of the kind the display needs: a video panel for a scanout display, a GRAM panel otherwise.
 */
static bool display_panel_fits(const display_t *display, const display_panel_info_t *info)
{
    return display->scanout ? (!info->gram && info->video_mode) : info->gram;
}

int display_init(display_t *display)
{
    display_panel_t *panel;
    display_panel_info_t info = { 0 };
    display_scanout_status_t scanout_status;
    bool panel_was_init;
    bool scanout_was_init = false;
    bool config_attempted = false;
    bool powered = false;
    int ret;

    if (!display || !display->panel) {
        return -EINVAL;
    }
    if (display_os_in_isr()) {
        return -EPERM;
    }
    if (display->state != DISPLAY_STATE_UNINIT) {
        return display->state == DISPLAY_STATE_ERROR ? -EPIPE : 0;
    }
    panel = display->panel;

    /* A panel and a scanout belong to one display only */
    if (panel->claimed) {
        return -EBUSY;
    }
    /* Lower layers the application already brought up must be idle: the display starts from scratch */
    if (panel->initialized && panel->state != DISPLAY_PANEL_OFF) {
        return -EBUSY;
    }
    if (display->scanout && display->scanout->initialized) {
        ret = display_scanout_get_status(display->scanout, &scanout_status);
        if (ret) {
            return ret;
        }
        if (scanout_status.state != DISPLAY_SCANOUT_STOPPED || scanout_status.pending_op != DISPLAY_FRAME_OP_NONE) {
            return -EBUSY;
        }
    }
    if (display->scanout) {
        ret = display_scanout_set_callback(display->scanout, display_scanout_event, display);
        if (ret) {
            return ret;
        }
        scanout_was_init = display->scanout->initialized;
    }
    panel->claimed = true; /* from here on every error path has to undo it */

    /* Initialize the layers, then bring the screen up; on error undo only what this call did */
    panel_was_init = panel->initialized;
    ret = display_panel_init(panel);
    if (!ret) {
        display_panel_get_info(panel, &info);
        if (!display_panel_fits(display, &info)) {
            ret = -EINVAL;
        }
    }
    if (!ret && display->scanout) {
        ret = display_scanout_init(display->scanout);
    }
    if (!ret) {
        display_panel_set_backlight(panel, 0); /* off until the application switches it on; -ENOTSUP is fine */
        ret = display_panel_prepare(panel);    /* power, reset, initialization sequence */
        powered = ret == 0;
    }
    /* Display on before pixels: a DSI panel must get it over LP before HS starts */
    if (!ret) {
        ret = display_panel_enable(panel);
    }
    if (!ret && display->scanout) {
        if (info.video_start_delay_ms) {
            display_os_delay_ms(info.video_start_delay_ms);
        }
        config_attempted = true;
        ret = display_scanout_config(display->scanout, info.video_mode, &display->scanout->cfg);
    }
    if (ret) {
        int cleanup_ret;

        /* Even a failed config may have enabled output. Never send LP/power off before it stops. */
        if (display->scanout && (!scanout_was_init || config_attempted)) {
            cleanup_ret = display_scanout_deinit(display->scanout);
            if (cleanup_ret) {
                display->state = DISPLAY_STATE_ERROR;
                return cleanup_ret; /* keep panel/owner/callback so deinit can retry */
            }
        }
        if (powered) {
            cleanup_ret = display_panel_unprepare(panel);
            if (cleanup_ret) {
                display->state = DISPLAY_STATE_ERROR;
                return cleanup_ret;
            }
        }
        if (display->scanout) {
            display_scanout_set_callback(display->scanout, NULL, NULL);
        }
        if (!panel_was_init) {
            cleanup_ret = display_panel_deinit(panel);
            if (cleanup_ret) {
                display->state = DISPLAY_STATE_ERROR;
                return cleanup_ret;
            }
        }
        panel->claimed = false;
        return ret;
    }

    display->state = DISPLAY_STATE_READY;
    return 0;
}

int display_deinit(display_t *display)
{
    int ret;

    if (display_os_in_isr()) {
        return -EPERM;
    }
    if (!display) {
        return -EINVAL;
    }
    if (!display_ready(display)) {
        return 0;
    }
    display_panel_set_backlight(display->panel, 0);
    if (display->scanout) {
        ret = display_scanout_deinit(display->scanout);
        if (ret) {
            display->state = DISPLAY_STATE_ERROR; /* output/DSI HS may still run: no LP or power off */
            return ret;
        }
    }
    ret = display_panel_unprepare(display->panel); /* display off, sleep in, power off */
    if (ret) {
        return ret; /* keep resources and owner for retry */
    }
    ret = display_panel_deinit(display->panel);
    if (ret) {
        return ret;
    }
    if (display->scanout) {
        display_scanout_set_callback(display->scanout, NULL, NULL);
    }
    display->panel->claimed = false;
    display->state = DISPLAY_STATE_UNINIT;
    return 0;
}

int display_set_callback(display_t *display, display_event_cb_t cb, void *ctx)
{
    display_io_t *io;
    uintptr_t flags;
    int ret;

    if (!display_ready(display)) {
        return -EINVAL;
    }

    /* GRAM: holding the bus lock guarantees that no transfer of this display is in flight,
     * so no DRAW_DONE can read the callback while it changes. */
    io = display->scanout ? NULL : display_panel_get_io(display->panel);
    if (io) {
        ret = display_bus_hold(io->bus);
        if (ret) {
            return ret;
        }
    }
    /* Scanout events are not covered by the bus lock: change both fields atomically */
    flags = display_os_irq_save();
    display->cb = cb;
    display->cb_ctx = ctx;
    display_os_irq_restore(flags);
    if (io) {
        display_bus_unhold(io->bus);
    }
    return 0;
}

int display_set_backlight(display_t *display, uint16_t level)
{
    if (!display_ready(display)) {
        return -EINVAL;
    }
    return display_panel_set_backlight(display->panel, level);
}

int display_get_info(display_t *display, display_info_t *info)
{
    display_panel_info_t panel_info;
    display_io_info_t io = { 0 };

    if (!display_ready(display) || !info) {
        return -EINVAL;
    }
    display_panel_get_info(display->panel, &panel_info);

    *info = (display_info_t){ 0 };
    info->kind = display->scanout ? DISPLAY_KIND_FB : DISPLAY_KIND_GRAM;
    info->state = display->state;
    info->width = panel_info.width;
    info->height = panel_info.height;
    info->rotation = panel_info.rotation;
    info->x_align = panel_info.x_align;
    info->y_align = panel_info.y_align;
    if (info->kind == DISPLAY_KIND_GRAM) {
        display_io_get_info(display_panel_get_io(display->panel), &io);
        info->format = io.format;
        info->wire = io.wire;
        info->dma = (io.caps.flags & DISPLAY_IO_CAP_DMA) != 0;
        info->max_draw_bytes = io.caps.max_color_bytes;
        info->buf_align = io.caps.color_align > 1 ? io.caps.color_align : 1;
    } else {
        display_scanout_get_caps(display->scanout, &info->scanout);
        info->format = display->scanout->cfg.format;
        info->buf_align = 1; /* frames follow info.scanout.addr_align */
    }
    return 0;
}

int display_draw(display_t *display, const display_area_t *area, const void *pixels, size_t len)
{
    if (!display_ready(display)) {
        return -EINVAL;
    }
    if (display->scanout) {
        return -ENOTSUP;
    }
    return display_panel_draw(display->panel, area, pixels, len);
}

int display_draw_async(display_t *display, const display_area_t *area, const void *pixels, size_t len)
{
    if (!display_ready(display)) {
        return -EINVAL;
    }
    if (display->scanout) {
        return -ENOTSUP;
    }
    return display_panel_draw_async(display->panel, area, pixels, len, display_draw_done, display);
}

int display_flip(display_t *display, const display_fb_t *fb)
{
    if (!display_ready(display)) {
        return -EINVAL;
    }
    if (!display->scanout) {
        return -ENOTSUP;
    }
    if (display->state != DISPLAY_STATE_READY) {
        return -EPIPE;
    }
    return display_scanout_flip(display->scanout, fb);
}

int display_set_rotation(display_t *display, display_rotation_t rotation)
{
    if (!display_ready(display)) {
        return -EINVAL;
    }
    return display_panel_set_rotation(display->panel, rotation);
}

display_panel_t *display_get_panel(display_t *display)
{
    return display ? display->panel : NULL;
}

display_scanout_t *display_get_scanout(display_t *display)
{
    return display ? display->scanout : NULL;
}
