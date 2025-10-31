#ifndef _LWIP_DEFAULT_HOOKS_H_
#define _LWIP_DEFAULT_HOOKS_H_
#include "lwip/arch.h"
#include "lwip/err.h"
#include "lwip/ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LWIP_HOOK_INCLUDE
#include LWIP_HOOK_INCLUDE
#endif

bool otbr_lwip_hook_ip6_input(struct pbuf *p, struct netif *inp);
#define LWIP_HOOK_IP6_INPUT otbr_lwip_hook_ip6_input

struct netif * otbr_lwip_hook_ip6_route(const ip6_addr_t * src, const ip6_addr_t * dest);
#define LWIP_HOOK_IP6_ROUTE otbr_lwip_hook_ip6_route

const ip6_addr_t * otbr_lwip_hook_nd6_get_gw(struct netif * netif, const ip6_addr_t * dest);
#define LWIP_HOOK_ND6_GET_GW otbr_lwip_hook_nd6_get_gw

#ifdef __cplusplus
}
#endif

#endif /* _LWIP_DEFAULT_HOOKS_H_ */
