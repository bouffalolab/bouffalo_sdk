/**
 * @file display_spi.c
 * @brief Hardware SPI bus backend.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bus/spi/display_spi.h"
#include "bflb_spi.h"
#include "bflb_dma.h"
#include <stddef.h>

/* The core hands out &bus->base and this backend casts it back: base has to stay the first member */
_Static_assert(offsetof(display_spi_bus_t, base) == 0, "display_bus_t must be the first member");

#if !SPI_FIFO_WIDTH_VARIABLE_SUPPORT
#error "display_spi requires a SPI controller with variable FIFO width (BL616/BL616CL/BL618DG)"
#endif

#define SPI_FREQ_MAX       40000000u /* BL616 family: 80 MHz peripheral clock, divider 2 */
#define SPI_PARAM_MAX      4096u
#define DMA_LLI_ITEMS      4064u /* transfer units per LLI node */
#define DMA_WIDTH_UNKNOWN  0xFFu
#define SPI_FIFO_THRESHOLD (2 * 4 - 1) /* 2 bursts of INCR4, minus one */

/* SPI mode (clock polarity and phase) of the C4 interface, private to this backend */
#ifndef CONFIG_DISPLAY_SPI_MODE
#define CONFIG_DISPLAY_SPI_MODE 0
#endif

/** @brief Get the SPI bus from the core object (base is the first member). */
static display_spi_bus_t *to_spi_bus(display_bus_t *bus)
{
    return (display_spi_bus_t *)bus;
}

/** @brief Set the SPI frame width (8 bit for commands, 16/24 bit for pixels). */
static void spi_set_width(display_spi_bus_t *bus, uint32_t width)
{
    bflb_spi_feature_control(bus->spi, SPI_CMD_SET_DATA_WIDTH, width);
}

/**
 * @brief Program the DMA channel width when the pixel format changes.
 */
static void spi_dma_config(display_spi_bus_t *bus, uint8_t width)
{
    struct bflb_dma_channel_config_s dma_cfg = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_SPI0_TX,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR4,
        .dst_burst_count = DMA_BURST_INCR4,
        .src_width = width,
        .dst_width = width,
    };

    if (bus->dma_width == width) {
        return; /* already programmed */
    }
#if defined(DMA_REQUEST_SPI1_TX)
    if (bus->spi->idx == 1) {
        dma_cfg.dst_req = DMA_REQUEST_SPI1_TX;
    }
#endif
    bflb_dma_channel_init(bus->dma, &dma_cfg);
    /* channel_init() enables TC again; keep it masked until write_dma starts this transfer. */
    bflb_dma_channel_tcint_mask(bus->dma, true);
    bus->dma_width = width;
}

/**
 * @brief DMA interrupt. The FIFO still holds data when the DMA finishes: wait until the wire is idle.
 */
static void spi_dma_isr(void *arg)
{
    display_spi_bus_t *bus = arg;

    while (bflb_spi_isbusy(bus->spi)) {}
    bflb_spi_link_txdma(bus->spi, false);
    bflb_dma_channel_tcint_mask(bus->dma, true);
    bflb_spi_feature_control(bus->spi, SPI_CMD_CLEAR_RX_FIFO, 0);
    spi_set_width(bus, SPI_DATA_WIDTH_8BIT);
    display_bus_complete(&bus->base, 0);
}

/*****************************************************************************
 * Bus operations
 *****************************************************************************/

