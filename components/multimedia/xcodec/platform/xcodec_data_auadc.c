/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#include "xcodec_dev_defaults.h"
#include "codec_data.h"
#include <errno.h>
#include <stdlib.h>

typedef struct {
    xcodec_data_if_t base;
    xcodec_auadc_cfg_t cfg;
    codec_data_stream_t stream;
    uint32_t sample_rate;
    uint16_t mask;
    uint8_t slots;
    bool formatted;
    bool opened;
} auadc_data_t;

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

static int data_open(const xcodec_data_if_t *base, void *config, int size)
{
    auadc_data_t *data = (auadc_data_t *)base;
    const xcodec_auadc_cfg_t *cfg = config;
    if (!cfg || size != sizeof(*cfg)) {
        return XCODEC_DEV_INVALID_ARG;
    }
    if (data->opened) {
        return XCODEC_DEV_WRONG_STATE;
    }
    if (!cfg->auadc || !cfg->dma_rx ||
        cfg->config.data_format != AUADC_DATA_FORMAT_16BIT || cfg->config.fifo_threshold > 7 ||
        cfg->config.input_mode > AUADC_INPUT_MODE_PDM_R) {
        return XCODEC_DEV_INVALID_ARG;
    }

#if AUADC_ANALOG_ADC_SUPPORT
    if (cfg->config.input_mode == AUADC_INPUT_MODE_ADC &&
        (!cfg->analog_config.auadc_analog_en || cfg->analog_config.adc_mode != AUADC_ADC_MODE_AUDIO ||
         cfg->analog_config.adc_pga_mode > AUADC_ADC_PGA_MODE_DC_SINGLE ||
         cfg->analog_config.adc_pga_posi_ch > 7 || cfg->analog_config.adc_pga_nega_ch > 7 ||
         cfg->analog_config.adc_pga_gain < 6 || cfg->analog_config.adc_pga_gain > 42 ||
         cfg->analog_config.adc_pga_gain % 3)) {
        return XCODEC_DEV_INVALID_ARG;
    }
#else
    if (cfg->config.input_mode == 0) {
        return XCODEC_DEV_INVALID_ARG;
    }
#endif
    const xcodec_channel_cfg_t defaults = XCODEC_CHANNEL_DEFAULT_CONFIG();
    data->cfg = *cfg;
    data->cfg.channel.block_frames = cfg->channel.block_frames ? cfg->channel.block_frames : defaults.block_frames;
    data->cfg.channel.timeout_ms = cfg->channel.timeout_ms ? cfg->channel.timeout_ms : defaults.timeout_ms;
    data->slots = 1;
    data->opened = true;
    return XCODEC_DEV_OK;
}

static bool data_is_open(const xcodec_data_if_t *base)
{
    return ((const auadc_data_t *)base)->opened;
}

static void stream_enable(codec_data_stream_t *s, bool enable)
{
    bflb_auadc_link_rxdma(s->peripheral, enable);
    bflb_auadc_feature_control(s->peripheral, enable ? AUADC_CMD_RECORD_START : AUADC_CMD_RECORD_STOP, 0);
}

static void stream_clear(codec_data_stream_t *s)
{
    bflb_auadc_feature_control(s->peripheral, AUADC_CMD_CLEAR_RX_FIFO, 0);
}

static int data_enable(const xcodec_data_if_t *base, xcodec_dev_type_t type, bool enable)
{
    auadc_data_t *data = (auadc_data_t *)base;
    if (!data->opened || type < XCODEC_DEV_TYPE_IN || type > XCODEC_DEV_TYPE_IN_OUT) {
        return XCODEC_DEV_INVALID_ARG;
    }
    if (type != XCODEC_DEV_TYPE_IN || (enable && !data->formatted)) {
        return XCODEC_DEV_WRONG_STATE;
    }
    if (data->stream.running == enable) {
        return XCODEC_DEV_OK;
    }
    return driver_result(codec_data_enable(&data->stream, enable));
}

