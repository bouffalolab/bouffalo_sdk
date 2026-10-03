#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "wl80211.h"
#include "wl80211_mac.h"
#include "timeout.h"
#include "supplicant.h"

#include "bl_wpa.h"

// #define DEBUG_FUNCTION

#define WG wl80211_glb

const struct wpa_funcs *wpa_cbs;
static uint8_t *g_assoc_ie = NULL;
static uint16_t g_assoc_ie_len = 0;

#if defined(CONFIG_WL80211_P2P)
#include "wl80211_platform.h"

#define ETH_HDR_LEN 14

struct wl80211_ieee802_1x_hdr {
    uint8_t version;
    uint8_t type;
    uint16_t length;
} __attribute__((packed));

enum {
    WL80211_EAPOL_TYPE_EAP_PACKET = 0,
    WL80211_EAPOL_TYPE_EAPOL_START = 1,
};

extern struct wpa_sm gWpaSm;
extern void *wl80211_ap_get_hostapd_ctx(void);

struct wl80211_appie_slot {
    uint8_t *ie;
    uint16_t len;
};

static struct {
    struct wl80211_appie_slot appie[2][WIFI_APPIE_MAX];
    struct wl80211_appie_slot appie_ram[2][WIFI_APPIE_RAM_MAX - WIFI_APPIE_MAX];
    const struct wps_funcs *wps_cb;
    wps_status_t wps_status;
    uint8_t sta_gtk[WL80211_VIF_MAX][32];
    uint8_t sta_gtk_len[WL80211_VIF_MAX];
    bool sta_gtk_valid[WL80211_VIF_MAX];
} wl80211_supplicant_ctx;

static struct wl80211_appie_slot *wl80211_get_appie_slot(bool sta, wifi_appie_t type)
{
    if (type >= WIFI_APPIE_MAX) {
        return NULL;
    }

    return &wl80211_supplicant_ctx.appie[sta ? 1 : 0][type];
}

static struct wl80211_appie_slot *wl80211_get_appie_ram_slot(bool sta, wifi_appie_ram_t type)
{
    int idx = (int)type;

    if (idx < WIFI_APPIE_MAX || idx >= WIFI_APPIE_RAM_MAX) {
        return NULL;
    }

    return &wl80211_supplicant_ctx.appie_ram[sta ? 1 : 0][idx - WIFI_APPIE_MAX];
}

static int wl80211_replace_appie(struct wl80211_appie_slot *slot, const uint8_t *ie, uint16_t len)
{
    if (slot == NULL) {
        return -1;
    }

    if (slot->ie != NULL) {
        free(slot->ie);
        slot->ie = NULL;
        slot->len = 0;
    }

    if (ie == NULL || len == 0) {
        return 0;
    }

    slot->ie = malloc(len);
    if (slot->ie == NULL) {
        return -1;
    }

    memcpy(slot->ie, ie, len);
    slot->len = len;
    return 0;
}

static bool wl80211_eapol_should_route_to_wps(const uint8_t *payload, size_t len)
{
    const struct wl80211_ieee802_1x_hdr *xhdr;

    if (payload == NULL || len < ETH_HDR_LEN + sizeof(*xhdr)) {
        return false;
    }

    xhdr = (const struct wl80211_ieee802_1x_hdr *)(payload + ETH_HDR_LEN);
    return xhdr->type == WL80211_EAPOL_TYPE_EAP_PACKET || xhdr->type == WL80211_EAPOL_TYPE_EAPOL_START;
}
#endif

int wl80211_supplicant_init(void)
{
    int ret = bl_supplicant_init();

    return ret;
}

int wl80211_supplicant_register_wpa_cb_internal(const struct wpa_funcs *cb)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
#endif
    wpa_cbs = cb;
    return 0;
}

int wl80211_supplicant_unregister_wpa_cb_internal(void)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
#endif
    wpa_cbs = NULL;
    return 0;
}

