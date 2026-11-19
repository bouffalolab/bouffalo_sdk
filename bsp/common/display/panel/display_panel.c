/**
 * @file display_panel.c
 * @brief Panel core: state machine, argument validation, board power and reset.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "panel/display_panel.h"
#include "display_os.h"

/** @brief Board callback of a panel, NULL when not provided. */
#define PANEL_BOARD_OP(panel, op) ((panel)->board.ops ? (panel)->board.ops->op : NULL)

/**
 * @brief Switch the board power of a panel; no-op without a board power callback.
 */
static int panel_power(display_panel_t *panel, bool on)
{
    int (*power)(void *, bool) = PANEL_BOARD_OP(panel, power);

    return power ? power(panel->board.ctx, on) : 0;
}

int display_panel_init(display_panel_t *panel)
{
    bool io_was_attached;
    int ret;

    if (!panel || !panel->ops || !panel->ops->init) {
        return -EINVAL;
    }
    if (panel->initialized) {
        return 0;
    }
    if (PANEL_BOARD_OP(panel, init)) {
        ret = panel->board.ops->init(panel->board.ctx);
        if (ret) {
            return ret;
        }
    }
    /* Remember who attached the endpoint so that a failed init does not detach the application's one */
    io_was_attached = panel->io && panel->io->attached;
    if (panel->io) {
        ret = display_io_init(panel->io);
        if (ret) {
            return ret;
        }
    }
    ret = panel->ops->init(panel);
    if (ret) {
        if (panel->io && !io_was_attached) {
            display_io_deinit(panel->io); /* an endpoint the application attached stays attached */
        }
        return ret;
    }
    panel->state = DISPLAY_PANEL_OFF;
    panel->initialized = true;
    return 0;
}

int display_panel_deinit(display_panel_t *panel)
{
    int ret;

    if (!panel) {
        return -EINVAL;
    }
    if (!panel->initialized) {
        return 0;
    }
    ret = display_panel_unprepare(panel);
    if (ret) {
        /* Keep the endpoint and initialized state so a board power/reset error can be retried. */
        return ret;
    }
    if (panel->io) {
        int r = display_io_deinit(panel->io);

        if (r) {
            return ret ? ret : r; /* the endpoint is still attached: stay initialized so deinit can be retried */
        }
    }
    panel->initialized = false;
    return ret;
}

int display_panel_get_info(display_panel_t *panel, display_panel_info_t *info)
{
    if (!panel || !panel->initialized || !info) {
        return -EINVAL;
    }
    *info = panel->info;
    return 0;
}

display_io_t *display_panel_get_io(display_panel_t *panel)
{
    return panel ? panel->io : NULL;
}

int display_panel_prepare(display_panel_t *panel)
{
    int ret;

    if (!panel || !panel->initialized) {
        return -EINVAL;
    }
    if (panel->state != DISPLAY_PANEL_OFF) {
        return 0;
    }
    ret = panel_power(panel, true);
    if (ret) {
        return ret;
    }
    ret = panel->ops->prepare ? panel->ops->prepare(panel) : 0;
    if (ret) {
        if (panel_power(panel, false)) {
            /* Power-off failed; retain a state from which unprepare can retry. */
            panel->state = DISPLAY_PANEL_PREPARED;
        }
        return ret;
    }
    panel->state = DISPLAY_PANEL_PREPARED;
    return 0;
}

int display_panel_enable(display_panel_t *panel)
{
    int ret;

    if (!panel || !panel->initialized) {
        return -EINVAL;
    }
    if (panel->state == DISPLAY_PANEL_ENABLED) {
        return 0;
    }
    if (panel->state != DISPLAY_PANEL_PREPARED) {
        return -EPIPE;
    }
    ret = panel->ops->enable ? panel->ops->enable(panel) : 0;
    if (!ret) {
        panel->state = DISPLAY_PANEL_ENABLED;
    }
    return ret;
}

int display_panel_disable(display_panel_t *panel)
{
    int ret;

    if (!panel || !panel->initialized) {
        return -EINVAL;
    }
    if (panel->state != DISPLAY_PANEL_ENABLED) {
        return 0;
    }
    ret = panel->ops->disable ? panel->ops->disable(panel) : 0;
    panel->state = DISPLAY_PANEL_PREPARED; /* treated as off even on error; power-off follows */
    return ret;
}

