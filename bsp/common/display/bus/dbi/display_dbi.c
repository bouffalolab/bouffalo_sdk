/**
 * @file display_dbi.c
 * @brief DBI bus backend.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bus/dbi/display_dbi.h"
#include "bflb_dbi.h"
#include "bflb_dma.h"
#include "bflb_irq.h"
#include <stddef.h>

/* The core hands out &bus->base and this backend casts it back: base has to stay the first member */
_Static_assert(offsetof(display_dbi_bus_t, base) == 0, "display_bus_t must be the first member");

#if defined(BL616CL) || defined(BL618DG)
#define DBI_CLK_MAX_B8     54000000u
#define DBI_CLK_MAX_SERIAL 80000000u
#else
#define DBI_CLK_MAX_B8     27000000u
#define DBI_CLK_MAX_SERIAL 40000000u
#endif

/* Clock polarity and phase of the serial interfaces: DBI_CLOCK_MODE_0..3. */
#ifndef CONFIG_DISPLAY_DBI_CLK_MODE
#define CONFIG_DISPLAY_DBI_CLK_MODE 1
#endif

#define DBI_PARAM_MAX             255u         /* data length of bflb_dbi_send_cmd_data() is uint8_t */
#define DBI_READ_MAX              8u           /* DBI_READ_DATA_BYTE_MAX */
#define DBI_PIXEL_MAX             0x00FFFFFFu  /* 24-bit pixel counter */
#define DMA_LLI_BYTES             (4064u * 4u) /* one LLI node at 32-bit width */
#define QSPI_ADDR_BYTES           3u
#define QSPI_CODE_ADDR_MASK       0x00FFFFFFu /* the protocol code: address template in bits 23..0, opcode in 31..24 */
#define QSPI_CODE_OPCODE_SHIFT    24u
#define QSPI_ADDR_CMD_SHIFT       8u    /* the DCS command goes to bits 15..8 of the address */
#define DBI_DMA_BIG_FIFO_CHANNELS 6u    /* BL616CL: DMA channels 0..5 have a larger FIFO */
#define QSPI_LINES_NONE           0xFFu /* QSPI wire layout not known: program it before the next message */

/* TX FIFO threshold (entries) matching the DMA burst: INCR4 -> 3, INCR8 -> 7 */
#define DBI_FIFO_THRESHOLD_INCR4  3
#define DBI_FIFO_THRESHOLD_INCR8  7

#if defined(DBI_QSPI_SUPPORT) && DBI_QSPI_SUPPORT
#define DBI_HAS_QSPI 1
#else
#define DBI_HAS_QSPI 0
#endif

/** @brief Get the DBI bus from the core object (base is the first member). */
static display_dbi_bus_t *to_dbi_bus(display_bus_t *bus)
{
    return (display_dbi_bus_t *)bus;
}

/**
 * @brief Find an interface in the list of the bus.
 */
static const display_dbi_iface_t *dbi_find_iface(const display_dbi_bus_t *bus, display_iface_t iface)
{
    const display_dbi_iface_t *it = bus->cfg.ifaces;

    for (; it && it->iface != DISPLAY_IFACE_NONE; it++) {
        if (it->iface == iface) {
            return it;
        }
    }
    return NULL;
}

/**
 * @brief Switch the board pin functions from the active interface to another one;
 *        DISPLAY_IFACE_NONE only releases the active one.
 */
static void dbi_switch_pins(display_dbi_bus_t *bus, display_iface_t iface)
{
    const display_dbi_iface_t *old = dbi_find_iface(bus, (display_iface_t)bus->iface);
    const display_dbi_iface_t *next = dbi_find_iface(bus, iface);

    if (old && old->func_deinit) {
        old->func_deinit();
    }
    if (next && next->func_init) {
        next->func_init();
    }
    bus->iface = (uint8_t)iface;
}

/**
 * @brief Map a memory format to the DBI FIFO input format.
 */
static uint8_t dbi_input_format(display_format_t format)
{
    switch (format) {
        case DISPLAY_FMT_RGB888:
            return DBI_PIXEL_INPUT_FORMAT_RGB_888;
        case DISPLAY_FMT_XRGB8888:
            return DBI_PIXEL_INPUT_FORMAT_NRGB_8888;
#if defined(DBI_YUYV_SUPPORT) && DBI_YUYV_SUPPORT
        case DISPLAY_FMT_YUYV:
            return DBI_PIXEL_INPUT_FORMAT_YUYV;
#endif
        default:
            return DBI_PIXEL_INPUT_FORMAT_RGB_565;
    }
}

/**
 * @brief DBI interrupt: TC means the last pixel has left the pins.
 */
