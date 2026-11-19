/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#include "board.h"
#include "board_audio.h"
#include "xcodec_dev_defaults.h"
#include "audio_codec.h"

typedef struct {
    audio_codec_dev_t codec;
    xcodec_board_t board;
} codec_res_t;

static codec_res_t codec_res;

static int board_audio_ctrl_acquire(void)
{
    return board_shared_i2c_acquire(true) ? XCODEC_DEV_DRV_ERR : XCODEC_DEV_OK;
}

static void board_audio_ctrl_release(void)
{
    board_shared_i2c_release();
}

xcodec_board_handle_t xcodec_dev_board_init(void)
{
    if (codec_res.codec.drv || codec_res.board.play_codec || codec_res.board.record_codec ||
        codec_res.board.play_data || codec_res.board.record_data) {
        return NULL;
    }
    if (board_shared_i2c_init()) {
        return NULL;
    }
    board_i2s_codec_gpio_init();

    xcodec_dev_sample_info_t format = XCODEC_DEV_SAMPLE_INFO_DEFAULT_CONFIG();
    audio_codec_cfg_t codec_cfg = {
        .sample_rate_hz = format.sample_rate,
#ifdef CONFIG_AUDIO_CODEC_JY6416_ENABLE
        .channels = 6,
#else
        .channels = 4,
#endif
        .i2s_fmt = AUDIO_CODEC_I2S_FMT_DSP,
        .bits = format.bits_per_sample,
        .enable_adc = true,
        .enable_dac = true,
        .mic_input_mode = AUDIO_CODEC_MIC_INPUT_DIFF,
    };

    if (board_audio_ctrl_acquire()) {
        goto fail;
    }
    audio_codec_type_t type = audio_codec_scan(bflb_device_get_by_name("i2c0"), &codec_res.codec);
    int ret = type == AUDIO_CODEC_TYPE_UNKNOWN ? XCODEC_DEV_NOT_FOUND :
              audio_codec_init(&codec_res.codec, &codec_cfg);
    board_audio_ctrl_release();
    if (ret) {
        goto fail;
    }

    xcodec_audio_codec_cfg_t ctrl_cfg = {
        .codec = &codec_res.codec,
        .acquire = board_audio_ctrl_acquire,
        .release = board_audio_ctrl_release,
        .mode = AUDIO_CODEC_WORK_MODE_PLAYBACK,
    };
    codec_res.board.play_codec = xcodec_ctrl_if_new(&ctrl_cfg);
    if (!codec_res.board.play_codec) {
        goto fail;
    }

    ctrl_cfg.mode = AUDIO_CODEC_WORK_MODE_RECORDING;
    codec_res.board.record_codec = xcodec_ctrl_if_new(&ctrl_cfg);
    if (!codec_res.board.record_codec) {
        goto fail;
    }

    xcodec_i2s_cfg_t cfg = XCODEC_I2S_DEFAULT_CONFIG(
        bflb_device_get_by_name("i2s0"),
        bflb_device_get_by_name(BOARD_AUDIO_DMA_TX_NAME),
        bflb_device_get_by_name(BOARD_AUDIO_DMA_RX_NAME));

    cfg.config.format_mode = I2S_MODE_DSP_SHORT_FRAME_SYNC;
    cfg.config.channel_mode = codec_cfg.channels == 6 ?
                              I2S_CHANNEL_MODE_NUM_6 : I2S_CHANNEL_MODE_NUM_4;
    cfg.slots = codec_cfg.channels;
    cfg.lr_merge = false;
    cfg.set_clock = board_audio_pll_config_for_rate;
    cfg.channel = xcodec_channel_default_config;

    codec_res.board.play_data = xcodec_i2s_data_new(&cfg);
    codec_res.board.record_data = codec_res.board.play_data;

    if (!codec_res.board.play_data) {
        goto fail;
    }
    return &codec_res.board;

fail:
    xcodec_dev_board_deinit();
    return NULL;
}

int xcodec_dev_board_deinit(void)
{
    int ret;
    if (codec_res.board.record_data && codec_res.board.record_data != codec_res.board.play_data &&
        (ret = xcodec_delete_data_if(codec_res.board.record_data))) {
        return ret;
    }
    codec_res.board.record_data = NULL;
    if (codec_res.board.play_data && (ret = xcodec_delete_data_if(codec_res.board.play_data))) {
        return ret;
    }
    codec_res.board.play_data = NULL;
    if (codec_res.board.record_codec && (ret = xcodec_delete_codec_if(codec_res.board.record_codec))) {
        return ret;
    }
    codec_res.board.record_codec = NULL;
    if (codec_res.board.play_codec && (ret = xcodec_delete_codec_if(codec_res.board.play_codec))) {
        return ret;
    }
    codec_res.board.play_codec = NULL;
    if (codec_res.codec.drv) {
        if ((ret = board_audio_ctrl_acquire())) {
            return ret;
        }
        ret = audio_codec_deinit(&codec_res.codec);
        board_audio_ctrl_release();
        if (ret && ret != -2) {
            return XCODEC_DEV_DRV_ERR;
        }
    }
    codec_res = (codec_res_t){0};
    return XCODEC_DEV_OK;
}
