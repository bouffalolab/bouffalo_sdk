/**
 ****************************************************************************************
 *
 * @file cfgmacsw.h
 *
 * @brief Definition of message index and associated structures for messages of type
 * @ref FHOST_MSG_CFGMACSW.
 *
 *
 ****************************************************************************************
 */
#ifndef _CFGMACSW_H_
#define _CFGMACSW_H_

#include "macsw.h"
#include "phy.h"

#include "rtos_def.h"
#include "fhost_api.h"

/**
 ****************************************************************************************
 * @defgroup FHOST_CFGMACSW FHOST_CFGMACSW
 * @ingroup FHOST_CNTRL
 * Interface description between fhost control task and supplicant application.
 *
 * @{
 ****************************************************************************************
 */
/// UDP port for CFGMACSW connection
#define CFGMACSW_PORT 1630

/// CFGMACSW index message
enum cfgmacsw_msg_index {
    /// Sent by supplicant to retrieve HW capability (param: none)
    CFGMACSW_HW_FEATURE_CMD = 1,
    /// Response to CFGMACSW_HW_FEATURE_CMD (param: @ref cfgmacsw_hw_feature)
    CFGMACSW_HW_FEATURE_RESP,
    /// Sent by supplicant to retrieve FW capability (param: none)
    CFGMACSW_GET_CAPA_CMD,
    /// Response to CFGMACSW_GET_CAPA_CMD (param: none)
    CFGMACSW_GET_CAPA_RESP,
    /// Sent by Supplicant to install/remove Encryption key (param: @ref cfgmacsw_set_key)
    CFGMACSW_SET_KEY_CMD,
    /// Response to CFGMACSW_SET_KEY_CMD (param: @ref cfgmacsw_resp)
    CFGMACSW_SET_KEY_RESP,
    /// Sent by Supplicant to start a SCAN (param: @ref cfgmacsw_scan)
    CFGMACSW_SCAN_CMD,
    /// Response to CFGMACSW_SCAN_CMD (param: @ref cfgmacsw_resp)
    CFGMACSW_SCAN_RESP,
    /// Event sent when scan is done (param: @ref cfgmacsw_scan_completed)
    CFGMACSW_SCAN_DONE_EVENT,
    /// Event sent when a new AP is found (param: @ref cfgmacsw_scan_result)
    CFGMACSW_SCAN_RESULT_EVENT,//10
    /// Sent by supplicant to initiate a connection (param: @ref cfgmacsw_connect)
    CFGMACSW_CONNECT_CMD,
    /// Response to CFGMACSW_CONNECT_CMD (param: @ref cfgmacsw_resp)
    CFGMACSW_CONNECT_RESP,
    /// Event sent when the connection is finished (param: @ref cfgmacsw_connect_event)
    CFGMACSW_CONNECT_EVENT,
    /// Sent by supplicant to end a connection (param: @ref cfgmacsw_disconnect)
    CFGMACSW_DISCONNECT_CMD,
    /// Response to CFGMACSW_DISCONNECT_CMD (param: @ref cfgmacsw_resp)
    CFGMACSW_DISCONNECT_RESP,
    /// Event sent if the connection is lost (param: @ref cfgmacsw_disconnect_event)
    CFGMACSW_DISCONNECT_EVENT,
    /// Sent by supplicant to open/close a control port (param: @ref cfgmacsw_ctrl_port)
    CFGMACSW_CTRL_PORT_CMD,
    /// Response to CFGMACSW_CTRL_PORT_CMD (param: @ref cfgmacsw_resp)
    CFGMACSW_CTRL_PORT_RESP,
    /// Event sent if a Michael MIC failure is detected (param: @ref cfgmacsw_mic_failure_event)
    CFGMACSW_MIC_FAILURE_EVENT,
    /// Sent by Application to retrieve system statistics (param: none)
    CFGMACSW_SYS_STATS_CMD,//20
    /// Response to CFGMACSW_SYS_STATS_CMD (param: @ref cfgmacsw_sys_stats_resp)
    CFGMACSW_SYS_STATS_RESP,
    /// Sent by smartconfig to obtain scan results (param: none)
    CFGMACSW_SCAN_RESULTS_CMD,
    /// Response to CFGMACSW_SCAN_RESULTS_CMD (param: @ref cfgmacsw_scan_results_resp)
    CFGMACSW_SCAN_RESULTS_RESP,
    /// Sent by Application to retrieve FW/PHY supported features (param: none)
    CFGMACSW_LIST_FEATURES_CMD,
    /// Response to CFGMACSW_LIST_FEATURES_CMD (param: @ref cfgmacsw_list_features_resp)
    CFGMACSW_LIST_FEATURES_RESP,
    /// Sent to change the type of an vif at MAC level. MAC VIF is deleted (if it exists) and
    /// re-created with the new type (unless type is VIF_UNKNOWN) (param: @ref cfgmacsw_set_vif_type)
    CFGMACSW_SET_VIF_TYPE_CMD,
    /// Response to CFGMACSW_SET_VIF_TYPE_CMD (param: @ref cfgmacsw_resp)
    CFGMACSW_SET_VIF_TYPE_RESP,
    /// Sent by Application to configure a monitor interface (param: @ref cfgmacsw_monitor_cfg)
    CFGMACSW_MONITOR_CFG_CMD,
    /// Response to CFGMACSW_MONITOR_CFG_CMD (param: @ref cfgmacsw_resp)
    CFGMACSW_MONITOR_CFG_RESP,
    /// Event sent by the RX task when management frame is forwarded by the wifi task (param: @ref cfgmacsw_rx_mgmt_event)
    CFGMACSW_RX_MGMT_EVENT,//30
    /// Unprotected deauthentication frame notification. This event is used to indicate that an unprotected
    /// deauthentication frame was dropped when MFP is in use
    CFGMACSW_UNPROT_DEAUTHENTICATE,
    /// nprotected disassociation frame notification. This event is used to indicate that an unprotected
    /// disassociation frame was dropped when MFP is in use.
    CFGMACSW_UNPROT_DISASSOCIATE,
    /// Event to defer TX status processing (param: @ref cfgmac_tx_status_event)
    CFGMACSW_TX_STATUS_EVENT,
    /// Event sent by wifi task to request external authentication (i.e. Supplicant will do the
    /// authentication procedure for the wifi task, used for SAE) (param: @ref cfgmacsw_external_auth_event)
    CFGMACSW_EXTERNAL_AUTH_EVENT,
    /// Sent by Supplicant to pass external authentication status (param: @ref cfgmacsw_external_auth_status)
    CFGMACSW_EXTERNAL_AUTH_STATUS_RESP,
    /// Response to CFGMACSW_EXTERNAL_AUTH_STATUS_RESP (param: @ref cfgmacsw_resp)
    CFGMACSW_EXTERNAL_AUTH_STATUS_RESP_RESP,
    /// Sent by Supplicant to start an AP (param: @ref cfgmacsw_start_ap)
    CFGMACSW_START_AP_CMD,
    /// Response to CFGMACSW_START_AP_CMD (param: @ref cfgmacsw_resp)
    CFGMACSW_START_AP_RESP,
    /// Sent by Supplicant to stop an AP (param: @ref cfgmacsw_stop_ap)
    CFGMACSW_STOP_AP_CMD,
    /// Response to CFGMACSW_STOP_AP_CMD (param: @ref cfgmacsw_resp)
    CFGMACSW_STOP_AP_RESP,
    /// Sent by Supplicant to configure EDCA parameter for one AC (param: @ref cfgmacsw_set_edca)
    CFGMACSW_SET_EDCA_CMD,
    /// Response to CFGMACSW_SET_EDCA_CMD (param: @ref cfgmacsw_resp)
    CFGMACSW_SET_EDCA_RESP,
    /// Sent by Supplicant to update the beacon (param: @ref cfgmacsw_bcn_update)
    CFGMACSW_BCN_UPDATE_CMD,
    /// Response to CFGMACSW_BCN_UPDATE (param: @ref cfgmacsw_resp)
    CFGMACSW_BCN_UPDATE_RESP,
    /// Beacon Transmission Control
    CFGMACSW_BCN_CONTROL_CMD,
    /// resp of Beacon Transmission Control
    CFGMACSW_BCN_CONTROL_RESP,
    /// Send to supplicant to register a new Station (param: @ref cfgmacsw_sta_add)
    CFGMACSW_STA_ADD_CMD,
    /// Response to CFGMACSW_STA_ADD_CMD (param: @ref cfgmacsw_resp)
    CFGMACSW_STA_ADD_RESP,
    /// Send to supplicant to un-register a Station (param: @ref cfgmacsw_sta_remove)
    CFGMACSW_STA_REMOVE_CMD,
    /// Response to CFGMACSW_STA_REMOVE_CMD (param: @ref cfgmacsw_resp)
    CFGMACSW_STA_REMOVE_RESP,
    /// Send by supplicant to retrieve Key current sequence number (param: @ref cfgmacsw_key_seqnum)
    CFGMACSW_KEY_SEQNUM_CMD,
    /// Response to CFGMACSW_KEY_SEQNUM_CMD (param: @ref cfgmacsw_key_seqnum_resp)
    CFGMACSW_KEY_SEQNUM_RESP,
    /// GET ME PARAM(param: @ref cfgmacsw_me_param)
    CFGMACSW_ME_PARAM_CMD,
    /// Response to CFGMACSW_ME_PARAM_CMD (param: @ref cfgmacsw_resp)
    CFGMACSW_ME_PARAM_RESP,
    /// Enable Power Save (param: @ref cfgmacsw_set_ps_mode)
    CFGMACSW_SET_PS_MODE_CMD,
    /// Response to CFGMACSW_SET_PS_MODE_CMD (param: @ref cfgmacsw_resp)
    CFGMACSW_SET_PS_MODE_RESP,
    /// Request statistics information for a station (param: @ref cfgmacsw_get_sta_info)
    CFGMACSW_GET_STA_INFO_CMD,
    /// Response to CFGMACSW_GET_STA_INFO_CMD with statistics of the station
    /// (param: @ref cfgmacsw_get_sta_info_resp)
    CFGMACSW_GET_STA_INFO_RESP,
    /// Request to probe if a client is still present (param: @ref cfgmacsw_probe_client)
    CFGMACSW_PROBE_CLIENT_CMD,
    /// Response to CFGMACSW_PROBE_CLIENT_CMD. Only indicates if request was valid or not.
    /// The actual status will be in CFGMACSW_PROBE_CLIENT_EVENT (param: @ref cfgmacsw_resp)
    CFGMACSW_PROBE_CLIENT_RESP,
    /// Event sent after receiving CFGMACSW_PROBE_CLIENT_CMD, to indicate the actual client
    /// status (param: @ref cfgmacsw_probe_client_event)
    CFGMACSW_PROBE_CLIENT_EVENT,
    /// Request to remain on specific channel (param: @ref cfgmacsw_remain_on_channel)
    CFGMACSW_REMAIN_ON_CHANNEL_CMD,
    /// Response to CFGMACSW_REMAIN_ON_CHANNEL_CMD. Only indicates if request was valid or not.
    /// The actual status will be in CFGMACSW_REMAIN_ON_CHANNEL_EVENT (param: @ref cfgmacsw_resp)
    CFGMACSW_REMAIN_ON_CHANNEL_RESP,
    /// Event sent after receiving CFGMACSW_REMAIN_ON_CHANNEL_CMD, to indicate that the
    /// procedure is completed (param: @ref cfgmacsw_remain_on_channel_event)
    CFGMACSW_REMAIN_ON_CHANNEL_EVENT,
    /// Request to cancel remain on channel (param: @ref cfgmacsw_cancel_remain_on_channel)
    CFGMACSW_CANCEL_REMAIN_ON_CHANNEL_CMD,
    /// Response to CFGMACSW_CANCEL_REMAIN_ON_CHANNEL_CMD. Only indicates if request was valid or not.
    /// The actual status will be in CFGMACSW_CANCEL_REMAIN_ON_CHANNEL_EVENT (param: @ref cfgmacsw_resp)
    CFGMACSW_CANCEL_REMAIN_ON_CHANNEL_RESP,
    /// Event sent after receiving CFGMACSW_CANCEL_REMAIN_ON_CHANNEL_CMD, to indicate that
    /// the procedure is completed (param: @ref cfgmacsw_remain_on_channel_event)
    CFGMACSW_REMAIN_ON_CHANNEL_EXP_EVENT,
    /// Request RC statistics for a station (param: @ref cfgmacsw_rc_cmd)
    CFGMACSW_RC_CMD,
    /// Response to CFGMACSW_RC_CMD. Indicate the RC statisticts
    /// (param: @ref cfgmacsw_rc_resp)
    CFGMACSW_RC_RESP,
    /// Request by Application to setup NOA protocol (param: @ref cfgmacsw_p2p_noa_cmd)
    CFGMACSW_P2P_NOA_CMD,
    /// Response to CFGMACSW_P2P_NOA_CMD (param: @ref cfgmacsw_resp)
    CFGMACSW_P2P_NOA_RESP,
    /// Request to set RC rate (param: @ref cfgmacsw_rc_set_rate)
    CFGMACSW_RC_SET_RATE_CMD,
    /// Response to CFGMACSW_RC_SET_RATE_CMD (param: @ref cfgmacsw_resp)
    CFGMACSW_RC_SET_RATE_RESP,
    /// Request to join a mesh network (param: @ref cfgmacsw_join_mesh)
    CFGMACSW_JOIN_MESH_CMD,
    /// Response to CFGMACSW_JOIN_MESH_CMD (param: @ref cfgmacsw_resp)
    CFGMACSW_JOIN_MESH_RESP,
    /// Request to leave a mesh network (param: @ref cfgmacsw_join_mesh)
    CFGMACSW_LEAVE_MESH_CMD,
    /// Response to CFGMACSW_LEAVE_MESH_CMD (param: @ref cfgmacsw_resp)
    CFGMACSW_LEAVE_MESH_RESP,
    /// Notification that a connection has been established or lost (param: @ref cfgmacsw_mesh_peer_update_ntf)
    CFGMACSW_MESH_PEER_UPDATE_NTF_CMD,
    /// Response to CFGMACSW_MESH_PEER_UPDATE_NTF_CMD (param: @ref cfgmacsw_resp)
    CFGMACSW_MESH_PEER_UPDATE_NTF_RESP,
    /// Notification of a new peer candidate MESH (param: @ref cfgmacsw_new_peer_candidate_event)
    CFGMACSW_NEW_PEER_CANDIDATE_EVENT,
    /// Request by Application to setup a FTM measurement (param: @ref cfgmacsw_ftm_start_cmd)
    CFGMACSW_FTM_START_CMD,
    /// Response to CFGMACSW_FTM_START_CMD (param: @ref cfgmacsw_resp)
    CFGMACSW_FTM_START_RESP,
    /// Event sent after receiving CFGMACSW_FTM_START_CMD, to indicate that the
    /// procedure is completed (param: @ref cfgmacsw_ftm_done_event)
    CFGMACSW_FTM_DONE_EVENT,
    /// Request to print status code (param: @ref cfgmacsw_status_code_print)
    CFGMACSW_STATUS_CODE_PRINT_CMD,
    /// Requset to send raw packet
    CFGMACSW_RAW_SEND_CMD,
    CFGMACSW_RAW_SEND_RESP,
    /// Requset to config TWT setup
    CFGMACSW_TWT_SETUP_CMD,
    CFGMACSW_TWT_SETUP_RESP,
    /// Requset to config TWT teardown
    CFGMACSW_TWT_TEARDOWN_CMD,
    CFGMACSW_TWT_TEARDOWN_RESP,
    /// Sent to get edca (param: @ref cfgmacsw_get_edca)
    CFGMACSW_GET_EDCA_CMD,
    /// Response to CFGMACSW_GET_EDCA_CMD  (param: @ref cfgmacsw_get_edca_resp)
    CFGMACSW_GET_EDCA_RESP,
    /// Sent to get the number of frames remaining in each Tx queue  (param: @ref cfgmacsw_msg)
    CFGMACSW_GET_REMAINING_TX_CMD,
    /// Response to CFGMACSW_GET_REMAINING_TX_CMD  (param: @ref cfgmacsw_get_remaining_tx_resp)
    CFGMACSW_GET_REMAINING_TX_RESP,

