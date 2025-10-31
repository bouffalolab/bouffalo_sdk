#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macsw/wl80211_mac.h"

#define WG wl80211_glb

#include "supplicant_api.h"

// #define DEBUG

#define WPA_CAPABILITY_MFPR BIT(6)
#define WPA_CAPABILITY_MFPC BIT(7)

enum {
  /// Flag indicating whether PMF Capable enabled
  PMF_CAPABLE = CO_BIT(11),
  /// Flag indicating whether PMF Required enabled
  PMF_REQUIRED = CO_BIT(12),
};

#ifdef DEBUG
struct info_element {
  uint8_t type;
  uint8_t payload_len;
  uint8_t payload[];
};

static void dump_ie(uint8_t *buffer, size_t len) {
  struct info_element *ie = (struct info_element *)buffer;
  uint8_t *buffer_end = buffer + len;
  while (ie->payload < buffer_end &&
         ie->payload + ie->payload_len <= buffer_end) {
    printf("ie id: %d(0x%02x), len: %d: ", ie->type, ie->type,
           ie->payload_len + 2);
    for (int i = 0; i < ie->payload_len; i++) {
      printf("%02X ", ie->payload[i]);
    }
    printf("\n");
    /* move to next ie */
    ie = (struct info_element *)(ie->payload + ie->payload_len);
  }
}
#endif

static int wpa_apply_security_policy(sec_proto_t *sec_proto_get,
                                     uint16_t *key_mgmt_get,
                                     uint8_t *pairwise_cipher_get,
                                     uint8_t *group_cipher_get,
                                     uint8_t *mgmt_group_cipher_get,
                                     wifi_wpa_ie_t *wpa_ie,
                                     uint32_t conn_flags) {
  int pairwise_cipher;
  int group_cipher;
  int mgmt_group_cipher;
  int key_mgmt;

  pairwise_cipher = (wpa_ie->pairwise_cipher == WIFI_CIPHER_TYPE_TKIP_CCMP)
                        ? WIFI_CIPHER_TYPE_CCMP
                        : wpa_ie->pairwise_cipher;

  group_cipher = wpa_ie->group_cipher;
  mgmt_group_cipher = wpa_ie->mgmt_group_cipher;

  if (0 == (conn_flags & (PMF_CAPABLE | PMF_REQUIRED))) {
    if (wpa_ie->key_mgmt & WPA_KEY_MGMT_PSK_SHA256) {
      key_mgmt = WPA_KEY_MGMT_PSK_SHA256;
    } else if (wpa_ie->key_mgmt & WPA_KEY_MGMT_PSK) {
      key_mgmt = WPA_KEY_MGMT_PSK;
    } else {
      return -1;
    }
  } else if (conn_flags & PMF_REQUIRED) {
    if (wpa_ie->key_mgmt & WPA_KEY_MGMT_SAE) {
      key_mgmt = WPA_KEY_MGMT_SAE;
    } else if (wpa_ie->key_mgmt & WPA_KEY_MGMT_PSK_SHA256) {
      key_mgmt = WPA_KEY_MGMT_PSK_SHA256;
    } else {
      return -1;
    }
  } else {
    if (wpa_ie->key_mgmt & WPA_KEY_MGMT_SAE) {
      key_mgmt = WPA_KEY_MGMT_SAE;
    } else if (wpa_ie->key_mgmt & WPA_KEY_MGMT_PSK_SHA256) {
      key_mgmt = WPA_KEY_MGMT_PSK_SHA256;
    } else if (wpa_ie->key_mgmt & WPA_KEY_MGMT_PSK) {
      key_mgmt = WPA_KEY_MGMT_PSK;
    } else {
      return -1;
    }
  }

  *pairwise_cipher_get = pairwise_cipher;
  *group_cipher_get = group_cipher;
  *mgmt_group_cipher_get = mgmt_group_cipher;
  *key_mgmt_get = key_mgmt;

  if (wpa_ie->capabilities & WPA_CAPABILITY_MFPC) {
    if (wpa_ie->capabilities & WPA_CAPABILITY_MFPR ||
        key_mgmt == WPA_KEY_MGMT_SAE) {
      *mgmt_group_cipher_get = WIFI_CIPHER_TYPE_AES_CMAC128;
    }
  }

  if (key_mgmt == WPA_KEY_MGMT_SAE) {
    *sec_proto_get = SEC_PROTO_WPA3;
  }

  return 0;
}

