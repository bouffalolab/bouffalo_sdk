/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if 0
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
        msp_msleep(1);
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

#include "msp_bl616_gpio.h"
#include "msp_bl616_aupwm.h"
#include "msp_bl616_auadc.h"
int codec_aupwm_pinmux_config(uint8_t pin)
{
    GLB_GPIO_Cfg_Type gpio_cfg;

    gpio_cfg.drive = 2;
    gpio_cfg.smtCtrl = 1;
    gpio_cfg.gpioMode = GPIO_MODE_AF;
    gpio_cfg.pullType = GPIO_PULL_NONE;
    gpio_cfg.gpioFun = GPIO_FUN_AUPWM;
    gpio_cfg.outputMode = 0;
    gpio_cfg.gpioPin = pin;
    ms_GLB_GPIO_Init(&gpio_cfg);

    return 0;
}

int codec_auadc_pinmux_config(uint8_t pin)
{
    GLB_GPIO_Cfg_Type gpio_cfg;

    gpio_cfg.drive = 2;
    gpio_cfg.smtCtrl = 1;
    gpio_cfg.gpioMode = GPIO_MODE_AF;
    gpio_cfg.pullType = GPIO_PULL_NONE;
    gpio_cfg.gpioFun = GPIO_FUN_ANALOG;
    gpio_cfg.outputMode = 0;
    gpio_cfg.gpioPin = pin;
    ms_GLB_GPIO_Init(&gpio_cfg);

    return 0;
}

void msp_loop_test(void)
{
    codec_auadc_pinmux_config(27);
    codec_auadc_pinmux_config(28);
    codec_aupwm_pinmux_config(14);
    codec_aupwm_pinmux_config(15);
    msp_task_new("auiotest", audio_auio_task, NULL, 4096);
}
#else

#include <msp/kernel.h>

#include "xcodec.h"
#include "codec_output.h"
#include "codec_input.h"
#include <xutils/xringbuffer.h>

/* ================================ test code ================================ */
#include "msp_bl616_glb.h"
#include "msp_bl616_gpio.h"

#define AUDIO_ALIGNMENT_MASK           (0x001f)
#define AUDIO_ALIGNMENT_BYTES          (32)

/**
 *  @define audio output config
 */
#define CSI_HW_DMA_OUTPUT_IDX          (0)
#define CSI_HW_DMA_OUTPUT_CHN          (0)

#define CSI_HW_NODE_OUTPUT_COUNT       (16)
#define CSI_HW_OUTPUT_PERIOD           (512)
#define CSI_SW_OUTPUT_PERIOD           (2 * CSI_HW_OUTPUT_PERIOD) // csi/user software use
#define CSI_DMA_NODE_OUTPUT_ITEM       (24)                // sizeof(auo_segment_t) = 24

#define HW_OUTPUTBUF_SIZE    (CSI_HW_NODE_OUTPUT_COUNT * (CSI_SW_OUTPUT_PERIOD + CSI_DMA_NODE_OUTPUT_ITEM) + 2 * AUDIO_ALIGNMENT_BYTES)
#define HW_OUTPUTBUF_DUMMY   (1 * CSI_SW_OUTPUT_PERIOD - 1)// (1 * CSI_SW_OUTPUT_PERIOD - 1) is best one

/**
 *  @define audio input config
 */
#define CSI_HW_DMA_INPUT_IDX          (0)
#define CSI_HW_DMA_INPUT_CHN          (1)

#define CSI_HW_NODE_INPUT_COUNT       (16)
#define CSI_HW_INPUT_PERIOD           (512)
#define CSI_SW_INPUT_PERIOD           (2 * CSI_HW_INPUT_PERIOD) // csi/user software use
#define CSI_DMA_NODE_INPUT_ITEM       (24)                // sizeof(auo_segment_t) = 24

#define HW_INPUTBUF_SIZE    (CSI_HW_NODE_INPUT_COUNT * (CSI_SW_INPUT_PERIOD + CSI_DMA_NODE_INPUT_ITEM) + 2 * AUDIO_ALIGNMENT_BYTES)
#define HW_INPUTBUF_DUMMY   (1 * CSI_SW_INPUT_PERIOD - 1)// (1 * CSI_SW_INPUT_PERIOD - 1) is best one
#if 0
static x_ringbuf_t output_ring_buffer;

static void _codec_output_event_cb(xcodec_output_t *output, xcodec_event_t event, void *arg)
{
    if (event == XCODEC_EVENT_PERIOD_WRITE_COMPLETE) {
        //user_log("output CODEC_EVENT_PERIOD_WRITE_COMPLETE\r\n");
    }

    if (event == XCODEC_EVENT_WRITE_BUFFER_EMPTY) {
        user_log("output CODEC_EVENT_WRITE_BUFFER_EMPTY\r\n");
    }
}

