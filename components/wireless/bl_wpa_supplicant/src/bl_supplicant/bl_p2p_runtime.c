#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "async_event.h"
#include "bl_supplicant/bl_p2p.h"
#include "bl_wps.h"
#include "common/ieee802_11_defs_p2p.h"
#include "p2p/p2p.h"
#include "supplicant.h"
#include "utils/common.h"
#include "utils/wpa_debug.h"
#include "utils/wpabuf.h"
#include "wl80211.h"
#include "wl80211_platform.h"
#include "wifi_mgmr.h"
#include "wps/wps.h"
#include "wps/wps_defs.h"

#define BL_P2P_LISTEN_CHANNEL 6
#define BL_P2P_LISTEN_REG_CLASS 81
#define BL_P2P_MAX_2G_CHANNEL 13
#define BL_P2P_SOCIAL_CHANNELS 3
#define BL_P2P_HDR_LEN 24
#define BL_P2P_ADDR1_OFT 4
#define BL_P2P_ADDR2_OFT 10
#define BL_P2P_ADDR3_OFT 16
#define BL_P2P_PROBE_RESP_IE_OFT (BL_P2P_HDR_LEN + 12)
#define BL_P2P_DEFAULT_DEVICE_NAME "BL616 P2P"
#define BL_P2P_DEFAULT_PIN "12345670"
#define BL_P2P_DEFAULT_GO_INTENT 7
#define BL_P2P_LISTEN_WPS_GO_INTENT 14
#define BL_P2P_MAX_GO_INTENT 15
#define BL_P2P_CONFIG_METHODS_DEFAULT \
    (WPS_CONFIG_DISPLAY | WPS_CONFIG_KEYPAD | WPS_CONFIG_PUSHBUTTON)
#define BL_P2P_GO_DHCP_START 2
#define BL_P2P_GO_DHCP_LIMIT 253
#define BL_P2P_JOIN_SCAN_WAIT_MS 3000
#define BL_P2P_JOIN_SCAN_POLL_MS 100
#define BL_P2P_DBG_NAME_LEN 65
#define BL_P2P_RAW_RATE_1MBPS 0
#define BL_P2P_RAW_RATE_6MBPS 4
#define BL_P2P_RAW_RETRY_LIMIT 7
#define BL_P2P_ACTION_RAW_RETRY_LIMIT 15
#define BL_P2P_FRAME_SUCCESSFUL_TX_BIT (1U << 23)
#define BL_P2P_VERBOSE_ACTION 0
#define BL_P2P_VERBOSE_LISTEN 0
#define BL_P2P_VERBOSE_PROBE 0
#define BL_P2P_VERBOSE_PR_TX 0
#define BL_P2P_PROBE_RESP_1M_COPY 0
#define BL_P2P_LISTEN_WPS_LISTEN_MS 5000

extern void _wifi_mgmr_ap_stop_dhcpd(void);
void rtos_start_evt_task(void (*handler)(void));

int bl_p2p_glue_register_mgmt_rx(wl80211_mgmt_rx_cb_t cb, void *prv);
int bl_p2p_glue_unregister_mgmt_rx(void);

struct bl_p2p_tx_status {
    struct bl_p2p_runtime *runtime;
    unsigned int freq;
    uint16_t frame_len;
    uint8_t dst[ETH_ALEN];
    uint8_t src[ETH_ALEN];
    uint8_t bssid[ETH_ALEN];
    uint8_t frame[];
};

struct bl_p2p_action_rx_pending {
    struct bl_p2p_runtime *runtime;
    unsigned int freq;
    uint16_t body_len;
    uint8_t da[ETH_ALEN];
    uint8_t sa[ETH_ALEN];
    uint8_t bssid[ETH_ALEN];
    uint8_t body[];
};

struct bl_p2p_runtime {
    struct p2p_config config;
    struct p2p_data *p2p;
    struct p2p_group *group;
    struct timeout_s listen_timeout;
    unsigned int ready         : 1;
    unsigned int scan_pending  : 1;
    unsigned int listen_active : 1;
    unsigned int stop_notified : 1;
    unsigned int scan_started  : 1;
    unsigned int group_started : 1;
    unsigned int pending_auth  : 1;
    unsigned int scan_work_queued : 1;
    unsigned int listen_stop_queued : 1;
    unsigned int listen_stop_notify : 1;
    unsigned int listen_wps_armed : 1;
    unsigned int listen_wps_channel_locked : 1;
    unsigned int listen_wps_peer_authorized : 1;
    unsigned int listen_wps_pin_user_supplied : 1;
    unsigned int listen_wps_pin_announced : 1;
    unsigned int group_start_queued : 1;
    uint16_t listen_freq;
    uint32_t listen_duration_ms;
    uint8_t listen_wps_locked_channel;
    unsigned int listen_wps_probe_count;
    unsigned int listen_wps_pd_count;
    unsigned int listen_wps_listen_count;
    unsigned int listen_wps_remaining_ms;
    int pending_go_intent;
    unsigned int pending_freq;
    enum bl_p2p_wps_mode listen_wps_mode;
    enum p2p_wps_method pending_wps_method;
    struct p2p_go_neg_results pending_go_neg;
    char device_name[WPS_MAX_DEVICE_NAME_LEN + 1];
    char target_ssid[SSID_MAX_LEN + 1];
    uint8_t pending_peer_addr[ETH_ALEN];
    uint8_t listen_wps_authorized_peer[ETH_ALEN];
    char pending_pin[9];
    bl_p2p_listen_pin_cb_t listen_pin_cb;
    void *listen_pin_cb_arg;
};

static struct bl_p2p_runtime g_bl_p2p_runtime;

/* Runtime-configurable GO intents (see p2p_go_intent shell command). Kept
 * outside of the runtime struct so they survive bl_p2p_init()/deinit(). */
static int g_bl_p2p_go_intent = BL_P2P_DEFAULT_GO_INTENT;
static int g_bl_p2p_listen_wps_go_intent = BL_P2P_LISTEN_WPS_GO_INTENT;

static void bl_p2p_listen_timeout(struct timeout_s *timeout);
static void bl_p2p_stop_listen_internal(struct bl_p2p_runtime *runtime, bool notify_end);
static void bl_p2p_group_remove(struct bl_p2p_runtime *runtime, bool stop_ap);
static void bl_p2p_scan_done_tsk(void);
static void bl_p2p_listen_stop_tsk(void);
static void bl_p2p_action_rx_tsk(void);
static void bl_p2p_probe_resp_tsk(void);
static void bl_p2p_group_start_tsk(void);
static const char *bl_p2p_raw_rate_name(uint8_t rate);
static void bl_p2p_schedule_scan_done(struct bl_p2p_runtime *runtime);
static void bl_p2p_schedule_stop_listen(struct bl_p2p_runtime *runtime, bool notify_end);
static int bl_p2p_start_go(struct bl_p2p_runtime *runtime,
                           const struct p2p_go_neg_results *res,
                           bool autonomous);
static int bl_p2p_start_client(struct bl_p2p_runtime *runtime,
                               const struct p2p_go_neg_results *res);
static const struct wl80211_scan_result_item *
bl_p2p_find_peer_scan_result(const uint8_t peer_addr[ETH_ALEN]);
static const struct wl80211_scan_result_item *
bl_p2p_wait_peer_scan_result(const uint8_t peer_addr[ETH_ALEN]);
static int bl_p2p_prepare_join_results(
    struct bl_p2p_runtime *runtime, const uint8_t peer_addr[ETH_ALEN],
    enum p2p_wps_method method, const char *pin,
    struct p2p_go_neg_results *res);

static uint32_t bl_p2p_ipv4_addr(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    return ((uint32_t) d << 24) | ((uint32_t) c << 16) |
           ((uint32_t) b << 8) | (uint32_t) a;
}

static const char *bl_p2p_ensure_pin(struct bl_p2p_runtime *runtime)
{
    unsigned int pin;

    if (runtime == NULL) {
        return NULL;
    }

    if (runtime->pending_pin[0] == '\0') {
        pin = wps_generate_pin();
        if (pin == (unsigned int)-1) {
            os_strlcpy(runtime->pending_pin, BL_P2P_DEFAULT_PIN,
                       sizeof(runtime->pending_pin));
        } else {
            snprintf(runtime->pending_pin, sizeof(runtime->pending_pin),
                     "%08u", pin);
        }
    }

    return runtime->pending_pin;
}

static bool bl_p2p_listen_wps_is_pbc(enum bl_p2p_wps_mode mode)
{
    return mode == BL_P2P_WPS_PBC;
}

static uint16_t bl_p2p_listen_wps_config_methods(enum bl_p2p_wps_mode mode)
{
    /* Advertise local capabilities in Probe Response. For local-display PIN
     * onboarding, peers should ask this device to display a PIN and then let
     * their user enter it, so the local device advertises KEYPAD. The later
     * Provision Discovery Request from the peer should carry DISPLAY. */
    return bl_p2p_listen_wps_is_pbc(mode) ? WPS_CONFIG_PUSHBUTTON :
                                            WPS_CONFIG_KEYPAD;
}

static uint16_t bl_p2p_listen_wps_pd_req_methods(enum bl_p2p_wps_mode mode)
{
    return bl_p2p_listen_wps_is_pbc(mode) ? WPS_CONFIG_PUSHBUTTON :
                                            WPS_CONFIG_DISPLAY;
}

static uint16_t bl_p2p_listen_wps_dev_pw_id(enum bl_p2p_wps_mode mode)
{
    return bl_p2p_listen_wps_is_pbc(mode) ? DEV_PW_PUSHBUTTON :
                                            DEV_PW_USER_SPECIFIED;
}

static const char *bl_p2p_listen_wps_mode_name(enum bl_p2p_wps_mode mode)
{
    return bl_p2p_listen_wps_is_pbc(mode) ? "pbc" : "pin";
}

static const char *bl_p2p_listen_wps_prepare_pin(struct bl_p2p_runtime *runtime,
                                                 const uint8_t *peer)
{
    const char *pin;

    if (runtime == NULL || peer == NULL) {
        return NULL;
    }

    pin = bl_p2p_ensure_pin(runtime);
    if (pin == NULL) {
        return NULL;
    }

    if (!runtime->listen_wps_pin_announced) {
        printf("P2P-PROV-DISC-SHOW-PIN " MACSTR " %s\r\n",
               MAC2STR(peer), pin);
        if (runtime->listen_pin_cb != NULL) {
            runtime->listen_pin_cb(peer, pin, runtime->listen_pin_cb_arg);
        }
        runtime->listen_wps_pin_announced = 1U;
    }

    return pin;
}

static int bl_p2p_listen_wps_authorize(struct bl_p2p_runtime *runtime,
                                      const uint8_t *peer,
                                      const char *reason,
                                      bool force)
{
    const char *pin = NULL;
    bool same_peer;
    int ret;

    if (runtime == NULL || runtime->p2p == NULL || peer == NULL ||
        !runtime->listen_wps_armed) {
        return -1;
    }

    same_peer = runtime->listen_wps_peer_authorized &&
                os_memcmp(runtime->listen_wps_authorized_peer, peer,
                          ETH_ALEN) == 0;
    if (same_peer && !force) {
        return 0;
    }

    if (!bl_p2p_listen_wps_is_pbc(runtime->listen_wps_mode)) {
        pin = bl_p2p_listen_wps_prepare_pin(runtime, peer);
    }

    printf("[P2P] listen_wps auth peer=" MACSTR " mode=%s reason=%s\r\n",
           MAC2STR(peer), bl_p2p_listen_wps_mode_name(runtime->listen_wps_mode),
           reason != NULL ? reason : "unknown");

    ret = bl_p2p_connect(peer, runtime->listen_wps_mode, pin,
                         g_bl_p2p_listen_wps_go_intent, true);
    if (ret < 0) {
        printf("[P2P] listen_wps auth FAIL peer=" MACSTR " ret=%d\r\n",
               MAC2STR(peer), ret);
        runtime->listen_wps_peer_authorized = 0U;
        return ret;
    }

    runtime->listen_wps_peer_authorized = 1U;
    os_memcpy(runtime->listen_wps_authorized_peer, peer, ETH_ALEN);
    return 0;
}

static enum p2p_wps_method bl_p2p_mode_to_method(enum bl_p2p_wps_mode mode)
{
    switch (mode) {
        case BL_P2P_WPS_PIN_DISPLAY:
            return WPS_PIN_DISPLAY;
        case BL_P2P_WPS_PIN_KEYPAD:
            return WPS_PIN_KEYPAD;
        case BL_P2P_WPS_PBC:
        default:
            return WPS_PBC;
    }
}

static wps_type_t bl_p2p_method_to_wps_type(enum p2p_wps_method method)
{
    return method == WPS_PBC ? WPS_TYPE_PBC : WPS_TYPE_PIN;
}

static const char *bl_p2p_method_name(enum p2p_wps_method method)
{
    switch (method) {
        case WPS_PIN_DISPLAY:
            return "display";
        case WPS_PIN_KEYPAD:
            return "keypad";
        case WPS_PBC:
            return "pbc";
        default:
            return "unknown";
    }
}

static int bl_p2p_action_subtype(const uint8_t *body, size_t body_len)
{
    if (body == NULL || body_len < 8 ||
        body[0] != WLAN_ACTION_PUBLIC ||
        body[1] != WLAN_PA_VENDOR_SPECIFIC ||
        WPA_GET_BE32(body + 2) != P2P_IE_VENDOR_TYPE) {
        return -1;
    }

    return body[6];
}

static int bl_p2p_action_dialog_token(const uint8_t *body, size_t body_len)
{
    if (bl_p2p_action_subtype(body, body_len) < 0) {
        return -1;
    }

    return body[7];
}

static const char *bl_p2p_action_subtype_name(int subtype)
{
    switch (subtype) {
    case P2P_GO_NEG_REQ:
        return "GO_NEG_REQ";
    case P2P_GO_NEG_RESP:
        return "GO_NEG_RESP";
    case P2P_GO_NEG_CONF:
        return "GO_NEG_CONF";
    case P2P_INVITATION_REQ:
        return "INVITATION_REQ";
    case P2P_INVITATION_RESP:
        return "INVITATION_RESP";
    case P2P_PROV_DISC_REQ:
        return "PROV_DISC_REQ";
    case P2P_PROV_DISC_RESP:
        return "PROV_DISC_RESP";
    default:
        return "unknown";
    }
}

