#include "shell.h"
#include <FreeRTOS.h>
#include "task.h"
#include "board.h"

#include "rfparam_adapter.h"

#include "bflb_mtd.h"
#include "easyflash.h"

#include <lwip/tcpip.h>

#include "wifi_mgmr_ext.h"
#ifndef BL602
#include "fhost_api.h"
#include "wifi_mgmr.h"
#endif
#include "mm.h"

#define DBG_TAG "MAIN"
#include "log.h"
#include "async_event.h"

static struct bflb_device_s *uart0;

#define WIFI_STACK_SIZE  (1536)
#define TASK_PRIORITY_FW (16)

extern void shell_init_with_task(struct bflb_device_s *shell);
extern void wifi_event_handler(async_input_event_t ev, void *priv);

#ifdef BL602
extern void wifi_task_create(void);
extern int wifi_mgmr_task_start(void);
#endif

#if defined(CONFIG_BLUETOOTH)
extern int btble_cli_start(void);
#endif

void wifi_start_firmware_task(void *param)
{
    LOG_I("Starting wifi ...\r\n");

    async_register_event_filter(EV_WIFI, wifi_event_handler, NULL);


    wifi_task_create();

    #ifndef BL602
    LOG_I("Starting fhost ...\r\n");
    fhost_init();
    #endif

    vTaskDelete(NULL);
}

void wifi_event_handler(async_input_event_t ev, void *priv)
{
    uint32_t code = ev->code;

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
            LOG_I("[SYS] Memory left is %d Bytes\r\n", kfree_size(0));
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
            LOG_I("[APP] [EVT] [AP] [ADD] %lld\r\n", xTaskGetTickCount());
        } break;
        case CODE_WIFI_ON_AP_STA_DEL: {
            LOG_I("[APP] [EVT] [AP] [DEL] %lld\r\n", xTaskGetTickCount());
        } break;
        case CODE_WIFI_ON_AP_CSA_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_CSA_DONE, channel %u\r\n",
                  __func__, (unsigned int)ev->value);
        } break;
        case CODE_WIFI_ON_STA_CSA_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_STA_CSA_DONE, channel %u\r\n",
                  __func__, (unsigned int)ev->value);
        } break;
        default: {
            LOG_I("[APP] [EVT] Unknown code %u \r\n", code);
        }
    }
}

int main(void)
{
    board_init();

    configASSERT((configMAX_PRIORITIES > 4));

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    bflb_mtd_init();
    /* ble stack need easyflash kv */
    easyflash_init();

    /* Init rf */
    if (0 != rfparam_init(0, NULL, 0)) {
        printf("PHY RF init failed!\r\n");
        return 0;
    }

    tcpip_init(NULL, NULL);
    xTaskCreate(wifi_start_firmware_task, "wifi init", 1024, NULL, 10, NULL);
#if defined(CONFIG_BLUETOOTH)
    btble_cli_start();
#endif

    vTaskStartScheduler();

    while (1) {
    }
}