void _find_wpa_rsn_ie(uint32_t var_part_addr, uint32_t var_part_len,
                            uint32_t *wpa_ie, uint32_t *rsn_ie) {
  uint16_t len;
  // FIXME: FIND WPS IE?
  *wpa_ie = mac_vsie_find(var_part_addr, var_part_len,
                          (uint8_t const *)"\x00\x50\xF2\x01", 4, &len);
  *rsn_ie = mac_ie_find(var_part_addr, var_part_len, MAC_ELTID_RSN_IEEE, &len);
}

static int process_wpa_ie(sec_proto_t *sec_proto, uint16_t *key_mgmt,
                          uint8_t *pairwise_cipher, uint8_t *group_cipher,
                          uint8_t *mgmt_group_cipher, uint32_t var_part_addr,
                          uint32_t var_part_len, uint32_t conn_flags) {
  int ret = 0;
  uint32_t wpa_ie_addr = 0, rsn_ie_addr = 0;
  uint32_t elmt_addr = 0;
  size_t ie_len;
  wifi_wpa_ie_t wpa_ie;

  memset(&wpa_ie, 0, sizeof(wpa_ie));

  _find_wpa_rsn_ie(var_part_addr, var_part_len, &wpa_ie_addr, &rsn_ie_addr);
  if (rsn_ie_addr) {
    elmt_addr = rsn_ie_addr;
    *sec_proto = SEC_PROTO_WPA2;
  } else if (wpa_ie_addr) {
    elmt_addr = wpa_ie_addr;
    *sec_proto = SEC_PROTO_WPA;
  }

  if (elmt_addr) {
    ie_len = co_read8p(elmt_addr + MAC_INFOELT_LEN_OFT) + MAC_INFOELT_INFO_OFT;
    if (wpa_cbs->wpa_parse_wpa_ie((uint8_t *)elmt_addr, ie_len, &wpa_ie)) {
      printf("Parsing WPA IE/RSNE failed. Maybe AP has got a bug\r\n");
      ret = -1;
      goto end;
    }

    if (wpa_apply_security_policy(sec_proto, key_mgmt, pairwise_cipher,
                                  group_cipher, mgmt_group_cipher, &wpa_ie,
                                  conn_flags)) {
      ret = -1;
      goto end;
    }
  } else {
    printf("WPA/RSN IE not found!\r\n");
  }

end:
  return ret;
}

struct macsw_connect_bss_candidate {
  STAILQ_ENTRY(macsw_connect_bss_candidate) entry;
  struct wl80211_scan_ap_ind ind;
  uint8_t *raw_frame;
  uint16_t raw_frame_len;
};

struct macsw_connect_ctx {
  struct wl80211_scan_ops so;
  struct scanu_start_req *scan_req;
  struct sm_connect_req *connect_req;
  STAILQ_HEAD(, macsw_connect_bss_candidate) bss_list;
};

static void scan_start(struct wl80211_scan_ops *o) {
  struct macsw_connect_ctx *ops = (void *)o;

  macif_kmsg_call(-1, NULL, 0, SCANU_START_REQ, TASK_SCANU, ops->scan_req,
                  sizeof(struct scanu_start_req));

  free(ops->scan_req);
  ops->scan_req = NULL;
}