#if BL_P2P_VERBOSE_PROBE
static void bl_p2p_copy_printable(char *dst, size_t dst_len,
                                  const uint8_t *src, size_t src_len)
{
    size_t i;
    size_t copy;

    if (dst == NULL || dst_len == 0) {
        return;
    }

    dst[0] = '\0';
    if (src == NULL || src_len == 0) {
        return;
    }

    copy = src_len;
    if (copy > dst_len - 1) {
        copy = dst_len - 1;
    }

    for (i = 0; i < copy; i++) {
        uint8_t c = src[i];
        dst[i] = (c >= 0x20 && c <= 0x7e) ? (char)c : '.';
    }
    dst[copy] = '\0';
}

static void bl_p2p_parse_wps_summary(const uint8_t *ie, size_t ie_len,
                                     char *name, size_t name_len,
                                     uint16_t *config_methods,
                                     int *dev_password_id)
{
    const uint8_t *pos = ie;
    const uint8_t *end = ie + ie_len;

    while (end - pos >= 4) {
        uint16_t attr = WPA_GET_BE16(pos);
        uint16_t len = WPA_GET_BE16(pos + 2);

        pos += 4;
        if ((size_t)(end - pos) < len) {
            break;
        }

        if (attr == ATTR_DEV_NAME && name != NULL && name[0] == '\0') {
            bl_p2p_copy_printable(name, name_len, pos, len);
        } else if (attr == ATTR_CONFIG_METHODS && len >= 2 &&
                   config_methods != NULL) {
            *config_methods = WPA_GET_BE16(pos);
        } else if (attr == ATTR_DEV_PASSWORD_ID && len >= 2 &&
                   dev_password_id != NULL) {
            *dev_password_id = (int)WPA_GET_BE16(pos);
        }

        pos += len;
    }
}

static void bl_p2p_parse_device_info_summary(const uint8_t *data, size_t len,
                                             char *name, size_t name_len,
                                             uint16_t *config_methods)
{
    const uint8_t *pos = data;
    const uint8_t *end = data + len;
    uint8_t num_sec_dev_types;
    uint16_t nlen;

    if (len < ETH_ALEN + 2 + 8 + 1) {
        return;
    }

    pos += ETH_ALEN;
    if (config_methods != NULL) {
        *config_methods = WPA_GET_BE16(pos);
    }
    pos += 2;
    pos += 8;
    num_sec_dev_types = *pos++;
    if ((size_t)(end - pos) < (size_t)num_sec_dev_types * 8U) {
        return;
    }
    pos += (size_t)num_sec_dev_types * 8U;

    if (end - pos < 4 || WPA_GET_BE16(pos) != ATTR_DEV_NAME) {
        return;
    }
    pos += 2;
    nlen = WPA_GET_BE16(pos);
    pos += 2;
    if ((size_t)(end - pos) < nlen) {
        return;
    }

    bl_p2p_copy_printable(name, name_len, pos, nlen);
}

static void bl_p2p_parse_p2p_summary(const uint8_t *ie, size_t ie_len,
                                     char *name, size_t name_len,
                                     uint16_t *config_methods)
{
    const uint8_t *pos = ie;
    const uint8_t *end = ie + ie_len;

    while (end - pos >= 3) {
        uint8_t attr = pos[0];
        uint16_t len = WPA_GET_LE16(pos + 1);

        pos += 3;
        if ((size_t)(end - pos) < len) {
            break;
        }

        if (attr == P2P_ATTR_DEVICE_INFO && name != NULL &&
            name[0] == '\0') {
            bl_p2p_parse_device_info_summary(pos, len, name, name_len,
                                             config_methods);
        }

        pos += len;
    }
}

static void bl_p2p_log_ie_summary(const char *tag, const uint8_t *ies,
                                  size_t ies_len)
{
    const uint8_t *pos = ies;
    const uint8_t *end = ies + ies_len;
    char ssid[SSID_MAX_LEN + 1];
    char wps_name[BL_P2P_DBG_NAME_LEN];
    char p2p_name[BL_P2P_DBG_NAME_LEN];
    uint16_t wps_methods = 0;
    uint16_t p2p_methods = 0;
    int dev_password_id = -1;
    int has_ssid = 0;
    int has_wps = 0;
    int has_p2p = 0;
    int malformed = 0;

    ssid[0] = '\0';
    wps_name[0] = '\0';
    p2p_name[0] = '\0';

    while (end - pos >= 2) {
        uint8_t id = pos[0];
        uint8_t len = pos[1];
        const uint8_t *data;

        pos += 2;
        if ((size_t)(end - pos) < len) {
            malformed = 1;
            break;
        }

        data = pos;
        if (id == WLAN_EID_SSID) {
            has_ssid = 1;
            if (len == 0) {
                os_strlcpy(ssid, "<empty>", sizeof(ssid));
            } else {
                bl_p2p_copy_printable(ssid, sizeof(ssid), data, len);
            }
        } else if (id == WLAN_EID_VENDOR_SPECIFIC && len >= 4) {
            uint32_t vendor_type = WPA_GET_BE32(data);

            if (vendor_type == WPS_IE_VENDOR_TYPE) {
                has_wps = 1;
                bl_p2p_parse_wps_summary(data + 4, len - 4, wps_name,
                                         sizeof(wps_name), &wps_methods,
                                         &dev_password_id);
            } else if (vendor_type == P2P_IE_VENDOR_TYPE) {
                has_p2p = 1;
                bl_p2p_parse_p2p_summary(data + 4, len - 4, p2p_name,
                                         sizeof(p2p_name), &p2p_methods);
            }
        }

        pos += len;
    }

    printf("[P2P-DBG] %s ssid='%s' wps_name='%s' p2p_name='%s' "
           "wps_methods=0x%04x p2p_methods=0x%04x dev_pw_id=%d "
           "has_wps=%d has_p2p=%d malformed=%d\r\n",
           tag, has_ssid ? ssid : "<none>", wps_name, p2p_name,
           wps_methods, p2p_methods, dev_password_id, has_wps, has_p2p,
           malformed);
}

static void bl_p2p_log_probe_req_ies(const uint8_t *ies, size_t ies_len)
{
    bl_p2p_log_ie_summary("probe_req_ie", ies, ies_len);
}

static void bl_p2p_log_probe_resp_frame(const uint8_t *frame, size_t frame_len)
{
    if (frame == NULL || frame_len <= BL_P2P_PROBE_RESP_IE_OFT) {
        return;
    }

    printf("[P2P-DBG] TX probe_resp frame da=" MACSTR " sa=" MACSTR
           " bssid=" MACSTR "\r\n",
           MAC2STR(frame + BL_P2P_ADDR1_OFT),
           MAC2STR(frame + BL_P2P_ADDR2_OFT),
           MAC2STR(frame + BL_P2P_ADDR3_OFT));
    bl_p2p_log_ie_summary("probe_resp_ie",
                          frame + BL_P2P_PROBE_RESP_IE_OFT,
                          frame_len - BL_P2P_PROBE_RESP_IE_OFT);
}
#endif

static void bl_p2p_apply_default_methods(struct bl_p2p_runtime *runtime)
{
    if (runtime == NULL || runtime->p2p == NULL) {
        return;
    }

    p2p_set_config_methods(runtime->p2p, BL_P2P_CONFIG_METHODS_DEFAULT);
    p2p_set_go_intent(runtime->p2p, (u8) g_bl_p2p_go_intent);
}

static void bl_p2p_store_group_ie(wifi_appie_ram_t type, struct wpabuf *ie)
{
    if (ie != NULL && wpabuf_len(ie) != 0) {
        bl_wifi_set_appie_ram_internal(0xff, type, wpabuf_head_u8(ie),
                                       (uint16_t) wpabuf_len(ie), false);
    } else {
        bl_wifi_unset_appie_ram_internal(0xff, type, false);
    }
}

static void bl_p2p_group_ie_update(void *ctx, struct wpabuf *beacon_ies,
                                   struct wpabuf *probe_resp_ies)
{
    (void) ctx;

    bl_p2p_store_group_ie(WIFI_APPIE_P2P_BEACON_GO, beacon_ies);
    bl_p2p_store_group_ie(WIFI_APPIE_P2P_PROBE_RESP_GO, probe_resp_ies);
    wpabuf_free(beacon_ies);
    wpabuf_free(probe_resp_ies);
}

static void bl_p2p_group_idle_update(void *ctx, int idle)
{
    (void) ctx;
    wpa_printf(MSG_INFO, "P2P GO idle=%d", idle);
}

static void bl_p2p_publish_assoc_resp_ie(struct bl_p2p_runtime *runtime,
                                         uint8_t status)
{
    struct wpabuf *ie;

    if (runtime == NULL || runtime->group == NULL) {
        bl_wifi_unset_appie_ram_internal(0xff, WIFI_APPIE_P2P_ASSOC_RESP,
                                         false);
        return;
    }

    ie = p2p_group_assoc_resp_ie(runtime->group, status);
    bl_p2p_store_group_ie(WIFI_APPIE_P2P_ASSOC_RESP, ie);
    wpabuf_free(ie);
}

static void bl_p2p_wps_event_cb(bl_wps_event_t event, void *payload, void *cb_arg)
{
    struct bl_p2p_runtime *runtime = cb_arg;

    switch (event) {
        case BL_WPS_EVENT_COMPLETE:
            printf("[P2P] client WPS completed\r\n");
            break;
        case BL_WPS_EVENT_PIN: {
            bl_wps_pin_t *pin = payload;

            if (pin != NULL) {
                os_strlcpy(runtime->pending_pin, pin->pin,
                           sizeof(runtime->pending_pin));
                printf("[P2P] WPS PIN ready: %s\r\n", pin->pin);
            }
            break;
        }
        case BL_WPS_EVENT_REG_SUCCESS:
            printf("[P2P] GO WPS registrar completed\r\n");
            if (runtime != NULL && runtime->group != NULL) {
                p2p_group_notif_formation_done(runtime->group);
                bl_p2p_publish_assoc_resp_ie(runtime, P2P_SC_SUCCESS);
            }
            break;
        case BL_WPS_EVENT_FAILURE:
            printf("[P2P] WPS failed\r\n");
            break;
        case BL_WPS_EVENT_TIMEOUT:
            printf("[P2P] WPS timeout\r\n");
            break;
        case BL_WPS_EVENT_SESSION_OVERLAP:
            printf("[P2P] WPS session overlap\r\n");
            break;
        case BL_WPS_EVENT_SCAN_ERROR:
            printf("[P2P] WPS scan error\r\n");
            break;
        default:
            break;
    }

    if (payload != NULL) {
        vPortFree(payload);
    }
}

static void bl_p2p_debug_print(void *ctx, int level, const char *msg)
{
    (void) ctx;
    wpa_printf(level, "P2P: %s", msg);
}

static void bl_p2p_event_dev_found(void *ctx, const uint8_t *addr,
                                   const struct p2p_peer_info *info,
                                   int new_device)
{
    (void) ctx;

    if (info == NULL) {
        return;
    }

    wpa_printf(MSG_INFO,
               "P2P peer %s " MACSTR " name='%s' listen_config=0x%04x dev_capab=0x%02x group_capab=0x%02x",
               new_device ? "found" : "updated",
               MAC2STR(addr),
               info->device_name,
               info->config_methods,
               info->dev_capab,
               info->group_capab);
}

static void bl_p2p_event_dev_lost(void *ctx, const uint8_t *dev_addr)
{
    (void) ctx;
    wpa_printf(MSG_INFO, "P2P peer lost " MACSTR, MAC2STR(dev_addr));
}

static void bl_p2p_event_find_stopped(void *ctx)
{
    (void) ctx;
    wl80211_p2p_set_discovery(false);
    wpa_printf(MSG_INFO, "P2P device discovery stopped");
}

static void bl_p2p_event_go_neg_req_rx(void *ctx, const uint8_t *src,
                                       uint16_t dev_passwd_id,
                                       uint8_t go_intent)
{
    struct bl_p2p_runtime *runtime = ctx;

#if BL_P2P_VERBOSE_ACTION
    printf("[P2P] go_neg_req peer=" MACSTR " dev_pw_id=%u go_intent=%u\r\n",
           MAC2STR(src), dev_passwd_id, go_intent);
#endif

    if (runtime != NULL && runtime->listen_wps_armed &&
        dev_passwd_id ==
            bl_p2p_listen_wps_dev_pw_id(runtime->listen_wps_mode)) {
        bl_p2p_listen_wps_authorize(runtime, src, "late_go_neg", true);
    }
}

static void bl_p2p_event_go_neg_completed(void *ctx,
                                          struct p2p_go_neg_results *res)
{
    struct bl_p2p_runtime *runtime = ctx;

    if (res == NULL) {
        return;
    }

    printf("[P2P] GO negotiation completed status=%d role=%s freq=%d"
           " method=%s ssid_len=%u\r\n",
           res->status, res->role_go ? "GO" : "client", res->freq,
           bl_p2p_method_name(res->wps_method),
           (unsigned int) res->ssid_len);

    if (runtime == NULL) {
        wl80211_p2p_set_group_forming(false);
        return;
    }

    if (res->status != P2P_SC_SUCCESS) {
        if (runtime->listen_wps_armed &&
            !is_zero_ether_addr(res->peer_device_addr)) {
            runtime->listen_wps_peer_authorized = 0U;
            bl_p2p_listen_wps_authorize(runtime, res->peer_device_addr,
                                       "go_neg_retry", true);
        }
        wl80211_p2p_set_group_forming(false);
        return;
    }

    runtime->listen_wps_armed = 0U;
    runtime->listen_wps_peer_authorized = 0U;
    os_memcpy(&runtime->pending_go_neg, res, sizeof(runtime->pending_go_neg));

    if (runtime->group_start_queued) {
#if BL_P2P_VERBOSE_ACTION
        printf("[P2P] group start already queued\r\n");
#endif
        return;
    }

    runtime->group_start_queued = 1U;
    rtos_start_evt_task(bl_p2p_group_start_tsk);
}

