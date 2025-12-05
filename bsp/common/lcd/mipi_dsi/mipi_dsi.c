#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "bflb_core.h"
#include "bflb_dsi.h"
#include "mipi_dsi.h"
#include "mipi_display.h"

/* Cached device instance to avoid repeated lookups in the hot path */
static struct bflb_device_s *dsi_dev = NULL;

/* Helper to resolve the bflb device instance for the compatibility wrappers */
static struct bflb_device_s *get_default_dsi_dev(void)
{
    struct bflb_device_s *dev = bflb_device_get_by_name("dsi");
    if (!dev) {
        /* Try alternative name used in some trees */
        dev = bflb_device_get_by_name("dsi0");
    }
    return dev;
}

bool mipi_dsi_packet_format_is_short(uint8_t type)
{
    switch (type) {
        case MIPI_DSI_V_SYNC_START:
        case MIPI_DSI_V_SYNC_END:
        case MIPI_DSI_H_SYNC_START:
        case MIPI_DSI_H_SYNC_END:
        case MIPI_DSI_COMPRESSION_MODE:
        case MIPI_DSI_END_OF_TRANSMISSION:
        case MIPI_DSI_COLOR_MODE_OFF:
        case MIPI_DSI_COLOR_MODE_ON:
        case MIPI_DSI_SHUTDOWN_PERIPHERAL:
        case MIPI_DSI_TURN_ON_PERIPHERAL:
        case MIPI_DSI_GENERIC_SHORT_WRITE_0_PARAM:
        case MIPI_DSI_GENERIC_SHORT_WRITE_1_PARAM:
        case MIPI_DSI_GENERIC_SHORT_WRITE_2_PARAM:
        case MIPI_DSI_GENERIC_READ_REQUEST_0_PARAM:
        case MIPI_DSI_GENERIC_READ_REQUEST_1_PARAM:
        case MIPI_DSI_GENERIC_READ_REQUEST_2_PARAM:
        case MIPI_DSI_DCS_SHORT_WRITE:
        case MIPI_DSI_DCS_SHORT_WRITE_PARAM:
        case MIPI_DSI_DCS_READ:
        case MIPI_DSI_EXECUTE_QUEUE:
        case MIPI_DSI_SET_MAXIMUM_RETURN_PACKET_SIZE:
            return true;
    }
    return false;
}

bool mipi_dsi_packet_format_is_long(uint8_t type)
{
    switch (type) {
        case MIPI_DSI_NULL_PACKET:
        case MIPI_DSI_BLANKING_PACKET:
        case MIPI_DSI_GENERIC_LONG_WRITE:
        case MIPI_DSI_DCS_LONG_WRITE:
        case MIPI_DSI_PICTURE_PARAMETER_SET:
        case MIPI_DSI_COMPRESSED_PIXEL_STREAM:
        case MIPI_DSI_LOOSELY_PACKED_PIXEL_STREAM_YCBCR20:
        case MIPI_DSI_PACKED_PIXEL_STREAM_YCBCR24:
        case MIPI_DSI_PACKED_PIXEL_STREAM_YCBCR16:
        case MIPI_DSI_PACKED_PIXEL_STREAM_30:
        case MIPI_DSI_PACKED_PIXEL_STREAM_36:
        case MIPI_DSI_PACKED_PIXEL_STREAM_YCBCR12:
        case MIPI_DSI_PACKED_PIXEL_STREAM_16:
        case MIPI_DSI_PACKED_PIXEL_STREAM_18:
        case MIPI_DSI_PIXEL_STREAM_3BYTE_18:
        case MIPI_DSI_PACKED_PIXEL_STREAM_24:
            return true;
    }

    return false;
}

