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

#include "solution.h"
#include "FreeRTOS.h"
#include "task.h"

#define DBG_TAG "SOLUTION"
#include "log.h"

#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_IMG_RAW_OUT) || IS_ENABLED(CONFIG_SOLUTION_QUEUE_IMG_RAW_IN) || \
    IS_ENABLED(CONFIG_SOLUTION_QUEUE_MJPEG_OUT) || IS_ENABLED(CONFIG_SOLUTION_QUEUE_MJPEG_IN) ||     \
    IS_ENABLED(CONFIG_SOLUTION_QUEUE_AUDIO_OUT) || IS_ENABLED(CONFIG_SOLUTION_QUEUE_AUDIO_IN)
#include "frame_queue.h"
#endif

/********* solution audio *********/
#if IS_ENABLED(CONFIG_SOLUTION_FUNC_AUDIO_AUADC)
#include "auadc.h"
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_AUDIO_AUDAC)
#include "audac.h"
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_AUDIO_I2S_IN) || IS_ENABLED(CONFIG_SOLUTION_FUNC_AUDIO_I2S_OUT)
#include "external_codec_i2s.h"
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_AUDIO_LOOPBACK)
#include "audio_loopback.h"
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_AUDIO_ES8388)
#include "codec_es8388_cfg.h"
#endif

/********* solution video *********/
#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DISP_DBI)
#include "dbi_disp.h"
uint32_t g_dbi_disp_fps = 0;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DISP_RGB)
#include "rgb_disp.h"
uint32_t g_rgb_disp_fps = 0;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_MJPEG_ENC)
#include "mjpeg_enc.h"
uint32_t g_mjpeg_enc_fps = 0;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_MJPEG_DEC)
#include "mjpeg_dec.h"
uint32_t g_mjpeg_dec_fps = 0;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DVP)
#include "dvp.h"
uint32_t g_dvp_fps = 0;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DVP_JPEG_ENC)
#include "dvp_mjpeg_enc.h"
uint32_t g_dvp_mjpeg_enc_fps = 0;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_UVC_YUYV) || IS_ENABLED(CONFIG_SOLUTION_FUNC_UVC_JPEG)
uint32_t g_uvc_fps = 0;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_SDCARD_AVI_AUDS) || IS_ENABLED(CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS)
#include "jpeg_sd.h"
#include "avi_jpeg_sd.h"
#if IS_ENABLED(CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS)
uint32_t g_avi_sd_fps = 0;
#endif
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_HIBOOSTER_TX)
uint32_t g_hb_tx_fps = 0;
#include "hb_start_tx.h"
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_HIBOOSTER_RX)
uint32_t g_hb_rx_fps = 0;
#include "hb_start_rx.h"
#endif

int solution_fps_str_get(char *str_buff_total, uint32_t buff_size)
{
    __UNUSED int str_size;
    __UNUSED int str_total_size;
    __UNUSED char str_buff[32];

    if (buff_size < 16) {
        return -1;
    }

    str_total_size = snprintf(str_buff_total, buff_size, "FPS:");

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DISP_DBI)
    str_size = snprintf(str_buff, sizeof(str_buff), " lcd:%2d,", g_dbi_disp_fps);
    if (str_size + str_total_size >= buff_size) {
        return -1;
    }
    strcat(str_buff_total, str_buff);
    str_total_size += str_size;