static void bl_p2p_event_prov_disc_req(void *ctx, const uint8_t *peer,
                                       uint16_t config_methods,
                                       const uint8_t *dev_addr,
                                       const uint8_t *pri_dev_type,
                                       const char *dev_name,
                                       uint16_t supp_config_methods,
                                       uint8_t dev_capab,
                                       uint8_t group_capab,
                                       const uint8_t *group_id,
                                       size_t group_id_len)
{
    struct bl_p2p_runtime *runtime = ctx;
    (void) dev_addr;
    (void) pri_dev_type;
    (void) supp_config_methods;
    (void) dev_capab;
    (void) group_capab;
    (void) group_id;
    (void) group_id_len;

    /* Listen-WPS onboarding: the local device stays in P2P Device listen and
     * waits for the peer to start Provision Discovery. For PIN display mode,
     * generate/announce the PIN only after a concrete peer requests it. */
    if (runtime != NULL && runtime->listen_wps_armed) {
        bool same_peer = runtime->listen_wps_peer_authorized &&
                         os_memcmp(runtime->listen_wps_authorized_peer, peer,
                                   ETH_ALEN) == 0;
        bool log_pd;
        uint16_t expected_methods =
            bl_p2p_listen_wps_pd_req_methods(runtime->listen_wps_mode);

        if (!same_peer) {
            runtime->listen_wps_pd_count = 0U;
            runtime->listen_wps_pin_announced = 0U;
            if (!runtime->listen_wps_pin_user_supplied) {
                runtime->pending_pin[0] = '\0';
            }
        }
        runtime->listen_wps_pd_count++;
        log_pd = runtime->listen_wps_pd_count <= 3U ||
                 (runtime->listen_wps_pd_count % 16U) == 0U;

        if (log_pd) {
            printf("[P2P] prov_disc_req peer=" MACSTR
                   " methods=0x%04x name='%s' count=%u%s\r\n",
                   MAC2STR(peer), config_methods,
                   dev_name != NULL ? dev_name : "",
                   runtime->listen_wps_pd_count,
                   same_peer ? " auth" : "");
        }

        if (!(config_methods & expected_methods)) {
            printf("[P2P] listen_wps prov_disc method mismatch mode=%s methods=0x%04x expected=0x%04x\r\n",
                   bl_p2p_listen_wps_mode_name(runtime->listen_wps_mode),
                   config_methods, expected_methods);
            return;
        }

        if (!same_peer) {
            if (bl_p2p_listen_wps_is_pbc(runtime->listen_wps_mode)) {
                printf("P2P-PROV-DISC-PBC-REQ " MACSTR "\r\n",
                       MAC2STR(peer));
            } else if (bl_p2p_listen_wps_prepare_pin(runtime, peer) == NULL) {
                printf("[P2P] listen_wps PIN generate FAIL peer=" MACSTR "\r\n",
                       MAC2STR(peer));
                return;
            }
            bl_p2p_listen_wps_authorize(runtime, peer, "prov_disc", false);
        }

        return;
    }

    printf("[P2P] prov_disc_req peer=" MACSTR " methods=0x%04x name='%s'\r\n",
           MAC2STR(peer), config_methods, dev_name != NULL ? dev_name : "");
}

static void bl_p2p_event_prov_disc_resp(void *ctx, const uint8_t *peer,
                                        uint16_t config_methods)
{
    (void) ctx;
    wpa_printf(MSG_INFO,
               "P2P provision discovery response from " MACSTR
               " methods=0x%04x",
               MAC2STR(peer), config_methods);
}

static void bl_p2p_event_prov_disc_fail(void *ctx, const uint8_t *peer,
                                        enum p2p_prov_disc_status status,
                                        uint32_t adv_id,
                                        const uint8_t *adv_mac,
                                        const char *deferred_session_resp)
{
    (void) ctx;
    (void) adv_id;
    (void) adv_mac;
    (void) deferred_session_resp;
    wpa_printf(MSG_INFO,
               "P2P provision discovery failed for " MACSTR " status=%d",
               MAC2STR(peer), status);
}

static void bl_p2p_send_action_done(void *ctx)
{
    (void) ctx;
}

static int bl_p2p_go_connected(void *ctx, const uint8_t *dev_addr)
{
    (void) ctx;

    if (wl80211_glb.p2p.role != WL80211_P2P_ROLE_CLIENT) {
        return 0;
    }

    return !wl80211_p2p_addr_is_zero(wl80211_glb.p2p.group.go_dev_addr) &&
           os_memcmp(wl80211_glb.p2p.group.go_dev_addr, dev_addr, ETH_ALEN) == 0;
}

static int bl_p2p_is_concurrent_session_active(void *ctx)
{
    (void) ctx;
    return wl80211_sta_is_connected() || wl80211_ap_status();
}

static int bl_p2p_is_p2p_in_progress(void *ctx)
{
    (void) ctx;
    return wl80211_glb.p2p.group_forming;
}

static unsigned int bl_p2p_current_freq(const struct bl_p2p_runtime *runtime)
{
    if (runtime != NULL && runtime->listen_active && runtime->listen_freq != 0) {
        return runtime->listen_freq;
    }

    if (wl80211_glb.ap_chan_freq != 0) {
        return wl80211_glb.ap_chan_freq;
    }

    if (wl80211_sta_is_connected()) {
        int channel = wl80211_sta_get_channel();

        if (channel > 0) {
            return (unsigned int) wl80211_channel_to_freq(channel);
        }
    }

    return wl80211_channel_to_freq(BL_P2P_LISTEN_CHANNEL);
}

static void bl_p2p_set_runtime_channel(struct bl_p2p_runtime *runtime,
                                       uint8_t channel)
{
    if (runtime == NULL || channel == 0) {
        return;
    }

    runtime->config.reg_class = BL_P2P_LISTEN_REG_CLASS;
    runtime->config.channel = channel;
    runtime->config.op_reg_class = BL_P2P_LISTEN_REG_CLASS;
    runtime->config.op_channel = channel;

    if (runtime->p2p != NULL) {
        p2p_set_listen_channel(runtime->p2p, BL_P2P_LISTEN_REG_CLASS,
                               channel, 1);
        p2p_set_oper_channel(runtime->p2p, BL_P2P_LISTEN_REG_CLASS,
                             channel, 1);
    }
}

static uint8_t bl_p2p_listen_wps_listen_channel(struct bl_p2p_runtime *runtime)
{
    uint8_t channel;

    if (runtime == NULL) {
        return BL_P2P_LISTEN_CHANNEL;
    }

    if (runtime->listen_wps_channel_locked &&
        runtime->listen_wps_locked_channel != 0) {
        return runtime->listen_wps_locked_channel;
    }

    channel = runtime->config.channel;
    if (channel == 0 || channel > BL_P2P_MAX_2G_CHANNEL) {
        channel = BL_P2P_LISTEN_CHANNEL;
    }

    return channel;
}

static int bl_p2p_listen_wps_listen(struct bl_p2p_runtime *runtime,
                                   unsigned int duration_ms)
{
    uint8_t channel;
    int freq;
    int ret;

    if (runtime == NULL || runtime->p2p == NULL ||
        runtime->listen_wps_remaining_ms == 0) {
        return -1;
    }

    if (duration_ms == 0 || duration_ms > BL_P2P_LISTEN_WPS_LISTEN_MS) {
        duration_ms = BL_P2P_LISTEN_WPS_LISTEN_MS;
    }
    if (duration_ms > runtime->listen_wps_remaining_ms) {
        duration_ms = runtime->listen_wps_remaining_ms;
    }

    channel = bl_p2p_listen_wps_listen_channel(runtime);
    bl_p2p_set_runtime_channel(runtime, channel);
    freq = wl80211_channel_to_freq(channel);
    if (freq <= 0) {
        return -1;
    }

    runtime->listen_wps_listen_count++;
    if (runtime->listen_wps_listen_count <= 3U ||
        (runtime->listen_wps_listen_count % 12U) == 0U) {
        printf("[P2P] listen_wps listen ch=%u freq=%d dur=%u%s\r\n",
               channel, freq, duration_ms,
               runtime->listen_wps_channel_locked ? " locked" : "");
    }

    ret = p2p_listen(runtime->p2p, duration_ms);
    if (ret == 0) {
        runtime->listen_wps_remaining_ms -= duration_ms;
    }

    return ret;
}

static void bl_p2p_listen_wps_note_probe(struct bl_p2p_runtime *runtime,
                                        const uint8_t *peer,
                                        unsigned int freq,
                                        enum p2p_probe_req_status status)
{
    int channel;
    bool log_probe;

    if (runtime == NULL || !runtime->listen_wps_armed ||
        status != P2P_PREQ_PROCESSED) {
        return;
    }

    channel = wl80211_freq_to_channel((int) freq);
    runtime->listen_wps_probe_count++;

    if (channel > 0 &&
        (!runtime->listen_wps_channel_locked ||
         runtime->listen_wps_locked_channel != (uint8_t) channel)) {
        runtime->listen_wps_channel_locked = 1U;
        runtime->listen_wps_locked_channel = (uint8_t) channel;
        bl_p2p_set_runtime_channel(runtime, (uint8_t) channel);
        log_probe = true;
    } else {
        log_probe = runtime->listen_wps_probe_count <= 3U ||
                    (runtime->listen_wps_probe_count % 16U) == 0U;
    }

    if (log_probe) {
        printf("[P2P] listen_wps probe peer=" MACSTR " ch=%d freq=%u count=%u%s\r\n",
               MAC2STR(peer), channel, freq, runtime->listen_wps_probe_count,
               runtime->listen_wps_channel_locked ? " locked" : "");
    }
}

static void bl_p2p_fill_channels(struct p2p_channels *channels)
{
    const struct ieee80211_dot_d *country = wl80211_get_country();
    size_t i;

    os_memset(channels, 0, sizeof(*channels));

    if (country == NULL || country->channel24G_num == 0) {
        channels->reg_class[0].reg_class = BL_P2P_LISTEN_REG_CLASS;
        channels->reg_class[0].channel[0] = 1;
        channels->reg_class[0].channel[1] = 6;
        channels->reg_class[0].channel[2] = 11;
        channels->reg_class[0].channels = BL_P2P_SOCIAL_CHANNELS;
        channels->reg_classes = 1;
        return;
    }

    channels->reg_class[0].reg_class = BL_P2P_LISTEN_REG_CLASS;
    for (i = 0; i < country->channel24G_num &&
                i < P2P_MAX_REG_CLASS_CHANNELS; i++) {
        channels->reg_class[0].channel[i] = country->channel24G_chan[i];
    }
    channels->reg_class[0].channels = i;
    channels->reg_classes = 1;
}

static int bl_p2p_store_scan_ie(struct bl_p2p_runtime *runtime,
                                const uint8_t *dev_id)
{
    struct wpabuf *ie;
    size_t ie_len;
    int ret;

    ie_len = p2p_scan_ie_buf_len(runtime->p2p);
    ie = wpabuf_alloc(ie_len);
    if (ie == NULL) {
        return -1;
    }

    p2p_scan_ie(runtime->p2p, ie, dev_id, 0);
    ret = bl_wifi_set_appie_ram_internal(0xff, WIFI_APPIE_P2P_PROBE_REQ,
                                         wpabuf_head_u8(ie),
                                         (uint16_t) wpabuf_len(ie), true);
    wpabuf_free(ie);
    return ret;
}

static int bl_p2p_build_action_frame(struct bl_p2p_tx_status *tx, const uint8_t *dst,
                                     const uint8_t *src, const uint8_t *bssid,
                                     const uint8_t *body, size_t body_len)
{
    uint16_t fc;

    if (tx == NULL || dst == NULL || src == NULL || bssid == NULL ||
        body == NULL || body_len == 0) {
        return -1;
    }

    fc = IEEE80211_FC(WLAN_FC_TYPE_MGMT, WLAN_FC_STYPE_ACTION);
    os_memcpy(tx->dst, dst, ETH_ALEN);
    os_memcpy(tx->src, src, ETH_ALEN);
    os_memcpy(tx->bssid, bssid, ETH_ALEN);

    WPA_PUT_LE16(tx->frame, fc);
    WPA_PUT_LE16(tx->frame + 2, 0);
    os_memcpy(tx->frame + 4, dst, ETH_ALEN);
    os_memcpy(tx->frame + 10, src, ETH_ALEN);
    os_memcpy(tx->frame + 16, bssid, ETH_ALEN);
    WPA_PUT_LE16(tx->frame + 22, 0);
    os_memcpy(tx->frame + BL_P2P_HDR_LEN, body, body_len);

    return 0;
}

static void bl_p2p_action_tx_complete(struct bl_p2p_tx_status *tx,
                                      enum p2p_send_action_result result)
{
    if (tx == NULL || tx->runtime == NULL || tx->runtime->p2p == NULL) {
        free(tx);
        return;
    }

    p2p_send_action_cb(tx->runtime->p2p, tx->freq, tx->dst, tx->src, tx->bssid,
                       result);
    free(tx);
}

static void bl_p2p_action_tx_status(void *opaque, int status)
{
    struct bl_p2p_tx_status *tx = opaque;
    enum p2p_send_action_result result;
    int subtype;

    if (tx == NULL) {
        return;
    }

    subtype = bl_p2p_action_subtype(tx->frame + BL_P2P_HDR_LEN,
                                    tx->frame_len - BL_P2P_HDR_LEN);
    result = (status == 0 || (status & BL_P2P_FRAME_SUCCESSFUL_TX_BIT)) ?
             P2P_SEND_ACTION_SUCCESS : P2P_SEND_ACTION_NO_ACK;
    if (result != P2P_SEND_ACTION_SUCCESS) {
        printf("[P2P] TX action no_ack peer=" MACSTR " subtype=%s(%d)"
               " status=0x%08x\r\n",
               MAC2STR(tx->dst), bl_p2p_action_subtype_name(subtype),
               subtype, (unsigned int) status);
    }
#if BL_P2P_VERBOSE_ACTION
    else {
        printf("[P2P] TX action cfm peer=" MACSTR " subtype=%s(%d)"
               " status=0x%08x\r\n",
               MAC2STR(tx->dst), bl_p2p_action_subtype_name(subtype),
               subtype, (unsigned int) status);
    }
#endif
    bl_p2p_action_tx_complete(tx, result);
}

/* Keep P2P action parsing/GO negotiation out of the wifi fw RX task. */
#define BL_P2P_ACT_RX_QUEUE_LEN 8
struct bl_p2p_act_rx_entry {
    struct bl_p2p_action_rx_pending *rx;
};
static struct bl_p2p_act_rx_entry g_bl_p2p_act_rx_q[BL_P2P_ACT_RX_QUEUE_LEN];
static volatile uint8_t g_bl_p2p_act_rx_head;
static volatile uint8_t g_bl_p2p_act_rx_tail;
static volatile uint8_t g_bl_p2p_act_rx_busy;
static unsigned int g_bl_p2p_act_rx_drop_count;

