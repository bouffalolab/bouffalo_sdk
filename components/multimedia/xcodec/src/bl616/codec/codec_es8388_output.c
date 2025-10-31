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

#if CODEC_OUTPUT_DEBUG_TRACE
auo_ch_t *g_auo_ctx = NULL;
#endif

#define ALIGN_TO_32(addr) ((addr) & ~0x1F)

static void _auo_recv(auo_ch_t *context)
{
    static uint64_t old_time = 0, tmp_time = 0;
    int32_t ret, val;
    uint32_t dst;
    uint32_t wi, ri, ar, get_size;

#if CODEC_OUTPUT_DEBUG_TRACE
        context->debug.count_read++;
#endif
    dst = (*((volatile uint32_t *)(uintptr_t)(context->dma->dst_addr)));
    ar =  ALIGN_TO_32(dst - context->head);
    wi = context->ringbuffer->write_index;
    ri = context->ringbuffer->read_index;
    if ((wi % 32 !=0) ||(ri %32 !=0)) {
        user_log("wi:%d,ri:%d\r\n", wi,ri);
    }

    ret = mringbuffer_data_len(context->ringbuffer);
    if((context->per_node_size > ret) || (context->per_node_size == ret)) {
        memset(context->ringbuffer->buffer_ptr, 0, context->ringbuffer->buffer_size);
        msp_cache_flush((uint32_t *)context->ringbuffer->buffer_ptr, context->ringbuffer->buffer_size);
        /* dma halt */
        val = *(volatile uint32_t *)(context->dma->halt_reg);
        *(volatile uint32_t *)(context->dma->halt_reg) = (val | context->dma->halt);
        context->dma->halt_flag = 1;
        context->callback(context, BL_EVENT_WRITE_BUFFER_EMPTY, context->arg);
    } else {
        if (ri >= wi) {
            if (ar > ri) {
                get_size = ar - ri;
                memset(context->ringbuffer->buffer_ptr + ri, 0, get_size);
                msp_cache_flush((uint32_t *)(context->ringbuffer->buffer_ptr + ri), get_size);
            } else if (ar <= wi) {
                get_size = context->ringbuffer->buffer_size - (ri - ar);
                memset(context->ringbuffer->buffer_ptr + ri, 0, context->ringbuffer->buffer_size - ri);
                msp_cache_flush((uint32_t *)(context->ringbuffer->buffer_ptr + ri), context->ringbuffer->buffer_size - ri);
                memset(context->ringbuffer->buffer_ptr, 0, ar);
                msp_cache_flush((uint32_t *)context->ringbuffer->buffer_ptr, ar);
            } else {
                memset(context->ringbuffer->buffer_ptr, 0, context->ringbuffer->buffer_size);
                msp_cache_flush((uint32_t *)context->ringbuffer->buffer_ptr, context->ringbuffer->buffer_size);
                /* dma halt */
                val = *(volatile uint32_t *)(context->dma->halt_reg);
                *(volatile uint32_t *)(context->dma->halt_reg) = (val | context->dma->halt);
                context->dma->halt_flag = 1;
                context->callback(context, BL_EVENT_WRITE_BUFFER_EMPTY, context->arg);
            }
        } else {
            if ((ar > ri) && (ar <= wi)) {
                get_size = ar - ri;
                memset(context->ringbuffer->buffer_ptr + ri, 0, get_size);
                msp_cache_flush((uint32_t *)(context->ringbuffer->buffer_ptr + ri), get_size);
            } else {
                memset(context->ringbuffer->buffer_ptr, 0, context->ringbuffer->buffer_size);
                msp_cache_flush((uint32_t *)context->ringbuffer->buffer_ptr, context->ringbuffer->buffer_size);
                /* dma halt */
                val = *(volatile uint32_t *)(context->dma->halt_reg);
                *(volatile uint32_t *)(context->dma->halt_reg) = (val | context->dma->halt);
                context->dma->halt_flag = 1;
                context->callback(context, BL_EVENT_WRITE_BUFFER_EMPTY, context->arg);
            }
        }
    }
    tmp_time = msp_now_ms();
    ret = mringbuffer_get_index(context->ringbuffer, ar);
    if (ret <= 0) {
        user_log("ar:0x%08lx, ri:0x%08lx, wi:0x%08lx, ret:%ld\r\n",
            ar, context->ringbuffer->read_index, context->ringbuffer->write_index, ret);

        // context->callback(context, BL_EVENT_WRITE_BUFFER_EMPTY, context->arg);
    } else {
#if CODEC_OUTPUT_DEBUG_TRACE
        context->debug.bytes_read += ret;
#endif
        if (context->callback) {
            context->callback(context, BL_EVENT_NODE_WRITE_COMPLETE, context->arg);
        }

        if ((ret > (context->per_node_size + context->per_node_size / 2))
            || ((old_time != 0) && ((tmp_time - old_time) > 30))) {
            user_log("ar:0x%08lx, ri:0x%08lx, wi:0x%08lx, ret:%ld = %lldms\r\n",
                ar, context->ringbuffer->read_index, context->ringbuffer->write_index,
                ret, (tmp_time - old_time));
            user_log("under run because of no enough data. music end ? wireless network signal instability? or enable wifi/bt but no coex?\r\n");
        } else {
            user_log("ar:0x%08lx, ri:0x%08lx, wi:0x%08lx, ret:%ld\r\n",
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

void audio_poweron(void)
{

    // default close pa
    //codec_gpio_pinmux_config(pa_pin, 0);
    msp_codec_pa_init_pre();

    msp_msleep(10);
}

int auo_init(auo_ch_t *context)
{
    if (NULL == context) {
        user_log("error\r\n");
        return -1;
    }
#if CONFIG_MSP_USE_STATIC_RAM
    static uint8_t g_task_buffer[1024];
    static uint8_t g_task_handle[256];
#endif

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
#if CONFIG_MSP_USE_STATIC_RAM
    msp_task_new_static(&(context->task), AUO_TASK, auo_task_entry, context, 1024, context->task_pri, &g_task_buffer, &g_task_handle, 256);//aos_DEFAULT_APP_PRI - 6);
#else
    msp_task_new_ext(&(context->task), AUO_TASK, auo_task_entry, context, context->stack_size, context->task_pri);//aos_DEFAULT_APP_PRI - 6);
#endif
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
#endif
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
        .src_burst_count = DMA_BURST_INCR4,
        .dst_burst_count = DMA_BURST_INCR4,
        .src_width = DMA_DATA_WIDTH_32BIT,
        .dst_width = DMA_DATA_WIDTH_32BIT,
    }; 
    bflb_dma_channel_init(device_dma, &tx_config);

    if (NULL == context || NULL == context->buffer) {
        user_log("error!\r\n");
        return -1;
    }
    if (context->sound_channel_num == 1) {
        audio_dma_ctrl.TransferSize = node_size / 2;
        audio_dma_ctrl.SWidth = DMA_TRNS_WIDTH_16BITS;
        audio_dma_ctrl.DWidth = DMA_TRNS_WIDTH_16BITS;
    } else if (context->sound_channel_num == 2) {
        audio_dma_ctrl.TransferSize = node_size / 4;
        audio_dma_ctrl.SWidth = DMA_TRNS_WIDTH_32BITS;
        audio_dma_ctrl.DWidth = DMA_TRNS_WIDTH_32BITS;
    } else {
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
        context->dma->node[i].dma_cfg.destDmaAddr = (uint32_t)(0x2000AB00 + 0x88);
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

    bflb_dma_channel_irq_attach(device_dma, _auo_tx_dma_irq_hander, (void *)(context));
    if (dma_id == 0) {
        ms_GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_DMA_0);
    } else if (dma_id == 1) {
        ms_GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_DMA_1);
    } else {
        ms_GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_DMA_2);
    }
    msp_DMA_Enable(dma_id);
    msp_DMA_Channel_Disable(dma_id, dma_ch);

    //msp_dma_irq_callback_set(dma_ch, _auo_tx_dma_irq_hander, (void *)(context));
    msp_DMA_LLI_Init(dma_id, dma_ch, (DMA_LLI_Cfg_Type *)&lli_cfg_dma);
    msp_DMA_LLI_Update(dma_id, dma_ch, (uint32_t)(&(context->dma->node[0].dma_cfg)));
    msp_DMA_IntMask(dma_id, dma_ch, DMA_INT_ALL, MASK);

    // msp_irq_register(MSP_DMA_IRQn, dma_interrupt_cb, (void *)(context));
    // msp_irq_enable(MSP_DMA_IRQn);
    msp_DMA_IntMask(dma_id, dma_ch, DMA_INT_TCOMPLETED, UNMASK);
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
    } else {
        _auo_tx_dma_link_destroy(context, dma);
        user_log("_auo_tx_dma_link_destroy done\r\n");
    }

    msp_mutex_unlock(&(context->mutex));

    return 0;
}

