/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdio.h>

#include <msp_sys.h>
#include <msp_dma.h>
#include <msp_bl606p_dma.h>
#include <msp_bl606p_audio.h>
#include <codec_input.h>

#include <drv/codec.h>
#include <drv/dma.h>
#include <drv/irq.h>
#include <soc.h>
#include <msp/kernel.h>

#include <xutils/debug.h>
#include <xutils/mringbuffer.h>

#define RAMDUMP_CONFIG_DEBUG (0)

#if RAMDUMP_CONFIG_DEBUG
#define RAW_AUO_DUMP_DATA_LEN (8*1024*1024)
uint8_t *g_rawaui_data = (uint8_t *)0x55000000;
uint32_t g_rawaui_data_len = 0;
#endif

#if CODEC_INPUT_DEBUG_TRACE
aui_ch_t *g_aui_ctx = NULL;
#endif
extern void Audio_Ckg_En(void);
extern void Audio_Analog_Adc_Disable(void);

int g_audio_analog_gain1 = DEF_AUDIO_ANALOG_GAIN_INPUT1;
int g_audio_analog_gain2 = DEF_AUDIO_ANALOG_GAIN_INPUT2;
int g_audio_analog_gain3 = DEF_AUDIO_ANALOG_GAIN_INPUT3;

uint32_t g_audio_digital_gain1 = DEF_AUDIO_DIGITAL_VOLUME_INPUT1;
uint32_t g_audio_digital_gain2 = DEF_AUDIO_DIGITAL_VOLUME_INPUT2;
uint32_t g_audio_digital_gain3 = DEF_AUDIO_DIGITAL_VOLUME_INPUT3;

#if 0
static int _aui_set_buffer(aui_ch_t *context, uint32_t initial, uint32_t threshold)
{
    uint32_t i;
    uint32_t node_size = context->per_node_size;
    int res = 0;

    //user_log("initial = %ld, threshold = %ld\r\n", initial, threshold);

    for (i = initial; i < threshold; i++) {

        /* normal 1, but overrun */
        if (2 == context->dma->node[i].dirty) {//0-dirty; 1-clear; 2-valid data
            res = 1;
        }

        /* if buffer is invalued, move ridx */
        if (context->dma->node[i].dirty == 1) {// 0-dirty; 1-clear; 2-valid data
            user_log(" _write:%d\r\n", node_size);
            context->dma->node[i].dirty = 2;

            user_log("set %d dirty->2\r\n", i);
            mringbuffer_put(context->ringbuffer, NULL, node_size);
            if (context->callback) {
                context->callback(context, BL_EVENT_NODE_READ_COMPLETE, context->arg);
            }
        }
    }

    return res;
}

static void _aui_set_used_buffer(aui_ch_t *context)
{
    uint32_t next_idx;
    int res = 0;

    next_idx = ((*((volatile uint32_t *)(uintptr_t)(context->dma->src_addr))) - context->head) / context->per_node_size;

    MSP_LOGD("next_idx = %ld, src = 0x%08lx, head = 0x%08lx, per_node_size = %ld, context->pre_indx = %ld\r\n", \
            next_idx, (*((volatile uint32_t *)(uintptr_t)(context->dma->src_addr))), \
            context->head, context->per_node_size, context->pre_indx);

    /* dma node in sample cycle */
    if (next_idx == context->pre_indx) {
        user_log("next_idx(%d) == pre_indx(%d)\r\n", next_idx, context->pre_indx);
    } else if (next_idx > context->pre_indx) {
        res |= _aui_set_buffer(context, context->pre_indx, next_idx);
        if (1 < (next_idx - context->pre_indx)) {
            user_log("!!!!!! aui set %ld (%ld~%ld)\r\n",
                    (next_idx - context->pre_indx), context->pre_indx, next_idx);
        }
    } else {
    /* dma node not in sample cycle */
        res |= _aui_set_buffer(context, context->pre_indx, context->maxcount);
        res |= _aui_set_buffer(context, 0, next_idx);
        if (1 < ((context->maxcount - context->pre_indx) + next_idx)) {
            user_log("!!!!!! aui set %ld (%ld~%ld~%ld)\r\n",
                    ((context->maxcount - context->pre_indx) + next_idx),
                    context->pre_indx, context->maxcount, next_idx);
        }
    }

    context->pre_indx = next_idx;

    if (res) {
        user_log("MIC buf overrun remain %ld\r\n", aui_buffer_remain(context));
        context->callback(context, BL_EVENT_READ_BUFFER_FULL, context->arg);
    }
}
#endif

static void _aui_recv(aui_ch_t *context)
{
    static uint64_t old_time = 0, tmp_time = 0;
    int32_t ret;
    uint32_t src;
    uint32_t aw;

    src = (*((volatile uint32_t *)(uintptr_t)(context->dma->src_addr)));
    aw =  src - context->head;

    tmp_time = msp_now_ms();
    ret = mringbuffer_put_index(context->ringbuffer, aw);
    if (ret <= 0) {
#if 1
        MSP_LOGW("aw:0x%08lx, ri:0x%08lx, wi:0x%08lx, ret:%ld, rb_data_len:%ld\r\n",
            aw, context->ringbuffer->read_index, context->ringbuffer->write_index,
            ret, mringbuffer_data_len(context->ringbuffer));
#endif
        context->callback(context, BL_EVENT_READ_BUFFER_FULL, context->arg);
    } else {
#if CODEC_INPUT_DEBUG_TRACE
        context->debug.bytes_write += ret;
#endif
        if (context->callback) {
            context->callback(context, BL_EVENT_NODE_READ_COMPLETE, context->arg);
        }

        if ((ret > (context->per_node_size + context->per_node_size / 2))
            || ((old_time != 0) && ((tmp_time - old_time) > 30))) {
            MSP_LOGW("Please check whether the application software has long-term block logic\r\n");
            MSP_LOGW("aw:0x%08lx,ri:0x%08lx,wi:0x%08lx,ret:%ld,block:%lldms,rb_data_len:%ld\r\n",
                aw, context->ringbuffer->read_index, context->ringbuffer->write_index,
                ret, (tmp_time - old_time), mringbuffer_data_len(context->ringbuffer));
        } else {
            MSP_LOGD("aw:0x%08lx, ri:0x%08lx, wi:0x%08lx, ret:%ld, rb_data_len:%ld\r\n",
                aw, context->ringbuffer->read_index, context->ringbuffer->write_index,
                ret, mringbuffer_data_len(context->ringbuffer));
        }
    }
    old_time = tmp_time;
}

