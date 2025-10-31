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

#include "bl616d_glb.h"

//#include "rfparam_adapter.h"
#include "wl_api.h"

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
static wifi_conf_t conf = {
    .country_code = "CN",
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Functions
 ****************************************************************************/
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

static ATTR_NOCACHE_RAM_SECTION uint32_t phyrf_ram[2048];

void wireless_config_init(struct wl_param_t *hw_param)
{
}

void wifi_start_firmware_task(void *param)
{
    /* network init */
    tcpip_init(NULL, NULL);

    LOG_I("Starting wifi ...\r\n");

    /* enable wifi clock */
#if 1
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_IP_WIFI_PHY | GLB_AHB_CLOCK_IP_WIFI_MAC_PHY | GLB_AHB_CLOCK_IP_WIFI_PLATFORM);
    //GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_WIFI);

    /* Enable wifi irq */
    extern void interrupt0_handler(void);
    bflb_irq_attach(WIFI_TO_CPU_IRQn, (irq_callback)interrupt0_handler, NULL);
    bflb_irq_enable(WIFI_TO_CPU_IRQn);
#endif

    wl_cfg = wl_cfg_get((uint8_t *)phyrf_ram);
    wl_cfg->capcode_get = NULL; // rfparam_get_capcode;
    wl_cfg->capcode_set = NULL; // rfparam_set_capcode;
    wl_cfg->param_load = NULL;  // rfparam_load;
    wl_cfg->en_full_cal = 1;
    wl_cfg->en_param_load = 0;
    wl_cfg->mode = WL_API_MODE_ALL;
    wl_cfg->log_level = 0;
    wl_cfg->log_printf = NULL;
    wl_cfg->param.xtalfreq_hz = 40 * 1000 * 1000; //xtal_value;
    for (int i = 0; i < sizeof(wl_cfg->param.pwrtarget.pwr_11b) / sizeof(wl_cfg->param.pwrtarget.pwr_11b[0]); i++) {
        wl_cfg->param.pwrtarget.pwr_11b[i] = 10;
    }
    for (int i = 0; i < sizeof(wl_cfg->param.pwrtarget.pwr_11g) / sizeof(wl_cfg->param.pwrtarget.pwr_11g[0]); i++) {
        wl_cfg->param.pwrtarget.pwr_11g[i] = 10;
    }
    for (int i = 0; i < sizeof(wl_cfg->param.pwrtarget.pwr_11n_ht20) / sizeof(wl_cfg->param.pwrtarget.pwr_11n_ht20[0]); i++) {
        wl_cfg->param.pwrtarget.pwr_11n_ht20[i] = 10;
    }
    for (int i = 0; i < sizeof(wl_cfg->param.pwrtarget.pwr_11n_ht40) / sizeof(wl_cfg->param.pwrtarget.pwr_11n_ht40[0]); i++) {
        wl_cfg->param.pwrtarget.pwr_11n_ht40[i] = 10;
    }
    for (int i = 0; i < sizeof(wl_cfg->param.pwrtarget.pwr_11ac_vht20) / sizeof(wl_cfg->param.pwrtarget.pwr_11ac_vht20[0]); i++) {
        wl_cfg->param.pwrtarget.pwr_11ac_vht20[i] = 10;
    }
    for (int i = 0; i < sizeof(wl_cfg->param.pwrtarget.pwr_11ac_vht40) / sizeof(wl_cfg->param.pwrtarget.pwr_11ac_vht40[0]); i++) {
        wl_cfg->param.pwrtarget.pwr_11ac_vht40[i] = 10;
    }
    for (int i = 0; i < sizeof(wl_cfg->param.pwrtarget.pwr_11ac_vht80) / sizeof(wl_cfg->param.pwrtarget.pwr_11ac_vht80[0]); i++) {
        wl_cfg->param.pwrtarget.pwr_11ac_vht80[i] = 10;
    }

    for (int i = 0; i < sizeof(wl_cfg->param.pwrtarget.pwr_11ax_he20) / sizeof(wl_cfg->param.pwrtarget.pwr_11ax_he20[0]); i++) {
        wl_cfg->param.pwrtarget.pwr_11ax_he20[i] = 10;
    }
    for (int i = 0; i < sizeof(wl_cfg->param.pwrtarget.pwr_11ax_he40) / sizeof(wl_cfg->param.pwrtarget.pwr_11ax_he40[0]); i++) {
        wl_cfg->param.pwrtarget.pwr_11ax_he40[i] = 10;
    }
    for (int i = 0; i < sizeof(wl_cfg->param.pwrtarget.pwr_11ax_he80) / sizeof(wl_cfg->param.pwrtarget.pwr_11ax_he80[0]); i++) {
        wl_cfg->param.pwrtarget.pwr_11ax_he80[i] = 10;
    }

    wl_init();

    LOG_I("PHY RF init success!\r\n");

    wifi_task_create();

    LOG_I("Starting fhost ...\r\n");
    fhost_init();

    vTaskDelete(NULL);
}
