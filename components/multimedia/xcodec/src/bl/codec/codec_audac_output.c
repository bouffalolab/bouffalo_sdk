/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <msp_port.h>
#include "codec_output.h"
#include "codec_debug.h"
#include <bflb_audac.h>
#include <bflb_l1c.h>
#include <bflb_clock.h>
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

static struct bflb_device_s *aupwm_dev = NULL;
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
        bflb_l1c_dcache_clean_range((uint32_t *)context->ringbuffer->buffer_ptr, context->ringbuffer->buffer_size);
        bflb_dma_feature_control(context->device_dma, DMA_CMD_SET_SUSPEND, 0);
        context->dma->halt_flag = 1;
        context->callback(context, BL_EVENT_WRITE_BUFFER_EMPTY, context->arg);
    } else {
        if (ri >= wi) {
            if (ar > ri) {
                get_size = ar - ri;
                memset(context->ringbuffer->buffer_ptr + ri, 0, get_size);
                bflb_l1c_dcache_clean_range((uint32_t *)(context->ringbuffer->buffer_ptr + ri), get_size);
            } else if (ar <= wi) {
                get_size = context->ringbuffer->buffer_size - (ri - ar);
                memset(context->ringbuffer->buffer_ptr + ri, 0, context->ringbuffer->buffer_size - ri);
                bflb_l1c_dcache_clean_range((uint32_t *)(context->ringbuffer->buffer_ptr + ri), context->ringbuffer->buffer_size - ri);
                memset(context->ringbuffer->buffer_ptr, 0, ar);
                bflb_l1c_dcache_clean_range((uint32_t *)context->ringbuffer->buffer_ptr, ar);
            } else {
                memset(context->ringbuffer->buffer_ptr, 0, context->ringbuffer->buffer_size);
                bflb_l1c_dcache_clean_range((uint32_t *)context->ringbuffer->buffer_ptr, context->ringbuffer->buffer_size);
                /* dma halt */
                bflb_dma_feature_control(context->device_dma, DMA_CMD_SET_SUSPEND, 0);
                context->dma->halt_flag = 1;
                context->callback(context, BL_EVENT_WRITE_BUFFER_EMPTY, context->arg);
            }
        } else {
            if ((ar > ri) && (ar <= wi)) {
                get_size = ar - ri;
                memset(context->ringbuffer->buffer_ptr + ri, 0, get_size);
                bflb_l1c_dcache_clean_range((uint32_t *)(context->ringbuffer->buffer_ptr + ri), get_size);
            } else {
                memset(context->ringbuffer->buffer_ptr, 0, context->ringbuffer->buffer_size);
                bflb_l1c_dcache_clean_range((uint32_t *)context->ringbuffer->buffer_ptr, context->ringbuffer->buffer_size);
                /* dma halt */
                bflb_dma_feature_control(context->device_dma, DMA_CMD_SET_SUSPEND, 0);
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
            continue;
        }

        if (res == 0) {
            if ((flag & EVENT_AU_STOP) == EVENT_AU_STOP) {
                break;
            } else if ((flag & EVENT_AU_ISR) == EVENT_AU_ISR) {
                msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
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

static void audiopwm_analog_digital_init(uint32_t sample_rate, uint32_t channel_num, uint32_t mode)
{

    struct bflb_audac_volume_config_s aupwm_volume_cfg = {
        .mute_ramp_en = false,
        .mute_up_ramp_rate = AUDAC_RAMP_RATE_FS_2,
        .mute_down_ramp_rate = AUDAC_RAMP_RATE_FS_2,
        .volume_update_mode = AUDAC_VOLUME_UPDATE_MODE_FORCE,
        .volume_ramp_rate = AUDAC_RAMP_RATE_FS_2,
        .volume_zero_cross_timeout = AUDAC_RAMP_RATE_FS_2,
    };

    struct bflb_audac_init_config_s aupwm_cfg = {
        .sampling_rate = AUDAC_SAMPLING_RATE_48K,
        .output_mode = AUDAC_OUTPUT_MODE_PWM,
        .source_channels_num = AUDAC_SOURCE_CHANNEL_SINGLE,
        .mixer_mode = AUDAC_MIXER_MODE_ONLY_L,
        .data_format = AUDAC_DATA_FORMAT_16BIT,
        .fifo_threshold = 7,
    };

    switch (sample_rate) {
        case 48000:
            aupwm_cfg.sampling_rate = AUDAC_SAMPLING_RATE_48K;
            break;
        case 32000:
            aupwm_cfg.sampling_rate = AUDAC_SAMPLING_RATE_32K;
            break;
        case 24000:
            aupwm_cfg.sampling_rate = AUDAC_SAMPLING_RATE_24K;
            break;
        case 16000:
            aupwm_cfg.sampling_rate = AUDAC_SAMPLING_RATE_16K;
            break;
        case 8000:
            aupwm_cfg.sampling_rate = AUDAC_SAMPLING_RATE_8K;
            break;
        case 44100:
            aupwm_cfg.sampling_rate = AUDAC_SAMPLING_RATE_44P1K;
            break;
        case 22050:
            aupwm_cfg.sampling_rate = AUDAC_SAMPLING_RATE_22P05K;
            break;
    }

    bflb_audac_init(aupwm_dev, &aupwm_cfg);

    bflb_audac_volume_init(aupwm_dev, &aupwm_volume_cfg);

    bflb_audac_link_rxdma(aupwm_dev, true);
}

void audio_poweron(void)
{
    msp_codec_pa_init_pre();

    aupwm_dev = bflb_device_get_by_name("audac");

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

    audio_poweron();
    context->task_exit = 0;
    msp_event_new(&(context->event), 0);
#if CONFIG_MSP_USE_STATIC_RAM
    msp_task_new_static(&(context->task), AUO_TASK, auo_task_entry, context, 1024, context->task_pri, &g_task_buffer, &g_task_handle, 256);//aos_DEFAULT_APP_PRI - 6);
#else
    msp_task_new_ext(&(context->task), AUO_TASK, auo_task_entry, context, context->stack_size, context->task_pri);//aos_DEFAULT_APP_PRI - 6);
#endif
    msp_codec_pa_init();

    return 0;
}

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

    audiopwm_analog_digital_init(config->sample_rate, config->sound_channel_num, config->mode);

    context->bit_width = config->bit_width;
    context->buffer = config->buffer;
    context->buffer_size = config->buffer_size;
    context->per_node_size = config->per_node_size;
    context->sound_channel_num = config->sound_channel_num;
    context->st = 0;
    context->pre_indx = 0;

    msp_mutex_unlock(&(context->mutex));

    return 0;
}

static int _auo_tx_dma_link_destroy(auo_ch_t *context, void *dma)
{
    if (NULL == context) {
        user_log("error!\r\n");
        return -1;
    }

    user_log("context = %p\r\n", context);

    msp_free(context->priv);
    bflb_dma_channel_irq_detach(context->device_dma);
    bflb_dma_channel_stop(context->device_dma);

    return 0;
}

static int _auo_tx_dma_link(auo_ch_t *context, void *dma)
{
    int i;
    uint8_t dma_id, dma_ch;
    uint32_t min_buf_size, dma_node_num, node_size;
    uint8_t *pbuf, *node_pbuf, *hw_node_addr, *hw_buff_addr;

    if (NULL == context || NULL == context->buffer) {
        user_log("error!\r\n");
        return -1;
    }

    node_size = context->per_node_size;

    union bflb_dma_lli_control_s audio_dma_ctrl = {0};
    audio_dma_ctrl.bits.TransferSize = node_size / 2;
    audio_dma_ctrl.bits.SBSize       = DMA_BURST_INCR4;
    audio_dma_ctrl.bits.DBSize       = DMA_BURST_INCR4;
    audio_dma_ctrl.bits.SWidth       = DMA_DATA_WIDTH_16BIT;
    audio_dma_ctrl.bits.DWidth       = DMA_DATA_WIDTH_16BIT;
    audio_dma_ctrl.bits.SI           = DMA_ADDR_INCREMENT_ENABLE;
    audio_dma_ctrl.bits.DI           = DMA_ADDR_INCREMENT_DISABLE;
    audio_dma_ctrl.bits.I            = 1;

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

    struct bflb_dma_channel_config_s tx_config = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_AUDAC_TX,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_16BIT,
        .dst_width = DMA_DATA_WIDTH_16BIT,
    };

    bflb_dma_channel_init(device_dma, &tx_config);

    user_log("context = %p\r\n", context);
    memset(context->buffer, 0, context->buffer_size);

    dma_node_num = context->maxcount;

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

    context->dma->node = (auo_segment_t *)hw_node_addr;

    for (i = 0; i < dma_node_num; i++) {
        context->dma->node[i].dma_cfg.src_addr = (uint32_t)hw_buff_addr + i * node_size;
        context->dma->node[i].dma_cfg.dst_addr = (uint32_t)DMA_ADDR_AUDAC_TDR;
        context->dma->node[i].dma_cfg.control = audio_dma_ctrl;

        if (i == dma_node_num - 1) {
            context->dma->node[i].dma_cfg.nextlli = (uint32_t)(&(context->dma->node[0].dma_cfg));
        } else {
            context->dma->node[i].dma_cfg.nextlli = (uint32_t)(&(context->dma->node[i + 1].dma_cfg));
        }
    }
    context->head = context->dma->node[0].dma_cfg.src_addr;

    for (i = 0; i < dma_node_num; i++) {
        user_log("node[%3d] %p --> src:%p dest:%p, next:%p\r\n",
                i,
                (void *)&(context->dma->node[i].dma_cfg),
                (void *)context->dma->node[i].dma_cfg.src_addr,
                (void *)context->dma->node[i].dma_cfg.dst_addr,
                (void *)context->dma->node[i].dma_cfg.nextlli);
    }
        bflb_l1c_dcache_clean_range((uint32_t *)hw_node_addr, (uint32_t)hw_buff_addr + dma_node_num * node_size - 1 - (uint32_t)(hw_node_addr));
    dma_id = context->ctrl_id;
    dma_ch = context->ch_id;

    bflb_dma_channel_irq_attach(device_dma, _auo_tx_dma_irq_hander, (void *)(context));

    bflb_dma_channel_stop(device_dma);

    bflb_dma_feature_control(device_dma, DMA_CMD_SET_LLI_CONFIG, (size_t)(&(context->dma->node[0].dma_cfg)));

    bflb_dma_channel_tcint_mask(device_dma, false);


    bflb_audac_feature_control(aupwm_dev, AUDAC_CMD_PLAY_START, 0);

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
        bflb_audac_feature_control(aupwm_dev, AUDAC_CMD_CLEAR_TX_FIFO, 0);
    } else {
        _auo_tx_dma_link_destroy(context, dma);
        user_log("_auo_tx_dma_link_destroy done\r\n");
    }

    msp_mutex_unlock(&(context->mutex));

    return 0;
}

uint32_t auo_buffer_avail(auo_ch_t *context)
{
    uint32_t len;

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    len = mringbuffer_space_len(context->ringbuffer);
    msp_mutex_unlock(&(context->mutex));

    return len;
}

uint32_t auo_buffer_remain(auo_ch_t *context)
{
    uint32_t len;

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    len = mringbuffer_data_len(context->ringbuffer);
    msp_mutex_unlock(&(context->mutex));

    return len;
}

static int _auo_hw_start(auo_ch_t *context)
{

    msp_msleep(30);
    user_log("context = %p\r\n", context);

    msp_codec_pa_after_opendac();

    bflb_dma_channel_start(context->device_dma);
    bflb_dma_feature_control(context->device_dma, DMA_CMD_SET_RESUME, 1);
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

    msp_codec_pa_before_closedac();

    bflb_audac_feature_control(aupwm_dev, AUDAC_CMD_PLAY_STOP, 0);

    bflb_dma_channel_stop(context->device_dma);

    context->st = 0;
    context->pre_indx = 0;

    msp_mutex_unlock(&(context->mutex));

    if (context->task_exit == 0) {
        msp_event_set(&context->event, EVENT_AU_STOP, 0);
        while (0 == context->task_exit) {
            count++;
            msp_msleep(10);
            if (count > 20000) {
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

    bflb_dma_feature_control(context->device_dma, DMA_CMD_SET_SUSPEND, 0);

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
    uint32_t mv_len;

    fmt_size = mringbuffer_data_len(context->ringbuffer);

    fmt_buf = msp_malloc(fmt_size);
    if (NULL == fmt_buf) {
        MSP_LOGE("mem error\r\n");
        return -1;
    }

    mringbuffer_get(context->ringbuffer, (uint8_t *)fmt_buf, fmt_size);
    mringbuffer_reset(context->ringbuffer);

    mv_len = (fmt_size/node_size)*node_size;

    if (mv_len > 0) {
        mringbuffer_put(context->ringbuffer,
            (uint8_t *)(fmt_buf + fmt_size%node_size),
            mv_len);

        tmp_buf = (uint8_t *)((uint32_t *)context->dma->node[0].dma_cfg.src_addr);
        memset(tmp_buf + mv_len, 0, node_size * dma_node_num - mv_len);

        bflb_l1c_dcache_clean_range((uint32_t *)context->dma->node[0].dma_cfg.src_addr,
            node_size * dma_node_num);
    } else {
        memset((uint32_t *)context->dma->node[0].dma_cfg.src_addr,
            0,
            node_size * dma_node_num);
        bflb_l1c_dcache_clean_range((uint32_t *)context->dma->node[0].dma_cfg.src_addr,
            node_size * dma_node_num);
    }

    msp_free(fmt_buf);

    return 0;
}

int auo_resume(auo_ch_t *context)
{
    uint8_t dma_id = context->ctrl_id;
    uint8_t dma_ch = context->ch_id;

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    user_log("context = %p\r\n", context);

    auo_ringbuff_format(context);
    bflb_dma_feature_control(context->device_dma, DMA_CMD_SET_LLI_CONFIG, (size_t)(&(context->dma->node[0].dma_cfg)));
    bflb_dma_feature_control(context->device_dma, DMA_CMD_SET_RESUME, 1);

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
        bflb_l1c_dcache_clean_range);

    if (context->dma->halt_flag) {
#ifdef CONFIG_XCODEC_OUTPUT_CACHE
        cache_flag = 1;
        cache_total += size;
        if (cache_total >= CACHE_SIZE) {
            cache_total = 0;
            cache_flag = 0;
            bflb_dma_feature_control(context->device_dma, DMA_CMD_SET_LLI_CONFIG, (size_t)(&(context->dma->node[0].dma_cfg)));
            bflb_dma_feature_control(context->device_dma, DMA_CMD_SET_RESUME, 1);
            context->dma->halt_flag = 0;
        }
#else
        bflb_dma_feature_control(context->device_dma, DMA_CMD_SET_LLI_CONFIG, (size_t)(&(context->dma->node[0].dma_cfg)));
        bflb_dma_feature_control(context->device_dma, DMA_CMD_SET_RESUME, 1);
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
