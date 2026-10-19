#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_i2s.h"
#include "bflb_dma.h"
#include "bflb_l1c.h"

#include "bl616_glb.h"

#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <queue.h>
#include <portmacro.h>

#include "soln_fbq.h"

#include "board.h"

#include "soln_aud_external_codec_i2s.h"

#include "soln_aud_codec_es8388_cfg.h"

#ifdef CONFIG_SOLN_AUD_I2S_LOG_LEVEL
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOLN_AUD_I2S_LOG_LEVEL
#endif
#define DBG_TAG "SOLN_I2S"
#include "log.h"

/* device hd */
static struct bflb_device_s *audio_i2s;

#if IS_ENABLED(CONFIG_SOLN_AUD_I2S_IN_EN)

/***************** i2s input: *****************/
/* enable/disable record */
#define I2S_INPUT_EVENT_ENABLE (0x01 << 0)
/* pause record */
#define I2S_INPUT_EVENT_PAUSE  (0x01 << 1)

static TaskHandle_t i2s_input_resume_task_hd;
static EventGroupHandle_t i2s_input_event_group;

/* i2s input dma device hd */
static struct bflb_device_s *i2s_dma_ch_input;
/* dma lli pool */
static ATTR_NOCACHE_RAM_SECTION __ALIGNED(32) struct bflb_dma_channel_lli_pool_s input_dma_lli_pool[CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM];
/* multiple frame */
static volatile uint16_t input_using_num;
static fbq_elem_t *input_using_frame[CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM];

/* i2s input dma isr */
ATTR_TCM_SECTION static void i2s_input_dma_isr(void *arg)
{
    int ret;
    uint16_t frame_index, lli_index;
    EventBits_t record_event_bits;

    LOG_D("i2s in isr\r\n");

    record_event_bits = xEventGroupGetBitsFromISR(i2s_input_event_group);

    /* push frame data */
    for (frame_index = 0; frame_index < input_using_num; frame_index++) {
        /* set size */
        fbq_elem_t *elem = input_using_frame[frame_index];
        elem->size = I2S_INPUT_FRAME_SIZE;
        elem->type_mask = SOLUTION_FBQ_TYPE_MASK_AUDIO_PCM_I2S_IN;

        bflb_l1c_dcache_invalidate_range(elem->data, elem->size + 31U);

        (void)fbq_push_mask(soln_fbq_aud_pcm_local(), elem, FBQ_OUTPUT_ALL, 0);
        fbq_free(elem);
        input_using_frame[frame_index] = NULL;
    }

    if ((record_event_bits & I2S_INPUT_EVENT_ENABLE) == 0) {
        /* disable record */
        xEventGroupSetBitsFromISR(i2s_input_event_group, I2S_INPUT_EVENT_PAUSE, NULL);
        return;
    }

    /* get new buffer */
    for (frame_index = 0; frame_index < (CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM / 3); frame_index++) {
        ret = fbq_alloc(soln_fbq_aud_pcm_local(), &input_using_frame[frame_index], 0);
        if (ret != FBQ_OK) {
            break;
        }
    }

    /* No buffs are available, pause record task */
    if (frame_index == 0) {
        xEventGroupSetBitsFromISR(i2s_input_event_group, I2S_INPUT_EVENT_PAUSE, NULL);
        return;
    }

    /* update num */
    input_using_num = frame_index;
#if (I2S_INPUT_DMA_AUTO_DELE_EN)
    int queue_waiting_num = fbq_free_count(soln_fbq_aud_pcm_local());
#endif

    /* dma lli cfg */
    for (lli_index = 0; lli_index < frame_index; lli_index++) {
        input_dma_lli_pool[lli_index].src_addr = DMA_ADDR_I2S_RDR;
        input_dma_lli_pool[lli_index].dst_addr = (uint32_t)(input_using_frame[lli_index]->data);

#if (I2S_INPUT_DMA_AUTO_DELE_EN)
        if (queue_waiting_num < CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM / 4) {
            input_dma_lli_pool[lli_index].control.bits.TransferSize = I2S_INPUT_FRAME_SIZE / 2 + (CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM / 4 - queue_waiting_num);
        } else {
            input_dma_lli_pool[lli_index].control.bits.TransferSize = I2S_INPUT_FRAME_SIZE / 2;
        }
#else
        input_dma_lli_pool[lli_index].control.bits.TransferSize = I2S_INPUT_FRAME_SIZE / 2;

#endif

        input_dma_lli_pool[lli_index].nextlli = (uint32_t)(&input_dma_lli_pool[lli_index + 1]);
        input_dma_lli_pool[lli_index].control.bits.I = 0;
    }

    if (lli_index > 0) {
        input_dma_lli_pool[lli_index - 1].nextlli = 0;
        input_dma_lli_pool[lli_index - 1].control.bits.I = 1;
    }

    /* start dma */
    if (lli_index > 0) {
        bflb_dma_feature_control(i2s_dma_ch_input, DMA_CMD_SET_LLI_CONFIG, (uint32_t)&input_dma_lli_pool[0]);
        bflb_dma_channel_start(i2s_dma_ch_input);
    }
}

