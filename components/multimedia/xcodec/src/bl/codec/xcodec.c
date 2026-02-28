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
#include <bflb_gpio.h>
#include <bflb_audac.h>
#include <bflb_auadc.h>
#include <xutils/xringbuffer.h>
#include <msp/kernel.h>
#include <msp_port.h>
#ifdef AOS_COMP_ULOG
#include <ulog/ulog.h>
#else
#define LOGD(...)
#endif

#define XCODEC_CONFIG_DEBUG (0)

#define TAG                 "xcodec"

#if XCODEC_CONFIG_DEBUG
#define user_log(M, ...)                                                                                \
    do {                                                                                                \
        printf("[%9lu]======[%s:%d] " M "", (uint32_t)msp_now_ms(), __func__, __LINE__, ##__VA_ARGS__); \
    } while (0 == 1)
#else
#define user_log(M, ...)
#endif

#define XCODEC_DMA_OUTPUT_NODE_NUM      (16)
#define XCODEC_DMA_OUTPUT_PER_NODE_SIZE (1024)
#define XCODEC_DMA_INPUT_NODE_NUM       (11)
#define XCODEC_DMA_INPUT_PER_NODE_SIZE  (1920) //20ms    //(1536)//16ms

#define XCODEC_OUTPUT_TASK_PRI          (2)
#define XCODEC_OUTPUT_TASK_STACK_SIZE   (1024) //bytes
#define XCODEC_OUTPUT_EVENT_TIMEROUT    (10)   //ms

#define XCODEC_INPUT_TASK_PRI           (2)
#define XCODEC_INPUT_TASK_STACK_SIZE    (8192)
#define XCODEC_INPUT_EVENT_TIMEROUT     (20)

#ifdef BL618DG
#define DMA_BASE 0x20081000
#elif defined(BL616)
#define DMA_BASE 0x2000c000
#endif

static aui_ch_t rx_context;
static struct bflb_device_s *audac_dev = NULL;

#ifndef CONFIG_CODEC_USE_I2S
int codec_aupwm_pinmux_config(uint8_t pin)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(gpio, pin, GPIO_FUNC_AUDAC_PWM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);

    return 0;
}

int codec_auadc_pinmux_config(uint8_t pin)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(gpio, pin, GPIO_ANALOG | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    return 0;
}
#endif
xcodec_error_t xcodec_init(xcodec_t *codec, uint32_t idx)
{
    if (idx != 0) {
        return XCODEC_ERROR;
    }

    return XCODEC_OK;
}

xcodec_error_t xcodec_input_open(xcodec_t *codec, xcodec_input_t *ch, uint32_t ch_idx)
{
    int ret = -1;

    if (ch_idx != 0) {
        return XCODEC_ERROR;
    }

    if (!msp_mutex_is_valid(&rx_context.mutex)) {
        msp_mutex_new(&rx_context.mutex);
    } else {
        user_log("an input already open, close first please!!!\r\n");
        return XCODEC_ERROR;
    }

    msp_mutex_lock(&(rx_context.mutex), MSP_WAIT_FOREVER);

    msp_dma_ch_cfg_t dma_channel_cfg;
    dma_channel_cfg = msp_dma_channel_config();
    rx_context.ctrl_id = dma_channel_cfg.dma_rx_ctrl_id;
    rx_context.ch_id = dma_channel_cfg.dma_rx_ch_id;

    rx_context.ch_idx = ch_idx;
    rx_context.ringbuffer = ch->ring_buf;
    rx_context.dma = msp_malloc(sizeof(aui_dma_t));

    rx_context.task_pri = XCODEC_INPUT_TASK_PRI;
    rx_context.stack_size = XCODEC_INPUT_TASK_STACK_SIZE;   //Bytes
    rx_context.event_timeout = XCODEC_INPUT_EVENT_TIMEROUT; //ms
    rx_context.maxcount = XCODEC_DMA_INPUT_NODE_NUM;
    rx_context.per_node_size = XCODEC_DMA_INPUT_PER_NODE_SIZE;
    memset(rx_context.dma, 0, sizeof(aui_dma_t));
    rx_context.dma->src_addr = DMA_BASE + 0x104 + 0x100 * rx_context.ch_id;

    codec->input_chs = ch;
    ch->codec = codec;
    ch->ch_idx = ch_idx;
    ch->state.error = 0U;
    ch->state.readable = 0U;
    ch->state.writeable = 0U;

    ret = aui_init(&rx_context);

    msp_mutex_unlock(&(rx_context.mutex));

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

    ch->period = config->period;

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
#if CONFIG_CODEC_USE_I2S
    msp_i2s_device_init(config->sample_rate);
#else
    msp_codec_pin_cfg_t pin_cfg = msp_codec_pin_config();
    /* means unused pin */
    if (pin_cfg.input_positive_pin != 255) {
        codec_auadc_pinmux_config(pin_cfg.input_positive_pin);
    }
    if (pin_cfg.input_negative_pin != 255) {
        codec_auadc_pinmux_config(pin_cfg.input_negative_pin);
    }

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
        msp_dma_ch_cfg_t dma_channel_cfg;
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

    return;
}

