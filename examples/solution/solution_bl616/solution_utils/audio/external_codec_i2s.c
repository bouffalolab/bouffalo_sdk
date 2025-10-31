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

#include "frame_queue_ctrl.h"
#include "frame_queue.h"

#include "board.h"

#include "external_codec_i2s.h"

#include "codec_es8388_cfg.h"

#if (0)
#define I2S_DBG(a, ...) printf("[I2S dbg]:" a, ##__VA_ARGS__)
#else
#define I2S_DBG(a, ...)
#endif

#if (1)
#define I2S_INFO(a, ...) printf("[I2S info]:" a, ##__VA_ARGS__)
#else
#define I2S_INFO(a, ...)
#endif

#define I2S_ERR(a, ...) printf("[I2S err]:" a, ##__VA_ARGS__)

/* device hd */
static struct bflb_device_s *audio_i2s;

#if (CONFIG_SOLUTION_FUNC_AUDIO_I2S_IN)

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
static ATTR_NOCACHE_RAM_SECTION __ALIGNED(32) struct bflb_dma_channel_lli_pool_s input_dma_lli_pool[AUDIO_IN_FRAME_NUM];
/* multiple frame */
static volatile uint16_t input_using_num;
static audio_in_frame_t input_using_frame[AUDIO_IN_FRAME_NUM];

