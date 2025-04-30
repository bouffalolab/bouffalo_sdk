/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdio.h>
#include <string.h>

#include <msp_sys.h>
//#include <msp_dma.h>
#include <msp_bl616_dma.h>
#include <codec_input.h>

#include <msp_port.h>
#include <msp/kernel.h>
#include <bflb_dma.h>
#include <xutils/debug.h>
#define AUDIO_CONFIG_DEBUG  (0)

#if AUDIO_CONFIG_DEBUG
#define user_log(M, ...) do {  printf("[%9lu]=========[%s:%d] " M "",     \
                                 (uint32_t)(msp_now_ms()),__func__, __LINE__, \
                                 ##__VA_ARGS__);                       \
                               } while(0==1)


#else
#define user_log(M, ...)
#endif

aui_ch_t *g_aui_ctx = NULL;

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
        user_log("input res:%d, flag:%d\r\n", res, flag);
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
    msp_task_exit(0);
}

static void _aui_rx_dma_irq_hander(void *arg)
{
    aui_ch_t *context = (aui_ch_t *)arg;

    if (NULL != context) {
        msp_event_set(&context->event, EVENT_AU_ISR, MSP_EVENT_OR);
    }
}

int aui_init(aui_ch_t *context)
{
    if (NULL == context) {
        user_log("error\r\n");
        return -1;
    }

    user_log("context = %p\r\n", context);
#if CODEC_INPUT_DEBUG_TRACE
    g_aui_ctx = context;
    memset(&(context->debug), 0, sizeof(context->debug));
    context->debug.count_init++;
#endif
    context->task_exit = 0;
    msp_event_new(&(context->event), 0);
    msp_task_new_ext(&(context->task), AUI_TASK, aui_task_entry, context,
                     context->stack_size, context->task_pri);

    return 0;
}

int aui_channel_config(aui_ch_t *context, aui_cfg_t *config)
{
    if (NULL == context || NULL == config) {
        user_log("error!\r\n");
        return -1;
    }
#if CODEC_INPUT_DEBUG_TRACE
    context->debug.count_config++;
#endif
    user_log("context:%p,rate:%ld,ch:%ld,width:%ld,mode:%d\r\n",
            context,
            config->sample_rate,
            config->sound_channel_num,
            config->bit_width,
            config->mode);

    context->bit_width         = config->bit_width;
    context->buffer            = config->buffer;
    context->buffer_size       = config->buffer_size;
    context->per_node_size     = config->per_node_size;
    context->sound_channel_num = config->sound_channel_num;
    // init global value
    context->st = 0;
    context->pre_indx = 0;

    return 0;
}

static int _aui_rx_dma_link_destroy(aui_ch_t *context, void *dma)
{
    uint8_t dma_id, dma_ch;

    if (NULL == context) {
        user_log("error!\r\n");
        return -1;
    }

    user_log("context = %p\r\n", context);

    dma_id = context->ctrl_id;
    dma_ch = context->ch_id;
    msp_free(context->priv);

    bflb_dma_channel_irq_detach(context->device_dma);
    bflb_dma_channel_stop(context->device_dma);    
    //xringbuffer_destroy(context->ringbuffer);

    return 0;
}

