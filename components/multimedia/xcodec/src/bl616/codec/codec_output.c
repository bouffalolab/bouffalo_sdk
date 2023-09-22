/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <msp_sys.h>
#include <msp_port.h>
#include <msp_bl616_dma.h>
#include <msp_bl616_aupwm.h>
#include <msp_bl616_glb.h>
#include "codec_output.h"
#include "codec_debug.h"
#include <bflb_dma.h>
#include <msp_port.h>

#include <msp/kernel.h>
#include <xutils/debug.h>
#define AUDIO_CONFIG_DEBUG        (0)

#if AUDIO_CONFIG_DEBUG

extern long long msp_now_ms(void);
#define user_log(M, ...) do {  printf("[%9ld]======[%s:%d] " M "",  \
                                 (uint32_t)msp_now_ms(), __func__, __LINE__,            \
                                 ##__VA_ARGS__);                    \
                               } while(0==1)
#else
#define user_log(M, ...)
#endif


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
            msp_cache_flush((uint32_t *)context->ringbuffer->buffer_ptr, get_size);
        } else if (ar <= wi) {
            get_size = context->ringbuffer->buffer_size - (ri - ar);
            memset(context->ringbuffer->buffer_ptr + ri, 0, context->ringbuffer->buffer_size - ri);
            msp_cache_flush((uint32_t *)(context->ringbuffer->buffer_ptr + ri), context->ringbuffer->buffer_size - ri);
            memset(context->ringbuffer->buffer_ptr, 0, ar);
            msp_cache_flush((uint32_t *)context->ringbuffer->buffer_ptr, ar);
        } else {
            memset(context->ringbuffer->buffer_ptr, 0, context->ringbuffer->buffer_size);
            msp_cache_flush((uint32_t *)context->ringbuffer->buffer_ptr, context->ringbuffer->buffer_size);
        }
    } else {
        if ((ar > ri) && (ar <= wi)) {
            get_size = ar - ri;
            memset(context->ringbuffer->buffer_ptr + ri, 0, get_size);
            msp_cache_flush((uint32_t *)(context->ringbuffer->buffer_ptr + ri), get_size);
        } else {
            memset(context->ringbuffer->buffer_ptr, 0, context->ringbuffer->buffer_size );
            msp_cache_flush((uint32_t *)context->ringbuffer->buffer_ptr, context->ringbuffer->buffer_size);
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
                //_auo_clean_used_buffer(arg);
                _auo_recv(arg);
                msp_mutex_unlock(&(context->mutex));
            }
        }
    }
    msp_event_free(&context->event);
    msp_mutex_free(&context->mutex);
    context->task_exit = 1;
    msp_task_exit(0);
}

static void _auo_tx_dma_irq_hander(void *arg)
{
    auo_ch_t *context = (auo_ch_t *)arg;
    if (NULL != context) {
        msp_event_set(&context->event, EVENT_AU_ISR, MSP_EVENT_OR);
    }
}