#define EVENT_AU_ISR          (0x00000001)
#define EVENT_AU_STOP         (0x00000002)

static void aui_task_entry(void *arg)
{
    aui_ch_t *context = (aui_ch_t *)arg;
    unsigned int flag = 0;
    int res = 0;

    while (1) {
        res = msp_event_get(&(context->event), EVENT_AU_ISR|EVENT_AU_STOP, MSP_EVENT_OR_CLEAR, &flag, context->event_timeout);

        if (res != 0) {
            // maybe timeout
            continue;
        }

        if (res == 0) {
            if ((flag & EVENT_AU_STOP) == EVENT_AU_STOP) {
                break;
            } else if ((flag & EVENT_AU_ISR) == EVENT_AU_ISR) {
                msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
                //_aui_set_used_buffer(arg);
                _aui_recv(arg);
                msp_mutex_unlock(&(context->mutex));
            }
        }
    }

    context->task_exit = 1;
    msp_task_exit(NULL);
}

static void _aui_rx_dma_irq_hander(void *arg)
{
    aui_ch_t *context = (aui_ch_t *)arg;

    if (NULL != context) {
        msp_event_set(&context->event, EVENT_AU_ISR, MSP_EVENT_OR);
    }
}

static void aui_adc_set_mute(uint32_t channel,  Audio_Mute_Type mute)
{
    switch (channel) {
        case BL_AUDIO_LEFT_CHANNEL:
            Audio_ADC_Set_Mute(DISABLE, AUDIO_ADC_0, mute);
            break;
        case BL_AUDIO_STEREO_CHANNEL:
            Audio_ADC_Set_Mute(DISABLE, AUDIO_ADC_0, mute);
            Audio_ADC_Set_Mute(DISABLE, AUDIO_ADC_1, mute);
            break;
        case BL_AUDIO_THREE_CHANNEL:
            Audio_ADC_Set_Mute(ENABLE, 0, mute);
            break;
        default:
            Audio_ADC_Set_Mute(ENABLE, 0, mute);
            break;
    }
}

static void _aui_get_freq(Audio_Clock_CFG_Type *cfg)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AUDIO_BASE, AUDIO_AUD_TOP);
    cfg->adc_clock =  BL_GET_REG_BITS_VAL(tmpVal, AUDIO_ADC_RATE);
    cfg->dac_clock =  BL_GET_REG_BITS_VAL(tmpVal, AUDIO_DAC_RATE);
    cfg->pdm_clk_inv = BL_GET_REG_BITS_VAL(tmpVal, AUDIO_PDM_ITF_INV_SEL);
    cfg->adc_clk_inv = BL_GET_REG_BITS_VAL(tmpVal, AUDIO_ADC_ITF_INV_SEL);
    cfg->dac_clk_inv = BL_GET_REG_BITS_VAL(tmpVal, AUDIO_DAC_ITF_INV_SEL);
}

static void _aui_set_track(uint32_t channel)
{
    switch (channel) {
        case BL_AUDIO_LEFT_CHANNEL:
            Audio_ADC_Enable(AUDIO_ADC_0);
            break;
        case BL_AUDIO_STEREO_CHANNEL:
            Audio_ADC_Enable(AUDIO_ADC_0);
            Audio_ADC_Enable(AUDIO_ADC_1);
            break;
        case BL_AUDIO_THREE_CHANNEL:
            Audio_ADC_Enable(AUDIO_ADC_0);
            Audio_ADC_Enable(AUDIO_ADC_1);
            Audio_ADC_Enable(AUDIO_ADC_2);
            break;
        default:
            MSP_LOGE("Channel Is Invaild\r\n");
            break;
    }
}

static void _aui_set_freq(uint32_t freq)
{
    Audio_Clock_CFG_Type audio_clkcfg_set;

    _aui_get_freq(&audio_clkcfg_set);

    switch (freq) {
        case BL_AUDIO_RATE_8K:
            audio_clkcfg_set.adc_clock = AUDIO_ADC_8_KHZ;
            break;
        case BL_AUDIO_RATE_16K:
            audio_clkcfg_set.adc_clock = AUDIO_ADC_16_KHZ;
            break;
        case BL_AUDIO_RATE_24K:
            audio_clkcfg_set.adc_clock = AUDIO_ADC_24_KHZ;
            break;
        case BL_AUDIO_RATE_32K:
            //audio_clkcfg_set.adc_clock = AUDIO_ADC_32_KHZ;
            audio_clkcfg_set.adc_clock = AUDIO_ADC_16_KHZ;
            break;
        case BL_AUDIO_RATE_48K:
        case BL_AUDIO_RATE_44_1K:
            audio_clkcfg_set.adc_clock = AUDIO_ADC_48_KHZ;
            break;
        case BL_AUDIO_RATE_96K:
            audio_clkcfg_set.adc_clock = AUDIO_ADC_96_KHZ;
            break;
        default:
            break;
    }

    //Audio_Poweron();
    Audio_Clock_Init(&audio_clkcfg_set);
    Audio_Ckg_En();
}

