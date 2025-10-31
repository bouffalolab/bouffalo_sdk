#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "macsw.h"
#include "macsw/wl80211_mac.h"

#include "supplicant_api.h"

#define WG wl80211_glb

struct info_element {
#define IE_ID_SSID 0x00
#define IE_ID_DS_CHANNEL 0x03
  uint8_t type;
  uint8_t len;
  uint8_t payload[];
};

struct beacon {
  uint8_t timestamp[8];
  uint16_t beacon_int;
  uint16_t capab_info;
  /* followed by some of SSID, Supported rates,
   * FH Params, DS Params, CF Params, IBSS Params, TIM */
  uint8_t variable[];
} __PACKED;

/* current ap index TODO move to WG */
static uint8_t ap_idx = -1;

void *macsw_sta_vif = NULL;
uint8_t macsw_sta_vif_idx = 0;

static int scan_result_ind(ke_msg_id_t const msgid, void *param,
                           ke_task_id_t const dest_id,
                           ke_task_id_t const src_id) {
  struct rxu_mgt_ind *ind = param;
  struct mac_hdr *mgmt = (struct mac_hdr *)ind->payload;
  struct beacon *beacon = (struct beacon *)(mgmt + 1);
  struct wl80211_scan_ap_ind item;
  struct wl80211_scan_ops *ops;

  GLOBAL_INT_DISABLE();
  ops = STAILQ_FIRST(&(WG.scan_ops));
  GLOBAL_INT_RESTORE();

  assert(ops != NULL);
  assert(ops->scan_ap_ind != NULL);

  struct info_element *found_ie;
  uint16_t found_ie_len;

  found_ie = (void *)(uintptr_t)mac_ie_find(
      (uintptr_t)beacon->variable, ind->length - sizeof(struct mac_hdr),
      IE_ID_DS_CHANNEL, &found_ie_len);

  if (found_ie != NULL) {
    item.freq = phy_channel_to_freq(ind->band, found_ie->payload[0]);
  } else {
    item.freq = ind->center_freq;
  }

  found_ie = (void *)(uintptr_t)mac_ie_find(
      (uintptr_t)beacon->variable, ind->length - sizeof(struct mac_hdr),
      IE_ID_SSID, &found_ie_len);

  if (found_ie != NULL) {
    memset(item.ssid, 0, 33);
    int ie_len = found_ie_len - MAC_INFOELT_INFO_OFT;
    strncpy(item.ssid, (char *)found_ie->payload, ie_len > 32 ? 32 : ie_len);
  } else {
    item.ssid[0] = 0;
  }

  memcpy(item.bssid, mgmt->addr3.array, 6);

  item.flags = 0;
  if (msgid == SCANU_RESULT_IND) {
    item.flags |= WL80211_SCAN_AP_FLAGS_CACHED;
  }

  item.rssi = ind->rssi;

  // call upper beacon handler
  ops->scan_ap_ind(ops, &item, (uint8_t *)mgmt, ind->length);
  return 0;
}

static int scan_cfm(ke_msg_id_t const msgid, void *param,
                    ke_task_id_t const dest_id, ke_task_id_t const src_id) {
  struct wl80211_scan_ops *ops;

  GLOBAL_INT_DISABLE();
  ops = STAILQ_FIRST(&WG.scan_ops);
  assert(ops != NULL);
  STAILQ_REMOVE_HEAD(&WG.scan_ops, entry);
  GLOBAL_INT_RESTORE();

  assert(ops->scan_done_ind != NULL);

  ops->scan_done_ind(ops);
  // ops has been freed

  // launch next scan operation
  GLOBAL_INT_DISABLE();
  ops = STAILQ_FIRST(&WG.scan_ops);
  GLOBAL_INT_RESTORE();

  if (ops != NULL) {
    ops->scan_start(ops);
  }

  return 0;
}

