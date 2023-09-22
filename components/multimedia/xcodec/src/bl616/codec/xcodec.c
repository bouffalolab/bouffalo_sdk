/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <string.h>
#include "xcodec.h"
#include "codec_output.h"
#include "codec_input.h"
#include "codec_volume.h"

#include <xutils/xringbuffer.h>
#include <msp/kernel.h>
#include <msp_port.h>
#ifdef AOS_COMP_ULOG
#include <ulog/ulog.h>
#else
#define LOGD(...)
#endif

#define TG6210A_CONFIG_DEBUG  (0)

#define TAG                "xcodec"

#if TG6210A_CONFIG_DEBUG
#define user_log(M, ...) do {  printf("[%9lu]======[%s:%d] " M "",  \
                                 (uint32_t)msp_now_ms(),          \
                                 __func__,                       \
                                 __LINE__,                       \
                                 ##__VA_ARGS__);                 \
                               } while(0==1)
#else
#define user_log(M, ...)
#endif

#define XCODEC_DMA_OUTPUT_NODE_NUM        (16)
#define XCODEC_DMA_OUTPUT_PER_NODE_SIZE   (1024)
#define XCODEC_DMA_INPUT_NODE_NUM         (11)
#define XCODEC_DMA_INPUT_PER_NODE_SIZE    (1920)//20ms    //(1536)//16ms

#define XCODEC_OUTPUT_TASK_PRI                   (2)
#define XCODEC_OUTPUT_TASK_STACK_SIZE            (8192) //bytes
#define XCODEC_OUTPUT_EVENT_TIMEROUT             (10)    //ms

#define XCODEC_INPUT_TASK_PRI                    (2)
#define XCODEC_INPUT_TASK_STACK_SIZE             (8192)
#define XCODEC_INPUT_EVENT_TIMEROUT              (20)

static aui_ch_t rx_context;

#include "msp_bl616_glb.h"
#include "msp_bl616_gpio.h"
#include "msp_bl616_glb_gpio.h"

int codec_gpio_pinmux_config(uint8_t pin, uint8_t enable)
{
    GLB_GPIO_Cfg_Type gpio_cfg;

    gpio_cfg.drive = 0;
    gpio_cfg.smtCtrl = 1;
    gpio_cfg.gpioMode = GPIO_MODE_OUTPUT;
    gpio_cfg.pullType = GPIO_PULL_UP;
    gpio_cfg.gpioFun = GPIO_FUN_GPIO;
    gpio_cfg.outputMode = 0;
    gpio_cfg.gpioPin = pin;
    ms_GLB_GPIO_Init(&gpio_cfg);
    msp_GLB_GPIO_Write(pin, enable);

    return 0;
}

#if !CONFIG_CODEC_USE_I2S_TX 
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
#endif
#if !CONFIG_CODEC_USE_I2S_RX
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
#endif
xcodec_error_t xcodec_init(xcodec_t *codec, uint32_t idx)
{
    if(idx != 0) {
        return XCODEC_ERROR;
    }

    return XCODEC_OK;
}

