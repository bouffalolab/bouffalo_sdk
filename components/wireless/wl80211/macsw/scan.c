#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macsw/wl80211_mac.h"

#include "supplicant_api.h"

#define WG wl80211_glb

struct macsw_scan_ops {
  struct wl80211_scan_ops o;
  struct scanu_start_req *req;
};

int bssidcmp(struct wl80211_scan_result_item *e1,
             struct wl80211_scan_result_item *e2) {
  return memcmp(e1->bssid, e2->bssid, 6);
}

// scan result tree
struct _scan_result_tree wl80211_scan_result =
    RB_INITIALIZER(&wl80211_scan_result);
RB_GENERATE(_scan_result_tree, wl80211_scan_result_item, entry, bssidcmp);

static void scan_start(struct wl80211_scan_ops *o) {
  struct macsw_scan_ops *ops = (void *)o;
  macif_kmsg_call(-1, NULL, 0, SCANU_START_REQ, TASK_SCANU, ops->req,
                  sizeof(struct scanu_start_req));
  free(ops->req);
  ops->req = NULL;
}

/* called when beacon or probe response received */
static void scan_ind_cb(struct wl80211_scan_ops *o,
                        struct wl80211_scan_ap_ind *i, uint8_t *raw_frame,
                        uint16_t raw_frame_len) {
  uint32_t var_part_addr, var_part_len;
  struct bcn_frame const *frm = (struct bcn_frame const *)raw_frame;
  uint32_t wpa_ie_addr = 0, rsn_ie_addr = 0;
  uint32_t elmt_addr = 0;
  wifi_wpa_ie_t wpa_ie;
  uint16_t len;
  bool has_he, /*has_vht, */ has_ht, has_ext_rates;

  struct wl80211_scan_result_item *ap_item, *ap_item_orig;
  ap_item = malloc(sizeof(struct wl80211_scan_result_item));
  assert(ap_item != NULL);

  var_part_addr = (uint32_t)frm->variable;
  var_part_len = raw_frame_len - MAC_BEACON_VARIABLE_PART_OFT;

  ap_item->flags = 0;
  _find_wpa_rsn_ie(var_part_addr, var_part_len, &wpa_ie_addr, &rsn_ie_addr);
  if (wpa_ie_addr) {
    ap_item->flags |= WL80211_SCAN_AP_RESULT_FLAGS_HAS_WPA;
    elmt_addr = wpa_ie_addr;
  }
  if (rsn_ie_addr) {
    ap_item->flags |= WL80211_SCAN_AP_RESULT_FLAGS_HAS_RSN;
    elmt_addr = rsn_ie_addr;
  }

  /* set prime key and insert tree */
  memcpy(ap_item->bssid, i->bssid, 6);

  ap_item_orig = RB_INSERT(_scan_result_tree, &wl80211_scan_result, ap_item);
  if (ap_item_orig == NULL) { /* new bssid first seen */
    if (elmt_addr) {
      size_t ie_len;
      ie_len =
          co_read8p(elmt_addr + MAC_INFOELT_LEN_OFT) + MAC_INFOELT_INFO_OFT;
      if (wpa_cbs->wpa_parse_wpa_ie((uint8_t *)elmt_addr, ie_len, &wpa_ie)) {
        printf("Parsing WPA IE/RSNE failed. Maybe AP has got a bug\n");
      }
      ap_item->key_mgmt = wpa_ie.key_mgmt;

#ifdef INVARIANTS
      if (wpa_ie.proto == WPA_PROTO_WPA) {
        assert((ap_item->flags & WL80211_SCAN_AP_RESULT_FLAGS_HAS_RSN) == 0 &&
               (ap_item->flags & WL80211_SCAN_AP_RESULT_FLAGS_HAS_WPA));
      } else if (wpa_ie.proto == WPA_PROTO_RSN) {
        assert((ap_item->flags & WL80211_SCAN_AP_RESULT_FLAGS_HAS_RSN));
      }
#endif

      int ciphers[2] = {wpa_ie.pairwise_cipher, wpa_ie.group_cipher};
      int j;

      for (j = 0; j < sizeof(ciphers) / sizeof(ciphers[0]); ++j) {
        int cipher = ciphers[j];
        if (cipher == WIFI_CIPHER_TYPE_TKIP) {
          ap_item->flags |= WL80211_SCAN_AP_RESULT_FLAGS_HAS_TKIP;
        }
        if (cipher == WIFI_CIPHER_TYPE_CCMP) {
          ap_item->flags |= WL80211_SCAN_AP_RESULT_FLAGS_HAS_CCMP;
        }
        if (cipher == WIFI_CIPHER_TYPE_TKIP_CCMP) {
          ap_item->flags |= WL80211_SCAN_AP_RESULT_FLAGS_HAS_CCMP;
          ap_item->flags |= WL80211_SCAN_AP_RESULT_FLAGS_HAS_TKIP;
        }
      }
    } else {
      ap_item->key_mgmt = 0;
    }

    ap_item->rssi = i->rssi;

    if (i->ssid[0]) {
      ap_item->ssid = strdup(i->ssid);
    } else {
      ap_item->ssid = NULL;
    }

    // FIXME support 5g
    ap_item->band = 0;
    ap_item->channel = phy_freq_to_channel(0, i->freq);

    if (mac_vsie_find(var_part_addr, var_part_len,
                      (const uint8_t *)"\x00\x50\xF2\x04", 4, &len)) {
      ap_item->flags |= WL80211_SCAN_AP_RESULT_FLAGS_HAS_WPS;
    }

    has_he = mac_ext_ie_find(var_part_addr, var_part_len, MAC_ELTID_EXT_HE_CAPA,
                             &len);
    // has_vht = mac_ie_find(var_part_addr, var_part_len, MAC_ELTID_VHT_CAPA,
    // &len);
    has_ht = mac_ie_find(var_part_addr, var_part_len, MAC_ELTID_HT_CAPA, &len);
    has_ext_rates =
        mac_ie_find(var_part_addr, var_part_len, MAC_ELTID_EXT_RATES, &len);

    if (has_he) {
      // WiFi 6 (802.11ax) 2.4G
      if (has_ht) {
        ap_item->mode = WIFI_MODE_BGNAX;
      } else {
        ap_item->mode = WIFI_MODE_BGAX;
      }
    } else if (has_ht) {
      // WiFi 4 (802.11n) 2.4G
      ap_item->mode = WIFI_MODE_BGN;
    } else if (has_ext_rates) {
      // 802.11g
      ap_item->mode = WIFI_MODE_BG;
    } else {
      // Legacy 802.11b
      ap_item->mode = WIFI_MODE_B;
    }
  } else {
    /* record exist, update rssi, and free the new one */
    ap_item_orig->rssi = i->rssi;
    free(ap_item);
  }
}

