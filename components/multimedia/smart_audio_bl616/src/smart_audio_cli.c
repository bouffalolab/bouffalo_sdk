/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <math.h>
#include <smart_audio.h>
#include <player.h>
#include <media.h>
#include <cli.h>
#include <msp/kernel.h>
// #include <board.h>

#include <avutil/vol_scale.h>
#include <devices/drv_snd_bl616.h>
// #include "bt/app_bt.h"

#define TAG "player"

/******************************************************
 * 测试音播放
 *****************************************************/
#include "avutil/named_straightfifo.h"
static volatile int g_task_play_sin_is_run = 0;
static volatile int g_task_loop_play_is_run = 0;
static msp_task_t g_loop_task;

static void audio_loop_proc_task(void *arg)
{
    smtaudio_state_t state;

    while (1) {
        state = smtaudio_get_state();
        //printf("====####state:%d\r\n", state);
        if (state == SMTAUDIO_STATE_STOP) {
            //printf("====####test\r\n");
            smtaudio_start(MEDIA_SYSTEM, arg, 0, 0);
        }
        msp_msleep(100);
    }

    msp_task_exit(0);
}

static int nsfifo_is_reof(nsfifo_t *fifo)
{
    int     ret;
    uint8_t reof;
    uint8_t weof;
    ret = nsfifo_get_eof(fifo, &reof, &weof);

    return (ret == 0) ? reof : 1;
}

static void play_sin(int second)
{
    int       reof     = 0;
    char *    pos      = NULL;
    int       len      = 0;
    nsfifo_t *fifo     = NULL;
    int       pos_50ms = 0;

    uint8_t wav_head[] = {0x52, 0x49, 0x46, 0x46, 0x24, 0xA6, 0x0E, 0x00, 0x57, 0x41, 0x56,
                          0x45, 0x66, 0x6D, 0x74, 0x20, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00,
                          0x01, 0x00, 0x80, 0x3E, 0x00, 0x00, 0x00, 0x7D, 0x00, 0x00, 0x02,
                          0x00, 0x10, 0x00, 0x64, 0x61, 0x74, 0x61, 0x00, 0xA6, 0x0E, 0x00};

    int wav_head_int[11];

    int16_t sin_1k[16] = {
        0, 12539,  23169,  30272,  32767,  30272,  23169,  12539,
        0, -12540, -23170, -30273, -32767, -30273, -23170, -12540,
    };
    char sin1k_50ms[sizeof(sin_1k) * 50];

    memcpy(wav_head_int, wav_head, sizeof(wav_head));

    /* fill the 50 ms buf*/
    for (int i = 0; i < 50; i++) {
        memcpy(sin1k_50ms + i * sizeof(sin_1k), sin_1k, sizeof(sin_1k));
    }

    if (second < 1) {
        second = 30;
    }

    /*每秒32000 + 36*/
    wav_head_int[1]  = 32000 * second + 36; /*wav 文件大小 - 8*/
    wav_head_int[10] = 32000 * second;      /* PCM数据字节数 */

    fifo = nsfifo_open("fifo://sintest", O_CREAT, 16000 / 1000 * 16 / 8 * 10 * 400);

    smtaudio_stop(MEDIA_SYSTEM);
    smtaudio_start(MEDIA_SYSTEM, "fifo://sintest", 0, 0);

    len = nsfifo_get_wpos(fifo, &pos, 4000);
    if (len > sizeof(wav_head)) {
        memcpy(pos, wav_head_int, sizeof(wav_head));
        nsfifo_set_wpos(fifo, sizeof(wav_head));
    } else {
        nsfifo_close(fifo);
        return;
    }

    int wlen = 0;
    while (wlen < wav_head_int[10]) {
        if (g_task_play_sin_is_run == 0) {
            break;
        }

        reof = nsfifo_is_reof(fifo); /** peer read reach to end */
        if (reof) {
            break;
        }

        //LOGD("sin", "wpos before");
        len = nsfifo_get_wpos(fifo, &pos, 4000);
        if (len <= 0) {
            /* If available space is too small, wait player eat the buffer */
            msp_msleep(10);
            continue;
        }
        //LOGD("sin", "wpos after %d", len);

        int curwlen = MIN(sizeof(sin1k_50ms) - pos_50ms, len);

        memcpy(pos, sin1k_50ms + pos_50ms, curwlen);
        pos_50ms = (pos_50ms + curwlen) % sizeof(sin1k_50ms);
        pos += curwlen;
        wlen += curwlen;

        nsfifo_set_wpos(fifo, curwlen);
    }

    LOGD("sin", "wpos eof");
    /* set write eof */
    nsfifo_set_eof(fifo, 0, 1);

    while (0 == nsfifo_is_reof(fifo)) {
        msp_msleep(100); //wait peer drain fifo
    }
    nsfifo_close(fifo);
}

