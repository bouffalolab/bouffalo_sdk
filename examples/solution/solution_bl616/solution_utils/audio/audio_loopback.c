#include "bflb_mtimer.h"
#include "bflb_l1c.h"

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <queue.h>
#include <portmacro.h>

#include "frame_queue_ctrl.h"
#include "frame_queue.h"

#include "audio_loopback.h"

#if (0)
#define LOOPBACK_DBG(a, ...) printf("[LOOPBACK dbg]:" a, ##__VA_ARGS__)
#else
#define LOOPBACK_DBG(a, ...)
#endif

#if (1)
#define LOOPBACK_INFO(a, ...) printf("[LOOPBACK info]:" a, ##__VA_ARGS__)
#else
#define LOOPBACK_INFO(a, ...)
#endif

#define LOOPBACK_ERR(a, ...) printf("[LOOPBACK err]:" a, ##__VA_ARGS__)

/* audio queue output id */
static uint16_t audio_in_out_queue_lookback_id;

/* task hd */
static TaskHandle_t audio_loopback_task_hd;

static void audio_play_lookback_task(void *pvParameters)
{
    int ret;
    audio_in_frame_t audio_input_frame;
    audio_out_frame_t audio_output_frame;

    uint64_t time_last;
    uint32_t data_size;

    data_size = 0;

    time_last = bflb_mtimer_get_time_ms();

    while (1) {
        /* pop input frame */
        ret = frame_queue_output_pop(g_audio_in_frame_ctrl, (frame_elem_t *)&audio_input_frame, audio_in_out_queue_lookback_id, 1000);
        if (ret < 0) {
            LOOPBACK_INFO("[audac] lookback pop timeout %d, continue wait... \r\n", ret);
            continue;
        }

        /* allco frame buff */
        ret = frame_queue_alloc_lock(g_audio_out_frame_ctrl, (frame_elem_t *)&audio_output_frame, 1000);
        if (ret < 0) {
            frame_queue_output_free(g_audio_in_frame_ctrl, (frame_elem_t *)&audio_input_frame);
            LOOPBACK_INFO("[audac] lookback alloc timeout %d, continue wait... \r\n", ret);
            continue;
        }

        bflb_l1c_dcache_invalidate_range(audio_input_frame.elem_base.frame_addr, audio_input_frame.data_size);

        /* cpoy input_data to output_data */
        memcpy(audio_output_frame.elem_base.frame_addr, audio_input_frame.elem_base.frame_addr, audio_input_frame.data_size);
        audio_output_frame.data_size = audio_input_frame.data_size;

        /* clean cache */
        bflb_l1c_dcache_clean_range(audio_output_frame.elem_base.frame_addr, audio_output_frame.data_size);

        /* free the audio_in frame */
        frame_queue_output_free(g_audio_in_frame_ctrl, (frame_elem_t *)&audio_input_frame);

        /* push audio_out frame */
        for (uint8_t i = 0; i < OUTPUT_QUEUE_NUM_MAX; i++) {
            frame_queue_push(g_audio_out_frame_ctrl, (frame_elem_t *)&audio_output_frame, i, 0);
        }
        frame_queue_free_unlock(g_audio_out_frame_ctrl, (frame_elem_t *)&audio_output_frame);

        /*  */
        data_size += audio_output_frame.data_size;

        if (data_size >= 256 * 1000) {
            uint64_t time = bflb_mtimer_get_time_ms();

            LOOPBACK_INFO("loopback sample data rate: %d Byte/s\r\n", data_size * 1000 / (uint32_t)(time - time_last));
            data_size = 0;

            time_last = time;
        }
    }
}

/* audio_loopback_task init */
int audio_loopback_task_init(void)
{
    LOOPBACK_INFO("audio_play_task_init\r\n");

    /* create audio_in output queue */
    audio_in_out_queue_lookback_id = AUDIO_IN_FRAME_STREAM_LOOKBACK_ID;
    if (frame_queue_output_create(g_audio_in_frame_ctrl, &audio_in_out_queue_lookback_id, AUDIO_IN_FRAME_STREAM_LOOKBACK_DEPTH) < 0) {
        LOOPBACK_ERR("audio_in frame lookback out queue create failed\r\n");
        return -1;
    } else {
        LOOPBACK_INFO("audio_in frame lookback out queue ID: %d\r\n", audio_in_out_queue_lookback_id);
    }

    /*  */
    xTaskCreate(audio_play_lookback_task, (char *)"play_lookback_task", 512, NULL, AUDIO_LOOPBACK_PRIORITY, &audio_loopback_task_hd);

    return 0;
}
