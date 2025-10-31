#ifndef __WIFI_NETIF_H__
#define __WIFI_NETIF_H__
int wifi_netif_dhcp_start(struct netif *netif);
int wifi_netif_dhcp_stop(struct netif *netif);
#endif