int codec_output_test(uint8_t *buf, uint32_t buf_size, uint32_t times)
{
    #if 1
    // output + input
    xcodec_t g_codec;
    // out
    xcodec_output_t g_codec_outch;
    uint8_t *g_hw_outbuf = NULL;
    uint32_t g_dmachoutput_handle;// fixme unused

    // output
    xcodec_output_config_t output_config;
    uint32_t write_len, real_w;

    xcodec_error_t ret;
    uint32_t tmp_offset = 0;

    /* set pa pin */
    g_codec_outch.pa_pin = 4;
    g_codec.output_chs = &g_codec_outch;

    printf("ok\r\n");
    // output + input
    ret = xcodec_init(&g_codec, 0);
    if (ret != XCODEC_OK) {
        printf("xcodec_init error = %d\r\n", ret);
        return -1;
    }

    // output init

    g_codec_outch.sound_channel_num = 1;
    g_codec_outch.ring_buf = &output_ring_buffer;
    xcodec_output_open(&g_codec, &g_codec_outch, 0);
    #endif
    xcodec_output_attach_callback(&g_codec_outch, _codec_output_event_cb, NULL);

    // output malloc & config
    g_hw_outbuf = msp_malloc(HW_OUTPUTBUF_SIZE);
    if (!g_hw_outbuf) {
        printf("fixme, mem error\r\n");
    }
    output_config.bit_width = 16;
    output_config.sample_rate = 16000;
    output_config.buffer = g_hw_outbuf;
    output_config.buffer_size = HW_OUTPUTBUF_SIZE;
    output_config.period = CSI_SW_OUTPUT_PERIOD;
    output_config.mode = XCODEC_OUTPUT_DIFFERENCE;
    output_config.sound_channel_num = 1;
    output_config.pa_pin = 4;
    xcodec_output_config(&g_codec_outch, &output_config);

    xcodec_dma_ch_t *dma_hdl = msp_zalloc_check(sizeof(xcodec_dma_ch_t));
    dma_hdl->ctrl_id = 0;
    dma_hdl->ch_id = 0;

    xcodec_output_link_dma(&g_codec_outch, dma_hdl);
    //xcodec_output_digital_gain(0, -40.5);//

    #if 1
    while (1) {
        // output
        write_len = xcodec_output_buffer_avail(&g_codec_outch);
        //printf("write_len:%d\r\n", write_len);
        if ((write_len + tmp_offset) > buf_size) {
            write_len = buf_size - tmp_offset;
        }

        if (write_len) {
            real_w = xcodec_output_write_async(&g_codec_outch,
                    (void *)(buf + tmp_offset), write_len);
#if 0
            if (real_w != write_len) {
                printf("error, never here\r\n");

                printf("real_w:%ld, write_len:%ld\r\n", real_w, write_len);

                msp_free(g_hw_outbuf);
                return -2;
            }
#endif
            tmp_offset += real_w;
        }
        //printf("tmp:%d, buf_size:%d\r\n", tmp_offset, buf_size);
        if ( tmp_offset == buf_size) {
            if (0 == (--times)){
                //msp_msleep(50);// delay in auo_stop
                break;
            }

            tmp_offset = 0;
        }
        msp_msleep(1);
    }

    // output
    msp_msleep(200);
    xcodec_output_stop(&g_codec_outch);
    xcodec_output_link_dma(&g_codec_outch, NULL);
    xcodec_output_detach_callback(&g_codec_outch);
    xcodec_output_close(&g_codec_outch);

    xcodec_uninit(&g_codec);

    msp_free(g_hw_outbuf);
    msp_free(dma_hdl);
    #endif
    return 0;
}

#endif

/*=====================loop code=========================*/
#include "msp_bl616_aupwm.h"
#include "msp_bl616_auadc.h"
int codec_aupwm_pinmux_config(uint8_t pin)
{
    GLB_GPIO_Cfg_Type gpio_cfg;

    gpio_cfg.drive = 2;
    gpio_cfg.smtCtrl = 1;
    gpio_cfg.gpioMode = GPIO_MODE_AF;
    gpio_cfg.pullType = GPIO_PULL_NONE;
    gpio_cfg.gpioFun = GPIO_FUN_AUPWM;
    gpio_cfg.outputMode = 0;
    gpio_cfg.gpioPin = pin;
    ms_GLB_GPIO_Init(&gpio_cfg);

    return 0;
}

