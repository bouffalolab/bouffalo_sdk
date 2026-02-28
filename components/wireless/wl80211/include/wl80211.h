#ifndef __WL80211_H__
#define __WL80211_H__

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/queue.h>
#include <sys/tree.h>

#define INVARIANTS
#define COMPAT_WIFI_MGMR

#ifndef container_of
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

/* async event type */
#define EV_WL80211     ((uintptr_t)wl80211_post_event)

#define ALIGN4_HI(val) (((val) + 3) & ~3)

#if defined(__NuttX__)
#include <sys/uio.h>
#elif !defined(IOVEC_DEFINED)
#define IOVEC_DEFINED
struct iovec {
    void *iov_base;
    size_t iov_len;
};
#endif

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
#define WL80211_SCAN_AP_RESULT_FLAGS_HAS_RSN  0x1
#define WL80211_SCAN_AP_RESULT_FLAGS_HAS_WPA  0x2
#define WL80211_SCAN_AP_RESULT_FLAGS_HAS_CCMP 0x4
#define WL80211_SCAN_AP_RESULT_FLAGS_HAS_TKIP 0x8
#define WL80211_SCAN_AP_RESULT_FLAGS_HAS_WPS  0x10
    uint8_t flags;

    uint32_t key_mgmt : 20;
    uint32_t mode     : 16;
};

int bssidcmp(struct wl80211_scan_result_item *e1, struct wl80211_scan_result_item *e2);

/* scan result tree */
RB_HEAD(_scan_result_tree, wl80211_scan_result_item);
extern struct _scan_result_tree wl80211_scan_result;
RB_PROTOTYPE(_scan_result_tree, wl80211_scan_result_item, entry, bssidcmp);

struct wl80211_scan_ops {
    STAILQ_ENTRY(wl80211_scan_ops) entry;
    void (*scan_start)(struct wl80211_scan_ops *);
    void (*scan_ap_ind)(struct wl80211_scan_ops *, struct wl80211_scan_ap_ind *, uint8_t *, uint16_t);
    void (*scan_done_ind)(struct wl80211_scan_ops *);
};

struct aid_list_entry {
    uint8_t mac[6];
    void *wpa_sm;
    uint8_t used;
    uint8_t sta_idx;
};

enum wl80211_auth_type {
    WL80211_AUTHTYPE_OPEN_SYSTEM,
    WL80211_AUTHTYPE_SHARED_KEY,
    WL80211_AUTHTYPE_SAE,

    /* keep last */
    __WL80211_AUTHTYPE_NUM,
    WL80211_AUTHTYPE_MAX = __WL80211_AUTHTYPE_NUM - 1,
    WL80211_AUTHTYPE_AUTOMATIC
};

enum wl80211_mfp {
    WL80211_MFP_NO,
    WL80211_MFP_REQUIRED,
    WL80211_MFP_OPTIONAL,
};

enum wl80211_vif_type {
    WL80211_VIF_STA,
    WL80211_VIF_AP,
    WL80211_VIF_MAX,
};

enum wl80211_phy_band {
    WL80211_BAND_2G4,
    WL80211_BAND_5G,
    WL80211_BAND_MAX,
};

enum wl80211_chan_width {
    WL80211_CHAN_WIDTH_20,
    WL80211_CHAN_WIDTH_40,
    WL80211_CHAN_WIDTH_80,
    WL80211_CHAN_WIDTH_160,
    WL80211_CHAN_WIDTH_80P80,
};

enum ieee80211_channel_flags {
    IEEE80211_CHAN_DISABLED = 1 << 0,
    IEEE80211_CHAN_NO_IR = 1 << 1,
    /* hole at 1<<2 */
    IEEE80211_CHAN_RADAR = 1 << 3,
    IEEE80211_CHAN_NO_HT40PLUS = 1 << 4,
    IEEE80211_CHAN_NO_HT40MINUS = 1 << 5,
    IEEE80211_CHAN_NO_OFDM = 1 << 6,
    IEEE80211_CHAN_NO_80MHZ = 1 << 7,
    IEEE80211_CHAN_NO_160MHZ = 1 << 8,
    IEEE80211_CHAN_INDOOR_ONLY = 1 << 9,
    IEEE80211_CHAN_IR_CONCURRENT = 1 << 10,
    IEEE80211_CHAN_NO_20MHZ = 1 << 11,
    IEEE80211_CHAN_NO_10MHZ = 1 << 12,
    IEEE80211_CHAN_NO_HE = 1 << 13,
    IEEE80211_CHAN_1MHZ = 1 << 14,
    IEEE80211_CHAN_2MHZ = 1 << 15,
    IEEE80211_CHAN_4MHZ = 1 << 16,
    IEEE80211_CHAN_8MHZ = 1 << 17,
    IEEE80211_CHAN_16MHZ = 1 << 18,
};

