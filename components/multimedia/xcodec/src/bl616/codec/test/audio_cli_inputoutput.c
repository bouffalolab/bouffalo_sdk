#if 0//def BLYOC_CSI_AUDIO_TEST_TX
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <aos/aos.h>
#include "semaphore.h"
#include "drv/codec.h"
#include "drv/dma.h"
#include "drv/tick.h"
#include "drv/ringbuf.h"
#include "blyoc_audio.h"
#include  "wa_all.h"

#define CODEC_OUTPUT_ENABLE        (1)
#define CODEC_INPUT_ENABLE         (1)

// output + input
static csi_codec_t g_codec;

#if CODEC_OUTPUT_ENABLE
// output
#define HW_OUTBUF_SIZE      (40*1024)
static uint8_t              *g_audio_outbuf = NULL;
static uint32_t             g_audio_outsize = 0;

static uint8_t              *g_hw_outbuf;

static csi_codec_output_t   g_cooutch;
static void         g_dmachoutput_handle;
static xringbuf_t        output_ring_buffer;
#endif

#if CODEC_INPUT_ENABLE
// input
#define HW_INBUF_SIZE       (40*1024)
static uint8_t              *g_hw_input;
static csi_codec_output_t   g_coinch;
static void         g_dmachinput_handle;
static xringbuf_t        input_ring_buffer;
uint8_t s_tmpbuf[4096];

#endif

#if CODEC_OUTPUT_ENABLE
static void _codec_output_event_cb(csi_codec_output_t *output, csi_codec_event_t event, void *arg)
{
    if (event == CODEC_EVENT_PERIOD_WRITE_COMPLETE) {
        //printf("++++++++++++++++++ output CODEC_EVENT_PERIOD_WRITE_COMPLETE\r\n");
    }

    if (event == CODEC_EVENT_WRITE_BUFFER_EMPTY) {
        //printf("++++++++++++++++++ output CODEC_EVENT_WRITE_BUFFER_EMPTY\r\n");
    }
}
#endif

#if CODEC_INPUT_ENABLE
static void codec_input_event_cb_fun(csi_codec_input_t *i2s, csi_codec_event_t event, void *arg)
{
    if (event == CODEC_EVENT_PERIOD_READ_COMPLETE) {
        //printf("++++++++++++++++++ input CODEC_EVENT_PERIOD_READ_COMPLETE\r\n");
    }
}
#endif

int save_psram(uint8_t *buf, uint32_t buf_size)
{
    static uint32_t s_save_len = 0;

    if ((s_save_len + buf_size) >= 0x200000) {
        return 1;
    }

    memcpy((void *)(0x54200000 + s_save_len), buf, buf_size);
    return 0;
}

