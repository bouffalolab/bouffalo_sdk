
#ifdef BLYOC_CSI_AUDIO_TEST_TX
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <xutils/types.h>
#include <xutils/debug.h>
#include <xutils/list.h>
#include <xutils/xringbuffer.h>
#include <msp/kernel.h>

#include "drv/codec.h"
#include "drv/dma.h"
#include "drv/tick.h"
#include "drv/ringbuf.h"
#include  "blyoc_audio.h"

static csi_codec_t codec;
static csi_codec_output_t codec_output_ch;
static csi_codec_input_t  codec_input_ch;


static void dma_ch_output_handle;
static void dma_ch_input_handle;
#define OUTPUT_BUF_SIZE 48000
static uint8_t output_buf[OUTPUT_BUF_SIZE];

#define INPUT_BUF_SIZE  OUTPUT_BUF_SIZE
static uint8_t input_buf[INPUT_BUF_SIZE];

static xringbuf_t output_ring_buffer;
static xringbuf_t input_ring_buffer;

static volatile uint32_t cb_output_transfer_flag = 0;
static volatile uint32_t cb_input_transfer_flag = 1;

static uint8_t read_data[INPUT_BUF_SIZE / 2];

static void codec_output_event_cb_fun(csi_codec_output_t *output, csi_codec_event_t event, void *arg)
{
    if (event == CODEC_EVENT_PERIOD_WRITE_COMPLETE) {
        cb_output_transfer_flag += 1;
    }
}

static void codec_input_event_cb_fun(csi_codec_input_t *i2s, csi_codec_event_t event, void *arg)
{
    if (event == CODEC_EVENT_PERIOD_READ_COMPLETE) {
        cb_input_transfer_flag += 1;
    }
}

static int test_codec(void)
{
    csi_error_t ret;
    csi_codec_output_config_t output_config;
    csi_codec_input_config_t input_config;

    ret = csi_codec_init(&codec, 0);
    if (ret != CSI_OK) {
        MSP_LOGE("csi_codec_init error\n");
        return -1;
    }

    codec_output_ch.ring_buf = &output_ring_buffer;
    codec_output_ch.sound_channel_num = 2;
    csi_codec_output_open(&codec, &codec_output_ch, 0);
    /* output ch config */
    csi_codec_output_attach_callback(&codec_output_ch, codec_output_event_cb_fun, NULL);

    output_config.bit_width = 16;
    output_config.sample_rate = 16000;
    output_config.buffer = output_buf;
    output_config.buffer_size = OUTPUT_BUF_SIZE;
    output_config.period = OUTPUT_BUF_SIZE / 2;
    output_config.mode = CODEC_OUTPUT_SINGLE_ENDED;
    output_config.sound_channel_num = 2;
    csi_codec_output_config(&codec_output_ch, &output_config);
    csi_codec_output_analog_gain(&codec_output_ch, 0xffad);
    csi_codec_output_buffer_reset(&codec_output_ch);

    csi_codec_output_link_dma(&codec_output_ch, &dma_ch_output_handle);


    codec_input_ch.ring_buf = &input_ring_buffer;
    codec_input_ch.sound_channel_num = 2;
    csi_codec_input_open(&codec, &codec_input_ch, 0);
    /* input ch config */
    csi_codec_input_attach_callback(&codec_input_ch, codec_input_event_cb_fun, NULL);

    input_config.bit_width = 16;
    input_config.sample_rate = 16000;
    input_config.buffer = input_buf;
    input_config.buffer_size = INPUT_BUF_SIZE;
    input_config.period = INPUT_BUF_SIZE / 2;
    input_config.mode = CODEC_INPUT_DIFFERENCE;
    input_config.sound_channel_num = 2;
    csi_codec_input_config(&codec_input_ch, &input_config);
    csi_codec_input_analog_gain(&codec_input_ch, 0x0);
    csi_codec_input_link_dma(&codec_input_ch, &dma_ch_input_handle);

    MSP_LOGI("start code input and output\n");

    csi_codec_output_start(&codec_output_ch);
    csi_codec_input_start(&codec_input_ch);
    uint32_t end_time = 60000 + bl_os_clock_gettime_ms(); ///< playback 60S

    volatile uint8_t new_data_flag = 1U;

    MSP_LOGI("read and write\n");
    while (1) {
        if (cb_input_transfer_flag) {
            csi_codec_input_read_async(&codec_input_ch, read_data, sizeof(read_data));
            cb_input_transfer_flag = 0U;
            new_data_flag = 1U;
        }

        if (new_data_flag) {
            csi_codec_output_write_async(&codec_output_ch, read_data, sizeof(read_data));
            new_data_flag = 0U;
            cb_output_transfer_flag = 0U;
        }

        if (bl_os_clock_gettime_ms() > end_time) {
            break;
        }
    }

    MSP_LOGI("stop code input and output\n");
    csi_codec_input_stop(&codec_input_ch);
    csi_codec_output_stop(&codec_output_ch);

    csi_codec_input_link_dma(&codec_input_ch, NULL);
    csi_codec_output_link_dma(&codec_output_ch, NULL);

    csi_codec_output_detach_callback(&codec_output_ch);
    csi_codec_input_detach_callback(&codec_input_ch);

    csi_codec_uninit(&codec);

    return 0;
}

void _audio_loop_task(void *arg)
{
    int ret;

    ret = test_codec();
    if (ret < 0) {
        MSP_LOGE("test_codec fail\n");
    }
}

int ng_aul_cmd(char *buf, int len, int argc, char **argv)
{
    msp_task_new("aulcli", _audio_loop_task, NULL, 4096);

    return 0;
}

#endif