static void _aui_rx_fifo_cfg(uint32_t playBitWidth)
{
    Audio_FifoCfg_Type audioFifoCfg;

    switch (playBitWidth)
    {
        case BL_AUDIO_BIT_WIDTH_16:
            audioFifoCfg.resolution = AUDIO_RES_16_BITS;
            audioFifoCfg.ailgnMode = AUDIO_FIFO_AILGN_MSB_AT_BIT15;
            break;
        case BL_AUDIO_BIT_WIDTH_24:
            audioFifoCfg.resolution = AUDIO_RES_20_BITS;
            audioFifoCfg.ailgnMode = AUDIO_FIFO_AILGN_MSB_AT_BIT23;
            break;
        case BL_AUDIO_BIT_WIDTH_32:
            audioFifoCfg.resolution = AUDIO_RES_20_BITS;
            audioFifoCfg.ailgnMode = AUDIO_FIFO_AILGN_MSB_AT_BIT31;
            break;
        default:
          MSP_LOGE("iBIT WIDTH Is Invaild\r\n");
          break;
    }

    audioFifoCfg.FifoIntThreshold = 3;// 3+1 = 4
    audioFifoCfg.dmaThresholdMode = AUDIO_DRQ_EQUEL_TO_IRQ;
    audioFifoCfg.dmaEn = ENABLE;

    Audio_RxFifoConfig(&audioFifoCfg);
}

static void _aui_rx_fifo_ensable(uint8_t channel)
{
    switch (channel) {
        case BL_AUDIO_LEFT_CHANNEL:
            Audio_RxFifoEnable(BL_AUDIO_LEFT_CHANNEL_);
            break;
        case BL_AUDIO_STEREO_CHANNEL:
            Audio_RxFifoEnable(BL_AUDIO_STEREO_CHANNEL_);
            break;
        case BL_AUDIO_THREE_CHANNEL:
            Audio_RxFifoEnable(BL_AUDIO_THREE_CHANNEL_);
            break;
        default:
            MSP_LOGE("Channel Is Invaild\r\n");
            break;
    }
}

static void _aui_rx_fifo_disable(uint8_t channel)
{
    switch (channel) {
        case BL_AUDIO_LEFT_CHANNEL:
            Audio_RxFifoDisable(BL_AUDIO_LEFT_CHANNEL_);
            break;
        case BL_AUDIO_STEREO_CHANNEL:
            Audio_RxFifoDisable(BL_AUDIO_STEREO_CHANNEL_);
            break;
        case BL_AUDIO_THREE_CHANNEL:
            Audio_RxFifoDisable(BL_AUDIO_THREE_CHANNEL_);
            break;
        default:
            MSP_LOGE("Channel Is Invaild\r\n");
            break;
    }
}

static void _audio_digital_adc(aui_ch_t *context)
{
    int val;
    Audio_Volume_Cfg_Type audioVolumeCfg = {
        .muteMode               = AUIDO_MUTE_DIRECTLY,
        .muteRmpdnRate          = AUIDO_RAMP_RATE_128_FS,//AUIDO_RAMP_RATE_2048_FS;
        .muteRmpupRate          = AUIDO_RAMP_RATE_128_FS,
        .volumeCtrMode          = AUIDO_VOLUME_UPDATE_AT_RAMP,
        .volumeZeroCrossRate    = AUIDO_ZERO_CROSS_RATE_2_FS,//AUIDO_ZERO_CROSS_RATE_2048_FS;
        .volumeRmpRate          = AUIDO_RAMP_RATE_2_FS,//AUIDO_RAMP_RATE_2048_FS;
        .volumeZeroCrossTimeout = AUIDO_ZERO_CROSS_RATE_2_FS,//AUIDO_ZERO_CROSS_RATE_2048_FS;
    };

    Audio_FifoCfg_Type audioFifoCfg = {
        .resolution         = AUDIO_RES_16_BITS,
        .ailgnMode          = AUDIO_FIFO_AILGN_MSB_AT_BIT15,
        .FifoIntThreshold   = 12,//drq dma request
        .dmaThresholdMode   = AUDIO_DRQ_EQUEL_TO_IRQ,
        .dmaEn              = ENABLE,
    };

    Audio_RxFifoConfig(&audioFifoCfg);
    // set mute ramp
    Audio_ADC_Software_Volume_Config(AUDIO_ADC_0, &audioVolumeCfg);
    Audio_ADC_Software_Volume_Config(AUDIO_ADC_1, &audioVolumeCfg);
    Audio_ADC_Software_Volume_Config(AUDIO_ADC_2, &audioVolumeCfg);
    Audio_ADC_Set_Volume_Reg(ENABLE, 0, AUDIO_DEFAULT_INITIAL_VOLUME);// 生效

#if 0
    Audio_ADC_Enable(AUDIO_ADC_0);
    Audio_ADC_Enable(AUDIO_ADC_1);
    Audio_ADC_Enable(AUDIO_ADC_2);
#endif

    /*digital*/
    if (context->mic_type) {
        Audio_Set_ADC_Source(AUDIO_ADC_0, AUDIO_ADC_PDM);
        Audio_Set_ADC_Source(AUDIO_ADC_1, AUDIO_ADC_PDM);
        Audio_Set_ADC_Source(AUDIO_ADC_2, AUDIO_ADC_ANALOG);

        if (context->pdm_channel) {
            Audio_ADC_PDM_Enable(AUDIO_PDM_1);
            Audio_ADC_PDM_Channel_Sel(AUDIO_PDM_0, AUDIO_PDM_1_L);
            Audio_ADC_PDM_Channel_Sel(AUDIO_PDM_1, AUDIO_PDM_1_R);
        } else {
            Audio_ADC_PDM_Enable(AUDIO_PDM_0);
            Audio_ADC_PDM_Channel_Sel(AUDIO_PDM_0, AUDIO_PDM_0_L);
            Audio_ADC_PDM_Channel_Sel(AUDIO_PDM_1, AUDIO_PDM_0_R);
        }
    } else {
        Audio_Set_ADC_Source(AUDIO_ADC_0, AUDIO_ADC_ANALOG);
        Audio_Set_ADC_Source(AUDIO_ADC_1, AUDIO_ADC_ANALOG);
        Audio_Set_ADC_Source(AUDIO_ADC_2, AUDIO_ADC_ANALOG);
    }
    /* unmute all channel */
    Audio_ADC_Set_Mute(ENABLE, 0, AUDIO_UNMUTE);

    //Audio_ADC_Set_Volume(ENABLE, 0, -60);
#if 1
    if (g_audio_digital_gain1 < 0) {
        //正增益*2 ， 负增益*2+512
        val = g_audio_digital_gain1 * 2 + 512;
    } else {
        //正增益*2 ， 负增益*2+512
        val = g_audio_digital_gain1 * 2;
    }
    Audio_ADC_Set_Volume(ENABLE, 0, val);
#else
    Audio_ADC_Set_Volume(DISABLE, 0, DEF_AUDIO_DIGITAL_VOLUME_INPUT1);
    Audio_ADC_Set_Volume(DISABLE, 0, DEF_AUDIO_DIGITAL_VOLUME_INPUT2);
    Audio_ADC_Set_Volume(DISABLE, 0, DEF_AUDIO_DIGITAL_VOLUME_INPUT3);
#endif

    /* disable all rx*/
    _aui_rx_fifo_disable(BL_AUDIO_THREE_CHANNEL);
}

