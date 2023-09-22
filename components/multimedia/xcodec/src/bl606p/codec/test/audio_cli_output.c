
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

// output + input
static csi_codec_t g_codec;

// output
#define HW_OUTBUF_SIZE      (40*1024)

static uint8_t              *g_audio_outbuf = NULL;
static uint32_t             g_audio_outsize = 0;

static uint8_t              *g_hw_outbuf = NULL;

static csi_codec_output_t   g_cooutch;
static void         g_dmachoutput_handle;
static xringbuf_t        output_ring_buffer;

static void _codec_output_event_cb(csi_codec_output_t *output, csi_codec_event_t event, void *arg)
{
    if (event == CODEC_EVENT_PERIOD_WRITE_COMPLETE) {
        //MSP_LOGD("++++++++++++++++++ output CODEC_EVENT_PERIOD_WRITE_COMPLETE\r\n");
    }

    if (event == CODEC_EVENT_WRITE_BUFFER_EMPTY) {
        //MSP_LOGD("++++++++++++++++++ output CODEC_EVENT_WRITE_BUFFER_EMPTY\r\n");
    }
}

static int _codec_auo_test(uint32_t timeout)
{
    // output
    csi_codec_output_config_t output_config;
    uint32_t write_len, tmp;

    csi_error_t ret;
    uint32_t tmp_offset = 0;
    uint32_t time_end = 0;


    // output + input
    ret = csi_codec_init(&g_codec, 0);
    if (ret != CSI_OK) {
        MSP_LOGE("csi_codec_init error = %d\r\n", ret);
        return -1;
    }

    g_cooutch.ring_buf = &output_ring_buffer;
    g_cooutch.sound_channel_num = 2;
    csi_codec_output_open(&g_codec, &g_cooutch, 0);
    csi_codec_output_attach_callback(&g_cooutch, _codec_output_event_cb, NULL);

    output_config.bit_width = 16;
    output_config.sample_rate = 16000;
    output_config.buffer = g_hw_outbuf;
    output_config.buffer_size = HW_OUTBUF_SIZE;
    output_config.period = 2400;
    output_config.mode = CODEC_OUTPUT_SINGLE_ENDED;
    output_config.sound_channel_num = 1;
    csi_codec_output_config(&g_cooutch, &output_config);
    csi_codec_output_analog_gain(&g_cooutch, 0x0);   ///< 设置声音大小0x2f
    csi_codec_output_digital_gain(&g_cooutch, 0xffad); // 0xffad
    //csi_codec_output_buffer_reset(&g_cooutch);
    csi_codec_output_link_dma(&g_cooutch, &g_dmachoutput_handle);
    //csi_codec_output_start(&g_cooutch);

    time_end = timeout + bl_os_clock_gettime_ms(); ///< playback 60S

    // output
    g_audio_outbuf  = wa_16K_16bit_mono_pcm;
    g_audio_outsize = wa_16K_16bit_mono_pcm_len;

    while (1) {
        // output
        write_len = csi_codec_output_buffer_avail(&g_cooutch);

        //MSP_LOGD("get_write_len = %d\r\n", write_len);
        if ((write_len + tmp_offset) > g_audio_outsize) {
            write_len = g_audio_outsize - tmp_offset;
        }

        if (write_len) {
            tmp = csi_codec_output_write_async(&g_cooutch, (void *)(g_audio_outbuf + tmp_offset), write_len);
            if (tmp != write_len) {
                MSP_LOGE("error, never here\r\n");
                return;
            }
            tmp_offset += write_len;
        }
        if ( tmp_offset == g_audio_outsize) {
            //bl_os_msleep(1000);
            tmp_offset = 0;
        }

        if (bl_os_clock_gettime_ms() > time_end) {
            break;
        }
        bl_os_msleep(1);
    }

    // output
    csi_codec_output_stop(&g_cooutch);
    csi_codec_output_link_dma(&g_cooutch, NULL);
    csi_codec_output_detach_callback(&g_cooutch);
    csi_codec_output_close(&g_cooutch);

    return 0;
}

static void _audio_output_task(void *arg)
{
    int ret;

    ret = _codec_auo_test(10000);
    if (ret < 0) {
        MSP_LOGE("test_codec fail\n");
        return -1;
    }

    if (g_hw_outbuf) {
        bl_os_free(g_hw_outbuf);
        g_hw_outbuf = NULL;
    }

    msp_task_exit(0);
}

int ng_auo_cmd(char *buf, int len, int argc, char **argv)
{
    g_hw_outbuf = bl_os_malloc(HW_OUTBUF_SIZE);
    if (NULL == g_hw_outbuf) {
        return -1;
    }

    msp_task_new("auocli", _audio_output_task, NULL, 4096);

    return 0;
}

#endif