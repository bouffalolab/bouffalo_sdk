
#ifdef BLYOC_CSI_AUDIO_TEST_TX
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <xutils/types.h>
#include <xutils/debug.h>
#include <xutils/list.h>
// #include <xutils/xringbuffer.h>
#include <msp/kernel.h>

#include "semaphore.h"
#include "drv/codec.h"
#include "drv/dma.h"
#include "drv/tick.h"
#include "drv/ringbuf.h"
#include "blyoc_audio.h"
#include  "wa_all.h"

#define SAVE_FILE_SIZE     (512*1024)

// output + input
static csi_codec_t g_codec;

// input
#define HW_INBUF_SIZE       (40*1200)

static uint8_t              *g_hw_input = NULL;
static csi_codec_output_t   g_coinch;
static void         g_dmachinput_handle;
static xringbuf_t        input_ring_buffer;

static void codec_input_event_cb_fun(csi_codec_input_t *i2s, csi_codec_event_t event, void *arg)
{
    if (event == CODEC_EVENT_PERIOD_READ_COMPLETE) {
        //MSP_LOGD("++++++++++++++++++ input CODEC_EVENT_PERIOD_READ_COMPLETE\r\n");
    }
}

uint32_t g_save_len = 0;
int _save_psram(uint8_t *buf, uint32_t buf_size)
{
    int ret = 0;
    uint32_t real_len = 0;

    if ((g_save_len + buf_size) >= SAVE_FILE_SIZE) {
        ret = 1;
        real_len = SAVE_FILE_SIZE - g_save_len;
    } else {
        real_len = buf_size;
    }

    MSP_LOGD("real_len = %ld, ret = %d\r\n", real_len, ret);
    if (real_len) {
        memcpy((void *)(0x54200000 + g_save_len), buf, real_len);
        g_save_len += real_len;
    }

    return ret;
}

static int _codec_input_test(uint32_t timeout)
{
    csi_error_t ret;
    uint32_t tmp_offset = 0;
    uint32_t time_end = 0;
    uint8_t s_tmpbuf[2048];

    // input
    csi_codec_input_config_t input_config;
    uint32_t read_len;

    // output + input
    ret = csi_codec_init(&g_codec, 0);
    if (ret != CSI_OK) {
        MSP_LOGE("csi_codec_init error = %d\r\n", ret);
        return -1;
    }

    // input
    g_coinch.ring_buf = &input_ring_buffer;
    g_coinch.sound_channel_num = 3;
    csi_codec_input_open(&g_codec, &g_coinch, 0);
    /* input ch config */
    csi_codec_input_attach_callback(&g_coinch, codec_input_event_cb_fun, NULL);

    input_config.bit_width = 16;
    input_config.sample_rate = 16000;
    input_config.buffer = g_hw_input;
    input_config.buffer_size = HW_INBUF_SIZE;
    input_config.period = 1200;
    input_config.mode = CODEC_INPUT_DIFFERENCE;
    input_config.sound_channel_num = 3;
    csi_codec_input_config(&g_coinch, &input_config);
    csi_codec_input_analog_gain(&g_coinch, 0x0);

    //csi_codec_input_analog_gain(&codec_input_ch, 0x8);
    //csi_codec_input_digital_gain(&codec_input_ch, 25);

    csi_codec_input_link_dma(&g_coinch, &g_dmachinput_handle);

    csi_codec_input_start(&g_coinch);

    time_end = timeout + bl_os_clock_gettime_ms(); ///< playback 60S

    g_save_len = 0;
    while (1) {
        // input
        read_len = csi_codec_input_buffer_avail(&g_coinch);
        if (read_len > sizeof(s_tmpbuf)) {
            read_len = sizeof(s_tmpbuf);
        }
        if (read_len) {
            MSP_LOGI("read_len = %ld, g_save_len = %ld\r\n", read_len, g_save_len);
            csi_codec_input_read_async(&g_coinch, s_tmpbuf, read_len);
            if(_save_psram(s_tmpbuf, read_len)) {
                MSP_LOGI("save complete\r\n");
                break;
            }
        }

        if (bl_os_clock_gettime_ms() > time_end) {
            break;
        }
        bl_os_msleep(1);
    }

    // input
    csi_codec_input_stop(&g_coinch);
    csi_codec_input_link_dma(&g_coinch, NULL);
    csi_codec_input_detach_callback(&g_coinch);
    csi_codec_input_close(&g_coinch);

    csi_codec_uninit(&g_codec);

    return 0;
}

static void _audio_input_task(void *arg)
{
    int ret;

    ret = _codec_input_test(60000);
    if (ret < 0) {
        MSP_LOGE("test_codec fail\n");
        return -1;
    }

    if (g_hw_input) {
        bl_os_free(g_hw_input);
        g_hw_input = NULL;
    }

    msp_task_exit(0);
}

int ng_aui_cmd(char *buf, int len, int argc, char **argv)
{
    g_hw_input = bl_os_malloc(HW_INBUF_SIZE);
    if (NULL == g_hw_input) {
        return -1;
    }

    msp_task_new("auicli", _audio_input_task, NULL, 8192);

    return 0;
}


#endif