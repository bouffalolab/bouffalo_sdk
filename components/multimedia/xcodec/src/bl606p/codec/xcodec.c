/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include "xcodec.h"
#include "codec_output.h"
#include "codec_input.h"
#include "codec_volume.h"

#include <drv/dma.h>
#include <drv/irq.h>
#include <drv/gpio.h>
#include <drv/pin.h>
#include <drv/porting.h>

#include <xutils/xringbuffer.h>
#include <xutils/debug.h>
#include <msp/kernel.h>

#define XCODEC_DMA_OUTPUT_NODE_NUM        (16)
#define XCODEC_DMA_OUTPUT_PER_NODE_SIZE   (1024)
#define XCODEC_DMA_INPUT_NODE_NUM         (11)
#define XCODEC_DMA_INPUT_PER_NODE_SIZE    (1920)//20ms    //(1536)//16ms

static aui_ch_t rx_context;

xcodec_error_t xcodec_init(xcodec_t *codec, uint32_t idx)
{
    if(idx != 0) {
        return XCODEC_ERROR;
    }
    uint32_t pa_pin = codec->output_chs->pa_pin;
    audio_poweron(pa_pin);

    return XCODEC_OK;
}
xcodec_error_t xcodec_input_open(xcodec_t *codec, xcodec_input_t *ch, uint32_t ch_idx)
{
    int ret = -1;

    if(ch_idx != 0)
    {
        return XCODEC_ERROR;
    }

    rx_context.ch_idx = ch_idx;
    rx_context.ringbuffer = (m_ringbuf_t *)ch->ring_buf;
    rx_context.dma = msp_malloc(sizeof(aui_dma_t));

    rx_context.task_pri = 1;
    rx_context.stack_size = 2048;//Bytes
    rx_context.event_timeout = 20;//ms

    memset(rx_context.dma, 0, sizeof(aui_dma_t));
    rx_context.dma->src_addr = 0x2000C204;

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
    // ch->period            = config->period;
    // ch->sound_channel_num = config->sound_channel_num;

    cfg.sample_rate = config->sample_rate;
    cfg.bit_width = config->bit_width;
    cfg.buffer = config->buffer;
    cfg.buffer_size = config->buffer_size;
    cfg.sound_channel_num = config->sound_channel_num;
    cfg.per_node_size = config->period;
    cfg.mic_type = config->mic_type;
    cfg.pdm_channel = config->pdm_channel;
    cfg.pdm_in_pin = config->pdm_in_pin;
    cfg.pdm_clk_pin = config->pdm_clk_pin;

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
        MSP_LOGE("rx event not support event = %d\r\n", event);
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
        MSP_LOGI("dma->ctrl_id:%d, dma->ch_id:%d\r\n", dma->ctrl_id, dma->ch_id);
        rx_context.ctrl_id = dma->ctrl_id;
        rx_context.ch_id = dma->ch_id;
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
    tx_context.ringbuffer = (m_ringbuf_t *)ch->ring_buf;
    tx_context.dma = msp_malloc(sizeof(auo_dma_t));
    tx_context.pa_pin = codec->output_chs->pa_pin;

    tx_context.task_pri = 1;
    tx_context.stack_size = 2048;//Bytes
    tx_context.event_timeout = 10;//ms

    memset(tx_context.dma, 0, sizeof(auo_dma_t));
    tx_context.dma->dst_addr = 0x2000C100;
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
    // ch->period            = config->period;
    // ch->sound_channel_num = config->sound_channel_num;

    cfg.sample_rate = config->sample_rate;
    cfg.bit_width = config->bit_width;
    cfg.buffer = config->buffer;
    cfg.buffer_size = config->buffer_size;
    cfg.sound_channel_num = config->sound_channel_num;
    cfg.per_node_size = config->period;
    cfg.pa_pin = config->pa_pin;

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
        MSP_LOGE("event not support\r\n");
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
        //MSP_LOGI("dma->ctrl_id:%d, dma->ch_id:%d\r\n", dma->ctrl_id, dma->ch_id);
        tx_context.ctrl_id = dma->ctrl_id;
        tx_context.ch_id = dma->ch_id;
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
        MSP_LOGW("codec have close ?\r\n");
        return 0;
    }
    //return 0;
    return auo_write(&tx_context, data, size);
}

uint32_t xcodec_output_write_async(xcodec_output_t *ch, const void *data, uint32_t size)
{
    if (NULL == tx_context.dma) {
        MSP_LOGW("codec have close ?\r\n");
        return 0;
    }
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
    MSP_LOGD("xcodec_output_digital_gain = %ld\r\n", val);
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