xcodec_error_t xcodec_input_open(xcodec_t *codec, xcodec_input_t *ch, uint32_t ch_idx)
{
    int ret = -1;

    if(ch_idx != 0)
    {
        return XCODEC_ERROR;
    }

    msp_dma_ch_cfg_t dma_channel_cfg;        
    //rx_context.ctrl_id = dma->ctrl_id;
    //rx_context.ch_id = dma->ch_id;
    dma_channel_cfg = msp_dma_channel_config();
    rx_context.ctrl_id = dma_channel_cfg.dma_rx_ctrl_id;
    rx_context.ch_id = dma_channel_cfg.dma_rx_ch_id;

    rx_context.ch_idx = ch_idx;
    rx_context.ringbuffer = ch->ring_buf;
    rx_context.dma = msp_malloc(sizeof(aui_dma_t));

    rx_context.task_pri = XCODEC_INPUT_TASK_PRI;
    rx_context.stack_size = XCODEC_INPUT_TASK_STACK_SIZE;//Bytes
    rx_context.event_timeout = XCODEC_INPUT_EVENT_TIMEROUT;//ms
    rx_context.maxcount = XCODEC_DMA_INPUT_NODE_NUM;
    rx_context.per_node_size = XCODEC_DMA_INPUT_PER_NODE_SIZE;
    memset(rx_context.dma, 0, sizeof(aui_dma_t));
    rx_context.dma->src_addr = 0x2000C104 + 0x100 * rx_context.ch_id;

    codec->input_chs      = ch;
    ch->codec             = codec;
    ch->ch_idx            = ch_idx;
    ch->state.error       = 0U;
    ch->state.readable    = 0U;
    ch->state.writeable   = 0U;
    
   
    ret = aui_init(&rx_context);

    if (ret) {
        return XCODEC_ERROR;
    } else {
        return XCODEC_OK;
    }
}

xcodec_error_t xcodec_input_config(xcodec_input_t *ch, xcodec_input_config_t *config)
{
    int ret = -1;
    aui_cfg_t cfg;

    // ch->ring_buf->buffer  = (void *)config->buffer;
    // ch->ring_buf->length  = config->buffer_size;
    ch->period            = config->period;
    // ch->sound_channel_num = config->sound_channel_num;

    cfg.sample_rate = config->sample_rate;
    cfg.bit_width = config->bit_width;
    cfg.buffer = config->buffer;
    cfg.buffer_size = config->buffer_size;
    cfg.sound_channel_num = config->sound_channel_num;
    cfg.per_node_size = config->period;
    cfg.mode = config->mode;
    cfg.mic_type = config->mic_type;
    cfg.pdm_channel = config->pdm_channel;
    cfg.pdm_in_pin = config->pdm_in_pin;
    cfg.pdm_clk_pin = config->pdm_clk_pin;
    rx_context.maxcount = config->buffer_size / (config->period + sizeof(aui_segment_t));
    //cfg.positive_pin = config->positive_pin;
    //cfg.negative_pin = config->negative_pin;
#if !CONFIG_CODEC_USE_I2S_RX 
    msp_codec_pin_cfg_t pin_cfg = msp_codec_pin_config();
    codec_auadc_pinmux_config(pin_cfg.input_positive_pin);
    codec_auadc_pinmux_config(pin_cfg.input_negative_pin);

    cfg.positive_pin = pin_cfg.input_positive_pin;
    cfg.negative_pin = pin_cfg.input_negative_pin;
#endif
    ret = aui_channel_config(&rx_context, &cfg);

    if (ret) {
        return XCODEC_ERROR;
    } else {
        return XCODEC_OK;
    }
}

void xcodec_rx_user_cb(aui_ch_t *context, audio_codec_event_t event, void *arg)
{
    static uint32_t r_node_num = 0;
    xcodec_input_t *ch = (xcodec_input_t *)arg;
    //printf("context->per_node_size:%lu, ch->period:%lu\r\n", context->per_node_size, ch->period);
    if (event == BL_EVENT_NODE_READ_COMPLETE) {
        r_node_num++;

        if (r_node_num * context->per_node_size >= ch->period) {
            if (ch->callback) {
                ch->callback(ch, XCODEC_EVENT_PERIOD_READ_COMPLETE, ch->arg);
            }
            r_node_num = 0;
        }
    } else if (event == BL_EVENT_READ_BUFFER_FULL) {
        if (ch->callback) {
            ch->callback(ch, XCODEC_EVENT_READ_BUFFER_FULL, ch->arg);
        }
    } else {
        printf("rx event not support event = %d\r\n", event);
    }
}

