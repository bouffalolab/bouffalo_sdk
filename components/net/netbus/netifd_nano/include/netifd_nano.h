#ifndef __NETIFD_NANO_H__
#define __NETIFD_NANO_H__

#include "lwip/ip_addr.h"
#include <netif/bridgeif.h>
#include <lwip/netif.h>
#include <lwip/netifapi.h>
#include "lwip/dhcp.h"
//#include "lwip/pppapi.h"
#include "lwip/tcpip.h"
#include "lwip/etharp.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char name[16];
    ip4_addr_t ipaddr;
    ip4_addr_t netmask;
    ip4_addr_t gw;
} netifd_config_t;

/* Network Interface Configuration */

/**
 * @brief Initialize network interface
 * @param netifd_config_t* config: Network interface configuration
 * @return int: 0 on success, other values indicate failure
 */
int netifd_init(netifd_config_t* config);

/**
 * @brief Set network interface IP address
 * @param char* ifname: Interface name
 * @param ip4_addr_t ipaddr: IP address
 * @return int: 0 on success, other values indicate failure
 */
int netifd_set_ipaddr(const char* ifname, ip4_addr_t ipaddr);

/**
 * @brief Get network interface IP address
 * @param char* ifname: Interface name
 * @param ip4_addr_t* ipaddr: Storage for IP address
 * @return int: 0 on success, other values indicate failure
 */
int netifd_get_ipaddr(const char* ifname, ip4_addr_t* ipaddr);

/* Dynamic Interface Management */

/**
 * @brief Bring up the network interface
 * @param char* ifname: Interface name
 * @return int: 0 on success, other values indicate failure
 */
int netifd_up(const char* ifname);

/**
 * @brief Bring down the network interface
 * @param char* ifname: Interface name
 * @return int: 0 on success, other values indicate failure
 */
int netifd_down(const char* ifname);

/**
 * @brief Register callback function for network interface status changes
 * @param void (*callback)(const char* ifname, int status): Callback function
 * @return int: 0 on success, other values indicate failure
 */
int netifd_register_status_callback(void (*callback)(const char* ifname, int status));

/* Protocol Management */

/**
 * @brief Start DHCP client
 * @param char* ifname: Interface name
 * @return int: 0 on success, other values indicate failure
 */
int netifd_dhcp_start(const char* ifname);

/**
 * @brief Stop DHCP client
 * @param char* ifname: Interface name
 * @return int: 0 on success, other values indicate failure
 */
int netifd_dhcp_stop(const char* ifname);

/**
 * @brief Start PPPoE client
 * @param char* ifname: Interface name
 * @param char* username: Username
 * @param char* password: Password
 * @return int: 0 on success, other values indicate failure
 */
int netifd_pppoe_start(const char* ifname, const char* username, const char* password);

/**
 * @brief Stop PPPoE client
 * @param char* ifname: Interface name
 * @return int: 0 on success, other values indicate failure
 */
int netifd_pppoe_stop(const char* ifname);

/* Bridge Management */

/**
 * @brief Create bridge interface(The bridge_name parameter in the input may be renamed)
 * @param char* bridge_name: Bridge interface name
 * @return int: 0 on success, other values indicate failure
 */
int netifd_bridge_create(const char* bridge_name);

/**
 * @brief Delete bridge interface
 * @param char* bridge_name: Bridge interface name
 * @return int: 0 on success, other values indicate failure
 */
int netifd_bridge_delete(const char* bridge_name);

/**
 * @brief Add network interface to bridge interface
 * @param char* bridge_name: Bridge interface name
 * @param char* ifname: Interface name
 * @return int: 0 on success, other values indicate failure
 */
int netifd_bridge_add_if(const char* bridge_name, const char* ifname);

/**
 * @brief Remove network interface from bridge interface
 * @param char* bridge_name: Bridge interface name
 * @param char* ifname: Interface name
 * @return int: 0 on success, other values indicate failure
 */
int netifd_bridge_remove_if(const char* bridge_name, const char* ifname);

#ifdef __cplusplus
}
#endif

#endif /* NETIFD_API_H */