static void scan_ind_cb(struct wl80211_scan_ops *o,
                        struct wl80211_scan_ap_ind *i, uint8_t *raw_frame,
                        uint16_t raw_frame_len) {
  struct macsw_connect_ctx *ops = (void *)o;
  if (i->flags & WL80211_SCAN_AP_FLAGS_CACHED) {
#if 0
    struct mac_hdr *machdr = (void *)i->raw_frame;
    uint16_t frame_control = machdr->fctl;

    /* filter only probe resp */
    if ((frame_control & MAC_FCTRL_TYPE_MASK) != MAC_FCTRL_MGT_T ||
        (frame_control & MAC_FCTRL_SUBT_MASK) != MAC_FCTRL_PROBERSP_ST) {
      return;
    }
#endif

    struct macsw_connect_bss_candidate *bss_info =
        malloc(sizeof(struct macsw_connect_bss_candidate) + raw_frame_len);
    assert(bss_info != NULL);

    uint8_t *raw_frame_ptr = (uint8_t *)(bss_info + 1);
    memcpy(raw_frame_ptr, raw_frame, raw_frame_len);
    memcpy(&bss_info->ind, i, sizeof(struct wl80211_scan_ap_ind));

    /* update raw_frame pointer */
    bss_info->raw_frame = raw_frame_ptr;
    bss_info->raw_frame_len = raw_frame_len;

    STAILQ_INSERT_TAIL(&ops->bss_list, bss_info, entry);
    printf("record ssid: %s, rssi: %d, bssid: %02X:%02X:%02X:%02X:%02X:%02X\n", i->ssid, i->rssi, i->bssid[0], i->bssid[1], i->bssid[2], i->bssid[3], i->bssid[4], i->bssid[5]);
  }
}

/* select best one bss and do connect */
static void scan_done_cb(struct wl80211_scan_ops *o) {
  struct macsw_connect_ctx *ops = (void *)o;
  struct macsw_connect_bss_candidate *c, *tc, *best = NULL;

  assert(ops->scan_req == NULL);
  assert(ops->connect_req != NULL);

  STAILQ_FOREACH_SAFE(c, &ops->bss_list, entry, tc) {
    if (best == NULL) {
      best = c;
    } else {
      if (c->ind.rssi > best->ind.rssi) {
        best = c;
      }
    }
  }

  /* save best one, free others */
  if (best) {
    STAILQ_REMOVE(&ops->bss_list, best, macsw_connect_bss_candidate, entry);
  }
  STAILQ_FOREACH_SAFE(c, &ops->bss_list, entry, tc) {
    STAILQ_REMOVE(&ops->bss_list, c, macsw_connect_bss_candidate, entry);
    free(c);
  }

  if (best) {
    printf("select best rssi %d to connect\n", best->ind.rssi);
  } else {
    printf("network not found\n");
    goto _EXIT;
  }

  wifi_connect_parm_t parm;
  struct mac_addr vif_mac;
  uint32_t var_part_addr, var_part_len;
  struct bcn_frame const *frm = (struct bcn_frame const *)best->raw_frame;

  var_part_addr = (uint32_t)frm->variable;
  var_part_len = best->raw_frame_len - MAC_BEACON_VARIABLE_PART_OFT;

  memset(&parm, 0, sizeof(parm));
  parm.vif_idx = macsw_sta_vif_idx;

  mac_vif_get_mac_addr(vif_info_get_vif(parm.vif_idx), &vif_mac);
  memcpy(parm.mac, vif_mac.array, ETH_ALEN);
  memcpy(parm.bssid, best->ind.bssid, ETH_ALEN);

#ifdef DEBUG
  printf("=== ie ===\n");
  dump_ie((uint8_t *)var_part_addr, var_part_len);
  printf("=== ie ===\n");
#endif

  uint32_t conn_flags = 0;

#if defined(MACSW_MFP) && MACSW_MFP
  conn_flags |= PMF_CAPABLE;
  if (ops->connect_req->flags & MFP_IN_USE) {
    conn_flags |= PMF_REQUIRED;
  }
#endif

  if (!process_wpa_ie(&parm.proto, &parm.key_mgmt, &parm.pairwise_cipher,
                      &parm.group_cipher, &parm.mgmt_group_cipher,
                      var_part_addr, var_part_len, conn_flags) &&
      (parm.proto == SEC_PROTO_WPA || parm.proto == SEC_PROTO_WPA2 ||
       parm.proto == SEC_PROTO_WPA3)) {

    if (parm.proto == SEC_PROTO_WPA3) {
      parm.pmf_required = 1;
    }
    wpa_cbs->wpa_sta_config(&parm);
  } else {
    parm.proto = SEC_PROTO_NONE;
  }

  /* update connect param */
  ops->connect_req->chan.freq = best->ind.freq;
  MAC_ADDR_CPY(&ops->connect_req->bssid, &best->ind.bssid);

  ops->connect_req->wpa = 0;
  if (parm.proto == SEC_PROTO_NONE) {
    printf("use sec proto: Open\n");
    ops->connect_req->auth_type = MAC_AUTH_ALGO_OPEN;
  } else {
    if (parm.proto == SEC_PROTO_WEP_STATIC) {
      ops->connect_req->auth_type = MAC_AUTH_ALGO_SHARED;
    } else if (parm.proto == SEC_PROTO_WPA) {
      printf("use sec proto: WPA\n");
      ops->connect_req->wpa = WPA_PROTO_WPA;
      ops->connect_req->flags |= WPA_WPA2_IN_USE;
      ops->connect_req->auth_type = MAC_AUTH_ALGO_OPEN;
    } else if (parm.proto == SEC_PROTO_WPA2) {
      printf("use sec proto: WPA2/RSN\n");
      ops->connect_req->wpa = WPA_PROTO_RSN;
      ops->connect_req->flags |= WPA_WPA2_IN_USE;
      ops->connect_req->auth_type = MAC_AUTH_ALGO_OPEN;
    } else if (parm.proto == SEC_PROTO_WPA3) {
      printf("use sec proto: WPA3\n");
      ops->connect_req->wpa = WPA_PROTO_RSN;
      ops->connect_req->flags |= WPA_WPA2_IN_USE | MFP_IN_USE;
      ops->connect_req->auth_type = MAC_AUTH_ALGO_SAE;
    } else if (parm.proto == SEC_PROTO_WAPI) {
      printf("unsupport sec proto: wapi\n");
      goto _EXIT;
    } else {
      printf("unkonwn sec proto\n");
      goto _EXIT;
    }
    printf("use sec type: PTK=%s GTK=%s\n", bl_wifi_cipher_to_str(parm.pairwise_cipher), bl_wifi_cipher_to_str(parm.group_cipher));
  }

  // FIXME generate by wpa supplicant, remove this ugly code
  extern uint8_t *g_assoc_rsn_ie;
  extern uint16_t g_assoc_rsn_ie_len;

#ifdef DEBUG
  // ref: ieee802_11_defs.h @ Information Element IDs
  dump_ie((uint8_t *)g_assoc_rsn_ie, g_assoc_rsn_ie_len);
#endif
  ops->connect_req->ie_len = g_assoc_rsn_ie_len;

  /* start connect */
  macif_kmsg_call_t(-1, NULL, 0, SM_CONNECT_REQ, TASK_SM, 2, ops->connect_req,
                    sizeof(struct sm_connect_req), g_assoc_rsn_ie,
                    g_assoc_rsn_ie_len);
_EXIT:
  if (best) {
    free(best);
  }
  free(ops->connect_req);
  free(ops);
}