int aui_init(aui_ch_t *context)
{
    if (NULL == context) {
        MSP_LOGA("error\r\n");
        return -1;
    }

    MSP_LOGI("context = %p\r\n", context);
#if CODEC_INPUT_DEBUG_TRACE
    g_aui_ctx = context;
    memset(&(context->debug), 0, sizeof(context->debug));
    context->debug.count_init++;
#endif

    msp_mutex_new(&(context->mutex));

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);

    context->task_exit = 0;
    msp_event_new(&(context->event), 0);
    msp_task_new_ext(&(context->task), "auitsk", aui_task_entry, context,
                     context->stack_size, context->task_pri);

    msp_mutex_unlock(&(context->mutex));

    return 0;
}

static void _audio_anolog_adc(void)
{
    Audio_Analog_Adc();

    mic_analog_gain_set(0, g_audio_analog_gain1);
    mic_analog_gain_set(1, g_audio_analog_gain2);
    mic_analog_gain_set(2, g_audio_analog_gain3);
}

int aui_channel_config(aui_ch_t *context, aui_cfg_t *config)
{
    if (NULL == context || NULL == config) {
        MSP_LOGA("error!\r\n");
        return -1;
    }

#if CODEC_INPUT_DEBUG_TRACE
    context->debug.count_config++;
#endif

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);

    context->bit_width = config->bit_width;
    context->buffer = config->buffer;
    context->buffer_size = config->buffer_size;
    context->per_node_size = config->per_node_size;
    context->sound_channel_num = config->sound_channel_num;
    context->mic_type = config->mic_type;
    context->pdm_channel= config->pdm_channel;
    context->pdm_in_pin = config->pdm_in_pin;
    context->pdm_clk_pin = config->pdm_clk_pin;

    if (config->mic_type) {
        bl_gpio_set_for_pdm(config->pdm_in_pin, config->pdm_clk_pin);
    }

    MSP_LOGI("context = %p\r\n", context);
    _aui_set_freq(config->sample_rate);
    _aui_set_track(config->sound_channel_num);
    _aui_rx_fifo_cfg(config->bit_width);

    _audio_anolog_adc();
    _audio_digital_adc(context);

    // init global value
    context->st = 0;
    context->pre_indx = 0;

    msp_mutex_unlock(&(context->mutex));

    return 0;
}

