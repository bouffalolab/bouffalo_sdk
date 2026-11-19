/**
 * @file display_soft_dbi_c.c
 * @brief Software DBI Type C (3-wire / 4-wire serial) bus backend, bit-banged on GPIO.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bus/soft_dbi_c/display_soft_dbi_c.h"
#include "bflb_gpio.h"
#include <stddef.h>

/* The core hands out &bus->base and this backend casts it back: base has to stay the first member */
_Static_assert(offsetof(display_soft_dbi_c_bus_t, base) == 0, "display_bus_t must be the first member");

#define SOFT_DBI_C_PARAM_MAX 4096u

/** @brief Get the software DBI Type C bus from the core object (base is the first member). */
static display_soft_dbi_c_bus_t *to_soft_bus(display_bus_t *bus)
{
    return (display_soft_dbi_c_bus_t *)bus;
}

/** @brief Drive a bus pin through the GPIO set/reset registers. */
static void soft_pin_write(display_soft_dbi_c_bus_t *bus, const display_pin_t *pin, bool level)
{
    if (level) {
        bflb_gpio_set(bus->gpio, pin->num);
    } else {
        bflb_gpio_reset(bus->gpio, pin->num);
    }
}

/** @brief Sample MISO; reads as low when no MISO pin is wired. */
static bool soft_miso_read(display_soft_dbi_c_bus_t *bus)
{
    return display_pin_used(&bus->cfg.miso) && bflb_gpio_read(bus->gpio, bus->cfg.miso.num);
}

/**
 * @brief Clock one bit out and one bit in (SPI mode 0: data before the rising edge, idle low).
 *
 * @return the sampled MISO level
 */
static bool soft_clock_bit(display_soft_dbi_c_bus_t *bus, bool bit)
{
    bool in;

    soft_pin_write(bus, &bus->cfg.mosi, bit);
    soft_pin_write(bus, &bus->cfg.sck, true);
    in = soft_miso_read(bus);
    soft_pin_write(bus, &bus->cfg.sck, false);
    return in;
}

/**
 * @brief Transfer one byte, MSB first. On the 3-wire interface the D/C bit (0: command, 1: data) goes first.
 */
static uint8_t soft_xfer_byte(display_soft_dbi_c_bus_t *bus, const display_io_t *io, bool data, uint8_t out)
{
    uint8_t in = 0;

    if (io->cfg.iface == DISPLAY_IFACE_DBI_C3) {
        soft_clock_bit(bus, data);
    }
    for (int i = 7; i >= 0; i--) {
        in = (uint8_t)((in << 1) | soft_clock_bit(bus, (out >> i) & 1u));
    }
    return in;
}

/**
 * @brief Send pixels in wire order (R first).
 */
static void soft_send_pixels(display_soft_dbi_c_bus_t *bus, const display_io_t *io, const uint8_t *px, uint32_t len)
{
    switch (io->cfg.format) {
        case DISPLAY_FMT_RGB565: /* little-endian uint16: high byte first */
            for (uint32_t i = 0; i + 1 < len; i += 2) {
                soft_xfer_byte(bus, io, true, px[i + 1]);
                soft_xfer_byte(bus, io, true, px[i]);
            }
            break;
        case DISPLAY_FMT_RGB888: /* memory B, G, R */
            for (uint32_t i = 0; i + 2 < len; i += 3) {
                soft_xfer_byte(bus, io, true, px[i + 2]);
                soft_xfer_byte(bus, io, true, px[i + 1]);
                soft_xfer_byte(bus, io, true, px[i]);
            }
            break;
        default: /* XRGB8888: memory B, G, R, X */
            for (uint32_t i = 0; i + 3 < len; i += 4) {
                soft_xfer_byte(bus, io, true, px[i + 2]);
                soft_xfer_byte(bus, io, true, px[i + 1]);
                soft_xfer_byte(bus, io, true, px[i]);
            }
            break;
    }
}