    /// Get wifi stats
    CFGMACSW_GET_STATS_CMD,
    /// Clear wifi stats
    CFGMACSW_CLR_STATS_CMD,

    CFGMACSW_STATS_RESP,
#ifdef CFG_BL_WIFI_PS_ENABLE
    /// Requset to send null packet
    CFGMACSW_NULL_DATA_SEND_CMD,
    CFGMACSW_NULL_DATA_SEND_RESP,
#endif
};

/// CFGMACSW status
enum cfgmacsw_status {
    /// Success status
    CFGMACSW_SUCCESS = 0,
    /// Generic error status
    CFGMACSW_ERROR,
    /// Error invalid VIF index parameter
    CFGMACSW_INVALID_VIF,
    /// Error invalid STA index parameter
    CFGMACSW_INVALID_STA,
    /// Error invalid parameter
    CFGMACSW_INVALID_PARAM,
};

/// CFGMACSW message header
struct cfgmacsw_msg_hdr {
    /// For CFGMACSW commands, queue handle to use to push the response
    rtos_queue resp_queue;
    /// Length, in bytes, of the message (including this header)
    uint16_t len;
    /// ID of the message.
    uint16_t id;
};

/// CFGMACSW generic message structure
struct cfgmacsw_msg {
    /// header
    struct cfgmacsw_msg_hdr hdr;
};