int mipi_dsi_create_packet(struct mipi_dsi_packet *packet,
                           const struct mipi_dsi_msg *msg)
{
    if (!packet || !msg) {
        return -EINVAL;
    }

    if (!mipi_dsi_packet_format_is_short(msg->type) &&
        !mipi_dsi_packet_format_is_long(msg->type)) {
        return -EINVAL;
    }

    if (msg->channel > 3) {
        return -EINVAL;
    }

    memset(packet, 0, sizeof(*packet));
    packet->header[0] = ((msg->channel & 0x3) << 6) | (msg->type & 0x3f);

    if (mipi_dsi_packet_format_is_long(msg->type)) {
        packet->header[1] = (msg->tx_len >> 0) & 0xff;
        packet->header[2] = (msg->tx_len >> 8) & 0xff;
        packet->payload_length = msg->tx_len;
        packet->payload = msg->tx_buf;
    } else {
        const uint8_t *tx = msg->tx_buf;
        packet->header[1] = (msg->tx_len > 0) ? tx[0] : 0;
        packet->header[2] = (msg->tx_len > 1) ? tx[1] : 0;
    }

    packet->size = sizeof(packet->header) + packet->payload_length;

    return 0;
}

static int mipi_dsi_device_transfer(const struct mipi_dsi_device *dsi,
                                       struct mipi_dsi_msg *msg)
{
    bflb_dsi_lpdt_msg_t drvMsg = {
        .data_type = msg->type,
        .virtual_chan = msg->channel,
        .tx_buf = msg->tx_buf,
        .tx_len = msg->tx_len,
        .rx_buf = msg->rx_buf,
        .rx_len = 0,
    };

    /* Prefer cached device if available to avoid repeated lookups */
    if (!dsi_dev) {
        dsi_dev = get_default_dsi_dev();
    }
    if (!dsi_dev) {
        return -ENOSYS;
    }

    if (mipi_dsi_packet_format_is_short(msg->type)) {
        if (dsi && (dsi->mode_flags & MIPI_DSI_MODE_LPM)) {
            if (0 != bflb_dsi_lpdt_send_short_packet(dsi_dev, &drvMsg)) {
                return -EIO;
            }
        } else {
            return -ENOSYS;
        }
    } else {
        if (dsi && (dsi->mode_flags & MIPI_DSI_MODE_LPM)) {
            if (0 != bflb_dsi_lpdt_send_long_packet(dsi_dev, &drvMsg)) {
                return -EIO;
            }
        } else {
            return -ENOSYS;
        }
    }

    if (msg->flags & MIPI_DSI_MSG_REQ_ACK) {
        if (dsi && (dsi->mode_flags & MIPI_DSI_MODE_LPM)) {
            if (0 == bflb_dsi_lpdt_recv(dsi_dev, &drvMsg)) {
                msg->rx_len = drvMsg.rx_len;
                return (int)msg->rx_len;
            } else {
                return -EIO;
            }
        } else {
            return -ENOSYS;
        }
    }

    return (int)msg->tx_len;
}

int mipi_dsi_controller_init(const struct mipi_dsi_device *dsi)
{
    dsi_dev = get_default_dsi_dev();
    if (!dsi_dev) {
        return -ENOSYS;
    }

    bflb_dsi_config_t cfg = {
        .virtual_chan = 0,
        .lane_num = BFLB_DSI_LANES_4,
        .lane_order = BFLB_DSI_LANE_ORDER_3210,
        .data_type = BFLB_DSI_DATA_RGB888,
        .sync_type = BFLB_DSI_HS_SYNC_EVENT_MODE,
        .vsa = 10,
        .vfp = 10,
    };
    bflb_dsi_dphy_config_t dphy = {
        .time_clk_exit = 5,
        .time_clk_trail = 3,
        .time_clk_zero = 15,
        .time_data_exit = 5,
        .time_data_prepare = 2,
        .time_data_trail = 3,
        .time_data_zero = 15,
        .time_lpx = 3,
        .time_req_ready = 1,
        .time_ta_get = 15,
        .time_ta_go = 12,
        .time_wakeup = 40001,
    };
    uint32_t usedLanes = BFLB_DSI_LANE_CLOCK;

    if (dsi) {
        cfg.virtual_chan = (uint8_t)dsi->channel;
        if (dsi->lanes == 1) {
            cfg.lane_num = BFLB_DSI_LANES_1;
        } else if (dsi->lanes == 2) {
            cfg.lane_num = BFLB_DSI_LANES_2;
        } else {
            cfg.lane_num = BFLB_DSI_LANES_4;
        }
        cfg.data_type = (uint8_t)dsi->format;
    }

    if (cfg.lane_num == BFLB_DSI_LANES_1) {
        usedLanes |= BFLB_DSI_LANE_DATA0;
    } else if (cfg.lane_num == BFLB_DSI_LANES_2) {
        usedLanes |= BFLB_DSI_LANE_DATA0 | BFLB_DSI_LANE_DATA1;
    } else {
        usedLanes |= BFLB_DSI_LANE_DATA0 | BFLB_DSI_LANE_DATA1 | BFLB_DSI_LANE_DATA2 | BFLB_DSI_LANE_DATA3;
    }

    bflb_dsi_init(dsi_dev, &cfg);
    bflb_dsi_phy_reset(dsi_dev);
    bflb_dsi_phy_enable(dsi_dev);
    bflb_dsi_phy_config(dsi_dev, &dphy);
    bflb_dsi_phy_enable_lanes(dsi_dev, usedLanes);

    return 0;
}