int wl80211_supplicant_eapol_input(uint8_t vif_type, uint8_t *payload, size_t len)
{
#if defined(CONFIG_WL80211_P2P)
    const struct mac_eth_hdr *hdr = (const struct mac_eth_hdr *)payload;
    const uint8_t *sa = (const uint8_t *)hdr->sa.array;
    assert(hdr->len /* type */ == 0x8e88);

    void *ap_ctx = wl80211_ap_get_hostapd_ctx();

    /* route eapol packet*/
    if (vif_type == WL80211_VIF_STA && WG.associated) {
        if (!WG.link_up) {
            WG.authenticating = 1;
        }

        if (wl80211_supplicant_ctx.wps_cb != NULL && wl80211_supplicant_ctx.wps_status == WPS_STATUS_PENDING &&
            wl80211_eapol_should_route_to_wps(payload, len)) {
            wl80211_supplicant_ctx.wps_cb->wps_sm_rx_eapol((uint8_t *)hdr->sa.array, (uint8_t *)payload + ETH_HDR_LEN,
                                                           len - ETH_HDR_LEN);
        } else {
            wpa_cbs->wpa_sta_rx_eapol((uint8_t *)hdr->sa.array, (uint8_t *)payload + ETH_HDR_LEN, len - ETH_HDR_LEN);
        }
    } else if (vif_type == WL80211_VIF_AP && WG.ap_en && ap_ctx) {
        void *wpa_sm = _ap_get_wpa_sm((uint8_t *)hdr->sa.array);
        const struct wps_ap_funcs *wps_ap_cb = bl_wifi_get_wps_ap_cb_internal();

        if (wps_ap_cb != NULL && wps_ap_cb->rx_eapol != NULL &&
            wps_ap_cb->rx_eapol((const uint8_t *)hdr->sa.array, (uint8_t *)payload + ETH_HDR_LEN, len - ETH_HDR_LEN)) {
            return 0;
        }

        wpa_cbs->wpa_ap_rx_eapol(ap_ctx, wpa_sm, (uint8_t *)payload + ETH_HDR_LEN, len - ETH_HDR_LEN);
    }

    return 0;
#else
#define ETH_HDR_LEN 14

    const struct mac_eth_hdr *hdr = (const struct mac_eth_hdr *)payload;
    assert(hdr->len /* type */ == 0x8e88);

    void *wl80211_ap_get_hostapd_ctx(void);
    void *ap_ctx = wl80211_ap_get_hostapd_ctx();

    /* route eapol packet*/
    if (vif_type == WL80211_VIF_STA) {
        wpa_cbs->wpa_sta_rx_eapol((uint8_t *)hdr->sa.array, (uint8_t *)payload + ETH_HDR_LEN, len - ETH_HDR_LEN);
    } else if (vif_type == WL80211_VIF_AP && ap_ctx) {
        void *wpa_sm = _ap_get_wpa_sm((uint8_t *)hdr->sa.array);
        wpa_cbs->wpa_ap_rx_eapol(ap_ctx, wpa_sm, (uint8_t *)payload + ETH_HDR_LEN, len - ETH_HDR_LEN);
    }

    return 0;
#endif
}

int wl80211_supplicant_set_sta_key_internal(uint8_t vif_idx, uint8_t sta_idx, wpa_alg_t alg, int key_idx, int set_tx,
                                            uint8_t *seq, size_t seq_len, uint8_t *key, size_t key_len, bool pairwise)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
