/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdio.h>

#include <msp_sys.h>
#include <msp_dma.h>
#include <msp_port.h>
#include <msp_bl606p_dma.h>
#include <msp_bl606p_audio.h>
#include <msp_bl606p_glb.h>
#include "codec_output.h"
#include "codec_debug.h"

#include <drv/codec.h>
#include <drv/dma.h>
#include <drv/irq.h>
#include <soc.h>

#include <msp/kernel.h>
#include <xutils/debug.h>
#include <xutils/mringbuffer.h>

volatile uint16_t digital_volume_reg = 0;
volatile uint8_t mute_flag = 0;

#if CODEC_OUTPUT_DEBUG_TRACE
auo_ch_t *g_auo_ctx = NULL;
#endif

#if 0
static int _auo_clean_buffer(auo_ch_t *context, uint32_t initial, uint32_t threshold)
{
    uint32_t i;
    uint32_t node_size = context->per_node_size;
    int res = 0;

    for (i = initial; i < threshold; i++) {
        /* normal 2, but overrun */
        if (1 == context->dma->node[i].dirty) {
            res = 1;
        }

        /* if buffer is valued, move ridx */
        if (context->dma->node[i].dirty == 2) {// 0-dirty; 1-clear; 2-valid data
            user_log("xringbuffer_read:%d\r\n", node_size);
            context->dma->node[i].dirty = 1;

            user_log("set %d dirty->1\r\n", i);
            xringbuffer_read(context->ringbuffer, NULL, node_size);
            if (context->callback) {
                context->callback(context, BL_EVENT_NODE_WRITE_COMPLETE, context->arg);
            }
        }

        memset((void *)context->dma->node[i].dma_cfg.srcDmaAddr, 0, node_size);
        msp_cache_flush((uint32_t *)context->dma->node[i].dma_cfg.srcDmaAddr, node_size);
    }

    return res;
}

static void _auo_clean_used_buffer(auo_ch_t *context)
{
    int res = 0;
    uint32_t node_size = context->per_node_size;
    uint32_t next_idx;

    next_idx = ((*((volatile uint32_t *)(uintptr_t)(context->dma->dst_addr))) - context->head) / context->per_node_size;
    context->pre_indx = ((context->ringbuffer->ridx) / node_size);

    /* dma node in sample cycle */
    if (next_idx == context->pre_indx) {
        user_log("next_idx(%d) == pre_indx(%d)\r\n", next_idx, context->pre_indx);
    } else if (next_idx > context->pre_indx) {
        res |= _auo_clean_buffer(context, context->pre_indx, next_idx);
        if (1 < (next_idx - context->pre_indx)) {
            user_log("!!!!!! auo clean %ld (%ld~%ld)\r\n",
                    (next_idx - context->pre_indx), context->pre_indx, next_idx);
        }
    } else {
    /* dma node not in sample cycle */
        res |= _auo_clean_buffer(context, context->pre_indx, context->maxcount);
        res |= _auo_clean_buffer(context, 0, next_idx);
        if (1 < ((context->maxcount - context->pre_indx) + next_idx)) {
            user_log("!!!!!! auo clean %d (%d~%d~%d)\r\n",
                    ((context->maxcount - context->pre_indx) + next_idx),
                    context->pre_indx, context->maxcount, next_idx);
        }
    }

    if (res) {
        context->callback(context, BL_EVENT_WRITE_BUFFER_EMPTY, context->arg);
        user_log("SPK buf overrun remain %ld\r\n", xringbuffer_available_read_space(context->ringbuffer));
    }
}
#endif

static void _auo_recv(auo_ch_t *context)
{
    static uint64_t old_time = 0, tmp_time = 0;
    int32_t ret;
    uint32_t dst;
    uint32_t wi, ri, ar, get_size;

#if CODEC_OUTPUT_DEBUG_TRACE
        context->debug.count_read++;
#endif
    dst = (*((volatile uint32_t *)(uintptr_t)(context->dma->dst_addr)));
    ar =  dst - context->head;
    
    wi = context->ringbuffer->write_index;
    ri = context->ringbuffer->read_index;
    
    if (ri >= wi) {
        if (ar > ri) {
            get_size = ar - ri;
            memset(context->ringbuffer->buffer_ptr + ri, 0, get_size);
        } else if (ar <= wi) {
            get_size = context->ringbuffer->buffer_size - (ri - ar);
            memset(context->ringbuffer->buffer_ptr + ri, 0, context->ringbuffer->buffer_size - ri);
            memset(context->ringbuffer->buffer_ptr, 0, ar);
        } else {
            memset(context->ringbuffer->buffer_ptr, 0, context->ringbuffer->buffer_size );
        }
    } else {
        if ((ar > ri) && (ar <= wi)) {
            get_size = ar - ri;
            memset(context->ringbuffer->buffer_ptr + ri, 0, get_size);
        } else {
            memset(context->ringbuffer->buffer_ptr, 0, context->ringbuffer->buffer_size );
        }
    }

    tmp_time = msp_now_ms();
    ret = mringbuffer_get_index(context->ringbuffer, ar);
    if (ret <= 0) {
        MSP_LOGD("ar:0x%08lx, ri:0x%08lx, wi:0x%08lx, ret:%ld\r\n",
            ar, context->ringbuffer->read_index, context->ringbuffer->write_index, ret);

        context->callback(context, BL_EVENT_WRITE_BUFFER_EMPTY, context->arg);
    } else {
#if CODEC_OUTPUT_DEBUG_TRACE
        context->debug.bytes_read += ret;
#endif
        if (context->callback) {
            context->callback(context, BL_EVENT_NODE_WRITE_COMPLETE, context->arg);
        }

        if ((ret > (context->per_node_size + context->per_node_size / 2))
            || ((old_time != 0) && ((tmp_time - old_time) > 30))) {
            MSP_LOGD("ar:0x%08lx, ri:0x%08lx, wi:0x%08lx, ret:%ld = %lldms\r\n",
                ar, context->ringbuffer->read_index, context->ringbuffer->write_index,
                ret, (tmp_time - old_time));
            MSP_LOGD("under run because of no enough data. music end ? wireless network signal instability? or enable wifi/bt but no coex?\r\n");
        } else {
            MSP_LOGD("ar:0x%08lx, ri:0x%08lx, wi:0x%08lx, ret:%ld\r\n",
                ar, context->ringbuffer->read_index, context->ringbuffer->write_index, ret);
        }
    }
    old_time = tmp_time;
}