static void dbi_isr(int irq, void *arg)
{
    display_dbi_bus_t *bus = arg;

    (void)irq;
    if (!(bflb_dbi_get_intstatus(bus->dbi) & DBI_INTSTS_TC)) {
        return;
    }
    bflb_dbi_link_txdma(bus->dbi, false);
    bflb_dbi_int_clear(bus->dbi, DBI_INTCLR_TC);
    bflb_dbi_tcint_mask(bus->dbi, true);
    bflb_dma_channel_stop(bus->dma);
    display_bus_complete(&bus->base, 0);
}

/*****************************************************************************
 * Bus operations
 *****************************************************************************/

/** @brief Bus op init: look up the controller and DMA channel, install the TC interrupt. */
static int dbi_init(display_bus_t *base)
{
    display_dbi_bus_t *bus = to_dbi_bus(base);
    bool use_dma = bus->cfg.dma && bus->cfg.dma[0];

    /* A zero value must not silently change the meaning: these are configuration errors */
    if (!bus->cfg.ifaces || bus->cfg.ifaces->iface == DISPLAY_IFACE_NONE ||
        (use_dma && (!bus->cfg.lli || !bus->cfg.lli_count))) {
        return -EINVAL;
    }
    bus->dbi = bflb_device_get_by_name(bus->cfg.dev ? bus->cfg.dev : "dbi");
    if (!bus->dbi) {
        return -ENODEV;
    }
    bus->iface = DISPLAY_IFACE_NONE; /* pins are set up by the first apply */
    bus->dma = NULL;
    bus->fifo_threshold = DBI_FIFO_THRESHOLD_INCR4;
    if (use_dma) {
        struct bflb_dma_channel_config_s dma_cfg = {
            .direction = DMA_MEMORY_TO_PERIPH,
            .src_req = DMA_REQUEST_NONE,
            .dst_req = DMA_REQUEST_DBI_TX,
            .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
            .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
            .src_burst_count = DMA_BURST_INCR4,
            .dst_burst_count = DMA_BURST_INCR4,
            .src_width = DMA_DATA_WIDTH_32BIT,
            .dst_width = DMA_DATA_WIDTH_32BIT,
        };

        bus->dma = bflb_device_get_by_name(bus->cfg.dma);
        if (!bus->dma) {
            return -ENODEV;
        }
#if defined(BL616CL)
        if (bus->dma->sub_idx < DBI_DMA_BIG_FIFO_CHANNELS) { /* larger FIFO: INCR8 */
            bus->fifo_threshold = DBI_FIFO_THRESHOLD_INCR8;
            dma_cfg.src_burst_count = DMA_BURST_INCR8;
            dma_cfg.dst_burst_count = DMA_BURST_INCR8;
        }
#endif
        bflb_dma_channel_init(bus->dma, &dma_cfg);
    }

    bflb_dbi_tcint_mask(bus->dbi, true);
    bflb_irq_attach(bus->dbi->irq_num, dbi_isr, bus);
    bflb_irq_enable(bus->dbi->irq_num);
    return 0;
}

/** @brief Bus op deinit: stop the controller and DMA, release the board pins. */
static void dbi_deinit(display_bus_t *base)
{
    display_dbi_bus_t *bus = to_dbi_bus(base);

    bflb_irq_disable(bus->dbi->irq_num);
    bflb_dbi_tcint_mask(bus->dbi, true);
    if (bus->dma) {
        bflb_dma_channel_stop(bus->dma);
    }
    bflb_dbi_deinit(bus->dbi);
    dbi_switch_pins(bus, DISPLAY_IFACE_NONE);
    bus->exclusive = false;
}

/** @brief Bus op attach: check the interface and report the capabilities of the DBI. */
static int dbi_attach(display_bus_t *base, display_io_t *io)
{
    display_dbi_bus_t *bus = to_dbi_bus(base);
    const display_io_cfg_t *cfg = &io->cfg;
    bool qspi = cfg->iface == DISPLAY_IFACE_QSPI;
    uint32_t mask = DISPLAY_FMT_BIT(DISPLAY_FMT_RGB565) | DISPLAY_FMT_BIT(DISPLAY_FMT_RGB888) |
                    DISPLAY_FMT_BIT(DISPLAY_FMT_XRGB8888);
    int ret;

#if defined(DBI_YUYV_SUPPORT) && DBI_YUYV_SUPPORT
    mask |= DISPLAY_FMT_BIT(DISPLAY_FMT_YUYV);
#endif
    /* Only the interfaces this bus was set up for (with their board pin functions) */
    if (!dbi_find_iface(bus, cfg->iface) || (qspi && !DBI_HAS_QSPI)) {
        return -ENOTSUP;
    }
    ret = display_bus_claim(base, &bus->exclusive, cfg);
    if (ret) {
        return ret;
    }

    io->caps = (display_io_caps_t){
        .flags = (bus->dma ? DISPLAY_IO_CAP_DMA : 0) | (qspi ? 0 : DISPLAY_IO_CAP_READ),
        .format_mask = mask,
        .max_param_bytes = DBI_PARAM_MAX,
        .max_color_bytes = bus->dma ? bus->cfg.lli_count * DMA_LLI_BYTES : DBI_PIXEL_MAX,
        .color_align = 4, /* FIFO and DMA fetch 32-bit words */
    };
    return 0;
}