/** @brief Bus op init: configure the controller, DMA channel and board pins. */
static int spi_init(display_bus_t *base)
{
    display_spi_bus_t *bus = to_spi_bus(base);
    struct bflb_spi_config_s cfg = {
        .freq = DISPLAY_IFACE_DEFAULT_HZ(DISPLAY_IFACE_DBI_C4),
        .role = SPI_ROLE_MASTER,
        .mode = CONFIG_DISPLAY_SPI_MODE,
        .data_width = SPI_DATA_WIDTH_8BIT,
        .bit_order = SPI_BIT_MSB,
        .byte_order = SPI_BYTE_MSB, /* 16/24-bit frames high byte first: no RGB565 swapping */
        .tx_fifo_threshold = SPI_FIFO_THRESHOLD,
        .rx_fifo_threshold = SPI_FIFO_THRESHOLD,
    };
    bool use_dma = bus->cfg.dma && bus->cfg.dma[0];

    /* A zero value must not silently change the meaning: these are configuration errors */
    if (!display_pin_used(&bus->cfg.dc) || (use_dma && (!bus->cfg.lli || !bus->cfg.lli_count))) {
        return -EINVAL;
    }
    bus->spi = bflb_device_get_by_name(bus->cfg.dev ? bus->cfg.dev : "spi0");
    if (!bus->spi) {
        return -ENODEV;
    }
    bflb_spi_init(bus->spi, &cfg);
    display_pin_output(&bus->cfg.dc, true);

    bus->dma = NULL;
    bus->dma_width = DMA_WIDTH_UNKNOWN;
    if (use_dma) {
        bus->dma = bflb_device_get_by_name(bus->cfg.dma);
        if (!bus->dma) {
            return -ENODEV;
        }
        bus->tx_fifo_addr = DMA_ADDR_SPI0_TDR;
#if defined(DMA_ADDR_SPI1_TDR)
        if (bus->spi->idx == 1) {
            bus->tx_fifo_addr = DMA_ADDR_SPI1_TDR;
        }
#endif
    spi_dma_config(bus, DMA_DATA_WIDTH_32BIT);
        bflb_dma_channel_irq_attach(bus->dma, spi_dma_isr, bus);
    /* irq_attach() unmasks TC; no transfer is active during initialization. */
        bflb_dma_channel_tcint_mask(bus->dma, true);
    }
    if (bus->cfg.func_init) {
        bus->cfg.func_init();
    }
    return 0;
}

/** @brief Bus op deinit: stop the DMA, release the controller and the board pins. */
static void spi_deinit(display_bus_t *base)
{
    display_spi_bus_t *bus = to_spi_bus(base);

    if (bus->dma) {
        bflb_dma_channel_stop(bus->dma);
        bflb_dma_channel_irq_detach(bus->dma);
    }
    bflb_spi_deinit(bus->spi);
    if (bus->cfg.func_deinit) {
        bus->cfg.func_deinit();
    }
    bus->exclusive = false;
}

/** @brief Bus op attach: only the 4-wire interface, RGB565 and XRGB8888. */
static int spi_attach(display_bus_t *base, display_io_t *io)
{
    display_spi_bus_t *bus = to_spi_bus(base);
    const display_io_cfg_t *cfg = &io->cfg;
    int ret;

    if (cfg->iface != DISPLAY_IFACE_DBI_C4) {
        return -ENOTSUP;
    }
    ret = display_bus_claim(base, &bus->exclusive, cfg);
    if (ret) {
        return ret;
    }

    io->caps = (display_io_caps_t){
        /* RGB565 is sent as 16 bit; XRGB8888 as 3 bytes, i.e. 24 bit */
        .flags = DISPLAY_IO_CAP_FIXED_WIRE | (bus->dma ? DISPLAY_IO_CAP_DMA : 0),
        .format_mask = DISPLAY_FMT_BIT(DISPLAY_FMT_RGB565) | DISPLAY_FMT_BIT(DISPLAY_FMT_XRGB8888),
        .max_param_bytes = SPI_PARAM_MAX,
        /* Conservative: LLI nodes counted at the smallest unit (16 bit); 32-bit XRGB8888 units fit anyway */
        .max_color_bytes = bus->dma ? bus->cfg.lli_count * DMA_LLI_ITEMS * 2u : UINT32_MAX,
        .color_align = 4,
    };
    return 0;
}