#elif IS_ENABLED(CONFIG_SOLUTION_FUNC_DISP_RGB)
    str_size = snprintf(str_buff, sizeof(str_buff), " lcd:%2d,", g_rgb_disp_fps);
    if (str_size + str_total_size >= buff_size) {
        return -1;
    }
    strcat(str_buff_total, str_buff);
    str_total_size += str_size;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DVP)
    str_size = snprintf(str_buff, sizeof(str_buff), " dvp:%2d,", g_dvp_fps);
    if (str_size + str_total_size >= buff_size) {
        return -1;
    }
    strcat(str_buff_total, str_buff);
    str_total_size += str_size;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_MJPEG_ENC)
    str_size = snprintf(str_buff, sizeof(str_buff), " jpeg_enc:%2d,", g_mjpeg_enc_fps);
    if (str_size + str_total_size >= buff_size) {
        return -1;
    }
    strcat(str_buff_total, str_buff);
    str_total_size += str_size;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_MJPEG_DEC)
    str_size = snprintf(str_buff, sizeof(str_buff), " jpeg_dec:%2d,", g_mjpeg_dec_fps);
    if (str_size + str_total_size >= buff_size) {
        return -1;
    }
    strcat(str_buff_total, str_buff);
    str_total_size += str_size;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DVP_JPEG_ENC)
    str_size = snprintf(str_buff, sizeof(str_buff), " dvp_jpeg_enc:%2d,", g_dvp_mjpeg_enc_fps);
    if (str_size + str_total_size >= buff_size) {
        return -1;
    }
    strcat(str_buff_total, str_buff);
    str_total_size += str_size;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_UVC_YUYV) || IS_ENABLED(CONFIG_SOLUTION_FUNC_UVC_JPEG)
    str_size = snprintf(str_buff, sizeof(str_buff), " uvc:%2d,", g_uvc_fps);
    if (str_size + str_total_size >= buff_size) {
        return -1;
    }
    strcat(str_buff_total, str_buff);
    str_total_size += str_size;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS)
    str_size = snprintf(str_buff, sizeof(str_buff), " avi_sd:%2d,", g_avi_sd_fps);
    if (str_size + str_total_size >= buff_size) {
        return -1;
    }
    strcat(str_buff_total, str_buff);
    str_total_size += str_size;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_HIBOOSTER_TX)
    str_size = snprintf(str_buff, sizeof(str_buff), " hb_tx:%2d,", g_hb_tx_fps);
    if (str_size + str_total_size >= buff_size) {
        return -1;
    }
    strcat(str_buff_total, str_buff);
    str_total_size += str_size;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_HIBOOSTER_RX)
    str_size = snprintf(str_buff, sizeof(str_buff), " hb_rx:%2d", g_hb_rx_fps);
    if (str_size + str_total_size >= buff_size) {
        return -1;
    }
    strcat(str_buff_total, str_buff);
    str_total_size += str_size;
#endif

    if (str_buff_total[str_total_size - 1] == ',') {
        str_buff_total[str_total_size - 1] = '\0';
        str_total_size--;
    }

    return str_total_size;
}

static void fps_printf_task(void *pvParameters)
{
    int ret;
    TickType_t xLastWakeTime;
    __UNUSED uint32_t time_ms, now_ms, diff_ms;

    (void)pvParameters;
    xLastWakeTime = xTaskGetTickCount();

    time_ms = bflb_mtimer_get_time_ms();

    for (uint32_t i = 0;; i++) {
        vTaskDelayUntil(&xLastWakeTime, 1000);

        vTaskSuspendAll();

        now_ms = bflb_mtimer_get_time_ms();
        diff_ms = now_ms - time_ms;
        time_ms = now_ms;

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DISP_DBI)
        static uint32_t dbi_disp_cnt_old = 0;
        g_dbi_disp_fps = (g_dbi_disp_total_frame_cnt - dbi_disp_cnt_old) * 1000 / diff_ms;
        dbi_disp_cnt_old = g_dbi_disp_total_frame_cnt;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DISP_RGB)
        static uint32_t rgb_disp_cnt_old = 0;
        g_rgb_disp_fps = (g_rgb_disp_total_frame_cnt - rgb_disp_cnt_old) * 1000 / diff_ms;
        rgb_disp_cnt_old = g_rgb_disp_total_frame_cnt;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DVP)
        static uint32_t dvp_cnt_old = 0;
        g_dvp_fps = (g_dvp_total_frame_cnt - dvp_cnt_old) * 1000 / diff_ms;
        dvp_cnt_old = g_dvp_total_frame_cnt;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_MJPEG_ENC)
        static uint32_t mjpeg_enc_cnt_old = 0;
        g_mjpeg_enc_fps = (g_mjpeg_enc_total_frame_cnt - mjpeg_enc_cnt_old) * 1000 / diff_ms;
        mjpeg_enc_cnt_old = g_mjpeg_enc_total_frame_cnt;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_MJPEG_DEC)
        static uint32_t mjpeg_dec_cnt_old = 0;
        g_mjpeg_dec_fps = (g_mjpeg_dec_total_frame_cnt - mjpeg_dec_cnt_old) * 1000 / diff_ms;
        mjpeg_dec_cnt_old = g_mjpeg_dec_total_frame_cnt;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DVP_JPEG_ENC)
        static uint32_t dvp_mjpeg_enc_cnt_old = 0;
        g_dvp_mjpeg_enc_fps = (g_dvp_mjpeg_enc_total_frame_cnt - dvp_mjpeg_enc_cnt_old) * 1000 / diff_ms;
        dvp_mjpeg_enc_cnt_old = g_dvp_mjpeg_enc_total_frame_cnt;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_UVC_YUYV) || IS_ENABLED(CONFIG_SOLUTION_FUNC_UVC_JPEG)
        extern volatile uint32_t g_uvc_total_frame_cnt;
        static uint32_t uvc_cnt_old = 0;
        g_uvc_fps = (g_uvc_total_frame_cnt - uvc_cnt_old) * 1000 / diff_ms;
        uvc_cnt_old = g_uvc_total_frame_cnt;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS)
        static uint32_t avi_sd_cnt_old = 0;
        g_avi_sd_fps = (g_avi_sd_total_frame_cnt - avi_sd_cnt_old) * 1000 / diff_ms;
        avi_sd_cnt_old = g_avi_sd_total_frame_cnt;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_HIBOOSTER_TX)
        static uint32_t hb_tx_cnt_old = 0;
        g_hb_tx_fps = (g_hb_tx_total_frame_cnt - hb_tx_cnt_old) * 1000 / diff_ms;
        hb_tx_cnt_old = g_hb_tx_total_frame_cnt;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_HIBOOSTER_RX)
        static uint32_t hb_rx_cnt_old = 0;
        g_hb_rx_fps = (g_hb_rx_total_frame_cnt - hb_rx_cnt_old) * 1000 / diff_ms;
        hb_rx_cnt_old = g_hb_rx_total_frame_cnt;