#endif
#if defined(CONFIG_WL80211_P2P)
    int mac_cipher_suite_value(uint32_t cipher_suite);

    uint8_t cipher_suite;

    (void)key_idx;
    (void)set_tx;
    (void)seq;
    (void)seq_len;

    if (vif_idx >= WL80211_VIF_MAX) {
        return -1;
    }

    if (alg == WIFI_WPA_ALG_NONE) {
        if (sta_idx == 0xFF) {
            wl80211_mac_clr_gtk(vif_idx, sta_idx);
            wl80211_supplicant_ctx.sta_gtk_valid[vif_idx] = false;
            wl80211_supplicant_ctx.sta_gtk_len[vif_idx] = 0;
        } else {
            wl80211_mac_clr_ptk(vif_idx, sta_idx);
        }
        return 0;
    }

    if (!pairwise && key_len > sizeof(wl80211_supplicant_ctx.sta_gtk[vif_idx])) {
        wl80211_printf("unsupported GTK length = %u\r\n", (unsigned int)key_len);
        return -1;
    }

    if (key_len == WPA_AES_KEY_LEN) {
        cipher_suite = mac_cipher_suite_value(MAC_RSNIE_CIPHER_CCMP_128);
    } else if (key_len == WPA_TKIP_KEY_LEN) {
        cipher_suite = mac_cipher_suite_value(MAC_RSNIE_CIPHER_TKIP);

        uint32_t array[WPA_TKIP_KEY_LEN / 4];
        uint32_t val1, val2;
        memcpy(&array, key, WPA_TKIP_KEY_LEN);
        val1 = array[4];
        val2 = array[5];
        array[4] = array[6];
        array[5] = array[7];
        array[6] = val1;
        array[7] = val2;
        memcpy(key, &array, WPA_TKIP_KEY_LEN);
    } else if (key_len == WPA_WEP104_KEY_LEN) {
        cipher_suite = mac_cipher_suite_value(MAC_RSNIE_CIPHER_WEP_104);
    } else if (key_len == WPA_WEP40_KEY_LEN) {
        cipher_suite = mac_cipher_suite_value(MAC_RSNIE_CIPHER_WEP_40);
    } else {
        wl80211_printf("unsupport key_len = %d!!!\r\n", key_len);
        abort();
    }

    if (pairwise) {
        wl80211_mac_set_key(vif_idx, sta_idx, key_idx, key, key_len, cipher_suite, 0, pairwise); // install PTK
    } else {
        wl80211_mac_set_key(vif_idx, 0xFF, key_idx, key, key_len, cipher_suite, 0, pairwise); // install GTK
        memcpy(wl80211_supplicant_ctx.sta_gtk[vif_idx], key, key_len);
        wl80211_supplicant_ctx.sta_gtk_len[vif_idx] = key_len;
        wl80211_supplicant_ctx.sta_gtk_valid[vif_idx] = true;
    }

    return 0;
#else
#define MAC_RSNIE_CIPHER_USE_GROUP    0x000FAC00
#define MAC_RSNIE_CIPHER_WEP_40       0x000FAC01
#define MAC_RSNIE_CIPHER_TKIP         0x000FAC02
#define MAC_RSNIE_CIPHER_RSVD         0x000FAC03
#define MAC_RSNIE_CIPHER_CCMP_128     0x000FAC04
#define MAC_RSNIE_CIPHER_WEP_104      0x000FAC05
#define MAC_RSNIE_CIPHER_BIP_CMAC_128 0x000FAC06
#define MAC_RSNIE_CIPHER_NO_GROUP     0x000FAC07
#define MAC_RSNIE_CIPHER_GCMP_128     0x000FAC08
#define MAC_RSNIE_CIPHER_GCMP_256     0x000FAC09
#define MAC_RSNIE_CIPHER_CCMP_256     0x000FAC0A
#define MAC_RSNIE_CIPHER_BIP_GMAC_128 0x000FAC0B
#define MAC_RSNIE_CIPHER_BIP_GMAC_256 0x000FAC0C
#define MAC_RSNIE_CIPHER_BIP_CMAC_256 0x000FAC0D

    int mac_cipher_suite_value(uint32_t cipher_suite);

    uint8_t cipher_suite;

    if (alg == WIFI_WPA_ALG_NONE) {
        if (sta_idx == 0xFF) {
            wl80211_mac_clr_gtk(vif_idx, sta_idx);
        } else {
            wl80211_mac_clr_ptk(vif_idx, sta_idx);
        }
        return 0;
    }

    if (key_len == WPA_AES_KEY_LEN) {
        cipher_suite = mac_cipher_suite_value(MAC_RSNIE_CIPHER_CCMP_128);
    } else if (key_len == WPA_TKIP_KEY_LEN) {
        cipher_suite = mac_cipher_suite_value(MAC_RSNIE_CIPHER_TKIP);

        uint32_t array[WPA_TKIP_KEY_LEN / 4];
        uint32_t val1, val2;
        memcpy(&array, key, WPA_TKIP_KEY_LEN);
        val1 = array[4];
        val2 = array[5];
        array[4] = array[6];
        array[5] = array[7];
        array[6] = val1;
        array[7] = val2;
        memcpy(key, &array, WPA_TKIP_KEY_LEN);
    } else if (key_len == WPA_WEP104_KEY_LEN) {
        cipher_suite = mac_cipher_suite_value(MAC_RSNIE_CIPHER_WEP_104);
    } else if (key_len == WPA_WEP40_KEY_LEN) {
        cipher_suite = mac_cipher_suite_value(MAC_RSNIE_CIPHER_WEP_40);
    } else {
        wl80211_printf("unsupport key_len = %d!!!\r\n", key_len);
        abort();
    }

    if (pairwise) {
        wl80211_mac_set_key(vif_idx, sta_idx, key_idx, key, key_len, cipher_suite, 0, pairwise); // install PTK
    } else {
        wl80211_mac_set_key(vif_idx, 0xFF, key_idx, key, key_len, cipher_suite, 0, pairwise); // install GTK
    }

    return 0;
