#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_audac.h"
#include "bflb_dma.h"
#include "bflb_l1c.h"

#include "bl616_glb.h"

#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <queue.h>
#include <portmacro.h>

#include "soln_fbq.h"

#include "soln_audac.h"

#ifdef CONFIG_SOLN_AUD_AUDAC_LOG_LEVEL
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOLN_AUD_AUDAC_LOG_LEVEL
#endif
#define DBG_TAG "SOLN_AUDAC"
#include "log.h"

/***** audac play ctrl *****/
/* enable/disable play */
#define AUDAC_EVENT_ENABLE    (0x01 << 0)
/* pause play */
#define AUDAC_EVENT_PAUSE     (0x01 << 1)
/* pause play */
#define AUDAC_EVENT_RATE_CONV (0x01 << 2)

static uint16_t remote_pcm_output_audac_id;

static TaskHandle_t audio_play_resume_task_hd;
static TaskHandle_t audio_play_rate_convert_task_hd;
static QueueHandle_t audio_play_rate_convert_queue;
static EventGroupHandle_t audio_play_event_group;

static struct bflb_device_s *audac_hd;
static struct bflb_device_s *audac_dma_hd;

static ATTR_NOCACHE_RAM_SECTION __ALIGNED(32) struct bflb_dma_channel_lli_pool_s dma_lli_pool[CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM];

static fbq_elem_t *audac_using_frame[CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM];
static volatile uint16_t audac_using_num;

/* audio_dma_isr */
ATTR_TCM_SECTION static void audac_dma_isr(void *arg)
{
    int ret;
    uint16_t frame_index, lli_index;
    EventBits_t play_event_bits;

    play_event_bits = xEventGroupGetBitsFromISR(audio_play_event_group);

    /* free frame data */
    for (frame_index = 0; frame_index < audac_using_num; frame_index++) {
        fbq_free(audac_using_frame[frame_index]);
        audac_using_frame[frame_index] = NULL;
    }

    if ((play_event_bits & AUDAC_EVENT_ENABLE) == 0) {
        /* disable play */
        xEventGroupSetBitsFromISR(audio_play_event_group, AUDAC_EVENT_PAUSE, NULL);
        return;
    }

    /* get new buffer frame */
    for (frame_index = 0; frame_index < (CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM / 3); frame_index++) {
        ret = fbq_pop(soln_fbq_aud_pcm_remote(), &audac_using_frame[frame_index], remote_pcm_output_audac_id, 0);
        if (ret != FBQ_OK) {
            break;
        }
    }

    /* update num */
    audac_using_num = frame_index;

    /* No buffers are available, pause play task */
    if (frame_index == 0) {
        xEventGroupSetBitsFromISR(audio_play_event_group, AUDAC_EVENT_PAUSE, NULL);
        return;
    }

#if (AUDAC_DMA_AUTO_DELE_EN)
    int queue_waiting_num = fbq_output_count(soln_fbq_aud_pcm_remote(), remote_pcm_output_audac_id);
#endif

    /* dma lli cfg */
    for (lli_index = 0; lli_index < frame_index; lli_index++) {
        dma_lli_pool[lli_index].src_addr = (uint32_t)(audac_using_frame[lli_index]->data);
        dma_lli_pool[lli_index].dst_addr = DMA_ADDR_AUDAC_TDR;
#if (AUDAC_DMA_AUTO_DELE_EN)
        if (queue_waiting_num > CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM / 4) {
            dma_lli_pool[lli_index].control.bits.TransferSize = audac_using_frame[lli_index]->size / 2 - (queue_waiting_num - CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM / 4);
        } else {
            dma_lli_pool[lli_index].control.bits.TransferSize = audac_using_frame[lli_index]->size / 2;
        }
#else
        dma_lli_pool[lli_index].control.bits.TransferSize = audac_using_frame[lli_index]->size / 2;

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
        bflb_dma_feature_control(audac_dma_hd, DMA_CMD_SET_LLI_CONFIG, (uint32_t)&dma_lli_pool[0]);
        bflb_dma_channel_start(audac_dma_hd);
    }
}

static void audio_play_rate_convert_task(void *pvParameters)
{
    fbq_elem_t *audio_frame;
    uint16_t *p_buff;
    uint16_t sample_cnt;

    while (1) {
        /* get data frame (form audac_dma_isr / usb) */
        xQueueReceive(audio_play_rate_convert_queue, &audio_frame, portMAX_DELAY);

        /* TODO: do conversion, 48K to 16k */
        p_buff = audio_frame->data;
        sample_cnt = audio_frame->size / 2;
        for (uint16_t i = 0; i < sample_cnt / 3; i++) {
            p_buff[i] = p_buff[i * 3];
        }

        audio_frame->size /= 3;

        /* clean dcache */
        bflb_l1c_dcache_clean_range(audio_frame->data, audio_frame->size);

        /* push to output queue */
        (void)fbq_push_mask(soln_fbq_aud_pcm_remote(), audio_frame, FBQ_OUTPUT_ALL, 0);
        fbq_free(audio_frame);
    }
}