/// CFGMACSW generic response structure
struct cfgmacsw_resp {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Status
    uint32_t status;
};

struct fhost_mm_start_req
{
    /// PHY configuration
    struct phy_cfg_tag phy_cfg;
    /// UAPSD timeout
    uint32_t uapsd_timeout;
    /// Local LP clock accuracy (in ppm)
    uint16_t lp_clk_accuracy;
    /// Array of TX timeout values (in ms, one per TX queue) - 0 sets default value
    uint16_t tx_timeout[AC_MAX];
};

struct fhost_me_config_req
{
    /// HT Capabilities
    struct mac_htcapability ht_cap;
    /// VHT Capabilities
    struct mac_vhtcapability vht_cap;
    /// HE capabilities
    struct mac_hecapability he_cap;
    /// Lifetime of packets sent under a BlockAck agreement (expressed in TUs)
    uint16_t tx_lft;
    /// Maximum supported BW
    uint8_t phy_bw_max;
    /// Boolean indicating if HT is supported or not
    bool ht_supp;
    /// Boolean indicating if VHT is supported or not
    bool vht_supp;
    /// Boolean indicating if HE is supported or not
    bool he_supp;
    /// Boolean indicating if HE OFDMA UL is enabled or not
    bool he_ul_on;
    /// Boolean indicating if PS mode shall be enabled or not
    bool ps_on;
    /// Boolean indicating if Antenna Diversity shall be enabled or not
    bool ant_div_on;
    /// Boolean indicating if Dynamic PS mode shall be used or not
    bool dpsm;
    /// Indicates whether AMSDU shall be forced or not
    enum amsdu_tx amsdu_tx;
    #ifdef CFG_LPM
    /// chip_version
    uint8_t chip_version;
    #endif
};

