#include "audio_codec_bl.h"

#include <stddef.h>
#include <string.h>

#include "audio_codec_driver.h"
#include "bflb_audac.h"
#include "bflb_auadc.h"
#include "bflb_clock.h"
#include "board.h"

#ifndef CONFIG_BSP_AUDIO_CODEC_BL_AUDAC_OUTPUT_MODE
#define AUDIO_CODEC_BL_AUDAC_OUTPUT_MODE AUDAC_OUTPUT_MODE_PWM
#else
#define AUDIO_CODEC_BL_AUDAC_OUTPUT_MODE CONFIG_BSP_AUDIO_CODEC_BL_AUDAC_OUTPUT_MODE
#endif

#ifndef CONFIG_BSP_AUDIO_CODEC_BL_AUADC_INPUT_MODE
#if AUADC_ANALOG_ADC_SUPPORT
#define AUDIO_CODEC_BL_AUADC_INPUT_MODE AUADC_INPUT_MODE_ADC
#else
#define AUDIO_CODEC_BL_AUADC_INPUT_MODE AUADC_INPUT_MODE_PDM_L
#endif
#else
#define AUDIO_CODEC_BL_AUADC_INPUT_MODE CONFIG_BSP_AUDIO_CODEC_BL_AUADC_INPUT_MODE
#endif

#ifndef CONFIG_BSP_AUDIO_CODEC_BL_AUADC_POS_CHANNEL
#define AUDIO_CODEC_BL_AUADC_POS_CHANNEL 4U
#else
#define AUDIO_CODEC_BL_AUADC_POS_CHANNEL CONFIG_BSP_AUDIO_CODEC_BL_AUADC_POS_CHANNEL
#endif

#ifndef CONFIG_BSP_AUDIO_CODEC_BL_AUADC_NEG_CHANNEL
#define AUDIO_CODEC_BL_AUADC_NEG_CHANNEL 7U
#else
#define AUDIO_CODEC_BL_AUADC_NEG_CHANNEL CONFIG_BSP_AUDIO_CODEC_BL_AUADC_NEG_CHANNEL
#endif

typedef struct {
    const char *chip_name;
    bool playback_supported;
    bool capture_supported;
    bool analog_capture_supported;
    bool pdm_capture_supported;
    uint8_t max_playback_channels;
    uint8_t max_capture_channels;
} audio_codec_bl_caps_t;

typedef struct {
    bool used;
    struct bflb_device_s *audac_dev;
    struct bflb_device_s *auadc_dev;
    audio_codec_cfg_t cfg;
    const audio_codec_bl_caps_t *caps;
    int volume_percent;
    int record_volume_percent;
    int mic_pga_percent;
    bool mute;
} audio_codec_bl_state_t;

static const audio_codec_bl_caps_t g_audio_codec_bl_caps =
#if defined(BL616)
    {
        .chip_name = "bl616",
        .playback_supported = true,
        .capture_supported = true,
        .analog_capture_supported = true,
        .pdm_capture_supported = true,
        .max_playback_channels = 1,
        .max_capture_channels = 1,
    };
#elif defined(BL618DG)
    {
        .chip_name = "bl618dg",
        .playback_supported = true,
        .capture_supported = true,
        .analog_capture_supported = true,
        .pdm_capture_supported = true,
        .max_playback_channels = 1,
        .max_capture_channels = 1,
    };
#elif defined(BL616CL)
    {
        .chip_name = "bl616cl",
        .playback_supported = true,
        .capture_supported = true,
        .analog_capture_supported = false,
        .pdm_capture_supported = true,
        .max_playback_channels = 1,
        .max_capture_channels = 1,
    };
#elif defined(BL702L)
    {
        .chip_name = "bl702l",
        .playback_supported = false,
        .capture_supported = true,
        .analog_capture_supported = true,
        .pdm_capture_supported = true,
        .max_playback_channels = 0,
        .max_capture_channels = 1,
    };
#else
    {
        .chip_name = "bl-internal",
        .playback_supported = true,
        .capture_supported = true,
        .analog_capture_supported = AUADC_ANALOG_ADC_SUPPORT ? true : false,
        .pdm_capture_supported = true,
        .max_playback_channels = 1,
        .max_capture_channels = 1,
    };
#endif

static audio_codec_bl_state_t g_audio_codec_bl_states[4];

static audio_codec_bl_state_t *audio_codec_bl_get_state(audio_codec_dev_t *dev)
{
    if (dev == NULL) {
        return NULL;
    }
    return (audio_codec_bl_state_t *)dev->priv;
}