#define EVENT_AU_ISR          (0x00000001)
#define EVENT_AU_STOP         (0x00000002)

static void auo_task_entry(void *arg)
{
    auo_ch_t *context = (auo_ch_t *)arg;
    unsigned int flag = 0;
    int res = 0;

    MSP_LOGI("auo_task_entry entry\r\n");
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
                // _auo_clean_used_buffer(arg);
                _auo_recv(arg);
                msp_mutex_unlock(&(context->mutex));
            }
        }
    }

    msp_event_free(&context->event);
    msp_mutex_free(&context->mutex);
    MSP_LOGI("auo_task_entry exit\r\n");
    context->task_exit = 1;
    msp_task_exit(NULL);
}

static void _auo_tx_dma_irq_hander(void *arg)
{
    auo_ch_t *context = (auo_ch_t *)arg;

    if (NULL != context) {
        msp_event_set(&context->event, EVENT_AU_ISR, MSP_EVENT_OR);
    }
}

static void auo_set_samplerate(int rate)
{
    int tmpVal;
    Audio_Clock_CFG_Type cfg;

    tmpVal = BL_RD_REG(AUDIO_BASE, AUDIO_AUD_TOP);
    cfg.adc_clock =  BL_GET_REG_BITS_VAL(tmpVal, AUDIO_ADC_RATE);
    cfg.dac_clock =  BL_GET_REG_BITS_VAL(tmpVal, AUDIO_DAC_RATE);
    cfg.pdm_clk_inv = BL_GET_REG_BITS_VAL(tmpVal, AUDIO_PDM_ITF_INV_SEL);
    cfg.adc_clk_inv = BL_GET_REG_BITS_VAL(tmpVal, AUDIO_ADC_ITF_INV_SEL);
    cfg.dac_clk_inv = BL_GET_REG_BITS_VAL(tmpVal, AUDIO_DAC_ITF_INV_SEL);
    if (48000 == rate) {
        if (AUDIO_DAC_48_KHZ == cfg.adc_clock) {
            return;
        }
        Audio_Clock_Init(&cfg);
    }
}

static void _auo_tx_fifo_cfg(uint32_t playBitWidth)
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

    audioFifoCfg.FifoIntThreshold = 3;
    audioFifoCfg.dmaThresholdMode = AUDIO_DRQ_EQUEL_TO_IRQ;
    audioFifoCfg.dmaEn = ENABLE;

    Audio_TxFifoConfig(&audioFifoCfg);
}

