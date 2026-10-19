#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_auadc.h"
#include "bflb_dma.h"
#include "bflb_l1c.h"

#include "bl616_glb.h"

#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <queue.h>
#include <portmacro.h>

#include "soln_fbq.h"

#include "soln_auadc.h"

#ifdef CONFIG_SOLN_AUD_AUADC_LOG_LEVEL
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOLN_AUD_AUADC_LOG_LEVEL
#endif
#define DBG_TAG "SOLN_AUADC"
#include "log.h"

/***** auadc record ctrl *****/
/* enable/disable record */
#define AUADC_EVENT_ENABLE    (0x01 << 0)
/* pause record */
#define AUADC_EVENT_PAUSE     (0x01 << 1)
/* pause record */
#define AUADC_EVENT_RATE_CONV (0x01 << 2)

static TaskHandle_t audio_record_resume_task_hd;
static TaskHandle_t audio_record_rate_convert_task_hd;
static QueueHandle_t audio_record_rate_convert_queue;
static EventGroupHandle_t audio_record_event_group;

static struct bflb_device_s *auadc_hd;
static struct bflb_device_s *auadc_dma_hd;

static ATTR_NOCACHE_RAM_SECTION __ALIGNED(32) struct bflb_dma_channel_lli_pool_s dma_lli_pool[CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM];

static fbq_elem_t *auadc_using_frame[CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM];
static volatile uint16_t auadc_using_num;

/* audio_dma_isr */
ATTR_TCM_SECTION static void auadc_dma_isr(void *arg)
{
    int ret;
    uint16_t frame_index, lli_index;
    EventBits_t record_event_bits;

    record_event_bits = xEventGroupGetBitsFromISR(audio_record_event_group);

    /* push frame data */
    for (frame_index = 0; frame_index < auadc_using_num; frame_index++) {
        /* set size */
        fbq_elem_t *elem = auadc_using_frame[frame_index];
        elem->size = AUADC_FRAME_SIZE;
        elem->type_mask = SOLUTION_FBQ_TYPE_MASK_AUDIO_PCM_AUADC;

        bflb_l1c_dcache_invalidate_range(elem->data, elem->size + 31U);

        if (record_event_bits & AUADC_EVENT_RATE_CONV) {
            /* push to rate_convert queue */
            ret = soln_aud_record_rate_convert_push(elem, 0);
            if (ret < 0) {
                fbq_free(elem);
            }
        } else {
            (void)fbq_push_mask(soln_fbq_aud_pcm_local(), elem, FBQ_OUTPUT_ALL, 0);
            fbq_free(elem);
        }
        auadc_using_frame[frame_index] = NULL;
    }

    if ((record_event_bits & AUADC_EVENT_ENABLE) == 0) {
        /* disable record */
        xEventGroupSetBitsFromISR(audio_record_event_group, AUADC_EVENT_PAUSE, NULL);
        return;
    }

    /* get new buffer */
    for (frame_index = 0; frame_index < (CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM / 3); frame_index++) {
        ret = fbq_alloc(soln_fbq_aud_pcm_local(), &auadc_using_frame[frame_index], 0);
        if (ret != FBQ_OK) {
            break;
        }
    }

    /* No buffs are available, pause record task */
    if (frame_index == 0) {
        xEventGroupSetBitsFromISR(audio_record_event_group, AUADC_EVENT_PAUSE, NULL);
        return;
    }

    /* update num */
    auadc_using_num = frame_index;
#if (AUADC_DMA_AUTO_DELE_EN)
    int queue_waiting_num = fbq_free_count(soln_fbq_aud_pcm_local());
#endif

    /* dma lli cfg */
    for (lli_index = 0; lli_index < frame_index; lli_index++) {
        dma_lli_pool[lli_index].src_addr = DMA_ADDR_AUADC_RDR;
        dma_lli_pool[lli_index].dst_addr = (uint32_t)(auadc_using_frame[lli_index]->data);

#if (AUADC_DMA_AUTO_DELE_EN)
        if (queue_waiting_num < CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM / 4) {
            dma_lli_pool[lli_index].control.bits.TransferSize = AUADC_FRAME_SIZE / 2 + (CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM / 4 - queue_waiting_num);
        } else {
            dma_lli_pool[lli_index].control.bits.TransferSize = AUADC_FRAME_SIZE / 2;
        }
#else
        dma_lli_pool[lli_index].control.bits.TransferSize = AUADC_FRAME_SIZE / 2;

#endif

        dma_lli_pool[lli_index].nextlli = (uint32_t)(&dma_lli_pool[lli_index + 1]);
        dma_lli_pool[lli_index].control.bits.I = 0;
    }

    if (lli_index > 0) {
        dma_lli_pool[lli_index - 1].nextlli = 0;
        dma_lli_pool[lli_index - 1].control.bits.I = 1;
    }

    /* start dma */
    if (lli_index > 0) {
        bflb_dma_feature_control(auadc_dma_hd, DMA_CMD_SET_LLI_CONFIG, (uint32_t)&dma_lli_pool[0]);
        bflb_dma_channel_start(auadc_dma_hd);
    }
}