/** @brief Bus op apply: restore clock, mode and byte order for an endpoint. */
static int spi_apply(display_bus_t *base, display_io_t *io)
{
    display_spi_bus_t *bus = to_spi_bus(base);
    uint32_t freq = display_io_get_freq(io);

    /* A borrower may have changed any of these: restore everything this backend depends on */
    bflb_spi_feature_control(bus->spi, SPI_CMD_SET_FREQ, freq < SPI_FREQ_MAX ? freq : SPI_FREQ_MAX);
    bflb_spi_feature_control(bus->spi, SPI_CMD_SET_MODE, CONFIG_DISPLAY_SPI_MODE);
    bflb_spi_feature_control(bus->spi, SPI_CMD_SET_BIT_ORDER, SPI_BIT_MSB);
    bflb_spi_feature_control(bus->spi, SPI_CMD_SET_BYTE_ORDER, SPI_BYTE_MSB);
    spi_set_width(bus, SPI_DATA_WIDTH_8BIT);
    return 0;
}

/**
 * @brief Command phase: DC at command level, the command byte as an 8-bit frame, then DC at data level.
 */
static void spi_send_cmd(display_spi_bus_t *bus, uint32_t cmd)
{
    if (cmd != DISPLAY_IO_NO_CMD) {
        display_pin_set(&bus->cfg.dc, false);
        bflb_spi_poll_send(bus->spi, cmd & 0xFF);
    }
    display_pin_set(&bus->cfg.dc, true);
}

/** @brief Bus op write: send one message by polling; pixels use a wider frame, 8 bit is restored after. */
static int spi_write(display_bus_t *base, display_io_t *io, const display_io_msg_t *msg)
{
    display_spi_bus_t *bus = to_spi_bus(base);

    if (msg->flags & DISPLAY_IO_MSG_READ) {
        return -ENOTSUP;
    }
    spi_send_cmd(bus, msg->cmd);
    if (!msg->len) {
        return 0;
    }
    if (!(msg->flags & DISPLAY_IO_MSG_COLOR)) {
        bflb_spi_poll_exchange(bus->spi, msg->tx, NULL, msg->len);
        return 0;
    }

    if (io->cfg.format == DISPLAY_FMT_RGB565) {
        spi_set_width(bus, SPI_DATA_WIDTH_16BIT);
        bflb_spi_poll_exchange(bus->spi, msg->tx, NULL, msg->len);
    } else { /* XRGB8888: one 24-bit frame per pixel */
        const uint32_t *px = msg->tx;

        spi_set_width(bus, SPI_DATA_WIDTH_24BIT);
        for (uint32_t i = 0; i < msg->len / 4u; i++) {
            bflb_spi_poll_send(bus->spi, px[i] & 0x00FFFFFFu);
        }
    }
    spi_set_width(bus, SPI_DATA_WIDTH_8BIT);
    return 0;
}

/** @brief Bus op write_dma: send the command, then let the DMA feed the TX FIFO. Completion: spi_dma_isr(). */
static int spi_write_dma(display_bus_t *base, display_io_t *io, const display_io_msg_t *msg)
{
    display_spi_bus_t *bus = to_spi_bus(base);
    bool rgb565 = io->cfg.format == DISPLAY_FMT_RGB565;
    struct bflb_dma_channel_lli_transfer_s transfer = {
        .src_addr = (uint32_t)(uintptr_t)msg->tx,
        .dst_addr = bus->tx_fifo_addr,
        .nbytes = msg->len,
    };

    spi_dma_config(bus, rgb565 ? DMA_DATA_WIDTH_16BIT : DMA_DATA_WIDTH_32BIT);
    if (bflb_dma_channel_lli_reload(bus->dma, bus->cfg.lli, bus->cfg.lli_count, &transfer, 1) < 0) {
        return -E2BIG;
    }
    /* The command goes out by polling before the DMA takes over the FIFO */
    spi_send_cmd(bus, msg->cmd);
    spi_set_width(bus, rgb565 ? SPI_DATA_WIDTH_16BIT : SPI_DATA_WIDTH_24BIT);
    bflb_spi_link_txdma(bus->spi, true);
    bflb_dma_channel_start(bus->dma);
    bflb_dma_channel_tcint_mask(bus->dma, false);
    return 0;
}

const display_bus_ops_t display_spi_bus_ops = {
    .init = spi_init,
    .deinit = spi_deinit,
    .attach = spi_attach,
    .apply = spi_apply,
    .write = spi_write,
    .write_dma = spi_write_dma,
};
