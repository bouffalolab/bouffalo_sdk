/**
 ****************************************************************************************
 *
 * @file fhost_api.h
 *
 * @brief Public API for fully hosted module.
 *
 *
 ****************************************************************************************
 */

#ifndef _FHOST_API_H_
#define _FHOST_API_H_

#include "rtos_al.h"

/**
 ****************************************************************************************
 * @defgroup FHOST_AL FHOST_AL
 * @ingroup FHOST
 *
 * The fully hosted module requires to be ported for the RTOS and Network stack used on
 * the final devices.
 * - RTOS: See @ref FHOST_RTOS_AL for details
 * - Network Stack: See @ref FHOST_NET_AL for details
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup FHOST_API FHOST_API
 * @ingroup FHOST
 * @brief Fully Hosted public API.
 *
 * This module list the public API provided by the fully hosted module for its usage in
 * external application/integration layers.
 *
 * It also defines the few functions that need to be implemented by the integration layer.
 *
 * @{
 ****************************************************************************************
 */

#include "net_al.h"
#include "net_def.h"
#include "macsw.h"

enum vif_ip_set_ret {
    VIF_SET_SUCCESS = 0,
    VIF_IDX_ERR = -1,
    VIF_NETIF_NULL = -2,
    DHCPC_START_FAILED = -3,
    DHCPC_START_TIMEOUT = -4,
    DHCPC_START_ABORT = -5,
    VIF_MODE_ERR = -6,
};
/**
 * FHOST configuration item description
 */
struct fhost_config_item
{
    /**
     * Id of the configuration item (@ref fhost_config_id)
     */
    uint16_t id;
    /**
     * Size in bytes of data buffer
     */
    uint16_t len;
    /**
     * Configuration data
     */
    void *data;
};

/**
 * IDs for FHOST configuration items
 */