int codec_auadc_pinmux_config(uint8_t pin)
{
    GLB_GPIO_Cfg_Type gpio_cfg;

    gpio_cfg.drive = 2;
    gpio_cfg.smtCtrl = 1;
    gpio_cfg.gpioMode = GPIO_MODE_AF;
    gpio_cfg.pullType = GPIO_PULL_NONE;
    gpio_cfg.gpioFun = GPIO_FUN_ANALOG;
    gpio_cfg.outputMode = 0;
    gpio_cfg.gpioPin = pin;
    ms_GLB_GPIO_Init(&gpio_cfg);

    return 0;
}

#define OUTPUT_BUF_SIZE     (21 * 1024)
#define STORE_BUF_SIZE      (75 * 1024)
#define OUTPUT_PERIOD       (16 * 1024)
#define INPUT_PERIOD    OUTPUT_PERIOD
#define PERIOD_NUM          (40)
#define INPUT_BUF_SIZE  OUTPUT_BUF_SIZE

static xcodec_t codec;
static xcodec_output_t codec_output_ch;
static xcodec_input_t  codec_input_ch;

static uint32_t dma_ch_output_handle;
static uint32_t dma_ch_input_handle;

static x_ringbuf_t loop_output_ring_buffer;
static x_ringbuf_t loop_input_ring_buffer;
//volatile uint8_t new_data_flag = 0U;

static volatile uint32_t cb_output_transfer_flag = 0;
static volatile uint32_t cb_input_transfer_flag = 1;
static uint32_t input_num = 0;

static int inti = 1;
static int out_flag = 0;
static int in_flag = 0;

static void codec_output_event_cb_fun(xcodec_output_t *output, xcodec_event_t event, void *arg)
{
    if (event == XCODEC_EVENT_PERIOD_WRITE_COMPLETE) {
        out_flag++;
        if (out_flag == 75) {
            printf("================out_flag:%d\r\n", out_flag);
            out_flag = 0;
            cb_output_transfer_flag = 0;
            cb_input_transfer_flag = 1;
            input_num = 0;
            AUPWM_Stop(AUPWM_MONO_LEFT);
            AUADC_Start();
        }
    }
}

static void codec_input_event_cb_fun(xcodec_input_t *i2s, xcodec_event_t event, void *arg)
{
    if (event == XCODEC_EVENT_PERIOD_READ_COMPLETE) {
        in_flag++;
        if (in_flag == PERIOD_NUM) {
            in_flag = 0;
            cb_input_transfer_flag = PERIOD_NUM;
            AUADC_Stop();
            //AUPWM_Start(AUPWM_MONO_LEFT);
        }
    }
}