/* i2s_input_resume_task */
static void i2s_input_resume_task(void *pvParameters)
{
    int ret;
    uint16_t frame_index, lli_index;

    vTaskDelay(10);

    while (1) {
        /* enable and pause, resume to record */
        xEventGroupWaitBits(i2s_input_event_group, (I2S_INPUT_EVENT_ENABLE | I2S_INPUT_EVENT_PAUSE), pdFALSE, pdTRUE, portMAX_DELAY);

        /* allco frame buff */
        ret = fbq_alloc(soln_fbq_aud_pcm_local(), &input_using_frame[0], 1000);
        if (ret != FBQ_OK) {
            LOG_W("i2s input alloc timeout %d, continue wait... \r\n", ret);
            continue;
        }

        LOG_D("i2s input resume\r\n");

        /* get more buffs */
        for (frame_index = 1; frame_index < (CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM / 4); frame_index++) {
            ret = fbq_alloc(soln_fbq_aud_pcm_local(), &input_using_frame[frame_index], 0);
            if (ret != FBQ_OK) {
                break;
            }
        }

        input_using_num = frame_index;
#if (I2S_INPUT_DMA_AUTO_DELE_EN)
        int queue_waiting_num = fbq_free_count(soln_fbq_aud_pcm_local());
#endif

        /* dma lli cfg */
        for (lli_index = 0; lli_index < frame_index; lli_index++) {
            input_dma_lli_pool[lli_index].src_addr = DMA_ADDR_I2S_RDR;
            input_dma_lli_pool[lli_index].dst_addr = (uint32_t)(input_using_frame[lli_index]->data);

#if (I2S_INPUT_DMA_AUTO_DELE_EN)
            if (queue_waiting_num < CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM / 4) {
                input_dma_lli_pool[lli_index].control.bits.TransferSize = I2S_INPUT_FRAME_SIZE / 2 + (CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM / 4 - queue_waiting_num);
            } else {
                input_dma_lli_pool[lli_index].control.bits.TransferSize = I2S_INPUT_FRAME_SIZE / 2;
            }
#else
            input_dma_lli_pool[lli_index].control.bits.TransferSize = I2S_INPUT_FRAME_SIZE / 2;

#endif

            input_dma_lli_pool[lli_index].nextlli = (uint32_t)(&input_dma_lli_pool[lli_index + 1]);
            input_dma_lli_pool[lli_index].control.bits.I = 0;
        }

        if (lli_index > 0) {
            input_dma_lli_pool[lli_index - 1].nextlli = 0;
            input_dma_lli_pool[lli_index - 1].control.bits.I = 1;
        }

        /* clear pause event bit */
        xEventGroupClearBits(i2s_input_event_group, I2S_INPUT_EVENT_PAUSE);

        /* start dma */
        bflb_dma_feature_control(i2s_dma_ch_input, DMA_CMD_SET_LLI_CONFIG, (uint32_t)&input_dma_lli_pool[0]);
        bflb_dma_channel_start(i2s_dma_ch_input);
    }
}

static void i2s_input_dma_init(void)
{
    struct bflb_dma_channel_config_s i2s_input_dma_cfg;

    i2s_input_dma_cfg.direction = DMA_PERIPH_TO_MEMORY;
    i2s_input_dma_cfg.src_req = DMA_REQUEST_I2S_RX;
    i2s_input_dma_cfg.dst_req = DMA_REQUEST_NONE;
    i2s_input_dma_cfg.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    i2s_input_dma_cfg.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    i2s_input_dma_cfg.src_burst_count = DMA_BURST_INCR8;
    i2s_input_dma_cfg.dst_burst_count = DMA_BURST_INCR8;
    i2s_input_dma_cfg.src_width = DMA_DATA_WIDTH_16BIT;
    i2s_input_dma_cfg.dst_width = DMA_DATA_WIDTH_16BIT;

    i2s_dma_ch_input = bflb_device_get_by_name(I2S_INPUT_DMA_NUME);
    bflb_dma_channel_init(i2s_dma_ch_input, &i2s_input_dma_cfg);
    bflb_dma_channel_irq_attach(i2s_dma_ch_input, i2s_input_dma_isr, NULL);

    for (uint8_t i = 0; i < CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM; i++) {
        input_dma_lli_pool[i].control.WORD = bflb_dma_feature_control(i2s_dma_ch_input, DMA_CMD_GET_LLI_CONTROL, 0);
    }
}