static int data_set_fmt(const xcodec_data_if_t *base, xcodec_dev_type_t type, xcodec_dev_sample_info_t *fs)
{
    auadc_data_t *data = (auadc_data_t *)base;
    if (!fs || !data->opened || type < XCODEC_DEV_TYPE_IN || type > XCODEC_DEV_TYPE_IN_OUT) {
        return XCODEC_DEV_INVALID_ARG;
    }
    if (fs->bits_per_sample != 16 || fs->channel != data->slots ||
        (fs->mclk_multiple && fs->mclk_multiple != 256) || (fs->channel_mask >> data->slots)) {
        return XCODEC_DEV_NOT_SUPPORT;
    }
    if (type != XCODEC_DEV_TYPE_IN || data->stream.running) {
        return XCODEC_DEV_WRONG_STATE;
    }
    uint8_t rate;
    switch (fs->sample_rate) {
        case 8000: rate = AUADC_SAMPLING_RATE_8K; break;
        case 16000: rate = AUADC_SAMPLING_RATE_16K; break;
        case 24000: rate = AUADC_SAMPLING_RATE_24K; break;
        case 32000: rate = AUADC_SAMPLING_RATE_32K; break;
        case 48000: rate = AUADC_SAMPLING_RATE_48K; break;
        default: return XCODEC_DEV_NOT_SUPPORT;
    }
    if (data->sample_rate != fs->sample_rate) {
        if (data->cfg.set_clock) {
            data->cfg.set_clock(fs->sample_rate);
        }
        struct bflb_auadc_init_config_s config = data->cfg.config;
        config.sampling_rate = rate;
        data->sample_rate = 0;
        int ret = bflb_auadc_init(data->cfg.auadc, &config);
#if AUADC_ANALOG_ADC_SUPPORT
        if (!ret && config.input_mode == AUADC_INPUT_MODE_ADC) {
            ret = bflb_auadc_adc_init(data->cfg.auadc, &data->cfg.analog_config);
        }
#endif
        if (ret) {
            data->formatted = false;
            return driver_result(ret);
        }
        data->cfg.config = config;
        data->sample_rate = fs->sample_rate;
    }
    data->mask = fs->channel_mask ? fs->channel_mask : (1U << data->slots) - 1U;
    data->formatted = true;
    return XCODEC_DEV_OK;
}

static int data_transfer(const xcodec_data_if_t *base, uint8_t *buffer, int size)
{
    auadc_data_t *data = (auadc_data_t *)base;
    return driver_result(codec_data_transfer(&data->stream, buffer, size, data->mask, data->cfg.channel.timeout_ms));
}

static int data_close(const xcodec_data_if_t *base)
{
    auadc_data_t *data = (auadc_data_t *)base;
    if (data->stream.running) {
        (void)data_enable(base, XCODEC_DEV_TYPE_IN, false);
    }
    codec_data_deinit(&data->stream);
    data->opened = false;
    data->formatted = false;
    return XCODEC_DEV_OK;
}

const xcodec_data_if_t *xcodec_auadc_data_new(const xcodec_auadc_cfg_t *cfg)
{
    auadc_data_t *data = calloc(1, sizeof(*data));
    if (!data) {
        return NULL;
    }
    data->base = (xcodec_data_if_t){
        .open = data_open,
        .is_open = data_is_open,
        .enable = data_enable,
        .set_fmt = data_set_fmt,
        .read = data_transfer,
        .close = data_close,
    };
    int ret = data_open(&data->base, (void *)cfg, sizeof(*cfg));
    if (!ret) {
        const struct bflb_dma_channel_config_s dma_cfg = {
            .direction = DMA_PERIPH_TO_MEMORY,
            .src_req = DMA_REQUEST_AUADC_RX,
            .dst_req = DMA_REQUEST_NONE,
            .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
            .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
            .src_burst_count = DMA_BURST_INCR4,
            .dst_burst_count = DMA_BURST_INCR4,
            .src_width = DMA_DATA_WIDTH_16BIT,
            .dst_width = DMA_DATA_WIDTH_16BIT,
        };
        data->stream.peripheral = cfg->auadc;
        data->stream.hw_enable = stream_enable;
        data->stream.hw_clear = stream_clear;
        ret = codec_data_init(&data->stream, cfg->dma_rx, &dma_cfg, DMA_ADDR_AUADC_RDR,
                              data->cfg.channel.block_frames, data->cfg.channel.period_count, data->slots, 2);
    }
    if (ret) {
        codec_data_deinit(&data->stream);
        free(data);
        return NULL;
    }
    return &data->base;
}