static void audio_init_digital_dac(void)
{
    Audio_Volume_Cfg_Type audioVolumeCfg = {
        .muteMode               = AUIDO_MUTE_DIRECTLY,
        .muteRmpdnRate          = AUIDO_RAMP_RATE_128_FS,//AUIDO_RAMP_RATE_2048_FS;
        .muteRmpupRate          = AUIDO_RAMP_RATE_128_FS,
        .volumeCtrMode          = AUIDO_VOLUME_UPDATE_FORCE,
        .volumeZeroCrossRate    = AUIDO_ZERO_CROSS_RATE_2_FS,//AUIDO_ZERO_CROSS_RATE_2048_FS;
        .volumeRmpRate          = AUIDO_RAMP_RATE_2_FS,//AUIDO_RAMP_RATE_2048_FS;
        .volumeZeroCrossTimeout = AUIDO_ZERO_CROSS_RATE_2_FS,//AUIDO_ZERO_CROSS_RATE_2048_FS;
    };

    Audio_FifoCfg_Type audioFifoCfg = {
        .resolution         = AUDIO_RES_16_BITS,
        .ailgnMode          = AUDIO_FIFO_AILGN_MSB_AT_BIT15,
        .FifoIntThreshold   = 3,//
        .dmaThresholdMode   = AUDIO_DRQ_EQUEL_TO_IRQ,
        .dmaEn              = ENABLE,
    };

    auo_set_samplerate(48000);

    // set mute ramp
    Audio_DAC_Software_Volume_Config(AUDIO_DAC_0, &audioVolumeCfg);
    Audio_DAC_Software_Volume_Config(AUDIO_DAC_1, &audioVolumeCfg);
    //Audio_DAC_Set_Volume_Reg(ENABLE, 0, AUDIO_DEFAULT_INITIAL_VOLUME);

    /* unmute stereo channel */
    if (0 == mute_flag) {
        Audio_DAC_Set_Mute(ENABLE, 0, AUDIO_UNMUTE);
    } else {
        Audio_DAC_Set_Mute(ENABLE, 0, AUDIO_MUTE);
    }

#if (1 == CTRL_AUDIO_DAC_ENABLE_CLK_DATA)
    Audio_DAC_Enable(AUDIO_DAC_0);
    Audio_DAC_Enable(AUDIO_DAC_1);
#endif
    Audio_Tx_Source_DAC_Sel(0, AUDIO_DAC_0);
    Audio_Tx_Source_DAC_Sel(1, AUDIO_DAC_0);

    Audio_DAC_Set_Volume(ENABLE, 0, digital_volume_reg);
    //Audio_SetDACAnalogGain(AUDIO_DAC_0, 1);// analog -3db
    //Audio_SetDACAnalogGain(AUDIO_DAC_1, 1);// analog -3db
    Audio_TxFifoDisable(3);

    Audio_TxFifoConfig(&audioFifoCfg);
    Audio_TxFifoEnable(BL_AUDIO_LEFT_CHANNEL_);   // left because hw copy at dac
    //Audio_TxFifoEnable(BL_AUDIO_STEREO_CHANNEL_); // stero

    Audio_TxFifoClear();
}

void audio_poweron(uint32_t pa_pin)
{
    // default close pa
    msp_codec_pa_init_pre(pa_pin);

    // adc + dac
    //GLB_Config_AUDIO_PLL(GLB_XTAL_40M, audioPllCfg_442P368M);
    //GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_AUDIO);

    // adc
    // msp_GLB_Set_Audio_ADC_CLK(ENABLE, 19);// delete ,because of use auto clk config

    // adc + dac
    Audio_Poweron_init();
    msp_msleep(10);

    // workaround digital dac module
    Audio_DAC_Enable(AUDIO_DAC_0);
    Audio_DAC_Enable(AUDIO_DAC_1);
    msp_msleep(1000);
}

int auo_init(auo_ch_t *context)
{
    if (NULL == context) {
        MSP_LOGE("error\r\n");
        return -1;
    }

#if CODEC_OUTPUT_DEBUG_TRACE
    g_auo_ctx = context;
    memset(&(context->debug), 0, sizeof(context->debug));
    context->debug.count_init++;
#endif
    MSP_LOGI("context = %p\r\n", context);
    uint32_t pa_pin = context->pa_pin;

    msp_mutex_new(&(context->mutex));

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    msp_codec_pa_init(pa_pin);

    context->task_exit = 0;

    msp_event_new(&(context->event), 0);
    msp_task_new_ext(&(context->task), "auotsk", auo_task_entry, context,
                     context->stack_size, context->task_pri);
    msp_mutex_unlock(&(context->mutex));

    return 0;
}

//static int _auo_tx_dma_link(auo_ch_t *context, void *arg);
int auo_channel_config(auo_ch_t *context, auo_cfg_t *config)
{
    if (NULL == context || NULL == config) {
        MSP_LOGE("error!\r\n");
        return -1;
    }

#if CODEC_OUTPUT_DEBUG_TRACE
    context->debug.count_config++;
#endif

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    MSP_LOGI("context:%p,rate:%d,ch:%d,width:%d\r\n",
            context,
            config->sample_rate,
            config->sound_channel_num,
            config->bit_width);

    /* digital init */
    //_auo_set_freq(config->sample_rate);



    audio_init_digital_dac();// AUDIO_RES_16_BITS FifoIntThreshold_3 48000
    Audio_Analog_Dac();

    /* analog init */
    //Audio_Analog_Dac();

    context->bit_width = config->bit_width;
    context->buffer = config->buffer;
    context->buffer_size = config->buffer_size;
    context->per_node_size = config->per_node_size;
    context->sound_channel_num = config->sound_channel_num;
    context->pa_pin = config->pa_pin;
    // init global value
    context->st = 0;
    context->pre_indx = 0;

    //_auo_tx_dma_link(context, 1);

    msp_mutex_unlock(&(context->mutex));

    return 0;
}

static int _auo_tx_dma_link_destroy(auo_ch_t *context, void *dma)
{
    uint8_t dma_id, dma_ch;

    if (NULL == context) {
        MSP_LOGE("error!\r\n");
        return -1;
    }

    MSP_LOGD("context = %p\r\n", context);

    dma_id = context->ctrl_id;
    dma_ch = context->ch_id;

    msp_DMA_Channel_Disable(dma_id, dma_ch);
    msp_DMA_IntMask(dma_id, dma_ch, DMA_INT_ALL, MASK);

    // mringbuffer_destroy(context->ringbuffer);

    return 0;
}