/// structure for CFGMACSW_HW_FEATURE_RESP
struct cfgmacsw_hw_feature {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// ME configuration
    struct fhost_me_config_req me_config;
    /// Channel configuration
    struct me_chan_config_req *chan;
    /// ap mode setting
    bool ap_11b_only;
};

/// structure for CFGMACSW_SET_KEY_CMD
struct cfgmacsw_set_key {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    int fhost_vif_idx;
    /// MAC addr (may be NULL for group key)
    const struct mac_addr *addr;
    /// Cipher
    int cipher_suite;
    /// Key index
    int key_idx;
    /// Key
    const uint8_t *key;
    /// Key length
    int key_len;
    /// Initial Sequence number
    const uint8_t *seq;
    /// Sequence number length
    int seq_len;
    /// Whether this is a pairwise key
    bool pairwise;
};

/// SSID representation used by CFGMACSW_SCAN_CMD and CFGMACSW_CONNECT_CMD
struct cfgmacsw_scan_ssid {
    /// SSID string
    const uint8_t *ssid;
    /// Length of the SSID string
    size_t len;
};

/// structure for CFGMACSW_RC_CMD
struct cfgmacsw_rc {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Sta idx
    int sta_idx;
};

/// structure for CFGMACSW_RC_SET_RATE_CMD
struct cfgmacsw_rc_set_rate {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Sta idx
    uint16_t sta_idx;
    /// Fixed rate configuration
    uint16_t fixed_rate_cfg;
};

/// structure for CFGMACSW_SCAN_RESULT_EVENT
struct cfgmacsw_rc_result {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Status
    uint32_t status;
    /// RC statistics
    //struct me_rc_stats_cfm cfm;
};

/// structure for CFGMACSW_STATUS_CODE_PRINT_CMD
struct cfgmacsw_status_code_print {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    int fhost_vif_idx;
    const char *msg;

};
typedef void (*cfgmacsw_raw_send_done)(void* env);
typedef void (*cfgmacsw_adhoc_tx_cfm)(void* env, uint32_t status);
struct cfgmacsw_raw_send {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    int fhost_vif_idx;
    /// Socket to use to send CFGRWNX events
    int sock;
    /// Sending channel
    int channel;
    /// Pakcet buffer
    void *pkt;
    /// Packet len
    uint32_t len;
    /// Send done callback
    cfgmacsw_raw_send_done cb;
    /// callback env
    void *env;
    /// Duration in ms, default 20ms
    uint32_t duration;
    /// whether need wait resp
    int need_rx;
    /// adhoc mode
    bool adhoc;
    /// Receiver address
    struct mac_addr *ra;
    /// Transmitter address
    struct mac_addr *ta;
    /// Tx rate
    uint8_t rate;
    /// Tx rts thrshold
    uint8_t rts_thrshold;
    /// Tx power
    int8_t tx_power;
    /// Tx retry limit
    uint8_t retry_limit;
    /// callback func for adhoc tx
    cfgmacsw_adhoc_tx_cfm cb_cfm;
};