static void audio_record_rate_convert_task(void *pvParameters)
{
    fbq_elem_t *audio_frame;
    uint16_t *p_buff;
    uint16_t sample_cnt;

    while (1) {
        /* get data frame (form auadc_dma_isr / usb) */
        xQueueReceive(audio_record_rate_convert_queue, &audio_frame, portMAX_DELAY);

        /* TODO: do conversion */
        p_buff = audio_frame->data;
        sample_cnt = audio_frame->size / 2;
        for (uint16_t i = 0; i < sample_cnt / 3; i++) {
            p_buff[i] = p_buff[i * 3] / 2;
        }

        audio_frame->size /= 3;

        /* clean dcache */
        bflb_l1c_dcache_clean_range(audio_frame->data, audio_frame->size);

        /* push to output queue */
        (void)fbq_push_mask(soln_fbq_aud_pcm_local(), audio_frame, FBQ_OUTPUT_ALL, 0);
        fbq_free(audio_frame);
    }
}

int soln_aud_record_rate_convert_push(fbq_elem_t *elem, uint32_t timeout)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t ret;

    if (audio_record_rate_convert_queue == NULL) {
        return -1;
    }

    if (xPortIsInsideInterrupt()) {
        ret = xQueueSendFromISR(audio_record_rate_convert_queue, &elem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        ret = xQueueSend(audio_record_rate_convert_queue, &elem, timeout);
    }

    if (ret == pdTRUE) {
        return 0;
    } else {
        return -2;
    }
}

static void audio_record_resume_task(void *pvParameters)
{
    int ret;
    uint16_t frame_index, lli_index;

    vTaskDelay(20);

    bflb_auadc_feature_control(auadc_hd, AUADC_CMD_RECORD_START, 0);

    while (1) {
        /* enable and pause, resume to record */
        xEventGroupWaitBits(audio_record_event_group, (AUADC_EVENT_ENABLE | AUADC_EVENT_PAUSE), pdFALSE, pdTRUE, portMAX_DELAY);

        /* allco frame buff */
        ret = fbq_alloc(soln_fbq_aud_pcm_local(), &auadc_using_frame[0], 1000);
        if (ret != FBQ_OK) {
            LOG_W("local PCM alloc timeout %d, continue wait... \r\n", ret);
            continue;
        }

        LOG_D("audio record resume\r\n");

        /* get more buffs */
        for (frame_index = 1; frame_index < (CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM / 3); frame_index++) {
            ret = fbq_alloc(soln_fbq_aud_pcm_local(), &auadc_using_frame[frame_index], 0);
            if (ret != FBQ_OK) {
                break;
            }
        }

        auadc_using_num = frame_index;
#if (AUADC_DMA_AUTO_DELE_EN)
        int queue_waiting_num = fbq_free_count(soln_fbq_aud_pcm_local());
#endif

        /* dma lli cfg */
        for (lli_index = 0; lli_index < frame_index; lli_index++) {
            dma_lli_pool[lli_index].src_addr = DMA_ADDR_AUADC_RDR;
            dma_lli_pool[lli_index].dst_addr = (uint32_t)(auadc_using_frame[lli_index]->data);

#if (AUADC_DMA_AUTO_DELE_EN)
            if (queue_waiting_num < CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM / 4) {
                dma_lli_pool[lli_index].control.bits.TransferSize = AUADC_FRAME_SIZE / 2 + (CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM / 4 - queue_waiting_num);
            } else {
                dma_lli_pool[lli_index].control.bits.TransferSize = AUADC_FRAME_SIZE / 2;
            }
#else
            dma_lli_pool[lli_index].control.bits.TransferSize = AUADC_FRAME_SIZE / 2;

#endif

            dma_lli_pool[lli_index].nextlli = (uint32_t)(&dma_lli_pool[lli_index + 1]);
            dma_lli_pool[lli_index].control.bits.I = 0;
        }

        if (lli_index > 0) {
            dma_lli_pool[lli_index - 1].nextlli = 0;
            dma_lli_pool[lli_index - 1].control.bits.I = 1;
        }

        /* clear pause event bit */
        xEventGroupClearBits(audio_record_event_group, AUADC_EVENT_PAUSE);

        /* start dma */
        bflb_dma_feature_control(auadc_dma_hd, DMA_CMD_SET_LLI_CONFIG, (uint32_t)&dma_lli_pool[0]);
        bflb_dma_channel_start(auadc_dma_hd);
    }
}

