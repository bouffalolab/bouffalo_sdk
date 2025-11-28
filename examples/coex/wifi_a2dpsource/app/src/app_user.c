#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include <bl_fw_api.h>
#include <fhost_api.h>
#include <wifi_mgmr_ext.h>
#include <wifi_mgmr.h>

#include <bflb_irq.h>
#include <bl616_glb.h>
#include <bflb_mtd.h>

#include <rfparam_adapter.h>

#define DBG_TAG "MAIN"
#include <log.h>


void wifi_event_handler(uint32_t code)
{
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
        } break;
        case CODE_WIFI_ON_GOT_IP: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_GOT_IP\r\n", __func__);
            LOG_I("[SYS] Memory left is %d Bytes\r\n", kfree_size());
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
        default: {
            LOG_I("[APP] [EVT] Unknown code %u \r\n", code);
        }
    }
}

void wifi_start_firmware_task(void *param)
{
    LOG_I("Starting wifi ...\r\n");

    wifi_task_create();

    LOG_I("Starting fhost ...\r\n");
    fhost_init();

    vTaskDelete(NULL);
}

int app_user_init(void)
{
    /* RF param init */
    if (0 != rfparam_init(0, NULL, 0)) {
        LOG_I("PHY RF init failed!\r\n");
        return 0;
    }

    /* Coex init, before wireless module init */
    coex_init();

    /* TCP/IP stack init */
    tcpip_init(NULL, NULL);
    wifi_mgmr_coex_enable(1);

    /* enable wifi clock */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_IP_WIFI_PHY | GLB_AHB_CLOCK_IP_WIFI_MAC_PHY | GLB_AHB_CLOCK_IP_WIFI_PLATFORM);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_WIFI);

    /* Enable wifi irq */
    extern void interrupt0_handler(void);
    bflb_irq_attach(WIFI_IRQn, (irq_callback)interrupt0_handler, NULL);
    bflb_irq_enable(WIFI_IRQn);

    /* Enable easyflash(littlefs) */
    bflb_mtd_init();
    // easyflash_init();

    /* romsfs init mount use media factory*/
    romfs_mount(0x378000);

    /* Start Wifi_FW */
    xTaskCreate(wifi_start_firmware_task, "wifi init", 1024, NULL, 10, NULL);

    /* start user app */
    app_bt_init();
    app_sample_init();

    return 0;
}
