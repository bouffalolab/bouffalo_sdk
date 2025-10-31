#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <supplicant_api.h>

#include "wl80211.h"
#include "timeout.h"

#include "macsw.h"

#if PSK_SUPPLICANT

#define DEBUG_FUNCTION

const struct wpa_funcs *wpa_cbs;
extern struct wpa_sm gWpaSm;

void sm_handle_supplicant_result(uint8_t sta_id, uint16_t reason_code);

int bl_wifi_ap_deauth_internal(uint8_t vif_idx, uint8_t sta_idx,
                               uint32_t reason) {
#ifdef DEBUG_FUNCTION
  printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__,
         __LINE__);
#endif
  printf("bl_wifi_ap_deauth_internal: vif_idx %u, sta_idx %u, reason %lu\r\n",
         vif_idx, sta_idx, reason);
  // apm_sta_remove(vif_idx, sta_idx, WLAN_FW_APM_DEAUTH_BY_AUTHENTICATOR,
  // reason);
  return 0;
}

int wl80211_mac_set_key(uint8_t key_idx, uint8_t sta_idx, uint8_t * key,
                        uint8_t key_len, uint8_t cipher_suite, bool pairwise);

int wl80211_mac_ctrl_port(uint8_t sta_id, int control_port_open);

bool bl_wifi_auth_done_internal(uint8_t sta_idx, uint16_t reason_code) {
#ifdef DEBUG_FUNCTION
  printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__,
         __LINE__);
  printf("bl_wifi_auth_done_internal: sta_idx %u, reason_code %u\r\n", sta_idx,
         reason_code);
#endif
  if (reason_code == 0) {
    printf("wpa auth success!\n");
    wl80211_mac_ctrl_port(sta_idx, 1);
    return true;
  }

  printf("%s:%d\n", __func__, __LINE__);
  void wl80211_mac_disconnect(uint16_t reason_code, uint16_t status_code);
  wl80211_mac_disconnect(reason_code, 0);
  return true;
}

void *bl_wifi_get_hostap_private_internal(void) {
#ifdef DEBUG_FUNCTION
  printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__,
         __LINE__);
#endif
  // return apm_get_hostapd_ctx();
  return NULL;
}

void bl_wifi_timer_start(bl_wifi_timer_t *ptimer, uint32_t time_ms) {
  timeout_start(&ptimer->e, time_ms);
}

void bl_wifi_timer_stop(bl_wifi_timer_t *ptimer) {
  timeout_stop(&ptimer->e);
}

void bl_wifi_timer_setfn(bl_wifi_timer_t *ptimer,
                         bl_wifi_timer_func_t *pfunction, void *parg) {
  ptimer->e.callback = (void *)pfunction;
  ptimer->e.opaque = parg;
}

int bl_wifi_set_appie_internal(uint8_t vif_idx, wifi_appie_t type, uint8_t *ie,
                               uint16_t len, bool sta) {
#ifdef DEBUG_FUNCTION
  printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__,
         __LINE__);
#endif
  return 0;
}

int bl_wifi_unset_appie_internal(uint8_t vif_idx, wifi_appie_t type) {
#ifdef DEBUG_FUNCTION
  printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__,
         __LINE__);
#endif
  return bl_wifi_set_appie_internal(vif_idx, type, NULL, 0, false);
}

bool bl_wifi_wpa_ptk_init_done_internal(uint8_t sta_idx) {
#ifdef DEBUG_FUNCTION
  printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__,
         __LINE__);
  printf("bl_wifi_wpa_ptk_init_done_internal: sta_idx %u\r\n", sta_idx);
#endif
  // apm_handle_auth_done(sta_idx);
  return true;
}

int bl_wifi_register_wpa_cb_internal(const struct wpa_funcs *cb) {
#ifdef DEBUG_FUNCTION
  printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__,
         __LINE__);
#endif
  wpa_cbs = cb;
  return 0;
}

int bl_wifi_unregister_wpa_cb_internal(void) {
#ifdef DEBUG_FUNCTION
  printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__,
         __LINE__);
#endif
  wpa_cbs = NULL;
  return 0;
}

bool bl_wifi_skip_supp_pmkcaching(void) {
#ifdef DEBUG_FUNCTION
  printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__,
         __LINE__);
#endif
  // FIXME
  return false;
}

int bl_wifi_sta_update_ap_info_internal(void) {
#ifdef DEBUG_FUNCTION
  printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__,
         __LINE__);
#endif
  // TODO: remove me
  return 1;
}

uint8_t bl_wifi_sta_set_reset_param_internal(uint8_t reset_flag) {
#ifdef DEBUG_FUNCTION
  printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__,
         __LINE__);
#endif
  // TODO: remove me
  return 0;
}

bool bl_wifi_sta_is_ap_notify_completed_rsne_internal(void) {
#ifdef DEBUG_FUNCTION
  printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__,
         __LINE__);
#endif
  // FIXME
  return true;
}

bool bl_wifi_sta_is_running_internal(void) {
#ifdef DEBUG_FUNCTION
  printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__,
         __LINE__);
#endif
  // FIXME
  return true;
}

int bl_wifi_set_ap_key_internal(uint8_t vif_idx, uint8_t sta_idx, wpa_alg_t alg,
                                int key_idx, uint8_t *key, size_t key_len,
                                bool pairwise) {
#ifdef DEBUG_FUNCTION
  printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__,
         __LINE__);
  printf("bl_wifi_set_ap_key_internal: vif_idx %u, sta_idx %u, alg %u, key_idx "
         "%u, key_len %u, pairwise %d\r\n",
         vif_idx, sta_idx, alg, key_idx, key_len, pairwise);