static void scan_done_cb(struct wl80211_scan_ops *o) {
  struct macsw_scan_ops *ops = (void *)o;

  assert(ops->req == NULL);
  free(ops);

  wl80211_post_event(WL80211_EVT_SCAN_DONE, 0);
}

int wl80211_mac_do_scan(void) {
  struct scanu_start_req *req = malloc(sizeof(struct scanu_start_req));
  struct macsw_scan_ops *ops = malloc(sizeof(struct macsw_scan_ops));
  struct wl80211_scan_ops *last_ops;
  assert(req != NULL && ops != NULL);

  int scan_param_passive = WG.scan_params.passive;
  uint32_t scan_param_duration = WG.scan_params.duration;
  uint32_t scan_param_probe_cnt = WG.scan_params.probe_cnt;
  int i, probe_interval;
  uint8_t channel;
  int freq;
  struct mac_chan_def *chan;
  int nb_chans = 0;

  memset(req, 0, sizeof(struct scanu_start_req));
  // FIXME: SCAN_PASSIVE_DURATION (220000) -> 215*TU
  probe_interval = (scan_param_duration ? scan_param_duration : 215) /
                   (scan_param_probe_cnt ? scan_param_probe_cnt : 2);
  if (probe_interval < 20) {
    printf("probe_interval (%d)ms less than 20ms\r\n", probe_interval);
    goto EXIT;
  }

  if (WG.scan_params.channels_cnt) {
    for (i = 0; i < WG.scan_params.channels_cnt && nb_chans < SCAN_CHANNEL_MAX; i++) {
      channel = WG.scan_params.channels[i];
      if (!wl80211_channel_valid_check(channel)) {
        if (channel > MAC_DOMAINCHANNEL_24G_MAX) {
          freq = phy_channel_to_freq(PHY_BAND_5G, channel);
        } else {
          freq = phy_channel_to_freq(PHY_BAND_2G4, channel);
        }
        chan = wl80211_mac_chan_get(freq);
        if (chan) {
          memcpy(&req->chan[nb_chans++], chan, sizeof(struct mac_chan_def));
        }
      }
    }
  } else {
    uint8_t channel24G_num;
    uint8_t channel5G_num;
    uint8_t *channel24G_list;
    uint8_t *channel5G_list;

    wl80211_get_channel_nums(wl80211_glb.country_code, (uint8_t *)&channel24G_num, (uint8_t *)&channel5G_num);
    wl80211_get_channel_list(wl80211_glb.country_code, &channel24G_list, &channel5G_list);

    for (i = 0; i < channel24G_num && nb_chans < SCAN_CHANNEL_MAX; i++) {
      channel = channel24G_list[i];
      freq = phy_channel_to_freq(PHY_BAND_2G4, channel);
      chan = wl80211_mac_chan_get(freq);
      if (chan) {
        memcpy(&req->chan[nb_chans++], chan, sizeof(struct mac_chan_def));
      }
    }
    for (i = 0; i < channel5G_num && nb_chans < SCAN_CHANNEL_MAX; i++) {
      channel = channel5G_list[i];
      freq = phy_channel_to_freq(PHY_BAND_5G, channel);
      chan = wl80211_mac_chan_get(freq);
      if (chan) {
        memcpy(&req->chan[nb_chans++], chan, sizeof(struct mac_chan_def));
      }
    }
  }
  assert(nb_chans > 0);
  req->chan_cnt = nb_chans;

  if (scan_param_passive) {
    for (i = 0; i < req->chan_cnt; i++)
      req->chan[i].flags |= CHAN_NO_IR;
  }

  req->ssid_cnt = 1;
  if (WG.scan_params.ssid_length <= MAC_SSID_LEN) {
    memcpy(req->ssid[0].array, WG.scan_params.ssid, WG.scan_params.ssid_length);
    req->ssid[0].length = WG.scan_params.ssid_length;
  }

  struct mac_addr empty_bssid = {{0, 0, 0}};

  // install bssid
  if (memcmp(WG.scan_params.bssid, empty_bssid.array, 6) == 0) {
    struct mac_addr broadcast = {{0xffff, 0xffff, 0xffff}};
    MAC_ADDR_CPY(&req->bssid, &broadcast);
  } else {
    MAC_ADDR_CPY(&req->bssid, WG.scan_params.bssid);
  }

  req->add_ies = (uintptr_t)NULL;
  req->add_ie_len = 0;
  req->vif_idx = macsw_sta_vif_idx;
  req->no_cck = 0;
  req->duration = scan_param_duration * TU_DURATION;
  req->scan_only = true;
  req->probe_cnt = scan_param_probe_cnt;
  req->flags = 0;

  memset(ops, 0, sizeof(*ops));
  ops->req = req;
  ops->o.scan_start = scan_start;
  ops->o.scan_ap_ind = scan_ind_cb;
  ops->o.scan_done_ind = scan_done_cb;
  ops->o.is_connecting_scan = 0;

  // add req to scan operation list
  GLOBAL_INT_DISABLE();
  last_ops = STAILQ_FIRST(&WG.scan_ops);
  STAILQ_INSERT_TAIL(&WG.scan_ops, &ops->o, entry);
  GLOBAL_INT_RESTORE();

  /* start scan immediately */
  if (last_ops == NULL) {
    ops->o.scan_start(&ops->o);
  }

  return 0;

EXIT:
  // FIXME
  free(req);
  free(ops);
  return 0;
}
