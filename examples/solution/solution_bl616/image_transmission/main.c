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

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include "bl_fw_api.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"

#include "bflb_irq.h"
#include "bflb_uart.h"

#include "bl616_glb.h"
#include "rfparam_adapter.h"

#include "bl_sys.h"
#include "board.h"
#include "shell.h"

#define DBG_TAG "MAIN"
#include "log.h"

#ifdef CONFIG_CHERRYUSB
#include "usbh_core.h"
#endif

#if (CONFIG_SOLUTION_QUEUE_YUYV || CONFIG_SOLUTION_QUEUE_MJPEG || CONFIG_SOLUTION_QUEUE_AUDIO_IN || CONFIG_SOLUTION_QUEUE_AUDIO_OUT)
#include "frame_queue.h"
#endif

/********* solution audio *********/
#if (CONFIG_SOLUTION_FUNC_AUDIO_AUADC)
#include "auadc.h"
#endif

#if (CONFIG_SOLUTION_FUNC_AUDIO_AUDAC)
#include "audac.h"
#endif

#if (CONFIG_SOLUTION_FUNC_AUDIO_I2S_IN || CONFIG_SOLUTION_FUNC_AUDIO_I2S_OUT)
#include "external_codec_i2s.h"
#endif

#if (CONFIG_SOLUTION_FUNC_UAC_IN || CONFIG_SOLUTION_FUNC_UAC_OUT)
#include "usbh_uvc_uac.h"
#endif

#if (CONFIG_SOLUTION_FUNC_AUDIO_LOOPBACK)
#include "audio_loopback.h"
#endif

#if (CONFIG_SOLUTION_FUNC_AUDIO_ES8388)
#include "codec_es8388_cfg.h"
#endif

/********* solution video *********/
#if (CONFIG_SOLUTION_FUNC_DBI_DISP)
#include "dbi_disp.h"
uint32_t g_dbi_disp_fps = 0;
#endif

#if (CONFIG_SOLUTION_FUNC_MJPEG)
#include "mjpeg.h"
uint32_t g_mjpeg_fps = 0;
#endif

#if (CONFIG_SOLUTION_FUNC_DVP)
#include "dvp.h"
uint32_t g_dvp_fps = 0;
#endif

#if (CONFIG_SOLUTION_FUNC_DVP_JPEG)
#include "dvp_mjpeg.h"
uint32_t g_dvp_mjpeg_fps = 0;
#endif

#if (CONFIG_SOLUTION_FUNC_UVC_YUYV || CONFIG_SOLUTION_FUNC_UVC_JPEG)
#include "usbh_uvc_uac.h"
uint32_t g_uvc_fps = 0;
#endif

#if (CONFIG_SOLUTION_FUNC_SDCARD_AVI_AUDS || CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS)
#include "jpeg_sd.h"
#include "avi_jpeg_sd.h"
#endif

#if (CONFIG_SOLUTION_FUNC_HIBOOSTER)
#include <mjpeg_booster.h>
#include <mjpeg_sw_buffer.h>
static void cmd_mjpeg_start(void);
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define WIFI_STACK_SIZE  (1536)
#define TASK_PRIORITY_FW (16)

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct bflb_device_s *uart0;

static TaskHandle_t wifi_fw_task;

static wifi_conf_t conf = {
    .country_code = "CN",
};

extern void shell_init_with_task(struct bflb_device_s *shell);

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Functions
 ****************************************************************************/

int wifi_start_firmware_task(void)
{
    LOG_I("Starting wifi ...\r\n");

    /* enable wifi clock */

    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_IP_WIFI_PHY | GLB_AHB_CLOCK_IP_WIFI_MAC_PHY | GLB_AHB_CLOCK_IP_WIFI_PLATFORM);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_WIFI);

    if (0 != rfparam_init(0, NULL, 0)) {
        LOG_I("PHY RF init failed!\r\n");
        return 0;
    }

    LOG_I("PHY RF init success!\r\n");

    /* Enable wifi irq */

    extern void interrupt0_handler(void);
    bflb_irq_attach(WIFI_IRQn, (irq_callback)interrupt0_handler, NULL);
    bflb_irq_enable(WIFI_IRQn);

    xTaskCreate(wifi_main, (char *)"fw", WIFI_STACK_SIZE, NULL, TASK_PRIORITY_FW, &wifi_fw_task);

    return 0;
}