enum fhost_config_id
{
    /**
     * Indicates last configuration item
     */
    FHOST_CFG_END = 0,
    /**
     * - use: Configure MAC address
     * - len: 6
     * - data: MAC address
     */
    FHOST_CFG_MAC_ADDR,
    /**
     * - use: Configure usage of Power Save mode
     * - len: 4
     * - data: 32bits boolean
     */
    FHOST_CFG_PS,
    /**
     * - use: Configure usage of Short Guard interval for 20MHz and 40MHz channels.
     * - len: 4
     * - data: 32bits boolean
     */
    FHOST_CFG_SGI,
    /**
     * - use: Configure usage of Short Guard interval for 80MHz channels.
     * - len: 4
     * - data: 32bits boolean
     */
    FHOST_CFG_SGI80,
    /**
     * - use: Configure the maximum number of Spatial Stream used.
     * The minimum value is 1. The maximum and default value is the number of SS
     * supported by the modem. Values out of the this range are ignored.
     * - len: 4
     * - data: 32bits val
     */
    FHOST_CFG_NSS,
    /**
     * - use: Configure tx lifetime.
     * - len: 4
     * - data: 32bits val, in ms
     */
    FHOST_CFG_TX_LFT,
    /**
     * - use: Configure UAPSD timeout (for station).
     * - len: 4
     * - data: 32bits val, in ms
     */
    FHOST_CFG_UAPSD_TIMEOUT,
    /**
     * - use: Configure Low Power clock accuracy.
     * - len: 4
     * - data: 32bits val, in ppm
     */
    FHOST_CFG_LP_CLK_ACCURACY,
    /**
     * - use: Configure usage of HT.
     * - len: 4
     * - data: 32bits boolean
     */
    FHOST_CFG_HT,
    /**
     * - use: Configure usage of 40MHz in HT.
     * - len: 4
     * - data: 32bits boolean
     */
    FHOST_CFG_40MHZ,
    /**
     * - use: Configure usage of VHT.
     * - len: 4
     * - data: 32bits boolean
     */
    FHOST_CFG_VHT,
    /**
     * - use: Configure maximum MCS for VHT. (Ignored if SW or HW doesn't support VHT)
     * - len: 4
     * - data: 32bits val. 0=MCS0-7, 1=MCS0-8, 2=MCS0-9
     */
    FHOST_CFG_VHT_MCS,
    /**
     * - use: Configure advertisement of LDPC RX support in HT/VHT.
     * (Ignored if modem doesn't support it)
     * - len: 4
     * - data: 32bits boolean.
     */
    FHOST_CFG_LDPC,
    /**
     * - use: Configure advertisment of STBC RX support in VHT.
     * (Ignored if modem doesn't support it)
     * - len: 4
     * - data: 32bits boolean.
     */
    FHOST_CFG_STBC,
    /**
     * - use: Configure advertisment of BEAMFORMEE support in VHT.
     * (Ignored if modem doesn't support it)
     * - len: 4
     * - data: 32bits boolean.
     */
    FHOST_CFG_BFMEE,
    /**
     * - use: Configure advertisment of BEAMFORMER support in VHT.
     * (Ignored if modem doesn't support it)
     * - len: 4
     * - data: 32bits boolean.
     */
    FHOST_CFG_BFMER,
    /**
     * - use: Configure advertisment of Multi User RX support in VHT.
     * (Ignored if modem doesn't support it)
     * - len: 4
     * - data: 32bits boolean.
     */
    FHOST_CFG_MURX,
    /**
     * - use: Configure advertisment of Multi User TX support in VHT.
     * (Ignored if modem doesn't support it)
     * - len: 4
     * - data: 32bits boolean.
     */
    FHOST_CFG_MUTX,
    /**
     * - use: Configure karst radio. (Ignored if other radio is used)
     * - len: sizeof(struct phy_karst_cfg_tag)
     * - data: struct phy_karst_cfg_tag.
     */
    FHOST_CFG_PHY_KARST,
    /**
     * - use: Configure trident radio. (Ignored if other radio is used)
     * - len: sizeof(struct phy_trd_cfg_tag)
     * - data: struct phy_trd_cfg_tag.
     */
    FHOST_CFG_PHY_TRD,
    /**
     * - use: Configure usage of Antenna Diversity.
     * - len: 4
     * - data: 32bits boolean
     */
    FHOST_CFG_ANT_DIV,
    /**
     * - use: Configure number of seconds since Epoch at boot.
     * - len: 4
     * - data: 32bits unsigned value
     */
    FHOST_CFG_EPOCH_SEC,
    /**
     * - use: Configure number of microseconds since Epoch at boot.
     * - len: 4
     * - data: 32bits unsigned value
     */
    FHOST_CFG_EPOCH_USEC,
    /**
     * - use: Configure usage of HE.
     * - len: 4
     * - data: 32bits boolean
     */
    FHOST_CFG_HE,
    /**
     * - use: Configure maximum MCS for HE. (Ignored if SW or HW doesn't support HE)
     * - len: 4
     * - data: 32bits val. 0=MCS0-7, 1=MCS0-9, 2=MCS0-11
     */
    FHOST_CFG_HE_MCS,
    /**
     * - use: Configure usage of 80Mhz in VHT/HE.
     * - len: 4
     * - data: 32bits boolean
     */
    FHOST_CFG_80MHZ,
    /**
     * - use: Configure usage of Dynamic Power-Save mode.
     * - len: 4
     * - data: 32bits boolean
     */
    FHOST_CFG_DPSM,
    /**
     * - use: Configure UAPSD queues (for station).
     * - len: 4
     * - data: 32bits bitfield (Bit0 = VO, Bit1 = VI, Bit2 = BK, Bit3 = BE)
     */
    FHOST_CFG_UAPSD_QUEUES,
    FHOST_CFG_MAX,
};

/**
 * Fully hosted module status
 */
struct fhost_status
{
    /**
     * Maximum number of interface supported
     */
    int vif_max_cnt;
    /**
     * Number of active interface
     */
    int vif_active_cnt;
    /**
     * Index of the first active interface. (Valid only if vif_active_cnt is not 0)
     */
    int vif_first_active;
    /**
     * Number of channel supported on 2.4GHz band
     */
    int chan_2g4_cnt;
    /**
     * Number of channel supported on 5GHz band
     */
    int chan_5g_cnt;
    /**
     * List of channels supported on 2.4GHz band
     */
    const struct mac_chan_def *chan_2g4;
    /**
     * List of channels supported on 5GHz band
     */
    const struct mac_chan_def *chan_5g;
};

/**
 * Fully hosted interface status
 */
struct fhost_vif_status
{
    /**
     * ID on the interface
     */
    int index;
    /**
     * Type of the interface (@ref mac_vif_type)
     */
    int type;
    /**
     * MAC address of the interface
     */
    const uint8_t *mac_addr;
    /**
     * Current operating channel.
     * Reset to 0 if there is no operating channel associated the interface
     * (e.g. non connected STA interface)
     */
    struct mac_chan_op chan;
    union {
        /**
         * Fields specific to a STA interface
         */
        struct {
            /**
             * BSSID of the AP. (Set to 0 if interface is not connected)
             */
            struct mac_addr bssid;
            /// AID of the station
            uint16_t aid;
            /**
             * RSSI (in dBm) of the last received beacon. (valid only if connected)
             */
            int8_t rssi;
        } sta;
    };
};

