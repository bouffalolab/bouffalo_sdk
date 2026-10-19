/****************************************************************************
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "bflb_core.h"
#include "bflb_mtimer.h"

#include <stdio.h>
#include <stdint.h>

#include "soln_solution.h"
#include "FreeRTOS.h"
#include "task.h"

#ifdef CONFIG_SOLN_LOG_LEVEL
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOLN_LOG_LEVEL
#endif
#define DBG_TAG "SOLN"
#include "log.h"

#ifndef CONFIG_SOLN_FPS_STAT_INTERVAL_MS
#define CONFIG_SOLN_FPS_STAT_INTERVAL_MS (1000)
#endif

#ifndef CONFIG_SOLN_FPS_PRINT_INTERVAL_MULTIPLIER
#define CONFIG_SOLN_FPS_PRINT_INTERVAL_MULTIPLIER (2)
#endif

#if (CONFIG_SOLN_FPS_STAT_INTERVAL_MS == 0)
#error "CONFIG_SOLN_FPS_STAT_INTERVAL_MS must be greater than zero"
#endif

#if (CONFIG_SOLN_FPS_PRINT_INTERVAL_MULTIPLIER == 0)
#error "CONFIG_SOLN_FPS_PRINT_INTERVAL_MULTIPLIER must be greater than zero"
#endif

#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_RAW_LOCAL_EN) || IS_ENABLED(CONFIG_SOLN_FBQ_VID_RAW_REMOTE_EN) ||   \
    IS_ENABLED(CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_EN) || IS_ENABLED(CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_EN) || \
    IS_ENABLED(CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_EN) || IS_ENABLED(CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_EN)
#include "soln_fbq.h"
#endif

/********* solution audio *********/
#if IS_ENABLED(CONFIG_SOLN_AUD_AUADC_EN)
#include "soln_auadc.h"
#endif

#if IS_ENABLED(CONFIG_SOLN_AUD_AUDAC_EN)
#include "soln_audac.h"
#endif

#if IS_ENABLED(CONFIG_SOLN_AUD_I2S_IN_EN) || IS_ENABLED(CONFIG_SOLN_AUD_I2S_OUT_EN)
#include "soln_aud_external_codec_i2s.h"
#endif

#if IS_ENABLED(CONFIG_SOLN_AUD_LOOPBACK_EN)
#include "soln_aud_loopback.h"
#endif

#if IS_ENABLED(CONFIG_SOLN_AUD_ES8388_EN)
#include "soln_aud_codec_es8388_cfg.h"
#endif

/********* solution video *********/
#if IS_ENABLED(CONFIG_SOLN_DISP_DBI_EN)
#include "soln_dbi_disp.h"
#endif

#if IS_ENABLED(CONFIG_SOLN_DISP_RGB_EN)
#include "soln_rgb_disp.h"
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_JPEG_ENC_EN)
#include "soln_mjpeg_enc.h"
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_JPEG_DEC_EN)
#include "soln_mjpeg_dec.h"
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_DVP_EN)
#include "soln_dvp.h"
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_DVP_JPEG_ENC_EN)
#include "soln_dvp_mjpeg_enc.h"
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_UVC_YUYV_EN) || IS_ENABLED(CONFIG_SOLN_VID_UVC_JPEG_EN)
#include "usbh_uvc_stream.h"
#endif

#if IS_ENABLED(CONFIG_SOLN_SD_AVI_AUDIO_EN) || IS_ENABLED(CONFIG_SOLN_SD_AVI_VIDEO_EN)
#include "soln_jpeg_sd.h"
#include "soln_avi_jpeg_sd.h"
#endif

#if IS_ENABLED(CONFIG_SOLN_HB_TX_EN)
#include "soln_hb_start_tx.h"
#endif

#if IS_ENABLED(CONFIG_SOLN_HB_RX_EN)
#include "soln_hb_start_rx.h"
#endif

typedef struct {
    uint32_t dbi_disp;
    uint32_t rgb_disp;
    uint32_t dvp;
    uint32_t mjpeg_enc;
    uint32_t mjpeg_dec;
    uint32_t dvp_mjpeg_enc;
    uint32_t uvc;
    uint32_t avi_sd;
    uint32_t hb_tx;
    uint32_t hb_rx;
} solution_frame_count_snapshot_t;

static solution_fps_stats_t s_fps_stats = { 0 };