// select bss before connect
static void scan_bss(struct macsw_connect_ctx *ctx) {
  struct scanu_start_req *req = malloc(sizeof(struct scanu_start_req));
  assert(req != NULL);

  struct wl80211_scan_ops *last_ops;

  assert(ctx != NULL);
  STAILQ_INIT(&ctx->bss_list);

  int scan_param_passive = 0;
  uint32_t scan_param_duration = 0;
  uint32_t scan_param_probe_cnt = 0;
  int i, probe_interval;

  // FIXME: SCAN_PASSIVE_DURATION (220000) -> 215*TU
  probe_interval = (scan_param_duration ? scan_param_duration : 215) /
                   (scan_param_probe_cnt ? scan_param_probe_cnt : 2);
  assert(probe_interval >= 20);

  // TODO support 5G
  memcpy(&req->chan[0], _macsw_chan_def.chan2G4,
         _macsw_chan_def.chan2G4_cnt * sizeof(struct mac_chan_def));
#if 0
  memcpy(&req->chan[chan_def.chan2G4_cnt], _macsw_chan_def.chan5G,
         _macsw_chan_def.chan5G_cnt * sizeof(struct mac_chan_def));
  req->chan_cnt = _macsw_chan_def.chan2G4_cnt + _macsw_chan_def.chan5G_cnt;
#else
  req->chan_cnt = _macsw_chan_def.chan2G4_cnt;
#endif

  if (scan_param_passive) {
    for (i = 0; i < req->chan_cnt; i++)
      req->chan[i].flags |= CHAN_NO_IR;
  }

  req->ssid_cnt = 1;
  memcpy(req->ssid->array, WG.ssid, 32);
  req->ssid->length = strlen((char *)WG.ssid);

  assert(req->ssid->length != 0);

  struct mac_addr empty_bssid = {{0, 0, 0}};

  // install bssid
  if (memcmp(WG.bssid, empty_bssid.array, 6) == 0) {
    struct mac_addr broadcast = {{0xffff, 0xffff, 0xffff}};
    MAC_ADDR_CPY(&req->bssid, &broadcast);
  } else {
    MAC_ADDR_CPY(&req->bssid, WG.bssid);
  }

  req->add_ies = (uintptr_t)NULL;
  req->add_ie_len = 0;
  req->vif_idx = macsw_sta_vif_idx;
  req->no_cck = 0;
  req->duration = scan_param_duration * TU_DURATION;
  req->scan_only = true;
  req->probe_cnt = scan_param_probe_cnt;
  req->flags = 0;

  ctx->scan_req = req;
  ctx->so.scan_start = scan_start;
  ctx->so.scan_ap_ind = scan_ind_cb;
  ctx->so.scan_done_ind = scan_done_cb;

  // add req to scan operation list
  GLOBAL_INT_DISABLE();
  last_ops = STAILQ_FIRST(&WG.scan_ops);
  STAILQ_INSERT_TAIL(&WG.scan_ops, &ctx->so, entry);
  GLOBAL_INT_RESTORE();

  /* start scan immediately */
  if (last_ops == NULL) {
    ctx->so.scan_start(&ctx->so);
  }
}

