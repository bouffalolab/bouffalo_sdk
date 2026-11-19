/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#include "xcodec_dev_defaults.h"
#include "codec_data.h"
#include "bflb_clock.h"
#include "bflb_reset.h"
#include "bflb_peri.h"
#include "hardware/i2s_reg.h"
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

#if defined(DMA_ADDR_I2S_TDR)
typedef struct {
    xcodec_data_if_t base;
    xcodec_i2s_cfg_t cfg;
    codec_data_stream_t rx;
    codec_data_stream_t tx;
    uint16_t masks[2];
    uint32_t sample_rate;
    bool formatted[2];
    bool enabled[2];
    bool starting;
    bool initialized;
    bool opened;
} i2s_data_t;

static int driver_result(int ret)
{
    switch (ret) {
        case -EINVAL: return XCODEC_DEV_INVALID_ARG;
        case -ENOMEM: return XCODEC_DEV_NO_MEM;
        case -ENOTSUP: return XCODEC_DEV_NOT_SUPPORT;
        case -EBUSY: return XCODEC_DEV_WRONG_STATE;
        default: return ret;
    }
}

static int i2s_data_open(const xcodec_data_if_t *base, void *cfg, int size)
{
    i2s_data_t *data = (i2s_data_t *)base;
    const xcodec_i2s_cfg_t *config = cfg;
    if (!config || size != sizeof(*config) || !config->i2s ||
        (!config->dma_rx && !config->dma_tx) ||
        (config->dma_rx && config->dma_rx == config->dma_tx) ||
        !IS_I2S_ROLE(config->config.role) || !IS_I2S_FORMAT_MODE(config->config.format_mode) ||
        !IS_I2S_CHANNEL_MODE(config->config.channel_mode) ||
        !IS_I2S_SLOT_WIDTH(config->config.frame_width) ||
        config->config.data_width != I2S_SLOT_WIDTH_16 ||
        config->config.frame_width < config->config.data_width ||
        (config->slots > 2 &&
         (config->config.frame_width != config->config.data_width ||
          config->config.format_mode < I2S_MODE_DSP_SHORT_FRAME_SYNC)) ||
        config->config.tx_fifo_threshold > 15 || config->config.rx_fifo_threshold > 15 ||
        config->slots != (config->config.channel_mode < 4 ? config->config.channel_mode + 1 : 6)) {
        return XCODEC_DEV_INVALID_ARG;
    }
    if (data->opened) {
        return XCODEC_DEV_WRONG_STATE;
    }
    const xcodec_channel_cfg_t defaults = XCODEC_CHANNEL_DEFAULT_CONFIG();
    data->cfg = *config;
    data->cfg.channel.block_frames = config->channel.block_frames ? config->channel.block_frames : defaults.block_frames;
    data->cfg.channel.timeout_ms = config->channel.timeout_ms ? config->channel.timeout_ms : defaults.timeout_ms;
    data->opened = true;
    return XCODEC_DEV_OK;
}

static bool i2s_data_is_open(const xcodec_data_if_t *base)
{
    return ((const i2s_data_t *)base)->opened;
}

static void i2s_stream_enable(codec_data_stream_t *s, bool enable)
{
    i2s_data_t *data = (void *)((uint8_t *)s - (s->input ? offsetof(i2s_data_t, rx) : offsetof(i2s_data_t, tx)));
    uintptr_t irq_state = bflb_irq_save();
    uint32_t control = getreg32(s->peripheral->reg_base + I2S_CONFIG_OFFSET);
    unsigned direction = s->input ? I2S_ARG_CMD_RX : I2S_ARG_CMD_TX;
    unsigned role = data->cfg.config.role == I2S_ROLE_MASTER ? I2S_ARG_CMD_MASTER : I2S_ARG_CMD_SLAVE;
    if (enable && !data->starting) {
        direction |= role;
    } else if (!enable && !(control & (s->input ? I2S_CR_I2S_TXD_EN : I2S_CR_I2S_RXD_EN))) {
        direction |= role;
    }
    if (s->input) {
        bflb_i2s_link_rxdma(s->peripheral, enable);
    } else {
        bflb_i2s_link_txdma(s->peripheral, enable);
    }
    bflb_i2s_feature_control(s->peripheral, enable ? I2S_CMD_ENABLE_CONTROL : I2S_CMD_DISABLE_CONTROL,
                             direction);
    bflb_irq_restore(irq_state);
}