/** @brief Bus op apply: reprogram the controller (mode, clock, pixel formats) for an endpoint. */
static int dbi_apply(display_bus_t *base, display_io_t *io)
{
    display_dbi_bus_t *bus = to_dbi_bus(base);
    const display_io_cfg_t *cfg = &io->cfg;
    uint32_t limit = cfg->iface == DISPLAY_IFACE_DBI_B8 ? DBI_CLK_MAX_B8 : DBI_CLK_MAX_SERIAL;
    uint32_t freq = display_io_get_freq(io);
    struct bflb_dbi_config_s dbi_cfg = {
        .pixel_input_format = dbi_input_format(cfg->format),
        .pixel_output_format =
            io->wire == DISPLAY_WIRE_16 ? DBI_PIXEL_OUTPUT_FORMAT_RGB_565 : DBI_PIXEL_OUTPUT_FORMAT_RGB_888,
        .clk_mode = CONFIG_DISPLAY_DBI_CLK_MODE,
        .clk_freq_hz = freq < limit ? freq : limit,
        .tx_fifo_threshold = bus->fifo_threshold,
    };

    switch (cfg->iface) {
        case DISPLAY_IFACE_DBI_B8:
            dbi_cfg.dbi_mode = DBI_MODE_TYPE_B;
            break;
        case DISPLAY_IFACE_DBI_C4:
            dbi_cfg.dbi_mode = DBI_MODE_TYPE_C_4_WIRE;
            break;
        case DISPLAY_IFACE_DBI_C3:
            dbi_cfg.dbi_mode = DBI_MODE_TYPE_C_3_WIRE;
            break;
        default:
#if DBI_HAS_QSPI
            if (!io->link) {
                return -ENOTSUP; /* the QSPI protocol comes from the panel IC */
            }
            /* Wires are programmed per message by dbi_prepare_cmd() */
            dbi_cfg.dbi_mode = DBI_MODE_EX_QSPI;
            dbi_cfg.cmd_wire_mode = DBI_QSPI_WIRE_NUM_1;
            dbi_cfg.addr_wire_mode = DBI_QSPI_WIRE_NUM_1;
            dbi_cfg.data_wire_mode = DBI_QSPI_WIRE_NUM_1;
            break;
#else
            return -ENOTSUP;
#endif
    }

    if (bus->iface != cfg->iface) {
        dbi_switch_pins(bus, cfg->iface);
    }
    /* Until LHAL has per-field feature_control commands, reprogram the whole controller.
     * This only happens between transactions, with every software CS inactive. */
    bflb_dbi_init(bus->dbi, &dbi_cfg);
    bus->qspi_lines = QSPI_LINES_NONE;
    /* Continuous mode (CS stays active across the words of a pixel transfer). Without it the controller leaves a gap
     * between the words of a pixel transfer: measured 2554 us instead of 1474 us for 38400 bytes at 27 MHz (Type B).
     * It does not depend on how CS is driven (software GPIO or the controller pin). */
    bflb_dbi_feature_control(bus->dbi, DBI_CMD_CS_CONTINUE, true);
    bflb_dbi_tcint_mask(bus->dbi, true);
    return 0;
}

/**
 * @brief Command byte on the wire.
 *
 * QSPI: load the layout of the message kind (parameters or pixels) from the panel IC: wires of the
 * address and data phases, and the 24-bit address with the DCS command in bits 15..8; return the
 * 1-wire opcode.
 */
static uint8_t dbi_prepare_cmd(display_dbi_bus_t *bus, const display_io_t *io, const display_io_msg_t *msg)
{
#if DBI_HAS_QSPI
    if (io->cfg.iface == DISPLAY_IFACE_QSPI) {
        const display_qspi_proto_t *proto = &io->link->qspi; /* apply() rejected a QSPI endpoint without link */
        const display_qspi_layout_t *layout = (msg->flags & DISPLAY_IO_MSG_COLOR) ? &proto->color : &proto->cmd;

        if (bus->qspi_lines != (uint8_t)layout->lines) {
            bflb_dbi_qspi_set_wire(bus->dbi, DBI_QSPI_WIRE_NUM_1,
                                   layout->lines == DISPLAY_QSPI_LINES_4_4 ? DBI_QSPI_WIRE_NUM_4 : DBI_QSPI_WIRE_NUM_1,
                                   layout->lines == DISPLAY_QSPI_LINES_1_1 ? DBI_QSPI_WIRE_NUM_1 : DBI_QSPI_WIRE_NUM_4);
            bus->qspi_lines = (uint8_t)layout->lines;
        }
        bflb_dbi_qspi_set_addr(bus->dbi, QSPI_ADDR_BYTES,
                               (layout->code & QSPI_CODE_ADDR_MASK) | ((msg->cmd & 0xFFu) << QSPI_ADDR_CMD_SHIFT));
        return (uint8_t)(layout->code >> QSPI_CODE_OPCODE_SHIFT);
    }
#else
    (void)bus;
    (void)io;
#endif
    return (uint8_t)msg->cmd;
}