#endif

#if IS_ENABLED(CONFIG_SOLN_AUD_I2S_OUT_EN)

/***************** i2s output: *****************/
/* enable/disable play */
#define I2S_OUTPUT_EVENT_ENABLE (0x01 << 0)
/* pause play */
#define I2S_OUTPUT_EVENT_PAUSE  (0x01 << 1)

/* Remote PCM queue output ID reserved for I2S playback. */
static uint16_t remote_pcm_output_i2s_id;

static TaskHandle_t i2s_output_resume_task_hd;
static EventGroupHandle_t i2s_output_event_group;

/* i2s output dma device hd */
static struct bflb_device_s *i2s_dma_ch_output;
/* dma lli pool */
static ATTR_NOCACHE_RAM_SECTION __ALIGNED(32) struct bflb_dma_channel_lli_pool_s output_dma_lli_pool[CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM];
/* multiple frame */
static volatile uint16_t output_using_num;
static fbq_elem_t *output_using_frame[CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM];

/* i2s output dma isr */
ATTR_TCM_SECTION static void i2s_output_dma_isr(void *arg)
{
    int ret;
    uint16_t frame_index, lli_index;
    EventBits_t play_event_bits;

    LOG_D("i2s out isr\r\n");

    play_event_bits = xEventGroupGetBitsFromISR(i2s_output_event_group);

    /* free frame data */
    for (frame_index = 0; frame_index < output_using_num; frame_index++) {
        fbq_free(output_using_frame[frame_index]);
        output_using_frame[frame_index] = NULL;
    }

    if ((play_event_bits & I2S_OUTPUT_EVENT_ENABLE) == 0) {
        /* disable play */
        xEventGroupSetBitsFromISR(i2s_output_event_group, I2S_OUTPUT_EVENT_PAUSE, NULL);
        return;
    }

    /* get new buffer frame */
    for (frame_index = 0; frame_index < (CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM / 4); frame_index++) {
        ret = fbq_pop(soln_fbq_aud_pcm_remote(), &output_using_frame[frame_index], remote_pcm_output_i2s_id, 0);
        if (ret != FBQ_OK) {
            break;
        }
    }

    /* update num */
    output_using_num = frame_index;

    /* No buffers are available, pause play task */
    if (frame_index == 0) {
        xEventGroupSetBitsFromISR(i2s_output_event_group, I2S_OUTPUT_EVENT_PAUSE, NULL);
        return;
    }

#if (I2S_OUTPUT_DMA_AUTO_DELE_EN)
    int queue_waiting_num = fbq_output_count(soln_fbq_aud_pcm_remote(), remote_pcm_output_i2s_id);
#endif

    /* dma lli cfg */
    for (lli_index = 0; lli_index < frame_index; lli_index++) {
        output_dma_lli_pool[lli_index].src_addr = (uint32_t)(output_using_frame[lli_index]->data);
        output_dma_lli_pool[lli_index].dst_addr = DMA_ADDR_I2S_TDR;
#if (I2S_OUTPUT_DMA_AUTO_DELE_EN)
        if (queue_waiting_num > CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM / 4) {
            output_dma_lli_pool[lli_index].control.bits.TransferSize = output_using_frame[lli_index]->size / 2 - (queue_waiting_num - CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM / 4);
        } else {
            output_dma_lli_pool[lli_index].control.bits.TransferSize = output_using_frame[lli_index]->size / 2;
        }
#else
        output_dma_lli_pool[lli_index].control.bits.TransferSize = output_using_frame[lli_index]->size / 2;

#endif

        output_dma_lli_pool[lli_index].nextlli = (uint32_t)(&output_dma_lli_pool[lli_index + 1]);
        output_dma_lli_pool[lli_index].control.bits.I = 0;
    }

    if (lli_index > 0) {
        output_dma_lli_pool[lli_index - 1].nextlli = 0;
        output_dma_lli_pool[lli_index - 1].control.bits.I = 1;
    }

    /* start dma */
    if (lli_index > 0) {
        bflb_dma_feature_control(i2s_dma_ch_output, DMA_CMD_SET_LLI_CONFIG, (uint32_t)&output_dma_lli_pool[0]);
        bflb_dma_channel_start(i2s_dma_ch_output);
    }
}

