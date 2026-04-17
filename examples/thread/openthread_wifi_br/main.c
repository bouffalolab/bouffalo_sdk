#include <FreeRTOS.h>
#include <task.h>

#if defined(BL616) || defined(BL616CL)
#include <rfparam_adapter.h>
#include "bl616.h"
#elif defined(BL702L)
#include "bl702l.h"
#endif

#include <assert.h>

#include <bl_sys.h>
#include <bflb_wdg.h>
#include <bflb_mtd.h>

#include <log.h>

#include <lmac154.h>
#if defined (BL616) || defined (BL702L)
#include <lmac154_fpt.h>
#endif
#include <zb_timer.h>

#include <lwip/tcpip.h>
#include <lwip/dhcp6.h>

#include <fhost.h>
#include <wifi_mgmr_ext.h>
#include <wifi_mgmr.h>

#include <rfparam_adapter.h>
#include <bl_sys.h>
#include <bflb_mtd.h>

#include OPENTHREAD_PROJECT_CORE_CONFIG_FILE
#include <openthread/thread.h>
#include <openthread/dataset_ftd.h>
#include <openthread/platform/settings.h>
#include <openthread/cli.h>
#include <openthread_port.h>
#include <openthread_br.h>
#include <otbr_rtos_lwip.h>
#include <openthread_rest.h>

#include "board.h"
#include "shell.h"
#include "async_event.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define OT_APP_NOTIFY_WIFI_INIT_EVENT   0x01000000
#define OT_APP_NOTIFY_WIFI_IP_EVENT     0x02000000

/****************************************************************************
 * Private Data
 ****************************************************************************/
static struct bflb_device_s *uart0;

static char otbr_wifi_ssid[33];
static char otbr_wifi_pass[65];

/****************************************************************************
 * Functions
 ****************************************************************************/
extern void __libc_init_array(void);
extern void shell_init_with_task(struct bflb_device_s *shell);

static void lmac154_app_init(void)
{
    irq_callback lmac154_isr_callback;

    lmac154_init();
    lmac154_enableCoex();
    lmac154_setStd2015Extra(true);
    lmac154_setTxRetry(0);
    lmac154_fptClear();
    lmac154_setEnhAckWaitTime((LMAC154_AIFS + 10 + (6 + 42) * 2) << LMAC154_US_PER_SYMBOL_BITS);
    lmac154_setRxStateWhenIdle(true);

#if defined(BL702L)
    lmac154_setTxRxTransTime(0xA0);
#endif
#if (defined (BL616) || defined(BL702L))
    lmac154_setFramePendingMode(LMAC154_FPT_ANY);
#endif
    zb_timer_cfg(bflb_mtimer_get_time_us() >> LMAC154_US_PER_SYMBOL_BITS);
    lmac154_disableRx();

    lmac154_isr_callback = (irq_callback)lmac154_getInterruptCallback();
    bflb_irq_attach(M154_INT_IRQn, lmac154_isr_callback, NULL);
    bflb_irq_enable(M154_INT_IRQn);
}

