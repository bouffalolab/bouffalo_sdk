/**
 * @file display_io.c
 * @brief IO endpoint API. This is the only place where messages are validated; the bus core and
 *        backends assume valid arguments.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bus/display_bus_driver.h"

int display_io_init(display_io_t *io)
{
    if (!io) {
        return -EINVAL;
    }
    return io->attached ? 0 : display_bus_attach(io);
}

int display_io_deinit(display_io_t *io)
{
    if (!io) {
        return -EINVAL;
    }
    return io->attached ? display_bus_detach(io) : 0;
}

int display_io_get_info(display_io_t *io, display_io_info_t *info)
{
    if (!io || !io->attached || !info) {
        return -EINVAL;
    }
    info->caps = io->caps;
    info->format = io->cfg.format;
    info->wire = io->wire;
    info->iface = io->cfg.iface;
    info->cmd_mode = display_io_get_cmd_mode(io);
    return 0;
}

/**
 * @brief Release the hold of display_io_set_link() / display_io_set_video(), after making the controller reconfigure
 *        for the endpoint on the next transfer.
 */
static void io_unhold_reapply(display_io_t *io)
{
    if (io->bus->applied == io) {
        io->bus->applied = NULL;
    }
    display_bus_unhold(io->bus);
}

int display_io_set_link(display_io_t *io, const display_link_t *link)
{
    int ret;

    if (!io || !io->attached || !link || link->iface != io->cfg.iface) {
        return -EINVAL;
    }
    if (link->iface == DISPLAY_IFACE_QSPI && (!link->qspi.cmd.code || !link->qspi.color.code)) {
        return -EINVAL; /* opcode 0: the QSPI protocol was not filled in */
    }
    ret = display_bus_hold(io->bus);
    if (ret) {
        return ret;
    }
    io->link = link;
    io_unhold_reapply(io);
    return 0;
}

int display_io_set_video(display_io_t *io, const display_video_mode_t *video)
{
    int ret;

    if (!io || !io->attached) {
        return -EINVAL;
    }
    ret = display_bus_hold(io->bus);
    if (ret) {
        return ret;
    }
    io->video = video;
    io_unhold_reapply(io);
    return 0;
}

uint32_t display_io_get_freq(const display_io_t *io)
{
    if (io->cfg.freq_hz) {
        return io->cfg.freq_hz;
    }
    if (io->link && io->link->default_hz) {
        return io->link->default_hz;
    }
    return DISPLAY_IFACE_DEFAULT_HZ(io->cfg.iface);
}

display_cmd_mode_t display_io_get_cmd_mode(const display_io_t *io)
{
    return io->link ? io->link->cmd_mode : DISPLAY_CMD_8;
}

/**
 * @brief Validate one message against the endpoint capabilities.
 */
static int msg_check(const display_io_t *io, const display_io_msg_t *msg, bool last)
{
    bool color = (msg->flags & DISPLAY_IO_MSG_COLOR) != 0;
    bool read = (msg->flags & DISPLAY_IO_MSG_READ) != 0;

    if (color && (read || !last || !msg->len || !msg->tx)) {
        return -EINVAL; /* pixels: last message only, with data */
    }
    if (read) {
        if (!(io->caps.flags & DISPLAY_IO_CAP_READ) || display_io_get_cmd_mode(io) != DISPLAY_CMD_8) {
            return -ENOTSUP;
        }
        if (!msg->rx || !msg->len) {
            return -EINVAL;
        }
    } else if (msg->len && !msg->tx) {
        return -EINVAL;
    }

    if (color) {
        if (!(io->caps.format_mask & DISPLAY_FMT_BIT(io->cfg.format))) {
            return -ENOTSUP; /* command-only endpoint */
        }
        if (msg->len % display_format_bytes(io->cfg.format)) {
            return -EINVAL;
        }
        if (msg->len > io->caps.max_color_bytes) {
            return -E2BIG;
        }
        if (io->caps.color_align > 1 && ((uintptr_t)msg->tx % io->caps.color_align)) {
            return -EINVAL;
        }
    } else if (msg->len > io->caps.max_param_bytes) {
        return -E2BIG;
    }
    return 0;
}

/**
 * @brief Validate a request.
 */
static int xfer_check(const display_io_t *io, const display_io_msg_t *msgs, size_t count, bool async)
{
    if (!io || !msgs || !count) {
        return -EINVAL;
    }
    if (!io->attached) {
        return -EPIPE;
    }
    if (display_os_in_isr()) {
        return -EPERM; /* waiting for the bus is not possible in an ISR */
    }
    for (size_t i = 0; i < count; i++) {
        int ret = msg_check(io, &msgs[i], i == count - 1);

        if (ret) {
            return ret;
        }
    }
    if (async && (!(msgs[count - 1].flags & DISPLAY_IO_MSG_COLOR) || msgs[count - 1].delay_ms)) {
        return -EINVAL;
    }
    return 0;
}

int display_io_xfer(display_io_t *io, const display_io_msg_t *msgs, size_t count)
{
    int ret = xfer_check(io, msgs, count, false);

    return ret ? ret : display_bus_xfer(io, msgs, count, NULL, NULL);
}

int display_io_xfer_async(display_io_t *io, const display_io_msg_t *msgs, size_t count, display_done_cb_t cb, void *ctx)
{
    int ret;

    if (!cb) {
        return -EINVAL;
    }
    ret = xfer_check(io, msgs, count, true);
    return ret ? ret : display_bus_xfer(io, msgs, count, cb, ctx);
}

int display_io_write(display_io_t *io, uint32_t cmd, const void *param, size_t len)
{
    display_io_msg_t msg = { .cmd = cmd, .tx = param, .len = (uint32_t)len };

    return display_io_xfer(io, &msg, 1);
}

int display_io_read(display_io_t *io, uint32_t cmd, void *buf, size_t len)
{
    display_io_msg_t msg = { .cmd = cmd, .rx = buf, .len = (uint32_t)len, .flags = DISPLAY_IO_MSG_READ };

    return display_io_xfer(io, &msg, 1);
}

int display_io_write_color(display_io_t *io, uint32_t cmd, const void *pixels, size_t len)
{
    display_io_msg_t msg = { .cmd = cmd, .tx = pixels, .len = (uint32_t)len, .flags = DISPLAY_IO_MSG_COLOR };

    return display_io_xfer(io, &msg, 1);
}

int display_io_write_color_async(display_io_t *io, uint32_t cmd, const void *pixels, size_t len, display_done_cb_t cb,
                                 void *ctx)
{
    display_io_msg_t msg = { .cmd = cmd, .tx = pixels, .len = (uint32_t)len, .flags = DISPLAY_IO_MSG_COLOR };

    return display_io_xfer_async(io, &msg, 1, cb, ctx);
}