#define IEEE80211_CHAN_NO_HT40 (IEEE80211_CHAN_NO_HT40PLUS | IEEE80211_CHAN_NO_HT40MINUS)

struct wl80211_connect_params {
    uint8_t ssid[33];
    uint8_t bssid[6];
    uint8_t password[65];
    //const uint8_t *pmk;

    uint8_t channel;

    uint8_t auth_type; // ref @ enum wl80211_auth_type
    uint8_t mfp;       // ref @ enum wl80211_mfp

    const uint8_t *ie;
    uint16_t ie_len;

    uint32_t flags;
};

struct wl80211_scan_params {
    const uint8_t *ssid;
    const uint8_t *bssid;
    uint8_t ssid_len;

    int channels_cnt;
    const uint8_t *channels;

    size_t ie_len;
    const uint8_t *ie;

    /* channel stay duration = probe_interval(TUs) * probe_cnt */
    uint32_t probe_cnt;
    uint32_t probe_interval;
#define WL80211_SCAN_FLAGS_PASSIVE 0x1
#define WL80211_SCAN_FLAGS_NOCCK   0x2
#define WL80211_SCAN_FLAGS_JOIN    0x4
    uint32_t flags;
};

struct wl80211_ap_settings {
    uint8_t ssid[33];
    uint8_t password[65];

    /* channel settings */
    uint8_t channel_width; // ref @ enum wl80211_chan_width
    uint32_t center_freq1;
    uint32_t center_freq2;
    uint32_t channel_flags; // ref @ enum ieee80211_channel_flags
    unsigned int max_power;

    /* ap settings */
    unsigned int beacon_interval;
    uint8_t auth_type; // ref @ enum wl80211_auth_type
    const uint8_t *ie;
    uint16_t ie_len;
    uint32_t flags;
};

struct ieee80211_dot_d {
    char *code;
    uint8_t channel24G_num;
    uint8_t channel5G_num;
    uint8_t channel24G_chan[14];
    uint8_t channel5G_chan[28];
};

typedef void (*wl80211_monitor_rx_cb_t)(void *ctx, void *pkt, size_t len, size_t mac_hdr_len, int rssi);
struct wl80211_monitor_settings {
    /* channel settings */
    enum wl80211_chan_width channel_width;
    uint32_t center_freq1;
    uint32_t center_freq2;

    void *recv_ctx;
    wl80211_monitor_rx_cb_t recv;
};

/* wl80211 global state */
struct wl80211_global_state {
    const struct ieee80211_dot_d *country;

    /* global status */
    unsigned int scanning       : 1;
    unsigned int connecting     : 1;
    unsigned int associated     : 1;
    unsigned int authenticating : 1;
    unsigned int link_up        : 1;
    unsigned int ap_en          : 1;
    unsigned int monitor_en     : 1;

    /* station settings */
    struct wl80211_connect_params *connect_params, *last_connect_params;
    /* sta info */
    uint8_t security; // FIXME
    uint16_t aid;

    /* scan param */
    STAILQ_HEAD(, wl80211_scan_ops) scan_ops;

    /* last disconnect reason */
    uint16_t status_code;
    uint16_t reason_code;

    /* ap settings */
    uint8_t ap_ssid[33];
    uint8_t ap_password[65];
    uint8_t ap_chan_band;
    uint16_t ap_chan_freq;
    uint16_t ap_beacon_interval;
    uint8_t ap_hidden_ssid;
    uint8_t *ap_ie;
    uint16_t ap_ie_len;
    uint8_t assoc_sta_count;
    struct aid_list_entry aid_list[4];
};
extern struct wl80211_global_state wl80211_glb;

