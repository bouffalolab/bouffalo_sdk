#include <stdint.h>

#include "wl80211.h"
#include "timeout.h"

extern void wl80211_mac_init(void);
extern int wl80211_lwip_init_sta(void);

void wl80211_init(void) {
  STAILQ_INIT(&wl80211_glb.scan_ops);

  timeout_init();

  wl80211_mac_init();

  wl80211_lwip_init_sta();
}
