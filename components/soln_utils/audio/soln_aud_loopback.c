#include "bflb_mtimer.h"
#include "bflb_l1c.h"

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <queue.h>
#include <portmacro.h>

#include "soln_fbq.h"

#include "soln_aud_loopback.h"

#ifdef CONFIG_SOLN_AUD_LOOPBACK_LOG_LEVEL
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOLN_AUD_LOOPBACK_LOG_LEVEL
#endif
#define DBG_TAG "SOLN_AUD_LOOP"
#include "log.h"

/* Local PCM queue output ID reserved for loopback. */
static uint16_t local_pcm_output_loopback_id;

/* task hd */
static TaskHandle_t audio_loopback_task_hd;

static void audio_play_lookback_task(void *pvParameters)
{
    int ret;
    fbq_elem_t *local_pcm_frame;
    fbq_elem_t *remote_pcm_frame;

    uint64_t time_last;
    uint32_t data_size;

    data_size = 0;

    time_last = bflb_mtimer_get_time_ms();

    while (1) {
        /* pop input frame */
        ret = fbq_pop(soln_fbq_aud_pcm_local(), &local_pcm_frame, local_pcm_output_loopback_id, 1000);
        if (ret != FBQ_OK) {
            LOG_W("local PCM pop timeout %d, continue wait... \r\n", ret);
            continue;
        }

        /* allco frame buff */
        ret = fbq_alloc(soln_fbq_aud_pcm_remote(), &remote_pcm_frame, 1000);
        if (ret != FBQ_OK) {
            fbq_free(local_pcm_frame);
            LOG_W("remote PCM alloc timeout %d, continue wait... \r\n", ret);
            continue;
        }
        remote_pcm_frame->type_mask = SOLUTION_FBQ_TYPE_MASK_AUDIO_PCM_LOOPBACK;

        if (local_pcm_frame->size > remote_pcm_frame->capacity) {
            LOG_E("frame size overflow: %lu > %lu\r\n",
                  (unsigned long)local_pcm_frame->size,
                  (unsigned long)remote_pcm_frame->capacity);
            fbq_free(local_pcm_frame);
            fbq_free(remote_pcm_frame);
            continue;
        }

        memcpy(remote_pcm_frame->data, local_pcm_frame->data, local_pcm_frame->size);
        remote_pcm_frame->size = local_pcm_frame->size;

        /* clean cache */
        bflb_l1c_dcache_clean_range(remote_pcm_frame->data, remote_pcm_frame->size);

        fbq_free(local_pcm_frame);

        uint32_t produced_size = remote_pcm_frame->size;
        (void)fbq_push_mask(soln_fbq_aud_pcm_remote(), remote_pcm_frame, FBQ_OUTPUT_ALL, 0);
        fbq_free(remote_pcm_frame);

        data_size += produced_size;

        if (data_size >= 256 * 1000) {
            uint64_t time = bflb_mtimer_get_time_ms();

            LOG_I("loopback sample data rate: %d Byte/s\r\n", data_size * 1000 / (uint32_t)(time - time_last));
            data_size = 0;

            time_last = time;
        }
    }
}

/* audio_loopback_task init */
int soln_aud_loopback_task_init(void)
{
    LOG_I("soln_aud_play_task_init\r\n");

    /* Create the loopback output subscription on the local PCM queue. */
    local_pcm_output_loopback_id = CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_LOOPBACK_ID;
    if (fbq_output_open(soln_fbq_aud_pcm_local(), &local_pcm_output_loopback_id,
                        CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_LOOPBACK_DEPTH,
                        SOLUTION_FBQ_TYPE_MASK_AUDIO_PCM_CAPTURE_ALL) != FBQ_OK) {
        LOG_E("local PCM loopback output create failed\r\n");
        return -1;
    } else {
        LOG_I("local PCM loopback output ID: %d\r\n", local_pcm_output_loopback_id);
    }

    /*  */
    xTaskCreate(audio_play_lookback_task, (char *)"play_lookback_task", 512, NULL, AUDIO_LOOPBACK_PRIORITY, &audio_loopback_task_hd);

    return 0;
}