#ifdef CFG_BL_WIFI_PS_ENABLE
struct cfgmacsw_null_data_send {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    int fhost_vif_idx;
};
#endif

/// structure for CFGMACSW_SCAN_CMD
struct cfgmacsw_scan {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    int fhost_vif_idx;
    /// SSID to scan
    struct cfgmacsw_scan_ssid *ssids;
    /// Number of SSID in ssids
    int ssid_cnt;
    /// Extra IE to add in the proce request
    const uint8_t *extra_ies;
    /// Size of the extra IEs
    int extra_ies_len;
    /// Array of frequencies to scan or %NULL for all frequencies.
    /// The frequency is set in MHz. The array is zero-terminated.
    int *freqs;
    /// Array of chans to scan or %NULL for all channels.
    const uint8_t *chans;
    /// Number of channels in chans
    int chans_cnt;
    /// Do not use CCK mode
    bool no_cck;
    /// BSSID to scan, can be NULL for wildcard BSSID
    const uint8_t *bssid;
    /// Scan duration, in TUs
    int duration;
    /// bouffalo flags for scan
    uint16_t flags;
    /// If the socket used for IPC
    bool is_cntrl_link;
    /// 0: active scan; 1: Passive scan request
    bool passive;
    /// the count of probe req sent on every channel
    uint32_t probe_cnt;
};

/// structure for CFGMACSW_SCAN_DONE_EVENT
struct cfgmacsw_scan_completed {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// Status
    uint32_t status;
    /// Nb result available with CFGMACSW_SCAN_RESULTS_CMD
    uint32_t result_cnt;
};

/// structure for CFGMACSW_SCAN_RESULT_EVENT
struct cfgmacsw_scan_result {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// Frequency
    uint16_t freq;
    /// RSSI of the received frame (dBm).
    int16_t rssi;
    /// Length of the frame (beacon or probe response)
    uint16_t length;
    /// Frame payload. Must be freed (using rtos_free) when event has been processed
    uint8_t *payload;
};

/// structure for CFGMACSW_CONNECT_CMD
struct cfgmacsw_connect {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// BSSID tO connect to
    const uint8_t *bssid;
    /// SSID to connect to
    struct cfgmacsw_scan_ssid ssid;
    /// Channel of the AP
    struct mac_chan_def chan;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// Authentication Type
    uint16_t auth_alg;
    /// WPAX
    uint8_t wpa;
    /// Connection flags
    uint32_t flags;
    /// Control port Ethertype
    uint16_t ctrl_port_ethertype;
    /// UAPSD queues (bit0: VO, bit1: VI, bit2: BK, bit3: BE). Set to 0xFFFF to use default config
    uint16_t uapsd;
    /// Length, in bytes, of the extra IE
    uint32_t ie_len;
    /// Extra IE to add to association request
    const uint8_t *ie;
};

/// structure for CFGMACSW_CONNECT_EVENT
struct cfgmacsw_connect_event {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// BSSID
    struct mac_addr bssid;
    /// Sta idx
    int sta_idx;
    /// Status code of the connection procedure
    uint16_t status_code;
    /// Frequency of the operational channel in MHz
    uint16_t freq;
    /// Length of the AssocReq IEs (in bytes)
    uint16_t assoc_req_ie_len;
    /// Length of the AssocRsp IEs (in bytes)
    uint16_t assoc_resp_ie_len;
    /// AssocReq IEs (assoc_req_ie_len) followed by AssocRsp IEs (assoc_resp_ie_len)
    /// Must be freed (using rtos_free) when event has been processed
    uint8_t *req_resp_ies;
};

/// structure for CFGMACSW_DISCONNECT_CMD
struct cfgmacsw_disconnect {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Reason of the disconnection
    uint16_t reason_code;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// Status code from wpa_supplicant
    uint16_t status_code;
};

/// structure for CFGMACSW_DISCONNECT_EVENT
struct cfgmacsw_disconnect_event {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Reason of the disconnection
    uint16_t reason_code;
    /// Vif idx
    uint16_t fhost_vif_idx;
};

/// structure for CFGMACSW_CTRL_PORT_CMD
struct cfgmacsw_ctrl_port {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// VIF index
    uint16_t fhost_vif_idx;
    /// Address of the STA managed by the port (Needed only for AP interface)
    struct mac_addr addr;
    /// Port status
    uint16_t authorized;
};

/// structure for CFGMACSW_MIC_FAILURE_EVENT
struct cfgmacsw_mic_failure_event {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Address of the sending STA
    struct mac_addr addr;
    /// Boolean indicating if the packet was a group or unicast one (true if group)
    bool ga;
    /// VIF index
    uint16_t fhost_vif_idx;
};

struct fhost_dbg_get_sys_stat_cfm
{
    /// Time spent in CPU sleep since last reset of the system statistics
    uint32_t cpu_sleep_time;
    /// Time spent in DOZE since last reset of the system statistics
    uint32_t doze_time;
    /// Total time spent since last reset of the system statistics
    uint32_t stats_time;
};