int display_panel_unprepare(display_panel_t *panel)
{
    int ret;

    if (!panel || !panel->initialized) {
        return -EINVAL;
    }
    if (panel->state == DISPLAY_PANEL_OFF) {
        return 0;
    }
    ret = display_panel_disable(panel); /* the first error is reported, but the power-off sequence always completes */
    if (panel->ops->unprepare) {
        int r = panel->ops->unprepare(panel);

        ret = ret ? ret : r;
    }
    int power_ret = panel_power(panel, false);
    if (!ret) {
        ret = power_ret;
    }
    if (!power_ret) {
        panel->state = DISPLAY_PANEL_OFF;
    }
    return ret;
}

int display_panel_set_rotation(display_panel_t *panel, display_rotation_t rotation)
{
    if (!panel || !panel->initialized || (unsigned)rotation > DISPLAY_ROTATION_270) {
        return -EINVAL;
    }
    if (!(panel->info.rotation_mask & (1u << rotation))) {
        return -ENOTSUP;
    }
    if (panel->info.rotation == rotation) {
        return 0;
    }
    return panel->ops->set_rotation ? panel->ops->set_rotation(panel, rotation) : -ENOTSUP;
}

int display_panel_set_backlight(display_panel_t *panel, uint16_t level)
{
    int (*backlight)(void *, uint16_t);

    if (!panel || !panel->initialized || level > DISPLAY_BACKLIGHT_MAX) {
        return -EINVAL;
    }
    backlight = PANEL_BOARD_OP(panel, backlight);
    return backlight ? backlight(panel->board.ctx, level) : -ENOTSUP;
}

int display_panel_hw_reset(display_panel_t *panel, uint16_t low_ms, uint16_t wait_ms)
{
    int (*reset)(void *, bool);
    int ret;

    if (!panel) {
        return -EINVAL;
    }
    reset = PANEL_BOARD_OP(panel, reset);
    if (!reset) {
        display_os_delay_ms(wait_ms); /* no reset line: the panel still needs the time after power-up */
        return 0;
    }
    ret = reset(panel->board.ctx, true);
    if (ret) {
        return ret;
    }
    display_os_delay_ms(low_ms);
    ret = reset(panel->board.ctx, false);
    display_os_delay_ms(wait_ms);
    return ret;
}

/**
 * @brief Validate a draw request: state, area bounds, alignment and buffer length.
 *
 * @retval 0 valid
 * @retval -EINVAL invalid argument, area or length
 * @retval -ENOTSUP not a GRAM panel
 * @retval -EPIPE panel is OFF
 */
static int panel_draw_check(const display_panel_t *panel, const display_area_t *area, const void *pixels, size_t len)
{
    const display_panel_info_t *info;
    uint8_t xa, ya;

    if (!panel || !panel->initialized || !area || !pixels) {
        return -EINVAL;
    }
    if (!panel->info.gram || !panel->ops->draw) {
        return -ENOTSUP;
    }
    if (panel->state == DISPLAY_PANEL_OFF) {
        return -EPIPE;
    }

    info = &panel->info;
    xa = info->x_align > 1 ? info->x_align : 1;
    ya = info->y_align > 1 ? info->y_align : 1;
    if (info->format == DISPLAY_FMT_YUYV && xa < 2) {
        xa = 2; /* two YUYV pixels share one U/V pair */
    }
    if (!area->w || !area->h || (uint32_t)area->x + area->w > info->width ||
        (uint32_t)area->y + area->h > info->height) {
        return -EINVAL;
    }
    if (area->x % xa || area->w % xa || area->y % ya || area->h % ya) {
        return -EINVAL;
    }
    if (len != (size_t)area->w * area->h * display_format_bytes(info->format)) {
        return -EINVAL;
    }
    return 0;
}

int display_panel_draw(display_panel_t *panel, const display_area_t *area, const void *pixels, size_t len)
{
    int ret = panel_draw_check(panel, area, pixels, len);

    return ret ? ret : panel->ops->draw(panel, area, pixels, len, NULL, NULL);
}

int display_panel_draw_async(display_panel_t *panel, const display_area_t *area, const void *pixels, size_t len,
                             display_done_cb_t cb, void *ctx)
{
    int ret;

    if (!cb) {
        return -EINVAL;
    }
    ret = panel_draw_check(panel, area, pixels, len);
    return ret ? ret : panel->ops->draw(panel, area, pixels, len, cb, ctx);
}
