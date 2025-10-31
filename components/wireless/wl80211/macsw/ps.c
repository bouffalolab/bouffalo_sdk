#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "macsw/wl80211_mac.h"

int wl80211_mac_set_ps_mode(int enable) {
#if MACSW_POWERSAVE
  struct me_set_ps_mode_req *req = malloc(sizeof(struct me_set_ps_mode_req));
  assert(req != NULL);

  req->ps_state = !!enable;
  if (enable) {
    req->ps_mode = PS_MODE_ON_DYN;
  } else {
    req->ps_mode = PS_MODE_OFF;
  }

  // response ME_SET_PS_MODE_CFM
  macif_kmsg_call(-1, NULL, 0, ME_SET_PS_MODE_REQ, TASK_ME, req,
                  sizeof(struct me_set_ps_mode_req));

  return 0;
#endif
  return -1;
}