static int audio_codec_bl_map_audac_rate(uint32_t sample_rate_hz, uint8_t *out_rate)
{
    if (out_rate == NULL) {
        return -1;
    }

    switch (sample_rate_hz) {
        case 8000U:
            *out_rate = AUDAC_SAMPLING_RATE_8K;
            return 0;
        case 16000U:
            *out_rate = AUDAC_SAMPLING_RATE_16K;
            return 0;
        case 22050U:
            *out_rate = AUDAC_SAMPLING_RATE_22P05K;
            return 0;
        case 24000U:
            *out_rate = AUDAC_SAMPLING_RATE_24K;
            return 0;
        case 32000U:
            *out_rate = AUDAC_SAMPLING_RATE_32K;
            return 0;
        case 44100U:
            *out_rate = AUDAC_SAMPLING_RATE_44P1K;
            return 0;
        case 48000U:
            *out_rate = AUDAC_SAMPLING_RATE_48K;
            return 0;
        default:
            return -1;
    }
}

static int audio_codec_bl_map_auadc_rate(uint32_t sample_rate_hz, uint8_t *out_rate)
{
    if (out_rate == NULL) {
        return -1;
    }

    switch (sample_rate_hz) {
        case 8000U:
            *out_rate = AUADC_SAMPLING_RATE_8K;
            return 0;
        case 16000U:
            *out_rate = AUADC_SAMPLING_RATE_16K;
            return 0;
        case 22050U:
        case 24000U:
            *out_rate = AUADC_SAMPLING_RATE_24K;
            return 0;
        case 32000U:
            *out_rate = AUADC_SAMPLING_RATE_32K;
            return 0;
        case 44100U:
        case 48000U:
            *out_rate = AUADC_SAMPLING_RATE_48K;
            return 0;
        default:
            return -1;
    }
}

static int audio_codec_bl_apply_volume(audio_codec_bl_state_t *state, int volume_percent)
{
    if (state == NULL || state->audac_dev == NULL || !state->caps->playback_supported) {
        return -2;
    }

    if (volume_percent < 0) {
        volume_percent = 0;
    } else if (volume_percent > 100) {
        volume_percent = 100;
    }

    state->volume_percent = volume_percent;
    int db_half = ((volume_percent * 190) + 50) / 100 - 190;
    return bflb_audac_feature_control(state->audac_dev, AUDAC_CMD_SET_VOLUME_VAL, (size_t)db_half);
}

static int audio_codec_bl_apply_record_volume(audio_codec_bl_state_t *state, int volume_percent)
{
    if (state == NULL || state->auadc_dev == NULL || !state->caps->capture_supported) {
        return -2;
    }

    if (volume_percent < 0) {
        volume_percent = 0;
    } else if (volume_percent > 100) {
        volume_percent = 100;
    }

    state->record_volume_percent = volume_percent;
    int db_half = ((volume_percent * 190) + 50) / 100 - 190;
    return bflb_auadc_feature_control(state->auadc_dev, AUADC_CMD_SET_VOLUME_VAL, (size_t)db_half);
}

static int audio_codec_bl_apply_mic_pga(audio_codec_bl_state_t *state, int pga_percent)
{
#if AUADC_ANALOG_ADC_SUPPORT
    if (state == NULL || state->auadc_dev == NULL || !state->caps->analog_capture_supported) {
        return -2;
    }

    if (pga_percent < 0) {
        pga_percent = 0;
    } else if (pga_percent > 100) {
        pga_percent = 100;
    }

    state->mic_pga_percent = pga_percent;
    int gain_db = 6 + (((pga_percent * (42 - 6)) + 50) / 100);
    gain_db = ((gain_db + 1) / 3) * 3;
    if (gain_db < 6) {
        gain_db = 6;
    } else if (gain_db > 42) {
        gain_db = 42;
    }

    return bflb_auadc_feature_control(state->auadc_dev, AUADC_CMD_SET_PGA_GAIN_VAL, (size_t)gain_db);
#else
    (void)state;
    (void)pga_percent;
    return -2;
#endif
}

