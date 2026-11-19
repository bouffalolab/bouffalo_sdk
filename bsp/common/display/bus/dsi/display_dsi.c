/**
 * @file display_dsi.c
 * @brief MIPI DSI host bus backend: checks, LP packet mapping, HS/LP arbitration.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bus/dsi/display_dsi_hw.h"
#include "scanout/bl618dg_dpu/display_bl618dg_dpu_clock.h"
#include <stddef.h>

/* The core hands out &bus->base and this backend casts it back: base has to stay the first member */
_Static_assert(offsetof(display_dsi_host_t, base) == 0, "display_bus_t must be the first member");
_Static_assert(offsetof(display_dsi_io_t, base) == 0, "display_io_t must be the first member");

/* DSI data types of the write packets */
#define DSI_DT_GENERIC_SHORT_0 0x03 /**< Generic short write, no parameter */
#define DSI_DT_GENERIC_SHORT_1 0x13 /**< Generic short write, 1 parameter */
#define DSI_DT_GENERIC_SHORT_2 0x23 /**< Generic short write, 2 parameters */
#define DSI_DT_GENERIC_LONG    0x29 /**< Generic long write */
#define DSI_DT_DCS_SHORT_0     0x05 /**< DCS short write, no parameter */
#define DSI_DT_DCS_SHORT_1     0x15 /**< DCS short write, 1 parameter */
#define DSI_DT_DCS_LONG        0x39 /**< DCS long write */

#define DSI_LANES_MAX          4u    /**< A D-PHY has 1, 2 or 4 data lanes */
#define DSI_PORCH_MAX          255u  /**< Vertical sync and front porch are 8-bit fields of the controller */
#define DSI_WIDTH_MAX          1280u /**< Largest line the line buffer of the controller takes */
#define DSI_VC_MAX             3u    /**< Virtual channels 0 to 3 */

/** @brief The number of data lanes is one a D-PHY has. */
static bool dsi_lanes_valid(uint8_t lanes)
{
    return lanes == 1 || lanes == 2 || lanes == DSI_LANES_MAX;
}

/** @brief The virtual channel of an endpoint (display_dsi_io_t, whose first member is the generic endpoint). */
static uint8_t dsi_vc(const display_io_t *io)
{
    return ((const display_dsi_io_t *)io)->virtual_channel;
}

/** @brief Close the video link of the controller, if one is open. */
static void dsi_close_video(display_dsi_host_t *host)
{
    if (host->video) {
        display_dsi_hw_close(host);
        host->video = NULL;
    }
}

/** @brief Get the DSI host from the core object (base is the first member). */
static display_dsi_host_t *to_host(display_bus_t *bus)
{
    return (display_dsi_host_t *)bus;
}

/**
 * @brief Select the packet for a command: DCS or generic write, short or long by the parameter count.
 *
 * @param [in]  flags   DISPLAY_DSI_PKT_* of the IC link
 * @param [in]  params  number of parameters after the command byte (a generic packet carries the command as its first
 *                      parameter, so one parameter makes a short packet of 2)
 * @param [out] type    DSI data type
 * @return true for a long packet
 */
static bool dsi_packet_type(uint8_t flags, uint32_t params, uint8_t *type)
{
    bool generic = (flags & DISPLAY_DSI_PKT_GENERIC) != 0;

    if (params >= 2 || (flags & DISPLAY_DSI_PKT_LONG)) {
        *type = generic ? DSI_DT_GENERIC_LONG : DSI_DT_DCS_LONG;
        return true;
    }
    if (params == 1) {
        *type = generic ? DSI_DT_GENERIC_SHORT_2 : DSI_DT_DCS_SHORT_1;
    } else {
        *type = generic ? DSI_DT_GENERIC_SHORT_1 : DSI_DT_DCS_SHORT_0;
    }
    return false;
}

/**
 * @brief Check the video link against the board wiring and the PLL.
 */
static int dsi_check_video(const display_dsi_host_t *host, const display_video_mode_t *video)
{
    const display_dsi_link_t *dsi = &video->dsi;

    if (video->link != DISPLAY_VLINK_DSI || !video->width || !video->height || !video->clock_hz) {
        return -EINVAL;
    }
    if (video->vsw > DSI_PORCH_MAX || video->vfp > DSI_PORCH_MAX || video->width > DSI_WIDTH_MAX) {
        return -EINVAL;
    }
    if (!dsi_lanes_valid(dsi->lanes) || dsi->lanes > host->cfg.lanes_wired) {
        return -EINVAL;
    }
    if (dsi->format < DISPLAY_DSI_FMT_RGB565 || dsi->format > DISPLAY_DSI_FMT_RGB888) {
        return -EINVAL;
    }
    if (host->cfg.lane_rate_max_mbps && dsi->lane_mbps > host->cfg.lane_rate_max_mbps) {
        return -ENOTSUP;
    }
    return display_bl618dg_dpu_dsipll_step(dsi->lane_mbps) < 0 ? -ENOTSUP : 0;
}

