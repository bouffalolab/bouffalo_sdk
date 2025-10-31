#include <bflb_irq.h>
#include <bflb_uart.h>

#include <rfparam_adapter.h>
#include <bl_sys.h>
#include <bflb_wdg.h>
#include <bflb_mtd.h>
#if defined (CONFIG_LITTLEFS)
#include <easyflash.h>
#endif
#include <log.h>

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <mem.h>

#include <lmac154.h>

#include <lwip/tcpip.h>
#include <lwip/dhcp6.h>

#if __has_include("bl_fw_api.h")
#include <bl_fw_api.h>
#else
#include <export/bl_fw_api.h>
#endif
#include "fhost_api.h"
#include <wifi_mgmr_ext.h>
#include <wifi_mgmr.h>

#include <openthread/thread.h>
#include <openthread/dataset_ftd.h>
#include <openthread/platform/settings.h>
#include <openthread/cli.h>
#include <openthread_port.h>
#include <openthread_br.h>
#include <ot_utils_ext.h>

#include <coex.h>

#include "board.h"
#include "shell.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define WIFI_STACK_SIZE  (1536)
#define TASK_PRIORITY_FW (16)

#define THREAD_CHANNEL      15
#define THREAD_PANID        0x6677
#define THREAD_EXTPANID     {0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22}
#define THREAD_NETWORK_KEY  {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct bflb_device_s *uart0;
static wifi_conf_t conf = {
    .country_code = "CN",
};

static char otbr_wifi_ssid[33];
static char otbr_wifi_pass[65];

/****************************************************************************
 * Functions
 ****************************************************************************/
extern void __libc_init_array(void);
extern void shell_init_with_task(struct bflb_device_s *shell);
static void netif_status_callback(struct netif *netif);
static void otr_start_default(void);

void vApplicationTickHook( void )
{
#ifdef BL616
    lmac154_monitor();
#endif
#if CONFIG_LMAC154_LOG
    lmac154_logs_output();
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

    coex_init();
    wifi_mgmr_coex_enable(1);

    memset(otbr_getThreadNetif(), 0, sizeof(struct netif));
    wifi_task_create();

    LOG_I("Starting fhost ...\r\n");
    fhost_init();

    vTaskDelete(NULL);
}

void wifi_event_handler(uint32_t code)
{
    switch (code) {
        case CODE_WIFI_ON_INIT_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_INIT_DONE\r\n", __func__);
            wifi_mgmr_init(&conf);

            if (strlen(otbr_wifi_ssid) > 0) {
                int iret = wifi_mgmr_sta_quickconnect(otbr_wifi_ssid, otbr_wifi_pass, 0, 0);
                LOG_I("[APP] [EVT] connect AP [%s]:[%s] with result %d\r\n", otbr_wifi_ssid, otbr_wifi_pass, iret);
            }

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

struct netif * otbr_getInfraNetif(void)
{
    return fhost_to_net_if(0);
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
            int wifi_mgmr_sta_connect_ind_stat_get(wifi_mgmr_connect_ind_stat_info_t *wifi_mgmr_ind_stat);


            if (otrGetInstance()) {
                if (false == otIp6IsEnabled(otrGetInstance())) {

                    printf("IPv4 address is assigned, start Thread stack.\r\n");
                    otIp6SetEnabled(otrGetInstance(), true);
                    otThreadSetEnabled(otrGetInstance(), true);
                }

                otbr_instance_routing_init();
            }
            else {
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
    ot_coexist_event_init();
    otAppCliInit((otInstance * )instance);
    otr_start_default();
    otbr_netif_init();
    otbr_nat64_init(OPENTHREAD_OTBR_CONFIG_NAT64_CIDR);

#if LWIP_NETIF_HOSTNAME
    netif_set_hostname(otbr_getInfraNetif(), otbr_hostname());
#endif

    if (false == otIp6IsEnabled(otrGetInstance())) {
        otIp6SetEnabled(otrGetInstance(), true);
        otThreadSetEnabled(otrGetInstance(), true);
    }
}

int main(void)
{

#if !defined(BL702L)
    bl_sys_rstinfo_init();
#endif

    board_init();

    bflb_mtd_init();
#if defined (CONFIG_LITTLEFS)
    easyflash_init();
#endif

#if defined(BL616) || defined(BL616L)
    /* Init rf */
    if (0 != rfparam_init(0, NULL, 0)) {
        printf("PHY RF init failed!\r\n");
        return 0;
    }
#endif

    __libc_init_array();

#if CONFIG_LMAC154_LOG
    lmac154_log_init();
#endif

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    /* WiFi and LWIP init */
    tcpip_init(NULL, NULL);
    xTaskCreate(wifi_start_firmware_task, "wifi init", 1024, NULL, 10, NULL);

    puts("[OS] Starting OS Scheduler...\r\n");
    vTaskStartScheduler();

    while (1) {
    }
}