/**
 * Fully hosted frame information
 */
struct fhost_frame_info
{
    /**
     * Interface index that received the frame. (-1 if unknown)
     */
    int fvif_idx;
    /**
     * Length (in bytes) of the frame.
     */
    uint16_t length;
    /**
     * Primary channel frequency (in MHz) on which the frame has been received.
     */
    uint16_t freq;
    /**
     * Received signal strength (in dBm)
     */
    int8_t rssi;
    /**
     * Frame payload. Can be NULL if monitor mode is started with @p uf parameter set to
     * true. In this case all other fields are still valid.
     */
    uint8_t *payload;
};

/**
 * Function prototype for RX callbacks
 */
typedef void (*cb_fhost_rx)(struct fhost_frame_info *info, void *arg);

/**
 * Configuration of VIF_MONITOR interface
 */
struct fhost_vif_monitor_cfg
{
    /**
     * Channel to monitor.
     * @ref fhost_configure_vif will fail if channel is not valid. It will also limit
     * the tx power.
     */
    struct mac_chan_op chan;
    /**
     * Report 'Unsupported Frames'.
     * If true, callback may be called with a payload parameter set to NULL
     */
    bool uf;
    /**
     * Frame received callback.
     * @attention The memory passed as parameter is only valid until it returns.
     */
    cb_fhost_rx cb;
    /**
     * Parameter for the monitor callback
     */
    void *cb_arg;
};

/**
 * Configuration for VIF_STA interface
 */
struct fhost_vif_sta_cfg
{
    /**
     * SSID to connect to (mandatory)
     */
    struct mac_ssid ssid;
    /**
     * AP password/PSK passed as a string (i.e. MUST be terminated by a null byte)
     */
    char key[64 + 1];
    /**
     * AP BSSID. Optional, clear it to 0 if not set.
     */
    struct mac_addr bssid;
    /**
     * AP frequency. Optional, you can specify up to two frequencies on which AP
     * will be scanned. This is to speed up connection and should be set 0 if no used.
     */
    uint16_t freq[2];
    /**
     * AP scan duration, In TUs.
     */
    uint16_t duration;
    /**
     * AP scan probe request TX count
     */
    uint16_t probe_cnt;
    /**
     * Auth and Assoc timeout, in sec. 0, use default value
     */
    uint8_t auth_timeout;
    /**
     * Timeout before EAPOL 1 after associtiated, in sec. 0, use default value
     */
    uint8_t eapol_1_timeout;
    /**
     * Remaining EAPOL session timeout, in sec. 0, use default value
     */
    uint8_t eapol_rem_timeout;
    /**
     * Bit field of Authentication and Key Management (@ref mac_akm_suite) accepted
     * for the connection.
     * Supported value: \n
     * - For an AP using WEP protocol select MAC_AKM_PRE_RSN
     * - For an AP using WPA1 protocol select MAC_AKM_PRE_RSN | MAC_AKM_PSK
     * - For an AP using WPA2 protocol select MAC_AKM_PSK
     * - For an AP using WPA3 protocol select MAC_AKM_SAE
     * - For an AP with no security select MAC_AKM_NONE
     * It is also possible to mix WPAx configuration, but not WEP and WPA.
     * For example MAC_AKM_PSK | MAC_AKM_SAE will allow connection to WPA2/WPA3 AP
     *
     * If set to 0, it default to MAC_AKM_NONE if @p key is the empty string,
     * MAC_AKM_PRE_RSN if the key length is less than 8 and
     * MAC_AKM_PRE_RSN | MAC_AKM_PSK | MAC_AKM_SAE otherwise.
     */
    uint32_t akm;
    /**
     * Whether PMF is enabled.
     *
     * Supported value: 0/1/2.
     * Passed to supplicant parameter ieee80211w.
     */
    uint8_t pmf;
    /**
     * Bit field of pairwise cipher suites (@ref mac_cipher_suite) allowed for the
     * connection. Used only for WPAx protocol (i.e. when akm include MAC_AKM_PSK
     * or MAC_AKM_SAE).
     * If set to 0, accept all supported cipher.
     */
    uint32_t unicast_cipher;
    /**
     * Bit field of cipher suites (@ref mac_cipher_suite) accepted as group key for
     * the connection. Used only for WPAx security protocol (i.e. when akm include
     * MAC_AKM_PSK or MAC_AKM_SAE).
     * If set to 0, accept all supported cipher.
     */
    uint32_t group_cipher;
    /**
     * Connection timeout in ms. If connection doesn't complete before timeout
     * @ref fhost_configure_vif will return an error.
     * If set to -1 timeout is disabled, and if set to 0 @ref fhost_configure_vif
     * will return without waiting for the connection to complete.
     */
    int timeout_ms;