static int dsi_init(display_bus_t *bus)
{
    display_dsi_host_t *host = to_host(bus);

    if (!dsi_lanes_valid(host->cfg.lanes_wired)) {
        return -EINVAL;
    }
    host->video = NULL;
    host->hs_running = false;
    host->fault = 0;
    return display_dsi_hw_init(host);
}

static void dsi_deinit(display_bus_t *bus)
{
    display_dsi_host_t *host = to_host(bus);

    if (host->hs_running) {
        display_dsi_hw_hs_stop(host); /* best effort: the bus is going away */
        host->hs_running = false;
    }
    dsi_close_video(host);
    host->exclusive = false;
}

static int dsi_attach(display_bus_t *bus, display_io_t *io)
{
    display_dsi_host_t *host = to_host(bus);
    int ret;

    if (io->cfg.iface != DISPLAY_IFACE_DSI_LP) {
        return -ENOTSUP;
    }
    if (dsi_vc(io) > DSI_VC_MAX) {
        return -EINVAL;
    }
    ret = display_bus_claim(bus, &host->exclusive, &io->cfg);
    if (ret) {
        return ret;
    }
    io->caps = (display_io_caps_t){
        .max_param_bytes = DISPLAY_DSI_LP_PAYLOAD_MAX - 1u, /* commands only: no read, no pixels */
    };
    return 0;
}

/**
 * @brief Bring the link of the endpoint up on the first command; a link that is up stays up.
 */
static int dsi_apply(display_bus_t *bus, display_io_t *io)
{
    display_dsi_host_t *host = to_host(bus);
    int ret;

    if (host->fault) {
        return -EIO;
    }
    if (host->hs_running) {
        return -EBUSY; /* reprogramming the PLL would stop the video */
    }
    if (!io->video) {
        return -EINVAL; /* display_io_set_video() has not been called */
    }
    if (host->video == io->video) {
        return 0;
    }
    ret = dsi_check_video(host, io->video);
    if (ret) {
        return ret;
    }
    dsi_close_video(host);
    ret = display_dsi_hw_open(host, io->video, dsi_vc(io));
    if (!ret) {
        host->video = io->video;
    }
    return ret;
}

static int dsi_write(display_bus_t *bus, display_io_t *io, const display_io_msg_t *msg)
{
    display_dsi_host_t *host = to_host(bus);
    uint8_t buf[DISPLAY_DSI_LP_PAYLOAD_MAX];
    uint8_t type;
    uint8_t flags;
    bool is_long;
    int ret;

    if (host->fault) {
        return -EIO;
    }
    if (host->hs_running) {
        return -EBUSY; /* an LP packet would silently stop the video */
    }
    if (msg->cmd > UINT8_MAX) {
        return -EINVAL; /* DCS commands are 8 bit, and a packet always carries one */
    }
    if (msg->len >= DISPLAY_DSI_LP_PAYLOAD_MAX) {
        return -E2BIG;
    }
    buf[0] = (uint8_t)msg->cmd;
    for (uint32_t i = 0; i < msg->len; i++) {
        buf[1 + i] = ((const uint8_t *)msg->tx)[i];
    }
    flags = io->link ? io->link->dsi_flags : 0;
    if (msg->flags & DISPLAY_IO_MSG_DSI_GENERIC) {
        flags |= DISPLAY_DSI_PKT_GENERIC;
    }
    if (msg->flags & DISPLAY_IO_MSG_DSI_LONG) {
        flags |= DISPLAY_DSI_PKT_LONG;
    }
    is_long = dsi_packet_type(flags, msg->len, &type);
    ret = display_dsi_hw_send(host, type, is_long, dsi_vc(io), buf, (uint16_t)(1u + msg->len));
    if (ret) {
        host->fault = ret; /* the controller cannot be reset on its own */
        return -EIO;
    }
    return 0;
}

const display_bus_ops_t display_dsi_bus_ops = {
    .init = dsi_init,
    .deinit = dsi_deinit,
    .attach = dsi_attach,
    .apply = dsi_apply,
    .write = dsi_write,
};

int display_dsi_hs_start(display_dsi_host_t *host)
{
    int ret;

    if (!host) {
        return -EINVAL;
    }
    ret = display_bus_hold(&host->base);
    if (ret) {
        return ret;
    }
    if (host->fault) {
        ret = -EIO;
    } else if (!host->video) {
        ret = -EINVAL;
    } else if (!host->hs_running) {
        display_dsi_hw_hs_start(host, host->video);
        host->hs_running = true;
    }
    display_bus_unhold(&host->base);
    return ret;
}

int display_dsi_hs_stop(display_dsi_host_t *host)
{
    int ret;

    if (!host) {
        return -EINVAL;
    }
    ret = display_bus_hold(&host->base);
    if (ret) {
        return ret;
    }
    if (host->hs_running) {
        ret = display_dsi_hw_hs_stop(host);
        if (!ret) {
            host->hs_running = false;
        }
    }
    display_bus_unhold(&host->base);
    return ret;
}