static void bl_p2p_action_rx_tsk(void)
{
    for (;;) {
        struct bl_p2p_act_rx_entry *e;
        struct bl_p2p_action_rx_pending *rx;

        if (g_bl_p2p_act_rx_head == g_bl_p2p_act_rx_tail) {
            g_bl_p2p_act_rx_busy = 0U;
            if (g_bl_p2p_act_rx_head != g_bl_p2p_act_rx_tail) {
                g_bl_p2p_act_rx_busy = 1U;
                continue;
            }
            return;
        }

        e = &g_bl_p2p_act_rx_q[g_bl_p2p_act_rx_head];
        rx = e->rx;
        e->rx = NULL;
        g_bl_p2p_act_rx_head = (uint8_t)((g_bl_p2p_act_rx_head + 1U) %
                                         BL_P2P_ACT_RX_QUEUE_LEN);

        if (rx != NULL && rx->runtime != NULL && rx->runtime->p2p != NULL &&
            rx->body_len > 1) {
            p2p_rx_action(rx->runtime->p2p, rx->da, rx->sa, rx->bssid,
                          rx->body[0], rx->body + 1, rx->body_len - 1,
                          rx->freq);
        }
        free(rx);
    }
}

static int bl_p2p_enqueue_action_rx(struct bl_p2p_runtime *runtime,
                                    const uint8_t *da,
                                    const uint8_t *sa,
                                    const uint8_t *bssid,
                                    const uint8_t *body,
                                    size_t body_len,
                                    unsigned int freq)
{
    struct bl_p2p_action_rx_pending *rx;
    uint8_t next;

    if (runtime == NULL || runtime->p2p == NULL || da == NULL || sa == NULL ||
        bssid == NULL || body == NULL || body_len <= 1 ||
        body_len > UINT16_MAX) {
        return -1;
    }

    next = (uint8_t)((g_bl_p2p_act_rx_tail + 1U) %
                     BL_P2P_ACT_RX_QUEUE_LEN);
    if (next == g_bl_p2p_act_rx_head) {
        struct bl_p2p_action_rx_pending *old =
            g_bl_p2p_act_rx_q[g_bl_p2p_act_rx_head].rx;

        g_bl_p2p_act_rx_q[g_bl_p2p_act_rx_head].rx = NULL;
        g_bl_p2p_act_rx_head = (uint8_t)((g_bl_p2p_act_rx_head + 1U) %
                                         BL_P2P_ACT_RX_QUEUE_LEN);
        free(old);
        g_bl_p2p_act_rx_drop_count++;
        if (g_bl_p2p_act_rx_drop_count <= 3U ||
            (g_bl_p2p_act_rx_drop_count % 32U) == 0U) {
            printf("[P2P] action_rx queue overflow drop_oldest count=%u\r\n",
                   g_bl_p2p_act_rx_drop_count);
        }
    }

    rx = calloc(1, sizeof(*rx) + body_len);
    if (rx == NULL) {
        printf("[P2P] action_rx malloc FAIL len=%u\r\n",
               (unsigned)body_len);
        return -1;
    }

    rx->runtime = runtime;
    rx->freq = freq;
    rx->body_len = (uint16_t)body_len;
    os_memcpy(rx->da, da, ETH_ALEN);
    os_memcpy(rx->sa, sa, ETH_ALEN);
    os_memcpy(rx->bssid, bssid, ETH_ALEN);
    os_memcpy(rx->body, body, body_len);

    g_bl_p2p_act_rx_q[g_bl_p2p_act_rx_tail].rx = rx;
    g_bl_p2p_act_rx_tail = next;

    if (!g_bl_p2p_act_rx_busy) {
        g_bl_p2p_act_rx_busy = 1U;
        rtos_start_evt_task(bl_p2p_action_rx_tsk);
    }

    return 0;
}

/* Deferred action-frame queue: send_action() is invoked from TASK_MM via
 * mgmt RX path (e.g. PD/GO-Neg request handling). wl80211_inject_frame()
 * synchronously calls into TASK_MM, so dispatch it from a worker task. */
#define BL_P2P_ACT_QUEUE_LEN 4
struct bl_p2p_act_pending {
    struct bl_p2p_tx_status *tx;
    uint16_t frame_len;
    unsigned int wait_time;
};
static struct bl_p2p_act_pending g_bl_p2p_act_q[BL_P2P_ACT_QUEUE_LEN];
static volatile uint8_t g_bl_p2p_act_head;
static volatile uint8_t g_bl_p2p_act_tail;
static volatile uint8_t g_bl_p2p_act_busy;

static void bl_p2p_action_tx_tsk(void)
{
    for (;;) {
        struct bl_p2p_act_pending *e;
        struct bl_p2p_tx_status *tx;
        struct wl80211_inject_frame_params params;
        int ret;
        const uint8_t *body;
        uint8_t dst[ETH_ALEN];
        unsigned int tx_freq;
        uint16_t tx_len;
        int subtype;

        if (g_bl_p2p_act_head == g_bl_p2p_act_tail) {
            g_bl_p2p_act_busy = 0U;
            if (g_bl_p2p_act_head != g_bl_p2p_act_tail) {
                g_bl_p2p_act_busy = 1U;
                continue;
            }
            return;
        }

        e = &g_bl_p2p_act_q[g_bl_p2p_act_head];
        tx = e->tx;
        body = tx->frame + BL_P2P_HDR_LEN;
        subtype = bl_p2p_action_subtype(body, e->frame_len - BL_P2P_HDR_LEN);
        os_memcpy(dst, tx->dst, ETH_ALEN);
        tx_freq = tx->freq;
        tx_len = e->frame_len;

        os_memset(&params, 0, sizeof(params));
        params.duration_ms = e->wait_time ? e->wait_time : 20U;
        params.frame = tx->frame;
        params.len = e->frame_len;
        params.freq = (uint16_t)tx->freq;
        params.wait_rx = false;
        params.fixed_rate = true;
        params.rate = BL_P2P_RAW_RATE_1MBPS;
        params.retry_limit = BL_P2P_ACTION_RAW_RETRY_LIMIT;
        params.status_cb = bl_p2p_action_tx_status;
        params.opaque = tx;

        ret = wl80211_inject_frame(&params);
        if (ret < 0) {
            printf("[P2P] TX action start FAIL ret=%d peer=" MACSTR
                   " subtype=%s(%d) freq=%u len=%u\r\n",
                   ret, MAC2STR(dst), bl_p2p_action_subtype_name(subtype),
                   subtype, tx_freq, tx_len);
            bl_p2p_action_tx_complete(tx, P2P_SEND_ACTION_FAILED);
        }
#if BL_P2P_VERBOSE_ACTION
        else {
            printf("[P2P] TX action start OK peer=" MACSTR
                   " subtype=%s(%d) freq=%u len=%u rate=%s\r\n",
                   MAC2STR(dst), bl_p2p_action_subtype_name(subtype),
                   subtype, tx_freq, tx_len,
                   bl_p2p_raw_rate_name(params.rate));
        }
#endif
        e->tx = NULL;
        g_bl_p2p_act_head = (uint8_t)((g_bl_p2p_act_head + 1U) % BL_P2P_ACT_QUEUE_LEN);
    }
}

static int bl_p2p_send_action(void *ctx, unsigned int freq, const uint8_t *dst,
                              const uint8_t *src, const uint8_t *bssid,
                              const uint8_t *buf, size_t len,
                              unsigned int wait_time, int *scheduled)
{
    struct bl_p2p_runtime *runtime = ctx;
    struct bl_p2p_tx_status *tx;
    size_t frame_len;
    uint8_t next;

    if (runtime == NULL || runtime->p2p == NULL || dst == NULL ||
        src == NULL || bssid == NULL || buf == NULL || len == 0) {
        return -1;
    }

    if (scheduled != NULL) {
        *scheduled = 1;
    }

    frame_len = BL_P2P_HDR_LEN + len;
    tx = calloc(1, sizeof(*tx) + frame_len);
    if (tx == NULL) {
        return -1;
    }

    tx->runtime = runtime;
    tx->freq = freq;
    tx->frame_len = (uint16_t)frame_len;
    if (bl_p2p_build_action_frame(tx, dst, src, bssid, buf, len) < 0) {
        free(tx);
        return -1;
    }

    {
        int subtype = bl_p2p_action_subtype(buf, len);
        int dialog = bl_p2p_action_dialog_token(buf, len);

#if BL_P2P_VERBOSE_ACTION
        printf("[P2P] TX action peer=" MACSTR " subtype=%s(%d)"
               " dialog=%d freq=%u len=%u wait=%u\r\n",
               MAC2STR(dst), bl_p2p_action_subtype_name(subtype), subtype,
               dialog, freq, (unsigned)frame_len, wait_time);
#else
        (void) subtype;
        (void) dialog;
#endif
    }

    next = (uint8_t)((g_bl_p2p_act_tail + 1U) % BL_P2P_ACT_QUEUE_LEN);
    if (next == g_bl_p2p_act_head) {
        printf("[P2P] TX action queue full\r\n");
        free(tx);
        return -1;
    }

    g_bl_p2p_act_q[g_bl_p2p_act_tail].tx = tx;
    g_bl_p2p_act_q[g_bl_p2p_act_tail].frame_len = (uint16_t) frame_len;
    g_bl_p2p_act_q[g_bl_p2p_act_tail].wait_time = wait_time;
    g_bl_p2p_act_tail = next;

    if (!g_bl_p2p_act_busy) {
        g_bl_p2p_act_busy = 1U;
        rtos_start_evt_task(bl_p2p_action_tx_tsk);
    }

    return 0;
}

/* Deferred probe-resp queue: send_probe_resp() is invoked from TASK_MM
 * context (mgmt RX path); calling wl80211_inject_frame() inline performs a
 * synchronous macif_kmsg_call() back to TASK_MM, which deadlocks. Copy the
 * frame and dispatch the actual inject from a worker task. */
#define BL_P2P_PR_QUEUE_LEN 12
struct bl_p2p_pr_pending {
    uint8_t *frame;
    uint16_t len;
    uint16_t freq;
    uint8_t rate;
    uint8_t copy_idx;
};
static struct bl_p2p_pr_pending g_bl_p2p_pr_q[BL_P2P_PR_QUEUE_LEN];
static volatile uint8_t g_bl_p2p_pr_head;
static volatile uint8_t g_bl_p2p_pr_tail;
static volatile uint8_t g_bl_p2p_pr_busy;
static unsigned int g_bl_p2p_pr_drop_count;
#if BL_P2P_VERBOSE_PR_TX
static unsigned int g_bl_p2p_pr_coalesce_count;
#endif

static const char *bl_p2p_raw_rate_name(uint8_t rate)
{
    switch (rate) {
    case BL_P2P_RAW_RATE_1MBPS:
        return "1M";
    case BL_P2P_RAW_RATE_6MBPS:
        return "6M";
    default:
        return "?";
    }
}

static void bl_p2p_probe_resp_tsk(void)
{
    struct bl_p2p_pr_pending *e;
    struct wl80211_inject_frame_params params;
    uint8_t *frame;
    uint16_t len;
    uint16_t freq;
    uint8_t rate;
    uint8_t copy_idx;
    int ret;

    for (;;) {
        if (g_bl_p2p_pr_head == g_bl_p2p_pr_tail) {
            g_bl_p2p_pr_busy = 0U;
            if (g_bl_p2p_pr_head != g_bl_p2p_pr_tail) {
                g_bl_p2p_pr_busy = 1U;
                continue;
            }
            return;
        }

        e = &g_bl_p2p_pr_q[g_bl_p2p_pr_head];
        frame = e->frame;
        len = e->len;
        freq = e->freq;
        rate = e->rate;
        copy_idx = e->copy_idx;
        e->frame = NULL;
        g_bl_p2p_pr_head = (uint8_t)((g_bl_p2p_pr_head + 1U) %
                                     BL_P2P_PR_QUEUE_LEN);

        os_memset(&params, 0, sizeof(params));
        params.frame = frame;
        params.len = len;
        params.freq = freq;
        params.fixed_rate = true;
        params.rate = rate;
        params.retry_limit = BL_P2P_RAW_RETRY_LIMIT;

        ret = wl80211_inject_frame(&params);
        if (ret < 0) {
            printf("[P2P] probe_resp inject FAIL ret=%d freq=%u len=%u rate=%s copy=%u\r\n",
                   ret, freq, len, bl_p2p_raw_rate_name(rate), (unsigned)copy_idx);
        }
#if BL_P2P_VERBOSE_PR_TX
        else {
            printf("[P2P] probe_resp inject ret=%d freq=%u len=%u rate=%s copy=%u\r\n",
                   ret, freq, len, bl_p2p_raw_rate_name(rate), (unsigned)copy_idx);
        }
#endif

        wl80211_platform_free_wram_nolimit(frame);
    }
}

static void bl_p2p_probe_resp_drop_pending(void)
{
    unsigned int dropped = 0;

    while (g_bl_p2p_pr_head != g_bl_p2p_pr_tail) {
        struct bl_p2p_pr_pending *old = &g_bl_p2p_pr_q[g_bl_p2p_pr_head];

        if (old->frame != NULL) {
            wl80211_platform_free_wram_nolimit(old->frame);
            old->frame = NULL;
        }
        g_bl_p2p_pr_head = (uint8_t)((g_bl_p2p_pr_head + 1U) %
                                     BL_P2P_PR_QUEUE_LEN);
        dropped++;
    }

    if (dropped == 0) {
        return;
    }

#if BL_P2P_VERBOSE_PR_TX
    g_bl_p2p_pr_coalesce_count += dropped;
    if (g_bl_p2p_pr_coalesce_count <= 3U ||
        (g_bl_p2p_pr_coalesce_count % 32U) == 0U) {
        printf("[P2P] probe_resp coalesce drop_pending total=%u\r\n",
               g_bl_p2p_pr_coalesce_count);
    }
#else
    (void) dropped;
#endif
}

