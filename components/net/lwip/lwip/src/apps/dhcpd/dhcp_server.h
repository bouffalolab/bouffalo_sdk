#ifndef DHCPV4_SERVER_H__
#define DHCPV4_SERVER_H__

#include <stdio.h>
#include <lwip/ip.h>

#ifdef __cplusplus
extern "C" {
#endif

/* start, limit is range of dhcpd ip pool, -1 mean use default value */
void dhcpd_start(struct netif *netif, int start, int limit);
void dhcpd_stop(const char *netif_name);
void dhcpd_stop_with_netif(const struct netif *netif);

void dhcpd_clear_dns_server(void* netif);
err_t dhcpd_add_dns_server(void* netif, const ip_addr_t *dnsserver);
err_t dhcpd_remove_dns_server(void* netif, const ip_addr_t *dnsserver);

#ifdef __cplusplus
}
#endif

#endif