static int connect_ind(ke_msg_id_t const msgid, void *param,
                       ke_task_id_t const dest_id, ke_task_id_t const src_id) {
  struct sm_connect_ind *ind = param;
#if 0
#define D(field, t) printf(#field ":" #t "\r\n", field)
  /* clang-format off */
  D(ind->status_code, %d);
  D(ind->ieeetypes_code, %x);
  D(ind->bssid.array[0], %x);
  D(ind->bssid.array[1], %x);
  D(ind->bssid.array[2], %x);

  D(ind->roamed, %d);
  D(ind->vif_idx, %d);
  D(ind->ap_idx, %d);
  D(ind->ch_idx, %d);
  D(ind->qos, %d);
  D(ind->acm, %d);
  D(ind->assoc_req_ie_len, %d);
  D(ind->assoc_rsp_ie_len, %d);
  D(ind->aid, %d);
  D(ind->chan.center1_freq, %d);
  D(ind->chan.center2_freq, %d);
  D(ind->security, %d); // wpa
  D(ind->ac_param[0], %d);
  D(ind->ac_param[1], %d);
  D(ind->ac_param[2], %d);
  D(ind->ac_param[3], %d);
  /* clang-format on */
#endif

  /* reset global flags */
  WG.authenticating = 0;
  WG.link_up = 0;

  if (g_sae_ctx) {
    free(g_sae_ctx);
    g_sae_ctx = NULL;
  }

  if (ind->status_code) {
    WG.associated = 0;
    wl80211_post_event(WL80211_EVT_DISCONNECTED,
                       (ind->status_code << 16) | (ind->ieeetypes_code));
  } else {
    ap_idx = ind->ap_idx;

    WG.associated = 1;
    WG.aid = ind->aid;
    WG.freq = ind->chan.center1_freq;

    memcpy(WG.bssid, ind->bssid.array, sizeof(WG.bssid));

    if (ind->security == 0) {
      wl80211_mac_ctrl_port(ind->ap_idx, 1);
    } else {
      wifi_connect_parm_t parm;
      memset(&parm, 0, sizeof(parm));
      parm.vif_idx = ind->vif_idx;
      parm.sta_idx = ap_idx;

      memcpy(parm.ssid.ssid, WG.ssid, sizeof(parm.ssid.ssid));
      parm.ssid.len = strlen((char *)WG.ssid);
      memcpy(parm.passphrase, WG.password, sizeof(parm.passphrase));

      wpa_cbs->wpa_sta_connect(&parm);
    }
  }
  return 0;
}

/* add key to keyram cfm */
static int key_add_cfm(ke_msg_id_t const msgid, void *param,
                       ke_task_id_t const dest_id, ke_task_id_t const src_id) {
  struct mm_key_add_cfm *cfm = param;

  if (cfm->status) {
    printf("install key to keyram failed!\n");
    // FIXME? disconnect?
    abort();
  }

  return 0;
}

/* set control port cfm */
static int ctrl_port_cfm(ke_msg_id_t const msgid, void *param,
                         ke_task_id_t const dest_id,
                         ke_task_id_t const src_id) {
  struct me_set_control_port_cfm *cfm = param;

  if (cfm->control_port_open) {
    WG.link_up = 1;
    // connect & link up event, notify upper layer(start DHCP etc.)
    wl80211_post_event(WL80211_EVT_CONNECTED, 0);
  } else {
    WG.link_up = 0;
  }

  return 0;
}

static int disconnect_ind(ke_msg_id_t const msgid, void *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id) {

  struct sm_disconnect_ind *ind = param;

  WG.link_up = 0;
  WG.associated = 0;
  WG.authenticating = 0;
  WG.aid = 0;
  WG.freq = 0;

  if (g_sae_ctx) {
    free(g_sae_ctx);
    g_sae_ctx = NULL;
  }

  wpa_cbs->wpa_sta_disconnected_cb(ind->reason_code);

  memset(WG.bssid, 0, sizeof(WG.bssid));
  if (WG.authenticating && WG.link_up == 0) {
    // deauth when authentication, is password error
    wl80211_post_event(WL80211_EVT_DISCONNECTED,
                       WLAN_FW_AUTHENTICATION_FAIILURE);
  } else {
    wl80211_post_event(WL80211_EVT_DISCONNECTED, ind->reason_code);
  }

  return 0;
}

