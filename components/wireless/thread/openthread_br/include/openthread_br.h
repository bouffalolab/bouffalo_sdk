#ifndef __OPENTHREAD_BR_H__
#define __OPENTHREAD_BR_H__

#include "otbr_err.h"

#define VERSION_OT_BR_MAJOR 1
#define VERSION_OT_BR_MINOR 6
#define VERSION_OT_BR_PATCH 17

// #define VERSION_OT_BR_EXTRA_INFO "customer-1"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*statChangedCallback_t)(otChangedFlags);
typedef void (*otbr_action_cb_t)(uint32_t arg);

struct netif * otbr_getInfraNetif(void);
struct netif * otbr_getThreadNetif(void);

void otbr_instance_init(void * aBackboneNetif);
void otbr_nat64_init(char *nat64Cidr);
void otbr_instance_routing_init(void);
void otbrInstance_addStateChangedCallback(statChangedCallback_t callback);

void otbr_netif_init(void);
err_t otbr_netif_output6_forward(uint8_t *aBuffer, uint32_t aLength);

bool otbr_netif_request_action(otbr_action_cb_t cb, uint32_t arg);
bool otbr_netif_request_frame_handle(otbr_action_cb_t cb, struct pbuf * p);

#ifdef __cplusplus
}
#endif

#endif /* __OPENTHREAD_BR_H__ */