/// Structure for @ref CFGMACSW_SYS_STATS_RESP
struct cfgmacsw_sys_stats_resp {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Statistics
    struct fhost_dbg_get_sys_stat_cfm stats;
};

/// Structure for CFGMACSW_SCAN_RESULTS_CMD
struct cfgmacsw_scan_results {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// index of the scan result element
    uint16_t idx;
};

/// Structure for CFGMACSW_SCAN_RESULTS_RESP
struct cfgmacsw_scan_results_resp {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// scan results
    struct mac_scan_result scan_result;
    /// Status
    uint32_t status;
};

struct fhost_mm_version_cfm
{
    /// Version of the LMAC FW
    uint32_t version_lmac;
    /// Version1 of the MAC HW (as encoded in version1Reg MAC HW register)
    uint32_t version_machw_1;
    /// Version2 of the MAC HW (as encoded in version2Reg MAC HW register)
    uint32_t version_machw_2;
    /// Version1 of the PHY (depends on actual PHY)
    uint32_t version_phy_1;
    /// Version2 of the PHY (depends on actual PHY)
    uint32_t version_phy_2;
    /// Supported Features
    uint32_t features;
    /// Maximum number of supported stations
    uint16_t max_sta_nb;
    /// Maximum number of supported virtual interfaces
    uint8_t max_vif_nb;
};

struct cfgmacsw_get_edca {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// HW queue
    uint8_t hw_queue;
};

/// Structure for @ref CFGMACSW_LIST_FEATURES_RESP
struct cfgmacsw_list_features_resp {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// structure containing FW/PHY features
    struct fhost_mm_version_cfm version;
};

struct cfgmacsw_get_edca_resp {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Arbitration InterFrame Space Number
    uint8_t aifsn;
    /// Contention Window minimum
    uint16_t cwmin;
    /// Contention Window maximum
    uint16_t cwmax;
    /// Status
    uint32_t status;
};

struct cfgmacsw_get_remaining_tx_resp {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// the number of frames remaining in each Tx queue.
    uint8_t tx_cnt[AC_MAX];
    /// Status
    uint32_t status;
};

/// Structure for @ref CFGMACSW_SET_VIF_TYPE_CMD
struct cfgmacsw_set_vif_type {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Index of the FHOST vif
    int fhost_vif_idx;
    /// Type to set on the interface
    enum mac_vif_type type;
    /// Is a P2P vif (only read if type is VIF_STA or VIF_AP)
    bool p2p;
};

/// Structure for CFGMACSW_MONITOR_CFG_CMD
struct cfgmacsw_monitor_cfg {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// Interface configuration
    struct fhost_vif_monitor_cfg cfg;
};

/// Structure for CFGMACSW_RX_MGMT_EVENT
struct cfgmacsw_rx_mgmt_event {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// Frequency
    uint16_t freq;
    /// RSSI of the received frame (dBm).
    int16_t rssi;
    /// Length of the frame
    uint16_t length;
    /// Frame payload.
    uint8_t *payload_in_heap;
};

/// Structure for CFGMACSW_TX_STATUS_EVENT
struct cfgmac_tx_status_event {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Frame data
    const uint8_t *data;
    /// Frame length
    uint32_t data_len;
    /// TX status
    bool acknowledged;
};

/// Structure for CFGMACSW_EXTERNAL_AUTH_EVENT
struct cfgmacsw_external_auth_event {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// BSSID
    struct mac_addr bssid;
    /// SSID
    struct mac_ssid ssid;
    /// AKM
    uint32_t akm;
};

/// Structure for CFGMACSW_EXTERNAL_AUTH_STATUS_RESP
struct cfgmacsw_external_auth_status {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// Authentication status
    uint16_t status;
};

/// Structure for CFGMACSW_START_AP_CMD
struct cfgmacsw_start_ap {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// Basic rates
    struct mac_rateset basic_rates;
    /// Operating Channel for the AP
    struct mac_chan_op chan;
    /// Buffer containing the Beacon
    uint8_t *bcn;
    /// Size, in bytes, of the Beacon buffer
    int bcn_len;
    /// Offset within the beacon of the TIM element (in bytes)
    int tim_oft;
    /// Size, in bytes, of the TIM element
    int tim_len;
    /// Beacon interval in TU
    int bcn_int;
    /// AP flags (@see mac_connection_flags)
    int flags;
    /// Port number for ethernet authentication frame
    uint16_t ctrl_ethertype;
};

/// Structure for CFGMACSW_STOP_AP_CMD
struct cfgmacsw_stop_ap {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
};

/// Structure for CFGMACSW_SET_EDCA_CMD
struct cfgmacsw_set_edca {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// HW queue
    uint8_t hw_queue;
    /// Arbitration InterFrame Space Number
    uint8_t aifsn;
    /// Contention Window minimum
    uint16_t cwmin;
    /// Contention Window maximum
    uint16_t cwmax;
    /// TXOP (in unit of 32us)
    uint16_t txop;
};

/// Structure for CFGMACSW_BCN_UPDATE_CMD
struct cfgmacsw_bcn_update {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// Buffer containing the Beacon
    uint8_t *bcn;
    /// Size, in bytes, of the Beacon buffer
    int bcn_len;
    /// Offset within the beacon of the TIM element (in bytes)
    int tim_oft;
    /// Size, in bytes, of the TIM element
    int tim_len;
    /// Offset of CSA (channel switch announcement) counters (0 means no counter)
    uint8_t csa_oft[BCN_MAX_CSA_CPT];
};