// TODO set static
struct wl80211_mac_sae_ctx *g_sae_ctx = NULL;

void _sae_send_frame(int auth_seq) {
  size_t sae_msg_len;
  uint8_t *sae_msg_buf;
  struct mac_hdr *buf;
  uint32_t frame;

  assert(g_sae_ctx != NULL);

  sae_msg_buf = wpa_cbs->wpa3_build_sae_msg(
      (uint8_t *)g_sae_ctx->ind.bssid.array,
      (uint8_t *)g_sae_ctx->vif_mac.array, WG.password, auth_seq, &sae_msg_len);

  assert(sae_msg_buf != NULL && sae_msg_len > 0);

  buf = malloc(sizeof(struct mac_hdr) + MAC_AUTH_CHALLENGE_OFT + sae_msg_len);
  buf->fctl = MAC_FCTRL_AUTHENT;
  buf->durid = 0;
  buf->addr1 = g_sae_ctx->ind.bssid;
  buf->addr2 = g_sae_ctx->vif_mac;
  buf->addr3 = g_sae_ctx->ind.bssid;

  frame = (uintptr_t)buf + sizeof(struct mac_hdr);

  co_write16p(frame + MAC_AUTH_ALGONBR_OFT, co_htows(MAC_AUTH_ALGO_SAE));
  co_write16p(frame + MAC_AUTH_SEQNBR_OFT, co_htows(auth_seq));
  co_write16p(frame + MAC_AUTH_STATUS_OFT, co_htows(MAC_ST_SUCCESSFUL));

  frame += MAC_AUTH_CHALLENGE_OFT;
  memcpy((void *)frame, sae_msg_buf, sae_msg_len);

  wl80211_output_raw(
      buf, sizeof(struct mac_hdr) + MAC_AUTH_CHALLENGE_OFT + sae_msg_len,
      WL80211_MAC_TX_FLAG_MGMT, NULL, NULL);

  free(buf);
}

static int external_auth_ind(ke_msg_id_t const msgid, void *param,
                             ke_task_id_t const dest_id,
                             ke_task_id_t const src_id) {
  struct sm_external_auth_required_ind *ind = param;

  if (g_sae_ctx == NULL) {
    g_sae_ctx = malloc(sizeof(struct wl80211_mac_sae_ctx));
    if (g_sae_ctx == NULL) {
      struct sm_external_auth_required_rsp rsp;
      rsp.vif_idx = ind->vif_idx;
      rsp.status = MAC_ST_FAILURE;
      macif_kmsg_call(-1, NULL, 0, SM_EXTERNAL_AUTH_REQUIRED_RSP, TASK_SM, &rsp,
                      sizeof(struct sm_external_auth_required_rsp));
      return 0;
    }
  }

  WG.authenticating = 1;

  mac_vif_get_mac_addr(vif_info_get_vif(ind->vif_idx), &g_sae_ctx->vif_mac);
  g_sae_ctx->ind = *ind;

  _sae_send_frame(MAC_AUTH_FIRST_SEQ);

  return 0;
}

static int credits_update_ind(ke_msg_id_t const msgid, void *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id) {
  extern int8_t macsw_txq_credits[TID_MAX];
  struct me_tx_credits_update_ind *ind = param;

  GLOBAL_INT_DISABLE();
  macsw_txq_credits[ind->tid] += ind->credits;
  GLOBAL_INT_RESTORE();

  return 0;
}