void wifi_event_handler(uint32_t code)
{
    switch (code) {
        case CODE_WIFI_ON_INIT_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_INIT_DONE\r\n", __func__);
            wifi_mgmr_init(&conf);
        } break;
        case CODE_WIFI_ON_MGMR_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_MGMR_DONE\r\n", __func__);
        } break;
        case CODE_WIFI_ON_SCAN_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_SCAN_DONE\r\n", __func__);
            wifi_mgmr_sta_scanlist();
        } break;
        case CODE_WIFI_ON_CONNECTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_CONNECTED\r\n", __func__);
            void mm_sec_keydump();
            mm_sec_keydump();
        } break;
        case CODE_WIFI_ON_GOT_IP: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_GOT_IP\r\n", __func__);
#if (CONFIG_SOLUTION_FUNC_HIBOOSTER)
            cmd_mjpeg_start();
#endif
        } break;
        case CODE_WIFI_ON_DISCONNECT: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_DISCONNECT\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STARTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STARTED\r\n", __func__);
#if (CONFIG_SOLUTION_FUNC_HIBOOSTER)
            cmd_mjpeg_start();
#endif
        } break;
        case CODE_WIFI_ON_AP_STOPPED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STOPPED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STA_ADD: {
            LOG_I("[APP] [EVT] [AP] [ADD] %lld\r\n", xTaskGetTickCount());
        } break;
        case CODE_WIFI_ON_AP_STA_DEL: {
            LOG_I("[APP] [EVT] [AP] [DEL] %lld\r\n", xTaskGetTickCount());
        } break;
        default: {
            LOG_I("[APP] [EVT] Unknown code %u \r\n", code);
        }
    }
}

#if CONFIG_SOLUTION_FUNC_HIBOOSTER

// MJPEG configs
#define CONF_MJPEG_Q_MAX                      50
#define CONF_MJPEG_Q_MIN                      10
#define CONF_MJPEG_Q_UPGRADE_SIZE_THRESHOLD   (15 * 1024)
#define CONF_MJPEG_Q_DOWNGRADE_SIZE_THRESHOLD (55 * 1024)

#define mainSTART_AP_TASK_PRIORITY            (10)

static void configStartSenderTask(void *_)
{
    int ret;
    ret = mjpeg_sender_config(8800, 1234);
    if (ret < 0)
        printf("mjpeg_sender_config error\r\n");
    ret = mjpeg_sender_start();
    if (ret < 0)
        printf("mjpeg_sender_start error\r\n");
    mjpeg_sw_fifo_config(CONF_MJPEG_Q_MAX, CONF_MJPEG_Q_MIN,
#if defined FLOW_MODE_ENABLE
                         CONF_MJPEG_Q_UPGRADE_SIZE_THRESHOLD, CONF_MJPEG_Q_DOWNGRADE_SIZE_THRESHOLD, DEFAULT_ACTIVE_CAM_IDX);
#else
                         CONF_MJPEG_Q_UPGRADE_SIZE_THRESHOLD, CONF_MJPEG_Q_DOWNGRADE_SIZE_THRESHOLD);
#endif
    vTaskDelete(NULL);
}

static int camera_stream_id = 2;

#if defined FLOW_MODE_ENABLE
int g_main_cam_status = CAM_STATUS_NOT_PROBED;
int g_flow_cam_status = CAM_STATUS_NOT_PROBED;
uint16_t g_frame_count_for_abnormality = (uint16_t)-1;

