#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macsw/wl80211_mac.h"
#define WG wl80211_glb

extern uint8_t macsw_sta_vif_idx;

void wl80211_mac_disconnect(uint16_t reason_code, uint16_t status_code) {
  struct sm_disconnect_req *req = malloc(sizeof(struct sm_disconnect_req));

  req->reason_code = reason_code;
  req->status_code = status_code;
  req->vif_idx = macsw_sta_vif_idx;

  macif_kmsg_call(-1, NULL, 0, SM_DISCONNECT_REQ, TASK_SM, req,
                  sizeof(struct sm_disconnect_req));
}