static solution_frame_count_snapshot_t solution_frame_count_snapshot_get(void)
{
    solution_frame_count_snapshot_t snapshot = { 0 };

#if IS_ENABLED(CONFIG_SOLN_DISP_DBI_EN)
    snapshot.dbi_disp = soln_dbi_disp_get_total_frame_count();
#endif

#if IS_ENABLED(CONFIG_SOLN_DISP_RGB_EN)
    snapshot.rgb_disp = soln_rgb_disp_get_total_frame_count();
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_DVP_EN)
    snapshot.dvp = soln_dvp_get_total_frame_count();
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_JPEG_ENC_EN)
    snapshot.mjpeg_enc = soln_mjpeg_enc_get_total_frame_count();
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_JPEG_DEC_EN)
    snapshot.mjpeg_dec = soln_mjpeg_dec_get_total_frame_count();
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_DVP_JPEG_ENC_EN)
    snapshot.dvp_mjpeg_enc = soln_dvp_mjpeg_enc_get_total_frame_count();
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_UVC_YUYV_EN) || IS_ENABLED(CONFIG_SOLN_VID_UVC_JPEG_EN)
    snapshot.uvc = soln_usbh_video_get_total_frame_count();
#endif

#if IS_ENABLED(CONFIG_SOLN_SD_AVI_VIDEO_EN)
    snapshot.avi_sd = soln_avi_jpeg_sd_get_total_frame_count();
#endif

#if IS_ENABLED(CONFIG_SOLN_HB_TX_EN)
    snapshot.hb_tx = soln_hb_sender_get_total_frame_count();
#endif

#if IS_ENABLED(CONFIG_SOLN_HB_RX_EN)
    snapshot.hb_rx = soln_hb_recv_get_total_frame_count();
#endif

    return snapshot;
}

static uint32_t solution_fps_calculate(uint32_t current_frame_count,
                                       uint32_t previous_frame_count,
                                       uint64_t elapsed_ms)
{
    if (elapsed_ms == 0U) {
        return 0U;
    }

    uint32_t frame_count = (current_frame_count >= previous_frame_count) ?
                               (current_frame_count - previous_frame_count) :
                               current_frame_count;
    return (uint32_t)(((uint64_t)frame_count * 1000U) / elapsed_ms);
}

static void solution_fps_stats_publish(const solution_fps_stats_t *stats)
{
    uintptr_t irq_flags = bflb_irq_save();
    s_fps_stats = *stats;
    bflb_irq_restore(irq_flags);
}

int soln_fps_stats_get(solution_fps_stats_t *stats)
{
    if (stats == NULL) {
        return -1;
    }

    uintptr_t irq_flags = bflb_irq_save();
    *stats = s_fps_stats;
    bflb_irq_restore(irq_flags);

    return 0;
}

static int solution_fps_str_append(char *buffer, uint32_t buffer_size,
                                   int offset, const char *format, uint32_t fps)
{
    if ((offset < 0) || ((uint32_t)offset >= buffer_size)) {
        return -1;
    }

    int size = snprintf(buffer + offset, buffer_size - (uint32_t)offset,
                        format, (unsigned long)fps);
    if ((size < 0) || ((uint32_t)size >= (buffer_size - (uint32_t)offset))) {
        return -1;
    }

    return offset + size;
}

int soln_fps_str_get(char *str_buff_total, uint32_t buff_size)
{
    solution_fps_stats_t stats;
    int str_total_size;

    if ((str_buff_total == NULL) || (buff_size < 16U)) {
        return -1;
    }

    if (soln_fps_stats_get(&stats) < 0) {
        return -1;
    }

    str_total_size = snprintf(str_buff_total, buff_size, "FPS:");
    if ((str_total_size < 0) || ((uint32_t)str_total_size >= buff_size)) {
        return -1;
    }

#if IS_ENABLED(CONFIG_SOLN_DISP_DBI_EN)
    str_total_size = solution_fps_str_append(str_buff_total, buff_size,
                                             str_total_size, " lcd:%2lu,", stats.dbi_disp);
    if (str_total_size < 0) {
        return -1;
    }
#elif IS_ENABLED(CONFIG_SOLN_DISP_RGB_EN)
    str_total_size = solution_fps_str_append(str_buff_total, buff_size,
                                             str_total_size, " lcd:%2lu,", stats.rgb_disp);
    if (str_total_size < 0) {
        return -1;
    }
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_DVP_EN)
    str_total_size = solution_fps_str_append(str_buff_total, buff_size,
                                             str_total_size, " dvp:%2lu,", stats.dvp);
    if (str_total_size < 0) {
        return -1;
    }
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_JPEG_ENC_EN)
    str_total_size = solution_fps_str_append(str_buff_total, buff_size,
                                             str_total_size, " jpeg_enc:%2lu,", stats.mjpeg_enc);
    if (str_total_size < 0) {
        return -1;
    }
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_JPEG_DEC_EN)
    str_total_size = solution_fps_str_append(str_buff_total, buff_size,
                                             str_total_size, " jpeg_dec:%2lu,", stats.mjpeg_dec);
    if (str_total_size < 0) {
        return -1;
    }
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_DVP_JPEG_ENC_EN)
    str_total_size = solution_fps_str_append(str_buff_total, buff_size,
                                             str_total_size, " dvp_jpeg_enc:%2lu,", stats.dvp_mjpeg_enc);
    if (str_total_size < 0) {
        return -1;
    }
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_UVC_YUYV_EN) || IS_ENABLED(CONFIG_SOLN_VID_UVC_JPEG_EN)
    str_total_size = solution_fps_str_append(str_buff_total, buff_size,
                                             str_total_size, " uvc:%2lu,", stats.uvc);
    if (str_total_size < 0) {
        return -1;
    }