// Process command sent from APP to MCU here
// This function is called in MCU receive thread, do NOT block
void process_app_custom_cmd(uint8_t *data_buf, uint16_t data_buf_len)
{
    // send it back to app
    //printf("[data tun] APP->MCU custom %.*s, and send back\r\n", (int)data_buf_len, data_buf);
    mjpeg_send_mcuctl_msg(data_buf, data_buf_len);
}
#endif

static void cameraTask(void *pvParameters)
{
    uint32_t tick_now, tick_last = 0;

    TickType_t xDelay = pdMS_TO_TICKS(100);

    /*enable_24mhz*/
    //bl_cam_enable_24MRef();
    vTaskDelay(xDelay);
    //enable_camera_module();
    vTaskDelay(xDelay);

    if (mjpeg_sw_fifo_init(640, 480)) {
        printf("[SENDER] mjpeg_sw_fifo_init failed\r\n");
        while (1) {
            /* empty */;
        }
    }

    /*cam is ready, now start task*/
    xTaskCreate(mjpeg_sender_task, "sender", 1024, (void *)&camera_stream_id, mainSTART_AP_TASK_PRIORITY, NULL);
    xTaskCreate(mjpeg_sender_ctl_task, "senderctl", 1024, NULL, mainSTART_AP_TASK_PRIORITY + 1, NULL);
    xTaskCreate(configStartSenderTask, "foo", 1024, NULL, mainSTART_AP_TASK_PRIORITY - 1, NULL);

    //cpu_utils_usage_init();
    xDelay = pdMS_TO_TICKS(1000);
    tick_last = xTaskGetTickCount();

    while (1) {
        tick_now = xTaskGetTickCount();
        if (((int32_t)tick_now) - ((int32_t)tick_last) > 10 * 1000) {
            tick_last = xTaskGetTickCount();
            /*debug every 30*/
            //printf("[MAIN] FPS is %u\r\n", (*(volatile uint16_t*)0x40016028 - frame_count)/10);
            //frame_count = *(volatile uint16_t*)0x40016028;
        }
        vTaskDelay(pdMS_TO_TICKS(50 / 2));
    }
}

static void cmd_mjpeg_start(void)
{
    printf("-------------------------->>> Starting MJPEG tasks\r\n");
    xTaskCreate(cameraTask, (char *)"camera", 512, NULL, mainSTART_AP_TASK_PRIORITY + 3, NULL);
}

#endif

static void fps_printf_task(void *pvParameters)
{
    TickType_t xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();

    while (1) {
        for (uint32_t i = 0; i < 5; i++) {
            printf("FPS ");

#if (CONFIG_SOLUTION_FUNC_DBI_DISP)
            static uint32_t dbi_disp_cnt_old = 0;
            g_dbi_disp_fps = g_dbi_disp_total_frame_cnt - dbi_disp_cnt_old;
            dbi_disp_cnt_old = g_dbi_disp_total_frame_cnt;
            printf("lcd: %2d, ", g_dbi_disp_fps);
#endif

#if (CONFIG_SOLUTION_FUNC_DVP)
            static uint32_t dvp_cnt_old = 0;
            g_dvp_fps = g_dvp_total_frame_cnt - dvp_cnt_old;
            dvp_cnt_old = g_dvp_total_frame_cnt;
            printf("dvp: %2d, ", g_dvp_fps);
#endif

#if (CONFIG_SOLUTION_FUNC_MJPEG)
            static uint32_t mjpeg_cnt_old = 0;
            g_mjpeg_fps = g_mjpeg_total_frame_cnt - mjpeg_cnt_old;
            mjpeg_cnt_old = g_mjpeg_total_frame_cnt;
            printf("mjpeg: %2d, ", g_mjpeg_fps);
#endif

#if (CONFIG_SOLUTION_FUNC_DVP_JPEG)
            static uint32_t dvp_mjpeg_cnt_old = 0;
            g_dvp_mjpeg_fps = g_dvp_mjpeg_total_frame_cnt - dvp_mjpeg_cnt_old;
            dvp_mjpeg_cnt_old = g_dvp_mjpeg_total_frame_cnt;
            printf("dvp_mjpeg: %2d, ", g_dvp_mjpeg_fps);
#endif

#if (CONFIG_SOLUTION_FUNC_UVC_YUYV || CONFIG_SOLUTION_FUNC_UVC_JPEG)
            static uint32_t uvc_cnt_old = 0;
            g_uvc_fps = g_uvc_total_frame_cnt - uvc_cnt_old;
            uvc_cnt_old = g_uvc_total_frame_cnt;
            printf("uvc: %2d", g_uvc_fps);
#endif

            printf("\r\n");
            vTaskDelayUntil(&xLastWakeTime, 1000);
        }
#if (configGENERATE_RUN_TIME_STATS)
        static char info_buffer[1024];
        vTaskGetRunTimeStats(info_buffer);
        printf("\r\n<taskname>   <runtime>   <cpu load>\r\n%s\r\n", info_buffer);
#endif
    }
}