static void i2s_stream_clear(codec_data_stream_t *s)
{
    uintptr_t irq_state = bflb_irq_save();
    bflb_i2s_feature_control(s->peripheral, s->input ? I2S_CMD_CLEAR_RX_FIFO : I2S_CMD_CLEAR_TX_FIFO, 0);
    bflb_irq_restore(irq_state);
}

static int i2s_stream_init(codec_data_stream_t *s, const xcodec_i2s_cfg_t *cfg, bool input)
{
    unsigned threshold = input ? cfg->config.rx_fifo_threshold : cfg->config.tx_fifo_threshold;
    /* A burst must fit the minimum FIFO space/data guaranteed by its request. */
    unsigned burst = threshold >= 3 ? DMA_BURST_INCR4 : DMA_BURST_INCR1;
    const struct bflb_dma_channel_config_s dma_cfg = {
        .direction = input ? DMA_PERIPH_TO_MEMORY : DMA_MEMORY_TO_PERIPH,
        .src_req = input ? DMA_REQUEST_I2S_RX : DMA_REQUEST_NONE,
        .dst_req = input ? DMA_REQUEST_NONE : DMA_REQUEST_I2S_TX,
        .src_addr_inc = input ? DMA_ADDR_INCREMENT_DISABLE : DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = input ? DMA_ADDR_INCREMENT_ENABLE : DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = burst,
        .dst_burst_count = burst,
        .src_width = DMA_DATA_WIDTH_32BIT,
        .dst_width = DMA_DATA_WIDTH_32BIT,
    };
    s->peripheral = cfg->i2s;
    s->hw_enable = i2s_stream_enable;
    s->hw_clear = i2s_stream_clear;
    s->copy = codec_data_copy_pcm16;
    return codec_data_init(s, input ? cfg->dma_rx : cfg->dma_tx, &dma_cfg,
                           input ? DMA_ADDR_I2S_RDR : DMA_ADDR_I2S_TDR,
                           cfg->channel.block_frames, cfg->channel.period_count, cfg->slots, cfg->lr_merge ? 2 : 4);
}

static int i2s_data_enable(const xcodec_data_if_t *base, xcodec_dev_type_t type, bool enable)
{
    i2s_data_t *data = (i2s_data_t *)base;
    if (!data->opened || type < XCODEC_DEV_TYPE_IN || type > XCODEC_DEV_TYPE_IN_OUT) {
        return XCODEC_DEV_INVALID_ARG;
    }
    codec_data_stream_t *streams[] = { &data->rx, &data->tx };
    for (unsigned i = 0; i < 2; ++i) {
        if ((type & (1U << i)) && (!streams[i]->allocation || (enable && !data->formatted[i]))) {
            return XCODEC_DEV_WRONG_STATE;
        }
    }
    bool was_running = data->rx.running || data->tx.running;
    bool enabled[2] = { data->enabled[0], data->enabled[1] };
    for (unsigned i = 0; i < 2; ++i) {
        if (type & (1U << i)) {
            enabled[i] = enable;
        }
    }
    bool running = enabled[0] || enabled[1];
    unsigned role = data->cfg.config.role == I2S_ROLE_MASTER ? I2S_ARG_CMD_MASTER : I2S_ARG_CMD_SLAVE;
    if (running && !was_running) {
        /* Arm every DMA/FIFO before starting the shared clock. */
        uintptr_t irq_state = bflb_irq_save();
        bflb_i2s_feature_control(data->cfg.i2s, I2S_CMD_DISABLE_CONTROL, role);
        bflb_irq_restore(irq_state);
        data->starting = true;
        for (unsigned i = 0; i < 2; ++i) {
            if (streams[i]->allocation) {
                int ret = codec_data_enable(streams[i], true);
                if (ret) {
                    for (unsigned j = 0; j < 2; ++j) {
                        if (streams[j]->allocation) {
                            (void)codec_data_enable(streams[j], false);
                        }
                    }
                    data->starting = false;
                    return driver_result(ret);
                }
                codec_data_set_enabled(streams[i], enabled[i]);
            }
        }
        data->starting = false;
        irq_state = bflb_irq_save();
        bflb_i2s_feature_control(data->cfg.i2s, I2S_CMD_ENABLE_CONTROL, role);
        bflb_irq_restore(irq_state);
    } else {
        for (unsigned i = 0; i < 2; ++i) {
            if (!streams[i]->allocation) {
                continue;
            }
            if (!running) {
                int ret = codec_data_enable(streams[i], false);
                if (ret) {
                    return driver_result(ret);
                }
            } else if (enabled[i] != data->enabled[i]) {
                codec_data_set_enabled(streams[i], enabled[i]);
            }
        }
    }
    data->enabled[0] = enabled[0];
    data->enabled[1] = enabled[1];
    return XCODEC_DEV_OK;
}