    /* Extra flags passed to wpa_supplicant. See FHOST_WPA_SSID_* in fhost_wpa.h. */
    int wpa_flags;
};


/**
 * AP mode
 */
enum fhost_ap_mode
{
    /**
     * Automatic mode selection
     */
    FHOST_AP_AUTO = 0,
    /**
     * Use non HT mode, 11g or 11a depending on the selected channel
     */
    FHOST_AP_NON_HT,
    /**
     * Use HT (11n) mode
     */
    FHOST_AP_HT,
    /**
     * Use VHT (11ac) mode. Fallback to HT if VHT is not possible
     */
    FHOST_AP_VHT,
    /**
     * Use HE (11ax) mode. Fallback to VHT or HT if HE is not possible
     */
    FHOST_AP_HE,
};

/**
 * Configuration for VIF_AP and VIF_MESH_POINT interface
 */
struct fhost_vif_ap_cfg
{
    /**
     * SSID to connect to (mandatory)
     */
    struct mac_ssid ssid;
    /**
     * AP operating channel (tx_power and flags fields are ignored).
     */
    struct mac_chan_op chan;
    /**
     * Beacon interval in TU
     */
    int bcn_interval;
    /**
     * DTIM period (number of beacon between 2 DTIM beacon)
     */
    int dtim_period;
    /**
     * AP operation mode (may be silently changed if not supported)
     */
    enum fhost_ap_mode mode;
    /**
     * Bit field of Authentication and Key Management (@ref mac_akm_suite)
     * Supported combination are:\n
     * - MAC_AKM_NONE : for open security
     * - MAC_AKM_PRE_RSN : for WEP security (deprecated)
     * - MAC_AKM_PRE_RSN | MAC_AKM_PSK : For WPA1 security (deprecated)
     * - MAC_AKM_PSK : For WPA2 security
     * - MAC_AKM_SAE : For WPA3 security
     * - MAC_AKM_PSK | MAC_AKM_SAE : For WPA2/WPA3 security
     */
    uint32_t akm;
    /**
     * AP password/PSK passed as a string (i.e. MUST be terminated by a null byte)
     */
    char key[64 + 1];
    /**
     * Bit field of Cipher suites (@ref mac_cipher_suite) allowed for pairwise key
     * if AP security protocol is WPAx. Several ciphers can be selected and unsupported
     * ciphers are silently ignored (unless they are all unsupported).
     * Can be left to 0 to use default one (depending of AKM selected):\n
     * - WPA1: TKIP
     * - WPA2/WPA3: CCMP
     */
    uint32_t unicast_cipher;
    /**
     * Bit field of Cipher suites (@ref mac_cipher_suite) allowed for group key
     * if AP security protocol is WPAx. Only one cipher can be selected.
     * Can be left to 0 to use default one (depending of AKM selected)
     * - WPA1: TKIP
     * - WPA2/WPA3: CCMP
     */
    uint32_t group_cipher;
    /**
     * Management Frame Protection (0: disable, 1: allowed, 2: required)
     * (Ignored if not supported by selected AKM)
     */
    int mfp;
    /**
     * This fhost_vif_ap_cfg structure is transformed into wpa_supplicant configuration.
     * For clarity, not all configuration options provided by wpa_supplicant are exposed
     * and this field allow to overcome this limitation by and directly passing
     * wpa_supplicant configuration option.
     * The string should have the following format: <option1> <value1>; <option2> <value2>;...
     * For example: "ap_max_inactivity 500;"
     */
    char *extra_cfg;
    /**
     * isolation mode (on/off)
     * on: don't send back to TX the unicast frames that are destined to a STA connected to the AP
     * off (default): send back to TX the unicast frames that are destined to a STA connected to the AP
     */
    bool isolation_mode;
    /**
     * hidden ssid (on/off)
     */
    bool hidden_ssid;
    /**
     * Basic rates for MESH
     */
    int basic_rates[MAC_RATESET_LEN];