static void i2s_output_resume_task(void *pvParameters)
{
    int ret;
    uint16_t frame_index, lli_index;

    vTaskDelay(50);

    while (1) {
        /* enable and pause, resume to play */
        xEventGroupWaitBits(i2s_output_event_group, (I2S_OUTPUT_EVENT_ENABLE | I2S_OUTPUT_EVENT_PAUSE), pdFALSE, pdTRUE, portMAX_DELAY);

        /* at least one buffer is required */
        ret = fbq_pop(soln_fbq_aud_pcm_remote(), &output_using_frame[0], remote_pcm_output_i2s_id, 1000);
        if (ret != FBQ_OK) {
            /* pool empty */
            LOG_W("remote PCM pop timeout %d, continue wait... \r\n", ret);
            continue;
        }

        LOG_D("audio play resume\r\n");

        /* get more buffers */
        for (frame_index = 1; frame_index < (CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM / 4); frame_index++) {
            ret = fbq_pop(soln_fbq_aud_pcm_remote(), &output_using_frame[frame_index], remote_pcm_output_i2s_id, 0);
            if (ret != FBQ_OK) {
                break;
            }
        }

        output_using_num = frame_index;
#if (I2S_OUTPUT_DMA_AUTO_DELE_EN)
        int queue_waiting_num = fbq_output_count(soln_fbq_aud_pcm_remote(), remote_pcm_output_i2s_id);
#endif

        /* dma lli cfg */
        for (lli_index = 0; lli_index < frame_index; lli_index++) {
            output_dma_lli_pool[lli_index].src_addr = (uint32_t)(output_using_frame[lli_index]->data);
            output_dma_lli_pool[lli_index].dst_addr = DMA_ADDR_I2S_TDR;
#if (I2S_OUTPUT_DMA_AUTO_DELE_EN)
            if (queue_waiting_num > CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM / 4) {
                output_dma_lli_pool[lli_index].control.bits.TransferSize = output_using_frame[lli_index]->size / 2 - (queue_waiting_num - CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM / 4);
            } else {
                output_dma_lli_pool[lli_index].control.bits.TransferSize = output_using_frame[lli_index]->size / 2;
            }
#else
            output_dma_lli_pool[lli_index].control.bits.TransferSize = output_using_frame[lli_index]->size / 2;

#endif
            output_dma_lli_pool[lli_index].nextlli = (uint32_t)(&output_dma_lli_pool[lli_index + 1]);
            output_dma_lli_pool[lli_index].control.bits.I = 0;
        }

        if (lli_index > 0) {
            output_dma_lli_pool[lli_index - 1].nextlli = 0;
            output_dma_lli_pool[lli_index - 1].control.bits.I = 1;
        }

        /* clear pause event bit */
        xEventGroupClearBits(i2s_output_event_group, I2S_OUTPUT_EVENT_PAUSE);

        /* start dma */
        bflb_dma_feature_control(i2s_dma_ch_output, DMA_CMD_SET_LLI_CONFIG, (uint32_t)&output_dma_lli_pool[0]);
        bflb_dma_channel_start(i2s_dma_ch_output);
    }
}

static void i2s_output_dma_init(void)
{
    struct bflb_dma_channel_config_s i2s_output_dma_cfg;

    i2s_output_dma_cfg.direction = DMA_MEMORY_TO_PERIPH;
    i2s_output_dma_cfg.src_req = DMA_REQUEST_NONE;
    i2s_output_dma_cfg.dst_req = DMA_REQUEST_I2S_TX;
    i2s_output_dma_cfg.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    i2s_output_dma_cfg.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    i2s_output_dma_cfg.src_burst_count = DMA_BURST_INCR8;
    i2s_output_dma_cfg.dst_burst_count = DMA_BURST_INCR8;
    i2s_output_dma_cfg.src_width = DMA_DATA_WIDTH_16BIT;
    i2s_output_dma_cfg.dst_width = DMA_DATA_WIDTH_16BIT;

    i2s_dma_ch_output = bflb_device_get_by_name(I2S_OUTPUT_DMA_NUME);
    bflb_dma_channel_init(i2s_dma_ch_output, &i2s_output_dma_cfg);
    bflb_dma_channel_irq_attach(i2s_dma_ch_output, i2s_output_dma_isr, NULL);

    for (uint8_t i = 0; i < CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM; i++) {
        output_dma_lli_pool[i].control.WORD = bflb_dma_feature_control(i2s_dma_ch_output, DMA_CMD_GET_LLI_CONTROL, 0);
    }
}
#endif