static int i2s_data_set_fmt(const xcodec_data_if_t *base, xcodec_dev_type_t type, xcodec_dev_sample_info_t *fs)
{
    i2s_data_t *data = (i2s_data_t *)base;
    if (!fs || !data->opened || type < XCODEC_DEV_TYPE_IN || type > XCODEC_DEV_TYPE_IN_OUT) {
        return XCODEC_DEV_INVALID_ARG;
    }
    if (!fs->sample_rate || fs->bits_per_sample != 16 || fs->channel != data->cfg.slots ||
        fs->mclk_multiple < 0 ||
        (fs->channel_mask >> data->cfg.slots)) {
        return XCODEC_DEV_NOT_SUPPORT;
    }
    codec_data_stream_t *streams[] = { &data->rx, &data->tx };
    for (unsigned i = 0; i < 2; ++i) {
        if ((type & (1U << i)) && (!streams[i]->allocation || data->enabled[i])) {
            return XCODEC_DEV_WRONG_STATE;
        }
    }
    if ((data->rx.running || data->tx.running) && fs->sample_rate != data->sample_rate) {
        return XCODEC_DEV_NOT_SUPPORT;
    }
    unsigned slot_width = 8U * (data->cfg.config.frame_width + 1U);
    unsigned frame_slots = data->cfg.slots == 1 ? 2 : data->cfg.slots;
    bool change_clock = !data->rx.running && !data->tx.running &&
                        fs->sample_rate != data->sample_rate && data->cfg.set_clock;
    if (change_clock) {
        data->cfg.set_clock(fs->sample_rate);
    }
    uint32_t clock = bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_I2S, data->cfg.i2s->idx);
    uint64_t bclk = (uint64_t)fs->sample_rate * slot_width * frame_slots;
    uint64_t divider = ((uint64_t)clock + bclk / 2U) / bclk;
    uint64_t expected = divider * bclk;
    uint64_t error = clock > expected ? clock - expected : expected - clock;
    bool master = data->cfg.config.role == I2S_ROLE_MASTER;
    /* Allow 100 ppm for fractional PLL quantization, while rejecting mismatched rates. */
    bool supported = !master || (divider >= 2 && divider <= 0xfff + 2U && error <= expected / 10000U);
    if (master && fs->mclk_multiple) {
        expected = (uint64_t)fs->sample_rate * fs->mclk_multiple;
        error = clock > expected ? clock - expected : expected - clock;
        supported = supported && error <= expected / 10000U;
    }
    if (!supported) {
        if (change_clock && data->sample_rate) {
            data->cfg.set_clock(data->sample_rate);
        }
        return XCODEC_DEV_NOT_SUPPORT;
    }
    if (!data->initialized) {
        struct bflb_i2s_config_s config = data->cfg.config;
        /* LHAL computes an unused divider even when external clocks drive slave mode. */
        config.bclk_freq_hz = master ? bclk : (clock >= 2 ? clock / 2U : 1U);
        /* Reset the TDM frame counter before the first shared-clock start. */
        int ret = bflb_peripheral_reset(BFLB_PERIPHERAL_I2S0);
        if (ret) {
            return driver_result(ret);
        }
        bflb_i2s_init(data->cfg.i2s, &config);
        uintptr_t irq_state = bflb_irq_save();
        bflb_i2s_feature_control(data->cfg.i2s, I2S_CMD_CHANNEL_LR_MERGE, data->cfg.lr_merge);
        bflb_irq_restore(irq_state);
        data->initialized = true;
    }
    if (master && !data->rx.running && !data->tx.running) {
        /* Change only the divider: reinitializing I2S would clear both FIFOs
         * and the board's framing, polarity and packing configuration. */
        uint32_t base_addr = data->cfg.i2s->reg_base;
        uint32_t div = divider - 2U;
        uint32_t timing = getreg32(base_addr + I2S_BCLK_CONFIG_OFFSET);
        uint32_t value = (timing & ~(I2S_CR_BCLK_DIV_L_MASK | I2S_CR_BCLK_DIV_H_MASK)) |
                         ((div / 2U) << I2S_CR_BCLK_DIV_L_SHIFT) |
                         ((div - div / 2U) << I2S_CR_BCLK_DIV_H_SHIFT);
        if (value != timing) {
            uint32_t config = getreg32(base_addr + I2S_CONFIG_OFFSET);
            putreg32(config & ~I2S_CR_I2S_M_EN, base_addr + I2S_CONFIG_OFFSET);
            putreg32(value, base_addr + I2S_BCLK_CONFIG_OFFSET);
            putreg32(config, base_addr + I2S_CONFIG_OFFSET);
        }
    }
    data->sample_rate = fs->sample_rate;
    for (unsigned i = 0; i < 2; ++i) {
        if (type & (1U << i)) {
            data->masks[i] = fs->channel_mask ? fs->channel_mask : (1U << data->cfg.slots) - 1U;
            data->formatted[i] = true;
        }
    }
    return XCODEC_DEV_OK;
}