    /**
     * Additional vendor specific elements for Beacon and Probe Response frames
     */
    char *ap_vendor_elements;

    /// Disable advertising WME/WMM Information Element in Beacon/ProbeResponse frames
    bool disable_wmm;
};

/**
 * Fully hosted interface configuration
 */
struct fhost_vif_cfg
{
    /**
     * Identify which part of the union has been updated
     */
    enum mac_vif_type cfg_type;
    union {
        /**
         * Configuration if @p cfg_type == VIF_STA
         */
        struct fhost_vif_sta_cfg sta;
        /**
         * Configuration if @p cfg_type == VIF_MONITOR
         */
        struct fhost_vif_monitor_cfg monitor;
        /**
         * Configuration if @p cfg_type == VIF_AP
         */
        struct fhost_vif_ap_cfg ap;
        /**
         * Configuration if @p cfg_type == VIF_MESH_POINT
         */
        struct fhost_vif_ap_cfg mesh_point;
    };
};

/**
 * Function prototype for frame transimission callbacks
 * - frame_id is the one returned by @ref fhost_send_80211_frame
 * - acknowledged indicates if the frame has been acknowledged by peer (or sucessfully sent
 *   for multicast frame)
 * - arg is the private argument passed when frame has been pushed for transmission
 */
typedef void (*cb_fhost_tx)(uint32_t frame_id, bool acknowledged, void *arg);

/*****************************************************************************************
 * Provided Functions
 ****************************************************************************************/
/**
 ****************************************************************************************
 * @brief Initialization of the fully hosted module.
 *
 * This function allocates all the resources needed by the different FHOST sub-modules.
 * This function will use the rtos_al API to create tasks, semaphores, etc. As such it
 * must be called after @ref rtos_init (or equivalent).
 * The user shall call @ref fhost_wait_ready to ensure that all sub-modules have been
 * fully initialized
 *
 * @param[in] init_mac  Whether to initialize LMAC/UMAC layers or not
 *
 * @return 0 on success and != 0 if error occurred.
 * now init in fw firstly
 ****************************************************************************************
 */
int fhost_init();

/**
 ****************************************************************************************
 * @brief Waits until fully hosted module is ready.
 *
 * This function will block until fully hosted module is ready to be used. Fully hosted
 * module is ready when all its tasks (created by @ref fhost_init) finished their
 * initialization.
 * It is not safe to call any of the functions defined thereafter without ensuring first
 * that the fully hosted module is ready.
 *
 * @return 0 on success and != 0 if error occurred
 ****************************************************************************************
 */
int fhost_wait_ready(void);

/**
 ****************************************************************************************
 * @brief Retrieve fully hosted module status.
 *
 * @param[out] status Structure to retrieve status
 ****************************************************************************************
 */
void fhost_get_status(struct fhost_status *status);

/**
 ****************************************************************************************
 * @brief Retrieve one interface status.
 *
 * @param[in]  fvif_idx  Index of the interface
 * @param[out] status    Structure to retrieve status
 *
 * @return 0 on success and != 0 if error occurred
 ****************************************************************************************
 */
int fhost_get_vif_status(int fvif_idx, struct fhost_vif_status *status);

/**
 ****************************************************************************************
 * @brief Retrieve one interface raw state.
 *
 * @param[in]  fvif_idx  Index of the interface
 *
 * @return raw state of the interface
 ****************************************************************************************
 */
int fhost_get_vif_raw_state(int fvif_idx);

/**
 ****************************************************************************************
 * @brief Retrieve one interface state.
 *
 * @param[in]  fvif_idx  Index of the interface
 *
 * @return state of the interface
 ****************************************************************************************
 */
int fhost_get_vif_state(int fvif_idx);

/**
 ****************************************************************************************
 * @brief Change interface type
 *
 * When the interface type is changed, the current interface will first be cleared (i.e.
 * close its connection, release its IP address ...)
 * If interface is already of the requested type the function returns immediately.
 * If you want to clear an interface without changing its type, you should first set its
 * type to VIF_UNKNOWN and then reset it.
 *
 * @param[in] link      Pointer to a cfgmacsw link
 * @param[in] fvif_idx  Index of the interface
 * @param[in] type      Type to set on the interface
 * @param[in] p2p       Whether interface will be used for a P2P (aka Wifi Direct)
 *                      connection (Used only if @p type is VIF_STA of VIF_AP)
 *
 * @return 0 on success and <0 if any error occurred.
 ****************************************************************************************
 */