#if !CONFIG_CODEC_USE_I2S_TX 
static void audiopwm_analog_digital_init(uint32_t sample_rate, uint32_t channel_num, uint32_t mode)
{
    AUPWM_Cfg_Type aupwm_cfg = {
        AUPWM_PWM_48K,
        ENABLE,
    };

    AUPWM_FifoCfg_Type aupwm_fifo_cfg = {
        ENABLE,
        AUPWM_MIXER_ONLY_LEFT,
        AUPWM_FIFO_AILGN_MSB_AT_BIT15,
        7,
        AUPWM_DRQ_EQUEL_TO_IRQ,
    };

    AUPWM_Volume_Cfg_Type aupwm_volume_cfg = {
        .muteMode = AUPWM_MUTE_DIRECTLY,
        .muteRmpdnRate = AUIDO_RAMP_RATE_2_FS,
        .muteRmpupRate = AUIDO_RAMP_RATE_2_FS,
        .volumeCtrMode = AUIDO_VOLUME_UPDATE_FORCE,
        .volumeZeroCrossRate = AUIDO_ZERO_CROSS_RATE_2_FS,
        .volumeRmpRate = AUIDO_RAMP_RATE_2_FS,
        .volumeZeroCrossTimeout = AUIDO_ZERO_CROSS_RATE_2_FS,
    };

#if 0
    //move to init
    AUPWM_Disable();
    AUPWM_Enable();
    msp_config_audiopll();// pll ,fixme ungate power ?
#endif
    AUPWM_VolumeConfig(&aupwm_volume_cfg);

    //AUPWM_SetVolume((uint16_t)(int16_t)(0));
    if (channel_num == 1) {
        aupwm_fifo_cfg.mixType = AUPWM_MIXER_ONLY_LEFT; 
    } else if (channel_num == 2) {
        aupwm_fifo_cfg.mixType = AUPWM_MIXER_L_PLUS_R_DIV_2; 
    } else {
        printf("not support channel:%d\r\n", channel_num);
    }

    switch (sample_rate) {
        case 48000:
            aupwm_cfg.pwm_mode = AUPWM_PWM_48K;
            break;
        case 32000:
            aupwm_cfg.pwm_mode = AUPWM_PWM_32K;
            break;
        case 24000:
            aupwm_cfg.pwm_mode = AUPWM_PWM_24K;
            break;
        case 16000:
            aupwm_cfg.pwm_mode = AUPWM_PWM_16K;
            break;
        case 8000:
            aupwm_cfg.pwm_mode = AUPWM_PWM_8K;
            break;
        case 44100:
            aupwm_cfg.pwm_mode = AUPWM_PWM_44P1K;
            break;
        case 22050:
            aupwm_cfg.pwm_mode = AUPWM_PWM_22P05K;
            break;
    }

    AUPWM_Init(&aupwm_cfg);
    AUPWM_FifoInit(&aupwm_fifo_cfg);
    //AUPWM_FifoDisable();
}
#endif
void audio_poweron(void)
{
    
    // default close pa
    //codec_gpio_pinmux_config(pa_pin, 0);
    msp_codec_pa_init_pre();
    
#if !CONFIG_CODEC_USE_I2S_TX 
    AUPWM_Enable();
    msp_config_audiopll();// pll ,fixme ungate power ?
#endif
    msp_msleep(10);
}

int auo_init(auo_ch_t *context)
{
    if (NULL == context) {
        user_log("error\r\n");
        return -1;
    }
#if CODEC_OUTPUT_DEBUG_TRACE
    g_auo_ctx = context;
    memset(&(context->debug), 0, sizeof(context->debug));
    context->debug.count_init++;
#endif
    user_log("context = %p\r\n", context);
    //LOGD(TAG, "%s, %d, context = %p\r\n", __func__, __LINE__, context);

    audio_poweron();
    //codec_pa_init();
    //codec_pa_disable();// gpio18
    context->task_exit = 0;
    msp_event_new(&(context->event), 0);
    msp_mutex_new(&(context->mutex));
    msp_task_new_ext(&(context->task), "auotsk", auo_task_entry, context, context->stack_size, context->task_pri);//aos_DEFAULT_APP_PRI - 6);
    //xTaskCreate(auo_task_entry, "name", context->stack_size, context, context->task_pri, context->task);
    // enable clock
    /* ungate audio */
    ms_GLB_PER_Clock_UnGate(1ULL << GLB_AHB_CLOCK_IP_AUDIO);
#if 0
    /* enable adc clock*/
    //GLB_Set_Audio_DAC_CLK(ENABLE, 19);
    Audio_Poweron_init();
#endif
    msp_codec_pa_init();
    //msp_msleep(1);

    return 0;
}