static int _auo_tx_dma_link(auo_ch_t *context, void *dma)
{
    int i;
    uint8_t dma_id, dma_ch;
    uint32_t min_buf_size, dma_node_num, node_size;
    uint8_t *pbuf, *node_pbuf, *hw_node_addr, *hw_buff_addr;

    node_size = context->per_node_size;

    DMA_LLI_Cfg_Type lli_cfg_dma = {
        DMA_TRNS_M2P,
        DMA_REQ_NONE,
        DMA_REQ_AUDIO_TX,
    };

    struct DMA_Control_Reg audio_dma_ctrl = {
        .TransferSize = 0,
        .SBSize       = DMA_BURST_SIZE_4,
        .DBSize       = DMA_BURST_SIZE_4,
        .SWidth       = DMA_TRNS_WIDTH_32BITS,
        .DWidth       = DMA_TRNS_WIDTH_32BITS,
        .SI           = DMA_MINC_ENABLE,
        .DI           = DMA_MINC_DISABLE,
        .dst_min_mode = DISABLE,
        .dst_add_mode = DISABLE,
        .fix_cnt      = 0,
        .I            = 1,
    };

    if (NULL == context || NULL == context->buffer) {
        MSP_LOGE("error!\r\n");
        return -1;
    }

    MSP_LOGD("context = %p\r\n", context);
    /* init user buffer */
    memset(context->buffer, 0, context->buffer_size);

    switch (context->bit_width) {
        case BL_AUDIO_BIT_WIDTH_16:
            audio_dma_ctrl.SWidth = DMA_TRNS_WIDTH_16BITS;    //DMA_TRNS_WIDTH_32BITS;
            audio_dma_ctrl.DWidth = DMA_TRNS_WIDTH_16BITS;
            audio_dma_ctrl.SBSize = DMA_BURST_SIZE_1;        // DMA_BURST_SIZE_4;
            audio_dma_ctrl.DBSize = DMA_BURST_SIZE_1;
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
    dma_node_num = ((context->buffer_size - 2 * AUDIO_ALIGNMENT_BYTES) / (sizeof(auo_segment_t) + node_size));
    context->maxcount = dma_node_num;

    /* init dma */
    node_pbuf = context->buffer;
    hw_node_addr = (uint8_t *)((intptr_t)(node_pbuf + AUDIO_ALIGNMENT_BYTES - 1) & ~AUDIO_ALIGNMENT_MASK);

    pbuf = hw_node_addr + dma_node_num * sizeof(auo_segment_t);
    hw_buff_addr = (uint8_t *)((intptr_t)(pbuf + AUDIO_ALIGNMENT_BYTES - 1) & ~AUDIO_ALIGNMENT_MASK);

    /* user buffer size must enouth for dma */
    min_buf_size = (uint32_t)(hw_buff_addr + dma_node_num * node_size - 1) - (uint32_t)(context->buffer);

    MSP_LOGI("appbuf:%p,hw_node:%p[%ld*%d],hw_buf:%p[%ld*%ld],rb_ptr:%p,algin use:%ld,dma:%d,%d\r\n",
            node_pbuf,
            hw_node_addr,
            dma_node_num, sizeof(auo_segment_t),
            hw_buff_addr,
            dma_node_num, node_size,
            context->ringbuffer,
            (context->buffer_size - min_buf_size),
            dma_id, dma_ch);

    if (min_buf_size > context->buffer_size) {
        MSP_LOGE("size too small!\r\n");
        while(1) {
            msp_msleep(100);
            MSP_LOGA("output min_buf-size:%ld, %ld\r\n", min_buf_size, context->buffer_size);
        }
        return -1;
    }

    mringbuffer_init(context->ringbuffer, hw_buff_addr, dma_node_num * node_size);

    context->dma->node = (auo_segment_t *)hw_node_addr;

    for (i = 0; i < dma_node_num; i++) {
        /* dma nodes are head of user buffer */
        /* srcDmaAddr must by 32 bytes aligned */
        context->dma->node[i].dma_cfg.srcDmaAddr = (uint32_t)hw_buff_addr + i * node_size;
        context->dma->node[i].dma_cfg.destDmaAddr = (uint32_t)AUDIO_TX_FIFO_ADDR;
        context->dma->node[i].dma_cfg.dmaCtrl = audio_dma_ctrl;
        context->dma->node[i].dirty = 1;

        if (i == dma_node_num - 1) {
            context->dma->node[i].dma_cfg.nextLLI = (uint32_t)(&(context->dma->node[0].dma_cfg));
        } else {
            context->dma->node[i].dma_cfg.nextLLI = (uint32_t)(&(context->dma->node[i + 1].dma_cfg));
        }
    }
    context->head = context->dma->node[0].dma_cfg.srcDmaAddr;
#if 1// dump dma config
    for (i = 0; i < dma_node_num; i++) {
        MSP_LOGD("node[%d] %p --> src:%p dest:%p, next:%p\r\n",
                i,
                &(context->dma->node[i].dma_cfg),
                context->dma->node[i].dma_cfg.srcDmaAddr,
                context->dma->node[i].dma_cfg.destDmaAddr,
                context->dma->node[i].dma_cfg.nextLLI);
    }
#endif

    msp_cache_flush((uint32_t *)hw_node_addr, (uint32_t)hw_buff_addr + dma_node_num * node_size - 1 - (uint32_t)(hw_node_addr));

    dma_id = context->ctrl_id;
    dma_ch = context->ch_id;

    msp_dma_set_clk(dma_id, dma_ch);
    msp_DMA_Enable(dma_id);
    msp_DMA_Channel_Disable(dma_id, dma_ch);

    msp_DMA_LLI_Init(dma_id, dma_ch, (DMA_LLI_Cfg_Type *)&lli_cfg_dma);
    msp_DMA_LLI_Update(dma_id, dma_ch, (uint32_t)(&(context->dma->node[0].dma_cfg)));
    msp_DMA_IntMask(dma_id, dma_ch, DMA_INT_ALL, MASK);

    msp_dma_irq_function(dma_id, dma_ch, &_auo_tx_dma_irq_hander, (void *)(context));
    //user_log("&_auo_tx_dma_irq_hander:%p\r\n", _auo_tx_dma_irq_hander);
    msp_DMA_IntMask(dma_id, dma_ch, DMA_INT_TCOMPLETED, UNMASK);

    //_auo_irq_unmask(AUDIO_IRQn, (void *)(context));

    // MSP_LOGI("hw_node_addr:%p,hw_buff_addr:%p,dma_id:%d,ch:%d,node_size:%ldB,node_num:%ld\r\n",
    //         hw_node_addr, hw_buff_addr, dma_id, dma_ch, node_size, dma_node_num);

    return 0;
}

int auo_tx_dma_link(auo_ch_t *context, void *dma)
{
    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);

    MSP_LOGI("context = %p, dma = %p\r\n",
            context, dma);

    if (dma) {
        _auo_tx_dma_link(context, dma);
#if 0
        Audio_TxFifoClear();
        if (BL_AUDIO_LEFT_CHANNEL == context->sound_channel_num) {
            Audio_TxFifoEnable(BL_AUDIO_LEFT_CHANNEL_);
        } else if (BL_AUDIO_STEREO_CHANNEL == context->sound_channel_num) {
             Audio_TxFifoEnable(BL_AUDIO_STEREO_CHANNEL_);
        } else {

        }

        Audio_TxFifoClear();

        //Audio_Ckg_En();// work clock

        msp_codec_pa_after_opendac();
#endif
    } else {
        _auo_tx_dma_link_destroy(context, dma);
    }

    msp_mutex_unlock(&(context->mutex));

    return 0;
}