struct cfgmacsw_bcn_control {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// bcn_mode:
    /// 0      Start/Stop beacon transmissions automatically
    ///         a.Beacon transmission is NOT started when SAP is started.
    ///         b.Once a Probe Request frame having the same SSID is received, replies with a Probe Response frame, then Beacon transmission is started.
    ///         c.Beacon transmission is stopped again if no STA is associated for more than bcn_timer seconds.
    /// 1      Do not transmit beacon frames
    /// 2      Transmit beacon frames (Default)
    uint8_t bcn_mode;
    /// Beacon transmission is stopped again if no STA is associated for more than bcn_timer seconds.
    int bcn_timer;
    /// should bcn tx stop
    bool bcn_stop;
};

/// Structure for CFGMACSW_STA_ADD_CMD
struct cfgmacsw_sta_add {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// Association ID of the station
    uint16_t aid;
    /// MAC address of the station to be added
    const struct mac_addr *addr;
    /// Supported legacy rates
    struct mac_rateset rate_set;
    /// HT Capabilities
    struct mac_htcapability ht_cap;
    /// VHT Capabilities
    struct mac_vhtcapability vht_cap;
    /// HE capabilities
    struct mac_hecapability he_cap;
    /// STA flags (@ref mac_sta_flags)
    uint32_t flags;
    /// Bit field indicating which queues have U-APSD enabled
    uint8_t uapsd_queues;
    /// Maximum size, in frames, of a APSD service period
    uint8_t max_sp_len;
    /// Operation mode information (valid if bit @ref STA_OPMOD_NOTIF is
    /// set in the flags)
    uint8_t opmode;
    /// Listen interval, in number of beacon period
    int listen_interval;
};

/// Structure for CFGMACSW_STA_REMOVE_CMD
struct cfgmacsw_sta_remove {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// MAC address of the station
    const struct mac_addr *addr;
};

/// structure for CFGMACSW_KEY_SEQNUM_CMD
struct cfgmacsw_key_seqnum {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// MAC address of the station (null for group key)
    const struct mac_addr *addr;
    /// Key index
    uint16_t key_idx;
};

/// structure for CFGMACSW_KEY_SEQNUM_RESP
struct cfgmacsw_key_seqnum_resp {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Status
    uint32_t status;
    /// Seq num
    uint64_t seqnum;
};

/// structure for CFGMACSW_SET_PS_MODE_CMD
struct cfgmacsw_set_ps_mode {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// PS mode status
    bool enabled;
    /// PS mode
    uint8_t ps_mode;
};

/// structure for CFGMACSW_TWT_SETUP_CMD
struct cfgmacsw_twt_setup_req {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// Setup request type
    uint8_t setup_type;
    /// Flow Type (0: Announced, 1: Unannounced)
    uint8_t flow_type;
    /// Wake interval Exponent
    uint8_t wake_int_exp;
    /// Unit of measurement of TWT Minimum Wake Duration (0:256us, 1:tu)
    bool wake_dur_unit;
    /// Nominal Minimum TWT Wake Duration
    uint8_t min_twt_wake_dur;
    /// TWT Wake Interval Mantissa
    uint16_t wake_int_mantissa;
};

/// structure for CFGMACSW_TWT_TEARDOWN_CMD
struct cfgmacsw_twt_teardown_req {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// TWT Negotiation type
    uint8_t neg_type;
    /// All TWT
    uint8_t all_twt;
    /// TWT flow ID
    uint8_t id;
};

enum stats_vif_type
{
    STATS_STA,
    STATS_AP,
    STATS_ADHOC,
    STATS_MAX,
};

struct cfgmacsw_get_stats_resp {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    struct ieee80211_stats stats[STATS_MAX];
    /// Status
    uint8_t status;
};

enum CFGMACSW_ME_PARAM_ID_E {
    CFGMACSW_ME_PARAM_ID_TX_SINGLE_RETRY_CNT_LIMIT,
    CFGMACSW_ME_PARAM_ID_TX_AMPDU_RETRY_CNT_LIMIT,
    CFGMACSW_ME_PARAM_ID_TX_AMPDU_PROTECT_ENABLE,
    CFGMACSW_ME_PARAM_ID_TX_AMPDU_DROP_TO_SINGLETON_RETRYCNT_THRESHOLD,
    CFGMACSW_ME_PARAM_ID_TX_POWER,
};

enum CFGMACSW_ME_PARAM_CMD_E {
    CFGMACSW_ME_PARAM_CMD_SET,
    CFGMACSW_ME_PARAM_CMD_GET,
};
/// structure for CFGMACSW_ME_PARAM_CMD
struct cfgmacsw_me_param {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// me param
    enum CFGMACSW_ME_PARAM_ID_E id;
    enum CFGMACSW_ME_PARAM_CMD_E cmd;// GET or SET
    uint8_t data[50];
};

/// structure for CFGMACSW_ME_PARAM_RESP
struct cfgmacsw_me_param_resp {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// me param
    enum CFGMACSW_ME_PARAM_ID_E id;
    enum CFGMACSW_ME_PARAM_CMD_E cmd;// GET or SET
    uint8_t data[50];
};

/// structure for CFGMACSW_GET_STA_INFO_CMD
struct cfgmacsw_get_sta_info {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// MAC address of the station
    const struct mac_addr *addr;
};

/// structure for CFGMACSW_GET_STA_INFO_RESP
struct cfgmacsw_get_sta_info_resp {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Station inactive time (msec)
    uint32_t inactive_msec;
};