/* Input callback function type */
typedef int (*wl80211_input_cb_t)(void *prv, uint8_t vif_type, void *rxhdr, void *buf, uint32_t frm_len,
                                  uint32_t status);

void wl80211_init(void);
int wl80211_printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

void wl80211_tcpip_input(uint8_t vif_type, void *rxhdr, void *buf, uint32_t frm_len, uint32_t status);
int wl80211_output_raw(uint8_t vif_type, void *buffer, uint16_t len, unsigned int flags, void (*cb)(void *),
                       void *opaque);

/* Input callback registration interface - allows external modules to register receive callback */
int wl80211_register_input_cb(wl80211_input_cb_t cb, void *prv);
int wl80211_unregister_input_cb(void);

enum {
    WL80211_EVT_SCAN_DONE,
    WL80211_EVT_STA_CONNECTED,
    WL80211_EVT_STA_DISCONNECTED,

    WL80211_EVT_AP_STARTED,
    WL80211_EVT_AP_STOPPED,
    WL80211_EVT_AP_STA_ADDED,
    WL80211_EVT_AP_STA_DEL,
};

// post wifi event in async
void wl80211_post_event(int code1, int code2);

// wl80211 control command
enum {
    WL80211_CTRL_SCAN,
    WL80211_CTRL_STA_CONNECT,     // param freq flags
    WL80211_CTRL_STA_SET_PS_MODE, // set wifi ps mode
    WL80211_CTRL_STA_DISCONNECT,

    WL80211_CTRL_STA_GET_MAC,
    WL80211_CTRL_STA_GET_RSSI,

    WL80211_CTRL_AP_SET_SSID,
    WL80211_CTRL_AP_SET_PASSWORD,
    WL80211_CTRL_AP_START,
    WL80211_CTRL_AP_STOP,

    WL80211_CTRL_MONITOR_START,
    WL80211_CTRL_MONITOR_STOP,

    WL80211_CTRL_INJECT_FRAME,
};

// wl80211 control api
// Return value: 0 on success, negative value on error
int wl80211_cntrl(int cmd, ...);

/**
 * Start WiFi scan
 *
 * Return value:
 *   0  : Success
 *   -1 : Country not set
 *   -2 : Scan not allowed in monitor mode
 */
static inline int wl80211_scan(struct wl80211_scan_params *param)
{
    return wl80211_cntrl(WL80211_CTRL_SCAN, param);
}

/**
 * Connect to WiFi AP as STA
 *
 * Return value:
 *   0  : Success
 *   -1 : Connect not allowed in monitor mode
 *   -2 : Connect already in progress
 *   -3 : Invalid SSID
 *   -4 : Invalid channel
 *   -5 : Invalid IE parameter
 */
static inline int wl80211_sta_connect(struct wl80211_connect_params *param)
{
    return wl80211_cntrl(WL80211_CTRL_STA_CONNECT, param);
}

/**
 * Disconnect from WiFi AP
 *
 * Return value: 0 on success
 */
static inline int wl80211_sta_disconnect(void)
{
    return wl80211_cntrl(WL80211_CTRL_STA_DISCONNECT);
}

static inline int _channel_to_freq(uint8_t band, int channel) // ref @ enum wl80211_phy_band
{
    if ((band == WL80211_BAND_2G4) && (channel >= 1) && (channel <= 14)) {
        if (channel == 14) {
            return 2484;
        } else {
            return 2407 + channel * 5;
        }
    } else if ((band == WL80211_BAND_5G) && (channel >= 1) && (channel <= 177)) {
        return 5000 + channel * 5;
    }

    return 0;
}

static inline int _freq_to_channel(uint8_t band, int freq) // ref @ enum wl80211_phy_band
{
    if ((band == WL80211_BAND_2G4) && (freq >= 2412) && (freq <= 2484)) {
        if (freq == 2484) {
            return 14;
        } else {
            return (freq - 2407) / 5;
        }
    } else if ((band == WL80211_BAND_5G) && (freq >= 5005) && (freq <= 5885)) {
        return (freq - 5000) / 5;
    }

    return 0;
}

