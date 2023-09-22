/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <msp/kernel.h>

#include "xcodec.h"
#include "codec_output.h"
#include <xutils/xringbuffer.h>

// output + input
static xcodec_t g_codec;

// output
#define HW_OUTBUF_SIZE      (40*1024)

static uint8_t          *g_audio_outbuf = NULL;
static uint32_t         g_audio_outsize = 0;
static uint32_t         g_audio_pa_pin = 0;
static uint32_t         g_audio_play_times = 1;

static uint8_t          *g_hw_outbuf = NULL;

static xcodec_output_t  g_cooutch;
static xcodec_dma_ch_t  g_dmachoutput_handle;
static x_ringbuf_t      output_ring_buffer;

static void _codec_output_event_cb(xcodec_output_t *output, xcodec_event_t event, void *arg)
{
    if (event == XCODEC_EVENT_PERIOD_WRITE_COMPLETE) {
        //printf("++++++++++++++++++ output CODEC_EVENT_PERIOD_WRITE_COMPLETE\r\n");
    }

    if (event == XCODEC_EVENT_WRITE_BUFFER_EMPTY) {
        //printf("++++++++++++++++++ output CODEC_EVENT_WRITE_BUFFER_EMPTY\r\n");
    }
}

static int _codec_auo_test(
    uint32_t timeout,   // unused, invalid arg
    uint32_t play_times // used
    )
{
    // output
    xcodec_output_config_t output_config;
    uint32_t write_len, tmp;

    xcodec_error_t ret;
    uint32_t tmp_offset = 0;
    // uint32_t time_end = 0;

    /* set pa pin */
    g_cooutch.pa_pin = g_audio_pa_pin;
    g_codec.output_chs = &g_cooutch;

    // output + input
    ret = xcodec_init(&g_codec, 0);
    if (ret != XCODEC_OK) {
        printf("xcodec_init error = %d\r\n", ret);
        return -1;
    }

    g_cooutch.ring_buf = &output_ring_buffer;
    g_cooutch.sound_channel_num = 2;

    xcodec_output_open(&g_codec, &g_cooutch, 0);
    xcodec_output_attach_callback(&g_cooutch, _codec_output_event_cb, NULL);

    output_config.bit_width = 16;
    output_config.sample_rate = 16000;
    output_config.buffer = g_hw_outbuf;
    output_config.buffer_size = HW_OUTBUF_SIZE;
    output_config.period = 1024;
    output_config.mode = XCODEC_OUTPUT_DIFFERENCE;
    output_config.sound_channel_num = 1;
    output_config.pa_pin = 4;
    xcodec_output_config(&g_cooutch, &output_config);
    // xcodec_output_analog_gain(&g_cooutch, 0x0);   ///< 设置声音大小0x2f
    // xcodec_output_digital_gain(&g_cooutch, 0xffad); // 0xffad
    //xcodec_output_buffer_reset(&g_cooutch);

    g_dmachoutput_handle.ctrl_id = 0;
    g_dmachoutput_handle.ch_id = 0;
    xcodec_output_link_dma(&g_cooutch, &g_dmachoutput_handle);
    //xcodec_output_start(&g_cooutch);

    // time_end = timeout + msp_now_ms(); ///< playback 60S

    while (1) {
        // output
        write_len = xcodec_output_buffer_avail(&g_cooutch);

        // printf("************************get_write_len = %d\r\n", write_len);
        if ((write_len + tmp_offset) > g_audio_outsize) {
            write_len = g_audio_outsize - tmp_offset;
        }

        if (write_len) {
            tmp = xcodec_output_write_async(&g_cooutch,
                                            (void *)(g_audio_outbuf + tmp_offset),
                                            write_len);
            if (tmp != write_len) {
                // printf("warning, never here %d,%d\r\n", tmp, write_len);
                // return;
            }
            tmp_offset += tmp;
        }
        // printf("tmp_offset:%d, g_audio_outsize:%d\r\n", tmp_offset, g_audio_outsize);
        if (tmp_offset == g_audio_outsize) {
            if (play_times != MSP_WAIT_FOREVER) {
                if (0 == (--play_times)) {
                    // msp_msleep(50);// delay in auo_stop
                    break;
                }
            }
            tmp_offset = 0;
        }

        // if (msp_now_ms() > time_end) {
        //     break;
        // }
        msp_msleep(1);
    }

    // fixme, need remove, add drain.
    msp_msleep(300);
    // output
    xcodec_output_stop(&g_cooutch);
    xcodec_output_link_dma(&g_cooutch, NULL);
    xcodec_output_detach_callback(&g_cooutch);
    xcodec_output_close(&g_cooutch);

    return 0;
}

static void _audio_output_task(void *arg)
{
    int ret;

    g_hw_outbuf = msp_malloc(HW_OUTBUF_SIZE);
    if (NULL == g_hw_outbuf) {
        return;
    }

    ret = _codec_auo_test(10000, g_audio_play_times);
    if (ret < 0) {
        printf("test_codec fail\n");
        return;
    }

    if (g_hw_outbuf) {
        msp_free(g_hw_outbuf);
        g_hw_outbuf = NULL;
    }

    msp_task_exit(0);
}

int codec_test_output(
    uint8_t *buf, uint32_t buf_size,
    uint32_t pa_pin,
    uint32_t play_times
    )
{
    g_audio_outbuf = buf;
    g_audio_outsize = buf_size;

    g_audio_pa_pin = pa_pin;
    g_audio_play_times = play_times;

    msp_task_new("auocli", _audio_output_task, NULL, 4096);

    return 0;
}