xcodec_error_t xcodec_input_attach_callback(xcodec_input_t *ch, void *callback, void *arg)
{
    ch->callback = callback;
    ch->arg = arg;

    aui_attach_callback(&rx_context, &xcodec_rx_user_cb, ch);

    return XCODEC_OK;
}

xcodec_error_t xcodec_input_link_dma(xcodec_input_t *ch, xcodec_dma_ch_t *dma)
{
    int ret = -1;
    
    if (dma) {
        //user_log("dma->ctrl_id:%d, dma->ch_id:%d\r\n", dma->ctrl_id, dma->ch_id);
        msp_dma_ch_cfg_t dma_channel_cfg;        
        //rx_context.ctrl_id = dma->ctrl_id;
        //rx_context.ch_id = dma->ch_id;
        dma_channel_cfg = msp_dma_channel_config();
        rx_context.ctrl_id = dma_channel_cfg.dma_rx_ctrl_id;
        rx_context.ch_id = dma_channel_cfg.dma_rx_ch_id;
        dma->dma_rx_srcPeriph = dma_channel_cfg.dma_rx_srcPeriph;
    }

    ret = aui_rx_dma_link(&rx_context, dma);

    if (ret) {
        return XCODEC_ERROR;
    } else {
        return XCODEC_OK;
    }
}

xcodec_error_t xcodec_input_start(xcodec_input_t *ch)
{

#if 1
    int ret = -1;

    ret = aui_start(&rx_context);

    if (ret) {
        return XCODEC_ERROR;
    } else {
        return XCODEC_OK;
    }
#endif
    return XCODEC_OK;
}


uint32_t xcodec_input_read(xcodec_input_t *ch, void *data, uint32_t size)
{
    return aui_read(&rx_context, data, size);
}

uint32_t xcodec_input_read_async(xcodec_input_t *ch, void *data, uint32_t size)
{
    return aui_read(&rx_context, data, size);
}


void xcodec_input_stop(xcodec_input_t *ch)
{
    aui_stop(&rx_context);
}

void xcodec_input_detach_callback(xcodec_input_t *ch)
{
    ch->callback = NULL;
    ch->arg = NULL;

    return ;
}

void xcodec_input_close(xcodec_input_t *ch)
{
    if (rx_context.dma) {
        msp_free(rx_context.dma);
        rx_context.dma = NULL;
    }
    return ;
}

uint32_t xcodec_input_buffer_avail(xcodec_input_t *ch)
{
    return aui_buffer_avail(&rx_context);
}
uint32_t xcodec_input_buffer_remain(xcodec_input_t *ch)
{
    return aui_buffer_remain(&rx_context);
}

xcodec_error_t xcodec_input_buffer_reset(xcodec_input_t *ch)
{
    // bl606p unnecessary
    return XCODEC_OK;
}

xcodec_error_t xcodec_input_mute(xcodec_input_t *ch, bool enable)
{
    //blyoc_audio_input_set_mute(ch, enable);
    //aui_mute(&rx_context, enable);
    mic_mute(0, enable);

    return XCODEC_OK;
}

xcodec_error_t xcodec_input_mix_gain(xcodec_input_t *ch, float val)
{
    //blyoc_audio_input_set_mix_gain(ch, 0, val);
    //bl606p not support

    return XCODEC_OK;
}
xcodec_error_t xcodec_input_digital_gain(xcodec_input_t *ch, float val)
{
    //blyoc_audio_input_digital_gain(ch, 1, val);
    //aui_digital_gain(&rx_context, val);// db
    //aui_digital_gain(&rx_context, 0);// float db

    return XCODEC_OK;
}
xcodec_error_t xcodec_input_analog_gain(xcodec_input_t *ch, float val)
{
    //blyoc_audio_input_analog_gain(ch, val);
    //aui_analog_gain(&rx_context, val);// db

    return XCODEC_OK;
}


static auo_ch_t tx_context;

