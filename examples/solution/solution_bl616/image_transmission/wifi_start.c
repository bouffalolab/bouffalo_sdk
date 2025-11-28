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
#include "fhost_api.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"

#include "rfparam_adapter.h"

#include "shell.h"

#define DBG_TAG "WIFI"
#include "log.h"

/********* solution video *********/
#if (CONFIG_SOLUTION_FUNC_HIBOOSTER)
#include <mjpeg_booster.h>
#include <mjpeg_sw_buffer.h>
static void cmd_mjpeg_start(void);
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Functions
 ****************************************************************************/
void wifi_event_handler(uint32_t code)
{
    switch (code) {
        case CODE_WIFI_ON_INIT_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_INIT_DONE\r\n", __func__);
            wifi_mgmr_task_start();
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

#define mainSTART_AP_TASK_PRIORITY            (15)

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

void wifi_start_firmware_task(void *param)
{
    /* network init */
    tcpip_init(NULL, NULL);

    LOG_I("Starting wifi ...\r\n");

    if (0 != rfparam_init(0, NULL, 0)) {
        LOG_I("PHY RF init failed!\r\n");
        return 0;
    }
    LOG_I("PHY RF init success!\r\n");

    wifi_task_create();

    LOG_I("Starting fhost ...\r\n");
    fhost_init();

    vTaskDelete(NULL);
}