// Return the amount of available data in the CODEC output buffer (users can know how much more can be written based on this value). The data amount is in bytes.
uint32_t auo_buffer_avail(auo_ch_t *context)
{
    uint32_t len;

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    len = mringbuffer_space_len(context->ringbuffer);
    msp_mutex_unlock(&(context->mutex));

    return len;
}

// Return the amount of used data in the CODEC output buffer (available for DMA transfer).
uint32_t auo_buffer_remain(auo_ch_t *context)
{
    uint32_t len;

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    len = mringbuffer_data_len(context->ringbuffer);
    msp_mutex_unlock(&(context->mutex));

    return len;
}

// Pause DMA and clear the data in the buffer.
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
    msp_i2s_tx_enable();
    msp_DMA_Channel_Enable(context->ctrl_id, context->ch_id);
    msp_DMA_Request_Enable(context->ctrl_id, context->ch_id);

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

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);

    if (!msp_task_exist(AUO_TASK)) {
        user_log("auo not init\r\n");
        return -1;
    }

#if CODEC_OUTPUT_DEBUG_TRACE
    context->debug.count_stop++;
#endif
    user_log("context = %p\r\n", context);

    //uint32_t pa_pin = context->pa_pin;
    msp_codec_pa_before_closedac();

    msp_DMA_Channel_Disable(context->ctrl_id, context->ch_id);

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
    msp_DMA_LLI_Update(dma_id, dma_ch, (uint32_t)(&(context->dma->node[0].dma_cfg)));

    msp_DMA_Request_Enable(context->ctrl_id, context->ch_id);

    msp_mutex_unlock(&(context->mutex));
    return 0;
}