int mipi_dsi_shutdown_peripheral(const struct mipi_dsi_device *dsi)
{
    struct mipi_dsi_msg msg = {
        .channel = dsi ? (uint8_t)dsi->channel : 0,
        .type = MIPI_DSI_SHUTDOWN_PERIPHERAL,
        .tx_buf = (uint8_t[]){0, 0},
        .tx_len = 2,
    };
    int ret = mipi_dsi_device_transfer(dsi, &msg);
    return (ret < 0) ? ret : 0;
}

int mipi_dsi_turn_on_peripheral(const struct mipi_dsi_device *dsi)
{
    struct mipi_dsi_msg msg = {
        .channel = dsi ? (uint8_t)dsi->channel : 0,
        .type = MIPI_DSI_TURN_ON_PERIPHERAL,
        .tx_buf = (uint8_t[]){0, 0},
        .tx_len = 2,
    };
    int ret = mipi_dsi_device_transfer(dsi, &msg);
    return (ret < 0) ? ret : 0;
}

int mipi_dsi_set_maximum_return_packet_size(const struct mipi_dsi_device *dsi, uint16_t value)
{
    uint8_t tx[2] = { value & 0xff, value >> 8 };
    struct mipi_dsi_msg msg = {
        .channel = dsi ? (uint8_t)dsi->channel : 0,
        .type = MIPI_DSI_SET_MAXIMUM_RETURN_PACKET_SIZE,
        .tx_len = sizeof(tx),
        .tx_buf = tx,
    };
    int ret = mipi_dsi_device_transfer(dsi, &msg);
    return (ret < 0) ? ret : 0;
}

int mipi_dsi_compression_mode(const struct mipi_dsi_device *dsi, bool enable)
{
    uint8_t tx[2] = { enable << 0, 0 };
    struct mipi_dsi_msg msg = {
        .channel = dsi ? (uint8_t)dsi->channel : 0,
        .type = MIPI_DSI_COMPRESSION_MODE,
        .tx_len = sizeof(tx),
        .tx_buf = tx,
    };
    int ret = mipi_dsi_device_transfer(dsi, &msg);
    return (ret < 0) ? ret : 0;
}

int mipi_dsi_picture_parameter_set(const struct mipi_dsi_device *dsi,
                                      const struct drm_dsc_picture_parameter_set *pps)
{
    struct mipi_dsi_msg msg = {
        .channel = dsi ? (uint8_t)dsi->channel : 0,
        .type = MIPI_DSI_PICTURE_PARAMETER_SET,
        .tx_len = 128, /* PPS is 128 bytes per VESA DSC */
        .tx_buf = pps,
    };
    int ret = mipi_dsi_device_transfer(dsi, &msg);
    return (ret < 0) ? ret : 0;
}

int mipi_dsi_generic_write(const struct mipi_dsi_device *dsi, const void *payload,
                              size_t size)
{
    struct mipi_dsi_msg msg = {
        .channel = dsi ? (uint8_t)dsi->channel : 0,
        .tx_buf = payload,
        .tx_len = size
    };

    switch (size) {
        case 0:
            msg.type = MIPI_DSI_GENERIC_SHORT_WRITE_0_PARAM;
            break;
        case 1:
            msg.type = MIPI_DSI_GENERIC_SHORT_WRITE_1_PARAM;
            break;
        case 2:
            msg.type = MIPI_DSI_GENERIC_SHORT_WRITE_2_PARAM;
            break;
        default:
            msg.type = MIPI_DSI_GENERIC_LONG_WRITE;
            break;
    }

    return mipi_dsi_device_transfer(dsi, &msg);
}