/* i2s init */
static void i2s_init()
{
    struct bflb_i2s_config_s i2s_cfg = {
        .bclk_freq_hz = 16000 * 16 * 2, /* bclk = Sampling_rate * frame_width * frame_solt_num */
        .role = I2S_ROLE_MASTER,
        .format_mode = I2S_MODE_LEFT_JUSTIFIED,
#if (I2S_CH_NUM_CFG == 1)
        .channel_mode = I2S_CHANNEL_MODE_NUM_1,
#elif (I2S_CH_NUM_CFG == 2)
        .channel_mode = I2S_CHANNEL_MODE_NUM_2,
#endif
        .frame_width = I2S_SLOT_WIDTH_16,
        .data_width = I2S_SLOT_WIDTH_16,
        .fs_offset_cycle = 0,

        .tx_fifo_threshold = 7,
        .rx_fifo_threshold = 7,
    };

    /* i2s gpio init */
    board_i2s_gpio_init();
    /* mclk freq and gpio init */
    board_mclk_out_init();

    audio_i2s = bflb_device_get_by_name("i2s0");
    /* i2s init */
    bflb_i2s_init(audio_i2s, &i2s_cfg);

    /* Mono channel select, false: L-channel, true: R-channel */
    bflb_i2s_feature_control(audio_i2s, I2S_CMD_MONO_CHANEL_SEL, true);

    /* enable dma */
    bflb_i2s_link_txdma(audio_i2s, true);
    bflb_i2s_link_rxdma(audio_i2s, true);

    size_t i2s_enable_arg = I2S_ARG_CMD_MASTER;
#if IS_ENABLED(CONFIG_SOLN_AUD_I2S_OUT_EN)
    i2s_enable_arg |= I2S_ARG_CMD_TX;
#endif
#if IS_ENABLED(CONFIG_SOLN_AUD_I2S_IN_EN)
    i2s_enable_arg |= I2S_ARG_CMD_RX;
#endif
    bflb_i2s_feature_control(audio_i2s, I2S_CMD_ENABLE_CONTROL, i2s_enable_arg);
}

/* audio task init */
int soln_aud_external_codec_i2s_task_init(void)
{
    LOG_I("soln_aud_external_codec_i2s_task_init\r\n");

    /* i2s init */
    i2s_init();

#if IS_ENABLED(CONFIG_SOLN_AUD_I2S_IN_EN)
    /* i2s input dma init */
    i2s_input_dma_init();

    /* audio_record event */
    i2s_input_event_group = xEventGroupCreate();
    xEventGroupSetBits(i2s_input_event_group, (I2S_INPUT_EVENT_ENABLE | I2S_INPUT_EVENT_PAUSE));

    /* creat record task  */
    xTaskCreate(i2s_input_resume_task, (char *)"i2s_input_task", 512, NULL, I2S_INPUT_TASK_PRIORITY, &i2s_input_resume_task_hd);
#endif

#if IS_ENABLED(CONFIG_SOLN_AUD_I2S_OUT_EN)
    /* i2s output dma init */
    i2s_output_dma_init();

    /* audio_play event */
    i2s_output_event_group = xEventGroupCreate();
    xEventGroupSetBits(i2s_output_event_group, (I2S_OUTPUT_EVENT_ENABLE | I2S_OUTPUT_EVENT_PAUSE));

    /* Create the I2S output subscription on the remote PCM queue. */
    remote_pcm_output_i2s_id = CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_I2S_ID;
    if (fbq_output_open(soln_fbq_aud_pcm_remote(), &remote_pcm_output_i2s_id,
                        CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_I2S_DEPTH,
                        SOLUTION_FBQ_TYPE_MASK_AUDIO_PCM_LOOPBACK) != FBQ_OK) {
        LOG_E("remote PCM I2S output subscription create failed\r\n");
        return -1;
    } else {
        LOG_I("remote PCM I2S output ID: %d\r\n", remote_pcm_output_i2s_id);
    }

    /* creat play task */
    xTaskCreate(i2s_output_resume_task, (char *)"i2s_output_task", 512, NULL, I2S_OUTPUT_TASK_PRIORITY, &i2s_output_resume_task_hd);
#endif

    return 0;
}