#ifdef CONFIG_XCODEC_OUTPUT_CACHE
#define CACHE_SIZE  (320*20)
#endif

uint32_t auo_write(auo_ch_t *context, const void *data, uint32_t size)
{
    uint32_t ret = 0;
    uintptr_t irq_stat;
    user_log("mringbuffer_put_fflush_cache %d\r\n", size);
    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);

#ifdef CONFIG_XCODEC_OUTPUT_CACHE
    static uint32_t cache_total = 0;
    static uint32_t cache_flag = 0;
#endif
    irq_stat = bflb_irq_save();    
    if (context->dma->halt_flag) {
#ifdef CONFIG_XCODEC_OUTPUT_CACHE
        if (!cache_flag)
#endif
        {
            mringbuffer_reset(context->ringbuffer);
        }
    }
    ret = mringbuffer_put_fflush_cache(context->ringbuffer,
        (uint8_t *)data, size,
        msp_cache_flush);
    user_log("write:%d\r\n", context->ringbuffer->write_index); 
    if (context->dma->halt_flag) {
#ifdef CONFIG_XCODEC_OUTPUT_CACHE
        cache_flag = 1;
        cache_total += size;
        if (cache_total >= CACHE_SIZE) {
            printf("cache ok\r\n");
            cache_total = 0;
            cache_flag = 0;
            msp_DMA_LLI_Update(context->ctrl_id, context->ch_id, (uint32_t)(&(context->dma->node[0].dma_cfg)));
            msp_DMA_Request_Enable(context->ctrl_id, context->ch_id);
            context->dma->halt_flag = 0;
        }
#else
        msp_DMA_LLI_Update(context->ctrl_id, context->ch_id, (uint32_t)(&(context->dma->node[0].dma_cfg)));
        msp_DMA_Request_Enable(context->ctrl_id, context->ch_id);
        context->dma->halt_flag = 0;
#endif
    }
    bflb_irq_restore(irq_stat);

    if (0 == context->st) {
#ifdef CONFIG_XCODEC_OUTPUT_CACHE
        cache_total += size;
        if (cache_total >= CACHE_SIZE) {
            cache_total = 0;
            context->st = 1;
            _auo_hw_start(context);
        }
#else
        context->st = 1;
        _auo_hw_start(context);
#endif
        // fixme for nosie at startup
    }

    msp_mutex_unlock(&(context->mutex));

#if CODEC_OUTPUT_DEBUG_TRACE
    context->debug.bytes_write += ret;
#endif

    return ret;
}

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