static int audio_codec_bl_init_playback(audio_codec_bl_state_t *state, const audio_codec_cfg_t *cfg)
{
    if (state == NULL || cfg == NULL || state->audac_dev == NULL || !state->caps->playback_supported) {
        return -2;
    }

    uint8_t sample_rate = 0;
    if (audio_codec_bl_map_audac_rate(cfg->sample_rate_hz, &sample_rate) != 0) {
        return -3;
    }

    struct bflb_audac_init_config_s audac_init_cfg = {
        .sampling_rate = sample_rate,
        .output_mode = AUDIO_CODEC_BL_AUDAC_OUTPUT_MODE,
        .source_channels_num = AUDAC_SOURCE_CHANNEL_SINGLE,
        .mixer_mode = AUDAC_MIXER_MODE_ONLY_L,
        .data_format = AUDAC_DATA_FORMAT_16BIT,
        .fifo_threshold = 3,
    };

    struct bflb_audac_volume_config_s audac_vol_cfg = {
        .mute_ramp_en = true,
        .mute_up_ramp_rate = AUDAC_RAMP_RATE_FS_32,
        .mute_down_ramp_rate = AUDAC_RAMP_RATE_FS_8,
        .volume_update_mode = AUDAC_VOLUME_UPDATE_MODE_RAMP,
        .volume_ramp_rate = AUDAC_RAMP_RATE_FS_128,
        .volume_zero_cross_timeout = AUDAC_RAMP_RATE_FS_128,
    };

    int ret = bflb_audac_init(state->audac_dev, &audac_init_cfg);
    if (ret != 0) {
        return ret;
    }

    ret = bflb_audac_volume_init(state->audac_dev, &audac_vol_cfg);
    if (ret != 0) {
        return ret;
    }

    ret = bflb_audac_link_rxdma(state->audac_dev, false);
    if (ret != 0) {
        return ret;
    }

    ret = bflb_audac_feature_control(state->audac_dev, AUDAC_CMD_SET_MUTE, state->mute);
    if (ret != 0) {
        return ret;
    }

    return audio_codec_bl_apply_volume(state, state->volume_percent);
}

static int audio_codec_bl_init_capture(audio_codec_bl_state_t *state, const audio_codec_cfg_t *cfg)
{
    if (state == NULL || cfg == NULL || state->auadc_dev == NULL || !state->caps->capture_supported) {
        return -2;
    }

    uint8_t sample_rate = 0;
    if (audio_codec_bl_map_auadc_rate(cfg->sample_rate_hz, &sample_rate) != 0) {
        return -3;
    }

    if ((AUDIO_CODEC_BL_AUADC_INPUT_MODE == AUADC_INPUT_MODE_PDM_L ||
         AUDIO_CODEC_BL_AUADC_INPUT_MODE == AUADC_INPUT_MODE_PDM_R) &&
        !state->caps->pdm_capture_supported) {
        return -4;
    }

#if AUADC_ANALOG_ADC_SUPPORT
    if (AUDIO_CODEC_BL_AUADC_INPUT_MODE == AUADC_INPUT_MODE_ADC && !state->caps->analog_capture_supported) {
        return -4;
    }
#endif

    struct bflb_auadc_init_config_s auadc_init_cfg = {
        .sampling_rate = sample_rate,
        .input_mode = AUDIO_CODEC_BL_AUADC_INPUT_MODE,
        .data_format = AUADC_DATA_FORMAT_16BIT,
        .fifo_threshold = 3,
    };

    int ret = bflb_auadc_init(state->auadc_dev, &auadc_init_cfg);
    if (ret != 0) {
        return ret;
    }

#if AUADC_ANALOG_ADC_SUPPORT
    if (AUDIO_CODEC_BL_AUADC_INPUT_MODE == AUADC_INPUT_MODE_ADC) {
        struct bflb_auadc_adc_init_config_s auadc_analog_cfg = {
            .auadc_analog_en = true,
            .adc_mode = AUADC_ADC_MODE_AUDIO,
            .adc_pga_mode = (cfg->mic_input_mode == AUDIO_CODEC_MIC_INPUT_SINGLE) ? AUADC_ADC_PGA_MODE_AC_SINGLE
                                                                                   : AUADC_ADC_PGA_MODE_AC_DIFFER,
            .adc_pga_posi_ch = AUDIO_CODEC_BL_AUADC_POS_CHANNEL,
            .adc_pga_nega_ch = AUDIO_CODEC_BL_AUADC_NEG_CHANNEL,
            .adc_pga_gain = 21,
            .adc_measure_rate = AUADC_ADC_MEASURE_RATE_SPS_2000,
        };

        ret = bflb_auadc_adc_init(state->auadc_dev, &auadc_analog_cfg);
        if (ret != 0) {
            return ret;
        }
    }
#endif

    ret = bflb_auadc_link_rxdma(state->auadc_dev, false);
    if (ret != 0) {
        return ret;
    }

    ret = audio_codec_bl_apply_record_volume(state, state->record_volume_percent);
    if (ret != 0) {
        return ret;
    }

#if AUADC_ANALOG_ADC_SUPPORT
    if (AUDIO_CODEC_BL_AUADC_INPUT_MODE == AUADC_INPUT_MODE_ADC) {
        ret = audio_codec_bl_apply_mic_pga(state, state->mic_pga_percent);
        if (ret != 0) {
            return ret;
        }
    }
#endif

    return 0;
}

