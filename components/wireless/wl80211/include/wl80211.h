#ifndef __WL80211_H__
#define __WL80211_H__

#include <stdint.h>
#include <stdbool.h>
#include <sys/queue.h>
#include <sys/tree.h>

#include "macsw.h"

#define INVARIANTS

#ifndef container_of
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

/* async event type */
#define EV_WL80211 ((uintptr_t)wl80211_post_event)

#define ALIGN4_HI(val) (((val) + 3) & ~3)

#define IOVEC_DEFINED
struct iovec {
    void *iov_base;
    size_t iov_len;
};

struct wl80211_scan_params {
    uint8_t ssid_length;
    uint8_t ssid[32];
    uint8_t bssid[6];
    int channels_cnt;
    uint8_t channels[42];
    uint32_t probe_cnt;
    uint32_t duration;
    bool passive;
};

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
    int is_connecting_scan;
};

struct aid_list_entry {
    uint8_t mac[6];
    void *wpa_sm;
    uint8_t used;
    uint8_t sta_idx;
};

/* wl80211 global state */
struct wl80211_global_state {
    char country_code[3];

    uint16_t status_code;
    uint16_t reason_code;
    unsigned int scanning       : 1;
    unsigned int associated     : 1;
    unsigned int authenticating : 1;
    unsigned int link_up        : 1;

    uint8_t ssid[33];
    uint8_t password[65];
    uint8_t bssid[6];
    uint16_t freq;
    uint8_t chan_band;
    uint8_t pmf_cfg;
    uint8_t security;
    uint16_t aid;
    STAILQ_HEAD(, wl80211_scan_ops) scan_ops;
    struct wl80211_scan_params scan_params;

    // AP info
    uint8_t ap_ssid[33];
    uint8_t ap_password[65];
    struct mac_rateset ap_rateset;
    uint16_t max_listen_interval;
    uint8_t assoc_sta_count;
    uint8_t max_sta_supported;
    struct aid_list_entry aid_list[MACSW_REMOTE_STA_MAX];
    uint8_t ap_setting_ctrl_port;
    void *hostapd;
};
extern struct wl80211_global_state wl80211_glb;

typedef enum {
    WL80211_VIF_STA,
    WL80211_VIF_AP,
    WL80211_VIF_MAX,
} wl80211_vif_type;

/* Input callback function type */
typedef int (*wl80211_input_cb_t)(void *prv, wl80211_vif_type vif, void *rxhdr, void *buf, uint32_t frm_len, uint32_t status);

void wl80211_init(void);
void wl80211_tcpip_input(wl80211_vif_type vif, void *rxhdr, void *buf, uint32_t frm_len, uint32_t status);
int wl80211_output_raw(wl80211_vif_type vif, void *buffer, uint16_t len, unsigned int flags, void (*cb)(void *), void *opaque);

/* Input callback registration interface - allows external modules to register receive callback */
int wl80211_register_input_cb(wl80211_input_cb_t cb, void *prv);
int wl80211_unregister_input_cb(void);

enum {
    WL80211_EVT_SCAN_DONE,
    WL80211_EVT_CONNECTED,
    WL80211_EVT_DISCONNECTED,
};

// post wifi event in async
void wl80211_post_event(int code1, int code2);

// wl80211 control command
enum {
    WL80211_CTRL_SCAN,
    WL80211_CTRL_SCAN_PARAMS,
    WL80211_CTRL_STA_SET_BSSID,
    WL80211_CTRL_STA_GET_BSSID,

    WL80211_CTRL_STA_SET_SSID,
    WL80211_CTRL_STA_GET_SSID,

    WL80211_CTRL_STA_SET_PASSWORD, // param akm, passwd, generate pmk
    WL80211_CTRL_STA_GET_PASSWORD,

    WL80211_CTRL_STA_SET_PSK_PMK,  // PMK Caching
    WL80211_CTRL_STA_GET_PSK_PMK,

    WL80211_CTRL_STA_GET_LAST_STATUS,

    WL80211_CTRL_STA_SET_FREQ,
    WL80211_CTRL_STA_SET_PMF,
    WL80211_CTRL_STA_CONNECT,     // param freq flags
    WL80211_CTRL_STA_SET_PS_MODE, // set wifi ps mode
    WL80211_CTRL_STA_DISCONNECT,
    WL80211_CTRL_STA_GET_MAC,
    WL80211_CTRL_STA_GET_RSSI,

    WL80211_CTRL_AP_SET_SSID,
    WL80211_CTRL_AP_SET_PASSWORD,
    WL80211_CTRL_AP_START,
    WL80211_CTRL_AP_STOP,
};

// wl80211 control api
// return 0 : ok, negative : fail
int wl80211_cntrl(int cmd, ...);

// wl80211 get channel nums api
// return 0 : ok, negative : fail
int wl80211_get_channel_nums(const char *country_code, uint8_t *c24G_cnt, uint8_t *c5G_cnt);

// wl80211 get channel list api
// return 0 : ok, negative : fail
int wl80211_get_channel_list(const char *country_code, uint8_t **c24G_list, uint8_t **c5G_list);

// wl80211 set country code api
// return 0 : ok, negative : fail
int wl80211_set_country_code(char *country_code);

// wl80211 get country code api
// return 0 : ok, negative : fail
int wl80211_get_country_code(char *country_code);

// wl80211 check channel valid api
// return 0 : ok, negative : fail
int wl80211_channel_valid_check(uint16_t channel);

// wl80211 check frequency valid api
// return 0 : ok, negative : fail
int wl80211_freq_valid_check(uint16_t freq);
#endif
