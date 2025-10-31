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

#include "frame_queue_ctrl.h"
#include "frame_queue.h"

#include "auadc.h"

#if (0)
#define AUADC_DBG(a, ...) printf("[AUADC dbg]:" a, ##__VA_ARGS__)
#else
#define AUADC_DBG(a, ...)
#endif

#if (1)
#define AUADC_INFO(a, ...) printf("[AUADC info]:" a, ##__VA_ARGS__)
#else
#define AUADC_INFO(a, ...)
#endif

#define AUADC_ERR(a, ...)     printf("[AUADC err]:" a, ##__VA_ARGS__)

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

static ATTR_NOCACHE_RAM_SECTION __ALIGNED(32) struct bflb_dma_channel_lli_pool_s dma_lli_pool[AUDIO_IN_FRAME_NUM];

static audio_in_frame_t auadc_using_frame[AUDIO_IN_FRAME_NUM];
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
        auadc_using_frame[frame_index].data_size = AUADC_FRAME_SIZE;

        bflb_l1c_dcache_invalidate_range(auadc_using_frame[frame_index].elem_base.frame_addr, auadc_using_frame[frame_index].data_size + 31);

        if (record_event_bits & AUADC_EVENT_RATE_CONV) {
            /* push to rate_convert queue */
            ret = audio_record_rate_convert_push(&auadc_using_frame[frame_index], 0);
            if (ret < 0) {
                /* unlock elem */
                frame_queue_free_unlock(g_audio_in_frame_ctrl, (frame_elem_t *)&auadc_using_frame[frame_index]);
            }
        } else {
            /* push to frame queue */
            for (uint8_t i = 0; i < OUTPUT_QUEUE_NUM_MAX; i++) {
                frame_queue_push(g_audio_in_frame_ctrl, (frame_elem_t *)&auadc_using_frame[frame_index], i, 0);
            }

            /* unlock elem */
            frame_queue_free_unlock(g_audio_in_frame_ctrl, (frame_elem_t *)&auadc_using_frame[frame_index]);
        }
    }

    if ((record_event_bits & AUADC_EVENT_ENABLE) == 0) {
        /* disable record */
        xEventGroupSetBitsFromISR(audio_record_event_group, AUADC_EVENT_PAUSE, NULL);
        return;
    }

    /* get new buffer */
    for (frame_index = 0; frame_index < (AUDIO_IN_FRAME_NUM / 3); frame_index++) {
        ret = frame_queue_alloc_lock(g_audio_in_frame_ctrl, (frame_elem_t *)&auadc_using_frame[frame_index], 0);
        if (ret < 0) {
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
    int queue_waiting_num = frame_queue_pool_get_available(g_audio_in_frame_ctrl);
#endif

    /* dma lli cfg */
    for (lli_index = 0; lli_index < frame_index; lli_index++) {
        dma_lli_pool[lli_index].src_addr = DMA_ADDR_AUADC_RDR;
        dma_lli_pool[lli_index].dst_addr = (uint32_t)(auadc_using_frame[lli_index].elem_base.frame_addr);

#if (AUADC_DMA_AUTO_DELE_EN)
        if (queue_waiting_num < AUDIO_IN_FRAME_NUM / 4) {
            dma_lli_pool[lli_index].control.bits.TransferSize = AUADC_FRAME_SIZE / 2 + (AUDIO_IN_FRAME_NUM / 4 - queue_waiting_num);
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
    audio_in_frame_t audio_frame;
    uint16_t *p_buff;
    uint16_t sample_cnt;

    while (1) {
        /* get data frame (form auadc_dma_isr / usb) */
        xQueueReceive(audio_record_rate_convert_queue, &audio_frame, portMAX_DELAY);

        /* TODO: do conversion */
        p_buff = audio_frame.elem_base.frame_addr;
        sample_cnt = audio_frame.data_size / 2;
        for (uint16_t i = 0; i < sample_cnt / 3; i++) {
            p_buff[i] = p_buff[i * 3] / 2;
        }

        audio_frame.data_size /= 3;

        /* clean dcache */
        bflb_l1c_dcache_clean_range(audio_frame.elem_base.frame_addr, audio_frame.data_size);

        /* push to output queue */
        for (uint8_t i = 0; i < OUTPUT_QUEUE_NUM_MAX; i++) {
            frame_queue_push(g_audio_in_frame_ctrl, (frame_elem_t *)&audio_frame, i, 0);
        }

        /* unlock */
        frame_queue_free_unlock(g_audio_in_frame_ctrl, (frame_elem_t *)&audio_frame);
    }
}

int audio_record_rate_convert_push(audio_in_frame_t *auadc_frame_info, uint32_t timeout)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t ret;

    if (audio_record_rate_convert_queue == NULL) {
        return -1;
    }

    if (xPortIsInsideInterrupt()) {
        ret = xQueueSendFromISR(audio_record_rate_convert_queue, auadc_frame_info, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        ret = xQueueSend(audio_record_rate_convert_queue, auadc_frame_info, timeout);
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
        ret = frame_queue_alloc_lock(g_audio_in_frame_ctrl, (frame_elem_t *)&auadc_using_frame[0], 1000);
        if (ret < 0) {
            AUADC_INFO("[auadc] audio_in alloc timeout %d, continue wait... \r\n", ret);
            continue;
        }

        AUADC_DBG("audio record resume\r\n");

        /* get more buffs */
        for (frame_index = 1; frame_index < (AUDIO_IN_FRAME_NUM / 3); frame_index++) {
            ret = frame_queue_alloc_lock(g_audio_in_frame_ctrl, (frame_elem_t *)&auadc_using_frame[frame_index], 0);
            if (ret < 0) {
                break;
            }
        }

        auadc_using_num = frame_index;
#if (AUADC_DMA_AUTO_DELE_EN)
        int queue_waiting_num = frame_queue_pool_get_available(g_audio_in_frame_ctrl);
#endif

        /* dma lli cfg */
        for (lli_index = 0; lli_index < frame_index; lli_index++) {
            dma_lli_pool[lli_index].src_addr = DMA_ADDR_AUADC_RDR;
            dma_lli_pool[lli_index].dst_addr = (uint32_t)(auadc_using_frame[lli_index].elem_base.frame_addr);

#if (AUADC_DMA_AUTO_DELE_EN)
            if (queue_waiting_num < AUDIO_IN_FRAME_NUM / 4) {
                dma_lli_pool[lli_index].control.bits.TransferSize = AUADC_FRAME_SIZE / 2 + (AUDIO_IN_FRAME_NUM / 4 - queue_waiting_num);
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

int audio_record_rate_convert_task_init(void)
{
    AUADC_INFO("audio record rate_convert init\r\n");

    /* audio_record_rate_convert queue */
    audio_record_rate_convert_queue = xQueueCreate(AUDIO_IN_FRAME_NUM, sizeof(audio_in_frame_t));

    /* audio_record_rate_convert task */
    xTaskCreate(audio_record_rate_convert_task, (char *)"record_rate_convert_task", 256, NULL, AUADC_TASK_PRIORITY_MAIN - 1, &audio_record_rate_convert_task_hd);

    return 0;
}

/* audio gpio init*/
void auadc_gpio_init(void)
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

    for (uint8_t i = 0; i < AUDIO_IN_FRAME_NUM; i++) {
        dma_lli_pool[i].control.WORD = bflb_dma_feature_control(auadc_dma_hd, DMA_CMD_GET_LLI_CONTROL, 0);
    }
}

int audio_record_task_init(void)
{
    AUADC_INFO("audio record init\r\n");

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
        AUADC_INFO("auadc enable rate_convert task\r\n");
        xEventGroupSetBits(audio_record_event_group, AUADC_EVENT_RATE_CONV);
    }

    /* creat record  */
    xTaskCreate(audio_record_resume_task, (char *)"record_resume_task", 512, NULL, AUADC_TASK_PRIORITY_MAIN, &audio_record_resume_task_hd);

    return 0;
}