// return -1: freq not vaild; 0: 2.4G; 1: 5G
static inline int wl80211_freq_check(int freq)
{
    int i;

    if (!wl80211_glb.country) {
        return -1;
    }

    for (i = 0; i < wl80211_glb.country->channel24G_num; i++) {
        if (freq == _channel_to_freq(WL80211_BAND_2G4, wl80211_glb.country->channel24G_chan[i])) {
            return WL80211_BAND_2G4;
        }
    }

    for (i = 0; i < wl80211_glb.country->channel5G_num; i++) {
        if (freq == _channel_to_freq(WL80211_BAND_5G, wl80211_glb.country->channel5G_chan[i])) {
            return WL80211_BAND_5G;
        }
    }

    return -1;
}

static inline int wl80211_channel_check(int channel)
{
    int i;

    if (!wl80211_glb.country) {
        return -1;
    }

    for (i = 0; i < wl80211_glb.country->channel24G_num; i++) {
        if (channel == wl80211_glb.country->channel24G_chan[i]) {
            return WL80211_BAND_2G4;
        }
    }

    for (i = 0; i < wl80211_glb.country->channel5G_num; i++) {
        if (channel == wl80211_glb.country->channel5G_chan[i]) {
            return WL80211_BAND_5G;
        }
    }

    return -1;
}

static inline int wl80211_channel_to_freq(int channel)
{
    int band = wl80211_channel_check(channel);
    if (band < 0) {
        return -1;
    }
    return _channel_to_freq(band, channel);
}

static inline int wl80211_freq_to_channel(int freq)
{
    int band = wl80211_freq_check(freq);
    if (band < 0) {
        return -1;
    }
    return _freq_to_channel(band, freq);
}

static inline int wl80211_sta_is_connected(void)
{
    return wl80211_glb.link_up;
}

/**
 * Set STA power save mode
 *
 * Return value: 0 on success, negative value on error
 */
static inline int wl80211_sta_set_ps(int mode)
{
    return wl80211_cntrl(WL80211_CTRL_STA_SET_PS_MODE, mode);
}

/**
 * Get STA connection BSSID
 *
 * Return value:
 *   0  : Success
 *   -1 : STA not connected
 */
static inline int wl80211_sta_get_bssid(uint8_t bssid[6])
{
    if (!wl80211_sta_is_connected()) {
        return -1;
    }
    assert(wl80211_glb.last_connect_params != NULL);
    memcpy(bssid, wl80211_glb.last_connect_params->bssid, 6);

    return 0;
}

/**
 * Get STA connection channel
 *
 * Return value:
 *   >=0 : Channel number
 *   -1  : STA not connected
 */
static inline int wl80211_sta_get_channel(void)
{
    if (!wl80211_sta_is_connected()) {
        return -1;
    }
    assert(wl80211_glb.last_connect_params != NULL);

    return wl80211_glb.last_connect_params->channel;
}

/**
 * Get STA association ID
 *
 * Return value:
 *   >=0 : AID
 *   -1  : STA not connected
 */
static inline int wl80211_sta_get_aid(void)
{
    if (!wl80211_sta_is_connected()) {
        return -1;
    }

    return wl80211_glb.aid;
}

/**
 * Get STA connection SSID
 *
 * Return value:
 *   0  : Success
 *   -1 : STA not connected
 */
static inline int wl80211_sta_get_ssid(char ssid[33])
{
    if (!wl80211_sta_is_connected()) {
        return -1;
    }
    assert(wl80211_glb.last_connect_params != NULL);

    memcpy(ssid, wl80211_glb.last_connect_params->ssid, 33);
    return 0;
}

/**
 * Get STA connection password
 *
 * Return value:
 *   0  : Success
 *   -1 : STA not connected
 */
static inline int wl80211_sta_get_password(char password[65])
{
    if (!wl80211_sta_is_connected()) {
        return -1;
    }
    assert(wl80211_glb.last_connect_params != NULL);

    memcpy(password, wl80211_glb.last_connect_params->password, 65);
    return 0;
}

/**
 * Get STA PMK
 *
 * Return value: 0 on success, -1 on failure (not implemented)
 */
static inline int wl80211_sta_get_pmk(void)
{
    // TODO
    return -1;
}

