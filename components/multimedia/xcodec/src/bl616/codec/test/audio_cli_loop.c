#if 0
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
#include <xutils/xringbuffer.h>

#include "xcodec.h"
#include "codec_output.h"
#include "codec_input.h"

#define CODEC_OUTPUT_ENABLE        (1)
#define CODEC_INPUT_ENABLE         (1)

// output + input
static xcodec_t g_codec;

#if CODEC_OUTPUT_ENABLE
// output
#define HW_OUTBUF_SIZE      (40*1024)
// static uint8_t              *g_audio_outbuf = NULL;
// static uint32_t             g_audio_outsize = 0;

static uint8_t              *g_hw_outbuf;

static xcodec_output_t   g_cooutch;
static xcodec_dma_ch_t      g_dmachoutput_handle;
static x_ringbuf_t           output_ring_buffer;
#endif

#if CODEC_INPUT_ENABLE
// input
#define HW_INBUF_SIZE       (40*1024)
static uint8_t              *g_hw_input;
static xcodec_output_t   g_coinch;
static xcodec_dma_ch_t      g_dmachinput_handle;
static x_ringbuf_t           input_ring_buffer;
uint8_t s_tmpbuf[4096];
#endif

#if CODEC_OUTPUT_ENABLE
static void _codec_output_event_cb(xcodec_output_t *output, xcodec_event_t event, void *arg)
{
    if (event == XCODEC_EVENT_PERIOD_WRITE_COMPLETE) {
        //printf("++++++++++++++++++ output CODEC_EVENT_PERIOD_WRITE_COMPLETE\r\n");
    }

    if (event == XCODEC_EVENT_WRITE_BUFFER_EMPTY) {
        //printf("++++++++++++++++++ output CODEC_EVENT_WRITE_BUFFER_EMPTY\r\n");
    }
}
#endif

#if CODEC_INPUT_ENABLE
static void codec_input_event_cb_fun(xcodec_input_t *i2s, xcodec_event_t event, void *arg)
{
    if (event == XCODEC_EVENT_PERIOD_READ_COMPLETE) {
        //printf("++++++++++++++++++ input CODEC_EVENT_PERIOD_READ_COMPLETE\r\n");
    }
}
#endif

static int _codec_auio_test(uint32_t timeout)
{
#if CODEC_OUTPUT_ENABLE
    // output
    xcodec_output_config_t output_config;
    uint32_t write_len, tmp;
#endif
    xcodec_error_t ret;
    uint32_t tmp_offset = 0;
    uint32_t time_end = 0;

#if CODEC_INPUT_ENABLE
    // input
    xcodec_input_config_t input_config;
    uint32_t read_len;
#endif

    // output + input
    ret = xcodec_init(&g_codec, 0);
    if (ret != XCODEC_OK) {
        printf("xcodec_init error = %d\r\n", ret);
        return -1;
    }

#if CODEC_OUTPUT_ENABLE
    g_cooutch.ring_buf = &output_ring_buffer;
    g_cooutch.sound_channel_num = 2;
    xcodec_output_open(&g_codec, &g_cooutch, 0);
    xcodec_output_attach_callback(&g_cooutch, _codec_output_event_cb, NULL);

    output_config.bit_width = 16;
    output_config.sample_rate = 16000;
    output_config.buffer = g_hw_outbuf;
    output_config.buffer_size = HW_OUTBUF_SIZE;
    output_config.period = 2400;
    output_config.mode = XCODEC_OUTPUT_DIFFERENCE;
    output_config.sound_channel_num = 2;
    xcodec_output_config(&g_cooutch, &output_config);
    xcodec_output_analog_gain(&g_cooutch, 0x0);   ///< 设置声音大小0x2f
    xcodec_output_digital_gain(&g_cooutch, 0xffad); // 0xffad
    //xcodec_output_buffer_reset(&g_cooutch);
    xcodec_output_link_dma(&g_cooutch, &g_dmachoutput_handle);
    //xcodec_output_start(&g_cooutch);
#endif

#if CODEC_INPUT_ENABLE
    // input
    g_coinch.ring_buf = &input_ring_buffer;
    g_coinch.sound_channel_num = 2;
    xcodec_input_open(&g_codec, &g_coinch, 0);
    /* input ch config */
    xcodec_input_attach_callback(&g_coinch, codec_input_event_cb_fun, NULL);

    input_config.bit_width = 16;
    input_config.sample_rate = 16000;
    input_config.buffer = g_hw_input;
    input_config.buffer_size = HW_INBUF_SIZE;
    input_config.period = 2400;
    input_config.mode = XCODEC_INPUT_DIFFERENCE;
    input_config.sound_channel_num = 2;
    xcodec_input_config(&g_coinch, &input_config);
    xcodec_input_analog_gain(&g_coinch, 0x0);

    //xcodec_input_analog_gain(&codec_input_ch, 0x8);
    //xcodec_input_digital_gain(&codec_input_ch, 25);

    xcodec_input_link_dma(&g_coinch, &g_dmachinput_handle);

    xcodec_input_start(&g_coinch);
#endif

    time_end = timeout + msp_now_ms(); ///< playback 60S

    while (1) {

        read_len = xcodec_input_read_async(&g_coinch, s_tmpbuf, sizeof(s_tmpbuf));
        if (read_len) {
            tmp = xcodec_output_write_async(&g_cooutch, (void *)s_tmpbuf, read_len);
        }

        if (msp_now_ms() > time_end) {
            break;
        }
        bl_os_msleep(1);
    }

#if CODEC_OUTPUT_ENABLE
    // output
    xcodec_output_stop(&g_cooutch);
    xcodec_output_link_dma(&g_cooutch, NULL);
    xcodec_output_detach_callback(&g_cooutch);
    xcodec_output_close(&g_cooutch);
#endif

#if CODEC_INPUT_ENABLE
    // input
    xcodec_input_stop(&g_coinch);
    xcodec_input_link_dma(&g_coinch, NULL);
    xcodec_input_detach_callback(&g_coinch);
    xcodec_input_close(&g_coinch);

    xcodec_uninit(&g_codec);
#endif

    return 0;
}

static void audio_auio_task(void *arg)
{
    int ret;
    g_hw_outbuf = msp_malloc(HW_OUTBUF_SIZE);
    g_hw_input = msp_malloc(HW_INBUF_SIZE);
    if ((NULL == g_hw_outbuf) || (NULL == g_hw_input)) {
        printf("mem fail\r\n");
        return;
    }
    ret = _codec_auio_test(60000);
    if (ret < 0) {
        printf("test_codec fail\r\n");
        return -1;
    }
}

int ng_auio_cmd(char *buf, int len, int argc, char **argv)
{
    aos_task_new("auiotest", audio_auio_task, NULL, 4096);
}
#endif