int soln_aud_play_rate_convert_push(fbq_elem_t *elem)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t ret;

    if (audio_play_rate_convert_queue == NULL) {
        return -1;
    }

    if (xPortIsInsideInterrupt()) {
        ret = xQueueSendFromISR(audio_play_rate_convert_queue, &elem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        ret = xQueueSend(audio_play_rate_convert_queue, &elem, portMAX_DELAY);
    }

    return (ret == pdTRUE) ? 0 : -1;
}

static void audio_play_resume_task(void *pvParameters)
{
    int ret;
    uint16_t frame_index, lli_index;

    vTaskDelay(10);

    bflb_audac_feature_control(audac_hd, AUDAC_CMD_PLAY_START, 0);

    while (1) {
        /* enable and pause, resume to play */
        xEventGroupWaitBits(audio_play_event_group, (AUDAC_EVENT_ENABLE | AUDAC_EVENT_PAUSE), pdFALSE, pdTRUE, portMAX_DELAY);

        /* at least one buffer is required */
        ret = fbq_pop(soln_fbq_aud_pcm_remote(), &audac_using_frame[0], remote_pcm_output_audac_id, 1000);
        if (ret != FBQ_OK) {
            /* audac pool empty */
            LOG_W("remote PCM pop timeout %d, continue wait... \r\n", ret);
            continue;
        }

        LOG_D("audio play resume\r\n");

        /* get more buffers */
        for (frame_index = 1; frame_index < (CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM / 3); frame_index++) {
            ret = fbq_pop(soln_fbq_aud_pcm_remote(), &audac_using_frame[frame_index], remote_pcm_output_audac_id, 0);
            if (ret != FBQ_OK) {
                break;
            }
        }

        audac_using_num = frame_index;
#if (AUDAC_DMA_AUTO_DELE_EN)
        int queue_waiting_num = fbq_output_count(soln_fbq_aud_pcm_remote(), remote_pcm_output_audac_id);
#endif

        /* dma lli cfg */
        for (lli_index = 0; lli_index < frame_index; lli_index++) {
            dma_lli_pool[lli_index].src_addr = (uint32_t)(audac_using_frame[lli_index]->data);
            dma_lli_pool[lli_index].dst_addr = DMA_ADDR_AUDAC_TDR;
#if (AUDAC_DMA_AUTO_DELE_EN)
            if (queue_waiting_num > CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM / 4) {
                dma_lli_pool[lli_index].control.bits.TransferSize = audac_using_frame[lli_index]->size / 2 - (queue_waiting_num - CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM / 4);
            } else {
                dma_lli_pool[lli_index].control.bits.TransferSize = audac_using_frame[lli_index]->size / 2;
            }
#else
            dma_lli_pool[lli_index].control.bits.TransferSize = audac_using_frame[lli_index]->size / 2;

#endif
            dma_lli_pool[lli_index].nextlli = (uint32_t)(&dma_lli_pool[lli_index + 1]);
            dma_lli_pool[lli_index].control.bits.I = 0;
        }

        if (lli_index > 0) {
            dma_lli_pool[lli_index - 1].nextlli = 0;
            dma_lli_pool[lli_index - 1].control.bits.I = 1;
        }

        /* clear pause event bit */
        xEventGroupClearBits(audio_play_event_group, AUDAC_EVENT_PAUSE);

        /* start dma */
        bflb_dma_feature_control(audac_dma_hd, DMA_CMD_SET_LLI_CONFIG, (uint32_t)&dma_lli_pool[0]);
        bflb_dma_channel_start(audac_dma_hd);
    }
}

int soln_aud_play_rate_convert_task_init(void)
{
    LOG_I("audio play rate_convert init\r\n");

    /* audio_play_rate_convert queue */
    audio_play_rate_convert_queue = xQueueCreate(CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM, sizeof(fbq_elem_t *));

    /* audio_play_rate_convert task */
    xTaskCreate(audio_play_rate_convert_task, (char *)"play_rate_convert_task", 256, NULL, AUDAC_TASK_PRIORITY_MAIN - 2, &audio_play_rate_convert_task_hd);

    return 0;
}