int mipi_dsi_generic_read(const struct mipi_dsi_device *dsi, const void *params,
                             size_t num_params, void *data, size_t size)
{
    struct mipi_dsi_msg msg = {
        .channel = dsi ? (uint8_t)dsi->channel : 0,
        .tx_len = num_params,
        .tx_buf = params,
        .rx_len = size,
        .rx_buf = data
    };

    switch (num_params) {
        case 0:
            msg.type = MIPI_DSI_GENERIC_READ_REQUEST_0_PARAM;
            break;
        case 1:
            msg.type = MIPI_DSI_GENERIC_READ_REQUEST_1_PARAM;
            break;
        case 2:
            msg.type = MIPI_DSI_GENERIC_READ_REQUEST_2_PARAM;
            break;
        default:
            return -EINVAL;
    }

    return mipi_dsi_device_transfer(dsi, &msg);
}

int mipi_dsi_dcs_write_buffer(const struct mipi_dsi_device *dsi,
                                 const void *data, size_t len)
{
    struct mipi_dsi_msg msg = {
        .channel = dsi ? (uint8_t)dsi->channel : 0,
        .tx_buf = data,
        .tx_len = len
    };

    if (len == 0) {
        return -EINVAL;
    } else if (len == 1) {
        msg.type = MIPI_DSI_DCS_SHORT_WRITE;
    } else if (len == 2) {
        msg.type = MIPI_DSI_DCS_SHORT_WRITE_PARAM;
    } else {
        msg.type = MIPI_DSI_DCS_LONG_WRITE;
    }

    return mipi_dsi_device_transfer(dsi, &msg);
}

int mipi_dsi_dcs_write(const struct mipi_dsi_device *dsi, uint8_t cmd,
                          const void *data, size_t len)
{
    int err;
    size_t size = 1 + len;
    uint8_t stack_tx[64];
    uint8_t *tx;

    if (len > sizeof(stack_tx) - 1) {
        return -ENOMEM;
    } else {
        tx = stack_tx;
    }

    tx[0] = cmd;
    if (data) {
        memcpy(&tx[1], data, len);
    }

    err = mipi_dsi_dcs_write_buffer(dsi, tx, size);
    return err;
}

int mipi_dsi_dcs_read(const struct mipi_dsi_device *dsi, uint8_t cmd, void *data,
                         size_t len)
{
    uint8_t wData[2] = {0};
    struct mipi_dsi_msg msg = {
        .channel = dsi ? (uint8_t)dsi->channel : 0,
        .type = MIPI_DSI_DCS_READ,
        .flags = MIPI_DSI_MSG_REQ_ACK,
        .tx_buf = wData,
        .tx_len = 1,
        .rx_buf = data,
        .rx_len = len
    };

    wData[0] = cmd;

    return mipi_dsi_device_transfer(dsi, &msg);
}

int mipi_dsi_dcs_nop(const struct mipi_dsi_device *dsi)
{
    int err = mipi_dsi_dcs_write(dsi, MIPI_DCS_NOP, NULL, 0);
    return (err < 0) ? err : 0;
}

int mipi_dsi_dcs_soft_reset(const struct mipi_dsi_device *dsi)
{
    int err = mipi_dsi_dcs_write(dsi, MIPI_DCS_SOFT_RESET, NULL, 0);
    return (err < 0) ? err : 0;
}

int mipi_dsi_dcs_get_power_mode(const struct mipi_dsi_device *dsi, uint8_t *mode)
{
    int err = mipi_dsi_dcs_read(dsi, MIPI_DCS_GET_POWER_MODE, mode, sizeof(*mode));
    if (err <= 0) {
        if (err == 0) {
            err = -ENODATA;
        }
        return err;
    }
    return 0;
}

int mipi_dsi_dcs_get_pixel_format(const struct mipi_dsi_device *dsi, uint8_t *format)
{
    int err = mipi_dsi_dcs_read(dsi, MIPI_DCS_GET_PIXEL_FORMAT, format, sizeof(*format));
    if (err <= 0) {
        if (err == 0) {
            err = -ENODATA;
        }
        return err;
    }
    return 0;
}