#endif

#if IS_ENABLED(CONFIG_SOLN_SD_AVI_VIDEO_EN)
    str_total_size = solution_fps_str_append(str_buff_total, buff_size,
                                             str_total_size, " avi_sd:%2lu,", stats.avi_sd);
    if (str_total_size < 0) {
        return -1;
    }
#endif

#if IS_ENABLED(CONFIG_SOLN_HB_TX_EN)
    str_total_size = solution_fps_str_append(str_buff_total, buff_size,
                                             str_total_size, " hb_tx:%2lu,", stats.hb_tx);
    if (str_total_size < 0) {
        return -1;
    }
#endif

#if IS_ENABLED(CONFIG_SOLN_HB_RX_EN)
    str_total_size = solution_fps_str_append(str_buff_total, buff_size,
                                             str_total_size, " hb_rx:%2lu,", stats.hb_rx);
    if (str_total_size < 0) {
        return -1;
    }
#endif

    if (str_buff_total[str_total_size - 1] == ',') {
        str_buff_total[str_total_size - 1] = '\0';
        str_total_size--;
    }

    return str_total_size;
}

static void fps_statistics_task(void *pvParameters)
{
    TickType_t xLastWakeTime;
    uint64_t previous_time_ms;
    solution_frame_count_snapshot_t previous_frame_count;
#if IS_ENABLED(CONFIG_SOLN_FPS_PRINT_EN)
    uint32_t samples_since_print = 0;
#endif
#if (configGENERATE_RUN_TIME_STATS)
    uint64_t runtime_stats_elapsed_ms = 0;
#endif

    (void)pvParameters;
    xLastWakeTime = xTaskGetTickCount();
    previous_time_ms = bflb_mtimer_get_time_ms();
    previous_frame_count = solution_frame_count_snapshot_get();

    while (1) {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(CONFIG_SOLN_FPS_STAT_INTERVAL_MS));

        uint64_t now_ms = bflb_mtimer_get_time_ms();
        uint64_t elapsed_ms = now_ms - previous_time_ms;
        solution_frame_count_snapshot_t current_frame_count = solution_frame_count_snapshot_get();
        solution_fps_stats_t stats = { 0 };

        stats.sample_elapsed_ms = (elapsed_ms > UINT32_MAX) ? UINT32_MAX : (uint32_t)elapsed_ms;

#if IS_ENABLED(CONFIG_SOLN_DISP_DBI_EN)
        stats.dbi_disp = solution_fps_calculate(current_frame_count.dbi_disp,
                                                previous_frame_count.dbi_disp, elapsed_ms);
#endif

#if IS_ENABLED(CONFIG_SOLN_DISP_RGB_EN)
        stats.rgb_disp = solution_fps_calculate(current_frame_count.rgb_disp,
                                                previous_frame_count.rgb_disp, elapsed_ms);
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_DVP_EN)
        stats.dvp = solution_fps_calculate(current_frame_count.dvp,
                                           previous_frame_count.dvp, elapsed_ms);
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_JPEG_ENC_EN)
        stats.mjpeg_enc = solution_fps_calculate(current_frame_count.mjpeg_enc,
                                                 previous_frame_count.mjpeg_enc, elapsed_ms);
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_JPEG_DEC_EN)
        stats.mjpeg_dec = solution_fps_calculate(current_frame_count.mjpeg_dec,
                                                 previous_frame_count.mjpeg_dec, elapsed_ms);
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_DVP_JPEG_ENC_EN)
        stats.dvp_mjpeg_enc = solution_fps_calculate(current_frame_count.dvp_mjpeg_enc,
                                                     previous_frame_count.dvp_mjpeg_enc, elapsed_ms);
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_UVC_YUYV_EN) || IS_ENABLED(CONFIG_SOLN_VID_UVC_JPEG_EN)
        stats.uvc = solution_fps_calculate(current_frame_count.uvc,
                                           previous_frame_count.uvc, elapsed_ms);
#endif