static int bl_p2p_enqueue_probe_resp(const struct wpabuf *buf,
                                     unsigned int freq,
                                     uint8_t rate,
                                     uint8_t copy_idx)
{
    uint8_t next;
    uint8_t *copy;

    if (copy_idx == 0) {
        bl_p2p_probe_resp_drop_pending();
    }

    next = (uint8_t)((g_bl_p2p_pr_tail + 1U) % BL_P2P_PR_QUEUE_LEN);
    if (next == g_bl_p2p_pr_head) {
        struct bl_p2p_pr_pending *old = &g_bl_p2p_pr_q[g_bl_p2p_pr_head];

        if (old->frame != NULL) {
            wl80211_platform_free_wram_nolimit(old->frame);
            old->frame = NULL;
        }
        g_bl_p2p_pr_head = (uint8_t)((g_bl_p2p_pr_head + 1U) %
                                     BL_P2P_PR_QUEUE_LEN);
        g_bl_p2p_pr_drop_count++;
        if (g_bl_p2p_pr_drop_count <= 3U ||
            (g_bl_p2p_pr_drop_count % 32U) == 0U) {
            printf("[P2P] probe_resp queue overflow drop_oldest count=%u\r\n",
                   g_bl_p2p_pr_drop_count);
        }
    }

    copy = wl80211_platform_malloc_wram_nolimit(wpabuf_len(buf));
    if (copy == NULL) {
        printf("[P2P] probe_resp malloc FAIL len=%u\r\n",
               (unsigned)wpabuf_len(buf));
        return -1;
    }
    memcpy(copy, wpabuf_head(buf), wpabuf_len(buf));

    g_bl_p2p_pr_q[g_bl_p2p_pr_tail].frame = copy;
    g_bl_p2p_pr_q[g_bl_p2p_pr_tail].len = (uint16_t)wpabuf_len(buf);
    g_bl_p2p_pr_q[g_bl_p2p_pr_tail].freq = (uint16_t)freq;
    g_bl_p2p_pr_q[g_bl_p2p_pr_tail].rate = rate;
    g_bl_p2p_pr_q[g_bl_p2p_pr_tail].copy_idx = copy_idx;
    g_bl_p2p_pr_tail = next;

    if (!g_bl_p2p_pr_busy) {
        g_bl_p2p_pr_busy = 1U;
        rtos_start_evt_task(bl_p2p_probe_resp_tsk);
    }

    return 0;
}

static int bl_p2p_send_probe_resp(void *ctx, const struct wpabuf *buf,
                                  unsigned int freq)
{
    struct bl_p2p_runtime *runtime = ctx;
    int ret;

    if (buf == NULL || wpabuf_len(buf) == 0 || freq == 0) {
        printf("[P2P] probe_resp DROP buf=%p len=%u freq=%u\r\n",
               buf, buf ? (unsigned)wpabuf_len(buf) : 0, freq);
        return -1;
    }

#if BL_P2P_VERBOSE_PROBE
    bl_p2p_log_probe_resp_frame(wpabuf_head(buf), wpabuf_len(buf));
    {
        const uint8_t *da = (const uint8_t *)wpabuf_head(buf) + BL_P2P_ADDR1_OFT;
        printf("[P2P] TX probe_resp QUEUE to=" MACSTR
               " freq=%u len=%u rates=%s%s\r\n",
               MAC2STR(da), freq, (unsigned)wpabuf_len(buf), "6M",
               (runtime != NULL && runtime->listen_wps_armed &&
                BL_P2P_PROBE_RESP_1M_COPY) ? "+1M" : "");
    }
#endif

    ret = bl_p2p_enqueue_probe_resp(buf, freq, BL_P2P_RAW_RATE_6MBPS, 0);
    if (runtime != NULL && runtime->listen_wps_armed &&
        BL_P2P_PROBE_RESP_1M_COPY) {
        if (bl_p2p_enqueue_probe_resp(buf, freq, BL_P2P_RAW_RATE_1MBPS, 1) < 0 &&
            ret == 0) {
            ret = -1;
        }
    }

    return ret;
}

static int bl_p2p_start_listen(void *ctx, unsigned int freq,
                               unsigned int duration,
                               const struct wpabuf *probe_resp_ie)
{
    struct bl_p2p_runtime *runtime = ctx;

    if (runtime == NULL || runtime->p2p == NULL || freq == 0 || duration == 0) {
        return -1;
    }

    bl_p2p_stop_listen_internal(runtime, false);

    if (probe_resp_ie != NULL && wpabuf_len(probe_resp_ie) != 0) {
        bl_wifi_set_appie_ram_internal(0xff, WIFI_APPIE_P2P_PROBE_RESP,
                                       wpabuf_head_u8(probe_resp_ie),
                                       (uint16_t) wpabuf_len(probe_resp_ie),
                                       true);
    } else {
        bl_wifi_unset_appie_ram_internal(0xff, WIFI_APPIE_P2P_PROBE_RESP, true);
    }

    /* Cap each ROC at 30s; longer listen-WPS windows are re-armed below. */
    if (duration > 30000U) {
        duration = 30000U;
    }

#if BL_P2P_VERBOSE_LISTEN
    printf("[P2P] listen start freq=%u dur=%u ie_len=%u\r\n",
           freq, duration,
           probe_resp_ie ? (unsigned)wpabuf_len(probe_resp_ie) : 0);
#endif
    if (wl80211_remain_on_channel_start(WL80211_VIF_STA, (uint16_t) freq,
                                        duration) < 0) {
        wl80211_printf("[P2P] listen ROC start failed freq=%u dur=%u\r\n",
                       freq, duration);
        bl_wifi_unset_appie_ram_internal(0xff, WIFI_APPIE_P2P_PROBE_RESP, true);
        return -1;
    }

    runtime->listen_freq = (uint16_t) freq;
    runtime->listen_duration_ms = duration;
    runtime->listen_active = 1U;
    runtime->stop_notified = 0U;
    runtime->listen_timeout.callback = bl_p2p_listen_timeout;
    runtime->listen_timeout.opaque = runtime;
    timeout_start(&runtime->listen_timeout, duration);
    p2p_listen_cb(runtime->p2p, freq, duration);

    return 0;
}

static void bl_p2p_stop_listen_internal(struct bl_p2p_runtime *runtime,
                                        bool notify_end)
{
    if (runtime == NULL || !runtime->listen_active) {
        return;
    }

    timeout_stop(&runtime->listen_timeout);
    wl80211_cancel_remain_on_channel(WL80211_VIF_STA);
    bl_wifi_unset_appie_ram_internal(0xff, WIFI_APPIE_P2P_PROBE_RESP, true);

    if (notify_end && !runtime->stop_notified && runtime->p2p != NULL) {
        runtime->stop_notified = 1U;
        p2p_listen_end(runtime->p2p, runtime->listen_freq);
    }

    runtime->listen_active = 0U;
    runtime->listen_freq = 0;
    runtime->listen_duration_ms = 0;
}

static void bl_p2p_stop_listen(void *ctx)
{
    bl_p2p_schedule_stop_listen(ctx, true);
}

static void bl_p2p_listen_timeout(struct timeout_s *timeout)
{
    struct bl_p2p_runtime *runtime;

    if (timeout == NULL) {
        return;
    }

    runtime = timeout->opaque;
    if (runtime == NULL || runtime->p2p == NULL || !runtime->listen_active) {
        return;
    }

    bl_p2p_schedule_stop_listen(runtime, true);
}

static int bl_p2p_scan(void *ctx, enum p2p_scan_type type, int freq,
                       unsigned int num_req_dev_types,
                       const uint8_t *req_dev_types, const uint8_t *dev_id,
                       uint16_t pw_id, bool include_6ghz)
{
    struct bl_p2p_runtime *runtime = ctx;
    struct wl80211_scan_params params;
    uint8_t channels[BL_P2P_SOCIAL_CHANNELS + 1];
    int channels_cnt = 0;
    int ret;

    (void) num_req_dev_types;
    (void) req_dev_types;
    (void) pw_id;
    (void) include_6ghz;

    if (runtime == NULL || runtime->p2p == NULL) {
        return -1;
    }

    if (bl_p2p_store_scan_ie(runtime, dev_id) < 0) {
        return -1;
    }

    os_memset(&params, 0, sizeof(params));
    switch (type) {
        case P2P_SCAN_SOCIAL:
            channels[channels_cnt++] = 1;
            channels[channels_cnt++] = 6;
            channels[channels_cnt++] = 11;
            break;
        case P2P_SCAN_SPECIFIC:
            if (freq > 0) {
                int channel = wl80211_freq_to_channel(freq);

                if (channel > 0) {
                    channels[channels_cnt++] = (uint8_t) channel;
                }
            }
            break;
        case P2P_SCAN_SOCIAL_PLUS_ONE:
            channels[channels_cnt++] = 1;
            channels[channels_cnt++] = 6;
            channels[channels_cnt++] = 11;
            if (freq > 0) {
                int channel = wl80211_freq_to_channel(freq);

                if (channel > 0 && channel != 1 && channel != 6 && channel != 11) {
                    channels[channels_cnt++] = (uint8_t) channel;
                }
            }
            break;
        case P2P_SCAN_FULL:
        default:
            channels_cnt = 0;
            break;
    }

    if (channels_cnt != 0) {
        params.channels = channels;
        params.channels_cnt = channels_cnt;
    }

    runtime->scan_pending = 1U;
    runtime->scan_started = 1U;
    wl80211_p2p_set_discovery(true);
    ret = wl80211_scan(&params);
    p2p_notify_scan_trigger_status(runtime->p2p, ret == 0 ? 0 : -1);
    if (ret != 0) {
        runtime->scan_pending = 0U;
        wl80211_p2p_set_discovery(false);
        return -1;
    }

    return 0;
}

static void bl_p2p_handle_scan_done(struct bl_p2p_runtime *runtime)
{
    struct wl80211_scan_result_item *item;
    struct os_reltime now;
    if (runtime == NULL || runtime->p2p == NULL || !runtime->scan_pending) {
        return;
    }

    runtime->scan_pending = 0U;
    os_get_reltime(&now);

    RB_FOREACH(item, _scan_result_tree, &wl80211_scan_result)
    {
        if (!(item->flags & WL80211_SCAN_AP_RESULT_FLAGS_HAS_P2P) ||
            item->p2p_ie == NULL || item->p2p_ie_len == 0) {
            continue;
        }

        p2p_scan_res_handler(runtime->p2p, item->bssid,
                             wl80211_channel_to_freq(item->channel),
                             &now, item->rssi, item->p2p_ie, item->p2p_ie_len);
    }
    p2p_scan_res_handled(runtime->p2p, 0);
}

static void bl_p2p_scan_done_tsk(void)
{
    struct bl_p2p_runtime *runtime = &g_bl_p2p_runtime;

    runtime->scan_work_queued = 0U;
    bl_p2p_handle_scan_done(runtime);
}

static void bl_p2p_listen_stop_tsk(void)
{
    struct bl_p2p_runtime *runtime = &g_bl_p2p_runtime;
    bool notify_end = runtime->listen_stop_notify != 0U;

    runtime->listen_stop_queued = 0U;
    runtime->listen_stop_notify = 0U;
    bl_p2p_stop_listen_internal(runtime, notify_end);

    /* Listen-WPS onboarding stays on the configured listen channel. Re-arm
     * right after each ROC ends until the requested total duration expires. */
    if (runtime->listen_wps_armed && runtime->p2p != NULL &&
        !runtime->group_started && !runtime->pending_auth) {
        if (runtime->listen_wps_remaining_ms == 0) {
            printf("[P2P] listen_wps expired mode=%s\r\n",
                   bl_p2p_listen_wps_mode_name(runtime->listen_wps_mode));
            bl_p2p_listen_wps_disarm();
        } else if (bl_p2p_listen_wps_listen(runtime, 0) < 0) {
            printf("[P2P] listen_wps re-listen FAIL\r\n");
        }
    }
}

static void bl_p2p_group_start_tsk(void)
{
    struct bl_p2p_runtime *runtime = &g_bl_p2p_runtime;
    struct p2p_go_neg_results res;
    int ret;

    os_memcpy(&res, &runtime->pending_go_neg, sizeof(res));
    runtime->group_start_queued = 0U;
    runtime->listen_wps_armed = 0U;
    runtime->listen_wps_peer_authorized = 0U;

    bl_p2p_stop_listen_internal(runtime, false);

    printf("[P2P] group start task role=%s freq=%d ssid_len=%u method=%s\r\n",
           res.role_go ? "GO" : "client", res.freq,
           (unsigned int) res.ssid_len, bl_p2p_method_name(res.wps_method));

    if (res.role_go) {
        ret = bl_p2p_start_go(runtime, &res, false);
    } else {
        ret = bl_p2p_start_client(runtime, &res);
    }

    if (ret < 0) {
        printf("[P2P] group start FAIL role=%s ret=%d\r\n",
               res.role_go ? "GO" : "client", ret);
        wl80211_p2p_set_group_forming(false);
    }
}

static void bl_p2p_schedule_scan_done(struct bl_p2p_runtime *runtime)
{
    if (runtime == NULL || runtime->scan_work_queued) {
        return;
    }

    runtime->scan_work_queued = 1U;
    rtos_start_evt_task(bl_p2p_scan_done_tsk);
}

static void bl_p2p_schedule_stop_listen(struct bl_p2p_runtime *runtime, bool notify_end)
{
    if (runtime == NULL) {
        return;
    }

    if (notify_end) {
        runtime->listen_stop_notify = 1U;
    }

    if (runtime->listen_stop_queued) {
        return;
    }

    runtime->listen_stop_queued = 1U;
    rtos_start_evt_task(bl_p2p_listen_stop_tsk);
}

static void bl_p2p_event_handler(async_input_event_t event, void *private_data)
{
    struct bl_p2p_runtime *runtime = private_data;

    if (event == NULL || runtime == NULL || runtime->p2p == NULL) {
        return;
    }

    if (event->code == WL80211_EVT_SCAN_DONE) {
        bl_p2p_schedule_scan_done(runtime);
        return;
    }

    if (event->code == WL80211_EVT_AP_STARTED &&
        wl80211_glb.p2p.role == WL80211_P2P_ROLE_GO &&
        runtime->group != NULL && !runtime->group_started) {
        runtime->group_started = 1U;
        printf("[P2P] GO AP started freq=%u\r\n",
               wl80211_glb.p2p.group.operating_freq);
        return;
    }

    if (event->code == WL80211_EVT_AP_STOPPED &&
        wl80211_glb.p2p.role == WL80211_P2P_ROLE_GO) {
        printf("[P2P] GO AP stopped\r\n");
        runtime->group_started = 0U;
        bl_p2p_group_remove(runtime, false);
        return;
    }

    if (event->code == WL80211_EVT_STA_DISCONNECTED &&
        wl80211_glb.p2p.role == WL80211_P2P_ROLE_CLIENT) {
        wl80211_p2p_mark_group_removed();
        runtime->group_started = 0U;
        printf("[P2P] client disconnected\r\n");
    }
}