volatile uint32_t *g_aui_rb_buf = NULL;
volatile uint32_t g_aui_rb_bufsize = 0;
static int _aui_rx_dma_link(aui_ch_t *context, void *dma)
{
    int i;
    uint8_t dma_id, dma_ch;
    uint32_t min_buf_size, dma_node_num, node_size;
    uint8_t *pbuf, *node_pbuf, *hw_node_addr, *hw_buff_addr;

    node_size = context->per_node_size;

    DMA_LLI_Cfg_Type lli_cfg_dma = {
        DMA_TRNS_P2M,
        DMA_REQ_AUDIO_RX,
        DMA_REQ_NONE,
    };

    struct DMA_Control_Reg audio_dma_ctrl = {
        .TransferSize = 0,
        .SBSize       = DMA_BURST_SIZE_4,
        .DBSize       = DMA_BURST_SIZE_4,
        .SWidth       = DMA_TRNS_WIDTH_32BITS,
        .DWidth       = DMA_TRNS_WIDTH_32BITS,
        .SI           = DMA_MINC_DISABLE,
        .DI           = DMA_MINC_ENABLE,
        .dst_min_mode = DISABLE,
        .dst_add_mode = DISABLE,
        .fix_cnt      = 0,
        .I            = 1,
    };

    if (NULL == context || NULL == context->buffer) {
        MSP_LOGE("error!\r\n");
        return -1;
    }

    MSP_LOGI("context = %p\r\n", context);

    /* init user buffer */
    memset(context->buffer, 0, context->buffer_size);
    //msp_cache_flush(context->buffer, context->buffer_size);

    switch (context->bit_width) {
        case BL_AUDIO_BIT_WIDTH_16:
            audio_dma_ctrl.SWidth = DMA_TRNS_WIDTH_16BITS;    //DMA_TRNS_WIDTH_32BITS;
            audio_dma_ctrl.DWidth = DMA_TRNS_WIDTH_16BITS;
            audio_dma_ctrl.SBSize = DMA_BURST_SIZE_4;        // DMA_BURST_SIZE_4;
            audio_dma_ctrl.DBSize = DMA_BURST_SIZE_4;
            audio_dma_ctrl.TransferSize = node_size / 2;
            break;
        case BL_AUDIO_BIT_WIDTH_24:
            audio_dma_ctrl.SWidth = DMA_TRNS_WIDTH_32BITS;
            audio_dma_ctrl.DWidth = DMA_TRNS_WIDTH_32BITS;
            audio_dma_ctrl.SBSize = DMA_BURST_SIZE_4;
            audio_dma_ctrl.DBSize = DMA_BURST_SIZE_4;
            audio_dma_ctrl.TransferSize = node_size / 4;
            break;
        case BL_AUDIO_BIT_WIDTH_32:
            audio_dma_ctrl.SWidth = DMA_TRNS_WIDTH_32BITS;
            audio_dma_ctrl.DWidth = DMA_TRNS_WIDTH_32BITS;
            audio_dma_ctrl.SBSize = DMA_BURST_SIZE_4;
            audio_dma_ctrl.DBSize = DMA_BURST_SIZE_4;
            audio_dma_ctrl.TransferSize = node_size / 4;
            break;
        default:
            MSP_LOGE("eBIT WIDTH Is Invaild\r\n");
            break;
    }

    // fixme
    dma_node_num = ((context->buffer_size - 2 * AUDIO_ALIGNMENT_BYTES) / (sizeof(aui_segment_t) + node_size));
    context->maxcount = dma_node_num;

    /* init dma */
    node_pbuf = context->buffer;
    hw_node_addr = (uint8_t *)((intptr_t)(node_pbuf + AUDIO_ALIGNMENT_BYTES - 1) & ~AUDIO_ALIGNMENT_MASK);

    pbuf = hw_node_addr + dma_node_num * sizeof(aui_segment_t);
    hw_buff_addr = (uint8_t *)((intptr_t)(pbuf + AUDIO_ALIGNMENT_BYTES - 1) & ~AUDIO_ALIGNMENT_MASK);

    MSP_LOGI("input:%p --> hw_node:%p[%ld*%d] --> hw_buf:%p[%ld*%ld]\r\n",
            node_pbuf,
            hw_node_addr,
            dma_node_num, sizeof(aui_segment_t),
            hw_buff_addr,
            dma_node_num, node_size);

    /* user buffer size must enouth for dma */
    min_buf_size = (uint32_t)(hw_buff_addr + dma_node_num * node_size - 1) - (uint32_t)(context->buffer);

    MSP_LOGI("min_buf-size:%ld, %ld\r\n", min_buf_size, context->buffer_size);
    if (min_buf_size > context->buffer_size) {
        MSP_LOGE("size too small!\r\n");
        while(1) {
            msp_msleep(100);
            MSP_LOGE("input min_buf-size:%ld, %ld\r\n", min_buf_size, context->buffer_size);
        }
        return -1;
    }

    g_aui_rb_buf = (uint32_t *)hw_buff_addr;
    g_aui_rb_bufsize = dma_node_num * node_size;

    mringbuffer_init(context->ringbuffer, hw_buff_addr, dma_node_num * node_size);

    context->dma->node = (aui_segment_t *)hw_node_addr;

    for (i = 0; i < dma_node_num; i++) {
        /* dma nodes are head of user buffer */
        /* srcDmaAddr must by 32 bytes aligned */
        context->dma->node[i].dma_cfg.srcDmaAddr = (uint32_t)AUDIO_RX_FIFO_ADDR;
        context->dma->node[i].dma_cfg.destDmaAddr = (uint32_t)hw_buff_addr + i * node_size;
        context->dma->node[i].dma_cfg.dmaCtrl = audio_dma_ctrl;
        // context->dma->node[i].dirty = 1;// 0-dirty; 1-clear; 2-valid data
        // MSP_LOGD("def %d dirty->1\r\n", i);

        if (i == dma_node_num - 1) {
            context->dma->node[i].dma_cfg.nextLLI = (uint32_t)(&(context->dma->node[0].dma_cfg));
        } else {
            context->dma->node[i].dma_cfg.nextLLI = (uint32_t)(&(context->dma->node[i + 1].dma_cfg));
        }
    }
    context->head = context->dma->node[0].dma_cfg.destDmaAddr;
#if 1// dump dma config
    for (i = 0; i < dma_node_num; i++) {
        MSP_LOGI("node[%d] 0x%08lx --> src:0x%08lx dest:0x%08lx, next:0x%08lx\r\n",
                i,
                (uint32_t)(&(context->dma->node[i].dma_cfg)),
                (uint32_t)context->dma->node[i].dma_cfg.srcDmaAddr,
                (uint32_t)context->dma->node[i].dma_cfg.destDmaAddr,
                context->dma->node[i].dma_cfg.nextLLI);
    }
#endif

    msp_cache_flush((uint32_t *)hw_node_addr, (uint32_t)hw_buff_addr + dma_node_num * node_size - 1 - (uint32_t)(hw_node_addr));

    dma_id = context->ctrl_id;
    dma_ch = context->ch_id;
    //user_log("dma_id = %d dma_ch = %d\r\n", dma_id, dma_ch);

    msp_dma_set_clk(dma_id, dma_ch);
    msp_DMA_Enable(dma_id);
    msp_DMA_Channel_Disable(dma_id, dma_ch);

    msp_DMA_LLI_Init(dma_id, dma_ch, (DMA_LLI_Cfg_Type *)&lli_cfg_dma);
    msp_DMA_LLI_Update(dma_id, dma_ch, (uint32_t)(&(context->dma->node[0].dma_cfg)));
    msp_DMA_IntMask(dma_id, dma_ch, DMA_INT_ALL, MASK);

    msp_dma_irq_function(dma_id, dma_ch, &_aui_rx_dma_irq_hander, (void *)(context));
    // user_log("&_aui_rx_dma_irq_hander:%p\r\n", _aui_rx_dma_irq_hander);
    msp_DMA_IntMask(dma_id, dma_ch, DMA_INT_TCOMPLETED, UNMASK);

    MSP_LOGI("pbuf:%p, ringbuffer:%p\r\n", pbuf, context->ringbuffer);
    MSP_LOGI("ringbuffer widx:%ld-%ld, ridx:%ld-%ld, buf:%p, bufsize:%ld\r\n",
            context->ringbuffer->write_index,
            context->ringbuffer->write_mirror,
            context->ringbuffer->read_index,
            context->ringbuffer->read_mirror,
            context->ringbuffer->buffer_ptr,
            context->ringbuffer->buffer_size);

    MSP_LOGI("hw_node_addr:%p, hw_buff_addr:%p, dma_id:%d,ch:%d, node_size:%ld, dma_node_num:%ld\r\n",
            hw_node_addr, hw_buff_addr, dma_id, dma_ch, node_size, dma_node_num);
    return 0;
}