static inline const struct ieee80211_dot_d *wl80211_get_country(void)
{
    return wl80211_glb.country;
}

/**
 * Set WiFi country code
 *
 * Return value: 0 on success, negative value on error
 */
int wl80211_set_country_code(const char *country_code);

/**
 * Get WiFi country code
 *
 * Return value: country code string, or NULL if not set
 */
static inline const char *wl80211_get_country_code(void)
{
    if (!wl80211_glb.country) {
        return NULL;
    }

    return wl80211_glb.country->code;
}

/**
 * Start AP mode
 *
 * Return value:
 *   0  : Success
 *   -1 : AP mode already enabled
 *   -2 : AP not allowed in monitor mode
 *   -3 : Failed to start AP (VIF creation failed)
 */
static inline int wl80211_ap_start(struct wl80211_ap_settings *ap_settings)
{
    return wl80211_cntrl(WL80211_CTRL_AP_START, ap_settings);
}

/**
 * Stop AP mode
 *
 * Return value: 0 on success
 */
static inline int wl80211_ap_stop(void)
{
    return wl80211_cntrl(WL80211_CTRL_AP_STOP);
}

/**
 * Get AP mode status
 *
 * Return value: 1 if AP enabled, 0 otherwise
 */
static inline int wl80211_ap_status(void)
{
    return wl80211_glb.ap_en;
}

/**
 * Start monitor mode
 *
 * Return value:
 *   0  : Success
 *   -1 : Monitor mode already enabled
 *   -2 : Monitor not allowed while AP enabled
 *   -3 : Failed to create monitor VIF
 */
static inline int wl80211_monitor_start(struct wl80211_monitor_settings *mon_settings)
{
    return wl80211_cntrl(WL80211_CTRL_MONITOR_START, mon_settings);
}

/**
 * Stop monitor mode
 *
 * Return value:
 *   0  : Success
 *   -1 : Failed to restore STA VIF after monitor stop
 */
static inline int wl80211_monitor_stop(void)
{
    return wl80211_cntrl(WL80211_CTRL_MONITOR_STOP);
}

/**
 * Get monitor mode status
 *
 * Return value: 1 if monitor enabled, 0 otherwise
 */
static inline int wl80211_monitor_status(void)
{
    return wl80211_glb.monitor_en;
}

/**
 * Frame injection parameters
 */
struct wl80211_inject_frame_params {
    void *frame;        /**< Complete 802.11 frame buffer */
    uint16_t len;       /**< Frame length in bytes */
    uint16_t freq;      /**< Channel frequency in MHz */
    void (*cb)(void *); /**< Optional completion callback */
    void *opaque;       /**< User data for callback */
};

/**
 * Inject a raw 802.11 frame on a specified channel (bypasses protocol stack)
 *
 * This function bypasses the WiFi protocol stack and directly transmits a user-constructed
 * 802.11 frame on the specified channel. Unlike wl80211_output_raw() which processes frames
 * through the protocol stack, this function requires the caller to provide a complete 802.11
 * frame with all fields properly set.
 *
 * Key differences from wl80211_output_raw():
 * - Caller must construct complete 802.11 MAC header (including RA, TA, sequence number)
 * - Caller specifies target channel via frequency parameter
 * - No automatic rate/power selection (uses MACSW defaults)
 * - Direct hardware-level frame injection
 * - Suitable for custom management frames, testing, and protocol development
 *
 * Prerequisites:
 * - WL80211_VIF_STA must exist (created automatically if needed)
 * - Frequency must be valid (2.4GHz: 2412-2484 MHz, 5GHz: 5005-5885 MHz)
 * - Frame buffer is copied to WRAM internally for hardware compatibility
 *
 * @param params Frame injection parameters
 *
 * @return 0 on success, negative value on error:
 *         -1 : Invalid parameters or VIF creation failed
 *         -2 : Monitor mode active (frame injection not allowed)
 *         -4 : Invalid frequency
 *         -7 : Memory allocation failed
 *         -10: MACSW rejected the request
 */
static inline int wl80211_inject_frame(struct wl80211_inject_frame_params *params)
{
    return wl80211_cntrl(WL80211_CTRL_INJECT_FRAME, params);
}
#endif
