#ifndef __ELOOP_RTOS_H
#define __ELOOP_RTOS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtos_al.h>

enum {
    ELOOP_EVT_WPA_IFACE_CTRL = 1,
    ELOOP_EVT_WPA_DRV_CTRL   = 2,
    ELOOP_EVT_WPA_L2_DATA    = 3,
};

struct event_sync_msg {
  rtos_semaphore done;
  char **resp;
  int *resp_len;
};

#ifdef __cplusplus
}
#endif

/* __ELOOP_RTOS_H */
#endif
