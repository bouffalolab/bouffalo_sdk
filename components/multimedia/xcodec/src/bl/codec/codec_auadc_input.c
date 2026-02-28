/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdio.h>
#include <string.h>
#include <codec_input.h>
#include <bflb_auadc.h>
#include <bflb_l1c.h>
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
static struct bflb_device_s *auadc_dev = NULL;

extern void Audio_Ckg_En(void);
extern void Audio_Analog_Adc_Disable(void);

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
            continue;
        }

        if (res == 0) {
            if ((flag & EVENT_AU_STOP) == EVENT_AU_STOP) {
                break;
            } else if ((flag & EVENT_AU_ISR) == EVENT_AU_ISR) {
                msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
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

static int audioadc_pin_to_channel(uint8_t pin)
{
    int channel = -1;

    switch (pin) {
        case 20:
            channel = 0;
            break;
        case 21:
            channel = 1;
            break;
        case 22:
            channel = 2;
            break;
        case 23:
            channel = 3;
            break;
        case 27:
            channel = 4;
            break;
        case 28:
            channel = 5;
            break;
        case 29:
            channel = 6;
            break;
        case 30:
            channel = 7;
            break;

        default :
            channel = -1;
            break;
    }

    return channel;
}

static void audioadc_analog_digital_init(aui_cfg_t *config)
{
    struct bflb_auadc_init_config_s auadc_cfg = {
        .input_mode = AUADC_INPUT_MODE_ADC,
        .data_format = AUADC_DATA_FORMAT_16BIT,
        .fifo_threshold = 4
    };

    if (config->sample_rate == 8000) {
        auadc_cfg.sampling_rate = AUADC_SAMPLING_RATE_8K;
    } else if (config->sample_rate == 16000) {
        auadc_cfg.sampling_rate = AUADC_SAMPLING_RATE_16K;
    } else if (config->sample_rate == 32000) {
        auadc_cfg.sampling_rate = AUADC_SAMPLING_RATE_32K;
    } else if (config->sample_rate == 48000) {
        auadc_cfg.sampling_rate = AUADC_SAMPLING_RATE_48K;
    } else {
        auadc_cfg.sampling_rate = AUADC_SAMPLING_RATE_16K;
    }

    struct bflb_auadc_adc_init_config_s auadc_adc_cfg = {
        .auadc_analog_en = true,
        .adc_mode = AUADC_ADC_MODE_AUDIO,
        .adc_pga_gain = 27,
    };

    if ((config->positive_pin != 255) && (config->negative_pin != 255)) {
        printf("input mode:diff\r\n");
        auadc_adc_cfg.adc_pga_mode = AUADC_ADC_PGA_MODE_AC_DIFFER;
    } else {
        printf("input mode:single\r\n");
        auadc_adc_cfg.adc_pga_mode = AUADC_ADC_PGA_MODE_AC_SINGLE;
    }

    int channel = -1;

    if (config->positive_pin != 255) {
        channel = audioadc_pin_to_channel(config->positive_pin);
        if (channel != -1) {
            printf("posi:%d\r\n", channel);
            auadc_adc_cfg.adc_pga_posi_ch = channel;
        } else {
            user_log("aui pin is error, pin:%d\r\n", config->positive_pin);
        }
    }

    if (config->negative_pin != 255) {
        channel = audioadc_pin_to_channel(config->negative_pin);
        if (channel != -1) {
            printf("nega:%d\r\n", channel);
            auadc_adc_cfg.adc_pga_nega_ch = channel;
        } else {
            user_log("aui pin is error, pin:%d\r\n", config->negative_pin);
        }
    }

    if (auadc_dev == NULL) {
        auadc_dev = bflb_device_get_by_name("auadc");
        if (auadc_dev == NULL) {
            printf("Failed to get auadc device\r\n");
            return;
        }
    }

    bflb_auadc_init(auadc_dev, &auadc_cfg);

    bflb_auadc_adc_init(auadc_dev, &auadc_adc_cfg);

    bflb_auadc_feature_control(auadc_dev, AUADC_CMD_SET_VOLUME_VAL, (size_t)(0));

    bflb_auadc_link_rxdma(auadc_dev, true);

    printf("AUADC LHAL init complete, rate=%d\r\n", config->sample_rate);
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


    audioadc_analog_digital_init(config);

    context->bit_width         = config->bit_width;
    context->buffer            = config->buffer;
    context->buffer_size       = config->buffer_size;
    context->per_node_size     = config->per_node_size;
    context->sound_channel_num = config->sound_channel_num;
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

    return 0;
}

static int _aui_rx_dma_link(aui_ch_t *context, void *dma)
{
    int i;
    uint8_t dma_id, dma_ch;
    uint32_t min_buf_size, dma_node_num, node_size;
    uint8_t *pbuf, *node_pbuf, *hw_node_addr, *hw_buff_addr;

    node_size = context->per_node_size;

    if (NULL == context || NULL == context->buffer) {
        user_log("error!\r\n");
        return -1;
    }

    union bflb_dma_lli_control_s audio_dma_ctrl = {0};
    audio_dma_ctrl.bits.TransferSize = node_size / 2;
    audio_dma_ctrl.bits.SBSize       = DMA_BURST_INCR4;
    audio_dma_ctrl.bits.DBSize       = DMA_BURST_INCR4;
    audio_dma_ctrl.bits.SWidth       = DMA_DATA_WIDTH_16BIT;
    audio_dma_ctrl.bits.DWidth       = DMA_DATA_WIDTH_16BIT;
    audio_dma_ctrl.bits.SI           = DMA_ADDR_INCREMENT_DISABLE;
    audio_dma_ctrl.bits.DI           = DMA_ADDR_INCREMENT_ENABLE;
    audio_dma_ctrl.bits.I            = 1;

    g_aui_ctx = context;

    user_log("context = %p\r\n", context);

    memset(context->buffer, 0, context->buffer_size);

    dma_node_num = context->maxcount;

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
        context->dma->node[i].dma_cfg.src_addr = (uint32_t)DMA_ADDR_AUADC_RDR;
        context->dma->node[i].dma_cfg.dst_addr = (uint32_t)hw_buff_addr + i * node_size;
        context->dma->node[i].dma_cfg.control = audio_dma_ctrl;

        if (i == dma_node_num - 1) {
            context->dma->node[i].dma_cfg.nextlli = (uint32_t)(&(context->dma->node[0].dma_cfg));
        } else {
            context->dma->node[i].dma_cfg.nextlli = (uint32_t)(&(context->dma->node[i + 1].dma_cfg));
        }
    }
    context->head = context->dma->node[0].dma_cfg.dst_addr;

    for (i = 0; i < dma_node_num; i++) {
        user_log("node[%d] 0x%08lx --> src:0x%08lx dest:0x%08lx, next:0x%08lx\r\n",
                i,
                (uint32_t)(&(context->dma->node[i].dma_cfg)),
                (uint32_t)context->dma->node[i].dma_cfg.src_addr,
                (uint32_t)context->dma->node[i].dma_cfg.dst_addr,
                context->dma->node[i].dma_cfg.nextlli);
    }

    bflb_l1c_dcache_clean_range((uint32_t *)hw_node_addr, (uint32_t)hw_buff_addr + dma_node_num * node_size - 1 - (uint32_t)(hw_node_addr));

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
    config.src_req = DMA_REQUEST_AUADC_RX;
    config.dst_req = DMA_REQUEST_NONE;
    config.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    config.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    config.src_burst_count = DMA_BURST_INCR4;
    config.dst_burst_count = DMA_BURST_INCR4;
    config.src_width = DMA_DATA_WIDTH_16BIT;
    config.dst_width = DMA_DATA_WIDTH_16BIT;
    bflb_dma_channel_init(device_dma, &config);

    bflb_dma_channel_irq_attach(device_dma, _aui_rx_dma_irq_hander, (void *)(context));

    bflb_dma_channel_stop(device_dma);

    bflb_dma_feature_control(device_dma, DMA_CMD_SET_LLI_CONFIG, (size_t)(&(context->dma->node[0].dma_cfg)));
    user_log("init_src:%x\r\n", (*((volatile uint32_t *)(uintptr_t)(context->dma->src_addr))));

    bflb_auadc_feature_control(auadc_dev, AUADC_CMD_RECORD_START, 0);

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

uint32_t aui_buffer_avail(aui_ch_t *context)
{
    uint32_t len;

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    len = mringbuffer_data_len(context->ringbuffer);
    msp_mutex_unlock(&(context->mutex));

    return len;
}

uint32_t aui_buffer_remain(aui_ch_t *context)
{
    uint32_t len;

    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    len = mringbuffer_space_len(context->ringbuffer);
    msp_mutex_unlock(&(context->mutex));

    return len;
}

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

    bflb_dma_channel_stop(context->device_dma);

    bflb_auadc_feature_control(auadc_dev, AUADC_CMD_CLEAR_RX_FIFO, 0);

    for (i = 0; i < context->maxcount; i ++) {
        memset((void *)context->dma->node[i].dma_cfg.dst_addr, 0, node_size);

        bflb_l1c_dcache_clean_range    ((uint32_t *)context->dma->node[i].dma_cfg.dst_addr, node_size);
    }

    context->st = 0;
    context->pre_indx = 0;
    mringbuffer_reset(context->ringbuffer);

    msp_mutex_unlock(&(context->mutex));

    return 0;
}

static int _aui_hw_start(aui_ch_t *context)
{
    user_log("context = %p\r\n", context);

    bflb_auadc_feature_control(auadc_dev, AUADC_CMD_CLEAR_RX_FIFO, 0);

    bflb_dma_channel_start(context->device_dma);

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

    bflb_dma_channel_stop(context->device_dma);

    if (context->task_exit == 0) {
        msp_event_set(&context->event, EVENT_AU_STOP, 0);

        while (0 == context->task_exit) {
            count++;
            msp_msleep(1);
            if (count > 20000) {
                user_log("context->task_exit timeout !!!!!!!!!!!!!!!!!!!!\r\n");
            }
        }

        msp_event_free(&context->event);
    }

    bflb_auadc_feature_control(auadc_dev, AUADC_CMD_RECORD_STOP, 0);

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
    bflb_dma_channel_stop(context->device_dma);
    msp_mutex_unlock(&(context->mutex));
    return 0;
}

int aui_resume(aui_ch_t *context)
{
#if CODEC_INPUT_DEBUG_TRACE
    context->debug.count_resume++;
#endif
    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);
    bflb_dma_channel_start(context->device_dma);
    msp_mutex_unlock(&(context->mutex));
    return 0;
}

uint32_t aui_read(aui_ch_t *context, const void *data, uint32_t size)
{
    uint32_t ret = 0, rel_size = 0;

#if CODEC_INPUT_DEBUG_TRACE
    context->debug.count_read++;
#endif
    msp_mutex_lock(&(context->mutex), MSP_WAIT_FOREVER);

    ret = mringbuffer_data_len(context->ringbuffer);

    if (ret >= size) {
        rel_size = size;
    } else {
        rel_size = ret - ret % 6;
    }
    if (rel_size == 0) {
        msp_mutex_unlock(&(context->mutex));
        return 0;
    }

    MSP_LOGD("mringbuffer_get_invalid_cache:%d\r\n", rel_size);
    ret = mringbuffer_get_invalid_cache(
        context->ringbuffer,
        (uint8_t *)data, rel_size,
        NULL);
    if (ret != rel_size) {
        MSP_LOGE("never run here!\r\n");
    }

#if RAMDUMP_CONFIG_DEBUG
    if (rel_size + g_rawaui_data_len > RAW_AUO_DUMP_DATA_LEN) {
    } else {
        memcpy(g_rawaui_data + g_rawaui_data_len, (uint8_t*)data, rel_size);
        g_rawaui_data_len += rel_size;
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


