#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "netifd_nano.h"
#include <netif/bridgeif.h>
#include <lwip/netif.h>
#include <lwip/netifapi.h>
#include <platform_al.h>

/* Network Interface Configuration */

int netifd_init(netifd_config_t* config)
{
    struct netif* netif = netif_find(config->name);
    if (netif == NULL) {
        return -1; // Interface not found
    }
    netif_set_addr(netif, &config->ipaddr, &config->netmask, &config->gw);
    return 0;
}

int netifd_set_ipaddr(const char* ifname, ip4_addr_t ipaddr)
{
    struct netif* netif = netif_find(ifname);
    if (netif == NULL) {
        return -1; // Interface not found
    }
    netif_set_ipaddr(netif, &ipaddr);
    return 0;
}

int netifd_get_ipaddr(const char* ifname, ip4_addr_t* ipaddr)
{
    struct netif* netif = netif_find(ifname);
    if (netif == NULL) {
        return -1; // Interface not found
    }
    *ipaddr = *netif_ip4_addr(netif);
    return 0;
}

/* Dynamic Interface Management */

int netifd_up(const char* ifname)
{
    struct netif* netif = netif_find(ifname);
    if (netif == NULL) {
        return -1; // Interface not found
    }
    netif_set_up(netif);
    return 0;
}

int netifd_down(const char* ifname)
{
    struct netif* netif = netif_find(ifname);
    if (netif == NULL) {
        return -1; // Interface not found
    }
    netif_set_down(netif);
    return 0;
}

int netifd_register_status_callback(void (*callback)(const char* ifname, int status))
{
    // Implementation depends on your application's requirements
    return 0;
}

/* Protocol Management */

int netifd_dhcp_start(const char* ifname)
{
    struct netif* netif = netif_find(ifname);
    if (netif == NULL) {
        return -1; // Interface not found
    }
    printf("netif:0x%p\r\n", netif);
    //dhcp_start(netif);
    netifapi_dhcp_start(netif);
    return 0;
}

int netifd_dhcp_stop(const char* ifname)
{
    struct netif* netif = netif_find(ifname);
    if (netif == NULL) {
        return -1; // Interface not found
    }
    //dhcp_stop(netif);
    netifapi_dhcp_stop(netif);
    return 0;
}

int netifd_pppoe_start(const char* ifname, const char* username, const char* password) {
    // Implementation depends on your PPPoE client setup
    return 0;
}

int netifd_pppoe_stop(const char* ifname)
{
    // Implementation depends on your PPPoE client setup
    return 0;
}

/* Bridge Management */
struct netif*  g_bridge = NULL;

extern int is_netif_in_bridge(struct netif *bridgeif, struct netif *netif);
int is_netif_in_bridge_port(struct netif *bridgeif, struct netif *netif)
{
    if (g_bridge != bridgeif) {
        return 0;
    }
    if (is_netif_in_bridge(bridgeif, netif)) {
        return 1;
    }

    return 0;
}

int netifd_bridge_create(const char* bridge_name)
{
    struct netif* netif = NULL;

    uint8_t mac[6] = {0x18,0xB9,0x05,0x44,0x55};

    platform_get_mac(mac);

    if (g_bridge) {
        printf("now not support multbridge.\r\n");
        return -1;
    }

    netif = malloc(sizeof(struct netif));
    if (!netif) {
        return -1;
    }
    g_bridge = netif;

    printf("malloc netif:0x%p\r\n", netif);
    if (NULL == netif) {
        return -1;
    }
    /*
     * max_ports: Maximum number of ports in the bridge (ports are stored in an array,
     *            this influences memory allocated for netif->state of the 
     *            bridge netif). 
     * max_fdb_dynamic_entries: Maximum number of dynamic/learning entries in the 
     *            bridge's forwarding database. In the default implementation, this
     *            controls memory consumption only.
     * max_fdb_static_entries: Maximum number of static forwarding entries. Influences
     *            memory consumption!
     * ethaddr: MAC address of the bridge (cannot use the netif's addresses)
     */

    // TODO: control dynamic_entries and static_entries according to actual situation
    bridgeif_initdata_t bridge_initdata = BRIDGEIF_INITDATA1(2, 8, 8,
                                          ETH_ADDR(mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]));

    netif = netif_add_noaddr(netif, &bridge_initdata, bridgeif_init, netif_input);// tcpip_input
    if (netif == NULL) {
        return -1; // Failed to create bridge interface
    }
    strncpy(netif->name, bridge_name, sizeof(netif->name) - 1);

    // The bridge_name parameter in the input may be renamed.
    return netif;
}

int netifd_bridge_delete(const char* bridge_name)
{
    struct netif* netif = netif_find(bridge_name);
    if (netif == NULL) {
        return -1; // Interface not found
    }
    netif_remove(netif);

    printf("free  netif:0x%p\r\n", netif);
    free(netif);

    return 0;
}

int netifd_bridge_add_if(const char* bridge_name, const char* ifname)
{
    struct netif* bridgeif = netif_find(bridge_name);
    struct netif* netif = netif_find(ifname);
    if (bridgeif == NULL || netif == NULL) {
        return -1; // Interface not found
    }
    bridgeif_add_port(bridgeif, netif);

    return 0;
}

int netifd_bridge_remove_if(const char* bridge_name, const char* ifname)
{
    struct netif* bridgeif = netif_find(bridge_name);
    struct netif* netif = netif_find(ifname);
    if (bridgeif == NULL || netif == NULL) {
        return -1; // Interface not found
    }
#if 0
    //bridgeif_remove_port(bridgeif, netif);
    return 0;
#else
    return -2;
#endif
}

