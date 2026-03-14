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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include <queue.h>
#include <semphr.h>

#include "bflb_irq.h"
#include "bflb_uart.h"

#include "bl_sys.h"
#include "board.h"
#include "shell.h"

#define DBG_TAG "MAIN"
#include "log.h"

#if IS_ENABLED(CONFIG_CHERRYUSB_HOST)
#include "usbh_core.h"
#endif

#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_YUYV) || IS_ENABLED(CONFIG_SOLUTION_QUEUE_MJPEG) || \
    IS_ENABLED(CONFIG_SOLUTION_QUEUE_AUDIO_IN) || IS_ENABLED(CONFIG_SOLUTION_QUEUE_AUDIO_OUT)
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
#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DBI_DISP)
#include "dbi_disp.h"
uint32_t g_dbi_disp_fps = 0;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_MJPEG)
#include "mjpeg.h"
uint32_t g_mjpeg_fps = 0;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DVP)
#include "dvp.h"
uint32_t g_dvp_fps = 0;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DVP_JPEG)
#include "dvp_mjpeg.h"
uint32_t g_dvp_mjpeg_fps = 0;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_UVC_YUYV) || IS_ENABLED(CONFIG_SOLUTION_FUNC_UVC_JPEG)
uint32_t g_uvc_fps = 0;
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_SDCARD_AVI_AUDS) || IS_ENABLED(CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS)
#include "jpeg_sd.h"
#include "avi_jpeg_sd.h"
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_HIBOOSTER_NEW)
uint32_t g_hibooster_fps = 0;
#include "hb_start.h"
#endif

extern void shell_init_with_task(struct bflb_device_s *shell);
extern void wifi_start_firmware_task(void *param);

static struct bflb_device_s *uart0;

static void fps_printf_task(void *pvParameters)
{
    TickType_t xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();

    while (1) {
        for (uint32_t i = 0; i < 5; i++) {
            printf("FPS ");

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DBI_DISP)
            static uint32_t dbi_disp_cnt_old = 0;
            g_dbi_disp_fps = g_dbi_disp_total_frame_cnt - dbi_disp_cnt_old;
            dbi_disp_cnt_old = g_dbi_disp_total_frame_cnt;
            printf("lcd: %2d, ", g_dbi_disp_fps);
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DVP)
            static uint32_t dvp_cnt_old = 0;
            g_dvp_fps = g_dvp_total_frame_cnt - dvp_cnt_old;
            dvp_cnt_old = g_dvp_total_frame_cnt;
            printf("dvp: %2d, ", g_dvp_fps);
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_MJPEG)
            static uint32_t mjpeg_cnt_old = 0;
            g_mjpeg_fps = g_mjpeg_total_frame_cnt - mjpeg_cnt_old;
            mjpeg_cnt_old = g_mjpeg_total_frame_cnt;
            printf("mjpeg: %2d, ", g_mjpeg_fps);
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DVP_JPEG)
            static uint32_t dvp_mjpeg_cnt_old = 0;
            g_dvp_mjpeg_fps = g_dvp_mjpeg_total_frame_cnt - dvp_mjpeg_cnt_old;
            dvp_mjpeg_cnt_old = g_dvp_mjpeg_total_frame_cnt;
            printf("dvp_mjpeg: %2d, ", g_dvp_mjpeg_fps);
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_UVC_YUYV) || IS_ENABLED(CONFIG_SOLUTION_FUNC_UVC_JPEG)
            extern volatile uint32_t g_uvc_total_frame_cnt;
            static uint32_t uvc_cnt_old = 0;
            g_uvc_fps = g_uvc_total_frame_cnt - uvc_cnt_old;
            uvc_cnt_old = g_uvc_total_frame_cnt;
            printf("uvc: %2d, ", g_uvc_fps);
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_HIBOOSTER_NEW)
            static uint32_t hibooster_cnt_old = 0;
            g_hibooster_fps = g_hibooster_total_frame_cnt - hibooster_cnt_old;
            hibooster_cnt_old = g_hibooster_total_frame_cnt;
            printf("hb: %2d", g_hibooster_fps);