void xcodec_input_close(xcodec_input_t *ch)
{
    if (rx_context.dma) {
        msp_free(rx_context.dma);
        rx_context.dma = NULL;
    }

    msp_mutex_free(&rx_context.mutex);

    return;
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
    return XCODEC_OK;
}

xcodec_error_t xcodec_input_mute(xcodec_input_t *ch, bool enable)
{
    return XCODEC_OK;
}

xcodec_error_t xcodec_input_mix_gain(xcodec_input_t *ch, float val)
{
    return XCODEC_OK;
}
xcodec_error_t xcodec_input_digital_gain(xcodec_input_t *ch, float val)
{
    return XCODEC_OK;
}
xcodec_error_t xcodec_input_analog_gain(xcodec_input_t *ch, float val)
{
    return XCODEC_OK;
}

static auo_ch_t tx_context;

xcodec_error_t xcodec_output_open(xcodec_t *codec, xcodec_output_t *ch, uint32_t ch_idx)
{
    int ret = -1;

    if (!msp_mutex_is_valid(&tx_context.mutex)) {
        msp_mutex_new(&tx_context.mutex);
    } else {
        user_log("an output already open, close first please!!!\r\n");
        return XCODEC_ERROR;
    }

    msp_mutex_lock(&(tx_context.mutex), MSP_WAIT_FOREVER);

    tx_context.ch_idx = ch_idx;
    tx_context.ringbuffer = ch->ring_buf;
    tx_context.dma = msp_malloc(sizeof(auo_dma_t));
    msp_dma_ch_cfg_t dma_channel_cfg;
    dma_channel_cfg = msp_dma_channel_config();
    tx_context.ctrl_id = dma_channel_cfg.dma_tx_ctrl_id;
    tx_context.ch_id = dma_channel_cfg.dma_tx_ch_id;

    tx_context.task_pri = XCODEC_OUTPUT_TASK_PRI;
    tx_context.stack_size = XCODEC_OUTPUT_TASK_STACK_SIZE;   //Bytes
    tx_context.event_timeout = XCODEC_OUTPUT_EVENT_TIMEROUT; //ms
    tx_context.maxcount = 16;
    tx_context.per_node_size = 1024;
    memset(tx_context.dma, 0, sizeof(auo_dma_t));
    tx_context.dma->dst_addr = DMA_BASE + 0x100 + 0x100 * tx_context.ch_id;
    tx_context.dma->halt_reg = tx_context.dma->dst_addr + 0x10;
    tx_context.dma->halt = 0x00040000;
    tx_context.dma->halt_flag = 0;
    tx_context.dma->unhalt = 0xFFFBFFFF;

    if (audac_dev == NULL) {
        audac_dev = bflb_device_get_by_name("audac");
    }

    codec->output_chs = ch;
    ch->codec = codec;
    ch->ch_idx = ch_idx;
    ch->state.error = 0U;
    ch->state.readable = 0U;
    ch->state.writeable = 0U;
    ret = auo_init(&tx_context);

    msp_mutex_unlock(&(tx_context.mutex));

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

    if (config->sample_rate % 8000) {
        return XCODEC_ERROR;
    }

    ch->period = config->period;

    cfg.sample_rate = config->sample_rate;
    cfg.bit_width = config->bit_width;
    cfg.buffer = config->buffer;
    cfg.buffer_size = config->buffer_size;
    cfg.sound_channel_num = config->sound_channel_num;
    cfg.per_node_size = config->period;
    tx_context.maxcount = config->buffer_size / (config->period + sizeof(auo_segment_t));
#if CONFIG_CODEC_USE_I2S
    msp_i2s_device_init(config->sample_rate);
#else
    msp_codec_pin_cfg_t pin_cfg = msp_codec_pin_config();
    if (pin_cfg.output_negative_pin != 255) {
        codec_aupwm_pinmux_config(pin_cfg.output_negative_pin);
    }
    if (pin_cfg.output_positive_pin != 255) {
        codec_aupwm_pinmux_config(pin_cfg.output_positive_pin);
    }
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
    return XCODEC_OK;
}