static int bl_p2p_mgmt_rx(void *prv, uint8_t vif_type, void *rxhdr,
                          const uint8_t *frame, uint32_t frame_len,
                          uint32_t status)
{
    struct bl_p2p_runtime *runtime = prv;
    uint16_t fc;
    uint16_t stype;
    unsigned int freq;
    const uint8_t *da;
    const uint8_t *sa;
    const uint8_t *bssid;

    (void) vif_type;
    (void) rxhdr;
    (void) status;

    if (runtime == NULL || runtime->p2p == NULL || frame == NULL ||
        frame_len < BL_P2P_HDR_LEN + 1) {
        return 0;
    }

    fc = WPA_GET_LE16(frame);
    stype = WLAN_FC_GET_STYPE(fc);
    freq = bl_p2p_current_freq(runtime);
    da = frame + BL_P2P_ADDR1_OFT;
    sa = frame + BL_P2P_ADDR2_OFT;
    bssid = frame + BL_P2P_ADDR3_OFT;

    if (stype == WLAN_FC_STYPE_PROBE_REQ) {
        enum p2p_probe_req_status pr_st;
#if BL_P2P_VERBOSE_PROBE
        printf("[P2P] RX probe_req from=" MACSTR " da=" MACSTR
               " freq=%u len=%u listen=%d\r\n",
               MAC2STR(sa), MAC2STR(da), freq, (unsigned)frame_len,
               runtime->listen_active);
        bl_p2p_log_probe_req_ies(frame + BL_P2P_HDR_LEN,
                                 frame_len - BL_P2P_HDR_LEN);
        pr_st = p2p_probe_req_rx(runtime->p2p, sa, da, bssid,
                         frame + BL_P2P_HDR_LEN,
                         frame_len - BL_P2P_HDR_LEN, freq, 0);
        printf("[P2P] probe_req_rx ret=%d\r\n", (int)pr_st);
#else
        pr_st = p2p_probe_req_rx(runtime->p2p, sa, da, bssid,
                                 frame + BL_P2P_HDR_LEN,
                                 frame_len - BL_P2P_HDR_LEN, freq, 0);
#endif
        bl_p2p_listen_wps_note_probe(runtime, sa, freq, pr_st);
        return 0;
    }

    if (stype == WLAN_FC_STYPE_ACTION && frame_len > BL_P2P_HDR_LEN + 1) {
        const uint8_t *body = frame + BL_P2P_HDR_LEN;
        int subtype = bl_p2p_action_subtype(body, frame_len - BL_P2P_HDR_LEN);
        int dialog = bl_p2p_action_dialog_token(body, frame_len - BL_P2P_HDR_LEN);

#if BL_P2P_VERBOSE_ACTION
        printf("[P2P] RX action peer=" MACSTR " subtype=%s(%d)"
               " dialog=%d freq=%u len=%u\r\n",
               MAC2STR(sa), bl_p2p_action_subtype_name(subtype), subtype,
               dialog, freq, (unsigned)frame_len);
#else
        (void) dialog;
#endif
        if (bl_p2p_enqueue_action_rx(runtime, da, sa, bssid, body,
                                     frame_len - BL_P2P_HDR_LEN, freq) < 0) {
            printf("[P2P] action_rx enqueue FAIL peer=" MACSTR
                   " subtype=%s(%d) len=%u\r\n",
                   MAC2STR(sa), bl_p2p_action_subtype_name(subtype), subtype,
                   (unsigned)frame_len);
        }
        return 0;
    }

    return 0;
}

static void bl_p2p_store_pending_pin(struct bl_p2p_runtime *runtime,
                                     const char *pin)
{
    char tmp[sizeof(runtime->pending_pin)];

    if (runtime == NULL) {
        return;
    }

    tmp[0] = '\0';
    if (pin != NULL && pin[0] != '\0') {
        os_strlcpy(tmp, pin, sizeof(tmp));
    }

    runtime->pending_pin[0] = '\0';
    if (tmp[0] != '\0') {
        os_strlcpy(runtime->pending_pin, tmp, sizeof(runtime->pending_pin));
    }
}

static void bl_p2p_prepare_pending(struct bl_p2p_runtime *runtime,
                                   const uint8_t *peer_addr,
                                   enum p2p_wps_method method,
                                   const char *pin, int go_intent)
{
    if (runtime == NULL) {
        return;
    }

    runtime->pending_wps_method = method;
    runtime->pending_go_intent = go_intent;
    bl_p2p_store_pending_pin(runtime, pin);
    if (peer_addr != NULL) {
        os_memcpy(runtime->pending_peer_addr, peer_addr, ETH_ALEN);
    } else {
        os_memset(runtime->pending_peer_addr, 0, ETH_ALEN);
    }
}

static int bl_p2p_init_group_ctx(struct bl_p2p_runtime *runtime,
                                 const struct p2p_go_neg_results *res)
{
    struct p2p_group_config *cfg;
    uint8_t ifaddr[ETH_ALEN];

    if (runtime == NULL || runtime->p2p == NULL || res == NULL) {
        return -1;
    }

    bl_p2p_group_remove(runtime, false);

    cfg = os_zalloc(sizeof(*cfg));
    if (cfg == NULL) {
        return -1;
    }

    if (wifi_mgmr_ap_mac_get(ifaddr) != 0 || is_zero_ether_addr(ifaddr)) {
        if (platform_get_mac(WL80211_VIF_STA, ifaddr) < 0) {
            os_free(cfg);
            return -1;
        }
    }

    cfg->persistent_group = res->persistent_group;
    cfg->max_clients = 4;
    cfg->ssid_len = res->ssid_len;
    cfg->freq = res->freq;
    cfg->cb_ctx = runtime;
    cfg->ie_update = bl_p2p_group_ie_update;
    cfg->idle_update = bl_p2p_group_idle_update;
    os_memcpy(cfg->interface_addr, ifaddr, ETH_ALEN);
    os_memcpy(cfg->ssid, res->ssid, res->ssid_len);

    runtime->group = p2p_group_init(runtime->p2p, cfg);
    if (runtime->group == NULL) {
        os_free(cfg);
        return -1;
    }

    bl_p2p_publish_assoc_resp_ie(runtime, P2P_SC_SUCCESS);
    return 0;
}

static int bl_p2p_configure_go_wps(struct bl_p2p_runtime *runtime,
                                   const struct p2p_go_neg_results *res)
{
    bl_wps_config_t config;
    bl_wps_err_t err;
    char ssid[SSID_MAX_LEN + 1];

    if (runtime == NULL || res == NULL) {
        return -1;
    }

    os_memset(&config, 0, sizeof(config));
    config.type = bl_p2p_method_to_wps_type(res->wps_method);
    config.pin = res->wps_method == WPS_PBC ? NULL : bl_p2p_ensure_pin(runtime);
    config.event_cb = bl_p2p_wps_event_cb;
    config.event_cb_arg = runtime;
    os_strlcpy(config.factory_info.device_name, "BL616 P2P GO",
               sizeof(config.factory_info.device_name));
    os_strlcpy(config.factory_info.model_name, "BL616",
               sizeof(config.factory_info.model_name));
    os_strlcpy(config.factory_info.model_number, "WL80211",
               sizeof(config.factory_info.model_number));
    os_strlcpy(config.factory_info.manufacturer, "Bouffalo Lab",
               sizeof(config.factory_info.manufacturer));

    os_memcpy(ssid, res->ssid, res->ssid_len);
    ssid[res->ssid_len] = '\0';
    err = bl_wifi_wps_ap_start(&config, ssid, res->passphrase);
    if (err != BL_WPS_ERR_OK) {
        printf("[P2P] GO WPS AP start FAIL err=%d\r\n", err);
        return -1;
    }

    printf("[P2P] GO WPS AP armed ssid=%s method=%s\r\n",
           ssid, bl_p2p_method_name(res->wps_method));
    return 0;
}

static int bl_p2p_start_go_ap(const struct p2p_go_neg_results *res)
{
    struct wl80211_ap_settings settings;
    const uint8_t *beacon_ie = NULL;
    const uint8_t *probe_resp_ie = NULL;
    uint16_t beacon_ie_len = 0;
    uint16_t probe_resp_ie_len = 0;
    int ret;

    if (res == NULL || res->ssid_len == 0 || res->freq == 0 ||
        res->passphrase[0] == '\0') {
        printf("[P2P] GO AP start rejected ssid_len=%u freq=%d pass=%d\r\n",
               res ? (unsigned int) res->ssid_len : 0U,
               res ? res->freq : 0,
               res != NULL && res->passphrase[0] != '\0');
        return -1;
    }

    if (bl_wifi_wps_ap_get_ies(&beacon_ie, &beacon_ie_len,
                               &probe_resp_ie, &probe_resp_ie_len) != 0) {
        printf("[P2P] GO AP start FAIL WPS IE fetch\r\n");
        return -1;
    }

    os_memset(&settings, 0, sizeof(settings));
    os_memcpy(settings.ssid, res->ssid, res->ssid_len);
    settings.ssid[res->ssid_len] = '\0';
    os_strlcpy((char *) settings.password, res->passphrase,
               sizeof(settings.password));
    settings.auth_type = WL80211_AUTHTYPE_OPEN_SYSTEM;
    settings.center_freq1 = res->freq;
    settings.channel_width = WL80211_CHAN_WIDTH_20;
    settings.beacon_interval = 100;
    settings.max_power = 0x14;
    settings.beacon_ie = beacon_ie;
    settings.beacon_ie_len = beacon_ie_len;
    settings.probe_resp_ie = probe_resp_ie;
    settings.probe_resp_ie_len = probe_resp_ie_len;

    ret = wl80211_ap_start(&settings);
    if (ret < 0) {
        printf("[P2P] GO AP start request failed ret=%d ssid=%s freq=%d\r\n",
               ret, (char *) settings.ssid, res->freq);
    }
    return ret;
}

static int bl_p2p_start_go(struct bl_p2p_runtime *runtime,
                           const struct p2p_go_neg_results *res,
                           bool autonomous)
{
#if BL_P2P_VERBOSE_LISTEN
    char ssid[SSID_MAX_LEN + 1];
#endif

    if (runtime == NULL || res == NULL) {
        return -1;
    }

    bl_wifi_wps_stop();
    if (wl80211_sta_is_connected()) {
        wifi_mgmr_sta_disconnect();
    }
    if (wl80211_ap_status()) {
        _wifi_mgmr_ap_stop_dhcpd();
        wifi_mgmr_ap_stop();
    }

    if (bl_p2p_init_group_ctx(runtime, res) < 0) {
        return -1;
    }

    if (bl_p2p_configure_go_wps(runtime, res) < 0) {
        bl_p2p_group_remove(runtime, false);
        return -1;
    }

    wl80211_p2p_set_role(WL80211_P2P_ROLE_GO);
    wl80211_p2p_set_group_forming(!autonomous);
    os_memcpy(wl80211_glb.p2p.group.peer_dev_addr, res->peer_device_addr,
              ETH_ALEN);
    wl80211_glb.p2p.group.persistent = res->persistent_group;

#if !defined(__NuttX__)
    wifi_mgmr_ap_netif_cfg_override(true, true, BL_P2P_GO_DHCP_START,
                                    BL_P2P_GO_DHCP_LIMIT,
                                    bl_p2p_ipv4_addr(192, 168, 169, 1),
                                    bl_p2p_ipv4_addr(255, 255, 255, 0));
#endif

    if (bl_p2p_start_go_ap(res) < 0) {
#if !defined(__NuttX__)
        wifi_mgmr_ap_netif_cfg_reset();
#endif
        bl_wifi_wps_stop();
        bl_p2p_group_remove(runtime, false);
        return -1;
    }

#if BL_P2P_VERBOSE_LISTEN
    os_memcpy(ssid, res->ssid, res->ssid_len);
    ssid[res->ssid_len] = '\0';
    printf("[P2P] GO start requested ssid=%s freq=%d method=%s%s\r\n",
           ssid, res->freq, bl_p2p_method_name(res->wps_method),
           runtime->pending_pin[0] ? " pin_ready" : "");
#endif
    return 0;
}

static int bl_p2p_start_client(struct bl_p2p_runtime *runtime,
                               const struct p2p_go_neg_results *res)
{
    bl_wps_config_t config;
    bl_wps_err_t err;

    if (runtime == NULL || res == NULL || res->ssid_len == 0) {
        printf("[P2P] start client rejected runtime=%p res=%p ssid_len=%u\r\n",
               runtime, res, res ? (unsigned int) res->ssid_len : 0U);
        return -1;
    }

    bl_wifi_wps_stop();
    if (wl80211_ap_status()) {
        _wifi_mgmr_ap_stop_dhcpd();
        wifi_mgmr_ap_stop();
    }

    os_memset(&config, 0, sizeof(config));
    config.type = bl_p2p_method_to_wps_type(res->wps_method);
    config.pin = res->wps_method == WPS_PBC ? NULL : bl_p2p_ensure_pin(runtime);
    config.event_cb = bl_p2p_wps_event_cb;
    config.event_cb_arg = runtime;
    os_strlcpy(config.factory_info.device_name, "BL616 P2P Client",
               sizeof(config.factory_info.device_name));
    os_strlcpy(config.factory_info.model_name, "BL616",
               sizeof(config.factory_info.model_name));
    os_strlcpy(config.factory_info.model_number, "WL80211",
               sizeof(config.factory_info.model_number));
    os_strlcpy(config.factory_info.manufacturer, "Bouffalo Lab",
               sizeof(config.factory_info.manufacturer));

    os_memcpy(runtime->target_ssid, res->ssid, res->ssid_len);
    runtime->target_ssid[res->ssid_len] = '\0';
    config.target_ssid = runtime->target_ssid;

    wl80211_p2p_set_role(WL80211_P2P_ROLE_CLIENT);
    wl80211_p2p_set_group_forming(true);
    os_memcpy(wl80211_glb.p2p.group.peer_dev_addr, res->peer_device_addr,
              ETH_ALEN);
    os_memcpy(wl80211_glb.p2p.group.go_dev_addr, res->peer_interface_addr,
              ETH_ALEN);
    wl80211_glb.p2p.group.operating_freq = (uint16_t) res->freq;
    wl80211_glb.p2p.group.persistent = res->persistent_group;

    err = bl_wifi_wps_start(&config);
    if (err != BL_WPS_ERR_OK) {
        wl80211_p2p_set_group_forming(false);
        printf("[P2P] client WPS start FAIL err=%d\r\n", err);
        return -1;
    }

    printf("[P2P] client WPS started ssid=%s method=%s\r\n",
           runtime->target_ssid, bl_p2p_method_name(res->wps_method));
    return 0;
}