static int _aui_rx_dma_link(aui_ch_t *context, void *dma)
{
    int i;
    uint8_t dma_id, dma_ch;
    uint32_t min_buf_size, dma_node_num, node_size;
    uint8_t *pbuf, *node_pbuf, *hw_node_addr, *hw_buff_addr;

    node_size = context->per_node_size;
    DMA_LLI_Cfg_Type lli_cfg_dma = {
        DMA_TRNS_P2M,
        DMA_REQ_SOLO_RX,
        DMA_REQ_NONE,
    };

    xcodec_dma_ch_t *dma_cfg = (xcodec_dma_ch_t *)dma;
    lli_cfg_dma.srcPeriph = dma_cfg->dma_rx_srcPeriph;
    
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
    audio_dma_ctrl.TransferSize = node_size / 2;
    if (NULL == context || NULL == context->buffer) {
        user_log("error!\r\n");
        return -1;
    }
    if (context->sound_channel_num == 1) {
        audio_dma_ctrl.SWidth = DMA_TRNS_WIDTH_16BITS;
        audio_dma_ctrl.DWidth = DMA_TRNS_WIDTH_16BITS;
    } else if (context->sound_channel_num == 2) {
        audio_dma_ctrl.SWidth = DMA_TRNS_WIDTH_32BITS;
        audio_dma_ctrl.DWidth = DMA_TRNS_WIDTH_32BITS;
    } else {

    }

    g_aui_ctx = context;

    user_log("context = %p\r\n", context);

    /* init user buffer */
    memset(context->buffer, 0, context->buffer_size);

    dma_node_num = context->maxcount;

    /* init dma */
    node_pbuf = context->buffer;
    hw_node_addr = (uint8_t *)((intptr_t)(node_pbuf + AUDIO_ALIGNMENT_BYTES - 1) & ~AUDIO_ALIGNMENT_MASK);

    pbuf = hw_node_addr + dma_node_num * sizeof(aui_segment_t);
    hw_buff_addr = (uint8_t *)((intptr_t)(pbuf + AUDIO_ALIGNMENT_BYTES - 1) & ~AUDIO_ALIGNMENT_MASK);

    user_log("input:%p --> hw_node:%p[%ld*%d] --> hw_buf:%p[%ld*%ld]\r\n",
            node_pbuf,
            hw_node_addr,
            dma_node_num, sizeof(aui_segment_t),
            hw_buff_addr,
            dma_node_num, node_size);

    /* user buffer size must enouth for dma */
    min_buf_size = (uint32_t)(hw_buff_addr + dma_node_num * node_size - 1) - (uint32_t)(context->buffer);

    user_log("min_buf-size:%ld, %ld\r\n", min_buf_size, context->buffer_size);
    if (min_buf_size > context->buffer_size) {
        user_log("size too small!\r\n");
        while(1) {
            msp_msleep(100);
            printf("input min_buf-size:%ld, %ld\r\n", min_buf_size, context->buffer_size);
        }
        return -1;
    }

    mringbuffer_init(context->ringbuffer, hw_buff_addr, dma_node_num * node_size);

    context->dma->node = (aui_segment_t *)hw_node_addr;

    for (i = 0; i < dma_node_num; i++) {
        /* dma nodes are head of user buffer */
        /* srcDmaAddr must by 32 bytes aligned */
        context->dma->node[i].dma_cfg.srcDmaAddr = (uint32_t)(0x2000AB00 + 0x8C);
        context->dma->node[i].dma_cfg.destDmaAddr = (uint32_t)hw_buff_addr + i * node_size;
        context->dma->node[i].dma_cfg.dmaCtrl = audio_dma_ctrl;

        if (i == dma_node_num - 1) {
            context->dma->node[i].dma_cfg.nextLLI = (uint32_t)(&(context->dma->node[0].dma_cfg));
        } else {
            context->dma->node[i].dma_cfg.nextLLI = (uint32_t)(&(context->dma->node[i + 1].dma_cfg));
        }
    }
    context->head = context->dma->node[0].dma_cfg.destDmaAddr;
#if 1// dump dma config
    for (i = 0; i < dma_node_num; i++) {
        user_log("node[%d] 0x%08lx --> src:0x%08lx dest:0x%08lx, next:0x%08lx\r\n",
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
    printf("dma_id:%d, dma_ch:%d\r\n", dma_id, dma_ch);
    
    if ((dma_id != 0) || (dma_ch > 3)) {
        printf("dma config error, dma_id:%d, dma_ch:%d\r\n", dma_id, dma_ch);
        return -1;
    }
    struct bflb_device_s *device_dma;
    char dma_name[10] = {0};
   
    snprintf(dma_name, sizeof(dma_name), "dma%d_ch%d", dma_id, dma_ch);
    printf("dma_dma_name:%s\r\n", dma_name);
    dma_name[9] = '\0';

    device_dma = bflb_device_get_by_name(dma_name);
    context->device_dma = device_dma;
    struct bflb_dma_channel_config_s config;
    config.direction = DMA_PERIPH_TO_MEMORY;
    config.src_req = DMA_REQUEST_I2S_RX;
    config.dst_req = DMA_REQUEST_NONE;
    config.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    config.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    config.src_burst_count = DMA_BURST_INCR4;
    config.dst_burst_count = DMA_BURST_INCR4;
    config.src_width = DMA_DATA_WIDTH_32BIT;
    config.dst_width = DMA_DATA_WIDTH_32BIT;
    bflb_dma_channel_init(device_dma, &config);

    bflb_dma_channel_irq_attach(device_dma, _aui_rx_dma_irq_hander, (void *)(context));
    msp_DMA_Enable(dma_id);
    msp_DMA_Channel_Disable(dma_id, dma_ch);

    //msp_dma_irq_callback_set(dma_ch, _aui_rx_dma_irq_hander, (void *)(context));

    msp_DMA_LLI_Init(dma_id, dma_ch, (DMA_LLI_Cfg_Type *)&lli_cfg_dma);
    msp_DMA_LLI_Update(dma_id, dma_ch, (uint32_t)(&(context->dma->node[0].dma_cfg)));
    msp_DMA_IntMask(dma_id, dma_ch, DMA_INT_ALL, MASK);

    // msp_irq_register(MSP_DMA_IRQn, dma_interrupt_cb, (void *)(context));//_aui_rx_dma_irq_hander
    // msp_irq_enable(MSP_DMA_IRQn);
    msp_DMA_IntMask(dma_id, dma_ch, DMA_INT_TCOMPLETED, UNMASK);
    user_log("init_src:%x\r\n", (*((volatile uint32_t *)(uintptr_t)(context->dma->src_addr))));
    
    //_auo_irq_unmask(AUDIO_IRQn, (void *)(context));
    user_log("pbuf:%p, ringbuffer:%p\r\n", pbuf, context->ringbuffer);
    user_log("ringbuffer widx:%ld-%ld, ridx:%ld-%ld, buf:%p, bufsize:%ld\r\n",
            context->ringbuffer->write_index,
            context->ringbuffer->write_mirror,
            context->ringbuffer->read_index,
            context->ringbuffer->read_mirror,
            context->ringbuffer->buffer_ptr,
            context->ringbuffer->buffer_size);
    user_log("hw_node_addr:%p, hw_buff_addr:%p, dma_id:%d,ch:%d, node_size:%ld, dma_node_num:%ld\r\n",
            hw_node_addr, hw_buff_addr, dma_id, dma_ch, node_size, dma_node_num);
    
    return 0;
}

int aui_rx_dma_link(aui_ch_t *context, void *dma)
{
    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);

    user_log("context = %p\r\n", context);

    if (dma) {
        _aui_rx_dma_link(context, dma);
    } else {
        _aui_rx_dma_link_destroy(context, dma);
        user_log("_aui_tx_dma_link_destroy done\r\n");
    }

    msp_mutex_unlock(&(context->mutex));

    return 0;
}

// 返回CODEC输出缓冲区内可用(用户根据该数值了解到还可以write多少)数据量 (bytes)
uint32_t aui_buffer_avail(aui_ch_t *context)
{   
    uint32_t len;

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    len = mringbuffer_data_len(context->ringbuffer);
    msp_mutex_unlock(&(context->mutex));

    return len;
}

// 返回CODEC输出缓冲区内已用(可用于dma搬运的)数据量
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

    user_log("context = %p\r\n", context);
    dma_id = context->ctrl_id;
    dma_ch = context->ch_id;

    msp_DMA_Channel_Disable(dma_id, dma_ch);
    
    for (i = 0; i < context->maxcount; i ++) {
        memset((void *)context->dma->node[i].dma_cfg.destDmaAddr, 0, node_size);
        msp_cache_flush((uint32_t *)context->dma->node[i].dma_cfg.destDmaAddr, node_size);
    }

    context->st = 0;
    context->pre_indx = 0;
    mringbuffer_reset(context->ringbuffer);
    //mringbuffer_clear(context->ringbuffer);

    msp_mutex_unlock(&(context->mutex));

    return 0;
}