#endif
}

bool wl80211_supplicant_auth_done_internal(uint8_t sta_idx, uint16_t reason_code)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
    wl80211_printf("%s: sta_idx %u, reason_code %u\r\n", __FUNCTION__, sta_idx, reason_code);
#endif

    if (reason_code == 0) {
        if (!WG.link_up) {
            wl80211_printf("wpa auth success\n"); // CI will check this
            wl80211_mac_ctrl_port(sta_idx, 1);
        }
        return true;
    }

#if defined(CONFIG_WL80211_P2P)
    wl80211_printf("wpa auth failed: sta_idx=%u reason=%u\r\n", sta_idx, reason_code);
    wl80211_mac_disconnect(reason_code, WLAN_FW_DISCONNECT_BY_USER_WITH_DEAUTH);
#else
    wl80211_printf("%s:%d\n", __func__, __LINE__);
    wl80211_mac_disconnect(reason_code, WLAN_FW_4WAY_HANDSHAKE_ERROR_PSK_TIMEOUT_FAILURE);
#endif
    return true;
}

void bl_wifi_timer_start(bl_wifi_timer_t *ptimer, uint32_t time_ms)
{
    timeout_start(&ptimer->e, time_ms);
}

void bl_wifi_timer_stop(bl_wifi_timer_t *ptimer)
{
    timeout_stop(&ptimer->e);
}

void bl_wifi_timer_setfn(bl_wifi_timer_t *ptimer, bl_wifi_timer_func_t *pfunction, void *parg)
{
    ptimer->e.callback = (void *)pfunction;
    ptimer->e.opaque = parg;
}

#if defined(CONFIG_WL80211_P2P)
void *bl_wifi_get_hostap_private_internal(void)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
#endif
    return wl80211_ap_get_hostapd_ctx();
}

int wl80211_supplicant_set_appie_internal(uint8_t vif_idx, wifi_appie_t type, uint8_t *ie, uint16_t len, bool sta)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
#endif
    struct wl80211_appie_slot *slot;

    (void)vif_idx;
    slot = wl80211_get_appie_slot(sta, type);
    return wl80211_replace_appie(slot, ie, len);
}

int wl80211_supplicant_unset_appie_internal(uint8_t vif_idx, wifi_appie_t type)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
#endif
    return wl80211_supplicant_set_appie_internal(vif_idx, type, NULL, 0, false);
}

const uint8_t *wl80211_supplicant_get_appie_internal(bool sta, wifi_appie_t type, uint16_t *len)
{
    struct wl80211_appie_slot *slot = wl80211_get_appie_slot(sta, type);

    if (len != NULL) {
        *len = 0;
    }
    if (slot == NULL || slot->ie == NULL) {
        return NULL;
    }

    if (len != NULL) {
        *len = slot->len;
    }
    return slot->ie;
}
#else
int wl80211_supplicant_set_appie_internal(uint8_t vif_idx, wifi_appie_t type, uint8_t *ie, uint16_t len, bool sta)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
#endif
    return 0;
}

int wl80211_supplicant_unset_appie_internal(uint8_t vif_idx, wifi_appie_t type)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
#endif
    return wl80211_supplicant_set_appie_internal(vif_idx, type, NULL, 0, false);
}
#endif

#if defined(CONFIG_WL80211_P2P)
int bl_wifi_set_appie_ram_internal(uint8_t vif_idx, wifi_appie_ram_t type, const uint8_t *ie, uint16_t len, bool sta)
{
    struct wl80211_appie_slot *slot;

    (void)vif_idx;
    slot = wl80211_get_appie_ram_slot(sta, type);
    return wl80211_replace_appie(slot, ie, len);
}

int bl_wifi_unset_appie_ram_internal(uint8_t vif_idx, wifi_appie_ram_t type, bool sta)
{
    return bl_wifi_set_appie_ram_internal(vif_idx, type, NULL, 0, sta);
}