xcodec_error_t xcodec_output_open(xcodec_t *codec, xcodec_output_t *ch, uint32_t ch_idx)
{
    int ret = -1;

    tx_context.ch_idx = ch_idx;
    tx_context.ringbuffer = ch->ring_buf;
    tx_context.dma = msp_malloc(sizeof(auo_dma_t));
    //tx_context.pa_pin = codec->output_chs->pa_pin;
    msp_dma_ch_cfg_t dma_channel_cfg;        
    //rx_context.ctrl_id = dma->ctrl_id;
    //rx_context.ch_id = dma->ch_id;
    dma_channel_cfg = msp_dma_channel_config();
    tx_context.ctrl_id = dma_channel_cfg.dma_tx_ctrl_id;
    tx_context.ch_id = dma_channel_cfg.dma_tx_ch_id;

    tx_context.task_pri = XCODEC_OUTPUT_TASK_PRI;
    tx_context.stack_size = XCODEC_OUTPUT_TASK_STACK_SIZE;//Bytes
    tx_context.event_timeout = XCODEC_OUTPUT_EVENT_TIMEROUT;//ms
    tx_context.maxcount = 16;
    tx_context.per_node_size = 1024;
    memset(tx_context.dma, 0, sizeof(auo_dma_t));
    tx_context.dma->dst_addr = 0x2000C100 + 0x100 * tx_context.ch_id;
    tx_context.dma->halt_reg = tx_context.dma->dst_addr + 0x10;
    tx_context.dma->halt = 0x00040000;
    tx_context.dma->unhalt = 0xFFFBFFFF;

    codec->output_chs     = ch;
    ch->codec             = codec;
    ch->ch_idx            = ch_idx;
    ch->state.error       = 0U;
    ch->state.readable    = 0U;
    ch->state.writeable   = 0U;
    ret = auo_init(&tx_context);

    if (ret) {
        return XCODEC_ERROR;
    } else {
        return XCODEC_OK;
    }
}

xcodec_error_t xcodec_output_config(xcodec_output_t *ch, xcodec_output_config_t *config)
{
    int ret = -1;
    auo_cfg_t cfg;

    if (config->sample_rate%8000) {
        return XCODEC_ERROR;
    }

    // ch->ring_buf->buffer  = config->buffer;
    // ch->ring_buf->length  = config->buffer_size;
     ch->period            = config->period;
    // ch->sound_channel_num = config->sound_channel_num;

    cfg.sample_rate = config->sample_rate;
    cfg.bit_width = config->bit_width;
    cfg.buffer = config->buffer;
    cfg.buffer_size = config->buffer_size;
    cfg.sound_channel_num = config->sound_channel_num;
    //cfg.negative_pin = config->negative_pin;
    //cfg.positive_pin = config->positive_pin;
    cfg.per_node_size = config->period;
    tx_context.maxcount = config->buffer_size / (config->period + sizeof(auo_segment_t));
    //cfg.pa_pin = config->pa_pin;
#if CONFIG_CODEC_USE_I2S_TX
    msp_i2s_device_init(config->sample_rate);
#else
    msp_codec_pin_cfg_t pin_cfg = msp_codec_pin_config();
    
    codec_aupwm_pinmux_config(pin_cfg.output_negative_pin);
    codec_aupwm_pinmux_config(pin_cfg.output_positive_pin);
#endif
    ret = auo_channel_config(&tx_context, &cfg);

    if (ret) {
        return XCODEC_ERROR;
    } else {
        return XCODEC_OK;
    }
}

void xcodec_tx_user_cb(auo_ch_t *context, audio_codec_event_t event, void *arg)
{
    static uint32_t w_node_num = 0;
    xcodec_output_t *ch = (xcodec_output_t *)arg;

    if (event == BL_EVENT_NODE_WRITE_COMPLETE) {
        w_node_num++;

        if (w_node_num * context->per_node_size >= ch->period) {
            if (ch->callback) {
                ch->callback(ch, XCODEC_EVENT_PERIOD_WRITE_COMPLETE, ch->arg);
            }
            w_node_num = 0;
        }
    } else if (event == BL_EVENT_WRITE_BUFFER_EMPTY) {
        if (ch->callback) {
            ch->callback(ch, XCODEC_EVENT_WRITE_BUFFER_EMPTY, ch->arg);
        }
    } else {
        printf("event not support\r\n");
    }
}

