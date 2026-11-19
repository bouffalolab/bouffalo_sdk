/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#include "board.h"
#include "board_gpio.h"
#include "board_audio.h"
#include "xcodec_dev_defaults.h"

static xcodec_board_t audio_board;

xcodec_board_handle_t xcodec_dev_board_init(void)
{
    if (audio_board.play_codec || audio_board.record_codec ||
        audio_board.play_data || audio_board.record_data) {
        return NULL;
    }
    /* All supported converter rates share the 48 kHz-family reference clock. */
    board_audio_pll_config_for_rate(16000);
    board_auadc_gpio_init();
    board_audac_gpio_init();
    audio_board.record_codec = xcodec_builtin_codec_new(&(xcodec_builtin_codec_cfg_t){
        .auadc = bflb_device_get_by_name("auadc"),
#ifndef CONFIG_AUDIO_MIC_PDM_MODE
        .analog_input = true,
#endif
    });
    audio_board.play_codec = xcodec_builtin_codec_new(&(xcodec_builtin_codec_cfg_t){
        .audac = bflb_device_get_by_name("audac"),
    });
    if (!audio_board.record_codec || !audio_board.play_codec) {
        xcodec_dev_board_deinit();
        return NULL;
    }
    xcodec_audac_cfg_t play_cfg = XCODEC_AUDAC_DEFAULT_CONFIG(
        bflb_device_get_by_name("audac"), bflb_device_get_by_name("dma0_ch1"));
    xcodec_auadc_cfg_t record_cfg = XCODEC_AUADC_DEFAULT_CONFIG(
        bflb_device_get_by_name("auadc"), bflb_device_get_by_name("dma0_ch0"));

#ifndef CONFIG_AUDIO_MIC_PDM_MODE
    record_cfg.config.input_mode = AUADC_INPUT_MODE_ADC;
    record_cfg.analog_config = (struct bflb_auadc_adc_init_config_s){
        .auadc_analog_en = true,
        .adc_mode = AUADC_ADC_MODE_AUDIO,
        .adc_pga_mode = AUADC_ADC_PGA_MODE_AC_DIFFER,
        .adc_pga_posi_ch = AUADC_ADC_ANALOG_CH_4,
        .adc_pga_nega_ch = AUADC_ADC_ANALOG_CH_7,
        .adc_pga_gain = 21,
    };
#endif
    play_cfg.channel = xcodec_channel_default_config;
    record_cfg.channel = xcodec_channel_default_config;
    audio_board.play_data = xcodec_audac_data_new(&play_cfg);
    audio_board.record_data = xcodec_auadc_data_new(&record_cfg);

    if (!audio_board.play_data || !audio_board.record_data) {
        xcodec_dev_board_deinit();
        return NULL;
    }
    return &audio_board;
}

int xcodec_dev_board_deinit(void)
{
    int ret;
    if (audio_board.record_data && audio_board.record_data != audio_board.play_data &&
        (ret = xcodec_delete_data_if(audio_board.record_data))) {
        return ret;
    }
    audio_board.record_data = NULL;
    if (audio_board.play_data && (ret = xcodec_delete_data_if(audio_board.play_data))) {
        return ret;
    }
    audio_board.play_data = NULL;
    if (audio_board.record_codec && (ret = xcodec_delete_codec_if(audio_board.record_codec))) {
        return ret;
    }
    audio_board.record_codec = NULL;
    if (audio_board.play_codec && (ret = xcodec_delete_codec_if(audio_board.play_codec))) {
        return ret;
    }
    audio_board.play_codec = NULL;
    return XCODEC_DEV_OK;
}