#endif

        xTaskResumeAll();

        /* print fps every 2 seconds */
        if (i % 2 == 0) {
            char str_buff_total[128];
            ret = solution_fps_str_get(str_buff_total, sizeof(str_buff_total));
            if (ret <= 0) {
                LOG_E("get fps str failed\r\n");
            } else {
                LOG_I("%s\r\n", str_buff_total);
            }
        }

#if (configGENERATE_RUN_TIME_STATS)
        /* print task run time stats every 10 seconds */
        if (i % 10 == 0) {
            static char info_buffer[1024];
            vTaskGetRunTimeStats(info_buffer);
            puts("\r\nTask Name\t Run Time\t CPU Load\r\n");
            puts(info_buffer);
            puts("\r\n");
        }
#endif
    }
}

void solution_init(void)
{
#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_IMG_RAW_OUT) || IS_ENABLED(CONFIG_SOLUTION_QUEUE_IMG_RAW_IN) || \
    IS_ENABLED(CONFIG_SOLUTION_QUEUE_MJPEG_OUT) || IS_ENABLED(CONFIG_SOLUTION_QUEUE_MJPEG_IN) ||     \
    IS_ENABLED(CONFIG_SOLUTION_QUEUE_AUDIO_OUT) || IS_ENABLED(CONFIG_SOLUTION_QUEUE_AUDIO_IN)
    frame_ctrl_init_all();
#endif

    /****************** solution audio ******************/

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_AUDIO_AUADC)
    audio_record_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_AUDIO_AUDAC)
    audio_play_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_AUDIO_I2S_IN) || IS_ENABLED(CONFIG_SOLUTION_FUNC_AUDIO_I2S_OUT)
    external_codec_i2s_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_AUDIO_LOOPBACK)
    audio_loopback_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_AUDIO_ES8388)
    codec_es8388_cfg();
#endif

    /****************** solution video ******************/

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DISP_DBI)
    dbi_disp_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DISP_RGB)
    rgb_disp_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_MJPEG_ENC)
    mjpeg_enc_task_init(CONFIG_SOLUTION_VIDEO_DEFAULT_WIDTH, CONFIG_SOLUTION_VIDEO_DEFAULT_HIGHT, 50);
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_MJPEG_DEC)
    mjpeg_dec_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DVP)
    dvp_cam_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DVP_JPEG_ENC)
    dvp_mjpeg_enc_task_init(50);
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_SDCARD_AVI_AUDS) || IS_ENABLED(CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS)
    printf("Starting sdcard task init...\r\n");
    save_avi_to_sdcard_init();
#endif

    /************** image transmission *************** */

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_HIBOOSTER_TX)
    hb_sender_init(8800);
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_HIBOOSTER_RX)
    hb_recv_init(9000);
#endif

    xTaskCreate(fps_printf_task, (char *)"fps_printf", 512, NULL, 2, NULL);

    LOG_D("solution init done\r\n");
}