static void task_play_sin(void *arg)
{
    if (g_task_play_sin_is_run) {
        return;
    }

    g_task_play_sin_is_run = 1;

    ssize_t type = (ssize_t)arg;
    play_sin((int)type);

    g_task_play_sin_is_run = 0;

    msp_task_exit(0);
}

void play_sintest(int sec)
{
    ssize_t        second = sec;
    msp_task_t task_handle;
    msp_task_new_ext(&task_handle, "sin", task_play_sin, (void *)second, 4096,
                        MSP_DEFAULT_APP_PRI);
}

static int cli_player_proc(int argc, char **argv)
{
    if (argc < 2) {
        return -1;
    }

    if (strcmp(argv[1], "play") == 0) {
        char *   url;
        uint64_t seek_time;
        if (argc == 4) {
            url       = argv[3];
            seek_time = atoi(argv[2]);
        } else {
            url       = argv[2];
            seek_time = 0;
        }

        if (SMTAUDIO_STATE_MUTE != smtaudio_get_state()) {
            smtaudio_start(MEDIA_MUSIC, url, seek_time, 0);
        } else {
            printf("Device is mute\n");
        }
    } else if (strcmp(argv[1], "get_curtime") == 0) {
        aui_play_time_t ptime;

        int ret = aui_player_get_time(MEDIA_MUSIC, &ptime);
        if (ret == 0) {
            printf("get current time %lld %lld\n", (long long)ptime.curtime, (long long)ptime.duration);
        } else {
            printf("get time failed");
        }
    } else if (strcmp(argv[1], "vol") == 0) {
        if (argv[2]) {
            if (argv[2][0] == '+') {
                smtaudio_vol_up(10);
                printf("volume inc:success\n");
            } else if (argv[2][0] == '-') {
                smtaudio_vol_down(10);
                printf("volume dec:success\n");
            } else if (argv[2][0] == '?') {
                printf("volume get:%d\n", smtaudio_vol_get());
            } else {
                int vol = atoi(argv[2]);
                smtaudio_vol_set(vol);
                printf("volume set:%d\n", vol);
            }
        } else {
            printf("vol:error\n");
        }
    } else if (strcmp(argv[1], "stop") == 0) {
        int type = SMTAUDIO_TYPE_ALL;
        if (argc == 3) {
            type = atoi(argv[2]);
            type = ((type < SMTAUDIO_ONLINE_MUSIC) || (type > SMTAUDIO_BT_A2DP))
                       ? SMTAUDIO_TYPE_ALL
                       : type;
        }
        smtaudio_stop(type);
        if (g_task_loop_play_is_run) {
            msp_task_delete(&g_loop_task);
        }
        g_task_loop_play_is_run = 0;
        g_task_play_sin_is_run = 0; /* fix me:why play sin cannot stop */
    } else if (strcmp(argv[1], "pause") == 0) {
        smtaudio_pause();
    } else if (strcmp(argv[1], "resume") == 0) {
        smtaudio_resume();
    } else if (strcmp(argv[1], "splay") == 0) {
        smtaudio_start(MEDIA_SYSTEM, argv[2], 0, 1);
    } else if (strcmp(argv[1], "speed") == 0) {
        if (argc == 3) {
            char *ptr   = NULL;
            float speed = strtof(argv[2], &ptr);
            smtaudio_set_speed(speed);
            smtaudio_get_speed(&speed);
            printf("play speed:%.2f\n", speed);
        } else {
            printf("speed:error\n");
        }
    } else if (strcmp(argv[1], "sin") == 0) {
        if (SMTAUDIO_STATE_MUTE != smtaudio_get_state()) {
            msp_task_t task_handle;
            ssize_t second = 0;
            if(argv[2]) {
                second = atoi(argv[2]);
            } else {
                second = 10;
            }
            msp_task_new_ext(&task_handle, "sin", task_play_sin, (void *)second, 4096,
                             MSP_DEFAULT_APP_PRI);
        } else {
            printf("Device is mute\n");
        }
    } else if (strcmp(argv[1], "state_get") == 0) {
        int         smt_cur_state, smt_last_state;
        extern void smtaudio_substate_get(int *cur_state, int *last_state);
        smtaudio_substate_get(&smt_cur_state, &smt_last_state);
        printf("smt audio cur_state:%d last_state:%d\n", smt_cur_state, smt_last_state);
        extern void smtaudio_check_ready_list(void);
        smtaudio_check_ready_list();
    } else if (strcmp(argv[1], "volmap") == 0) {
        int16_t in[] = {32767};
        int16_t out[] = {0};
        float db_last = 0;

        for (int i = 0; i < 256; i++){
            vol_scale(in, 1, out, i);
            float db =  20*log10f((float)out[0]/32767);
            printf("%d, %.3f, %.3f\n", i, db, db- db_last);
            db_last = db;
        }
#ifdef CONFIG_BOARD_AUDIO
    } else if (strcmp(argv[1], "gain") == 0) {
        int gain = atoi(argv[2]);
        printf("set out gain %d", gain);
        board_audio_out_set_gain(0, gain);
        /* 重新初始化更新增益参数 */
        board_audio_init();
#endif
#if defined(CONFIG_BT_BREDR) && (CONFIG_BT_BREDR == 1)
    } else if (strcmp(argv[1], "a2dp") == 0) {
        if (argc == 3) {
            int cmd = atoi(argv[2]);
            if (cmd == 0) {
                app_bt_adv_enable(0);
            } else {
                app_bt_adv_enable(1);
            }
        }
#endif
    } else if (strcmp(argv[1], "loop_play") == 0) {
        char *   url;

        if (argc == 3) {
            url       = argv[2];
        } else {
            printf("url error!\r\n");
            return -1;
        }
        if (0 == g_task_loop_play_is_run) {
            g_task_loop_play_is_run = 1;
            if (SMTAUDIO_STATE_MUTE != smtaudio_get_state()) {
                msp_task_new_ext(&g_loop_task, "loop_play", audio_loop_proc_task, url, 4096, MSP_DEFAULT_APP_PRI + 4);
            } else {
                printf("Device is mute\n");
            }
        } else {
            printf("loop play is already run!\r\n");
        }
    } else {
        return -1;
    }
    return 0;
}

static void cmd_player_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc >= 2) {
        if (cli_player_proc(argc, argv) == 0) {
            return;
        }
    } else {
        printf("\tsmta play url[http://]\n");
        printf("\tsmta loop_play url[http://]\n");
        printf("\tsmta stop|pause|resume 0|1|2|255\n");
        printf("\tsmta vol +|-|?|[0-100]\n");
        printf("\tsmta sin 3\n");
        printf("\tsmta a2dp 1|0\n");
        printf("\tsmta state_get\n");
    }
}

void cli_reg_cmd_player(void)
{
    static const struct cli_command cmd_info = {"smta", "smartaudio test", cmd_player_func};

    aos_cli_register_command(&cmd_info);
}

const struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    { "smta", "smartaudio", cmd_player_func},
};

int smartapp_cli_init(void)
{
    return 0;
}