static int _aui_rx_dma_link_destroy(aui_ch_t *context, void *dma)
{
    uint8_t dma_id, dma_ch;

    if (NULL == context) {
        MSP_LOGE("error!\r\n");
        return -1;
    }

    dma_id = context->ctrl_id;
    dma_ch = context->ch_id;

    msp_DMA_Channel_Disable(dma_id, dma_ch);
    msp_DMA_IntMask(dma_id, dma_ch, DMA_INT_ALL, MASK);

    // buffer from app
    // mringbuffer_destroy(context->ringbuffer);

    return 0;
}

int aui_rx_dma_link(aui_ch_t *context, void *dma)
{
    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);

    MSP_LOGI("context = %p\r\n", context);

    if (dma) {
        _aui_rx_dma_link(context, dma);
    } else {
        _aui_rx_dma_link_destroy(context, dma);
    }

    msp_mutex_unlock(&(context->mutex));

    return 0;
}

// 返回CODEC输入缓冲区内可用数据量(用户根据该数值了解到还可以read多少)数据量(bytes)
uint32_t aui_buffer_avail(aui_ch_t *context)
{
    uint32_t len;

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    len = mringbuffer_data_len(context->ringbuffer);
    msp_mutex_unlock(&(context->mutex));

    return len;
}

// 返回CODEC输入缓冲区内已用数据量(可用于dma搬运的)数据量(bytes)
uint32_t aui_buffer_remain(aui_ch_t *context)
{
    uint32_t len;

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    len = mringbuffer_space_len(context->ringbuffer);
    msp_mutex_unlock(&(context->mutex));

    return len;
}
// 暂停dma，清空buffer里面的数据
uint32_t aui_buffer_reset(aui_ch_t *context)
{
    int i;
    uint8_t dma_id, dma_ch;
    uint32_t node_size = context->per_node_size;

#if CODEC_INPUT_DEBUG_TRACE
    context->debug.count_reset++;
#endif
    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);

    MSP_LOGI("context = %p\r\n", context);
    dma_id = context->ctrl_id;
    dma_ch = context->ch_id;

    _aui_rx_fifo_disable(context->sound_channel_num);
    msp_DMA_Channel_Disable(dma_id, dma_ch);
    Audio_RxFifoClear();

    for (i = 0; i < context->maxcount; i ++) {
        // context->dma->node[i].dirty = 1;// 0-dirty; 1-clear; 2-valid data
        memset((void *)context->dma->node[i].dma_cfg.destDmaAddr, 0, node_size);
        msp_cache_flush((uint32_t *)context->dma->node[i].dma_cfg.destDmaAddr, node_size);
    }

    context->st = 0;
    context->pre_indx = 0;

    mringbuffer_reset(context->ringbuffer);

    msp_mutex_unlock(&(context->mutex));

    return 0;
}

static void _aui_hpf_cfg(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(AUDIO_BASE, AUDIO_ADC_1);

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUDIO_ADC_0_K1, 3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUDIO_ADC_1_K1, 3);
    //tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUDIO_ADC_2_K1, 3);

    BL_WR_REG(AUDIO_BASE, AUDIO_ADC_1, tmpVal);
}

static int __aui_hw_start(aui_ch_t *context)
{
    MSP_LOGI("context = %p\r\n", context);
    //L1C_DCache_Clean_Invalid_All();

    // add aduio_ckg_en
    _aui_hpf_cfg();

    Audio_RxFifoClear();
    _aui_rx_fifo_ensable(context->sound_channel_num);
    // clean underrun overrun
    msp_DMA_Channel_Enable(context->ctrl_id, context->ch_id);

    Audio_Ckg_En();

    return 0;
}

int aui_start(aui_ch_t *context)
{
#if CODEC_INPUT_DEBUG_TRACE
    context->debug.count_start++;
#endif
    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    MSP_LOGI("context = %p\r\n", context);
    __aui_hw_start(context);

    msp_mutex_unlock(&(context->mutex));
    return 0;
}

int aui_stop(aui_ch_t *context)
{
    uint32_t count = 0;

#if CODEC_INPUT_DEBUG_TRACE
    context->debug.count_stop++;
#endif
    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    MSP_LOGI("context = %p\r\n", context);

    _aui_rx_fifo_disable(context->sound_channel_num);
    msp_DMA_Channel_Disable(context->ctrl_id, context->ch_id);

    Audio_ADC_Disable(AUDIO_ADC_0);
    Audio_ADC_Disable(AUDIO_ADC_1);
    Audio_ADC_Disable(AUDIO_ADC_2);

    Audio_Analog_Adc_Disable();

    if (context->task_exit == 0) {
        msp_event_set(&context->event, EVENT_AU_STOP, 0);

        while (0 == context->task_exit) {
            count++;
            msp_msleep(1);
            if (count > 20000) {// wait 20S
                MSP_LOGE("context->task_exit timeout !!!!!!!!!!!!!!!!!!!!\r\n");
            }
        }

        msp_event_free(&context->event);
        msp_mutex_free(&context->mutex);
    }

    context->st = 0;
    context->pre_indx = 0;

    msp_mutex_unlock(&(context->mutex));

    return 0;
}

int aui_pause(aui_ch_t *context)
{
#if CODEC_INPUT_DEBUG_TRACE
    context->debug.count_pause++;
#endif
    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    msp_DMA_Channel_Disable(context->ctrl_id, context->ch_id);
#if 0
    uint32_t tmpVal = 0;
    user_log("context = %p\r\n", context);

    tmpVal = BL_RD_REG(AUDIO_BASE, AUDIO_AUD_ITF);

    user_log("1itf:%x\r\n", tmpVal);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUDIO_DAC_ITF_EN, 0);
    BL_WR_REG(AUDIO_BASE, AUDIO_AUD_ITF, tmpVal);
    tmpVal = BL_RD_REG(AUDIO_BASE, AUDIO_AUD_ITF);
    user_log("2irf:%x\r\n", tmpVal);
