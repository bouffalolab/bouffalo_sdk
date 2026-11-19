#ifndef __RFPARAM_ADAPTER_INTERNAL_H__
#define __RFPARAM_ADAPTER_INTERNAL_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SDK-internal RF initialization entry.
 *
 * SYSINIT, the WiFi4/FHOST startup tasks and the board RF reconfiguration
 * code are the only callers. Applications must not call this function; their
 * legacy rfparam_init() entry is an empty compatibility wrapper.
 */
int32_t _rfparam_init(uint32_t base_addr, void *rf_para, uint32_t apply_flag);

#ifdef __cplusplus
}
#endif

#endif /* __RFPARAM_ADAPTER_INTERNAL_H__ */
