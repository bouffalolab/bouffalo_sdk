#ifndef __OPENTHREAD_BR_H__
#define __OPENTHREAD_BR_H__

#include <openthread/instance.h>

#include "otbr_err.h"

#define VERSION_OT_BR_MAJOR 1
#define VERSION_OT_BR_MINOR 7
#define VERSION_OT_BR_PATCH 1

// #define VERSION_OT_BR_EXTRA_INFO "customer-1"

#define OTBR_OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE 1
#define OTBR_OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE 1
#define OTBR_OPENTHREAD_CONFIG_IP6_MAX_DATAGRAM_LENGTH 1280
#define OTBR_OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE 1
#define OTBR_OPENTHREAD_CONFIG_MAX_MULTICAST_LISTENERS 75

#ifdef __cplusplus
extern "C" {
#endif

typedef void * otbr_lwip_netif_type_t;
typedef void (*statChangedCallback_t)(otChangedFlags flags);
typedef void (*otbr_action_cb_t)(uint32_t arg);

void otbr_instance_init(void * aBackboneNetif);
void otbr_nat64_init(char *nat64Cidr);
void otbr_instance_routing_init(void);
void otbrInstance_addStateChangedCallback(statChangedCallback_t callback);

void otbr_netif_init(void);
otbrError_t otbr_netif_output(otbr_lwip_netif_type_t aNetif, struct pbuf *aBuffer);

bool otbr_netif_request_action(otbr_action_cb_t cb, uint32_t arg);
bool otbr_netif_request_frame_handle(otbr_action_cb_t cb, struct pbuf * p);

#ifdef __cplusplus
}
#endif

#endif /* __OPENTHREAD_BR_H__ */