int soln_aud_record_rate_convert_task_init(void)
{
    LOG_I("audio record rate_convert init\r\n");

    /* audio_record_rate_convert queue */
    audio_record_rate_convert_queue = xQueueCreate(CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM, sizeof(fbq_elem_t *));

    /* audio_record_rate_convert task */
    xTaskCreate(audio_record_rate_convert_task, (char *)"record_rate_convert_task", 256, NULL, AUADC_TASK_PRIORITY_MAIN - 1, &audio_record_rate_convert_task_hd);

    return 0;
}

/* audio gpio init*/
static void auadc_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    /* auadc */
    bflb_gpio_init(gpio, GPIO_PIN_23, GPIO_ANALOG | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_2);
    bflb_gpio_init(gpio, GPIO_PIN_30, GPIO_ANALOG | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_2);
}

/* audio adc init */
static void auadc_init(void)
{
    /* audio adc config */
    struct bflb_auadc_init_config_s auadc_init_cfg = {
        .sampling_rate = AUADC_SAMPLING_RATE_16K,
        .input_mode = AUADC_INPUT_MODE_ADC,
        .data_format = AUADC_DATA_FORMAT_16BIT,
        .fifo_threshold = 3,
    };

    /* audio adc analog config */
    struct bflb_auadc_adc_init_config_s auadc_analog_init_cfg = {
        .auadc_analog_en = true,
        .adc_mode = AUADC_ADC_MODE_AUDIO,
        .adc_pga_mode = AUADC_ADC_PGA_MODE_AC_DIFFER,
        .adc_pga_posi_ch = AUADC_ADC_ANALOG_CH_3,
        .adc_pga_nega_ch = AUADC_ADC_ANALOG_CH_7,
        .adc_pga_gain = 36,
    };

    /* clock cfg */
    GLB_Config_AUDIO_PLL_To_491P52M();
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_AUDIO);

    /* auadc init */
    auadc_hd = bflb_device_get_by_name("auadc");
    bflb_auadc_init(auadc_hd, &auadc_init_cfg);
    bflb_auadc_adc_init(auadc_hd, &auadc_analog_init_cfg);
    /* set volume */
    bflb_auadc_feature_control(auadc_hd, AUADC_CMD_SET_VOLUME_VAL, (size_t)(0));
    /* auadc enable dma */
    bflb_auadc_link_rxdma(auadc_hd, true);
}

/* audio adc dma init */
static void auadc_dma_init(void)
{
    struct bflb_dma_channel_config_s auadc_dma_cfg;

    auadc_dma_cfg.direction = DMA_PERIPH_TO_MEMORY;
    auadc_dma_cfg.src_req = DMA_REQUEST_AUADC_RX;
    auadc_dma_cfg.dst_req = DMA_REQUEST_NONE;
    auadc_dma_cfg.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    auadc_dma_cfg.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    auadc_dma_cfg.src_burst_count = DMA_BURST_INCR4;
    auadc_dma_cfg.dst_burst_count = DMA_BURST_INCR4;
    auadc_dma_cfg.src_width = DMA_DATA_WIDTH_16BIT;
    auadc_dma_cfg.dst_width = DMA_DATA_WIDTH_16BIT;

    auadc_dma_hd = bflb_device_get_by_name(AUADC_DMA_NUME);
    bflb_dma_channel_init(auadc_dma_hd, &auadc_dma_cfg);
    bflb_dma_channel_irq_attach(auadc_dma_hd, auadc_dma_isr, NULL);

    for (uint8_t i = 0; i < CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_NUM; i++) {
        dma_lli_pool[i].control.WORD = bflb_dma_feature_control(auadc_dma_hd, DMA_CMD_GET_LLI_CONTROL, 0);
    }
}

int soln_aud_record_task_init(void)
{
    LOG_I("audio record init\r\n");

    /* gpio init */
    auadc_gpio_init();
    /* auadc init */
    auadc_init();
    /* auadc init */
    auadc_dma_init();

    /* audio_record event */
    audio_record_event_group = xEventGroupCreate();
    xEventGroupSetBits(audio_record_event_group, (AUADC_EVENT_ENABLE | AUADC_EVENT_PAUSE));

    if (audio_record_rate_convert_queue) {
        /* enable sample rate conversion */
        LOG_I("auadc enable rate_convert task\r\n");
        xEventGroupSetBits(audio_record_event_group, AUADC_EVENT_RATE_CONV);
    }

    /* creat record  */
    xTaskCreate(audio_record_resume_task, (char *)"record_resume_task", 512, NULL, AUADC_TASK_PRIORITY_MAIN, &audio_record_resume_task_hd);

    return 0;
}
