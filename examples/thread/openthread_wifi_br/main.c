#include <FreeRTOS.h>
#include <task.h>

#if defined(BL616) || defined(BL616CL)
#include <rfparam_adapter.h>
#endif

#include <bl_sys.h>
#include <bflb_wdg.h>
#include <bflb_mtd.h>

#include <log.h>

#include <lmac154.h>

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
#include <ot_utils_ext.h>
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
extern void wifi_event_handler(async_input_event_t ev, void *priv);
static void netif_status_callback(struct netif *netif);
static void otr_start_default(void);
static void app_start_otr(void);

void vApplicationTickHook( void )
{
#ifdef BL616
    lmac154_monitor(10000);
#endif
}

void wifi_start_firmware_task(void *param)
{
    uint16_t valueLength;

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

    vTaskDelete(NULL);
}

void wifi_event_handler(async_input_event_t ev, void *priv)
{
    uint32_t code = ev->code;

     switch (code) {
        case CODE_WIFI_ON_INIT_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_INIT_DONE\r\n", __func__);
            wifi_mgmr_task_start();

            app_start_otr();

            netif_set_status_callback(fhost_to_net_if(0), netif_status_callback);
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

otbr_lwip_netif_type_t otbr_getInfraNetif(void)
{
    return (otbr_lwip_netif_type_t)fhost_to_net_if(0);
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
    }

    if (OT_APP_NOTIFY_WIFI_IP_EVENT & sevent) {
        wifi_mgmr_sta_autoconnect_enable();

        if (false == otIp6IsEnabled(otrGetInstance())) {
            otIp6SetEnabled(otrGetInstance(), true);
            otThreadSetEnabled(otrGetInstance(), true);
        }
    }
}

static void netif_status_callback(struct netif *netif)
{
    typedef enum {
        ADDRESS_SHOW_IDX_IPV4 = 0,
        ADDRESS_SHOW_IDX_IPV6 = 1,
    } address_shown_t;
    static address_shown_t address_show_msk = 0;
    bool isIPv4AddressAssigned = false;

    if (netif->flags & NETIF_FLAG_UP) {
        if(!ip4_addr_isany(netif_ip4_addr(netif)) && (0 == (address_show_msk & (1 << ADDRESS_SHOW_IDX_IPV4)))) {
            printf("IPv4 address: %s\r\n", ip4addr_ntoa(netif_ip4_addr(netif)));
            printf("IPv4 mask: %s\r\n", ip4addr_ntoa(netif_ip4_netmask(netif)));
            printf("Gateway address: %s\r\n", ip4addr_ntoa(netif_ip4_gw(netif)));
            address_show_msk |= (1 << ADDRESS_SHOW_IDX_IPV4);
            isIPv4AddressAssigned = true;
        }

        for (uint32_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i ++ ) {
            if (!ip6_addr_isany(netif_ip6_addr(netif, i))
                && ip6_addr_ispreferred(netif_ip6_addr_state(netif, i))) {

                const ip6_addr_t* ip6addr = netif_ip6_addr(netif, i);
                if (ip6_addr_isany(ip6addr)) {
                    continue;
                }

                if(ip6_addr_islinklocal(ip6addr)){
                    if (0 == (address_show_msk & (1 << (i + ADDRESS_SHOW_IDX_IPV6)))) {
                        printf("IPv6 linklocal address: %s\r\n", ip6addr_ntoa(ip6addr));
                    }
                }
                else{
                    if (0 == (address_show_msk & (1 << (i + ADDRESS_SHOW_IDX_IPV6)))) {
                        printf("IPv6 address %d: %s\r\n", i, ip6addr_ntoa(ip6addr));
                    }
                }

                address_show_msk |= (1 << (i + ADDRESS_SHOW_IDX_IPV6));
            }
        }

        if (isIPv4AddressAssigned) {
            wifi_mgmr_sta_ps_enter();
            wifi_mgmr_sta_autoconnect_enable();

            if (otrGetInstance()) {
                if (false == otIp6IsEnabled(otrGetInstance())) {

                    printf("IPv4 address is assigned, start Thread stack.\r\n");
                    otIp6SetEnabled(otrGetInstance(), true);
                    otThreadSetEnabled(otrGetInstance(), true);
                }

                otbr_instance_routing_init();
            }

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
    else {
        address_show_msk = 0;
        printf("Interface is down status.\r\n");
    }
}

void otr_start_default(void)
{
    otOperationalDataset ds;

    if (!otDatasetIsCommissioned(otrGetInstance())) {

        if (OT_ERROR_NONE != otDatasetCreateNewNetwork(otrGetInstance(), &ds)) {
            printf("Failed to create dataset for Thread Network\r\n");
        }

        if (OT_ERROR_NONE != otDatasetSetActive(otrGetInstance(), &ds)) {
            printf("Failed to set active dataset\r\n");
        }
    }
}

void otrInitUser(otInstance * instance)
{
    otAppCliInit((otInstance * )instance);
    otr_start_default();
    otbr_netif_init();
    otbr_nat64_init(OPENTHREAD_OTBR_CONFIG_NAT64_CIDR);

#if LWIP_NETIF_HOSTNAME
    netif_set_hostname((struct netif *)otbr_getInfraNetif(), otbr_hostname());
#endif

    OT_APP_NOTIFY(OT_APP_NOTIFY_WIFI_INIT_EVENT);
}

void app_start_otr(void) 
{
    otRadio_opt_t opt;

    opt.byte = 0;

    opt.bf.isCoexEnable = true;
    opt.bf.isFtd = true;
    #if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    opt.bf.isLinkMetricEnable = true;
    #endif
    #if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    opt.bf.isCSLReceiverEnable = true;
    #endif
    #if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    opt.bf.isTimeSyncEnable = true;
    #endif

    otrStart(opt);
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
    xTaskCreate(wifi_start_firmware_task, "wifi init", 1024, NULL, 10, NULL);

    openthread_httpd_init(8081);

    puts("[OS] Starting OS Scheduler...\r\n");
    vTaskStartScheduler();

    while (1) {
    }
}
