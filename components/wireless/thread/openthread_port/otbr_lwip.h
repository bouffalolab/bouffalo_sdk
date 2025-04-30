
#ifndef __OTBR_LWIP_H
#define __OTBR_LWIP_H

#ifdef __cplusplus
extern "C" {
#endif
#include <lwip/netif.h>
#include <lwip/ip_addr.h>
#include <lwip/ip4_addr.h>
#include <lwip/ip6_addr.h>
#include <lwip/udp.h>

struct netif * otbr_getInfraNetif(void);
struct netif * otbr_getThreadNetif(void);

bool otbr_lwip_netif_is_up(struct netif *nif);
u8_t otbr_lwip_get_netif_index(struct netif *nif);
int otbr_lwip_get_ipv6_address_num(void);
int otbr_lwip_get_netif_num(struct netif *nif);

ip4_addr_t * otbr_lwip_get_ipv4_address(struct netif *nif);
ip4_addr_t * otbr_lwip_get_ipv4_mask(struct netif *nif);
ip4_addr_t * otbr_lwip_get_ipv4_gateway(struct netif *nif);
ip_addr_t * otbr_lwip_get_ipv6_addr(struct netif *nif);
u8_t * otbr_lwip_get_ip6_addr_state(struct netif *nif);
ip_addr_t * otbr_lwip_netif_ip_addr6(struct netif *nif, int i);
ip6_addr_t * otbr_lwip_netif_ip6_addr(struct netif *nif, int i);

struct mld_group * otbr_lwip_netif_mld6_data(struct netif *nif);
void otbr_lwip_netif_mld6_data_set(struct netif *nif, struct mld_group * group);
netif_mld_mac_filter_fn otbr_lwip_mld_mac_filter(struct netif *nif);
bool otbr_lwip_netif_checksum_enabled(struct netif *nif, uint16_t flag);


uint8_t otbr_lwip_get_dns_max_servers(void);
uint32_t otbr_lwip_get_max_rio_routes(void);
uint32_t otbr_lwip_get_rio_timeout(void);

char * otbr_vendor_name(void);
char * otbr_product_name(void);

void otbr_udp_set_multicast_ttl(struct udp_pcb * pcb, uint32_t ttl);
void otbr_udp_set_flags(struct udp_pcb * pcb, uint32_t flag);
void otbr_udp_clear_flags(struct udp_pcb * pcb, uint32_t flag);

#if !defined(LWIP_MULTICAST_TX_OPTIONS) || !LWIP_MULTICAST_TX_OPTIONS
#error "LWIP_MULTICAST_TX_OPTIONS is required."
#endif

#if !defined(LWIP_IPV4) || !LWIP_IPV4
#error "LWIP_IPV4 is required."
#endif

#if !defined(LWIP_IPV6) || !LWIP_IPV6
#error "LWIP_IPV6 is required."
#endif

#if defined(LWIP_SINGLE_NETIF) && LWIP_SINGLE_NETIF
#error "LWIP_SINGLE_NETIF is NOT required."
#endif

#if defined(LWIP_IPV6_SCOPES) && LWIP_IPV6_SCOPES
#error "LWIP_IPV6_SCOPES is NOT required."
#endif

#if !defined(LWIP_TCPIP_CORE_LOCKING) || !LWIP_TCPIP_CORE_LOCKING
#error "LWIP_TCPIP_CORE_LOCKING is required."
#endif

#ifdef __cplusplus
}
#endif

#endif