static int _aui_hw_start(aui_ch_t *context)
{
    user_log("context = %p\r\n", context);
    //L1C_DCache_Clean_Invalid_All();

    // add aduio_ckg_en
    // _aui_hpf_cfg();
    msp_i2s_rx_enable();
    // clean underrun overrun
    msp_DMA_Channel_Enable(context->ctrl_id, context->ch_id);

    return 0;
}

int aui_start(aui_ch_t *context)
{
#if CODEC_INPUT_DEBUG_TRACE
    context->debug.count_start++;
#endif
    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    user_log("context = %p\r\n", context);
    _aui_hw_start(context);
    msp_mutex_unlock(&(context->mutex));
    return 0;
}

int aui_stop(aui_ch_t *context)
{
    uint32_t count = 0;

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    
    if (!msp_task_exist(AUI_TASK)) {
        user_log("aui not init\r\n");
        return -1;
    }

#if CODEC_INPUT_DEBUG_TRACE
    context->debug.count_stop++;
#endif
    
    user_log("context = %p\r\n", context);

    msp_DMA_Channel_Disable(context->ctrl_id, context->ch_id);

    if (context->task_exit == 0) {
        msp_event_set(&context->event, EVENT_AU_STOP, 0);

        while (0 == context->task_exit) {
            count++;
            msp_msleep(1);
            if (count > 20000) {// wait 20S
                user_log("context->task_exit timeout !!!!!!!!!!!!!!!!!!!!\r\n");
            }
        }

        msp_event_free(&context->event);
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
    msp_mutex_unlock(&(context->mutex));
    return 0;
}

uint32_t aui_read(aui_ch_t *context, const void *data, uint32_t size)
{
    uint32_t ret = 0, rel_size = 0;
    //uint32_t start_idx, end_idx;
    //uint32_t node_size = context->per_node_size;

#if CODEC_INPUT_DEBUG_TRACE
    context->debug.count_read++;
#endif
    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);

    ret = mringbuffer_data_len(context->ringbuffer);

    /* less data */
    if (ret >= size) {
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

int aui_mute(aui_ch_t *context, uint8_t en)
{
    user_log("context = %p\r\n", context);

    return 0;
}

int aui_attach_callback(aui_ch_t *context, aui_cb_t callback, void *arg)
{
    context->callback = callback;
    context->arg = arg;

    return 0;
}