// 返回CODEC输出缓冲区内可用(用户根据该数值了解到还可以write多少)数据量 (bytes)
uint32_t auo_buffer_avail(auo_ch_t *context)
{
    uint32_t len;

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    len = mringbuffer_space_len(context->ringbuffer);
    msp_mutex_unlock(&(context->mutex));

    return len;
}

// 返回CODEC输出缓冲区内已用(可用于dma搬运的)数据量
uint32_t auo_buffer_remain(auo_ch_t *context)
{
    uint32_t len;

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    len = mringbuffer_data_len(context->ringbuffer);
    msp_mutex_unlock(&(context->mutex));

    return len;
}
// 暂停dma，清空buffer里面的数据
uint32_t auo_buffer_reset(auo_ch_t *context)
{
#if 0
    int i;
    uint8_t dma_id, dma_ch;
    uint32_t node_size = context->per_node_size;

#if CODEC_OUTPUT_DEBUG_TRACE
    context->debug.count_reset++;
#endif

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    MSP_LOGI("context = %p\r\n", context);


    dma_id = context->ctrl_id;
    dma_ch = context->ch_id;

    if (BL_AUDIO_LEFT_CHANNEL == context->sound_channel_num) {
        Audio_TxFifoDisable(BL_AUDIO_LEFT_CHANNEL_);
    } else if (BL_AUDIO_STEREO_CHANNEL == context->sound_channel_num) {
         Audio_TxFifoDisable(BL_AUDIO_STEREO_CHANNEL_);
    } else {

    }

    msp_DMA_Channel_Disable(dma_id, dma_ch);
    Audio_TxFifoClear();

    for (i = 0; i < context->maxcount; i ++) {
        context->dma->node[i].dirty = 1;
        memset((void *)context->dma->node[i].dma_cfg.srcDmaAddr, 0, node_size);
        msp_cache_flush((uint32_t *)context->dma->node[i].dma_cfg.srcDmaAddr, node_size);
    }

    context->st = 0;
    // context->pre_indx = 0;
    mringbuffer_reset(context->ringbuffer);
    MSP_LOGI("context = %p, end\r\n", context);

    msp_mutex_unlock(&(context->mutex));
#endif
    return 0;
}


