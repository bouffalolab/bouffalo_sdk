#ifndef _WL80211_MAC_H
#define _WL80211_MAC_H

#include <stddef.h>
#include <sys/queue.h>

#include "wl80211.h"

struct wl80211_mac_tx_desc {
  STAILQ_ENTRY(wl80211_mac_tx_desc) entries;

  void *opaque;

  // driver specified descriptor
  // TODO struct net80211 descriptor
};

#endif
