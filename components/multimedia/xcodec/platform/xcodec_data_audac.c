/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#include "xcodec_dev_defaults.h"
#include "codec_data.h"
#include <errno.h>
#include <stdlib.h>

typedef struct {
    xcodec_data_if_t base;
    xcodec_audac_cfg_t cfg;
    codec_data_stream_t stream;
    uint32_t sample_rate;
    uint16_t mask;
    uint8_t slots;
    bool formatted;
    bool opened;
} audac_data_t;

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
    audac_data_t *data = (audac_data_t *)base;
    const xcodec_audac_cfg_t *cfg = config;
    if (!cfg || size != sizeof(*cfg)) {
        return XCODEC_DEV_INVALID_ARG;
    }
    if (data->opened) {
        return XCODEC_DEV_WRONG_STATE;
    }
    if (!cfg->audac || !cfg->dma_tx ||
        cfg->config.data_format != AUDAC_DATA_FORMAT_16BIT || cfg->config.fifo_threshold > 7 ||
        cfg->config.output_mode > AUDAC_OUTPUT_MODE_GPDAC_CH_A_B ||
        (cfg->config.source_channels_num != AUDAC_SOURCE_CHANNEL_SINGLE &&
         cfg->config.source_channels_num != AUDAC_SOURCE_CHANNEL_DUAL) ||
        cfg->config.mixer_mode > AUDAC_MIXER_MODE_AVERAGE) {
        return XCODEC_DEV_INVALID_ARG;
    }
    const xcodec_channel_cfg_t defaults = XCODEC_CHANNEL_DEFAULT_CONFIG();
    data->cfg = *cfg;
    data->cfg.channel.block_frames = cfg->channel.block_frames ? cfg->channel.block_frames : defaults.block_frames;
    data->cfg.channel.timeout_ms = cfg->channel.timeout_ms ? cfg->channel.timeout_ms : defaults.timeout_ms;
    data->slots = cfg->config.source_channels_num == AUDAC_SOURCE_CHANNEL_DUAL ? 2 : 1;
    data->opened = true;
    return XCODEC_DEV_OK;
}

static bool data_is_open(const xcodec_data_if_t *base)
{
    return ((const audac_data_t *)base)->opened;
}

static void stream_enable(codec_data_stream_t *s, bool enable)
{
    bflb_audac_link_rxdma(s->peripheral, enable);
    bflb_audac_feature_control(s->peripheral, enable ? AUDAC_CMD_PLAY_START : AUDAC_CMD_PLAY_STOP, 0);
}

static void stream_clear(codec_data_stream_t *s)
{
    bflb_audac_feature_control(s->peripheral, AUDAC_CMD_CLEAR_TX_FIFO, 0);
}

static int data_enable(const xcodec_data_if_t *base, xcodec_dev_type_t type, bool enable)
{
    audac_data_t *data = (audac_data_t *)base;
    if (!data->opened || type < XCODEC_DEV_TYPE_IN || type > XCODEC_DEV_TYPE_IN_OUT) {
        return XCODEC_DEV_INVALID_ARG;
    }
    if (type != XCODEC_DEV_TYPE_OUT || (enable && !data->formatted)) {
        return XCODEC_DEV_WRONG_STATE;
    }
    if (data->stream.running == enable) {
        return XCODEC_DEV_OK;
    }
    return driver_result(codec_data_enable(&data->stream, enable));
}