static int _auo_hw_start(auo_ch_t *context)
{
    uint32_t val;

    // un halt dma
    MSP_LOGD("un halt\r\n");
    val =(uint32_t)(*(volatile uint32_t *)context->dma->halt_reg);
    *(volatile uint32_t *)(context->dma->halt_reg) = (uint32_t)(val & context->dma->unhalt);

    uint32_t pa_pin = context->pa_pin;
    msp_codec_pa_after_opendac(pa_pin);

    // MSP_LOGI("context = %p\r\n", context);
    MSP_LOGI("context = %p,init:%ld,cfg:%ld,"
            // "reset:%ld,start%ld,"
            // "stop:%d,read:%d,pause:%d,resume:%d,rB:%ld,wB:%ld,"
            "ri:0x%08lx,wi:0x%08lx,data_len:%ld,ctrl_id:%d,ch_id:%d,dma enable\r\n",
            context,
            context->debug.count_init,
            context->debug.count_config,
            // context->debug.count_reset,
            // context->debug.count_start,
            // context->debug.count_stop,
            // context->debug.count_read,
            // context->debug.count_pause,
            // context->debug.count_resume,
            // context->debug.bytes_read,
            // context->debug.bytes_write,
            context->ringbuffer->read_index,
            context->ringbuffer->write_index,
            mringbuffer_data_len(context->ringbuffer),
            context->ctrl_id, context->ch_id);

    msp_DMA_Channel_Enable(context->ctrl_id, context->ch_id);

    return 0;
}

int auo_start(auo_ch_t *context)
{
#if CODEC_OUTPUT_DEBUG_TRACE
    context->debug.count_start++;
#endif
#if 0
    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    MSP_LOGI("context = %p\r\n", context);
    msp_mutex_unlock(&(context->mutex));

    //_auo_hw_start(context);
#endif
    return 0;
}

int auo_stop(auo_ch_t *context)
{
    uint32_t count = 0;

#if CODEC_OUTPUT_DEBUG_TRACE
    context->debug.count_stop++;
#endif

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);

    // MSP_LOGI("context = %p\r\n", context);
    MSP_LOGI("context = %p,init:%ld,cfg:%ld,"
            // "reset:%ld,start%ld,"
            "stop:%d,read:%d,pause:%d,resume:%d,rB:%ld,wB:%ld,"
            "ri:0x%08lx,wi:0x%08lx,data_len:%ld\r\n",
            context,
            context->debug.count_init,
            context->debug.count_config,
            // context->debug.count_reset,
            // context->debug.count_start,
            context->debug.count_stop,
            context->debug.count_read,
            context->debug.count_pause,
            context->debug.count_resume,
            context->debug.bytes_read,
            context->debug.bytes_write,
            context->ringbuffer->read_index,
            context->ringbuffer->write_index,
            mringbuffer_data_len(context->ringbuffer));

    msp_codec_pa_before_closedac(context->pa_pin);

    Audio_Analog_Dac_Disable();

    msp_DMA_Channel_Disable(context->ctrl_id, context->ch_id);

    context->st = 0;
    // context->pre_indx = 0;

    msp_mutex_unlock(&(context->mutex));
   
    if (context->task_exit == 0) {
        msp_event_set(&context->event, EVENT_AU_STOP, 0);

        while (0 == context->task_exit) {
            count++;
            msp_msleep(1);
            if (count > 20000) {// wait 20S
                MSP_LOGE("context->task_exit timeout\r\n");
            }
        }
    }

    return 0;
}

int auo_pause(auo_ch_t *context)
{
    uint32_t val;

#if CODEC_OUTPUT_DEBUG_TRACE
    context->debug.count_pause++;
#endif

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    MSP_LOGI("context = %p\r\n", context);

    /* dma halt */
    val = *(volatile uint32_t *)(context->dma->halt_reg);
    *(volatile uint32_t *)(context->dma->halt_reg) = (val | context->dma->halt);

    msp_mutex_unlock(&(context->mutex));

    return 0;
}

static int auo_ringbuff_format(auo_ch_t *context)
{
    int fmt_size;
    char *fmt_buf = NULL;
    uint32_t node_size = context->per_node_size;
    uint32_t dma_node_num = context->maxcount;

    uint8_t *tmp_buf;
    uint32_t mv_len;// mv_len_algin_node

    // get fmt_size for malloc mem
    fmt_size = mringbuffer_data_len(context->ringbuffer);

    // malloc cache
    fmt_buf = msp_malloc(fmt_size);
    if (NULL == fmt_buf) {
        MSP_LOGE("mem error\r\n");
        return -1;
    }

    // mv to cache
    mringbuffer_get(context->ringbuffer, (uint8_t *)fmt_buf, fmt_size);
    mringbuffer_reset(context->ringbuffer);

    mv_len = (fmt_size/node_size)*node_size;

    if (mv_len > 0) {
        mringbuffer_put(context->ringbuffer,
            (uint8_t *)(fmt_buf + fmt_size%node_size),
            mv_len);

        tmp_buf = (uint8_t *)((uint32_t *)context->dma->node[0].dma_cfg.srcDmaAddr);
        memset(tmp_buf + mv_len, 0, node_size * dma_node_num - mv_len);

        msp_cache_flush((uint32_t *)context->dma->node[0].dma_cfg.srcDmaAddr,
            node_size * dma_node_num);
    } else {
        memset((uint32_t *)context->dma->node[0].dma_cfg.srcDmaAddr,
            0,
            node_size * dma_node_num);
        msp_cache_flush((uint32_t *)context->dma->node[0].dma_cfg.srcDmaAddr,
            node_size * dma_node_num);
    }

    // free
    msp_free(fmt_buf);

    return 0;
}