/// structure for CFGMACSW_PROBE_CLIENT_CMD
struct cfgmacsw_probe_client {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// MAC address of the station
    const struct mac_addr *addr;
};

/// structure for CFGMACSW_PROBE_CLIENT_EVENT
struct cfgmacsw_probe_client_event {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// MAC address of the station
    const struct mac_addr *addr;
    /// Whether client is still present
    bool client_present;
};

/// structure for CFGMACSW_REMAIN_ON_CHANNEL_CMD
struct cfgmacsw_remain_on_channel {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif Index
    uint8_t fhost_vif_idx;
    /// Channel frequency in MHz
    unsigned int freq;
    /// Duration in ms
    unsigned int duration;
};

/// structure for CFGMACSW_CANCEL_REMAIN_ON_CHANNEL_CMD
struct cfgmacsw_cancel_remain_on_channel {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif Index
    uint8_t fhost_vif_idx;
};

/// Structure for CFGMACSW_RX_MGMT_EVENT
struct cfgmacsw_remain_on_channel_event {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif Index
    uint8_t fhost_vif_idx;
    /// Channel frequency in MHz
    unsigned int freq;
    /// Duration in ms
    unsigned int duration;
};

/// Structure for CFGMACSW_P2P_NOA_CMD
struct cfgmacsw_p2p_noa_cmd {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// Count
    uint8_t count;
    /// Duration (in us)
    uint32_t duration_us;
    /// Interval (in us)
    uint32_t interval_us;
    /// Indicate if NoA can be paused for traffic reason
    bool dyn_noa;
};

/// structure for CFGMACSW_JOIN_MESH_CMD
struct cfgmacsw_join_mesh_cmd {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// DTIM period
    uint8_t dtim_period;
    /// Beacon interval
    uint16_t bcn_int;
    /// Length of the Mesh ID
    uint8_t mesh_id_len;
    /// Mesh ID
    const uint8_t *mesh_id;
    /// Length of the provided IEs (in bytes)
    uint8_t ie_len;
    /// IEs to download
    const uint8_t *ie;
    /// Legacy rate set
    struct mac_rateset rates;
    /// Indicate if Mesh Peering Management (MPM) protocol is handled in userspace
    bool user_mpm;
    /// Operating Channel for the MESH point
    struct mac_chan_op chan;
    /// Indicate if MESH Point is using authentication
    bool is_auth;
    /// Indicate which authentication method is used
    uint8_t auth_id;
};

/// structure for CFGMACSW_LEAVE_MESH_CMD
struct cfgmacsw_leave_mesh_cmd {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif Index
    uint8_t fhost_vif_idx;
};

/// structure for CFGMACSW_MESH_PEER_UPDATE_NTF
struct cfgmacsw_mesh_peer_update_ntf {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif idx
    uint16_t fhost_vif_idx;
    /// MAC address of the station
    const struct mac_addr *addr;
    /// Mesh Link State
    uint8_t state;
};

/// Structure for CFGMACSW_NEW_PEER_CANDIDATE_EVENT
struct cfgmacsw_new_peer_candidate_event {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif Index
    uint8_t fhost_vif_idx;
    /// Peer address. Must be freed (using rtos_free) when event has been processed
    uint8_t *peer;
    /// Beacon IEs. Must be freed (using rtos_free) when event has been processed
    uint8_t *ies;
    /// Length of ies (in bytes)
    uint16_t ie_len;
};

/// structure for CFGMACSW_FTM_CMD
struct cfgmacsw_ftm_start_cmd {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// Vif Index
    uint8_t fhost_vif_idx;
};

#if MACSW_FTM_INITIATOR
/// structure for CFGMACSW_FTM_DONE_EVENT
struct cfgmacsw_ftm_done_event {
    /// header
    struct cfgmacsw_msg_hdr hdr;
    /// parameters
    uint8_t vif_idx;
    /// Results
    struct mac_ftm_results results;
};
#endif

/**
 ****************************************************************************************
 * @brief Send a CFGMACSW event to the specified socket
 *
 * Event will be entirely copied to the socket so memory pointed by msg can be re-used
 * after calling this function.
 *
 * @param[in] msg_hdr  Pointer on the header of the event to send.
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 */
int fhost_cntrl_cfgmacsw_event_send(struct cfgmacsw_msg_hdr *msg_hdr);
/**
 ****************************************************************************************
 * @brief Send a CFGMACSW command to the Control TASK and get the response.
 *
 * @param[in]     cmd    Pointer to the command header (followed by the parameters)
 * @param[in,out] resp   Pointer to the response header (followed by the space for the
 *                       response parameters). This parameter can be set to NULL if no
 *                       response is expected.
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 */
int fhost_cntrl_cfgmacsw_cmd_send(struct cfgmacsw_msg_hdr *cmd,
                                 struct cfgmacsw_msg_hdr *resp);

/**
 ****************************************************************************************
 * @brief Discard oldest event available.
 *
 * This function doesn't wait and it is meant to be called after @ref
 * fhost_cntrl_cfgmacsw_event_peek_header once the event ID is known.
 *
 * @param[in] link     Link with CNTRL task to use
 * @param[in] msg_hdr  Event header from @ref fhost_cntrl_cfgmacsw_event_peek_header
 *
 * @return 0 on success and != 0 if error occurred.
 ****************************************************************************************
 */
int fhost_cntrl_cfgmacsw_event_discard(rtos_queue resp_queue,
                                      struct cfgmacsw_msg_hdr *msg_hdr);

/**
 * @}
 */
#endif /* _CFGMACSW_H_ */