/* MUST BE SORTED */
static const struct ke_msg_handler ke_msg_handlers[] = {
    {MM_KEY_ADD_CFM, (ke_msg_func_t)key_add_cfm},
    {SCANU_START_CFM, (ke_msg_func_t)scan_cfm},
    {SCANU_RESULT_IND, (ke_msg_func_t)scan_result_ind},
    {SCANU_RESULT_UPDATE, (ke_msg_func_t)scan_result_ind},
    {ME_SET_CONTROL_PORT_CFM, (ke_msg_func_t)ctrl_port_cfm},
    {ME_TX_CREDITS_UPDATE_IND, (ke_msg_func_t)credits_update_ind},
    {SM_CONNECT_IND, (ke_msg_func_t)connect_ind},
    {SM_DISCONNECT_IND, (ke_msg_func_t)disconnect_ind},
    {SM_EXTERNAL_AUTH_REQUIRED_IND, (ke_msg_func_t)external_auth_ind},
};

static int comp_ke_msg_id(const void *p1, const void *p2) {
  const struct ke_msg_handler *k1 = p1;
  const struct ke_msg_handler *k2 = p2;

  return k1->id < k2->id ? -1 : (k1->id == k2->id ? 0 : 1);
}

/* this function called by wifi task */
void wl80211_handle_ke_msg(struct ke_msg *ke_msg) {
  struct ke_msg_handler key, *res;

  assert(ke_msg != NULL);
  key.id = ke_msg->id;

  /* search handler */
  res = bsearch(&key, ke_msg_handlers, CO_ARRAY_SIZE(ke_msg_handlers),
                sizeof(struct ke_msg_handler), comp_ke_msg_id);

  // printf("ke_msg task:%d, id:%d\r\n", ke_msg->id >> 10, ke_msg->id);
  if (res != NULL) {
    assert(res->id == ke_msg->id);
    res->func(ke_msg->id, (void *)ke_msg->param, ke_msg->dest_id,
              ke_msg->src_id);
  }
  ke_msg_free(ke_msg);
}

uint8_t _macsw_get_staid(void *vif, struct mac_addr *dst_addr,
                         bool mgmt_frame) {
  uint8_t sta_id = INVALID_STA_IDX;

  switch (mac_vif_get_type(vif)) {
  case VIF_STA:
    if (mac_vif_get_active(vif))
      sta_id = ap_idx;
    break;
  case VIF_AP:
    if (MAC_ADDR_GROUP(dst_addr))
      sta_id = VIF_TO_BCMC_IDX(mac_vif_get_index(vif));
    else
      sta_id = vif_mgmt_get_staid(vif, dst_addr);
    break;
#if MACSW_MESH_EN
  case VIF_MESH_POINT:
    if (MAC_ADDR_GROUP(dst_addr))
      sta_id = VIF_TO_BCMC_IDX(mac_vif_get_index(vif));
    else if (mgmt_frame)
      sta_id = vif_mgmt_get_staid(vif, dst_addr);
    else
      sta_id = fhost_tx_get_mesh_staid(vif, dst_addr);
    break;
#endif // MACSW_MESH_EN
  default:
    break;
  }
  assert(sta_id < STA_MAX || sta_id == INVALID_STA_IDX);

  return sta_id;
}

/* TODO generate at compile time */
static const uint8_t me_config[] = {
    0x21, 0x01, 0x1F, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x48, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x90, 0x03,
    0xFC, 0xFF, 0x41, 0x00, 0xFC, 0xFF, 0x41, 0x00, 0x00, 0x00, 0x02, 0x00,
    0x00, 0x00, 0x00, 0xE0, 0x2B, 0x5B, 0x0D, 0x40, 0xCF, 0x04, 0x02, 0xB0,
    0x00, 0x00, 0xFD, 0xFF, 0xFD, 0xFF, 0xFC, 0xFF, 0xFC, 0xFF, 0xFC, 0xFF,
    0xFC, 0xFF, 0x08, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00};

