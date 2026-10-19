#include <lwip/netifapi.h>

#include "wifi_mgmr.h"
#include "wifi_netif.h"

#define DEBUG_HEADER "[WF][IF] "

int wifi_netif_dhcp_start(struct netif *netif)
{
    if (0 == wifiMgmr.wlan_sta.dhcp_started) {
        netifapi_dhcp_start(netif);
        wifiMgmr.wlan_sta.dhcp_started = 1;
    } else {
        bflb_os_printf(DEBUG_HEADER "Skip DHCPCing...\r\n");
    }
    return 0;
}

int wifi_netif_dhcp_stop(struct netif *netif)
{
    netifapi_dhcp_stop(netif);
    wifiMgmr.wlan_sta.dhcp_started = 0;
    return 0;
}

