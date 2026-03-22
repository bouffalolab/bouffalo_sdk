#ifndef __ELOOP_RTOS_H
#define __ELOOP_RTOS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtos_al.h>
#include <string.h>

enum {
    ELOOP_EVT_WPA_IFACE_CTRL = 1,
    ELOOP_EVT_WPA_DRV_CTRL   = 2,
    ELOOP_EVT_WPA_L2_DATA    = 3,     /* Legacy: shared event ID (fallback) */
    /* Per-interface L2 data events for multi-interface support */
    ELOOP_EVT_WPA_L2_DATA_WL1 = 10,   /* wl1 interface */
    ELOOP_EVT_WPA_L2_DATA_WL2 = 11,   /* wl2 interface */
};

struct event_sync_msg {
  rtos_semaphore done;
  char **resp;
  int *resp_len;
};

/* Helper to get event ID for interface name (wl1/wl2) */
static inline int eloop_get_l2_event_id(const char *ifname) {
    if (ifname == NULL) {
        return ELOOP_EVT_WPA_L2_DATA;  /* fallback for unknown interfaces */
    }
    /* Current system only has wl1 and wl2 interfaces */
    if (strcmp(ifname, "wl1") == 0) {
        return ELOOP_EVT_WPA_L2_DATA_WL1;
    } else if (strcmp(ifname, "wl2") == 0) {
        return ELOOP_EVT_WPA_L2_DATA_WL2;
    }
    /* Unknown interface (should not happen in current system) */
    return ELOOP_EVT_WPA_L2_DATA;
}

#ifdef __cplusplus
}
#endif

/* __ELOOP_RTOS_H */
#endif