uint32_t xcodec_output_write(xcodec_output_t *ch, const void *data, uint32_t size)
{
    if (NULL == tx_context.dma) {
        user_log("codec have close ?\r\n");
        return 0;
    }
    return auo_write(&tx_context, data, size);
}

uint32_t xcodec_output_write_async(xcodec_output_t *ch, const void *data, uint32_t size)
{
    if (NULL == tx_context.dma) {
        user_log("codec have close ?\r\n");
        return 0;
    }
    uint32_t align_size = size & ~31;
    return auo_write(&tx_context, data, align_size);
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
    if (tx_context.dma) {
        msp_free(tx_context.dma);
        tx_context.dma = NULL;
    }

    msp_mutex_free(&tx_context.mutex);

    return;
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
    return XCODEC_OK;
}

xcodec_error_t xcodec_output_mute(xcodec_output_t *ch, bool enable)
{
    return XCODEC_OK;
}

xcodec_error_t xcodec_output_digital_gain(xcodec_output_t *ch, float val)
{
    const float val2db[100] = { 0,   -0.25, -0.5, -0.75, -1,  -1.25, -1.5, -1.75, -2,  -2.5,  -3,  -3.5,  -4,  -4.5,
                                -5,  -5.5,  -6,   -6.5,  -7,  -7.5,  -8,   -8.5,  -9,  -9.5,  -10, -10.5, -11, -11.5,
                                -12, -12.5, -13,  -13.5, -14, -14.5, -15,  -15.5, -16, -16.5, -17, -17.5, -18, -18.5,
                                -19, -19.5, -20,  -20.5, -21, -21.5, -22,  -22.5, -23, -23.5, -24, -24.5, -25, -25.5,
                                -26, -26.5, -27,  -27.5, -28, -28.5, -29,  -29.5, -30, -30.5, -31, -31.5, -32, -32.5,
                                -33, -33.5, -34,  -34.5, -35, -35.5, -36,  -36.5, -37, -37.5, -38, -38.5, -39, -39.5,
                                -40, -40.5, -41,  -41.5, -42, -42.5, -43,  -43.5, -44, -44.5, -45, -45.5, -46, -46.5,
                                -47, -47.50 };
    int valtmp;
    uint16_t regval = 0;
    uint8_t mute = 0;

    if (val > 100) {
        valtmp = 100;
    } else if (val < 0) {
        valtmp = 0;
    } else {
        valtmp = (int)val;
    }

    if (0 != valtmp) {
        if (100 == valtmp) {
            regval = 0;
        } else {
            regval = (uint16_t)(val2db[100 - valtmp] * 2 + 512);
        }
    } else {
        mute = 1;
    }

    printf("val:%f, db=%f, valtmp:%d, regval = %d\r\n", val, val2db[100 - valtmp], valtmp, regval);

    if (0 == mute) {
        bflb_audac_feature_control(audac_dev, AUDAC_CMD_SET_MUTE, false);
        bflb_audac_feature_control(audac_dev, AUDAC_CMD_SET_VOLUME_VAL, regval);
    } else {
        bflb_audac_feature_control(audac_dev, AUDAC_CMD_SET_MUTE, true);
    }

    return XCODEC_OK;
}

xcodec_error_t xcodec_output_analog_gain(xcodec_output_t *ch, float val)
{
    return XCODEC_OK;
}

xcodec_error_t xcodec_output_mix_gain(xcodec_output_t *ch, float val)
{
    return XCODEC_OK;
}

xcodec_error_t xcodec_output_get_state(xcodec_output_t *ch, xcodec_state_t *state)
{
    return XCODEC_OK;
}

void xcodec_uninit(xcodec_t *codec)
{
    return;
}