/** @brief Number of pixels of a color message. */
static uint32_t dbi_pixel_count(const display_io_t *io, const display_io_msg_t *msg)
{
    return msg->len / (uint32_t)display_format_bytes(io->cfg.format);
}

/** @brief Bus op write: send one message with the CPU (FIFO), command phase optional. */
static int dbi_write(display_bus_t *base, display_io_t *io, const display_io_msg_t *msg)
{
    display_dbi_bus_t *bus = to_dbi_bus(base);
    bool no_cmd = msg->cmd == DISPLAY_IO_NO_CMD;
    uint8_t cmd;
    int ret;

    if (no_cmd && io->cfg.iface == DISPLAY_IFACE_QSPI) {
        return -ENOTSUP;
    }
    if ((msg->flags & DISPLAY_IO_MSG_READ) && msg->len > DBI_READ_MAX) {
        return -E2BIG;
    }

    cmd = no_cmd ? 0 : dbi_prepare_cmd(bus, io, msg);
    if (no_cmd) {
        bflb_dbi_feature_control(bus->dbi, DBI_CMD_MASK_CMD_PHASE, true);
    }
    if (msg->flags & DISPLAY_IO_MSG_READ) {
        ret = bflb_dbi_send_cmd_read_data(bus->dbi, cmd, (uint8_t)msg->len, msg->rx);
    } else if (msg->flags & DISPLAY_IO_MSG_COLOR) {
        uint32_t count = dbi_pixel_count(io, msg);

        ret = bflb_dbi_send_cmd_pixel(bus->dbi, cmd, count, (void *)msg->tx);
    } else {
        ret = bflb_dbi_send_cmd_data(bus->dbi, cmd, (uint8_t)msg->len, (uint8_t *)msg->tx);
    }
    if (no_cmd) {
        bflb_dbi_feature_control(bus->dbi, DBI_CMD_MASK_CMD_PHASE, false);
    }
    return ret < 0 ? -EIO : 0;
}

/**
 * @brief Bus op write_dma: start the controller in pixel mode and let the DMA feed its FIFO.
 *        Completion is reported by dbi_isr().
 */
static int dbi_write_dma(display_bus_t *base, display_io_t *io, const display_io_msg_t *msg)
{
    display_dbi_bus_t *bus = to_dbi_bus(base);
    uint32_t count = dbi_pixel_count(io, msg);
    struct bflb_dma_channel_lli_transfer_s transfer;
    uint32_t size;

    if (msg->cmd == DISPLAY_IO_NO_CMD) {
        return -ENOTSUP;
    }
    /* Number of bytes the DMA has to feed (rounded up to 32-bit words) */
    size = (uint32_t)bflb_dbi_feature_control(bus->dbi, DBI_CMD_GET_SIZE_OF_PIXEL_CNT, count);
    if (size > bus->cfg.lli_count * DMA_LLI_BYTES) {
        return -E2BIG;
    }
    transfer.src_addr = (uint32_t)(uintptr_t)msg->tx;
    transfer.dst_addr = DMA_ADDR_DBI_TDR;
    transfer.nbytes = size;
    if (bflb_dma_channel_lli_reload(bus->dma, bus->cfg.lli, bus->cfg.lli_count, &transfer, 1) < 0) {
        return -E2BIG;
    }
    /* Order: start the pixel command, unmask TC, link the DMA request, then start the channel */
    bflb_dbi_send_cmd_pixel(bus->dbi, dbi_prepare_cmd(bus, io, msg), count, NULL); /* start only */
    bflb_dbi_tcint_mask(bus->dbi, false);
    bflb_dbi_link_txdma(bus->dbi, true);
    bflb_dma_channel_start(bus->dma);
    return 0;
}

const display_bus_ops_t display_dbi_bus_ops = {
    .init = dbi_init,
    .deinit = dbi_deinit,
    .attach = dbi_attach,
    .apply = dbi_apply,
    .write = dbi_write,
    .write_dma = dbi_write_dma,
};