int main(void)
{
    /* board init */
    bl_cpu_sysmap_init(true, true);
    board_init();

    /* enable DVK board 5V_VDD */
    board_5v_enable();

    /* shell init */
    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);
    printf("\r\nShell ready\r\n");

    /* network init */
    tcpip_init(NULL, NULL);
    /* wifi init */
    wifi_start_firmware_task();

#ifdef CONFIG_CHERRYUSB
    /* USB config and init */
    printf("Starting usb host task...\r\n");
    usbh_initialize();
#endif

#if (CONFIG_SOLUTION_QUEUE_YUYV || CONFIG_SOLUTION_QUEUE_MJPEG || CONFIG_SOLUTION_QUEUE_AUDIO_IN || CONFIG_SOLUTION_QUEUE_AUDIO_OUT)
    /* all queue init */
    frame_ctrl_init_all();
#endif

    /****************** solution audio ******************/

#if (CONFIG_SOLUTION_FUNC_AUDIO_AUADC)
    // /* auadc rate_convert init */
    // audio_record_rate_convert_task_init();
    /* auadc init */
    audio_record_task_init();
#endif

#if (CONFIG_SOLUTION_FUNC_AUDIO_AUDAC)
    /* audac rate_convert init */
    // audio_play_rate_convert_task_init();
    /* audac init */
    audio_play_task_init();
#endif

#if (CONFIG_SOLUTION_FUNC_AUDIO_I2S_IN || CONFIG_SOLUTION_FUNC_AUDIO_I2S_OUT)
    /* i2s init */
    external_codec_i2s_task_init();
#endif

#if (CONFIG_SOLUTION_FUNC_AUDIO_LOOPBACK)
    /* audio loopback */
    audio_loopback_task_init();
#endif

#if (CONFIG_SOLUTION_FUNC_AUDIO_ES8388)
    /* es8388 init */
    codec_es8388_cfg();
#endif

    /****************** solution video ******************/

#if (CONFIG_SOLUTION_FUNC_DBI_DISP)
    /* dbi dislpay init */
    dbi_disp_task_init();
#endif

#if (CONFIG_SOLUTION_FUNC_MJPEG)
    /* mjpeg init */
    mjpeg_compress_task_init(CONFIG_SOLUTION_VIDEO_DEFAULT_WIDTH, CONFIG_SOLUTION_VIDEO_DEFAULT_HIGHT, 50);
#endif

#if (CONFIG_SOLUTION_FUNC_DVP)
    /* dvp cam init */
    dvp_cam_task_init();
#endif

#if (CONFIG_SOLUTION_FUNC_DVP_JPEG)
    dvp_mjpeg_task_init(50);
#endif

#if (CONFIG_SOLUTION_FUNC_SDCARD_AVI_AUDS || CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS)
    printf("Starting sdcard task init...\r\n");
    /* save to sdcard */
    // save_jpeg_to_sdcard_init();
    save_avi_to_sdcard_init();
#endif

    /* info printf task */
    xTaskCreate(fps_printf_task, (char *)"fps_printf", 1024, NULL, 2, NULL);

    /* freeRTOS start */
    vTaskStartScheduler();

    while (1) {
    }
}