static int data_set_fmt(const xcodec_data_if_t *base, xcodec_dev_type_t type, xcodec_dev_sample_info_t *fs)
{
    audac_data_t *data = (audac_data_t *)base;
    if (!fs || !data->opened || type < XCODEC_DEV_TYPE_IN || type > XCODEC_DEV_TYPE_IN_OUT) {
        return XCODEC_DEV_INVALID_ARG;
    }
    if (fs->bits_per_sample != 16 || fs->channel != data->slots ||
        (fs->mclk_multiple && fs->mclk_multiple != 256) || (fs->channel_mask >> data->slots)) {
        return XCODEC_DEV_NOT_SUPPORT;
    }
    if (type != XCODEC_DEV_TYPE_OUT || data->stream.running) {
        return XCODEC_DEV_WRONG_STATE;
    }
    uint8_t rate;
    switch (fs->sample_rate) {
        case 8000: rate = AUDAC_SAMPLING_RATE_8K; break;
        case 16000: rate = AUDAC_SAMPLING_RATE_16K; break;
        case 24000: rate = AUDAC_SAMPLING_RATE_24K; break;
        case 32000: rate = AUDAC_SAMPLING_RATE_32K; break;
        case 48000: rate = AUDAC_SAMPLING_RATE_48K; break;
        default: return XCODEC_DEV_NOT_SUPPORT;
    }
    if (data->sample_rate != fs->sample_rate) {
        if (data->cfg.set_clock) {
            data->cfg.set_clock(fs->sample_rate);
        }
        struct bflb_audac_init_config_s config = data->cfg.config;
        config.sampling_rate = rate;
        data->sample_rate = 0;
        int ret = bflb_audac_init(data->cfg.audac, &config);
        if (!ret) {
            const struct bflb_audac_volume_config_s volume = {
                .volume_update_mode = AUDAC_VOLUME_UPDATE_MODE_FORCE,
            };
            ret = bflb_audac_volume_init(data->cfg.audac, &volume);
        }
        if (!ret) {
            ret = bflb_audac_feature_control(data->cfg.audac, AUDAC_CMD_SET_MUTE, true);
        }
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
    audac_data_t *data = (audac_data_t *)base;
    return driver_result(codec_data_transfer(&data->stream, buffer, size, data->mask, data->cfg.channel.timeout_ms));
}

static int data_close(const xcodec_data_if_t *base)
{
    audac_data_t *data = (audac_data_t *)base;
    if (data->stream.running) {
        (void)data_enable(base, XCODEC_DEV_TYPE_OUT, false);
    }
    codec_data_deinit(&data->stream);
    data->opened = false;
    data->formatted = false;
    return XCODEC_DEV_OK;
}

const xcodec_data_if_t *xcodec_audac_data_new(const xcodec_audac_cfg_t *cfg)
{
    audac_data_t *data = calloc(1, sizeof(*data));
    if (!data) {
        return NULL;
    }
    data->base = (xcodec_data_if_t){
        .open = data_open,
        .is_open = data_is_open,
        .enable = data_enable,
        .set_fmt = data_set_fmt,
        .write = data_transfer,
        .close = data_close,
    };
    int ret = data_open(&data->base, (void *)cfg, sizeof(*cfg));
    if (!ret) {
        const struct bflb_dma_channel_config_s dma_cfg = {
            .direction = DMA_MEMORY_TO_PERIPH,
            .src_req = DMA_REQUEST_NONE,
            .dst_req = DMA_REQUEST_AUDAC_TX,
            .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
            .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
            .src_burst_count = DMA_BURST_INCR4,
            .dst_burst_count = DMA_BURST_INCR4,
            .src_width = DMA_DATA_WIDTH_16BIT,
            .dst_width = DMA_DATA_WIDTH_16BIT,
        };
        data->stream.peripheral = cfg->audac;
        data->stream.hw_enable = stream_enable;
        data->stream.hw_clear = stream_clear;
        data->stream.copy = codec_data_copy_pcm16;
        ret = codec_data_init(&data->stream, cfg->dma_tx, &dma_cfg, DMA_ADDR_AUDAC_TDR,
                              data->cfg.channel.block_frames, data->cfg.channel.period_count, data->slots, 2);
    }
    if (ret) {
        codec_data_deinit(&data->stream);
        free(data);
        return NULL;
    }
    return &data->base;
}