xcodec_error_t xcodec_output_attach_callback(xcodec_output_t *ch, void *callback, void *arg)
{
    ch->callback = callback;
    ch->arg = arg;

    auo_attach_callback(&tx_context, &xcodec_tx_user_cb, ch);

    return XCODEC_OK;
}

xcodec_error_t xcodec_output_link_dma(xcodec_output_t *ch, xcodec_dma_ch_t *dma)
{
    int ret = -1;

    if (dma) {
        msp_dma_ch_cfg_t dma_channel_cfg;        
        dma_channel_cfg = msp_dma_channel_config();
        tx_context.ctrl_id = dma_channel_cfg.dma_tx_ctrl_id;
        tx_context.ch_id = dma_channel_cfg.dma_tx_ch_id;
        dma->dma_tx_dstPeriph = dma_channel_cfg.dma_tx_dstPeriph;
        //user_log("dma->ctrl_id:%d, dma->ch_id:%d\r\n", dma->ctrl_id, dma->ch_id);
        //tx_context.ctrl_id = dma->ctrl_id;
        //tx_context.ch_id = dma->ch_id;
    }
    ret = auo_tx_dma_link(&tx_context, dma);

    if (ret) {
        return XCODEC_ERROR;
    } else {
        return XCODEC_OK;
    }
}

xcodec_error_t xcodec_output_start(xcodec_output_t *ch)
{
#if 0
    int ret = -1;

    ret = auo_start(&tx_context);

    if (ret) {
        return XCODEC_ERROR;
    } else {
        return XCODEC_OK;
    }
#endif
    return XCODEC_OK;
}

uint32_t xcodec_output_write(xcodec_output_t *ch, const void *data, uint32_t size)
{
    if (NULL == tx_context.dma) {
        user_log("codec have close ?\r\n");
        return 0;
    }
    //printf("----***********************************11111111111\r\n");
    //return 0;
    return auo_write(&tx_context, data, size);
}

uint32_t xcodec_output_write_async(xcodec_output_t *ch, const void *data, uint32_t size)
{
    if (NULL == tx_context.dma) {
        user_log("codec have close ?\r\n");
        return 0;
    }
    //printf("----***********************************\r\n");
    //return 0;
    return auo_write(&tx_context, data, size);
}

void xcodec_output_stop(xcodec_output_t *ch)
{
    auo_stop(&tx_context);
}

void xcodec_output_detach_callback(xcodec_output_t *ch)
{
    ch->callback = NULL;
    ch->arg = NULL;
}

void xcodec_output_close(xcodec_output_t *ch)
{
    //blyoc_audio_output_close(ch);
    if (tx_context.dma) {
        msp_free(tx_context.dma);
        tx_context.dma = NULL;
    }
}


xcodec_error_t xcodec_output_pause(xcodec_output_t *ch)
{
    auo_pause(&tx_context);

    return XCODEC_OK;
}

xcodec_error_t xcodec_output_resume(xcodec_output_t *ch)
{
    auo_resume(&tx_context);

    return XCODEC_OK;
}

uint32_t xcodec_output_buffer_avail(xcodec_output_t *ch)
{
    return auo_buffer_avail(&tx_context);
}

uint32_t xcodec_output_buffer_remain(xcodec_output_t *ch)
{
    return auo_buffer_remain(&tx_context);
}

xcodec_error_t xcodec_output_buffer_reset(xcodec_output_t *ch)
{
    //auo_buffer_reset(&tx_context);
    return XCODEC_OK;
}

