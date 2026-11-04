#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <stdbool.h>

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include <bl_fw_api.h>
#include "fhost_api.h"
#include <wifi_mgmr_ext.h>
#include <wifi_mgmr.h>

#include <bflb_irq.h>
#if defined(BL618DG)
#include <bl618dg_glb.h>
#else
#include <bl616_glb.h>
#endif
// #include <bflb_mtd.h>

#include <rfparam_adapter.h>

#define DBG_TAG "MAIN"
#include <log.h>
#include "async_event.h"
#include "app_apeth_bridge.h"
#ifdef CONFIG_NETIFD_NANO
#include "netifd_nano.h"
#endif
#include "mm.h"

#define WIFI_STACK_SIZE  (2048)
#define TASK_PRIORITY_FW (16)

// extern void app_atmoudle_init(void);
void wifi_event_handler(async_input_event_t ev, void *priv);
#ifdef CONFIG_WIFI_WDS
/* Wait until the Wi-Fi manager is ready before starting the bridge, so WDS is not configured before the firmware is up. */
static volatile bool wifi_mgmr_ready;
#endif

void wifi_start_firmware_task(void *param)
{
    LOG_I("Starting wifi ...\r\n");

    async_register_event_filter(EV_WIFI, wifi_event_handler, NULL);


    wifi_task_create();

    LOG_I("Starting fhost ...\r\n");
    fhost_init();

#ifdef CONFIG_WIFI_WDS
    while (!wifi_mgmr_ready)
        vTaskDelay(pdMS_TO_TICKS(10));
    if (app_emac_init())
        LOG_E("EMAC initialization failed\r\n");
    else
        app_wds_bridge_wifi_ready();
#endif
    vTaskDelete(NULL);
}

void wifi_event_handler(async_input_event_t ev, void *priv)
{
    uint32_t code = ev->code;

    #ifdef BL_HOSTROUTER_ENABLE
    extern void rnm_event_handler(uint32_t code);
    rnm_event_handler(code);
#endif
    switch (code) {
        case CODE_WIFI_ON_INIT_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_INIT_DONE\r\n", __func__);
            wifi_mgmr_task_start();
        } break;
        case CODE_WIFI_ON_MGMR_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_MGMR_DONE\r\n", __func__);
#ifdef CONFIG_WIFI_WDS
            wifi_mgmr_ready = true;
#else
            app_wds_bridge_wifi_ready();
#endif
#ifdef USBWIFI_ENABLE
            usbwifi_start(&g_usbwifi);
#elif defined(BL_HOSTROUTER_ENABLE)
extern int hostrouter_init(void);
            hostrouter_init();
#endif
        } break;
        case CODE_WIFI_ON_SCAN_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_SCAN_DONE\r\n", __func__);
            wifi_mgmr_sta_scanlist();
        } break;
        case CODE_WIFI_ON_CONNECTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_CONNECTED\r\n", __func__);
            void mm_sec_keydump();
            mm_sec_keydump();
            app_wds_bridge_radio_up(false);
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
            app_wds_bridge_radio_down(false);
        } break;
        case CODE_WIFI_ON_AP_STARTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STARTED\r\n", __func__);
            app_wds_bridge_radio_up(true);
        } break;
        case CODE_WIFI_ON_AP_STOPPED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STOPPED\r\n", __func__);
            app_wds_bridge_radio_down(true);
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

int app_apeth_bridge_init(void)
{
    /* RF param init */
    if (0 != rfparam_init(0, NULL, 0)) {
        LOG_I("PHY RF init failed!\r\n");
        return 0;
    }

#ifdef CONFIG_NETIFD_NANO
    netifd_nano_init();
#endif
    /* TCP/IP stack init */
    tcpip_init(NULL, NULL);

    /* enable wifi clock */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_IP_WIFI_PHY | GLB_AHB_CLOCK_IP_WIFI_MAC_PHY | GLB_AHB_CLOCK_IP_WIFI_PLATFORM);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_WIFI);

    /* Enable wifi irq */
    extern void interrupt0_handler(void);
    bflb_irq_attach(WIFI_IRQn, (irq_callback)interrupt0_handler, NULL);
    bflb_irq_enable(WIFI_IRQn);

    /* Enable lfs_kv(littlefs) */
    // bflb_mtd_init();
    // lfs_kv_init();

    /* romsfs init mount use media factory*/
    // romfs_mount(0x378000);

    if (app_wds_bridge_init()) {
        LOG_E("Bridge initialization failed\r\n");
        return -1;
    }
#ifndef CONFIG_WIFI_WDS
    if (app_emac_init())
        return -1;
#endif

    /* Start Wifi_FW */
    xTaskCreate(wifi_start_firmware_task, "wifi init", WIFI_STACK_SIZE, NULL, 10, NULL);

    return 0;
}