#endif
  /*if (alg == WIFI_WPA_ALG_NONE) {
      mm_sec_machwkey_del(MM_STA_TO_KEY(sta_idx));
  } else {
      set_key(vif_idx, sta_idx, alg, key_idx, 1, NULL, 0, key, key_len,
  pairwise);
  }*/
  return 0;
}

#define MAC_RSNIE_CIPHER_USE_GROUP 0x000FAC00
#define MAC_RSNIE_CIPHER_WEP_40 0x000FAC01
#define MAC_RSNIE_CIPHER_TKIP 0x000FAC02
#define MAC_RSNIE_CIPHER_RSVD 0x000FAC03
#define MAC_RSNIE_CIPHER_CCMP_128 0x000FAC04
#define MAC_RSNIE_CIPHER_WEP_104 0x000FAC05
#define MAC_RSNIE_CIPHER_BIP_CMAC_128 0x000FAC06
#define MAC_RSNIE_CIPHER_NO_GROUP 0x000FAC07
#define MAC_RSNIE_CIPHER_GCMP_128 0x000FAC08
#define MAC_RSNIE_CIPHER_GCMP_256 0x000FAC09
#define MAC_RSNIE_CIPHER_CCMP_256 0x000FAC0A
#define MAC_RSNIE_CIPHER_BIP_GMAC_128 0x000FAC0B
#define MAC_RSNIE_CIPHER_BIP_GMAC_256 0x000FAC0C
#define MAC_RSNIE_CIPHER_BIP_CMAC_256 0x000FAC0D
enum mac_cipher_suite mac_cipher_suite_value(uint32_t cipher_suite);

int bl_wifi_set_sta_key_internal(uint8_t vif_idx, uint8_t sta_idx,
                                 wpa_alg_t alg, int key_idx, int set_tx,
                                 uint8_t *seq, size_t seq_len, uint8_t *key,
                                 size_t key_len, bool pairwise) {
#ifdef DEBUG_FUNCTION
  printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__,
         __LINE__);
#endif
  uint8_t cipher_suite;

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
    printf("unsupport key_len = %d!!!\r\n", key_len);
    abort();
  }

  if (pairwise) {
    wl80211_mac_set_key(key_idx, sta_idx, key, key_len, cipher_suite, pairwise); // install PTK
  } else {
    wl80211_mac_set_key(key_idx, 255, key, key_len, cipher_suite, pairwise); // install GTK
  }

  return 0;
}

#define WPA_IGTK_LEN 16

int bl_wifi_set_igtk_internal(uint8_t vif_idx, uint8_t sta_idx,
                              uint16_t key_idx, const uint8_t *pn,
                              const uint8_t *key) {
  // igtk length is 16
  wl80211_mac_set_key(key_idx, 255, (uint8_t *)key, 16, MAC_CIPHER_BIP_CMAC_128, 0);
  return 0;
}

int bl_wifi_get_sta_gtk(uint8_t vif_idx, uint8_t *out_buf, uint8_t *out_len) {
#ifdef DEBUG_FUNCTION
  printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__,
         __LINE__);
#endif
  printf("Function '%s' has not been implemented!!!\r\n", __FUNCTION__);
  //while(1);
  return 0;
}

int bl_wifi_get_assoc_bssid_internal(uint8_t vif_idx, uint8_t *bssid) {
#ifdef DEBUG_FUNCTION
  printf(">>>>>>>>>>>> Call function: %s, line: %d\r\n", __FUNCTION__,
         __LINE__);
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

int bl_wifi_set_wps_cb_internal(const struct wps_funcs *wps_cb) {
  // wps_cbs = wps_cb;
  return 0;
}

wps_status_t bl_wifi_get_wps_status_internal(void) {
#if 0
    return sm_env.wps_status;
#else
  return WPS_STATUS_DISABLE;
#endif
}

void bl_wifi_set_wps_status_internal(wps_status_t status) {
#if 0
    sm_env.wps_status = status;
#endif
}

char *bl_wifi_cipher_to_str(uint8_t cipher)
{
    switch (cipher) {
        case WIFI_CIPHER_TYPE_NONE:
        {
            return "NONE";
        }
        break;
        case WIFI_CIPHER_TYPE_WEP40:
        {
            return "WEP40";
        }
        break;
        case WIFI_CIPHER_TYPE_WEP104:
        {
            return "WEP104";
        }
        break;
        case WIFI_CIPHER_TYPE_TKIP:
        {
            return "TKIP";
        }
        break;
        case WIFI_CIPHER_TYPE_CCMP:
        {
            return "CCMP";
        }
        break;
        case WIFI_CIPHER_TYPE_TKIP_CCMP:
        {
            return "TKIP_CCMP";
        }
        break;
        case WIFI_CIPHER_TYPE_AES_CMAC128:
        {
            return "AES_CMAC128";
        }
        break;
        case WIFI_CIPHER_TYPE_SMS4:
        {
            return "SMS4";
        }
        break;
        case WIFI_CIPHER_TYPE_UNKNOWN:
        {
            return "UNKNOWN";
        }
        break;
        default:
        {
            return "UNKNOWN";
        }
    }
}
#endif
