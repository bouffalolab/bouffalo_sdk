#include <stdio.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#include <async_event.h>
#include <bl_fw_api.h>
#include <fhost_api.h>
#include <mm.h>
#include <wifi_mgmr_ext.h>
#include <wifi_mgmr.h>

#ifdef BL616
#include <bl616_glb.h>
#endif

#ifdef CONFIG_ATMODULE
#include <at_main.h>
#endif

#include <app_user.h>

#define DBG_TAG "appwifi"
#include <log.h>

static bool wifi_init_done;

static void wifi_event_handler(async_input_event_t ev, void *priv)
{
    uint32_t code = ev->code;

    switch (code) {
        case CODE_WIFI_ON_INIT_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_INIT_DONE\r\n", __func__);
            wifi_mgmr_task_start();
        } break;
        case CODE_WIFI_ON_MGMR_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_MGMR_DONE\r\n", __func__);
            wifi_init_done = true;
        } break;
        case CODE_WIFI_ON_SCAN_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_SCAN_DONE\r\n", __func__);
        } break;
        case CODE_WIFI_ON_CONNECTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_CONNECTED\r\n", __func__);
        } break;
        #ifdef CODE_WIFI_ON_GOT_IP_ABORT
        case CODE_WIFI_ON_GOT_IP_ABORT: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_GOT_IP_ABORT\r\n", __func__);
        } break;
        #endif
        #ifdef CODE_WIFI_ON_GOT_IP_TIMEOUT
        case CODE_WIFI_ON_GOT_IP_TIMEOUT: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_GOT_IP_TIMEOUT\r\n", __func__);
        } break;
        #endif
        case CODE_WIFI_ON_GOT_IP: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_GOT_IP\r\n", __func__);
            LOG_I("[SYS] Memory left is %d Bytes\r\n", (int)kfree_size(0));
        } break;
        case CODE_WIFI_ON_DISCONNECT: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_DISCONNECT\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STARTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STARTED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STOPPED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STOPPED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STA_ADD: {
            LOG_I("[APP] [EVT] [AP] [ADD] %lld, code1:%u\r\n", xTaskGetTickCount(), priv);
        } break;
        case CODE_WIFI_ON_AP_STA_DEL: {
            LOG_I("[APP] [EVT] [AP] [DEL] %lld, code1:%u\r\n", xTaskGetTickCount(), priv);
        } break;
        default: {
            LOG_I("[APP] [EVT] Unknown code %u \r\n", code);
        }
    }
}

int app_wifi_init(void)
{
    LOG_I("Starting wifi ...\r\n");

#if defined(BL616)
    GLB_Set_EM_Sel(GLB_WRAM160KB_EM0KB);
#endif

    wifi_init_done = false;

    async_register_event_filter(EV_WIFI, wifi_event_handler, NULL);

    wifi_task_create();

    LOG_I("Starting fhost ...\r\n");
    fhost_init();

    while (!wifi_init_done) {
        vTaskDelay(1);
    }

#ifdef CONFIG_ATMODULE
    at_module_init();
#endif

    return 0;
}