static const struct wl80211_scan_result_item *
bl_p2p_find_peer_scan_result(const uint8_t peer_addr[ETH_ALEN])
{
    struct wl80211_scan_result_item *item;

    if (peer_addr == NULL) {
        return NULL;
    }

    RB_FOREACH(item, _scan_result_tree, &wl80211_scan_result)
    {
        if (!(item->flags & WL80211_SCAN_AP_RESULT_FLAGS_HAS_P2P)) {
            continue;
        }

        if ((!wl80211_p2p_addr_is_zero(item->p2p_dev_addr) &&
             os_memcmp(item->p2p_dev_addr, peer_addr, ETH_ALEN) == 0) ||
            os_memcmp(item->bssid, peer_addr, ETH_ALEN) == 0) {
            return item;
        }
    }

    return NULL;
}

static const struct wl80211_scan_result_item *
bl_p2p_wait_peer_scan_result(const uint8_t peer_addr[ETH_ALEN])
{
    const struct wl80211_scan_result_item *scan_item;
    unsigned int waited_ms;

    for (waited_ms = 0; waited_ms <= BL_P2P_JOIN_SCAN_WAIT_MS;
         waited_ms += BL_P2P_JOIN_SCAN_POLL_MS) {
        scan_item = bl_p2p_find_peer_scan_result(peer_addr);
        if (scan_item != NULL && scan_item->ssid != NULL &&
            scan_item->ssid[0] != '\0') {
            int freq = scan_item->p2p_oper_channel != 0 ?
                           wl80211_channel_to_freq(scan_item->p2p_oper_channel) :
                           wl80211_channel_to_freq(scan_item->channel);

            if (freq > 0) {
                if (waited_ms != 0U) {
                    wpa_printf(MSG_INFO,
                               "P2P join waited %u ms for peer " MACSTR
                               " to appear in scan cache",
                               waited_ms, MAC2STR(peer_addr));
                }
                return scan_item;
            }
        }

        if (waited_ms == BL_P2P_JOIN_SCAN_WAIT_MS) {
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(BL_P2P_JOIN_SCAN_POLL_MS));
    }

    return NULL;
}

static int bl_p2p_prepare_join_results(
    struct bl_p2p_runtime *runtime, const uint8_t peer_addr[ETH_ALEN],
    enum p2p_wps_method method, const char *pin, struct p2p_go_neg_results *res)
{
    const struct wl80211_scan_result_item *scan_item;
    const char *ssid;
    uint8_t peer_dev_addr[ETH_ALEN];
    int freq;
    size_t ssid_len;

    if (runtime == NULL || peer_addr == NULL || res == NULL) {
        wpa_printf(MSG_INFO,
                   "P2P join prepare rejected runtime=%p peer=%p res=%p",
                   runtime, peer_addr, res);
        return -1;
    }

    scan_item = bl_p2p_wait_peer_scan_result(peer_addr);
    if (scan_item == NULL) {
        wpa_printf(MSG_INFO, "P2P join prepare: peer " MACSTR
                             " not found in scan cache",
                   MAC2STR(peer_addr));
        return -1;
    }

    ssid = scan_item->ssid;
    if (ssid == NULL || ssid[0] == '\0') {
        wpa_printf(MSG_INFO, "P2P join prepare: peer " MACSTR
                             " has empty SSID in scan cache",
                   MAC2STR(peer_addr));
        return -1;
    }

    ssid_len = os_strlen(ssid);
    if (ssid_len == 0 || ssid_len > sizeof(res->ssid)) {
        wpa_printf(MSG_INFO,
                   "P2P join prepare: peer " MACSTR
                   " has invalid SSID length=%u limit=%u",
                   MAC2STR(peer_addr), (unsigned int) ssid_len,
                   (unsigned int) sizeof(res->ssid));
        return -1;
    }

    freq = scan_item->p2p_oper_channel != 0 ?
               wl80211_channel_to_freq(scan_item->p2p_oper_channel) :
               wl80211_channel_to_freq(scan_item->channel);
    if (freq <= 0) {
        wpa_printf(MSG_INFO,
                   "P2P join prepare: peer " MACSTR
                   " has invalid freq from oper_ch=%u channel=%u",
                   MAC2STR(peer_addr), scan_item->p2p_oper_channel,
                   scan_item->channel);
        return -1;
    }

    os_memset(res, 0, sizeof(*res));
    res->status = P2P_SC_SUCCESS;
    res->role_go = 0;
    res->freq = freq;
    res->ssid_len = ssid_len;
    os_memcpy(res->ssid, ssid, ssid_len);
    res->wps_method = method;
    res->persistent_group = !!(scan_item->p2p_group_capab &
                               P2P_GROUP_CAPAB_PERSISTENT_GROUP);

    if (!wl80211_p2p_addr_is_zero(scan_item->p2p_dev_addr)) {
        os_memcpy(peer_dev_addr, scan_item->p2p_dev_addr, ETH_ALEN);
    } else {
        os_memcpy(peer_dev_addr, peer_addr, ETH_ALEN);
    }
    os_memcpy(res->peer_device_addr, peer_dev_addr, ETH_ALEN);
    os_memcpy(res->peer_interface_addr, scan_item->bssid, ETH_ALEN);

    wpa_printf(MSG_INFO,
               "P2P join target selected: peer=" MACSTR
               " scan_bssid=" MACSTR " dev_addr=" MACSTR
               " ssid=%s freq=%d method=%s grp_cap=0x%02x oper_ch=%u",
               MAC2STR(peer_addr), MAC2STR(scan_item->bssid),
               MAC2STR(peer_dev_addr), ssid, freq,
               bl_p2p_method_name(method), scan_item->p2p_group_capab,
               scan_item->p2p_oper_channel);

    bl_p2p_prepare_pending(runtime, peer_dev_addr, method, pin, 1);
    return 0;
}

static void bl_p2p_group_remove(struct bl_p2p_runtime *runtime, bool stop_ap)
{
    if (runtime == NULL) {
        return;
    }

    bl_wifi_unset_appie_ram_internal(0xff, WIFI_APPIE_P2P_ASSOC_RESP, false);
    bl_wifi_unset_appie_ram_internal(0xff, WIFI_APPIE_P2P_BEACON_GO, false);
    bl_wifi_unset_appie_ram_internal(0xff, WIFI_APPIE_P2P_PROBE_RESP_GO,
                                     false);

    if (runtime->group != NULL) {
        p2p_group_deinit(runtime->group);
        runtime->group = NULL;
    }

    if (stop_ap && wl80211_ap_status()) {
        _wifi_mgmr_ap_stop_dhcpd();
        wifi_mgmr_ap_stop();
    }

    runtime->group_started = 0U;
    wl80211_p2p_mark_group_removed();
}

static void bl_p2p_reset_runtime(struct bl_p2p_runtime *runtime)
{
    bl_p2p_listen_pin_cb_t listen_pin_cb = runtime->listen_pin_cb;
    void *listen_pin_cb_arg = runtime->listen_pin_cb_arg;

    os_memset(runtime, 0, sizeof(*runtime));
    runtime->listen_pin_cb = listen_pin_cb;
    runtime->listen_pin_cb_arg = listen_pin_cb_arg;
    os_strlcpy(runtime->device_name, BL_P2P_DEFAULT_DEVICE_NAME,
               sizeof(runtime->device_name));
}

int bl_p2p_set_device_name(const char *name)
{
    struct bl_p2p_runtime *runtime = &g_bl_p2p_runtime;

    if (name == NULL || name[0] == '\0') {
        os_strlcpy(runtime->device_name, BL_P2P_DEFAULT_DEVICE_NAME,
                   sizeof(runtime->device_name));
    } else {
        os_strlcpy(runtime->device_name, name, sizeof(runtime->device_name));
    }

    if (runtime->ready) {
        runtime->config.dev_name = runtime->device_name;
    }

    return 0;
}

int bl_p2p_set_go_intent(int go_intent, int listen_wps_go_intent)
{
    struct bl_p2p_runtime *runtime = &g_bl_p2p_runtime;

    if (go_intent > BL_P2P_MAX_GO_INTENT ||
        listen_wps_go_intent > BL_P2P_MAX_GO_INTENT) {
        return -1;
    }

    /* A negative value keeps the current setting for that entry. */
    if (go_intent >= 0) {
        g_bl_p2p_go_intent = go_intent;
    }
    if (listen_wps_go_intent >= 0) {
        g_bl_p2p_listen_wps_go_intent = listen_wps_go_intent;
    }

    /* Push the value matching the current role to the live p2p instance so
     * the new setting takes effect without a re-init. */
    if (runtime->p2p != NULL) {
        p2p_set_go_intent(runtime->p2p,
                          (u8) (runtime->listen_wps_armed ?
                                    g_bl_p2p_listen_wps_go_intent :
                                    g_bl_p2p_go_intent));
    }

    return 0;
}

void bl_p2p_get_go_intent(int *go_intent, int *listen_wps_go_intent)
{
    if (go_intent != NULL) {
        *go_intent = g_bl_p2p_go_intent;
    }
    if (listen_wps_go_intent != NULL) {
        *listen_wps_go_intent = g_bl_p2p_listen_wps_go_intent;
    }
}

bool bl_p2p_is_ready(void)
{
    return g_bl_p2p_runtime.ready && g_bl_p2p_runtime.p2p != NULL;
}

int bl_p2p_init(void)
{
    struct bl_p2p_runtime *runtime = &g_bl_p2p_runtime;
    uint8_t mac[ETH_ALEN];
    const char *country;
    char device_name[sizeof(runtime->device_name)];
    static const uint8_t pri_dev_type[8] = {
        0x00, 0x0A, 0x00, 0x50, 0xF2, 0x04, 0x00, 0x05
    };

    if (runtime->ready) {
        return 0;
    }

    os_strlcpy(device_name,
               runtime->device_name[0] != '\0' ? runtime->device_name :
                                                 BL_P2P_DEFAULT_DEVICE_NAME,
               sizeof(device_name));
    bl_p2p_reset_runtime(runtime);
    os_strlcpy(runtime->device_name, device_name, sizeof(runtime->device_name));
    if (platform_get_mac(WL80211_VIF_STA, mac) < 0) {
        return -2;
    }

    country = wl80211_get_country_code();
    if (country == NULL || os_strlen(country) < 2) {
        return -3;
    }

    runtime->config.country[0] = country[0];
    runtime->config.country[1] = country[1];
    runtime->config.country[2] = 0x04;
    runtime->config.reg_class = BL_P2P_LISTEN_REG_CLASS;
    runtime->config.channel = BL_P2P_LISTEN_CHANNEL;
    runtime->config.op_reg_class = BL_P2P_LISTEN_REG_CLASS;
    runtime->config.op_channel = BL_P2P_LISTEN_CHANNEL;
    runtime->config.concurrent_operations = 1;
    runtime->config.max_peers = 32;
    runtime->config.passphrase_len = 8;
    runtime->config.max_listen = 5000;
    runtime->config.cb_ctx = runtime;
    runtime->config.debug_print = bl_p2p_debug_print;
    runtime->config.p2p_scan = bl_p2p_scan;
    runtime->config.send_probe_resp = bl_p2p_send_probe_resp;
    runtime->config.send_action = bl_p2p_send_action;
    runtime->config.send_action_done = bl_p2p_send_action_done;
    runtime->config.start_listen = bl_p2p_start_listen;
    runtime->config.stop_listen = bl_p2p_stop_listen;
    runtime->config.dev_found = bl_p2p_event_dev_found;
    runtime->config.dev_lost = bl_p2p_event_dev_lost;
    runtime->config.find_stopped = bl_p2p_event_find_stopped;
    runtime->config.go_neg_req_rx = bl_p2p_event_go_neg_req_rx;
    runtime->config.go_neg_completed = bl_p2p_event_go_neg_completed;
    runtime->config.prov_disc_req = bl_p2p_event_prov_disc_req;
    runtime->config.prov_disc_resp = bl_p2p_event_prov_disc_resp;
    runtime->config.prov_disc_fail = bl_p2p_event_prov_disc_fail;
    runtime->config.go_connected = bl_p2p_go_connected;
    runtime->config.is_concurrent_session_active = bl_p2p_is_concurrent_session_active;
    runtime->config.is_p2p_in_progress = bl_p2p_is_p2p_in_progress;
    runtime->config.config_methods = BL_P2P_CONFIG_METHODS_DEFAULT;
    runtime->config.dev_name = runtime->device_name;
    /* Build Wi-Fi Direct spec-compliant SSID postfix "-<device_name>" so the
     * advertised SSID becomes DIRECT-XX-<name>, which most P2P peers expect to
     * show in the Wi-Fi Direct device list. */
    {
        size_t name_len = os_strlen(runtime->device_name);
        size_t max = sizeof(runtime->config.ssid_postfix);
        if (name_len > 0 && max >= 2) {
            size_t copy = name_len + 1; /* leading '-' + name */
            if (copy > max) {
                copy = max;
            }
            runtime->config.ssid_postfix[0] = '-';
            os_memcpy(&runtime->config.ssid_postfix[1],
                      runtime->device_name, copy - 1);
            runtime->config.ssid_postfix_len = copy;
        } else {
            runtime->config.ssid_postfix_len = 0;
        }
    }
    runtime->config.manufacturer = "Bouffalo Lab";
    runtime->config.model_name = "BL616";
    runtime->config.model_number = "WL80211";
    runtime->config.serial_number = "BL616-P2P";
    os_memcpy(runtime->config.dev_addr, mac, ETH_ALEN);
    os_memcpy(runtime->config.pri_dev_type, pri_dev_type, sizeof(pri_dev_type));
    uuid_gen_mac_addr(mac, runtime->config.uuid);
    bl_p2p_fill_channels(&runtime->config.channels);
    bl_p2p_fill_channels(&runtime->config.cli_channels);

    runtime->p2p = p2p_init(&runtime->config);
    if (runtime->p2p == NULL) {
        bl_p2p_reset_runtime(runtime);
        return -4;
    }

    bl_p2p_apply_default_methods(runtime);

    if (bl_p2p_glue_register_mgmt_rx(bl_p2p_mgmt_rx, runtime) < 0) {
        p2p_deinit(runtime->p2p);
        bl_p2p_reset_runtime(runtime);
        return -5;
    }

    if (async_register_event_filter(EV_WL80211, bl_p2p_event_handler, runtime) != 0) {
        bl_p2p_glue_unregister_mgmt_rx();
        p2p_deinit(runtime->p2p);
        bl_p2p_reset_runtime(runtime);
        return -6;
    }

    {
        static const struct p2p_funcs p2p_cb = {
            .notify_ap_assoc = bl_p2p_notify_ap_assoc,
            .notify_ap_disassoc = bl_p2p_notify_ap_disassoc,
        };
        bl_wifi_register_p2p_cb_internal(&p2p_cb);
    }

    wl80211_p2p_mark_device_ready(false);
    runtime->ready = 1U;
    wpa_printf(MSG_INFO, "P2P runtime initialized on " MACSTR, MAC2STR(mac));
    return 0;
}