int auo_resume(auo_ch_t *context)
{
    uint32_t val;
    uint8_t dma_id = context->ctrl_id;
    uint8_t dma_ch = context->ch_id;

#if CODEC_OUTPUT_DEBUG_TRACE
    context->debug.count_resume++;
#endif

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    MSP_LOGI("context = %p\r\n", context);

#if 1
    // mv to head
    auo_ringbuff_format(context);
    // update dma cfg mv 0 idx
    msp_DMA_LLI_Update(dma_id, dma_ch, (uint32_t)(&(context->dma->node[0].dma_cfg)));
#endif

    val = *(volatile uint32_t *)(context->dma->halt_reg);
    *(volatile uint32_t *)(context->dma->halt_reg) = (val & context->dma->unhalt);

    msp_mutex_unlock(&(context->mutex));
    return 0;
}

#if 0
#define AUDIO_OUT_ALIGN_BYTE

uint32_t auo_write(auo_ch_t *context, const void *data, uint32_t size)
{
#ifdef AUDIO_OUT_ALIGN_BYTE
    uint32_t align_size = 0;
    uint32_t available_size = 0;
#endif
    uint32_t ret = 0;
    uint32_t start_idx, end_idx;
    uint32_t node_size = context->per_node_size;
    int i;

    uint32_t widx;
    uint32_t cur_idx;

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    if (size%1024) {
        user_log("context = %p, size = %ld\r\n", context, size);
    }

    //user_log("widx1:%d\r\n", context->ringbuffer->widx);
    start_idx = ((context->ringbuffer->widx) / node_size);

#ifdef AUDIO_OUT_ALIGN_BYTE
    align_size = size;
    if (xringbuffer_available_write_space(context->ringbuffer) > (3*node_size - 1)) {
        available_size = (xringbuffer_available_write_space(context->ringbuffer) - (3*node_size - 1));
    } else {
        available_size = 0;
    }
    if (available_size < align_size) {
        align_size = available_size & 0xFFFFFFFE;// ALGIN 2 Bytes
    }
    if (0 == align_size) {
        //MSP_LOGI(" ===== auow align_size 0\r\n");
        msp_mutex_unlock(&(context->mutex));
        return 0;
    }
    ret = xringbuffer_write(context->ringbuffer, (uint8_t *)data, align_size);

    codec_debug_input(TYPE_CODEC_DEBUG_OUTPUT, (uint8_t *)data, align_size);

#else
    ret = xringbuffer_write(context->ringbuffer, (uint8_t *)data, size);
#endif

    end_idx = ((context->ringbuffer->widx) / node_size);
    //user_log("widx2:%d\r\n", context->ringbuffer->widx);
    //user_log("pre%d, now:%d\r\n", start_idx, end_idx);
    if (start_idx == end_idx) {
        user_log("!!!!!!!!! start:%d--->end:%d\r\n", start_idx, end_idx);
    } else if (start_idx < end_idx) {
        for (i = start_idx; i < end_idx; i++) {
            msp_cache_flush((uint32_t *)context->dma->node[i].dma_cfg.srcDmaAddr, node_size);
            context->dma->node[i].dirty = 2;
        }
    } else {
        for (i = start_idx; i < context->maxcount; i++) {
            msp_cache_flush((uint32_t *)context->dma->node[i].dma_cfg.srcDmaAddr, node_size);
            context->dma->node[i].dirty = 2;
        }

        for (i = 0; i < end_idx; i++) {
            msp_cache_flush((uint32_t *)context->dma->node[i].dma_cfg.srcDmaAddr, node_size);
            context->dma->node[i].dirty = 2;
        }
    }

    if (0 == context->st) {
        context->st = 1;
        _auo_hw_start(context);
        // fixme for nosie at startup
    }

    /* for debug */
    widx = (context->ringbuffer->widx);
    cur_idx = (uint32_t)((*((volatile uint32_t *)(uintptr_t)(context->dma->dst_addr))) - (uint32_t)((uint32_t *)context->dma->node[0].dma_cfg.srcDmaAddr));

    if (widx >= cur_idx) {
        user_log("off:%8ld, dma_src:%6ld, widex:%6ld, rb_avild:%6ld\r\n",
                (widx - cur_idx),
                cur_idx, widx,
                xringbuffer_available_read_space(context->ringbuffer)
                );
    } else {
        user_log("off:%8ld, dma_src:%6ld, widex:%6ld, rb_avild:%6ld\r\n",
                (context->maxcount * node_size - (cur_idx - widx)),
                cur_idx, widx,
                xringbuffer_available_read_space(context->ringbuffer)
                );
    }

    msp_mutex_unlock(&(context->mutex));
    return ret;
}
#else//mringbuffer_put_fflush_cache
uint32_t auo_write(auo_ch_t *context, const void *data, uint32_t size)
{
    uint32_t ret = 0;

    MSP_LOGD("mringbuffer_put_fflush_cache %d\r\n", size);
    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    ret = mringbuffer_put_fflush_cache(context->ringbuffer,
        (uint8_t *)data, size,
        msp_cache_flush);

    if (0 == context->st) {
        context->st = 1;
        _auo_hw_start(context);
        // fixme for nosie at startup
    }

    msp_mutex_unlock(&(context->mutex));

#if CODEC_OUTPUT_DEBUG_TRACE
    context->debug.bytes_write += ret;
#endif

    return ret;
}
#endif

