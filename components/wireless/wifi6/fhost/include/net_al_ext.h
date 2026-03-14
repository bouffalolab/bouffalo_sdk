#ifndef __NET_AL_EXT_H
#define __NET_AL_EXT_H

#include <stdint.h>
#include <stdbool.h>
#include <net_def.h>

#define WIFI_STA_DHCPC_TIMEOUT_MS_DEFAULT 64000

/**
 * Enum for IP address configuration mode
 */
enum net_al_ext_ip_addr_mode
{
    IP_ADDR_NONE,
    IP_ADDR_STATIC_IPV4,
    IP_ADDR_DHCP_CLIENT,
};

/**
 * Fully Hosted IP address configuration (only IPv4 for now)
 */
struct net_al_ext_ip_addr_cfg
{
    /**
     * Select how to configure ip address when calling @ref fhost_set_vif_ip
     * Indicate how ip was configured when updated by @ref fhost_get_vif_ip (ref @ enum net_al_ext_ip_addr_mode)
     */
    uint8_t mode;
    /**
     * Whether interface must be the default output interface
     * (Unspecified when calling @ref fhost_get_vif_ip)
     */
    bool default_output;
    union {
        /**
         * IPv4 config.
         * Must be set when calling @ref fhost_set_vif_ip with @p mode==IP_ADDR_STATIC_IPV4
         * Always set when updated by @ref fhost_get_vif_ip (whatever the value of @p mode)
         */
        struct {
            /**
             * IPv4 address
             */
            uint32_t addr;
            /**
             * IPv4 address mask
             */
            uint32_t mask;
            /**
             * IPv4 address of the gateway
             */
            uint32_t gw;
            /**
             * DNS server to use. (Ignored if set to 0)
             */
            uint32_t dns;
        } ipv4;
        /**
         * DHCP config.
         * Must be set when calling @ref fhost_set_vif_ip with @p addr_mode==IP_ADDR_DHCP_CLIENT
         */
        struct {
            /**
             * Timeout, in ms, to obtained an IP address
             */
            uint32_t to_ms;
            /**
             * from_api, whether call dhcp start from api
             */
            uint32_t from_api;
        } dhcp;
    };
};

int net_al_ext_set_vif_ip(int fvif_idx, struct net_al_ext_ip_addr_cfg *cfg);
int net_al_ext_get_vif_ip(int fvif_idx, struct net_al_ext_ip_addr_cfg *cfg);

int net_al_link_set(net_al_if_t net_if);
int net_al_dhcpd_start(net_al_if_t net_if, int start, int limit);
int net_al_dhcpd_stop(net_al_if_t net_if);
int net_al_gw_service_enable(net_al_if_t net_if);
int net_al_gw_service_disable(net_al_if_t net_if);
int net_al_ext_dhcp_connect(int is_api, uint32_t to_ms);
void net_al_ext_dhcp_disconnect(void);
void net_al_ext_netif_status_callback(struct netif *netif);
int net_al_set_ipv6_enable(int enable);
#endif
