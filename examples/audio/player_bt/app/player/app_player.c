/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <math.h>
#include <smart_audio.h>
#include <media.h>

#ifdef CONFIG_SHELL
#include <devices/drv_snd_bl616.h>
#include <shell.h>
#else
#include <aos/cli.h>
#endif

#include <msp/kernel.h>
#include <board.h>

/* 相关配置需要调整到板卡中，或者app配置文件中 */
//#define PA_PIN                 4
//#define INPUT_NEGATIVE_PIN      (28)
//#define INPUT_POSITIVE_PIN      (27)
//#define OUTPUT_NEGATIVE_PIN     (14)
//#define OUTPUT_POSITIVE_PIN     (15)

static void board_audio_init(void)
{
    snd_bl616_config_t snd_config;
    // snd_config.audio_in_gain_list[0] = g_audio_in_gain[0];
    // snd_config.audio_in_gain_list[1] = g_audio_in_gain[1];
    // snd_config.audio_in_gain_list[2] = g_audio_in_gain[2];
    // snd_config.audio_out_gain_list[0] = g_audio_out_gain[0];
    // snd_config.audio_out_gain_list[1] = g_audio_out_gain[1];
    // snd_config.mic_type = INPUT_MIC_TYPE_ANALOG;
    // snd_config.pdm_channel = PDM_CHANNEL;
    // snd_config.pdm_in_pin = PDM_IN_PIN;
    // snd_config.pdm_clk_pin = PDM_CLK_PIN;
    //snd_config.pa_pin = PA_PIN;
    //snd_config.in_positive_pin = INPUT_NEGATIVE_PIN;
    //snd_config.in_negative_pin = INPUT_POSITIVE_PIN;
    //snd_config.out_positive_pin = OUTPUT_NEGATIVE_PIN;
    //snd_config.out_negative_pin = OUTPUT_POSITIVE_PIN;
    snd_card_bl616_register(&snd_config);
}

static void media_evt(int type, smtaudio_player_evtid_t evt_id)
{
  switch (evt_id) {
        case SMTAUDIO_PLAYER_EVENT_START:
            printf("SMTAUDIO_PLAYER_EVENT_START\r\n");
            break;
        case SMTAUDIO_PLAYER_EVENT_ERROR:
            printf("SMTAUDIO_PLAYER_EVENT_ERROR\r\n");
            break;
        case SMTAUDIO_PLAYER_EVENT_STOP:
            printf("SMTAUDIO_PLAYER_EVENT_STOP\r\n");
            break;
        case SMTAUDIO_PLAYER_EVENT_PAUSE:
            printf("SMTAUDIO_PLAYER_EVENT_PAUSE\r\n");
            break;
        case SMTAUDIO_PLAYER_EVENT_RESUME:
            printf("SMTAUDIO_PLAYER_EVENT_RESUME\r\n");
            break;
        case SMTAUDIO_PLAYER_EVENT_UNDER_RUN:
            printf("SMTAUDIO_PLAYER_EVENT_UNDER_RUN\r\n");
            break;
        case SMTAUDIO_PLAYER_EVENT_OVER_RUN:
            printf("SMTAUDIO_PLAYER_EVENT_OVER_RUN\r\n");
            break;
        default:
            break;
    }
}

int app_player_init(void)
{
    static char inited = 0;

    if (0 == inited) {
        board_audio_init();
        smtaudio_init(media_evt);
        smtaudio_register_local_play(20, NULL, 0, 1.0f, 48000);
        smtaudio_register_online_music(20, NULL, 0, 1.0f, 48000);
        // smtaudio_register_bt_a2dp(20, NULL, 0, 1.0f, 48000);

        player_init();

        inited = 1;
    }

    return 0;
}