const uint8_t *bl_wifi_get_appie_ram_internal(bool sta, wifi_appie_ram_t type, uint16_t *len)
{
    struct wl80211_appie_slot *slot = wl80211_get_appie_ram_slot(sta, type);

    if (len != NULL) {
        *len = 0;
    }
    if (slot == NULL || slot->ie == NULL) {
        return NULL;
    }

    if (len != NULL) {
        *len = slot->len;
    }
    return slot->ie;
}
#endif

bool wl80211_supplicant_skip_supp_pmkcaching(void)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
#endif
    // FIXME
    return false;
}

int wl80211_supplicant_sta_update_ap_info_internal(void)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
#endif
    // TODO: remove me
    return 1;
}

uint8_t wl80211_supplicant_sta_set_reset_param_internal(uint8_t reset_flag)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
#endif
    // TODO: remove me
    return 0;
}

bool wl80211_supplicant_sta_is_ap_notify_completed_rsne_internal(void)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
#endif
    // FIXME
    return true;
}

bool wl80211_supplicant_sta_is_running_internal(void)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
#endif
    // FIXME
    return true;
}

int wl80211_supplicant_set_assoc_ie(uint8_t *ie, uint16_t ie_len)
{
    g_assoc_ie = ie;
    g_assoc_ie_len = ie_len;

    return 0;
}

int wl80211_supplicant_get_assoc_ie(uint8_t **ie, uint16_t *ie_len)
{
    *ie = g_assoc_ie;
    *ie_len = g_assoc_ie_len;

    return 0;
}

void *wl80211_supplicant_get_hostap_private_internal(void)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
#endif

#if !defined(CONFIG_WL80211_P2P)
    void *wl80211_ap_get_hostapd_ctx(void);
#endif
    return wl80211_ap_get_hostapd_ctx();
}

int wl80211_supplicant_set_ap_key_internal(uint8_t vif_idx, uint8_t sta_idx, wpa_alg_t alg, int key_idx, uint8_t *key,
                                           size_t key_len, bool pairwise)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
    wl80211_printf(
        "bl_wifi_set_ap_key_internal: vif_idx %u, sta_idx %u, alg %u, key_idx %u, key_len %u, pairwise %d\r\n", vif_idx,
        sta_idx, alg, key_idx, key_len, pairwise);
#endif
    if (alg == WIFI_WPA_ALG_NONE) {
        if (sta_idx == 0xFF) {
            wl80211_mac_clr_gtk(vif_idx, sta_idx);
        } else {
            wl80211_mac_clr_ptk(vif_idx, sta_idx);
        }
        return 0;
    } else {
        if (pairwise) {
            wl80211_mac_set_key(vif_idx, sta_idx, key_idx, key, key_len, MAC_CIPHER_CCMP, 0, 1); // install PTK
        } else {
            wl80211_mac_set_key(vif_idx, 255, key_idx, key, key_len, MAC_CIPHER_CCMP, 0, 0); // install GTK
        }
    }

    return 0;
}

bool wl80211_supplicant_wpa_ptk_init_done_internal(uint8_t sta_idx)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
    wl80211_printf("%s: sta_idx %u\r\n", __FUNCTION__, sta_idx);
#endif

    wl80211_mac_ctrl_port(sta_idx, 1);
    return true;
}

int wl80211_supplicant_ap_deauth_internal(uint8_t vif_idx, uint8_t sta_idx, uint32_t reason)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
#endif
    wl80211_printf("%s: vif_idx %u, sta_idx %u, reason %lu\r\n", __FUNCTION__, vif_idx, sta_idx, reason);
    return 0;
}

int wl80211_supplicant_get_assoc_bssid_internal(uint8_t vif_idx, uint8_t *bssid)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
#endif
#if defined(CONFIG_WL80211_P2P)
    const struct wl80211_connect_params *params;

    (void)vif_idx;
    if (bssid == NULL) {
        return -1;
    }

    params = WG.connect_params ? WG.connect_params : WG.last_connect_params;
    if (params == NULL) {
        return -1;
    }

    memcpy(bssid, params->bssid, 6);
    return 0;
#else
#if 0
    struct vif_info_tag *vif;
    vif = vif_mgmt_get_vif(vif_idx);

    if (vif) {
        memcpy(bssid, &vif->bss_info.bssid, 6);
        return 0;
    } else {
        return -1;
    }
