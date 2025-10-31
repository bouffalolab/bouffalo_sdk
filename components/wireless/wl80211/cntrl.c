#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wl80211_mac.h"

struct wl80211_global_state wl80211_glb;
#define WG wl80211_glb

int wl80211_cntrl(int cmd, ...) {
  va_list args;

  va_start(args, cmd);
  switch (cmd) {
  default:
    printf("%s: unknow cmd: %d\n", __func__, cmd);
    return -1;

  case WL80211_CTRL_STA_SET_SSID: {
    uint8_t *ssid = va_arg(args, uint8_t *);
    if (ssid == NULL) {
      return -1;
    }
    strncpy((char *)WG.ssid, (char *)ssid, 32);
    break;
  }

  case WL80211_CTRL_STA_SET_BSSID: {
    uint8_t *bssid = va_arg(args, uint8_t *);
    if (bssid == NULL) {
      return -1;
    }
    memcpy(WG.bssid, bssid, 6);
  }

  case WL80211_CTRL_STA_SET_PASSWORD: {
    uint8_t *password = va_arg(args, uint8_t *);
    if (password == NULL) {
      return -1;
    }

    strncpy((char *)WG.password, (char *)password, 64);
    break;
  }

  case WL80211_CTRL_SCAN: {
    WG.scanning = 1;
    return wl80211_mac_do_scan();
  }

  case WL80211_CTRL_STA_CONNECT:
    return wl80211_mac_do_connect();

  case WL80211_CTRL_STA_SET_PS_MODE: {
    int enable = va_arg(args, int);
    return wl80211_mac_set_ps_mode(enable);
  }

  case WL80211_CTRL_STA_DISCONNECT: {
    void wl80211_mac_disconnect(uint16_t reason_code, uint16_t status_code);
    wl80211_mac_disconnect(0, 0);
    return 0;
  }
  }

  return 0;
}