int mipi_dsi_dcs_enter_sleep_mode(const struct mipi_dsi_device *dsi)
{
    int err = mipi_dsi_dcs_write(dsi, MIPI_DCS_ENTER_SLEEP_MODE, NULL, 0);
    return (err < 0) ? err : 0;
}

int mipi_dsi_dcs_exit_sleep_mode(const struct mipi_dsi_device *dsi)
{
    int err = mipi_dsi_dcs_write(dsi, MIPI_DCS_EXIT_SLEEP_MODE, NULL, 0);
    return (err < 0) ? err : 0;
}

int mipi_dsi_dcs_set_display_off(const struct mipi_dsi_device *dsi)
{
    int err = mipi_dsi_dcs_write(dsi, MIPI_DCS_SET_DISPLAY_OFF, NULL, 0);
    return (err < 0) ? err : 0;
}

int mipi_dsi_dcs_set_display_on(const struct mipi_dsi_device *dsi)
{
    int err = mipi_dsi_dcs_write(dsi, MIPI_DCS_SET_DISPLAY_ON, NULL, 0);
    return (err < 0) ? err : 0;
}

int mipi_dsi_dcs_set_column_address(const struct mipi_dsi_device *dsi, uint16_t start, uint16_t end)
{
    uint8_t payload[4] = { start >> 8, start & 0xff, end >> 8, end & 0xff };
    int err = mipi_dsi_dcs_write(dsi, MIPI_DCS_SET_COLUMN_ADDRESS, payload, sizeof(payload));
    return (err < 0) ? err : 0;
}

int mipi_dsi_dcs_set_page_address(const struct mipi_dsi_device *dsi, uint16_t start, uint16_t end)
{
    uint8_t payload[4] = { start >> 8, start & 0xff, end >> 8, end & 0xff };
    int err = mipi_dsi_dcs_write(dsi, MIPI_DCS_SET_PAGE_ADDRESS, payload, sizeof(payload));
    return (err < 0) ? err : 0;
}

int mipi_dsi_dcs_set_tear_off(const struct mipi_dsi_device *dsi)
{
    int err = mipi_dsi_dcs_write(dsi, MIPI_DCS_SET_TEAR_OFF, NULL, 0);
    return (err < 0) ? err : 0;
}

int mipi_dsi_dcs_set_tear_on(const struct mipi_dsi_device *dsi, int mode)
{
    uint8_t value = (uint8_t)mode;
    int err = mipi_dsi_dcs_write(dsi, MIPI_DCS_SET_TEAR_ON, &value, sizeof(value));
    return (err < 0) ? err : 0;
}

int mipi_dsi_dcs_set_pixel_format(const struct mipi_dsi_device *dsi, uint8_t format)
{
    int err = mipi_dsi_dcs_write(dsi, MIPI_DCS_SET_PIXEL_FORMAT, &format, sizeof(format));
    return (err < 0) ? err : 0;
}

int mipi_dsi_dcs_set_tear_scanline(const struct mipi_dsi_device *dsi, uint16_t scanline)
{
    uint8_t payload[2] = { scanline >> 8, scanline & 0xff };
    int err = mipi_dsi_dcs_write(dsi, MIPI_DCS_SET_TEAR_SCANLINE, payload, sizeof(payload));
    return (err < 0) ? err : 0;
}

int mipi_dsi_dcs_set_display_brightness(const struct mipi_dsi_device *dsi, uint16_t brightness)
{
    uint8_t payload[2] = { brightness & 0xff, brightness >> 8 };
    int err = mipi_dsi_dcs_write(dsi, MIPI_DCS_SET_DISPLAY_BRIGHTNESS, payload, sizeof(payload));
    return (err < 0) ? err : 0;
}

int mipi_dsi_dcs_get_display_brightness(const struct mipi_dsi_device *dsi, uint16_t *brightness)
{
    int err = mipi_dsi_dcs_read(dsi, MIPI_DCS_GET_DISPLAY_BRIGHTNESS, brightness, sizeof(*brightness));
    if (err <= 0) {
        if (err == 0) {
            err = -ENODATA;
        }
        return err;
    }
    return 0;
}