int fhost_set_vif_type(rtos_queue resp_queue, int fvif_idx, enum mac_vif_type type,
                       bool p2p);

/**
 ****************************************************************************************
 * @brief Configure an interface
 *
 * This function configures an interface.
 * The processing done depend of the interface type:
 * - MONITOR: Set the radio channel
 * - STA: Start wpa_supplicant to connect to an AP
 * - AP: Start an Access Point
 * - MESH_POINT: Start a MESH Point
 * The configuration type (i.e. fhost_vif_cfg.cfg_type) must match the interface type
 * otherwise an error is returned.
 *
 * @param[in] link      Pointer to a cfgmacsw link
 * @param[in] fvif_idx  Index of the interface
 * @param[in] cfg       Configuration of the interface
 *
 * @return 0 on success and <0 if any error occurred.
 ****************************************************************************************
 */
int fhost_configure_vif(rtos_queue resp_queue, int fvif_idx,
                        struct fhost_vif_cfg *cfg);

/**
 ****************************************************************************************
 * @brief Scan Access Points on a given interface
 *
 * This function is blocking until the scan is finished. Once the scan is complete you
 * can use @ref fhost_get_scan_results to retrieve the results.
 *
 * @param[in] link      Pointer to a cfgmacsw link
 * @param[in] fvif_idx  Index of the interface
 *
 * @return < 0 if an error occurred, otherwise it returns the number of Access point
 * found.
 ****************************************************************************************
 */
int fhost_scan(rtos_queue resp_queue, int fvif_idx);

/**
 ****************************************************************************************
 * @brief Retrieve scan results
 *
 * This function retrieves up to @p max_nb_result scan results starting at index
 * @p result_idx.
 * To retrieve all scan results, first call this function with result_idx = 0;
 * Then as long as the value returned is equal to @p max_nb_result you can retrieve the
 * other results by updating @p result_idx value (incremented by the number of results
 * already read).
 * All results have been read when the value returned is lower than @p max_nb_result.
 *
 * @param[in]  link           Pointer to a cfgmacsw link
 * @param[in]  result_idx     Index of the first result to read
 * @param[in]  max_nb_result  Size, in number of element, of @p results
 * @param[out] results        Buffer to retrieve the scan results.
 *
 * @return The number of results written in @p results.
 ****************************************************************************************
 */
int fhost_get_scan_results(rtos_queue resp_queue, int result_idx,
                           int max_nb_result, struct mac_scan_result *results);

/**
 ****************************************************************************************
 * @brief Transmit a 802.11 frame
 *
 * This function is used to send a pre-formatted 802.11 frame (most likely a Management
 * frame). The frame is sent as a singleton using  basic rates.
 * The memory passed as parameter is first copied before programming the transmission.
 * It can then be re-used immediately once the function returns.
 *
 * @attention If callback function is provided it will be executed in WIFI task context
 * and as such it should be fairly quick and cannot call any function of this API.
 *
 * @param[in] fvif_idx    Index of the interface
 * @param[in] frame       Pointer to the data to send
 * @param[in] length      Size, in bytes, of the provided data
 * @param[in] no_cck      Whether CCK rates must not be used to transmit this frame
 * @param[in] cfm_cb      Callback when transmission has been completed (may be NULL)
 * @param[in] cfm_cb_arg  Private argument for the callback.
 *
 * @return A unique frame id (the one passed in the confirmation callback) and 0/1/2/3 if
 * an error happened
 ****************************************************************************************
 */
uint32_t fhost_send_80211_frame(int fvif_idx, const uint8_t *frame, uint32_t length,
                                int no_cck, cb_fhost_tx cfm_cb, void *cfm_cb_arg);

/**
 ****************************************************************************************
 * @brief Start FTM measurements on a given interface
 *
 * This function is blocking until the FTM procedure is finished. The results of the
 * measurements are written to the dedicated structure passed as parameter.
 *
 * @param[in]  link      Pointer to a cfgmacsw link
 * @param[in]  fvif_idx  Index of the interface
 * @param[out] res       Pointer to the result structure
 *
 * @return < 0 if an error occurred, 0 otherwise.
 ****************************************************************************************
 */
int fhost_ftm(rtos_queue resp_queue, int fvif_idx,
              struct mac_ftm_results *res);

/// @}

#endif // _FHOST_API_H_