#else
    return -1;
#endif
#endif
}

#if defined(CONFIG_WL80211_P2P)
int wl80211_supplicant_set_wps_cb_internal(const struct wps_funcs *wps_cb)
{
    wl80211_supplicant_ctx.wps_cb = wps_cb;
    return 0;
}

const struct wps_funcs *bl_wifi_get_wps_cb_internal(void)
{
    return wl80211_supplicant_ctx.wps_cb;
}
#else
int wl80211_supplicant_set_wps_cb_internal(const struct wps_funcs *wps_cb)
{
    // wps_cbs = wps_cb;
    return 0;
}
#endif

#if defined(CONFIG_BL_SUPPLICANT_WPS) || defined(CONFIG_WL80211_P2P)
static const struct wps_ap_funcs *wps_ap_cbs;

int bl_wifi_register_wps_ap_cb_internal(const struct wps_ap_funcs *cb)
{
    wps_ap_cbs = cb;
    return 0;
}

int bl_wifi_unregister_wps_ap_cb_internal(void)
{
    wps_ap_cbs = NULL;
    return 0;
}

const struct wps_ap_funcs *bl_wifi_get_wps_ap_cb_internal(void)
{
    return wps_ap_cbs;
}
#else
int bl_wifi_register_wps_ap_cb_internal(const void *cb)
{
    (void)cb;
    return 0;
}

int bl_wifi_unregister_wps_ap_cb_internal(void)
{
    return 0;
}

const void *bl_wifi_get_wps_ap_cb_internal(void)
{
    return NULL;
}
#endif

#if defined(CONFIG_WL80211_P2P)
static const struct p2p_funcs *p2p_cbs;

int bl_wifi_register_p2p_cb_internal(const struct p2p_funcs *cb)
{
    p2p_cbs = cb;
    return 0;
}

int bl_wifi_unregister_p2p_cb_internal(void)
{
    p2p_cbs = NULL;
    return 0;
}

const struct p2p_funcs *bl_wifi_get_p2p_cb_internal(void)
{
    return p2p_cbs;
}
#else
int bl_wifi_register_p2p_cb_internal(const void *cb)
{
    (void)cb;
    return 0;
}

int bl_wifi_unregister_p2p_cb_internal(void)
{
    return 0;
}

const void *bl_wifi_get_p2p_cb_internal(void)
{
    return NULL;
}
#endif

#if defined(CONFIG_WL80211_P2P)
wps_status_t wl80211_supplicant_get_wps_status_internal(void)
{
    return wl80211_supplicant_ctx.wps_status;
}

void wl80211_supplicant_set_wps_status_internal(wps_status_t status)
{
    wl80211_supplicant_ctx.wps_status = status;
}
#else
wps_status_t wl80211_supplicant_get_wps_status_internal(void)
{
#if 0
    return sm_env.wps_status;
#else
    return WPS_STATUS_DISABLE;
#endif
}

void wl80211_supplicant_set_wps_status_internal(wps_status_t status)
{
#if 0
    sm_env.wps_status = status;
#endif
}
#endif

int wl80211_supplicant_set_igtk_internal(uint8_t vif_idx, uint8_t sta_idx, uint16_t key_idx, const uint8_t *pn,
                                         const uint8_t *key)
{
    // igtk length is 16
    wl80211_mac_set_key(vif_idx, 0xFF, key_idx, (uint8_t *)key, 16, MAC_CIPHER_BIP_CMAC_128, 0, 0);
    return 0;
}

int wl80211_supplicant_get_sta_gtk(uint8_t vif_idx, uint8_t *out_buf, uint8_t *out_len)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
#endif
#if defined(CONFIG_WL80211_P2P)
    uint8_t key_len;

    if (vif_idx >= WL80211_VIF_MAX || out_buf == NULL || out_len == NULL) {
        return -1;
    }

    if (!wl80211_supplicant_ctx.sta_gtk_valid[vif_idx]) {
        return -1;
    }

    key_len = wl80211_supplicant_ctx.sta_gtk_len[vif_idx];
    memcpy(out_buf, wl80211_supplicant_ctx.sta_gtk[vif_idx], key_len);
    *out_len = key_len;
    return 0;
#else
    wl80211_printf("Function '%s' has not been implemented!!!\r\n", __FUNCTION__);
    //while(1);
    return 0;
#endif
}