#endif
    msp_mutex_unlock(&(context->mutex));
    return 0;
}

int aui_resume(aui_ch_t *context)
{
#if CODEC_INPUT_DEBUG_TRACE
    context->debug.count_resume++;
#endif
    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    msp_DMA_Channel_Enable(context->ctrl_id, context->ch_id);
#if 0
    uint32_t tmpVal = 0;
    user_log("context = %p\r\n", context);

    tmpVal = BL_RD_REG(AUDIO_BASE, AUDIO_AUD_ITF);

    user_log("3itf:%x\r\n", tmpVal);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AUDIO_DAC_ITF_EN, 1);
    BL_WR_REG(AUDIO_BASE, AUDIO_AUD_ITF, tmpVal);
tmpVal = BL_RD_REG(AUDIO_BASE, AUDIO_AUD_ITF);
    user_log("4itf:%x\r\n", tmpVal);
#endif
    msp_mutex_unlock(&(context->mutex));
    return 0;
}

uint32_t aui_read(aui_ch_t *context, const void *data, uint32_t size)
{
    uint32_t ret = 0, rel_size = 0;
    uint32_t start_idx, end_idx;
    uint32_t node_size = context->per_node_size;
    int i;

#if CODEC_INPUT_DEBUG_TRACE
    context->debug.count_read++;
#endif
    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);

    ret = mringbuffer_data_len(context->ringbuffer);

    /* less data */
    if (ret > size) {
        rel_size = size;
    } else {
        rel_size = ret - ret % 6;
        //MSP_LOGW("ret:%d, size:%d\r\n", ret, size);
    }
    if (rel_size == 0) {
        msp_mutex_unlock(&(context->mutex));
        return 0;
    }

    /* before read, we should invalid cache */
    MSP_LOGD("mringbuffer_get_invalid_cache:%d\r\n", rel_size);
    ret = mringbuffer_get_invalid_cache(
        context->ringbuffer,
        (uint8_t *)data, rel_size,
        msp_cache_remove);
    if (ret != rel_size) {
        MSP_LOGE("never run here!\r\n");// fixme here is assert.
    }

#if RAMDUMP_CONFIG_DEBUG
    if (rel_size + g_rawaui_data_len > RAW_AUO_DUMP_DATA_LEN) {
    } else {
        memcpy(g_rawaui_data + g_rawaui_data_len, (uint8_t*)data, rel_size);
        g_rawaui_data_len += rel_size;
        //MSP_LOGD("g_rawaui_data_len:%d\r\n", g_rawaui_data_len);
    }
#endif

    msp_mutex_unlock(&(context->mutex));

#if CODEC_INPUT_DEBUG_TRACE
    context->debug.bytes_read += rel_size;
#endif

    return rel_size;
}

uint32_t aui_read_async(aui_ch_t *context, const void *data, uint32_t size)
{
    return aui_read(context, data, size);
}