static int audio_codec_bl_reinit(audio_codec_dev_t *dev)
{
    audio_codec_bl_state_t *state = audio_codec_bl_get_state(dev);
    if (state == NULL) {
        return -2;
    }

    board_audio_pll_config_for_rate(state->cfg.sample_rate_hz);

    if (state->cfg.enable_dac) {
        int ret = audio_codec_bl_init_playback(state, &state->cfg);
        if (ret != 0) {
            return ret;
        }
    }

    if (state->cfg.enable_adc) {
        int ret = audio_codec_bl_init_capture(state, &state->cfg);
        if (ret != 0) {
            return ret;
        }
    }

    return 0;
}

static int audio_codec_bl_init(audio_codec_dev_t *dev, const audio_codec_cfg_t *cfg)
{
    audio_codec_bl_state_t *state = audio_codec_bl_get_state(dev);
    if (state == NULL || cfg == NULL) {
        return -1;
    }

    state->cfg = *cfg;
    if (state->cfg.channels == 0U) {
        state->cfg.channels = 1U;
    }

    return audio_codec_bl_reinit(dev);
}

static int audio_codec_bl_set_mute(audio_codec_dev_t *dev, bool mute)
{
    audio_codec_bl_state_t *state = audio_codec_bl_get_state(dev);
    if (state == NULL || state->audac_dev == NULL || !state->caps->playback_supported) {
        return -2;
    }

    state->mute = mute;
    return bflb_audac_feature_control(state->audac_dev, AUDAC_CMD_SET_MUTE, mute ? 1U : 0U);
}

static int audio_codec_bl_set_volume(audio_codec_dev_t *dev, int volume_percent)
{
    return audio_codec_bl_apply_volume(audio_codec_bl_get_state(dev), volume_percent);
}

static int audio_codec_bl_set_record_volume(audio_codec_dev_t *dev, int volume_percent)
{
    return audio_codec_bl_apply_record_volume(audio_codec_bl_get_state(dev), volume_percent);
}

static int audio_codec_bl_set_mic_pga(audio_codec_dev_t *dev, int pga_percent)
{
    return audio_codec_bl_apply_mic_pga(audio_codec_bl_get_state(dev), pga_percent);
}

static int audio_codec_bl_set_sample_rate(audio_codec_dev_t *dev, uint32_t sample_rate_hz)
{
    audio_codec_bl_state_t *state = audio_codec_bl_get_state(dev);
    if (state == NULL) {
        return -2;
    }

    state->cfg.sample_rate_hz = sample_rate_hz;
    return audio_codec_bl_reinit(dev);
}

static uint32_t audio_codec_bl_calc_mclk_hz(uint32_t sample_rate_hz)
{
    return sample_rate_hz * 256U;
}

const audio_codec_driver_t audio_codec_bl_driver = {
    .name = "bl-internal",
    .i2c_addrs = NULL,
    .probe = NULL,
    .init = audio_codec_bl_init,
    .set_mute = audio_codec_bl_set_mute,
    .set_volume = audio_codec_bl_set_volume,
    .set_record_volume = audio_codec_bl_set_record_volume,
    .set_mic_pga = audio_codec_bl_set_mic_pga,
    .set_sample_rate = audio_codec_bl_set_sample_rate,
    .calc_mclk_hz = audio_codec_bl_calc_mclk_hz,
    .type = AUDIO_CODEC_TYPE_BL,
    .i2c_addrs_num = 0,
};

int audio_codec_bl_open(audio_codec_dev_t *out_dev, const audio_codec_bl_hw_cfg_t *cfg)
{
    if (out_dev == NULL || cfg == NULL) {
        return -1;
    }

    audio_codec_bl_state_t *state = NULL;
    for (size_t i = 0; i < (sizeof(g_audio_codec_bl_states) / sizeof(g_audio_codec_bl_states[0])); i++) {
        if (!g_audio_codec_bl_states[i].used) {
            state = &g_audio_codec_bl_states[i];
            memset(state, 0, sizeof(*state));
            state->used = true;
            break;
        }
    }

    if (state == NULL) {
        return -2;
    }

    state->audac_dev = cfg->audac;
    state->auadc_dev = cfg->auadc;
    state->caps = &g_audio_codec_bl_caps;
    state->cfg.sample_rate_hz = 16000U;
    state->cfg.channels = 1U;
    state->cfg.bits = AUDIO_CODEC_BITS_16;
    state->cfg.enable_dac = (cfg->audac != NULL);
    state->cfg.enable_adc = (cfg->auadc != NULL);
    state->cfg.mic_input_mode = AUDIO_CODEC_MIC_INPUT_DIFF;
    state->volume_percent = 50;
    state->record_volume_percent = 50;
    state->mic_pga_percent = 50;
    state->mute = false;

    out_dev->name = g_audio_codec_bl_caps.chip_name;
    out_dev->i2c = NULL;
    out_dev->drv = &audio_codec_bl_driver;
    out_dev->type = AUDIO_CODEC_TYPE_BL;
    out_dev->i2c_addr = 0;
    out_dev->priv = state;
    return 0;
}