//static int _auo_tx_dma_link(auo_ch_t *context, void *arg);
int auo_channel_config(auo_ch_t *context, auo_cfg_t *config)
{
    if (NULL == context || NULL == config) {
        user_log("error!\r\n");
        return -1;
    }
#if CODEC_OUTPUT_DEBUG_TRACE
    context->debug.count_config++;
#endif
    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    user_log("context:%p,rate:%lu,ch:%lu,width:%lu\r\n",
            context,
            config->sample_rate,
            config->sound_channel_num,
            config->bit_width);

#if !CONFIG_CODEC_USE_I2S_TX 
    audiopwm_analog_digital_init(config->sample_rate, config->sound_channel_num, config->mode);
#endif
    context->bit_width = config->bit_width;
    context->buffer = config->buffer;
    context->buffer_size = config->buffer_size;
    context->per_node_size = config->per_node_size;
    context->sound_channel_num = config->sound_channel_num;
    //context->pa_pin = config->pa_pin;
    // init global value
    context->st = 0;
    context->pre_indx = 0;

    //_auo_tx_dma_link(context, 1);

    msp_mutex_unlock(&(context->mutex));

    return 0;
}

static int _auo_tx_dma_link_destroy(auo_ch_t *context, void *dma)
{
    //uint8_t dma_id, dma_ch;

    if (NULL == context) {
        user_log("error!\r\n");
        return -1;
    }

    user_log("context = %p\r\n", context);

#if 0
    dma_id = context->ctrl_id;
    dma_ch = context->ch_id;

    DMA_Channel_Disable(dma_id, dma_ch);
    DMA_IntMask(dma_id, dma_ch, DMA_INT_ALL, MASK);
    msp_dma_chan_stop(dma_ch);
#endif
    msp_free(context->priv);
    bflb_dma_channel_irq_detach(context->device_dma);
    bflb_dma_channel_stop(context->device_dma);
    //xringbuffer_destroy(context->ringbuffer);

    return 0;
}