/*
micgain

micgain analog 1 27
micgain analog 2 27
micgain analog 3 0

micgain digital 1 27
micgain digital 2 27
micgain digital 3 0
*/
void cmd_micgain(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int digital = 1;
    int ch;
    int val;

    if (1 == argc) {
        // ====================== mic ======================
        MSP_LOGI("mic config:\r\n");
        // mic input
#if 0
        MSP_LOGI("mic analog ch1 = %ld\r\n", g_audio_analog_gain1);
        MSP_LOGI("mic analog ch2 = %ld\r\n", g_audio_analog_gain2);
        MSP_LOGI("mic analog ch3 = %ld\r\n", g_audio_analog_gain3);
        MSP_LOGI("mic digital ch1 = %ld\r\n", g_audio_digital_gain1);
        MSP_LOGI("mic digital ch2 = %ld\r\n", g_audio_digital_gain2);
        MSP_LOGI("mic digital ch3 = %ld\r\n", g_audio_digital_gain3);
#endif
        // mic reg
        MSP_LOGI("mic a ch1 reg:0x20055710 ---> 0x%08x -> (8->11):%d\r\n", (*(uint32_t *)0x20055710), ((*(uint32_t *)0x20055710)>>8)&0xF);
        MSP_LOGI("mic a ch2 reg:0x20055714 ---> 0x%08x -> (8->11):%d\r\n", (*(uint32_t *)0x20055714), ((*(uint32_t *)0x20055714)>>8)&0xF);
        MSP_LOGI("mic a ch3 reg:0x20055718 ---> 0x%08x -> (8->11):%d\r\n", (*(uint32_t *)0x20055718), ((*(uint32_t *)0x20055718)>>8)&0xF);
        MSP_LOGI("mic d ch1 reg:0x20055408 ---> 0x%08x -> (13->21):0x%x, 0x%x\r\n", (*(uint32_t *)0x20055408), (((*(uint32_t *)0x20055408)>>13)&0X1FF), (((*(uint32_t *)0x20055408)>>13)&0XFF));
        MSP_LOGI("mic d ch2 reg:0x20055410 ---> 0x%08x -> (13->21):0x%x, 0x%x\r\n", (*(uint32_t *)0x20055410), (((*(uint32_t *)0x20055410)>>13)&0X1FF), (((*(uint32_t *)0x20055410)>>13)&0XFF));
        MSP_LOGI("mic d ch3 reg:0x20055418 ---> 0x%08x -> (13->21):0x%x, 0x%x\r\n", (*(uint32_t *)0x20055418), (((*(uint32_t *)0x20055418)>>13)&0X1FF), (((*(uint32_t *)0x20055418)>>13)&0XFF));
        // mic vloume update flag
        MSP_LOGI("mic d s012 update_flag reg:0x2005540c ---> 0x%08x -> (0->0):%d\r\n", (*(uint32_t *)0x2005540c), ((*(uint32_t *)0x2005540c)>>0)&0x1);
        MSP_LOGI("mic d s  0 update_flag reg:0x20055410 ---> 0x%08x -> (0->0):%d\r\n", (*(uint32_t *)0x20055410), ((*(uint32_t *)0x20055410)>>0)&0x1);
        MSP_LOGI("mic d s  1 update_flag reg:0x20055418 ---> 0x%08x -> (0->0):%d\r\n", (*(uint32_t *)0x20055418), ((*(uint32_t *)0x20055418)>>0)&0x1);

        // ====================== speak ======================
        MSP_LOGI("speak config:\r\n");
        // speak output
        MSP_LOGI("speak a reg:0x20055708 ---> %08x -> (16->18):%d\r\n", (*(uint32_t *)0x20055708), ((*(uint32_t *)0x20055708)>>16)&0x7);
        MSP_LOGI("speak a reg:0x20055708 ---> %08x -> ( 0-> 2):%d\r\n", (*(uint32_t *)0x20055708), ((*(uint32_t *)0x20055708)>>0)&0x7);
        MSP_LOGI("speak d reg:0x20055434 ---> %08x -> (13->21):%d\r\n", (*(uint32_t *)0x20055434), ((*(uint32_t *)0x20055434)>>13)&0x1FF);
        MSP_LOGI("speak d reg:0x2005543c ---> %08x -> (13->21):%d\r\n", (*(uint32_t *)0x2005543c), ((*(uint32_t *)0x2005543c)>>13)&0x1FF);
        // speak vloume update flag
        MSP_LOGI("speak d s 12 update flag_reg:0x20055438 ---> 0x%08x -> ( 0-> 0):%d\r\n", (*(uint32_t *)0x20055438), ((*(uint32_t *)0x20055438)>>0)&0x1);
        MSP_LOGI("speak d s  1 update flag_reg:0x20055434 ---> 0x%08x -> (12->12):%d\r\n", (*(uint32_t *)0x20055434), ((*(uint32_t *)0x20055434)>>12)&0x1);
        MSP_LOGI("speak d s  2 update flag_reg:0x2005543c ---> 0x%08x -> (12->12):%d\r\n", (*(uint32_t *)0x2005543c), ((*(uint32_t *)0x2005543c)>>12)&0x1);

        return;
    }

    if (4 != argc) {
        MSP_LOGE("arg err\r\n");
        return;
    }

    if (strcmp(argv[1], "digital") == 0) {
        digital = 1;
    } else if (strcmp(argv[1], "analog") == 0) {
        digital = 0;
    } else {
        MSP_LOGE("arg error.\r\n");
        return;
    }

    ch = atoi(argv[2]);
    val = atoi(argv[3]);

    MSP_LOGI("micgain ch:%d gain:%d\r\n", ch, val);

    if (0 == digital) {
        if (ch == 1) {
            g_audio_analog_gain1 = (uint32_t)val;
            mic_analog_gain_set(0, g_audio_analog_gain1);
        } else if (ch == 2) {
            g_audio_analog_gain2 = (uint32_t)val;
            mic_analog_gain_set(1, g_audio_analog_gain2);
        } else if (ch == 3) {
            g_audio_analog_gain3 = (uint32_t)val;
            mic_analog_gain_set(2, g_audio_analog_gain3);
        }
    } else {
        // update global val
        if (ch == 1) {
            g_audio_digital_gain1 = val;
        } else if (ch == 2) {
            g_audio_digital_gain2 = val;
        } else if (ch == 3) {
            g_audio_digital_gain3 = val;
        }

        if (val < 0) {
            //正增益*2 ， 负增益*2+512
            val = val * 2 + 512;
        } else {
            //正增益*2 ， 负增益*2+512
            val = val * 2;
        }
        if (ch == 1) {
            Audio_ADC_Set_Volume(DISABLE, 0, val);
        } else if (ch == 2) {
            Audio_ADC_Set_Volume(DISABLE, 1, val);
        } else if (ch == 3) {
            Audio_ADC_Set_Volume(DISABLE, 2, val);
        }
    }
}

int aui_attach_callback(aui_ch_t *context, aui_cb_t callback, void *arg)
{
    context->callback = callback;
    context->arg = arg;

    return 0;
}

#if CODEC_INPUT_DEBUG_TRACE
void aui_debug_dump(void)
{
    aui_ch_t *context = g_aui_ctx;

    if (NULL == context) {
        MSP_LOGI("context invalid\r\n");
        return;
    }
    MSP_LOGI("aui->debug.count_init   = %ld\r\n", context->debug.count_init);
    MSP_LOGI("aui->debug.count_config = %ld\r\n", context->debug.count_config);
    MSP_LOGI("aui->debug.count_reset  = %ld\r\n", context->debug.count_reset);
    MSP_LOGI("aui->debug.count_start  = %ld\r\n", context->debug.count_start);
    MSP_LOGI("aui->debug.count_stop   = %ld\r\n", context->debug.count_stop);
    MSP_LOGI("aui->debug.count_read   = %ld\r\n", context->debug.count_read);
    MSP_LOGI("aui->debug.count_pause  = %ld\r\n", context->debug.count_pause);
    MSP_LOGI("aui->debug.count_resume = %ld\r\n", context->debug.count_resume);
    MSP_LOGI("aui->debug.bytes_read   = %ld\r\n", context->debug.bytes_read);
    MSP_LOGI("aui->debug.bytes_write  = %ld\r\n", context->debug.bytes_write);

    MSP_LOGI("ri:0x%08lx, wi:0x%08lx, rm:%ld, wm:%ld, rb_data_len:%ld\r\n",
                context->ringbuffer->read_index, context->ringbuffer->write_index,
                context->ringbuffer->read_mirror, context->ringbuffer->write_mirror,
                mringbuffer_data_len(context->ringbuffer));
}

void aui_dma_dump(void)
{
    __disable_irq();
    msp_DMA_Channel_Disable(g_aui_ctx->ctrl_id, g_aui_ctx->ch_id);
    MSP_LOGI("g_aui_rb_buf:%p, g_aui_rb_bufsize:0x%08lx\r\n", g_aui_rb_buf, g_aui_rb_bufsize);
    MSP_LOGI("buf:%p, buf_size:0x%08lx\r\n", g_aui_ctx->buffer, g_aui_ctx->buffer_size);
    while(1);
}
#endif