/* i2s input dma isr */
ATTR_TCM_SECTION static void i2s_input_dma_isr(void *arg)
{
    int ret;
    uint16_t frame_index, lli_index;
    EventBits_t record_event_bits;

    I2S_DBG("i2s in isr\r\n");

    record_event_bits = xEventGroupGetBitsFromISR(i2s_input_event_group);

    /* push frame data */
    for (frame_index = 0; frame_index < input_using_num; frame_index++) {
        /* set size */
        input_using_frame[frame_index].data_size = I2S_INPUT_FRAME_SIZE;

        bflb_l1c_dcache_invalidate_range(input_using_frame[frame_index].elem_base.frame_addr, input_using_frame[frame_index].data_size + 31);

        /* push to frame queue */
        for (uint8_t i = 0; i < OUTPUT_QUEUE_NUM_MAX; i++) {
            frame_queue_push(g_audio_in_frame_ctrl, (frame_elem_t *)&input_using_frame[frame_index], i, 0);
        }

        /* unlock elem */
        frame_queue_free_unlock(g_audio_in_frame_ctrl, (frame_elem_t *)&input_using_frame[frame_index]);
    }

    if ((record_event_bits & I2S_INPUT_EVENT_ENABLE) == 0) {
        /* disable record */
        xEventGroupSetBitsFromISR(i2s_input_event_group, I2S_INPUT_EVENT_PAUSE, NULL);
        return;
    }

    /* get new buffer */
    for (frame_index = 0; frame_index < (AUDIO_IN_FRAME_NUM / 3); frame_index++) {
        ret = frame_queue_alloc_lock(g_audio_in_frame_ctrl, (frame_elem_t *)&input_using_frame[frame_index], 0);
        if (ret < 0) {
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
    int queue_waiting_num = frame_queue_pool_get_available(g_audio_in_frame_ctrl);
#endif

    /* dma lli cfg */
    for (lli_index = 0; lli_index < frame_index; lli_index++) {
        input_dma_lli_pool[lli_index].src_addr = DMA_ADDR_I2S_RDR;
        input_dma_lli_pool[lli_index].dst_addr = (uint32_t)(input_using_frame[lli_index].elem_base.frame_addr);

#if (I2S_INPUT_DMA_AUTO_DELE_EN)
        if (queue_waiting_num < AUDIO_IN_FRAME_NUM / 4) {
            input_dma_lli_pool[lli_index].control.bits.TransferSize = I2S_INPUT_FRAME_SIZE / 2 + (AUDIO_IN_FRAME_NUM / 4 - queue_waiting_num);
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
        ret = frame_queue_alloc_lock(g_audio_in_frame_ctrl, (frame_elem_t *)&input_using_frame[0], 1000);
        if (ret < 0) {
            I2S_INFO("i2s input alloc timeout %d, continue wait... \r\n", ret);
            continue;
        }

        I2S_DBG("i2s input resume\r\n");

        /* get more buffs */
        for (frame_index = 1; frame_index < (AUDIO_IN_FRAME_NUM / 4); frame_index++) {
            ret = frame_queue_alloc_lock(g_audio_in_frame_ctrl, (frame_elem_t *)&input_using_frame[frame_index], 0);
            if (ret < 0) {
                break;
            }
        }

        input_using_num = frame_index;
#if (I2S_INPUT_DMA_AUTO_DELE_EN)
        int queue_waiting_num = frame_queue_pool_get_available(g_audio_in_frame_ctrl);
#endif

        /* dma lli cfg */
        for (lli_index = 0; lli_index < frame_index; lli_index++) {
            input_dma_lli_pool[lli_index].src_addr = DMA_ADDR_I2S_RDR;
            input_dma_lli_pool[lli_index].dst_addr = (uint32_t)(input_using_frame[lli_index].elem_base.frame_addr);

#if (I2S_INPUT_DMA_AUTO_DELE_EN)
            if (queue_waiting_num < AUDIO_IN_FRAME_NUM / 4) {
                input_dma_lli_pool[lli_index].control.bits.TransferSize = I2S_INPUT_FRAME_SIZE / 2 + (AUDIO_IN_FRAME_NUM / 4 - queue_waiting_num);
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

    for (uint8_t i = 0; i < AUDIO_IN_FRAME_NUM; i++) {
        input_dma_lli_pool[i].control.WORD = bflb_dma_feature_control(i2s_dma_ch_input, DMA_CMD_GET_LLI_CONTROL, 0);
    }
}

#endif

#if (CONFIG_SOLUTION_FUNC_AUDIO_I2S_OUT)

/***************** i2s output: *****************/
/* enable/disable play */
#define I2S_OUTPUT_EVENT_ENABLE (0x01 << 0)
/* pause play */
#define I2S_OUTPUT_EVENT_PAUSE  (0x01 << 1)

/* audio queue output id */
static uint16_t audio_out_out_queue_i2s_id;

static TaskHandle_t i2s_output_resume_task_hd;
static EventGroupHandle_t i2s_output_event_group;

/* i2s output dma device hd */
static struct bflb_device_s *i2s_dma_ch_output;
/* dma lli pool */
static ATTR_NOCACHE_RAM_SECTION __ALIGNED(32) struct bflb_dma_channel_lli_pool_s output_dma_lli_pool[AUDIO_OUT_FRAME_NUM];
/* multiple frame */
static volatile uint16_t output_using_num;
static audio_in_frame_t output_using_frame[AUDIO_IN_FRAME_NUM];

/* i2s output dma isr */
ATTR_TCM_SECTION static void i2s_output_dma_isr(void *arg)
{
    int ret;
    uint16_t frame_index, lli_index;
    EventBits_t play_event_bits;

    I2S_DBG("i2s out isr\r\n");

    play_event_bits = xEventGroupGetBitsFromISR(i2s_output_event_group);

    /* free frame data */
    for (frame_index = 0; frame_index < output_using_num; frame_index++) {
        /* free frame queue */
        frame_queue_output_free(g_audio_out_frame_ctrl, (frame_elem_t *)&output_using_frame[frame_index]);
    }

    if ((play_event_bits & I2S_OUTPUT_EVENT_ENABLE) == 0) {
        /* disable play */
        xEventGroupSetBitsFromISR(i2s_output_event_group, I2S_OUTPUT_EVENT_PAUSE, NULL);
        return;
    }

    /* get new buffer frame */
    for (frame_index = 0; frame_index < (AUDIO_OUT_FRAME_NUM / 4); frame_index++) {
        ret = frame_queue_output_pop(g_audio_out_frame_ctrl, (frame_elem_t *)&output_using_frame[frame_index], audio_out_out_queue_i2s_id, 0);
        if (ret < 0) {
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
    int queue_waiting_num = frame_queue_output_get_waiting(g_audio_out_frame_ctrl, audio_out_out_queue_i2s_id);
#endif

    /* dma lli cfg */
    for (lli_index = 0; lli_index < frame_index; lli_index++) {
        output_dma_lli_pool[lli_index].src_addr = (uint32_t)(output_using_frame[lli_index].elem_base.frame_addr);
        output_dma_lli_pool[lli_index].dst_addr = DMA_ADDR_I2S_TDR;
#if (I2S_OUTPUT_DMA_AUTO_DELE_EN)
        if (queue_waiting_num > AUDIO_OUT_FRAME_NUM / 4) {
            output_dma_lli_pool[lli_index].control.bits.TransferSize = output_using_frame[lli_index].data_size / 2 - (queue_waiting_num - AUDIO_OUT_FRAME_NUM / 4);
        } else {
            output_dma_lli_pool[lli_index].control.bits.TransferSize = output_using_frame[lli_index].data_size / 2;
        }
#else
        output_dma_lli_pool[lli_index].control.bits.TransferSize = output_using_frame[lli_index].data_size / 2;

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
        ret = frame_queue_output_pop(g_audio_out_frame_ctrl, (frame_elem_t *)&output_using_frame[0], audio_out_out_queue_i2s_id, 1000);
        if (ret < 0) {
            /* pool empty */
            I2S_INFO(" audio_out pop timeout %d, continue wait... \r\n", ret);
            continue;
        }

        I2S_DBG("audio play resume\r\n");

        /* get more buffers */
        for (frame_index = 1; frame_index < (AUDIO_OUT_FRAME_NUM / 4); frame_index++) {
            ret = frame_queue_output_pop(g_audio_out_frame_ctrl, (frame_elem_t *)&output_using_frame[frame_index], audio_out_out_queue_i2s_id, 0);
            if (ret < 0) {
                break;
            }
        }

        output_using_num = frame_index;
#if (I2S_OUTPUT_DMA_AUTO_DELE_EN)
        int queue_waiting_num = frame_queue_output_get_waiting(g_audio_out_frame_ctrl, audio_out_out_queue_i2s_id);
#endif

        /* dma lli cfg */
        for (lli_index = 0; lli_index < frame_index; lli_index++) {
            output_dma_lli_pool[lli_index].src_addr = (uint32_t)(output_using_frame[lli_index].elem_base.frame_addr);
            output_dma_lli_pool[lli_index].dst_addr = DMA_ADDR_I2S_TDR;
#if (I2S_OUTPUT_DMA_AUTO_DELE_EN)
            if (queue_waiting_num > AUDIO_OUT_FRAME_NUM / 4) {
                output_dma_lli_pool[lli_index].control.bits.TransferSize = output_using_frame[lli_index].data_size / 2 - (queue_waiting_num - AUDIO_OUT_FRAME_NUM / 4);
            } else {
                output_dma_lli_pool[lli_index].control.bits.TransferSize = output_using_frame[lli_index].data_size / 2;
            }
#else
            output_dma_lli_pool[lli_index].control.bits.TransferSize = output_using_frame[lli_index].data_size / 2;

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

    for (uint8_t i = 0; i < AUDIO_OUT_FRAME_NUM; i++) {
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

    bflb_i2s_feature_control(audio_i2s, I2S_CMD_DATA_ENABLE, I2S_CMD_DATA_ENABLE_TX | I2S_CMD_DATA_ENABLE_RX);
}

/* audio task init */
int external_codec_i2s_task_init(void)
{
    I2S_INFO("external_codec_i2s_task_init\r\n");

    /* i2s init */
    i2s_init();

#if (CONFIG_SOLUTION_FUNC_AUDIO_I2S_IN)
    /* i2s input dma init */
    i2s_input_dma_init();

    /* audio_record event */
    i2s_input_event_group = xEventGroupCreate();
    xEventGroupSetBits(i2s_input_event_group, (I2S_INPUT_EVENT_ENABLE | I2S_INPUT_EVENT_PAUSE));

    /* creat record task  */
    xTaskCreate(i2s_input_resume_task, (char *)"i2s_input_task", 512, NULL, I2S_INPUT_TASK_PRIORITY, &i2s_input_resume_task_hd);
#endif

#if (CONFIG_SOLUTION_FUNC_AUDIO_I2S_OUT)
    /* i2s output dma init */
    i2s_output_dma_init();

    /* audio_play event */
    i2s_output_event_group = xEventGroupCreate();
    xEventGroupSetBits(i2s_output_event_group, (I2S_OUTPUT_EVENT_ENABLE | I2S_OUTPUT_EVENT_PAUSE));

    /* create audio_out output queue */
    audio_out_out_queue_i2s_id = AUDIO_OUT_FRAME_STREAM_I2S_ID;
    if (frame_queue_output_create(g_audio_out_frame_ctrl, &audio_out_out_queue_i2s_id, AUDIO_OUT_FRAME_STREAM_I2S_DEPTH) < 0) {
        I2S_ERR("audio_out frame i2s output queue create failed\r\n");
        return -1;
    } else {
        I2S_INFO("audio_out frame i2s output queue ID: %d\r\n", audio_out_out_queue_i2s_id);
    }

    /* creat play task */
    xTaskCreate(i2s_output_resume_task, (char *)"i2s_output_task", 512, NULL, I2S_OUTPUT_TASK_PRIORITY, &i2s_output_resume_task_hd);
#endif

    return 0;
}
