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

#define WG          wl80211_glb

const struct wpa_funcs *wpa_cbs;
static uint8_t *g_assoc_ie = NULL;
static uint16_t g_assoc_ie_len = 0;


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
}

int wl80211_supplicant_set_sta_key_internal(uint8_t vif_idx, uint8_t sta_idx, wpa_alg_t alg, int key_idx, int set_tx, uint8_t *seq,
                                 size_t seq_len, uint8_t *key, size_t key_len, bool pairwise)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
#endif
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

    wl80211_printf("%s:%d\n", __func__, __LINE__);
    wl80211_mac_disconnect(0, reason_code);
    return true;
}

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

bool wl80211_supplicant_skip_supp_pmkcaching(void)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
#endif
    // FIXME
    return false;
}

int wl80211_supplicant_set_igtk_internal(uint8_t vif_idx, uint8_t sta_idx, uint16_t key_idx, const uint8_t *pn, const uint8_t *key)
{
    // igtk length is 16
    wl80211_mac_set_key(vif_idx, 0xFF, key_idx, (uint8_t *)key, 16, MAC_CIPHER_BIP_CMAC_128, 0, 0);
    return 0;
}

int wl80211_supplicant_get_sta_gtk(uint8_t vif_idx, uint8_t *out_buf, uint8_t *out_len)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
    wl80211_printf("Function '%s' has not been implemented!!!\r\n", __FUNCTION__);
#endif
    //while(1);
    return 0;
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

    void *wl80211_ap_get_hostapd_ctx(void);
    return wl80211_ap_get_hostapd_ctx();
}

int wl80211_supplicant_set_ap_key_internal(uint8_t vif_idx, uint8_t sta_idx, wpa_alg_t alg, int key_idx, uint8_t *key,
                                size_t key_len, bool pairwise)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
    wl80211_printf("%s: vif_idx %u, sta_idx %u, alg %u, key_idx "
                   "%u, key_len %u, pairwise %d\r\n",
                   __FUNCTION__, vif_idx, sta_idx, alg, key_idx, key_len, pairwise);
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
    // apm_sta_remove(vif_idx, sta_idx, WLAN_FW_APM_DEAUTH_BY_AUTHENTICATOR, reason);
    return 0;
}

int wl80211_supplicant_get_assoc_bssid_internal(uint8_t vif_idx, uint8_t *bssid)
{
#ifdef DEBUG_FUNCTION
    wl80211_printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__, __LINE__);
#endif
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
}

int wl80211_supplicant_set_wps_cb_internal(const struct wps_funcs *wps_cb)
{
    // wps_cbs = wps_cb;
    return 0;
}

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
