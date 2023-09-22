/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdlib.h>
#include <stdio.h>
#include "audio_file.h"
#include <xcodec.h>


/* user config param */
//#define OUTPUT_NEGATIVE_PIN     (14)
//#define OUTPUT_POSITIVE_PIN     (15)
//#define DMA_CHANNEL             (0)

#define OUTPUT_BUF_SIZE     (21 * 1024)         /*for ringbuffer*/ 
#define OUTPUT_PERIOD       (1024)              /*32ms = 1024/(16000*2/1000), sample rate=16kHz, sample depth=16bit*/

//#define DMA_NUM                 (0)

static xcodec_t codec;
static xcodec_output_t codec_output_ch;
static xcodec_dma_ch_t *dma_hdl_out;
static uint8_t g_start_run = 0;
static uint32_t dma_ch_output_handle;
static uint8_t *output_buf = NULL;
static m_ringbuf_t loop_output_ring_buffer;

static void codec_output_event_cb_fun(xcodec_output_t *output, xcodec_event_t event, void *arg)
{
    if (event == XCODEC_EVENT_PERIOD_WRITE_COMPLETE) {
        //printf("out_flag:%d\r\n", out_flag);
    } else if (event == XCODEC_EVENT_WRITE_BUFFER_EMPTY) {
        //printf("output CODEC_EVENT_WRITE_BUFFER_EMPTY\r\n");
    } else {

    }
}

static void _codec_output_init(uint32_t sample_rate)
{
    xcodec_error_t ret;
    xcodec_output_config_t output_config;
    codec.output_chs = &codec_output_ch;
    
    g_start_run = 1;    
    ret = xcodec_init(&codec, 0);
    if (ret != XCODEC_OK) {
        printf("xcodec_init error\n");
        g_start_run = 0;
        msp_task_exit(0);
    }

    uint8_t *p_buf = NULL;

    output_buf = msp_malloc(OUTPUT_BUF_SIZE);

    if (output_buf == NULL){
        printf("malloc failed!\r\n");
        g_start_run = 0;
        msp_task_exit(0);
    }
    printf("init\r\n");

    dma_hdl_out = msp_zalloc_check(sizeof(xcodec_dma_ch_t)); 
    //dma_hdl_out->ctrl_id = DMA_NUM;
    //dma_hdl_out->ch_id = DMA_CHANNEL;

    codec_output_ch.dma = dma_hdl_out;
    codec_output_ch.sound_channel_num = 1;
    codec_output_ch.ring_buf = &loop_output_ring_buffer;
    xcodec_output_open(&codec, &codec_output_ch, 0);
    /* output ch config */
    xcodec_output_attach_callback(&codec_output_ch, codec_output_event_cb_fun, NULL);

    output_config.bit_width = 16;
    output_config.sample_rate = sample_rate;
    output_config.buffer = output_buf;
    output_config.buffer_size = OUTPUT_BUF_SIZE;
    output_config.period = OUTPUT_PERIOD;
    output_config.mode = XCODEC_OUTPUT_DIFFERENCE;
    output_config.sound_channel_num = 1;
    //output_config.negative_pin = OUTPUT_NEGATIVE_PIN;
    //output_config.positive_pin = OUTPUT_POSITIVE_PIN;
    //output_config.pa_pin = 4;
    xcodec_output_config(&codec_output_ch, &output_config);
    xcodec_output_analog_gain(&codec_output_ch, 0xffad);
    xcodec_output_buffer_reset(&codec_output_ch);

    xcodec_output_link_dma(&codec_output_ch, dma_hdl_out);
}

static void _codec_output_stop(void)
{
    xcodec_output_stop(&codec_output_ch);
    xcodec_output_link_dma(&codec_output_ch, NULL);
    xcodec_output_detach_callback(&codec_output_ch);
    xcodec_output_close(&codec_output_ch);
    xcodec_uninit(&codec);
}
static void _codec_output_task_sin(void *arg)
{
    uint32_t res = 0, output_num = 0;
    
    _codec_output_init(16000); 
    xcodec_output_start(&codec_output_ch);
    
    while (output_num < sin_1khz_16k_sample_0db_pcm_len) {
        res = xcodec_output_write_async(&codec_output_ch, sin_1khz_16k_sample_0db_pcm + output_num, sin_1khz_16k_sample_0db_pcm_len - output_num);
        msp_msleep(5);
        output_num += res;
        //printf("res:%d, output_num:%d, src:%x\r\n", res, output_num, *(uint32_t *)0x2000C100);
    }
    printf("stop code output\r\n");
    _codec_output_stop();    
    
    output_num = 0;
    _codec_output_init(32000); 
    xcodec_output_start(&codec_output_ch);
    
    while (output_num < sin_1khz_16k_sample_0db_pcm_len) {
        res = xcodec_output_write_async(&codec_output_ch, sin_1khz_16k_sample_0db_pcm + output_num, sin_1khz_16k_sample_0db_pcm_len - output_num);
        msp_msleep(5);
        output_num += res;
        //printf("res:%d, output_num:%d, src:%x\r\n", res, output_num, *(uint32_t *)0x2000C100);
    }
    printf("stop code output\r\n");
    _codec_output_stop();    
   
    msp_free(output_buf);
    msp_free(dma_hdl_out);
    g_start_run = 0;
    msp_task_exit(0);
}

#ifdef CONFIG_SHELL
#include <shell.h>
int cmd_codec_speaker_sin(int argc, char **argv)
{
    if (g_start_run) {
        printf("output task is running\r\n");
        return 0;
    }
    msp_task_new("output_task_sin", _codec_output_task_sin, NULL, 4096);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_codec_speaker_sin, codec_speaker_sin, codec speaker test);
#endif