static int _auo_tx_dma_link(auo_ch_t *context, void *dma)
{
    int i;
    uint8_t dma_id, dma_ch;
    uint32_t min_buf_size, dma_node_num, node_size;
    uint8_t *pbuf, *node_pbuf, *hw_node_addr, *hw_buff_addr;

    node_size = context->per_node_size;
#if 1
    DMA_LLI_Cfg_Type lli_cfg_dma = {
        DMA_TRNS_M2P,
        DMA_REQ_NONE,
        DMA_REQ_AUDIO_TX,
    };
    
    xcodec_dma_ch_t *dma_cfg = (xcodec_dma_ch_t *)dma;
    lli_cfg_dma.dstPeriph = dma_cfg->dma_tx_dstPeriph;
    
    struct DMA_Control_Reg audio_dma_ctrl = {
        .TransferSize = 0,
        .SBSize       = DMA_BURST_SIZE_1,
        .DBSize       = DMA_BURST_SIZE_1,
        .SWidth       = DMA_TRNS_WIDTH_16BITS,
        .DWidth       = DMA_TRNS_WIDTH_16BITS,
        .SI           = DMA_MINC_ENABLE,
        .DI           = DMA_MINC_DISABLE,
        .dst_min_mode = DISABLE,
        .dst_add_mode = DISABLE,
        .fix_cnt      = 0,
        .I            = 1,
    };
    audio_dma_ctrl.TransferSize = node_size / 2;
#endif

    if (NULL == context || NULL == context->buffer) {
        user_log("error!\r\n");
        return -1;
    }

    user_log("context = %p\r\n", context);
    /* init user buffer */
    memset(context->buffer, 0, context->buffer_size);

    dma_node_num = context->maxcount;

    /* init dma */
    node_pbuf = context->buffer;
    hw_node_addr = (uint8_t *)((intptr_t)(node_pbuf + AUDIO_ALIGNMENT_BYTES - 1) & ~AUDIO_ALIGNMENT_MASK);

    pbuf = hw_node_addr + dma_node_num * sizeof(auo_segment_t);
    hw_buff_addr = (uint8_t *)((intptr_t)(pbuf + AUDIO_ALIGNMENT_BYTES - 1) & ~AUDIO_ALIGNMENT_MASK);

    user_log("output:%p --> hw_node:%p[%lu*%u] --> hw_buf:%p[%lu*%lu]\r\n",
            node_pbuf,
            hw_node_addr,
            dma_node_num, sizeof(auo_segment_t),
            hw_buff_addr,
            dma_node_num, node_size);

    /* user buffer size must enouth for dma */
    min_buf_size = (uint32_t)(hw_buff_addr + dma_node_num * node_size - 1) - (uint32_t)(context->buffer);

    user_log("min_buf-size:%ld, %ld\r\n", min_buf_size, context->buffer_size);
    if (min_buf_size > context->buffer_size) {
        user_log("size too small!\r\n");
        while(1) {
            msp_msleep(100);
            printf("output min_buf-size:%ld, %ld\r\n", min_buf_size, context->buffer_size);
        }
        return -1;
    }
    mringbuffer_init(context->ringbuffer, hw_buff_addr, dma_node_num * node_size);
#if 1
    context->dma->node = (auo_segment_t *)hw_node_addr;

    for (i = 0; i < dma_node_num; i++) {
        /* dma nodes are head of user buffer */
        /* srcDmaAddr must by 32 bytes aligned */
        context->dma->node[i].dma_cfg.srcDmaAddr = (uint32_t)hw_buff_addr + i * node_size;
#if CONFIG_CODEC_USE_I2S_TX
        context->dma->node[i].dma_cfg.destDmaAddr = (uint32_t)(0x2000AB00 + 0x88);
#else
        context->dma->node[i].dma_cfg.destDmaAddr = (uint32_t)AUDIO_TX_FIFO_ADDR;
#endif
        context->dma->node[i].dma_cfg.dmaCtrl = audio_dma_ctrl;

        if (i == dma_node_num - 1) {
            context->dma->node[i].dma_cfg.nextLLI = (uint32_t)(&(context->dma->node[0].dma_cfg));
        } else {
            context->dma->node[i].dma_cfg.nextLLI = (uint32_t)(&(context->dma->node[i + 1].dma_cfg));
        }
    }
    context->head = context->dma->node[0].dma_cfg.srcDmaAddr;

#if 1//def OUTPUT_LOCK_BUG_DEBUG// dump dma config
    for (i = 0; i < dma_node_num; i++) {
        user_log("node[%3d] %p --> src:%p dest:%p, next:%p\r\n",
                i,
                (void *)&(context->dma->node[i].dma_cfg),
                (void *)context->dma->node[i].dma_cfg.srcDmaAddr,
                (void *)context->dma->node[i].dma_cfg.destDmaAddr,
                (void *)context->dma->node[i].dma_cfg.nextLLI);
    }
#endif

    msp_cache_flush((uint32_t *)hw_node_addr, (uint32_t)hw_buff_addr + dma_node_num * node_size - 1 - (uint32_t)(hw_node_addr));
    dma_id = context->ctrl_id;
    dma_ch = context->ch_id;
#else
    lli = (struct bflb_dma_channel_lli_pool_s *)hw_node_addr;

    msp_cache_flush((uint32_t *)hw_node_addr, (uint32_t)hw_buff_addr + dma_node_num * node_size - 1 - (uint32_t)(hw_node_addr));


    // ms_GLB_Set_DMA_CLK(ENABLE, (GLB_DMA_CLK_ID_Type)dma_ch);
    // ms_GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_DMA_0);

    struct bflb_device_s *device_dma;
    device_dma = bflb_device_get_by_name("device_dma");

    context->device_dma = device_dma;

    struct bflb_dma_channel_config_s config;

    config.direction = DMA_PERIPH_TO_MEMORY;
    config.src_req = 0;
    config.dst_req = 0;
    config.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    config.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    config.src_burst_count = DMA_BURST_INCR4;
    config.dst_burst_count = DMA_BURST_INCR4;
    config.src_width = DMA_DATA_WIDTH_16BIT;
    config.dst_width = DMA_DATA_WIDTH_16BIT;
    bflb_dma_channel_init(device_dma, &config);

    bflb_dma_channel_irq_attach(device_dma, _auo_tx_dma_irq_hander, (void *)(context));

    struct bflb_dma_channel_lli_transfer_s *transfers;

    transfers = (struct bflb_dma_channel_lli_transfer_s *)msp_malloc(sizeof(struct bflb_dma_channel_lli_transfer_s) * dma_node_num);
    context->priv = transfers;

    for (i = 0; i < dma_node_num; i ++) {
        transfers[i].src_addr = (uint32_t)AUDIO_RX_FIFO_ADDR;
        transfers[i].dst_addr = (uint32_t)hw_buff_addr + i * node_size;
        transfers[i].nbytes = node_size / 2;
    }

    bflb_dma_channel_lli_reload(device_dma, lli, dma_node_num, transfers, dma_node_num);
    bflb_dma_channel_start(device_dma);
#endif
#if 1
    dma_id = context->ctrl_id;
    dma_ch = context->ch_id;
    printf("dma_id:%d, dma_ch:%d\r\n", dma_id, dma_ch);
    
    if ((dma_id != 0) || (dma_ch > 3)) {
        printf("dma config error, dma_id:%d, dma_ch:%d\r\n", dma_id, dma_ch);
        return -1;
    }
    struct bflb_device_s *device_dma;
    char dma_name[10] = {0};
   
    sprintf(dma_name, "dma%d_ch%d", dma_id, dma_ch);
    printf("dma_dma_name:%s\r\n", dma_name);
    dma_name[9] = '\0';

    device_dma = bflb_device_get_by_name(dma_name);  

    context->device_dma = device_dma;
#if 0
    config.direction = DMA_MEMORY_TO_PERIPH;
    config.src_req = 0;
    config.dst_req = 0;
    config.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    config.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    config.src_burst_count = DMA_BURST_INCR4;
    config.dst_burst_count = DMA_BURST_INCR4;
    config.src_width = DMA_DATA_WIDTH_16BIT;
    config.dst_width = DMA_DATA_WIDTH_16BIT;
#endif
    struct bflb_dma_channel_config_s tx_config = { 
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_I2S_TX,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_16BIT,
        .dst_width = DMA_DATA_WIDTH_16BIT,
    }; 
    bflb_dma_channel_init(device_dma, &tx_config);

    bflb_dma_channel_irq_attach(device_dma, _auo_tx_dma_irq_hander, (void *)(context));
    if (dma_id == 0) {
        ms_GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_DMA_0);
    } else if (dma_id == 1) {
        ms_GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_DMA_1);
    } else {
        ms_GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_DMA_2);
    }
    DMA_Enable(dma_id);
    DMA_Channel_Disable(dma_id, dma_ch);

    //msp_dma_irq_callback_set(dma_ch, _auo_tx_dma_irq_hander, (void *)(context));
    DMA_LLI_Init(dma_id, dma_ch, (DMA_LLI_Cfg_Type *)&lli_cfg_dma);
    DMA_LLI_Update(dma_id, dma_ch, (uint32_t)(&(context->dma->node[0].dma_cfg)));
    DMA_IntMask(dma_id, dma_ch, DMA_INT_ALL, MASK);

    // msp_irq_register(MSP_DMA_IRQn, dma_interrupt_cb, (void *)(context));
    // msp_irq_enable(MSP_DMA_IRQn);
    DMA_IntMask(dma_id, dma_ch, DMA_INT_TCOMPLETED, UNMASK);
