#include <string.h>
#include <netif/etharp.h>
#include <lwip/dns.h>

#ifdef BL602_MATTER_SUPPORT
#include <lwip/ethip6.h>
#endif

#include <wifi_pkt_hooks.h>

#include "bl_defs.h"
#include "bl_tx.h"
#include "bl_msg_tx.h"
#include "bflb_os_private.h"
#include "wifi_mgmr.h"
#include "wifi_mgmr_api.h"

#define WIFI_MTU_SIZE 1514

#if 0
#define NET_TRACE
#define ETH_RX_DUMP
#define ETH_TX_DUMP
#endif
#define MAX_ADDR_LEN    6

#ifdef NET_TRACE
#define NET_DEBUG         bflb_os_printf
#else
#define NET_DEBUG(...)
#endif
/**
 ****************************************************************************************
 *
 * @file wifi.c
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

static BL_TaskHandle_t taskHandle_output = NULL;

extern int bl_main_rtthread_start(struct bl_hw **bl_hw);

struct net_device
{
    struct bl_hw *bl_hw;
};

static struct net_device bl606a0_sta;

static void bl_tx_notify(void *cb_arg, bool tx_ok)
{
    //TODO static alloc taskHandle_output, no if else anymore
    if (taskHandle_output) {
        bflb_os_task_notify(taskHandle_output);
    }

    return;
}

#ifdef CFG_NETBUS_WIFI_ENABLE
/* ethernet device interface */
/* Transmit packet. */
err_t wifi_tx(struct netif *netif, struct pbuf* p)
{
    bool can_tx = false;
    bool from_local = false;
    if ((uintptr_t)p & 1) {
        can_tx = true;
    } else {
#if 0
        from_local = true;
        if (npf_is_arp(p)) {
            can_tx = true;
        }
        if (npf_is_dhcp(p)) {
            can_tx = true;
        }
#endif
    }
    if (!can_tx) {
        NET_DEBUG("[wifi tx] can not tx %p\r\n", p);
        return ERR_IF;
    }
    p = (struct pbuf *)((uintptr_t)p & ~1);

    struct wlan_netif *wlan;
    struct bl_tx_cfm custom_cfm = { bl_tx_notify, NULL };
#if 0
    struct net_device * bl606a0_sta = (struct net_device *)dev;
#endif
    struct pbuf* q;
    static uint32_t ticks;
#define WARNING_LIMIT_TICKS_TX_SIZE  2000
#ifdef ETH_TX_DUMP
    int dump_count = 0;
    uint8_t * dump_ptr;
    int dump_i;
#endif

#ifdef PKT_OUTPUT_HOOK
    if (bl_wifi_pkt_eth_output_hook) {
        bool is_sta = netif == wifi_mgmr_sta_netif_get();
        p = bl_wifi_pkt_eth_output_hook(is_sta, p, bl_wifi_pkt_eth_output_hook_arg);
        if (p == NULL) {
            // hook ate the packet
            return ERR_IF;
        }
    }
#endif
    if (p->tot_len > WIFI_MTU_SIZE) {
        if (bflb_os_get_time_ms() - ticks > WARNING_LIMIT_TICKS_TX_SIZE) {
            bflb_os_printf("[TX] %s, TX size too big: %u bytes\r\n", __func__, p->tot_len);
            ticks = bflb_os_get_time_ms();
        }
        return ERR_IF;
    }

#ifdef ETH_TX_DUMP
    NET_DEBUG("tx_dump, size:%d\r\n", p->tot_len);
#endif
    for (q = p; q != NULL; q = q->next)
    {
#ifdef ETH_RX_DUMP
        dump_ptr = q->payload;
        for(dump_i=0; dump_i<q->len; dump_i++)
        {
            NET_DEBUG("%02x ", *dump_ptr);
            if( ((dump_count+1)%8) == 0 )
            {
                NET_DEBUG("  ");
            }
            if( ((dump_count+1)%16) == 0 )
            {
                NET_DEBUG("\r\n");
            }
            dump_count++;
            dump_ptr++;
        }
#endif
    }
#ifdef ETH_RX_DUMP
    NET_DEBUG("\r\n");
#endif

    if (0 == taskHandle_output) {
        taskHandle_output = bflb_os_task_get_current_task();
    }
    wlan = container_of(netif, struct wlan_netif, netif);
    return bl_output(bl606a0_sta.bl_hw, (0 == wlan->mode), p, &custom_cfm, from_local);
}

#else