#endif

            printf("\r\n");
            vTaskDelayUntil(&xLastWakeTime, 1000);
        }
#if (configGENERATE_RUN_TIME_STATS)
        static char info_buffer[1024];
        vTaskGetRunTimeStats(info_buffer);
        puts("\r\nTask Name\t Run Time\t CPU Load\r\n");
        puts(info_buffer);
        puts("\r\n");
#endif
    }
}

void solution_init(void *param)
{
    (void)param;

    /* shell init */
    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);
    printf("\r\nShell ready\r\n");

#if IS_ENABLED(CONFIG_WIFI6)
    /* wifi init */
    xTaskCreate(wifi_start_firmware_task, "wifi init", 1024, NULL, 10, NULL);
    vTaskDelay(200); /* Wait for network ready */
#endif

#if IS_ENABLED(CONFIG_CHERRYUSB_HOST)
    /* USB host init */
    printf("Starting usb host task...\r\n");
    struct bflb_device_s *usb_dev = bflb_device_get_by_name("usb_v2");
    usbh_initialize(0, usb_dev->reg_base);
#endif

#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_YUYV) || IS_ENABLED(CONFIG_SOLUTION_QUEUE_MJPEG) || \
    IS_ENABLED(CONFIG_SOLUTION_QUEUE_AUDIO_IN) || IS_ENABLED(CONFIG_SOLUTION_QUEUE_AUDIO_OUT)
    /* all queue init */
    frame_ctrl_init_all();
#endif

    /****************** solution audio ******************/

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_AUDIO_AUADC)
    // /* auadc rate_convert init */
    // audio_record_rate_convert_task_init();
    /* auadc init */
    audio_record_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_AUDIO_AUDAC)
    /* audac rate_convert init */
    // audio_play_rate_convert_task_init();
    /* audac init */
    audio_play_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_AUDIO_I2S_IN) || IS_ENABLED(CONFIG_SOLUTION_FUNC_AUDIO_I2S_OUT)
    /* i2s init */
    external_codec_i2s_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_AUDIO_LOOPBACK)
    /* audio loopback */
    audio_loopback_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_AUDIO_ES8388)
    /* es8388 init */
    codec_es8388_cfg();
#endif

    /****************** solution video ******************/

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DBI_DISP)
    /* dbi dislpay init */
    dbi_disp_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_MJPEG)
    /* mjpeg init */
    mjpeg_compress_task_init(CONFIG_SOLUTION_VIDEO_DEFAULT_WIDTH, CONFIG_SOLUTION_VIDEO_DEFAULT_HIGHT, 50);
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DVP)
    /* dvp cam init */
    dvp_cam_task_init();
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_DVP_JPEG)
    dvp_mjpeg_task_init(50);
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_SDCARD_AVI_AUDS) || IS_ENABLED(CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS)
    printf("Starting sdcard task init...\r\n");
    /* save to sdcard */
    // save_jpeg_to_sdcard_init();
    save_avi_to_sdcard_init();
#endif

    /************** image transmission *************** */

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_HIBOOSTER_NEW)
    hb_sender_init(8800);
#endif

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_HIBOOSTER)
    void cmd_mjpeg_start(void);
    cmd_mjpeg_start();
#endif

    /* info printf task */
    xTaskCreate(fps_printf_task, (char *)"fps_printf", 512, NULL, 2, NULL);

    LOG_D("solution init done\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    /* board init */
    board_init();

#if IS_ENABLED(BL616) || IS_ENABLED(BL616L)
    /* Improve CPU performance */
    bl_cpu_sysmap_init(true, true);
#endif

    xTaskCreate(solution_init, (char *)"solution_init", 1024, NULL, configMAX_PRIORITIES - 2, NULL);
    vTaskStartScheduler();

    LOG_E("Scheduler start failed!\r\n");
    return -1;
}
