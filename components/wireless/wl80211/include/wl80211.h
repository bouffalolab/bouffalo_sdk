#ifndef __WL80211_H__
#define __WL80211_H__

#include <stdint.h>
#include <sys/queue.h>
#include <sys/tree.h>

#define INVARIANTS

#ifndef container_of
#define container_of(ptr, type, member)                                        \
  ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

#define ALIGN4_HI(val) (((val) + 3) & ~3)

#define IOVEC_DEFINED
struct iovec {
  void *iov_base;
  size_t iov_len;
};

struct wl80211_scan_ap_ind {
  char ssid[33];
  uint8_t bssid[6];
  uint16_t freq;
  int8_t rssi;
#define WL80211_SCAN_AP_FLAGS_CACHED 0x1
  uint8_t flags;
};

struct wl80211_scan_result_item {
  RB_ENTRY(wl80211_scan_result_item) entry;
  uint8_t bssid[6];
  const char *ssid; // by strdup
  uint8_t band;
  uint8_t channel;
  int8_t rssi;
#define WL80211_SCAN_AP_RESULT_FLAGS_HAS_RSN 0x1
#define WL80211_SCAN_AP_RESULT_FLAGS_HAS_WPA 0x2
#define WL80211_SCAN_AP_RESULT_FLAGS_HAS_CCMP 0x4
#define WL80211_SCAN_AP_RESULT_FLAGS_HAS_TKIP 0x8
#define WL80211_SCAN_AP_RESULT_FLAGS_HAS_WPS 0x10
  uint8_t flags;

  uint32_t key_mgmt : 20;
  uint32_t mode : 16;
};

int bssidcmp(struct wl80211_scan_result_item *e1,
                    struct wl80211_scan_result_item *e2);

/* scan result tree */
RB_HEAD(_scan_result_tree, wl80211_scan_result_item);
extern struct _scan_result_tree wl80211_scan_result;
RB_PROTOTYPE(_scan_result_tree, wl80211_scan_result_item, entry, bssidcmp);

struct wl80211_scan_ops {
  STAILQ_ENTRY(wl80211_scan_ops) entry;
  void (*scan_start)(struct wl80211_scan_ops *);
  void (*scan_ap_ind)(struct wl80211_scan_ops *, struct wl80211_scan_ap_ind *,
                      uint8_t *, uint16_t);
  void (*scan_done_ind)(struct wl80211_scan_ops *);
};

/* wl80211 global state */
struct wl80211_global_state {
  unsigned int scanning : 1;
  unsigned int associated : 1;
  unsigned int authenticating : 1;
  unsigned int link_up : 1;

  uint8_t ssid[33];
  uint8_t password[65];
  uint8_t bssid[6];
  uint16_t freq;
  uint16_t aid;
  STAILQ_HEAD(, wl80211_scan_ops) scan_ops;
};
extern struct wl80211_global_state wl80211_glb;

void wl80211_init(void);
void wl80211_tcpip_input(void *rxhdr, void *buf, uint32_t frm_len,
                         uint32_t status);
int wl80211_output_raw(void *buffer, uint16_t len, unsigned int flags,
                       void (*cb)(void *), void *opaque);

enum {
  WL80211_EVT_SCAN_DONE,
  WL80211_EVT_CONNECTED,
  WL80211_EVT_DISCONNECTED,
};

// post wifi event in async
void wl80211_post_event(int code1, int code2);

// wl80211 control command
enum {
  WL80211_CTRL_SCAN, // TODO add param
  WL80211_CTRL_STA_SET_BSSID,
  WL80211_CTRL_STA_SET_SSID,
  WL80211_CTRL_STA_SET_PSK_PMK,  // PMK Caching
  WL80211_CTRL_STA_SET_PASSWORD, // param akm, passwd, generate pmk
  WL80211_CTRL_STA_CONNECT,      // param freq flags
  WL80211_CTRL_STA_SET_PS_MODE,  // set wifi ps mode
  WL80211_CTRL_STA_DISCONNECT,
};

// wl80211 control api
// return 0 : ok, negative : fail
int wl80211_cntrl(int cmd, ...);
#endif
