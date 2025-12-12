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

int wl80211_mac_disconnect(uint16_t reason_code, uint16_t status_code);
int wl80211_mac_do_scan(void);
int wl80211_mac_do_connect(void);
int wl80211_mac_ctrl_port(uint8_t sta_id, int control_port_open);
int wl80211_mac_set_ps_mode(int enable);
int wl80211_mac_chan_config_update(uint8_t channel24G_num, uint8_t *channel24G_chan, uint8_t channel5G_num,
                                   uint8_t *channel5G_chan);
struct mac_chan_def *wl80211_mac_chan_get(int freq);
int wl80211_mac_start_ap(void);
int wl80211_mac_ap_ctrl_port(uint8_t sta_id, int control_port_open);
// AP mode process auth packet
void wl80211_mac_ap_auth_handler(void *frame_payload, uint32_t frame_length);
// AP mode process assoc packet
void wl80211_mac_ap_assoc_req_handler(void *frame_payload, uint32_t frame_length, bool is_reassoc);

// only for type checker
struct wl80211_tx_header;
#define WL80211_MAC_TX_FLAG_MGMT 0x1
int wl80211_mac_tx(wl80211_vif_type vif, struct wl80211_tx_header *desc, unsigned int flags, struct iovec *seg, int seg_cnt,
                   void *txdone_cb, void *opaque);

void wl80211_mac_rx_free(void *info);

//////////////////////
// 驱动专用接口和变量
// Utils helper function
//////////////////////
extern const struct me_chan_config_req _macsw_chan_def;

void _find_wpa_rsn_ie(uint32_t var_part_addr, uint32_t var_part_len, uint32_t *wpa_ie, uint32_t *rsn_ie);
uint8_t _macsw_get_staid(int vif_idx, struct mac_addr *dst_addr, bool mgmt_frame);
void _sae_send_frame(int auth_seq);
void _sme_auth_mgmt_rx(uint8_t *frame, size_t len);
int external_auth_ind(ke_msg_id_t const msgid, void *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
#endif