static const uint8_t mm_start[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/** List of supported Channel */
const struct me_chan_config_req _macsw_chan_def = {
/// define a channel
#define CHAN(_freq, _band, _flags, _pwr)                                       \
  {                                                                            \
      .freq = (_freq),                                                         \
      .band = (_band),                                                         \
      .flags = (_flags),                                                       \
      .tx_power = (_pwr),                                                      \
  }

/// define a channel in 2.4GHz band
#define CHAN_24(_freq, _flag, _pwr) CHAN(_freq, PHY_BAND_2G4, _flag, _pwr)
/// define a channel in 5GHz band
#define CHAN_5(_freq, _flag, _pwr) CHAN(_freq, PHY_BAND_5G, _flag, _pwr)

    .chan2G4_cnt = 14,
    .chan2G4[0] = CHAN_24(2412, 0, 20),
    .chan2G4[1] = CHAN_24(2417, 0, 20),
    .chan2G4[2] = CHAN_24(2422, 0, 20),
    .chan2G4[3] = CHAN_24(2427, 0, 20),
    .chan2G4[4] = CHAN_24(2432, 0, 20),
    .chan2G4[5] = CHAN_24(2437, 0, 20),
    .chan2G4[6] = CHAN_24(2442, 0, 20),
    .chan2G4[7] = CHAN_24(2447, 0, 20),
    .chan2G4[8] = CHAN_24(2452, 0, 20),
    .chan2G4[9] = CHAN_24(2457, 0, 20),
    .chan2G4[10] = CHAN_24(2462, 0, 20),
    .chan2G4[11] = CHAN_24(2467, 0, 20),
    .chan2G4[12] = CHAN_24(2472, 0, 20),
    .chan2G4[13] = CHAN_24(2484, 0, 20),

    .chan5G_cnt = 28,
    .chan5G[0] = CHAN_5(5180, 0, 20),                        // 36
    .chan5G[1] = CHAN_5(5200, 0, 20),                        // 40
    .chan5G[2] = CHAN_5(5220, 0, 20),                        // 44
    .chan5G[3] = CHAN_5(5240, 0, 20),                        // 48
    .chan5G[4] = CHAN_5(5260, CHAN_NO_IR | CHAN_RADAR, 20),  // 52
    .chan5G[5] = CHAN_5(5280, CHAN_NO_IR | CHAN_RADAR, 20),  // 56
    .chan5G[6] = CHAN_5(5300, CHAN_NO_IR | CHAN_RADAR, 20),  // 60
    .chan5G[7] = CHAN_5(5320, CHAN_NO_IR | CHAN_RADAR, 20),  // 64
    .chan5G[8] = CHAN_5(5500, CHAN_NO_IR | CHAN_RADAR, 20),  // 100
    .chan5G[9] = CHAN_5(5520, CHAN_NO_IR | CHAN_RADAR, 20),  // 104
    .chan5G[10] = CHAN_5(5540, CHAN_NO_IR | CHAN_RADAR, 20), // 108
    .chan5G[11] = CHAN_5(5560, CHAN_NO_IR | CHAN_RADAR, 20), // 112
    .chan5G[12] = CHAN_5(5580, CHAN_NO_IR | CHAN_RADAR, 20), // 116
    .chan5G[13] = CHAN_5(5600, CHAN_NO_IR | CHAN_RADAR, 20), // 120
    .chan5G[14] = CHAN_5(5620, CHAN_NO_IR | CHAN_RADAR, 20), // 124
    .chan5G[15] = CHAN_5(5640, CHAN_NO_IR | CHAN_RADAR, 20), // 128
    .chan5G[16] = CHAN_5(5660, CHAN_NO_IR | CHAN_RADAR, 20), // 132
    .chan5G[17] = CHAN_5(5680, CHAN_NO_IR | CHAN_RADAR, 20), // 136
    .chan5G[18] = CHAN_5(5700, CHAN_NO_IR | CHAN_RADAR, 20), // 140
    .chan5G[19] = CHAN_5(5720, CHAN_NO_IR | CHAN_RADAR, 20), // 144
    .chan5G[20] = CHAN_5(5745, 0, 20),                       // 149
    .chan5G[21] = CHAN_5(5765, 0, 20),                       // 153
    .chan5G[22] = CHAN_5(5785, 0, 20),                       // 157
    .chan5G[23] = CHAN_5(5805, 0, 20),                       // 161
    .chan5G[24] = CHAN_5(5825, 0, 20),                       // 165
    .chan5G[25] = CHAN_5(5845, 0, 20),
    .chan5G[26] = CHAN_5(5865, 0, 20),
    .chan5G[27] = CHAN_5(5885, 0, 20),
};

extern int _macsw_macif_init(void);
extern void wl80211_mac_rx_init(void);
extern void wl80211_mac_tx_init(void);
extern int mfg_media_read_macaddr_with_lock(uint8_t mac[6], uint8_t reload);

int _macsw_add_vif_sta(void) {
  struct mm_add_if_req *req = malloc(sizeof(struct mm_add_if_req));
  struct mm_add_if_cfm *cfm = malloc(sizeof(struct mm_add_if_cfm));

  // Set message parameters
  req->type = VIF_STA;
  req->p2p = false;

  if (0 == mfg_media_read_macaddr_with_lock((uint8_t *)req->addr.array, 1)) {
  }

  macif_kmsg_call(MM_ADD_IF_CFM, cfm, sizeof(struct mm_add_if_cfm),
                  MM_ADD_IF_REQ, TASK_MM, req, sizeof(struct mm_add_if_req));

  if (cfm->status) {
    printf("%s:%d, add sta vif failed\n", __func__, __LINE__);
    abort();
  }

  macsw_sta_vif = vif_info_get_vif(cfm->inst_nbr);
  macsw_sta_vif_idx = cfm->inst_nbr;

  free(req);
  free(cfm);

  return 0;
}

void wl80211_mac_init(void) {

  wl80211_mac_rx_init();
  wl80211_mac_tx_init();

  _macsw_macif_init();

#ifdef INVARIANTS
  /* check ke msg hander sorted */
  int last_ke_msg_id = 0;
  for (int i = 0; i < CO_ARRAY_SIZE(ke_msg_handlers); i++) {
    if (ke_msg_handlers[i].id < last_ke_msg_id) {
      printf("ke_msg_handlers not sorted! abort! \r\n");
      abort();
    }
    last_ke_msg_id = ke_msg_handlers[i].id;
  }
#endif

  // Reset macsw
  macif_kmsg_call(MM_RESET_CFM, NULL, 0, MM_RESET_REQ, TASK_MM, NULL, 0);

  struct mm_version_cfm *cfm = malloc(sizeof(struct mm_version_cfm));
  macif_kmsg_call(MM_VERSION_CFM, cfm, sizeof(struct mm_version_cfm),
                  MM_VERSION_REQ, TASK_MM, NULL, 0);
  free(cfm);

  // Init ME
  macif_kmsg_call(ME_CONFIG_CFM, NULL, 0, ME_CONFIG_REQ, TASK_ME,
                  (void *)me_config, sizeof(struct me_config_req));

  // init channels
  macif_kmsg_call(ME_CHAN_CONFIG_CFM, NULL, 0, ME_CHAN_CONFIG_REQ, TASK_ME,
                  (void *)&_macsw_chan_def, sizeof(_macsw_chan_def));

  // Start MACSW
  macif_kmsg_call(MM_START_CFM, NULL, 0, MM_START_REQ, TASK_MM,
                  (void *)mm_start, sizeof(struct mm_start_req));

  // Add default sta vif
  _macsw_add_vif_sta();
}