static void wifi_event_handler(async_input_event_t ev, void *priv)
{
    uint32_t code = ev->code;

     switch (code) {
        case CODE_WIFI_ON_INIT_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_INIT_DONE\r\n", __func__);
            wifi_mgmr_task_start();

            netif_create_ip6_linklocal_address(fhost_to_net_if(MGMR_VIF_STA), 1);
            ((struct netif *)fhost_to_net_if(MGMR_VIF_STA))->ip6_autoconfig_enabled = 1;
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
            OT_APP_NOTIFY(OT_APP_NOTIFY_WIFI_IP_EVENT);
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

static void prvInitTask(void *pvParameters)
{
    uint16_t valueLength;

    lmac154_app_init();

    LOG_I("Starting wifi ...\r\n");

    memset(otbr_getThreadNetif(), 0, sizeof(struct netif));
    otPlatSettingsInit(NULL, NULL, 0);

    memset(otbr_wifi_ssid, 0, sizeof(otbr_wifi_ssid));
    memset(otbr_wifi_pass, 0, sizeof(otbr_wifi_pass));

    valueLength = sizeof(otbr_wifi_ssid);
    otPlatSettingsGet(NULL, 0xff01, 0, (uint8_t *)otbr_wifi_ssid, &valueLength);
    valueLength = sizeof(otbr_wifi_pass);
    otPlatSettingsGet(NULL, 0xff02, 0, (uint8_t *)otbr_wifi_pass, &valueLength);
    printf("Load Wi-Fi AP SSID & password [%s]:[%s]\r\n", otbr_wifi_ssid, otbr_wifi_pass);

    memset(otbr_getThreadNetif(), 0, sizeof(struct netif));
    async_register_event_filter(EV_WIFI, wifi_event_handler, NULL);

    wifi_task_create();

    LOG_I("Starting fhost ...\r\n");
    fhost_init();

    LOG_I("Starting Thread ...\r\n");
    otrStart();

    vTaskDelete(NULL);
}

otbr_lwip_netif_type_t otbr_getInfraNetif(void)
{
    return (otbr_lwip_netif_type_t)fhost_to_net_if(MGMR_VIF_STA);
}

void otrAppProcess(ot_system_event_t sevent) 
{
    if (OT_APP_NOTIFY_WIFI_INIT_EVENT & sevent) {
        wifi_mgmr_sta_coex_enable();
        int iret = wifi_mgmr_sta_coex_duty_set(30);
        if (iret != 0) {
            LOG_E("Failed to set coexistence duty cycle with error code: %d\r\n", iret);
            return;
        }

        if (strlen(otbr_wifi_ssid) > 0) {
            int iret = wifi_mgmr_sta_quickconnect(otbr_wifi_ssid, otbr_wifi_pass, 0, 0);
            LOG_I("[APP] [EVT] connect AP [%s]:[%s] with result %d\r\n", otbr_wifi_ssid, otbr_wifi_pass, iret);
        }

        if (false == otIp6IsEnabled(otrGetInstance())) {
            otIp6SetEnabled(otrGetInstance(), true);
            otThreadSetEnabled(otrGetInstance(), true);
        }
    }

    if (OT_APP_NOTIFY_WIFI_IP_EVENT & sevent) {
        wifi_mgmr_sta_autoconnect_enable();

        if (strlen(otbr_wifi_ssid) == 0) {
            wifi_mgmr_connect_ind_stat_info_t ind;
            memset(&ind, 0, sizeof(wifi_mgmr_connect_ind_stat_info_t));

            if (0 == wifi_mgmr_sta_connect_ind_stat_get(&ind)) {
                memcpy(otbr_wifi_ssid, ind.ssid, sizeof(otbr_wifi_ssid));
                memcpy(otbr_wifi_pass, ind.passphr, sizeof(otbr_wifi_pass));

                otPlatSettingsSet(NULL, 0xff01, (uint8_t *)otbr_wifi_ssid, sizeof(otbr_wifi_ssid));
                otPlatSettingsSet(NULL, 0xff02, (uint8_t *)otbr_wifi_pass, sizeof(otbr_wifi_pass));
            }
        }
    }
}

void otrInitUser(otInstance * instance)
{
    otOperationalDataset ds;

    otAppCliInit((otInstance * )instance);

    if (!otDatasetIsCommissioned(otrGetInstance())) {

        if (OT_ERROR_NONE != otDatasetCreateNewNetwork(otrGetInstance(), &ds)) {
            printf("Failed to create dataset for Thread Network\r\n");
        }

        if (OT_ERROR_NONE != otDatasetSetActive(otrGetInstance(), &ds)) {
            printf("Failed to set active dataset\r\n");
        }
    }

    otbr_netif_init();
    otbr_nat64_init(OPENTHREAD_OTBR_CONFIG_NAT64_CIDR);

#if LWIP_NETIF_HOSTNAME
    netif_set_hostname((struct netif *)otbr_getInfraNetif(), otbr_hostname());
#endif

    OT_APP_NOTIFY(OT_APP_NOTIFY_WIFI_INIT_EVENT);
}

void vApplicationTickHook( void )
{
#ifdef BL616
    lmac154_monitor(10000);
#endif
}

int main(void)
{
#if defined(BL616)
    bl_sys_rstinfo_init();
#endif

    board_init();

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    __libc_init_array();

    bflb_mtd_init();

#if defined(BL616)
    if (0 != rfparam_init(0, NULL, 0)) {
        printf("PHY RF init failed!\r\n");
        return 0;
    }
#endif

    /* WiFi and LWIP init */
    tcpip_init(NULL, NULL);

    openthread_httpd_init(8081);

    xTaskCreate(prvInitTask, "init", 1024, NULL, 15, NULL);

    puts("[OS] Starting OS Scheduler...\r\n");
    vTaskStartScheduler();

    while (1) {
    }
}