#if IS_ENABLED(CONFIG_SOLN_SD_AVI_VIDEO_EN)
        stats.avi_sd = solution_fps_calculate(current_frame_count.avi_sd,
                                              previous_frame_count.avi_sd, elapsed_ms);
#endif

#if IS_ENABLED(CONFIG_SOLN_HB_TX_EN)
        stats.hb_tx = solution_fps_calculate(current_frame_count.hb_tx,
                                             previous_frame_count.hb_tx, elapsed_ms);
#endif

#if IS_ENABLED(CONFIG_SOLN_HB_RX_EN)
        stats.hb_rx = solution_fps_calculate(current_frame_count.hb_rx,
                                             previous_frame_count.hb_rx, elapsed_ms);
#endif

        solution_fps_stats_publish(&stats);
        previous_frame_count = current_frame_count;
        previous_time_ms = now_ms;

#if IS_ENABLED(CONFIG_SOLN_FPS_PRINT_EN)
        samples_since_print++;
        if (samples_since_print >= CONFIG_SOLN_FPS_PRINT_INTERVAL_MULTIPLIER) {
            char str_buff_total[128];
            int ret = soln_fps_str_get(str_buff_total, sizeof(str_buff_total));
            if (ret <= 0) {
                LOG_E("get fps str failed\r\n");
            } else {
                LOG_I("%s\r\n", str_buff_total);
            }
            samples_since_print = 0;
        }
#endif

#if (configGENERATE_RUN_TIME_STATS)
        /* print task run time stats every 10 seconds */
        runtime_stats_elapsed_ms += elapsed_ms;
        if (runtime_stats_elapsed_ms >= 10000U) {
            static char info_buffer[1024];
            vTaskGetRunTimeStats(info_buffer);
            puts("\r\nTask Name\t Run Time\t CPU Load\r\n");
            puts(info_buffer);
            puts("\r\n");
            runtime_stats_elapsed_ms %= 10000U;
        }
#endif
    }
}

void soln_init(void)
{
#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_RAW_LOCAL_EN) || IS_ENABLED(CONFIG_SOLN_FBQ_VID_RAW_REMOTE_EN) ||   \
    IS_ENABLED(CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_EN) || IS_ENABLED(CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_EN) || \
    IS_ENABLED(CONFIG_SOLN_FBQ_AUD_PCM_REMOTE_EN) || IS_ENABLED(CONFIG_SOLN_FBQ_AUD_PCM_LOCAL_EN)
    if (soln_fbq_init_all() != FBQ_OK) {
        LOG_E("FBQ initialization failed\r\n");
        return;
    }
#endif

    /****************** solution audio ******************/

#if IS_ENABLED(CONFIG_SOLN_AUD_AUADC_EN)
    soln_aud_record_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLN_AUD_AUDAC_EN)
    soln_aud_play_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLN_AUD_I2S_IN_EN) || IS_ENABLED(CONFIG_SOLN_AUD_I2S_OUT_EN)
    soln_aud_external_codec_i2s_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLN_AUD_LOOPBACK_EN)
    soln_aud_loopback_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLN_AUD_ES8388_EN)
    soln_aud_codec_es8388_cfg();
#endif

    /****************** solution video ******************/

#if IS_ENABLED(CONFIG_SOLN_DISP_DBI_EN)
    soln_dbi_disp_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLN_DISP_RGB_EN)
    soln_rgb_disp_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_JPEG_ENC_EN)
    soln_mjpeg_enc_task_init(CONFIG_SOLN_VID_DEFAULT_WIDTH, CONFIG_SOLN_VID_DEFAULT_HEIGHT, 50);
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_JPEG_DEC_EN)
    soln_mjpeg_dec_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_DVP_EN)
    soln_dvp_cam_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLN_VID_DVP_JPEG_ENC_EN)
    soln_dvp_mjpeg_enc_task_init(50);
#endif

#if IS_ENABLED(CONFIG_SOLN_SD_AVI_AUDIO_EN) || IS_ENABLED(CONFIG_SOLN_SD_AVI_VIDEO_EN)
    LOG_I("Starting sdcard task init...\r\n");
    soln_save_avi_to_sdcard_init();
#endif

    /************** image transmission *************** */

#if IS_ENABLED(CONFIG_SOLN_HB_TX_EN)
    /* Default startup of the sending end (server) */
    // soln_hb_sender_init(0);
#endif

#if IS_ENABLED(CONFIG_SOLN_HB_RX_EN)
    /* From the application initiating the receiving end */
    // soln_hb_recv_init(0, 0U, 0U, 0U, 0U, 0);
    // soln_hb_recv_start();
#endif

    xTaskCreate(fps_statistics_task, (char *)"fps_stats", 512, NULL, 2, NULL);

    LOG_D("solution init done\r\n");
}
