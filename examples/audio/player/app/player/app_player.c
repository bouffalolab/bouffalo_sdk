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
#include "avutil/named_straightfifo.h"
#include <msp_fs.h>
#include <xav_port.h>

/* Relevant configurations need to be adjusted either in the board or in the app's configuration file. */
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

#if defined(CONFIG_STREAMER_CUSTOM) && CONFIG_STREAMER_CUSTOM
// User-defined decryption interface
static custom_decode_cb_t decoder_test_program(const char *in_data, int len, int offset, char *out_data)
{
    if (out_data != NULL && in_data != NULL && len > 0)
    {
        memcpy(out_data, in_data, len);
    }
    return len;
}
#endif

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

#if defined(CONFIG_STREAMER_CUSTOM) && CONFIG_STREAMER_CUSTOM
    register_custom_decode_cb(decoder_test_program);
#endif

    return 0;
}

#define FRANE_SIZE    4096

static const char music_mp3[] = {
#include <music_mp3.h>
};

static void _fifo_task(void *arg)
{
    nsfifo_t* tts_fifo;
    char *pos;
    int cnt = 0, rc, wlen;
    uint8_t reof = 0;

    tts_fifo = nsfifo_open("fifo://tts/1", MSP_FS_CREAT, 8*1024);

    smtaudio_start(MEDIA_SYSTEM, "fifo://tts/1", 0, 0);

    while (cnt < sizeof(music_mp3)) {
        /* Get the writable pointer and length of the FIFO */
        wlen = nsfifo_get_wpos(tts_fifo, &pos, 8*1000);

        /* Check if the read end of the player FIFO has exited (possible playback error) */
        nsfifo_get_eof(tts_fifo, &reof, NULL);
        if (wlen <= 0 || reof) {
            printf("get wpos err. wlen = %d, reof = %d", wlen, reof);
            break;
        }

        /* Calculate the length of a single write operation, not exceeding wlen */
        rc = MIN(wlen, sizeof(music_mp3) - cnt);
        int real_len = (rc > FRANE_SIZE) ? FRANE_SIZE : rc;

        memcpy(pos, music_mp3 + cnt, real_len);

        /* Set the write pointer */
        nsfifo_set_wpos(tts_fifo, real_len);
        cnt += real_len;
    }
    /* set write eof */
    nsfifo_set_eof(tts_fifo, 0, 1);
    nsfifo_close(tts_fifo);
    
    msp_task_exit(0);
}

int app_play_fifo_music()
{
    msp_task_new("fifo_task", _fifo_task, NULL, 4096);
    return 0;
}