static int i2s_data_read(const xcodec_data_if_t *base, uint8_t *buffer, int size)
{
    i2s_data_t *data = (i2s_data_t *)base;
    return driver_result(codec_data_transfer(&data->rx, buffer, size, data->masks[0], data->cfg.channel.timeout_ms));
}

static int i2s_data_write(const xcodec_data_if_t *base, uint8_t *buffer, int size)
{
    i2s_data_t *data = (i2s_data_t *)base;
    return driver_result(codec_data_transfer(&data->tx, buffer, size, data->masks[1], data->cfg.channel.timeout_ms));
}

static int i2s_data_close(const xcodec_data_if_t *base)
{
    i2s_data_t *data = (i2s_data_t *)base;
    if (!data->opened) {
        return XCODEC_DEV_OK;
    }
    if (data->cfg.dma_rx) {
        int ret = i2s_data_enable(base, XCODEC_DEV_TYPE_IN, false);
        if (ret) {
            return ret;
        }
    }
    if (data->cfg.dma_tx) {
        int ret = i2s_data_enable(base, XCODEC_DEV_TYPE_OUT, false);
        if (ret) {
            return ret;
        }
    }
    codec_data_deinit(&data->rx);
    codec_data_deinit(&data->tx);
    if (data->initialized) {
        bflb_i2s_deinit(data->cfg.i2s);
        data->initialized = false;
    }
    data->opened = false;
    data->formatted[0] = data->formatted[1] = false;
    return XCODEC_DEV_OK;
}

const xcodec_data_if_t *xcodec_i2s_data_new(const xcodec_i2s_cfg_t *cfg)
{
    i2s_data_t *data = calloc(1, sizeof(*data));
    if (!data) {
        return NULL;
    }
    data->base = (xcodec_data_if_t){
        .open = i2s_data_open,
        .is_open = i2s_data_is_open,
        .enable = i2s_data_enable,
        .set_fmt = i2s_data_set_fmt,
        .read = i2s_data_read,
        .write = i2s_data_write,
        .close = i2s_data_close,
    };
    int ret = i2s_data_open(&data->base, (void *)cfg, sizeof(*cfg));
    if (!ret && cfg->dma_rx) {
        ret = i2s_stream_init(&data->rx, &data->cfg, true);
    }
    if (!ret && cfg->dma_tx) {
        ret = i2s_stream_init(&data->tx, &data->cfg, false);
    }
    if (ret) {
        codec_data_deinit(&data->rx);
        codec_data_deinit(&data->tx);
        free(data);
        return NULL;
    }
    return &data->base;
}
#else
const xcodec_data_if_t *xcodec_i2s_data_new(const xcodec_i2s_cfg_t *cfg)
{
    (void)cfg;
    return NULL;
}
#endif
