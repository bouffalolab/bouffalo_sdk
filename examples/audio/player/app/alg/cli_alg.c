/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdio.h>
#include <stdint.h>

#ifdef CONFIG_SHELL
#include <devices/drv_snd_bl616.h>
#include <shell.h>
#else
#include <aos/cli.h>
#endif

#include <msp/kernel.h>

#include <aec.h>

#define NEAREND_BUFFER_SAMPLES_NUM          (160)//Number of samples in nearend buffer

int aec_test_buf(uint16_t *out_buf, uint16_t *speaker_buf, uint16_t *mic_buf, uint32_t frame_count)
{
    int used_ms = 0;
    void *aecmInst = NULL;
    AecConfig config;

    int m;
    int start_ms, end_ms;

    // init
    msp_aec_create(&aecmInst);
    msp_aec_init(aecmInst, 16000, 16000);

    // config
    config.nlpMode = kAecNlpConservative;  //set aec mode
    msp_aec_set_config(aecmInst, config);

    // process
    start_ms = msp_now_ms();
    for (m = 0; (m + NEAREND_BUFFER_SAMPLES_NUM) <= frame_count; m+=NEAREND_BUFFER_SAMPLES_NUM) {// uint16_t
        msp_aec_bufferfarend(aecmInst, (const int16_t*)&(speaker_buf[m]), (int16_t)NEAREND_BUFFER_SAMPLES_NUM);
        msp_aec_process(aecmInst, (const int16_t*)&(mic_buf[m]),
            NULL, (int16_t*)&(out_buf[m]), NULL, (int16_t)NEAREND_BUFFER_SAMPLES_NUM, 40, 0);
    }
    end_ms = msp_now_ms();
    // printf("use_ms = %d, frame_count = %d\r\n", (end_ms - start_ms), frame_count);
    used_ms = end_ms - start_ms;

    // uninit
    msp_aec_free(aecmInst);

    return used_ms;
}

static void _alg_aec_task(void *arg)
{
    #define TEST_INPUT_FILE_LEN      (3)// 3S
    int res;

    extern const unsigned char speaker_16k_1ch_pcm[];
    // extern const unsigned int speaker_16k_1ch_pcm_len;// = 111492;
    extern const unsigned char micinput_16k_1ch_pcm[];
    // extern const unsigned int micinput_16k_1ch_pcm_len;// = 111492;

    unsigned char *out_pcm = NULL;//static unsigned char out_pcm[TEST_INPUT_FILE_LEN*16000*2];

    out_pcm = msp_malloc(TEST_INPUT_FILE_LEN*16000*2);
    if (NULL == out_pcm) {
        printf("mem error\r\n");
        return;
    }
    res = aec_test_buf((uint16_t *)out_pcm,
        (uint16_t *)speaker_16k_1ch_pcm,
        (uint16_t *)micinput_16k_1ch_pcm, TEST_INPUT_FILE_LEN*16000);

    printf("out_pcm:%p, used_ms:%d, cpu usage:%d%%\r\n",
        out_pcm, res, (res)/(TEST_INPUT_FILE_LEN*10));// (res * 100)/3000

    msp_free(out_pcm);

    msp_task_exit(0);
}

#ifdef CONFIG_SHELL
static void cmd_alg_func(int argc, char **argv)
#else
static void cmd_alg_func(char *wbuf, int wbuf_len, int argc, char **argv)
#endif
{
    msp_task_t task_handle = NULL;

    msp_task_new_ext(&task_handle,
            "algaec",
            _alg_aec_task,
            NULL,
            16*1024,
            10);
}

#ifdef CONFIG_SHELL
#include <shell.h>
SHELL_CMD_EXPORT_ALIAS(cmd_alg_func, alg, alg test);
#else
void cli_reg_cmd_alg(void)
{
    static const struct cli_command cmd_info = {"alg", "alg test", cmd_alg_func};

    msp_cli_register_command(&cmd_info);
}
#endif