void bl_p2p_deinit(void)
{
    struct bl_p2p_runtime *runtime = &g_bl_p2p_runtime;

    if (!runtime->ready) {
        return;
    }

    bl_wifi_unregister_p2p_cb_internal();
    bl_p2p_stop();
    async_unregister_event_filter(EV_WL80211, bl_p2p_event_handler, runtime);
    bl_p2p_glue_unregister_mgmt_rx();
    bl_wifi_unset_appie_ram_internal(0xff, WIFI_APPIE_P2P_PROBE_REQ, true);
    bl_wifi_unset_appie_ram_internal(0xff, WIFI_APPIE_P2P_PROBE_RESP, true);
    p2p_deinit(runtime->p2p);
    wl80211_p2p_mark_group_removed();
    bl_p2p_reset_runtime(runtime);
}

int bl_p2p_find(unsigned int timeout_sec)
{
    if (!bl_p2p_is_ready()) {
        return -1;
    }

    return p2p_find(g_bl_p2p_runtime.p2p,
                    timeout_sec,
                    P2P_FIND_ONLY_SOCIAL,
                    0, NULL, NULL, 0, 0, NULL, 0, false);
}

int bl_p2p_listen(unsigned int duration_ms)
{
    if (!bl_p2p_is_ready()) {
        return -1;
    }

    if (g_bl_p2p_runtime.listen_wps_armed) {
        return bl_p2p_listen_wps_listen(&g_bl_p2p_runtime, duration_ms);
    }

    if (duration_ms == 0) {
        duration_ms = 5000;
    }

    return p2p_listen(g_bl_p2p_runtime.p2p, duration_ms);
}

int bl_p2p_listen_on_channel(unsigned int duration_ms, uint8_t channel)
{
    int freq;

    if (!bl_p2p_is_ready() || channel == 0 ||
        channel > BL_P2P_MAX_2G_CHANNEL) {
        return -1;
    }

    freq = wl80211_channel_to_freq(channel);
    if (freq <= 0) {
        return -1;
    }

    if (duration_ms == 0) {
        duration_ms = 5000;
    }

    if (g_bl_p2p_runtime.listen_wps_armed) {
        g_bl_p2p_runtime.listen_wps_channel_locked = 1U;
        g_bl_p2p_runtime.listen_wps_locked_channel = channel;
        return bl_p2p_listen_wps_listen(&g_bl_p2p_runtime, duration_ms);
    }

    bl_p2p_set_runtime_channel(&g_bl_p2p_runtime, channel);
    return p2p_listen(g_bl_p2p_runtime.p2p, duration_ms);
}

void bl_p2p_set_listen_pin_cb(bl_p2p_listen_pin_cb_t cb, void *arg)
{
    g_bl_p2p_runtime.listen_pin_cb = cb;
    g_bl_p2p_runtime.listen_pin_cb_arg = arg;
}

int bl_p2p_listen_wps_start(enum bl_p2p_wps_mode mode, uint8_t channel,
                            unsigned int duration_ms, const char *pin)
{
    struct bl_p2p_runtime *runtime = &g_bl_p2p_runtime;

    if (!bl_p2p_is_ready() || channel == 0 ||
        channel > BL_P2P_MAX_2G_CHANNEL) {
        return -1;
    }

    if (mode != BL_P2P_WPS_PBC && mode != BL_P2P_WPS_PIN_DISPLAY) {
        return -1;
    }

    runtime->listen_wps_armed = 0;
    p2p_stop_find(runtime->p2p);
    p2p_flush(runtime->p2p);
    runtime->listen_wps_channel_locked = 0U;
    runtime->listen_wps_peer_authorized = 0U;
    runtime->listen_wps_locked_channel = 0U;
    runtime->listen_wps_probe_count = 0U;
    runtime->listen_wps_pd_count = 0U;
    runtime->listen_wps_listen_count = 0U;
    runtime->listen_wps_remaining_ms = duration_ms != 0 ? duration_ms : 20000U;
    runtime->listen_wps_mode = mode;
    runtime->listen_wps_pin_user_supplied = 0U;
    runtime->listen_wps_pin_announced = 0U;
    os_memset(runtime->listen_wps_authorized_peer, 0, ETH_ALEN);
    runtime->pending_pin[0] = '\0';

    if (!bl_p2p_listen_wps_is_pbc(mode) && pin != NULL && pin[0] != '\0') {
        os_strlcpy(runtime->pending_pin, pin, sizeof(runtime->pending_pin));
        runtime->listen_wps_pin_user_supplied = 1U;
    }

    runtime->listen_wps_armed = 1;
    runtime->listen_wps_channel_locked = 1U;
    runtime->listen_wps_locked_channel = channel;
    bl_p2p_set_runtime_channel(runtime, channel);
    p2p_set_config_methods(runtime->p2p,
                           bl_p2p_listen_wps_config_methods(mode));
    p2p_set_go_intent(runtime->p2p, (u8) g_bl_p2p_listen_wps_go_intent);
    /* Maximize listen-state duty cycle so peers have a high chance to catch
     * our probe response. Values are in units of 100 TU
     * (1 TU = 1024 us). 10..20 -> ~1024..2048 ms per listen window. */
    if (runtime->p2p != NULL) {
        p2p_set_disc_int(runtime->p2p, 10, 20, -1);
    }

    return bl_p2p_listen_wps_listen(runtime, 0);
}

void bl_p2p_listen_wps_disarm(void)
{
    g_bl_p2p_runtime.listen_wps_armed = 0;
    g_bl_p2p_runtime.listen_wps_channel_locked = 0U;
    g_bl_p2p_runtime.listen_wps_peer_authorized = 0U;
    g_bl_p2p_runtime.listen_wps_locked_channel = 0U;
    g_bl_p2p_runtime.listen_wps_probe_count = 0U;
    g_bl_p2p_runtime.listen_wps_pd_count = 0U;
    g_bl_p2p_runtime.listen_wps_listen_count = 0U;
    g_bl_p2p_runtime.listen_wps_remaining_ms = 0U;
    g_bl_p2p_runtime.listen_wps_mode = BL_P2P_WPS_PBC;
    g_bl_p2p_runtime.listen_wps_pin_user_supplied = 0U;
    g_bl_p2p_runtime.listen_wps_pin_announced = 0U;
    os_memset(g_bl_p2p_runtime.listen_wps_authorized_peer, 0, ETH_ALEN);
    g_bl_p2p_runtime.pending_pin[0] = '\0';
    bl_p2p_apply_default_methods(&g_bl_p2p_runtime);
    bl_p2p_set_runtime_channel(&g_bl_p2p_runtime, BL_P2P_LISTEN_CHANNEL);
}

const char *bl_p2p_listen_wps_pin(void)
{
    return g_bl_p2p_runtime.pending_pin[0] != '\0' ?
               g_bl_p2p_runtime.pending_pin :
               NULL;
}

int bl_p2p_stop(void)
{
    if (!bl_p2p_is_ready()) {
        return -1;
    }

    bl_p2p_listen_wps_disarm();
    bl_wifi_wps_stop();
    p2p_stop_find(g_bl_p2p_runtime.p2p);
    bl_p2p_stop_listen_internal(&g_bl_p2p_runtime, true);
    wl80211_p2p_set_discovery(false);
    if (wl80211_sta_is_connected()) {
        wifi_mgmr_sta_disconnect();
    }
    bl_p2p_group_remove(&g_bl_p2p_runtime, true);
    return 0;
}

int bl_p2p_group_add(unsigned int freq, enum bl_p2p_wps_mode mode,
                     const char *pin)
{
    struct p2p_go_neg_results res;
    struct bl_p2p_runtime *runtime = &g_bl_p2p_runtime;

    if (!bl_p2p_is_ready()) {
        return -1;
    }

    os_memset(&res, 0, sizeof(res));
    if (p2p_go_params(runtime->p2p, &res) < 0) {
        return -1;
    }

    if (freq == 0) {
        freq = bl_p2p_current_freq(runtime);
    }
    res.freq = (int) freq;
    res.wps_method = bl_p2p_mode_to_method(mode);
    bl_p2p_prepare_pending(runtime, NULL, res.wps_method, pin, 15);
    return bl_p2p_start_go(runtime, &res, true);
}

int bl_p2p_connect(const uint8_t peer_addr[6], enum bl_p2p_wps_mode mode,
                   const char *pin, int go_intent, bool auth)
{
    uint8_t ifaddr[ETH_ALEN];
    struct bl_p2p_runtime *runtime = &g_bl_p2p_runtime;
    enum p2p_wps_method method;

    if (!bl_p2p_is_ready() || peer_addr == NULL) {
        return -1;
    }

    /* 0 is a valid GO intent (always become client), so only reject values
     * outside of the 0..15 range defined by the P2P spec. */
    if (go_intent < 0 || go_intent > BL_P2P_MAX_GO_INTENT) {
        go_intent = g_bl_p2p_go_intent;
    }

    if (platform_get_mac(WL80211_VIF_STA, ifaddr) < 0) {
        return -1;
    }

    method = bl_p2p_mode_to_method(mode);
    bl_p2p_prepare_pending(runtime, peer_addr, method, pin, go_intent);
    wl80211_p2p_set_group_forming(true);
    wl80211_p2p_set_role(WL80211_P2P_ROLE_DEVICE);

    if (auth) {
        return p2p_authorize(runtime->p2p, peer_addr, method, go_intent,
                             ifaddr, runtime->pending_freq, 0, NULL, 0, 0, 0);
    }

    return p2p_connect(runtime->p2p, peer_addr, method, go_intent, ifaddr,
                       runtime->pending_freq, 0, NULL, 0, 0, 0, 0);
}

int bl_p2p_join(const uint8_t peer_addr[6], enum bl_p2p_wps_mode mode,
                const char *pin)
{
    struct bl_p2p_runtime *runtime = &g_bl_p2p_runtime;
    struct p2p_go_neg_results res;
    enum p2p_wps_method method;

    if (!bl_p2p_is_ready() || peer_addr == NULL) {
        wpa_printf(MSG_INFO,
                   "P2P join rejected ready=%d peer=%p",
                   bl_p2p_is_ready(), peer_addr);
        return -1;
    }

    method = bl_p2p_mode_to_method(mode);
    if (bl_p2p_prepare_join_results(runtime, peer_addr, method, pin, &res) <
        0) {
        wpa_printf(MSG_INFO,
                   "P2P join prepare failed peer=" MACSTR " method=%s",
                   MAC2STR(peer_addr), bl_p2p_method_name(method));
        return -1;
    }

    p2p_stop_find(runtime->p2p);
    bl_p2p_stop_listen_internal(runtime, true);
    wl80211_p2p_set_discovery(false);

    if (bl_p2p_start_client(runtime, &res) < 0) {
        wpa_printf(MSG_INFO,
                   "P2P join start client failed peer=" MACSTR " ssid=%s freq=%d method=%s",
                   MAC2STR(peer_addr), res.ssid, res.freq,
                   bl_p2p_method_name(res.wps_method));
        return -1;
    }

    return 0;
}

int bl_p2p_prov_disc(const uint8_t peer_addr[6], enum bl_p2p_wps_mode mode,
                     bool join)
{
    enum p2p_wps_method method;
    uint16_t config_methods;

    if (!bl_p2p_is_ready() || peer_addr == NULL) {
        return -1;
    }

    method = bl_p2p_mode_to_method(mode);
    switch (method) {
        case WPS_PIN_DISPLAY:
            config_methods = WPS_CONFIG_DISPLAY;
            break;
        case WPS_PIN_KEYPAD:
            config_methods = WPS_CONFIG_KEYPAD;
            break;
        case WPS_PBC:
        default:
            config_methods = WPS_CONFIG_PUSHBUTTON;
            break;
    }

    return p2p_prov_disc_req(g_bl_p2p_runtime.p2p, peer_addr, NULL,
                             config_methods, join ? 1 : 0, 0, 1);
}

void bl_p2p_notify_ap_assoc(const uint8_t *addr, const uint8_t *ie, size_t len)
{
    struct bl_p2p_runtime *runtime = &g_bl_p2p_runtime;
    int ret;

    if (!bl_p2p_is_ready() || runtime->group == NULL || addr == NULL ||
        ie == NULL || len == 0) {
        return;
    }

    ret = p2p_group_notif_assoc(runtime->group, addr, ie, len);
    printf("[P2P] GO STA assoc peer=" MACSTR " ie_len=%u ret=%d\r\n",
           MAC2STR(addr), (unsigned int) len, ret);
    if (ret == 0) {
        bl_p2p_publish_assoc_resp_ie(runtime, P2P_SC_SUCCESS);
    } else {
        bl_p2p_publish_assoc_resp_ie(runtime,
                                     P2P_SC_FAIL_INVALID_PARAMS);
    }
}

void bl_p2p_notify_ap_disassoc(const uint8_t *addr)
{
    struct bl_p2p_runtime *runtime = &g_bl_p2p_runtime;

    if (!bl_p2p_is_ready() || runtime->group == NULL || addr == NULL) {
        return;
    }

    printf("[P2P] GO STA disassoc peer=" MACSTR "\r\n", MAC2STR(addr));
    p2p_group_notif_disassoc(runtime->group, addr);
    bl_p2p_publish_assoc_resp_ie(runtime, P2P_SC_SUCCESS);
}