static err_t wifi_tx(struct netif *netif, struct pbuf* p)
{
    struct wlan_netif *wlan;
    struct bl_tx_cfm custom_cfm = { bl_tx_notify, NULL };
#if 0
    struct net_device * bl606a0_sta = (struct net_device *)dev;
#endif
    struct pbuf* q;
    static uint32_t ticks;
#define WARNING_LIMIT_TICKS_TX_SIZE  2000
#ifdef ETH_TX_DUMP
    int dump_count = 0;
    uint8_t * dump_ptr;
    int dump_i;
#endif

#ifdef PKT_OUTPUT_HOOK
    if (bl_wifi_pkt_eth_output_hook) {
        bool is_sta = netif == wifi_mgmr_sta_netif_get();
        p = bl_wifi_pkt_eth_output_hook(is_sta, p, bl_wifi_pkt_eth_output_hook_arg);
        if (p == NULL) {
            // hook ate the packet
            return ERR_IF;
        }
    }
#endif
    if (p->tot_len > WIFI_MTU_SIZE) {
        if (bflb_os_get_time_ms() - ticks > WARNING_LIMIT_TICKS_TX_SIZE) {
            bflb_os_printf("[TX] %s, TX size too big: %u bytes\r\n", __func__, p->tot_len);
            ticks = bflb_os_get_time_ms();
        }
        return ERR_IF;
    }

#ifdef ETH_TX_DUMP
    NET_DEBUG("tx_dump, size:%d\r\n", p->tot_len);
#endif
    for (q = p; q != NULL; q = q->next)
    {
#ifdef ETH_RX_DUMP
        dump_ptr = q->payload;
        for(dump_i=0; dump_i<q->len; dump_i++)
        {
            NET_DEBUG("%02x ", *dump_ptr);
            if( ((dump_count+1)%8) == 0 )
            {
                NET_DEBUG("  ");
            }
            if( ((dump_count+1)%16) == 0 )
            {
                NET_DEBUG("\r\n");
            }
            dump_count++;
            dump_ptr++;
        }
#endif
    }
#ifdef ETH_RX_DUMP
    NET_DEBUG("\r\n");
#endif

    if (0 == taskHandle_output) {
        taskHandle_output = bflb_os_task_get_current_task();
    }
    wlan = container_of(netif, struct wlan_netif, netif);
    return bl_output(bl606a0_sta.bl_hw, (0 == wlan->mode), p, &custom_cfm);
}
#endif

int bl_wifi_eth_tx(struct pbuf *p, bool is_sta, struct bl_tx_cfm *custom_cfm)
{
    err_t ret;
#ifdef CFG_NETBUS_WIFI_ENABLE
    ret = bl_output(bl606a0_sta.bl_hw, is_sta, p, custom_cfm, 1);
#else
    ret = bl_output(bl606a0_sta.bl_hw, is_sta, p, custom_cfm);
#endif
    if (ret == ERR_OK) {
        return 0;
    } else {
        return -1;
    }
}

static void netif_status_callback(struct netif *netif)
{
    bflb_os_printf("[lwip] netif status callback\r\n"
                "  IP: %s\r\n", ip4addr_ntoa(netif_ip4_addr(netif)));
    bflb_os_printf("  MK: %s\r\n", ip4addr_ntoa(netif_ip4_netmask(netif)));
    bflb_os_printf("  GW: %s\r\n", ip4addr_ntoa(netif_ip4_gw(netif)));
    if (ip4_addr_isany(netif_ip4_addr(netif))) {
        wifi_mgmr_api_ip_update();
    } else {
        wifi_mgmr_api_ip_got();
    }
}

err_t bl606a0_wifi_netif_init(struct netif *netif)
{
#if LWIP_NETIF_HOSTNAME
    netif->hostname = wifiMgmr.hostname;
#endif
    netif->hwaddr_len = ETHARP_HWADDR_LEN;
    /* set netif maximum transfer unit */
    netif->mtu = 1500;
    /* Accept broadcast address and ARP traffic */
#ifdef BL602_MATTER_SUPPORT
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP | NETIF_FLAG_MLD6;
    netif->output_ip6 = ethip6_output;
#else
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;
#endif
    netif->output = etharp_output;
    netif->linkoutput = wifi_tx;
    netif_set_status_callback(netif, netif_status_callback);

    return 0;
}

int bl606a0_wifi_init(wifi_conf_t *conf)
{
    uint8_t mac[6];
    int ret;

    bflb_os_printf("\r\n\r\n[BL] Initi Wi-Fi");
    memset(mac, 0, sizeof(mac));
    wifi_hosal_efuse_read_mac(mac);
    bflb_os_printf(" with MAC #### %02X:%02X:%02X:%02X:%02X:%02X ####\r\n", mac[0],
            mac[1],
            mac[2],
            mac[3],
            mac[4],
            mac[5]
    );
    snprintf(wifiMgmr.hostname, MAX_HOSTNAME_LEN_CHECK, "Bouffalolab_%s-%02x%02x%02x", BL_CHIP_NAME, mac[3], mac[4], mac[5]);
    wifiMgmr.hostname[MAX_HOSTNAME_LEN_CHECK - 1] = '\0';
    bflb_os_printf("     hostname: %s\r\n", wifiMgmr.hostname);
    bl_msg_update_channel_cfg(conf->country_code);
    strncpy(wifiMgmr.country_code, conf->country_code, sizeof(wifiMgmr.country_code));
    wifiMgmr.country_code[2] = '\0';
    bflb_os_printf("-----------------------------------------------------\r\n");
    //FIXME: This is an empty function
    //bl_wifi_clock_enable();//Enable wifi clock
    memset(&bl606a0_sta, 0, sizeof(bl606a0_sta));
    ret = bl_main_rtthread_start(&(bl606a0_sta.bl_hw));
    wifiMgmr.channel_nums = bl_msg_get_channel_nums();

    return ret;
}