uint32_t auo_write_async(auo_ch_t *context, const void *data, uint32_t size)
{
    return auo_write(context, data, size);
}

int auo_attach_callback(auo_ch_t *context, auo_cb_t callback, void *arg)
{
    context->callback = callback;
    context->arg = arg;

    return 0;
}

// todo: remove from this file
int volume2db2regval(int val)// 0~100
{
#if 1
    const float val2db[100] = {// val * 2 = real val
        0, -0.25, -0.5, -0.75, -1, -1.25, -1.5, -1.75, -2, -2.5,
        -3, -3.5, -4, -4.5, -5, -5.5, -6, -6.5, -7, -7.5,
        -8, -8.5, -9, -9.5, -10, -10.5, -11, -11.5, -12, -12.5,
        -13, -13.5, -14, -14.5, -15, -15.5, -16, -16.5, -17, -17.5,
        -18, -18.5, -19, -19.5, -20, -20.5, -21, -21.5, -22, -22.5,
        -23, -23.5, -24, -24.5, -25, -25.5, -26, -26.5, -27, -27.5,
        -28, -28.5, -29, -29.5, -30, -30.5, -31, -31.5, -32, -32.5,
        -33, -33.5, -34, -34.5, -35, -35.5, -36, -36.5, -37, -37.5,
        -38, -38.5, -39, -39.5, -40, -40.5, -41, -41.5, -42, -42.5,
        -43, -43.5, -44, -44.5, -45, -45.5, -46, -46.5, -47, -47.50
    };
#else
    const float val2db[100] = {// val * 2 = real val
        0, -0.25, -0.5, -0.5, -0.75, -0.75, -1, -1, -1.25, -1.25,
        -1.5, -1.5, -1.75, -1.75, -2, -2, -2.5, -2.5, -3, -3,
        -3.5, -3.5, -4, -4, -4.5, -4.5, -5, -5, -5.5, -5.5,
        -6, -6, -6.5, -6.5, -7, -7, -7.5,-7.5, -8, -8,
        -8.5, -8.5, -9, -9, -9.5, -9.5, -10, -10, -10.5, -10.5,
        -11, -11, -11.5, -11.5, -12, -12, -12.5,-12.5, -13, -13,
        -13.5, -13.5, -14, -14, -14.5, -14.5, -15, -15, -15.5, -15.5,
        -16, -16, -16.5, -16.5, -17, -17, -17.5,-17.5, -18, -18,
        -18.5, -19, -19.5, -20, -20.5, -21, -21.5, -22, -22.5,
        -23, -23.5, -24, -24.5, -25, -25.5, -26, -26.5, -27, -27.5,
    };
#endif
    int valtmp;
    uint16_t regval;
    uint8_t mute = 0;

    if (val > 100) {
        valtmp = 100;
    } else if (val < 0) {
        valtmp = 0;
    } else {
        valtmp = val;
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

    MSP_LOGI("val:%d, db=%f, regval = %d\r\n",
            val, val2db[100 - valtmp], regval);

    if (0 == mute) {
        mute_flag = 0;
        Audio_DAC_Set_Mute(ENABLE, 0, AUDIO_UNMUTE);
        Audio_DAC_Set_Volume(ENABLE, AUDIO_DAC_0, regval);
        digital_volume_reg = regval;
        //Audio_SetDACAnalogGain(AUDIO_DAC_1, regval);
    } else {
        mute_flag = 1;
        Audio_DAC_Set_Mute(ENABLE, 0, AUDIO_MUTE);
    }

    return 0;
}

#if CODEC_OUTPUT_DEBUG_TRACE
void auo_debug_dump(void)
{
    auo_ch_t *context = g_auo_ctx;

    if (NULL == context) {
        MSP_LOGI("context invalid\r\n");
        return;
    }
    MSP_LOGI("auo->debug.count_init   = %ld\r\n", context->debug.count_init);
    MSP_LOGI("auo->debug.count_config = %ld\r\n", context->debug.count_config);
    MSP_LOGI("auo->debug.count_reset  = %ld\r\n", context->debug.count_reset);
    MSP_LOGI("auo->debug.count_start  = %ld\r\n", context->debug.count_start);
    MSP_LOGI("auo->debug.count_stop   = %ld\r\n", context->debug.count_stop);
    MSP_LOGI("auo->debug.count_read   = %ld\r\n", context->debug.count_read);
    MSP_LOGI("auo->debug.count_pause  = %ld\r\n", context->debug.count_pause);
    MSP_LOGI("auo->debug.count_resume = %ld\r\n", context->debug.count_resume);
    MSP_LOGI("auo->debug.bytes_read   = %ld\r\n", context->debug.bytes_read);
    MSP_LOGI("auo->debug.bytes_write  = %ld\r\n", context->debug.bytes_write);

    MSP_LOGI("ri:0x%08lx, wi:0x%08lx, rm:%ld, wm:%ld, rb_data_len:%ld\r\n",
                context->ringbuffer->read_index, context->ringbuffer->write_index,
                context->ringbuffer->read_mirror, context->ringbuffer->write_mirror,
                mringbuffer_data_len(context->ringbuffer));
}
#endif