xcodec_error_t xcodec_output_mute(xcodec_output_t *ch, bool enable)
{
    //blyoc_audio_output_set_mute(ch, enable ? 0 : 1);

    //auo_mute(&tx_context, enable);
    spk_mute(0, 1);// (ch, en)
    return XCODEC_OK;
}

xcodec_error_t xcodec_output_digital_gain(xcodec_output_t *ch, float val)
{
    //blyoc_audio_output_digital_gain(ch, 1, val);
    // auo_digtal_gain(&tx_context, val*2);
    spk_digital_gain_set(0, val);//(int ch, float gaindb)
    LOGD(TAG, "xcodec_output_digital_gain = %ld\r\n", val);
    return XCODEC_OK;
}

xcodec_error_t xcodec_output_analog_gain(xcodec_output_t *ch, float val)
{
    //blyoc_audio_output_analog_gain(ch, val);
    return XCODEC_OK;
}

xcodec_error_t xcodec_output_mix_gain(xcodec_output_t *ch, float val)
{
    // bl606p not support

    return XCODEC_OK;
}

xcodec_error_t xcodec_output_get_state(xcodec_output_t *ch, xcodec_state_t *state)
{
    // bl606p always ok

    return XCODEC_OK;
}

void xcodec_uninit(xcodec_t *codec)
{
}







#if 0


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

/*=====================loop code=========================*/

#include <xutils/xringbuffer.h>

#include "msp_bl616_aupwm.h"
#include "msp_bl616_auadc.h"
#define OUTPUT_BUF_SIZE     (17 * 1024)
#define STORE_BUF_SIZE      (64 * 1024)
#define OUTPUT_PERIOD       (16 * 1024)
#define INPUT_PERIOD    OUTPUT_PERIOD
#define PERIOD_NUM          (4)
#define INPUT_BUF_SIZE  OUTPUT_BUF_SIZE

static xcodec_t codec;
static xcodec_output_t codec_output_ch;
static xcodec_input_t  codec_input_ch;

static uint32_t dma_ch_output_handle;
static uint32_t dma_ch_input_handle;

//static x_ringbuf_t output_ring_buffer;
//static x_ringbuf_t input_ring_buffer;
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
        if (out_flag == PERIOD_NUM) {
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

static int test_codec(void)
{
    xcodec_error_t ret;
    xcodec_output_config_t output_config;
    xcodec_input_config_t input_config;

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
    xcodec_output_open(&codec, &codec_output_ch, 0);
    /* output ch config */
    xcodec_output_attach_callback(&codec_output_ch, codec_output_event_cb_fun, NULL);

    output_config.bit_width = 16;
    output_config.sample_rate = 16000;
    output_config.buffer = output_buf;
    output_config.buffer_size = OUTPUT_BUF_SIZE;
    output_config.period = OUTPUT_PERIOD;
    output_config.mode = XCODEC_OUTPUT_SINGLE_ENDED;
    output_config.sound_channel_num = 1;
    xcodec_output_config(&codec_output_ch, &output_config);
    xcodec_output_analog_gain(&codec_output_ch, 0xffad);
    xcodec_output_buffer_reset(&codec_output_ch);

    xcodec_dma_ch_t *dma_hdl_out = msp_zalloc_check(sizeof(xcodec_dma_ch_t));
    dma_hdl_out->ctrl_id = 0;
    dma_hdl_out->ch_id = 0;

    xcodec_output_link_dma(&codec_output_ch, dma_hdl_out);
    /*stop*/
    AUPWM_Stop(AUPWM_MONO_LEFT);

    //codec_input_ch.ring_buf = &input_ring_buffer;
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

// extern int ng_auo_cmd(char *buf, int len, int argc, char **argv);
void msp_loop_test(void)
{
    // msp_task_new("msp_loop", _audio_loop_task, NULL, 4096);
    // ng_auo_cmd(NULL, 0, 0, NULL);
}
#endif