int wl80211_mac_do_connect(void) {
  struct macsw_connect_ctx *ctx = malloc(sizeof(struct macsw_connect_ctx));
  struct sm_connect_req *req = malloc(sizeof(struct sm_connect_req));
  assert(ctx != NULL && req != NULL);

  ctx->connect_req = req;
  memset(req, 0, sizeof(*req));

  struct mac_ssid ssid;
  ssid.length = strlen((char *)WG.ssid);
  memcpy(ssid.array, WG.ssid, ssid.length);
  req->ssid = ssid;

  req->ctrl_port_ethertype = 0x8E88;

  req->listen_interval = 0;
  req->dont_wait_bcmc = 0; // TODO ?
  req->mode = WIFI_MODE_BGNAX;

  req->uapsd_queues = 0;

  req->vif_idx = macsw_sta_vif_idx;

  // start scan
  scan_bss(ctx);

  return 0;
}

int wl80211_mac_set_key(uint8_t key_idx, uint8_t sta_idx, uint8_t *key,
                        uint8_t key_len, uint8_t cipher_suite, bool pairwise) {
  struct mm_key_add_req *req = malloc(sizeof(struct mm_key_add_req));

  memset(req, 0, sizeof(struct mm_key_add_req));
  req->key_idx = key_idx;
  req->sta_idx = sta_idx;

  if (key && key_len > 0)
    memcpy(req->key.array, key, key_len);
  req->key.length = key_len;

  req->cipher_suite = cipher_suite;
  req->inst_nbr = macsw_sta_vif_idx;
  req->spp = 0;
  req->pairwise = pairwise;

  macif_kmsg_call(-1, NULL, 0, MM_KEY_ADD_REQ, TASK_MM, req,
                  sizeof(struct mm_key_add_req));

  free(req);
  return 0;
}

int wl80211_mac_ctrl_port(uint8_t sta_id, int control_port_open) {
  struct me_set_control_port_req *req =
      malloc(sizeof(struct me_set_control_port_req));

  req->sta_idx = sta_id;
  req->control_port_open = control_port_open;

  macif_kmsg_call(-1, NULL, 0, ME_SET_CONTROL_PORT_REQ, TASK_ME, req,
                  sizeof(struct me_set_control_port_req));

  free(req);
  return 0;
}