static int _codec_auio_test(uint32_t timeout)
{
#if CODEC_OUTPUT_ENABLE
    // output
    csi_codec_output_config_t output_config;
    uint32_t write_len, tmp;
#endif
    csi_error_t ret;
    uint32_t tmp_offset = 0;
    uint32_t time_end = 0;

#if CODEC_INPUT_ENABLE
    // input
    csi_codec_input_config_t input_config;
    uint32_t read_len;
#endif

    // output + input
    ret = csi_codec_init(&g_codec, 0);
    if (ret != CSI_OK) {
        printf("csi_codec_init error = %d\r\n", ret);
        return -1;
    }

#if CODEC_OUTPUT_ENABLE
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
    output_config.sound_channel_num = 2;
    csi_codec_output_config(&g_cooutch, &output_config);
    csi_codec_output_analog_gain(&g_cooutch, 0x0);   ///< 设置声音大小0x2f
    csi_codec_output_digital_gain(&g_cooutch, 0xffad); // 0xffad
    //csi_codec_output_buffer_reset(&g_cooutch);
    csi_codec_output_link_dma(&g_cooutch, &g_dmachoutput_handle);
    //csi_codec_output_start(&g_cooutch);
#endif

#if CODEC_INPUT_ENABLE
    // input
    g_coinch.ring_buf = &input_ring_buffer;
    g_coinch.sound_channel_num = 2;
    csi_codec_input_open(&g_codec, &g_coinch, 0);
    /* input ch config */
    csi_codec_input_attach_callback(&g_coinch, codec_input_event_cb_fun, NULL);

    input_config.bit_width = 16;
    input_config.sample_rate = 16000;
    input_config.buffer = g_hw_input;
    input_config.buffer_size = HW_INBUF_SIZE;
    input_config.period = 2400;
    input_config.mode = CODEC_INPUT_DIFFERENCE;
    input_config.sound_channel_num = 2;
    csi_codec_input_config(&g_coinch, &input_config);
    csi_codec_input_analog_gain(&g_coinch, 0x0);

    //csi_codec_input_analog_gain(&codec_input_ch, 0x8);
    //csi_codec_input_digital_gain(&codec_input_ch, 25);

    csi_codec_input_link_dma(&g_coinch, &g_dmachinput_handle);

    csi_codec_input_start(&g_coinch);
#endif

    time_end = timeout + bl_os_clock_gettime_ms(); ///< playback 60S

#if CODEC_OUTPUT_ENABLE
    // output
    g_audio_outbuf  = wa_16K_16bit_stero_pcm;
    g_audio_outsize = wa_16K_16bit_stero_pcm_len;
#endif

    while (1) {
#if 1
#if CODEC_OUTPUT_ENABLE
        // output
        write_len = csi_codec_output_buffer_avail(&g_cooutch);

        //printf("************************get_write_len = %d\r\n", write_len);
        if ((write_len + tmp_offset) > g_audio_outsize) {
            write_len = g_audio_outsize - tmp_offset;
        }

        if (write_len) {
            tmp = csi_codec_output_write_async(&g_cooutch, (void *)(g_audio_outbuf + tmp_offset), write_len);
            if (tmp != write_len) {
                printf("error, never here\r\n");
                return;
            }
            tmp_offset += write_len;
        }
        if ( tmp_offset == g_audio_outsize) {
            //bl_os_msleep(1000);
            tmp_offset = 0;
        }
#endif

#if CODEC_INPUT_ENABLE
        // input
        read_len = csi_codec_input_buffer_avail(&g_coinch);
        if (read_len) {
            //printf("read_len = %ld\r\n", read_len);
            csi_codec_input_read_async(&g_coinch, NULL, read_len);
        }
#endif
#else

#if CODEC_OUTPUT_ENABLE
#if CODEC_INPUT_ENABLE
#if 0
        read_len = csi_codec_input_read_async(&g_coinch, s_tmpbuf, sizeof(s_tmpbuf));
        if (read_len) {
            tmp = csi_codec_output_write_async(&g_cooutch, (void *)s_tmpbuf, read_len);
        }
#else
        read_len = csi_codec_input_read_async(&g_coinch, s_tmpbuf, sizeof(s_tmpbuf));
        if (read_len) {
            if (save_psram(s_tmpbuf, read_len)) {
                printf("save complete.\r\n");
                break;
            }
        }
#endif
#endif
#endif

#endif
        if (bl_os_clock_gettime_ms() > time_end) {
            break;
        }
        bl_os_msleep(1);
    }

#if CODEC_OUTPUT_ENABLE
    // output
    csi_codec_output_stop(&g_cooutch);
    csi_codec_output_link_dma(&g_cooutch, NULL);
    csi_codec_output_detach_callback(&g_cooutch);
    csi_codec_output_close(&g_cooutch);
#endif

#if CODEC_INPUT_ENABLE
    // input
    csi_codec_input_stop(&g_coinch);
    csi_codec_input_link_dma(&g_coinch, NULL);
    csi_codec_input_detach_callback(&g_coinch);
    csi_codec_input_close(&g_coinch);

    csi_codec_uninit(&g_codec);
#endif

    return 0;
}

static void audio_auio_task(void *arg)
{
    int ret;
    g_hw_outbuf = bl_os_malloc(HW_OUTBUF_SIZE);
    g_hw_input = bl_os_malloc(HW_INBUF_SIZE);
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