/*****************************************************************************
 * Bus operations
 *****************************************************************************/

/** @brief Bus op init: claim the pins and put them at their idle levels. */
static int soft_init(display_bus_t *base)
{
    display_soft_dbi_c_bus_t *bus = to_soft_bus(base);

    bus->gpio = bflb_device_get_by_name("gpio");
    if (!bus->gpio || !display_pin_used(&bus->cfg.sck) || !display_pin_used(&bus->cfg.mosi)) {
        return -EINVAL;
    }
    display_pin_output(&bus->cfg.sck, false);
    display_pin_output(&bus->cfg.mosi, false);
    display_pin_output(&bus->cfg.dc, true);
    if (display_pin_used(&bus->cfg.miso)) {
        bflb_gpio_init(bus->gpio, bus->cfg.miso.num, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN);
    }
    return 0;
}

/** @brief Bus op attach: 3-wire always, 4-wire when the bus has a DC pin. */
static int soft_attach(display_bus_t *base, display_io_t *io)
{
    display_soft_dbi_c_bus_t *bus = to_soft_bus(base);
    const display_io_cfg_t *cfg = &io->cfg;
    bool c4 = cfg->iface == DISPLAY_IFACE_DBI_C4;
    bool supported = cfg->iface == DISPLAY_IFACE_DBI_C3 || (c4 && display_pin_used(&bus->cfg.dc));
    int ret;

    if (!supported) {
        return -ENOTSUP; /* C4 needs the DC pin of the bus */
    }
    ret = display_bus_claim(base, &bus->exclusive, cfg);
    if (ret) {
        return ret;
    }

    io->caps = (display_io_caps_t){
        .flags = DISPLAY_IO_CAP_FIXED_WIRE | ((display_pin_used(&bus->cfg.miso) && c4) ? DISPLAY_IO_CAP_READ : 0),
        .format_mask = DISPLAY_FMT_BIT(DISPLAY_FMT_RGB565) | DISPLAY_FMT_BIT(DISPLAY_FMT_RGB888) |
                       DISPLAY_FMT_BIT(DISPLAY_FMT_XRGB8888),
        .max_param_bytes = SOFT_DBI_C_PARAM_MAX,
        .max_color_bytes = UINT32_MAX,
    };
    return 0;
}

/** @brief Bus op deinit: nothing to release, the pins stay as they are. */
static void soft_deinit(display_bus_t *base)
{
    to_soft_bus(base)->exclusive = false;
}

/** @brief Bus op write: command (D/C low or 9th bit 0), then data, read or pixels, bit by bit. */
static int soft_write(display_bus_t *base, display_io_t *io, const display_io_msg_t *msg)
{
    display_soft_dbi_c_bus_t *bus = to_soft_bus(base);
    bool c4 = io->cfg.iface == DISPLAY_IFACE_DBI_C4;
    const uint8_t *tx = msg->tx;
    uint8_t *rx = msg->rx;

    if (msg->cmd != DISPLAY_IO_NO_CMD) {
        if (c4) {
            display_pin_set(&bus->cfg.dc, false);
        }
        soft_xfer_byte(bus, io, false, (uint8_t)msg->cmd);
        if (c4) {
            display_pin_set(&bus->cfg.dc, true);
        }
    }
    if (msg->flags & DISPLAY_IO_MSG_READ) {
        for (uint32_t i = 0; i < msg->len; i++) {
            rx[i] = soft_xfer_byte(bus, io, true, 0xFF);
        }
    } else if (msg->flags & DISPLAY_IO_MSG_COLOR) {
        soft_send_pixels(bus, io, tx, msg->len);
    } else {
        for (uint32_t i = 0; i < msg->len; i++) {
            soft_xfer_byte(bus, io, true, tx[i]);
        }
    }
    return 0;
}

const display_bus_ops_t display_soft_dbi_c_bus_ops = {
    .init = soft_init,
    .deinit = soft_deinit,
    .attach = soft_attach,
    .write = soft_write,
};
