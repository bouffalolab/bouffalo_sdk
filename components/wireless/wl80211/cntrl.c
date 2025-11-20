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

    memset(WG.ssid, 0, sizeof(WG.ssid));
    strncpy((char *)WG.ssid, (char *)ssid, 32);
    break;
  }

  case WL80211_CTRL_STA_SET_BSSID: {
    uint8_t *bssid = va_arg(args, uint8_t *);
    if (bssid == NULL) {
      return -1;
    }

    memcpy(WG.bssid, bssid, 6);
    break;
  }

  case WL80211_CTRL_STA_SET_PASSWORD: {
    uint8_t *password = va_arg(args, uint8_t *);
    if (password == NULL) {
      return -1;
    }

    memset(WG.password, 0, sizeof(WG.password));
    strncpy((char *)WG.password, (char *)password, 64);
    break;
  }

  case WL80211_CTRL_STA_SET_FREQ: {
    int freq = va_arg(args, int);
    if (freq && wl80211_freq_valid_check(freq)) {
      return -1;
    }

    WG.freq = (uint16_t)freq;
    break;
  }

  case WL80211_CTRL_STA_SET_PMF: {
    int pmf = va_arg(args, int);
    if (pmf < 0 || pmf > 2) {
      return -1;
    }

    WG.pmf_cfg = (uint8_t)pmf;
    break;
  }

  case WL80211_CTRL_SCAN: {
    memset(&WG.scan_params, 0, sizeof(struct wl80211_scan_params));
    WG.scanning = 1;
    return wl80211_mac_do_scan();
  }

  case WL80211_CTRL_SCAN_PARAMS: {
    struct wl80211_scan_params *params = va_arg(args, struct wl80211_scan_params *);
    if (params == NULL) {
      return -1;
    }

    memcpy(&WG.scan_params, params, sizeof(struct wl80211_scan_params));
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

  case WL80211_CTRL_STA_GET_MAC: {
    if (macsw_sta_vif == NULL) {
      return -1;
    }

    uint8_t *mac = va_arg(args, uint8_t *);
    if (mac == NULL) {
      return -1;
    }

    struct mac_addr mac_addr;
    mac_vif_get_mac_addr(macsw_sta_vif, &mac_addr);
    memcpy(mac, mac_addr.array, 6);
    return 0;
  }

  case WL80211_CTRL_STA_GET_RSSI: {
    if (macsw_sta_vif == NULL || wl80211_glb.link_up == 0) {
      return -1;
    }

    int *rssi = va_arg(args, int *);
    if (rssi == NULL) {
      return -1;
    }

    struct mac_addr mac_bssid;
    uint16_t mac_aid;
    int8_t mac_rssi;
    mac_vif_get_sta_status(macsw_sta_vif, &mac_bssid, &mac_aid, &mac_rssi);
    *rssi = mac_rssi;
    return 0;
  }
  }

  return 0;
}
