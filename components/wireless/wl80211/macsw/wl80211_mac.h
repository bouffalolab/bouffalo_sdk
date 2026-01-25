#ifndef _WL80211_MAC_H
#define _WL80211_MAC_H

#include "wl80211.h"
#include "macsw.h"

/* global variable */
extern uint8_t wl80211_mac_vif[WL80211_VIF_MAX];

struct macsw_tx_desc {
    /// Information provided by Host
    struct hostdesc host;
#if (!MACSW_UMAC_PRESENT)
    /// Information provided by UMAC
    struct umacdesc umac;
#endif

    /// TX HW descriptor attached to the MPDU
    // struct tx_hw_desc hw_desc;
    uint32_t hw_desc[STRUCT_SIZE_TX_HW_DESC / 4];
    /// Buffer header structure - Must be the last element of the structure as it
    /// shall be placed just before the payload space
    // struct txl_buffer_tag buffer;
    uint32_t buffer[STRUCT_SIZE_TXL_BUFFER_TAG / 4];
};

struct wl80211_mac_tx_desc {
    STAILQ_ENTRY(wl80211_mac_tx_desc) entries;

    /* tx done callback */
    void (*txdone_cb)(void *);
    void *opaque;

    // driver specified descriptor (macsw)
    struct macsw_tx_desc txdesc;
    // TODO struct net80211 descriptor
};

struct wl80211_mac_rx_desc {
    STAILQ_ENTRY(wl80211_mac_rx_desc) entries;
    /// Structure containing the information about the received payload - Payload
    /// must be just after !!!
    struct rx_info info;
    /// Payload buffer space - must be after rx_info  !!!
    uint32_t payload[CO_ALIGN4_HI(RX_MAX_AMSDU_SUBFRAME_LEN + 1) / sizeof(uint32_t)];
};

int wl80211_eapol_input(enum wl80211_vif_type vif, uint8_t *payload, size_t len);
int wl80211_mac_mgmt_input(uint8_t *payload, size_t len);
int wl80211_mac_disconnect(uint16_t reason_code, uint16_t status_code);
int wl80211_mac_do_scan(struct wl80211_scan_params *params);
int wl80211_mac_do_connect(struct wl80211_connect_params *conn_params);
int wl80211_mac_ctrl_port(uint8_t sta_id, int control_port_open);
int wl80211_mac_set_ps_mode(int enable);
int wl80211_mac_chan_config_update(uint8_t channel24G_num, const uint8_t *channel24G_chan, uint8_t channel5G_num,
                                   const uint8_t *channel5G_chan);
struct mac_chan_def *wl80211_mac_chan_get(int freq);

int wl80211_mac_start_ap(struct wl80211_ap_settings *ap_settings);
int wl80211_mac_stop_ap(void);
int wl80211_mac_ap_set_key(uint8_t key_idx, uint8_t sta_idx, uint8_t *key, uint8_t key_len, uint8_t cipher_suite,
                           bool pairwise);
int wl80211_mac_ap_del_key(uint8_t sta_idx);
int wl80211_mac_ap_del_sta(uint8_t sta_idx);
int wl80211_mac_ap_ctrl_port(uint8_t sta_id, int control_port_open);
// AP mode process auth packet
void wl80211_mac_ap_auth_handler(void *frame_payload, uint32_t frame_length);
// AP mode process assoc packet
void wl80211_mac_ap_assoc_req_handler(void *frame_payload, uint32_t frame_length, bool is_reassoc);
// AP mode process probe request packet
void wl80211_mac_ap_probe_req_handler(void *frame_payload, uint32_t frame_length);
// AP mode process deauth packet
void wl80211_mac_ap_deauth_handler(void *frame_payload, uint32_t frame_length);
// AP mode process disassoc packet
void wl80211_mac_ap_disassoc_handler(void *frame_payload, uint32_t frame_length);

extern wl80211_monitor_rx_cb_t _wl80211_monitor_rx;
extern void *_wl80211_monitor_rx_ctx;

int wl80211_mac_monitor_start(struct wl80211_monitor_settings *mon_setting);
int wl80211_mac_monitor_stop(void);

// only for type checker
struct wl80211_tx_header;
#define WL80211_MAC_TX_FLAG_MGMT 0x1
int wl80211_mac_tx(enum wl80211_vif_type vif, struct wl80211_tx_header *desc, unsigned int flags, struct iovec *seg,
                   int seg_cnt, void *txdone_cb, void *opaque);

void wl80211_mac_rx_free(void *info);

//////////////////////
// 驱动专用接口和变量
// Utils helper function
//////////////////////
extern const struct me_chan_config_req _macsw_chan_def;

int _macsw_add_vif(enum mac_vif_type vif_type, uint8_t mac[6]);
int _macsw_remove_vif(int vif_idx);
struct scanu_start_req *_macsw_make_scan_req(struct wl80211_scan_params *params);
struct me_config_req *_macsw_make_me_config(void);

void _find_wpa_rsn_ie(uint32_t var_part_addr, uint32_t var_part_len, uint32_t *wpa_ie, uint32_t *rsn_ie);
uint8_t _macsw_get_staid(int vif_idx, struct mac_addr *dst_addr, bool mgmt_frame);
void _sme_auth_mgmt_rx(uint8_t *frame, size_t len);
void *_ap_get_wpa_sm(uint8_t *mac);

int _external_auth_ind(ke_msg_id_t const msgid, void *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
int _ap_start_cfm(ke_msg_id_t const msgid, void *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
int _sta_add_cfm(ke_msg_id_t const msgid, void *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
#endif
