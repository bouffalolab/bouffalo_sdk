#include <lwip/tcpip.h>
#include "macsw/wl80211_mac.h"

_Static_assert(sizeof(struct wl80211_mac_tx_desc) <=
                   PBUF_LINK_ENCAPSULATION_HLEN,
               "tx_desc too large");

_Static_assert(sizeof(struct rx_info) >= sizeof(struct pbuf_custom),
               "pbuf_custom too large");