/* audio gpio init*/
static void audac_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    /* audac pwm output mode */
    bflb_gpio_init(gpio, GPIO_PIN_27, GPIO_FUNC_AUDAC_PWM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    bflb_gpio_init(gpio, GPIO_PIN_28, GPIO_FUNC_AUDAC_PWM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);

    /* PA enable */
    bflb_gpio_init(gpio, GPIO_PIN_25, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_26, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_set(gpio, GPIO_PIN_25);
    bflb_gpio_set(gpio, GPIO_PIN_26);
}

/* audio dac init */
static void audac_init(void)
{
    /* audio dac config */
    struct bflb_audac_init_config_s audac_init_cfg = {
        .sampling_rate = AUDAC_SAMPLING_RATE_16K,
        .output_mode = AUDAC_OUTPUT_MODE_PWM,
        .source_channels_num = AUDAC_SOURCE_CHANNEL_SINGLE,
        .mixer_mode = AUDAC_MIXER_MODE_ONLY_L,
        .data_format = AUDAC_DATA_FORMAT_16BIT,
        .fifo_threshold = 7,
    };

    /* audio dac volume config */
    struct bflb_audac_volume_config_s audac_volume_cfg = {
        .mute_ramp_en = false,
        .mute_up_ramp_rate = AUDAC_RAMP_RATE_FS_32,
        .mute_down_ramp_rate = AUDAC_RAMP_RATE_FS_8,
        .volume_update_mode = AUDAC_VOLUME_UPDATE_MODE_FORCE,
        .volume_ramp_rate = AUDAC_RAMP_RATE_FS_128,
        .volume_zero_cross_timeout = AUDAC_RAMP_RATE_FS_128,
    };

    /* clock cfg */
    GLB_Config_AUDIO_PLL_To_491P52M();
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_AUDIO);

    /* audac init */
    audac_hd = bflb_device_get_by_name("audac");
    bflb_audac_init(audac_hd, &audac_init_cfg);
    bflb_audac_feature_control(audac_hd, AUDAC_CMD_SET_VOLUME_VAL, (size_t)(-15 * 2));
    bflb_audac_volume_init(audac_hd, &audac_volume_cfg);
    /* audac enable dma */
    bflb_audac_link_rxdma(audac_hd, true);
}

/* audio dac dma init */
static void audac_dma_init(void)
{
    struct bflb_dma_channel_config_s audac_dma_cfg;

    audac_dma_cfg.direction = DMA_MEMORY_TO_PERIPH;
    audac_dma_cfg.src_req = DMA_REQUEST_NONE;
    audac_dma_cfg.dst_req = DMA_REQUEST_AUDAC_TX;
    audac_dma_cfg.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    audac_dma_cfg.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    audac_dma_cfg.src_burst_count = DMA_BURST_INCR8;
    audac_dma_cfg.dst_burst_count = DMA_BURST_INCR8;
    audac_dma_cfg.src_width = DMA_DATA_WIDTH_16BIT;
    audac_dma_cfg.dst_width = DMA_DATA_WIDTH_16BIT;

    audac_dma_hd = bflb_device_get_by_name(AUDAC_DMA_NUME);
    bflb_dma_channel_init(audac_dma_hd, &audac_dma_cfg);
    bflb_dma_channel_irq_attach(audac_dma_hd, audac_dma_isr, NULL);

    for (uint8_t i = 0; i < CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_NUM; i++) {
        dma_lli_pool[i].control.WORD = bflb_dma_feature_control(audac_dma_hd, DMA_CMD_GET_LLI_CONTROL, 0);
    }
}

int soln_aud_play_task_init(void)
{
    LOG_I("audio play init\r\n");

    /* gpio init */
    audac_gpio_init();
    /* audac init */
    audac_init();
    /* audac init */
    audac_dma_init();

    /* audio_play event */
    audio_play_event_group = xEventGroupCreate();
    xEventGroupSetBits(audio_play_event_group, (AUDAC_EVENT_ENABLE | AUDAC_EVENT_PAUSE));

    if (audio_play_rate_convert_task_hd) {
        /* enable sample rate conversion */
        xEventGroupSetBits(audio_play_event_group, AUDAC_EVENT_RATE_CONV);
    }

    /* Create the AUDAC output subscription on the remote PCM queue. */
    remote_pcm_output_audac_id = CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_AUDAC_ID;
    if (fbq_output_open(soln_fbq_aud_pcm_remote(), &remote_pcm_output_audac_id,
                        CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_AUDAC_DEPTH,
                        SOLUTION_FBQ_TYPE_MASK_AUDIO_PCM_LOOPBACK) != FBQ_OK) {
        LOG_E("remote PCM AUDAC output subscription create failed\r\n");
        return -1;
    } else {
        LOG_I("remote PCM AUDAC output ID: %d\r\n", remote_pcm_output_audac_id);
    }

    /* creat play  */
    xTaskCreate(audio_play_resume_task, (char *)"play_resume_task", 512, NULL, AUDAC_TASK_PRIORITY_MAIN, &audio_play_resume_task_hd);

    return 0;
}