#endif
    
#if !CONFIG_CODEC_USE_I2S_TX 
    if (context->sound_channel_num == 1) {
        AUPWM_Start(AUPWM_MONO_LEFT);
    } else if (context->sound_channel_num == 2) { 
        AUPWM_Start(AUPWM_STEREO);
    } else {
        printf("not support channel:%d\r\n", context->sound_channel_num);
    }
#endif
    //_auo_irq_unmask(AUDIO_IRQn, (void *)(context));
    user_log("pbuf:%p, ringbuffer:%p\r\n", pbuf, context->ringbuffer);
    user_log("hw_node_addr:%p, hw_buff_addr:%p, dma_id:%d,ch:%d, node_size:%ld, dma_node_num:%ld\r\n",
            hw_node_addr, hw_buff_addr, dma_id, dma_ch, node_size, dma_node_num);
    return 0;
}

int auo_tx_dma_link(auo_ch_t *context, void *dma)
{
    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    user_log("context = %p, dma = %p\r\n", context, dma);

    if (dma) {
        _auo_tx_dma_link(context, dma);
#if !CONFIG_CODEC_USE_I2S_TX 
        AUPWM_FifoClear();              // fixme necessary ?
#endif
    } else {
        _auo_tx_dma_link_destroy(context, dma);
        user_log("_auo_tx_dma_link_destroy done\r\n");
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

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    user_log("context = %p\r\n", context);

    dma_id = context->ctrl_id;
    dma_ch = context->ch_id;

    //AUPWM_FifoDisable();

    bflb_dma_channel_stop(context->device_dma);
    //DMA_Channel_Disable(dma_id, dma_ch);
    AUPWM_FifoClear();

    for (i = 0; i < context->maxcount; i ++) {
        context->dma->node[i].dirty = 1;
        memset((void *)context->dma->node[i].dma_cfg.srcDmaAddr, 0, node_size);
        msp_cache_flush((uint32_t *)context->dma->node[i].dma_cfg.srcDmaAddr, node_size);
    }

    context->st = 0;
    context->pre_indx = 0;
    xringbuffer_clear(context->ringbuffer);
    user_log("context = %p, end\r\n", context);

    msp_mutex_unlock(&(context->mutex));
#endif
    return 0;
}

static int _auo_hw_start(auo_ch_t *context)
{

    msp_msleep(30);
    user_log("context = %p\r\n", context);

    //uint32_t pa_pin = context->pa_pin;
    //codec_gpio_pinmux_config(pa_pin, 1);
    //msp_msleep(context->pa_delay_ms);
    msp_codec_pa_after_opendac();
#if CONFIG_CODEC_USE_I2S_TX
    msp_i2s_tx_enable();
#endif
    DMA_Channel_Enable(context->ctrl_id, context->ch_id);

#if 1
    DMA_Request_Enable(context->ctrl_id, context->ch_id);
#else
    uint32_t val;
    val =(uint32_t)(*(volatile uint32_t *)0x2000c110);
    *(volatile uint32_t *)0x2000c110 = (uint32_t)(val & 0xFFFBFFFF);
#endif
    return 0;
}

int auo_start(auo_ch_t *context)
{
#if CODEC_OUTPUT_DEBUG_TRACE
    context->debug.count_start++;
#endif
    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    user_log("context = %p\r\n", context);
    msp_mutex_unlock(&(context->mutex));

    //_auo_hw_start(context);
    return 0;
}

int auo_stop(auo_ch_t *context)
{
    uint32_t count = 0;
#if CODEC_OUTPUT_DEBUG_TRACE
    context->debug.count_stop++;
#endif
    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);

    user_log("context = %p\r\n", context);

    //uint32_t pa_pin = context->pa_pin;
    msp_codec_pa_before_closedac();
    
#if !CONFIG_CODEC_USE_I2S_TX 
    AUPWM_Disable();
#endif
    DMA_Channel_Disable(context->ctrl_id, context->ch_id);
#if 0
    DMA_Channel_Disable(dma_id, dma_ch);
    Audio_Analog_Dac_Disable();
    Audio_DAC_Disable(AUDIO_DAC_0);
    Audio_DAC_Disable(AUDIO_DAC_1);
    if (BL_AUDIO_LEFT_CHANNEL == context->sound_channel_num) {
        Audio_TxFifoDisable(BL_AUDIO_LEFT_CHANNEL_);
    } else if (BL_AUDIO_STEREO_CHANNEL == context->sound_channel_num) {
        Audio_TxFifoDisable(BL_AUDIO_STEREO_CHANNEL_);
    } else {
    }
#endif

    context->st = 0;
    context->pre_indx = 0;

    msp_mutex_unlock(&(context->mutex));
    
    if (context->task_exit == 0) {
        msp_event_set(&context->event, EVENT_AU_STOP, 0);
        while (0 == context->task_exit) {
            count++;
            msp_msleep(10);
            if (count > 20000) {// wait 20S
                printf("context->task_exit timeout !!!!!!!!!!!!!!!!!!!!\r\n");
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
    uint8_t dma_id = context->ctrl_id;
    uint8_t dma_ch = context->ch_id;

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    user_log("context = %p\r\n", context);

    // mv to head
    auo_ringbuff_format(context);
    // update dma cfg mv 0 idx
    DMA_LLI_Update(dma_id, dma_ch, (uint32_t)(&(context->dma->node[0].dma_cfg)));

#if 1
    DMA_Request_Enable(context->ctrl_id, context->ch_id);
#else
    uint32_t val;
    val = *(volatile uint32_t *)0x2000c110;
    *(volatile uint32_t *)0x2000c110 = (val & 0xFFFBFFFF);
#endif

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
    uint32_t tmp_srcaddr;

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
        //printf(" ===== auow align_size 0\r\n");
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
#else
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

/* ================================ test code ================================ */

/* aupwm codec init */
static void _aupwm_codec_init(void)
{
    AUPWM_Cfg_Type aupwm_cfg = {
        AUPWM_PWM_16K,
        ENABLE,
    };

    AUPWM_FifoCfg_Type aupwm_fifo_cfg = {
        ENABLE,
        AUPWM_MIXER_ONLY_LEFT,
        AUPWM_FIFO_AILGN_MSB_AT_BIT15,
        7,
        AUPWM_DRQ_EQUEL_TO_IRQ,
    };

    AUPWM_Volume_Cfg_Type aupwm_volume_cfg = {
        .muteMode = AUPWM_MUTE_DIRECTLY,
        .muteRmpdnRate = AUIDO_RAMP_RATE_2_FS,
        .muteRmpupRate = AUIDO_RAMP_RATE_2_FS,
        .volumeCtrMode = AUIDO_VOLUME_UPDATE_FORCE,
        .volumeZeroCrossRate = AUIDO_ZERO_CROSS_RATE_2_FS,
        .volumeRmpRate = AUIDO_RAMP_RATE_2_FS,
        .volumeZeroCrossTimeout = AUIDO_ZERO_CROSS_RATE_2_FS,
    };

    AUPWM_Disable();
    AUPWM_Enable();

    msp_config_audiopll();

    AUPWM_VolumeConfig(&aupwm_volume_cfg);

    //AUPWM_SetVolume((uint16_t)(int16_t)(-40 * 2));

    AUPWM_Init(&aupwm_cfg);
    AUPWM_FifoInit(&aupwm_fifo_cfg);
}

#if 0
/* aupwm gpio gpio init */
void aupwm_gpio_init(uint8_t pin)
{
    GLB_GPIO_Cfg_Type gpio_cfg;

    gpio_cfg.drive = 2;
    gpio_cfg.smtCtrl = 1;
    gpio_cfg.gpioMode = GPIO_MODE_AF;
    gpio_cfg.pullType = GPIO_PULL_NONE;
    gpio_cfg.gpioFun = GPIO_FUN_AUPWM;

    gpio_cfg.gpioPin = pin;
    ms_GLB_GPIO_Init(&gpio_cfg);
}
#endif

static void _isr_dma_cb(void *arg)
{
    static uint32_t count = 0;// fixme static
    count++;
    printf("_isr_dma_cb = %ld\r\n", count);
}

#if 0
void _lowdrv_out_pcm(uint8_t *pcm_buf, uint32_t pcm_len, void *arg)
{
    #define ONE_LIST_TRANSFER_COUNT 1024
    uint32_t i, i_next;
    DMA_LLI_Cfg_Type lli_cfg_dma = {
        DMA_TRNS_M2P,
        DMA_REQ_NONE,
        DMA_REQ_AUDIO_TX,
    };
    DMA_LLI_Ctrl_Type *lli_list = NULL;

    struct DMA_Control_Reg audio_dma_ctrl = {
        .TransferSize = ONE_LIST_TRANSFER_COUNT,// real_bytes = 1024 * 4
        //.SBSize       = DMA_BURST_SIZE_1,
        //.DBSize       = DMA_BURST_SIZE_1,
        .SBSize       = DMA_BURST_SIZE_1,
        .DBSize       = DMA_BURST_SIZE_1,
        .SWidth       = DMA_TRNS_WIDTH_16BITS,
        .DWidth       = DMA_TRNS_WIDTH_16BITS,
        .SI           = DMA_MINC_ENABLE,
        .DI           = DMA_MINC_DISABLE,
        .dst_min_mode = DISABLE,
        .dst_add_mode = DISABLE,
        .fix_cnt      = 0,
        .I            = 1,
    };
    uint8_t dma_id = 0;
    uint8_t dma_ch = 0;
    uint32_t list_count = pcm_len/(ONE_LIST_TRANSFER_COUNT * 2);

    // memory
    lli_list = msp_malloc_align(sizeof(DMA_LLI_Ctrl_Type) * list_count, SOC_CACHE_ALIGN);

#if 0
    aupwm_gpio_init(GLB_GPIO_PIN_14);
    aupwm_gpio_init(GLB_GPIO_PIN_15);
#endif

    //csi_dcache_disable();

    _aupwm_codec_init();

    // dma config
    ms_GLB_Set_DMA_CLK(ENABLE, (GLB_DMA_CLK_ID_Type)dma_ch);
    ms_GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_DMA_0);
    DMA_Enable(dma_id);
    DMA_Channel_Disable(dma_id, dma_ch);

    for (i = 0; i < list_count; i++) {
        if (i == (list_count - 1)) {
            i_next = 0;
        } else {
            i_next = i + 1;
        }

        lli_list[i].srcDmaAddr = (uint32_t)pcm_buf + i*(ONE_LIST_TRANSFER_COUNT * 2);
        lli_list[i].destDmaAddr = AUDIO_TX_FIFO_ADDR;
        lli_list[i].nextLLI = (uint32_t)(&(lli_list[i_next]));
        lli_list[i].dmaCtrl= audio_dma_ctrl;
        if (i_next == 0) {
            lli_list[i].dmaCtrl.I = 1;
        }
    }

    msp_cache_flush((uint32_t *)lli_list, sizeof(DMA_LLI_Ctrl_Type) * list_count);

    DMA_LLI_Init(dma_id, dma_ch, (DMA_LLI_Cfg_Type *)&lli_cfg_dma);
    DMA_LLI_Update(dma_id, dma_ch, (uint32_t)(&(lli_list[0])));

    // enable dma int
    DMA_IntMask(dma_id, dma_ch, DMA_INT_ALL, UNMASK);
    msp_irq_register(MSP_DMA_IRQn, _isr_dma_cb, NULL);
    msp_irq_enable(MSP_DMA_IRQn);

    // audio pwm cfg
    AUPWM_Start(AUPWM_MONO_LEFT);

    // dma channel enable
    DMA_Channel_Enable(dma_id, dma_ch);
}
#endif
