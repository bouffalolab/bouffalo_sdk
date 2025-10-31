#include <bflb_irq.h>
#include <bflb_uart.h>
#include <bflb_wdg.h>
#include <bflb_emac.h>
#include <bl616_glb.h>

#include <rfparam_adapter.h>
#include <bl_sys.h>
#include <bflb_mtd.h>

#include "eth_phy.h"
#include "ephy_general.h"

#if defined(CONFIG_LITTLEFS)
#include <easyflash.h>
#endif

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <mem.h>

#include "lwip/opt.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp6.h"
#if LWIP_DHCP
#include "lwip/dhcp.h"
#endif
#include "lwip_netif_emac.h"

#include <lmac154.h>

#include <openthread/thread.h>
#include <openthread/dataset_ftd.h>
#include <openthread/cli.h>
#include <openthread_port.h>
#include <openthread_br.h>
#if CONFIG_OTBR_REST
#include <openthread_rest.h>
#endif

#include "board.h"

#define DBG_TAG "MAIN"
#include "log.h"

#define THREAD_CHANNEL     15
#define THREAD_PANID       0x6677
#define THREAD_EXTPANID    { 0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22 }
#define THREAD_NETWORK_KEY { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff }

static struct bflb_device_s *uart0;

/* global network interface struct define */
static struct netif gnetif;

/****************************************************************************
 * Functions
 ****************************************************************************/
extern void __libc_init_array(void);
extern void shell_init_with_task(struct bflb_device_s *shell);

void vApplicationTickHook( void )
{
#ifdef BL616
    lmac154_monitor();
#endif
#if CONFIG_LMAC154_LOG
    lmac154_logs_output();
#endif
}

struct netif *otbr_getInfraNetif(void)
{
    return &gnetif;
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
        if (!ip4_addr_isany(netif_ip4_addr(netif)) && (0 == (address_show_msk & (1 << ADDRESS_SHOW_IDX_IPV4)))) {
            printf("IPv4 address: %s\r\n", ip4addr_ntoa(netif_ip4_addr(netif)));
            printf("IPv4 mask: %s\r\n", ip4addr_ntoa(netif_ip4_netmask(netif)));
            printf("Gateway address: %s\r\n", ip4addr_ntoa(netif_ip4_gw(netif)));
            address_show_msk |= (1 << ADDRESS_SHOW_IDX_IPV4);
            isIPv4AddressAssigned = true;
        }

        for (uint32_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
            if (!ip6_addr_isany(netif_ip6_addr(netif, i)) && ip6_addr_ispreferred(netif_ip6_addr_state(netif, i))) {
                const ip6_addr_t *ip6addr = netif_ip6_addr(netif, i);
                if (ip6_addr_isany(ip6addr)) {
                    continue;
                }

                if (ip6_addr_islinklocal(ip6addr)) {
                    if (0 == (address_show_msk & (1 << (i + ADDRESS_SHOW_IDX_IPV6)))) {
                        printf("IPv6 linklocal address: %s\r\n", ip6addr_ntoa(ip6addr));
                    }
                } else {
                    if (0 == (address_show_msk & (1 << (i + ADDRESS_SHOW_IDX_IPV6)))) {
                        printf("IPv6 address %d: %s\r\n", i, ip6addr_ntoa(ip6addr));
                    }
                }

                address_show_msk |= (1 << (i + ADDRESS_SHOW_IDX_IPV6));
            }
        }

        if (isIPv4AddressAssigned) {
            if (false == otIp6IsEnabled(otrGetInstance())) {
                printf("IPv4 address is assigned, start Thread stack.\r\n");
                otIp6SetEnabled(otrGetInstance(), true);
                otThreadSetEnabled(otrGetInstance(), true);
            }
            otbr_instance_routing_init();
        }

    } else {
        address_show_msk = 0;
        printf("Interface is down status.\r\n");
    }
}

void lwip_status_update_task(void *pvParameters)
{
    LOG_I("lwip_status_update_task Run...\r\n");

    /* Infinite loop */
    while (1) {
        vTaskDelay(200);

        LOCK_TCPIP_CORE();
        /* update link status */
        eth_link_state_update(&gnetif);
        UNLOCK_TCPIP_CORE();

#if LWIP_DHCP
        /* update dhcp status */
        dhcp_sta_update(&gnetif);
#endif
        
    }
}

/**
  * @brief  Setup the network interface
  * @param  None
  * @retval None
  */
static void netif_config(void *arg)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    memset(&ipaddr, 0, sizeof(ip_addr_t));
    memset(&netmask, 0, sizeof(ip_addr_t));
    memset(&gw, 0, sizeof(ip_addr_t));

    /* add the network interface */
    netif_add(&gnetif, &ipaddr.u_addr.ip4, &netmask.u_addr.ip4, &gw.u_addr.ip4, NULL, &eth_emac_if_init, &tcpip_input);

    /*  Registers the default network interface */
    netif_set_default(&gnetif);

    netif_create_ip6_linklocal_address(&gnetif, 1);
    gnetif.ip6_autoconfig_enabled = 1;
    dhcp6_enable_stateless(&gnetif);

    netif_set_status_callback(&gnetif, netif_status_callback);

#if CONFIG_OTBR_REST
    openthread_httpd_init(8081);
#endif

    /* lwip status update task */
    xTaskCreate((void *)lwip_status_update_task, (char *)"lwip_sta_update", 512, NULL, osPriorityHigh, NULL);
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

void otrInitUser(otInstance *instance)
{
    otAppCliInit((otInstance *)instance);
    otr_start_default();
    otbr_netif_init();
    otbr_nat64_init(OPENTHREAD_OTBR_CONFIG_NAT64_CIDR);
}

int main(void)
{
    otRadio_opt_t opt;
    
#if !defined(BL702L)
    bl_sys_rstinfo_init();
#endif

    board_init();

    /* emac gpio init */
    board_emac_gpio_init();

    bflb_mtd_init();
#if defined(CONFIG_LITTLEFS)
    easyflash_init();
#endif

#if defined(BL616)
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

    memset(otbr_getThreadNetif(), 0, sizeof(struct netif));

    tcpip_init(netif_config, NULL);

    opt.byte = 0;

    opt.bf.isCoexEnable = false;
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

    puts("[OS] Starting OS Scheduler...\r\n");
    vTaskStartScheduler();

    while (1) {
    }
}