#define XCODEC_DMA_INPUT_PER_NODE_SIZE    (1920)//20ms    //(1536)//16ms
static int test_codec(void)
{
    xcodec_error_t ret;
    xcodec_output_config_t output_config;
    xcodec_input_config_t input_config;

    codec_output_ch.pa_pin = 4;
    codec.output_chs = &codec_output_ch;

    ret = xcodec_init(&codec, 0);
    if (ret != XCODEC_OK) {
        printf("xcodec_init error\n");
        return -1;
    }

    uint8_t *output_buf = NULL;
    uint8_t *input_buf = NULL;
    uint8_t *p_buf = NULL;

    output_buf = msp_malloc(OUTPUT_BUF_SIZE);
    input_buf = msp_malloc(INPUT_BUF_SIZE);
    p_buf = msp_malloc(STORE_BUF_SIZE);

    if (p_buf == NULL || output_buf == NULL || input_buf == NULL) {
        printf("malloc failed!\r\n");
        return -1;
    }

    codec_auadc_pinmux_config(27);
    codec_auadc_pinmux_config(28);
    codec_aupwm_pinmux_config(14);
    codec_aupwm_pinmux_config(15);
    //codec_output_ch.ring_buf = &output_ring_buffer;
    codec_output_ch.sound_channel_num = 1;
    codec_output_ch.ring_buf = &loop_output_ring_buffer;
    xcodec_output_open(&codec, &codec_output_ch, 0);
    /* output ch config */
    xcodec_output_attach_callback(&codec_output_ch, codec_output_event_cb_fun, NULL);

    output_config.bit_width = 16;
    output_config.sample_rate = 16000;
    output_config.buffer = output_buf;
    output_config.buffer_size = HW_OUTPUTBUF_SIZE;
    output_config.period = CSI_SW_OUTPUT_PERIOD;
    output_config.mode = XCODEC_OUTPUT_SINGLE_ENDED;
    output_config.sound_channel_num = 1;
    output_config.pa_pin = 4;

    xcodec_output_config(&codec_output_ch, &output_config);
    xcodec_output_analog_gain(&codec_output_ch, 0xffad);
    xcodec_output_buffer_reset(&codec_output_ch);

    xcodec_dma_ch_t *dma_hdl_out = msp_zalloc_check(sizeof(xcodec_dma_ch_t));
    dma_hdl_out->ctrl_id = 0;
    dma_hdl_out->ch_id = 0;

    xcodec_output_link_dma(&codec_output_ch, dma_hdl_out);
    /*stop*/
    //DMA_Channel_Disable(dma_hdl_out->ctrl_id, dma_hdl_out->ch_id);
    AUPWM_Stop(AUPWM_MONO_LEFT);
printf("input\r\n");
    codec.input_chs = &codec_input_ch;
    codec_input_ch.ring_buf = &loop_input_ring_buffer;
    codec_input_ch.sound_channel_num = 1;
    xcodec_input_open(&codec, &codec_input_ch, 0);
    /* input ch config */
    xcodec_input_attach_callback(&codec_input_ch, codec_input_event_cb_fun, NULL);

    input_config.bit_width = 16;
    input_config.sample_rate = 16000;
    input_config.buffer = input_buf;
    input_config.buffer_size = INPUT_BUF_SIZE;
    input_config.period = INPUT_PERIOD;
    input_config.mode = XCODEC_INPUT_DIFFERENCE;
    input_config.sound_channel_num = 1;
    xcodec_input_config(&codec_input_ch, &input_config);
    xcodec_input_analog_gain(&codec_input_ch, 0x0);

    xcodec_dma_ch_t *dma_hdl_in = msp_zalloc_check(sizeof(xcodec_dma_ch_t));
    dma_hdl_in->ctrl_id = 0;
    dma_hdl_in->ch_id = 1;

    xcodec_input_link_dma(&codec_input_ch, dma_hdl_in);

    printf("start code input and output\n");
    xcodec_input_start(&codec_input_ch);
    //xcodec_output_start(&codec_output_ch);
    uint32_t end_time = 60000 + msp_now_ms(); ///< playback 60S
    uint32_t res = 0, output_num = 0;
    msp_msleep(100);
    printf("read and write\n");
    while (1) {
        if (cb_input_transfer_flag) {
            while (1) {
                if (xcodec_input_buffer_avail(&codec_input_ch) < XCODEC_DMA_INPUT_PER_NODE_SIZE) {
                    msp_msleep(100);
                } else {
                    break;
                }
            }
            res = xcodec_input_read_async(&codec_input_ch, p_buf + input_num, STORE_BUF_SIZE - input_num);
            input_num += res;
            printf("num_input:%lu\r\n", input_num);
            printf("ret_input:%lu\r\n", res);
            res = 0;
            /*we need read last data*/
            if (cb_input_transfer_flag == PERIOD_NUM) {
                cb_input_transfer_flag = 0;
                cb_output_transfer_flag = 1;
                res = 0;
                //new_data_flag = 1;
                inti = 1;
            }
        }
        msp_msleep(100);
        if (cb_output_transfer_flag) {
           if (inti == 1) {
                AUPWM_Start(AUPWM_MONO_LEFT);
                inti = 0;
                output_num = 0;
            }
            input_num -= res;
            res = xcodec_output_write_async(&codec_output_ch, p_buf + output_num, input_num);
            output_num += res;
            printf("res_output:%lu\r\n", res);
            //new_data_flag = 0U;
            printf("output_num_output:%lu\r\n", output_num);
            //cb_output_transfer_flag = 0U;
        }

        if (msp_now_ms() > end_time) {
            break;
        }
    }

    printf("stop code input and output\n");
    xcodec_input_stop(&codec_input_ch);
    xcodec_output_stop(&codec_output_ch);

    xcodec_input_close(&codec_input_ch);
    xcodec_output_close(&codec_output_ch);

    xcodec_input_link_dma(&codec_input_ch, NULL);
    xcodec_output_link_dma(&codec_output_ch, NULL);

    xcodec_output_detach_callback(&codec_output_ch);
    xcodec_input_detach_callback(&codec_input_ch);

    xcodec_uninit(&codec);

    msp_free(output_buf);
    msp_free(input_buf);
    msp_free(p_buf);
    msp_free(dma_hdl_in);
    msp_free(dma_hdl_out);
    cb_output_transfer_flag = 0;
    cb_input_transfer_flag = 1;
    input_num = 0;
    inti = 1;
    out_flag = 0;
    in_flag = 0;

    return 0;
}

void _audio_loop_task(void *arg)
{
    int ret;

    ret = test_codec();
    if (ret < 0) {
        printf("test_codec fail\n");
    }
    msp_task_exit(0);
}

void msp_loop_test(void)
{
    msp_task_t msp_task;
    msp_task_new_ext(&msp_task, "msp_loop", _audio_loop_task, NULL, 4096, 2);
}
#endif
