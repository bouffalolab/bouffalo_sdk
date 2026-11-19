/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#include <math.h>
#include <stdlib.h>
#include "bflb_audac.h"
#include "bflb_auadc.h"
#include "xcodec_builtin_codec.h"

typedef struct {
    xcodec_if_t base;
    xcodec_builtin_codec_cfg_t cfg;
    bool opened;
} builtin_codec_t;

static bool builtin_is_open(const xcodec_if_t *h)
{
    return h && ((const builtin_codec_t *)h)->opened;
}

static int builtin_volume(const xcodec_if_t *h, float db)
{
    const builtin_codec_t *codec = (const builtin_codec_t *)h;
    if (!codec || !isfinite(db) || db < -96.0f || db > 18.0f) {
        return XCODEC_DEV_INVALID_ARG;
    }
    if (!codec->opened) {
        return XCODEC_DEV_WRONG_STATE;
    }
    /* LHAL multiplies its integer dB argument by two for the register. */
    int gain_db = (int)(db + (db >= 0 ? 0.5f : -0.5f));
    if (gain_db < -95) {
        gain_db = -95;
    }
    return bflb_audac_feature_control(codec->cfg.audac, AUDAC_CMD_SET_VOLUME_VAL,
                                      (size_t)gain_db);
}

static int builtin_mute(const xcodec_if_t *h, bool mute)
{
    const builtin_codec_t *codec = (const builtin_codec_t *)h;
    if (!codec) {
        return XCODEC_DEV_INVALID_ARG;
    }
    if (!codec->opened) {
        return XCODEC_DEV_WRONG_STATE;
    }
    return bflb_audac_feature_control(codec->cfg.audac, AUDAC_CMD_SET_MUTE, mute);
}

static int builtin_gain(const xcodec_if_t *h, float db)
{
    const builtin_codec_t *codec = (const builtin_codec_t *)h;
    if (!codec || !isfinite(db) || db < 6.0f || db > 42.0f) {
        return XCODEC_DEV_INVALID_ARG;
    }
    if (!codec->opened) {
        return XCODEC_DEV_WRONG_STATE;
    }
#if AUADC_ANALOG_ADC_SUPPORT
    int gain = (int)(db / 3.0f + 0.5f) * 3;
    return bflb_auadc_feature_control(codec->cfg.auadc, AUADC_CMD_SET_PGA_GAIN_VAL, gain);
#else
    return XCODEC_DEV_NOT_SUPPORT;
#endif
}

static int builtin_close(const xcodec_if_t *h)
{
    if (!h) {
        return XCODEC_DEV_INVALID_ARG;
    }
    ((builtin_codec_t *)h)->opened = false;
    return XCODEC_DEV_OK;
}

const xcodec_if_t *xcodec_builtin_codec_new(const xcodec_builtin_codec_cfg_t *cfg)
{
    if (!cfg || (!cfg->audac && !cfg->auadc)) {
        return NULL;
    }
    builtin_codec_t *codec = calloc(1, sizeof(*codec));
    if (!codec) {
        return NULL;
    }
    codec->cfg = *cfg;
    codec->opened = true;
    codec->base.is_open = builtin_is_open;
    codec->base.close = builtin_close;
    if (cfg->audac) {
        codec->base.set_vol = builtin_volume;
        codec->base.mute = builtin_mute;
    }
#if AUADC_ANALOG_ADC_SUPPORT
    if (cfg->auadc && cfg->analog_input) {
        codec->base.set_mic_gain = builtin_gain;
    }
#else
    (void)builtin_gain;
#endif
    return &codec->base;
}
