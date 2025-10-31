#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "wl80211.h"
#include "macsw/wl80211_mac.h"
#include "wifi_mgmr.h"

void wifi_mgmr_sta_ps_enter(void) {
  if (wl80211_cntrl(WL80211_CTRL_STA_SET_PS_MODE, 1)){
    printf("enable ps mode failed!\r\n");
    abort();
  }
}

void wifi_mgmr_sta_ps_exit(void) {
  if (wl80211_cntrl(WL80211_CTRL_STA_SET_PS_MODE, 0)){
    printf("exit ps mode failed!\r\n");
    abort();
  }
}

int wifi_mgmr_sta_get_bssid(uint8_t bssid[6]) {
  if (wl80211_glb.link_up == 0) {
    return -1;
  }

  assert(bssid != NULL);
  memcpy(bssid, wl80211_glb.bssid, 6);
  return 0;
}

int wifi_mgmr_sta_mac_get(uint8_t mac[6]){
  struct mac_addr mac_addr;
  if (macsw_sta_vif == NULL) {
    return -1;
  }
  assert(mac != NULL);

  mac_vif_get_mac_addr(macsw_sta_vif, &mac_addr);
  memcpy(mac, mac_addr.array, 6);
  return 0;
}

int wifi_mgmr_sta_channel_get(uint8_t *channel) {
  if (wl80211_glb.link_up == 0) {
    return -1;
  }
  assert(channel != NULL);

  *channel = (uint8_t)phy_freq_to_channel(PHY_BAND_2G4, wl80211_glb.freq);
  return 0;
}

int wifi_mgmr_sta_aid_get(void) {
  if (wl80211_glb.link_up == 0) {
    return -1;
  }

  return wl80211_glb.aid;
}

int wifi_mgmr_sta_state_get(void) {
  return wl80211_glb.link_up;
}

char *wifi_mgmr_mode_to_str(uint32_t mode)
{
    wifimode_t allmode[] = {
        {"B",   WIFI_MODE_B},
        {"BG",  WIFI_MODE_BG},
        {"BGN", WIFI_MODE_BGN},
        {"BGAX", WIFI_MODE_BGAX},
        {"BGNAX", WIFI_MODE_BGNAX},
        {"A",   WIFI_MODE_A},
        {"AN",  WIFI_MODE_AN},
        // WiFi 5 (802.11ac)
        {"AAC", WIFI_MODE_AAC},
        {"ANAC", WIFI_MODE_ANAC},
        // WiFi 6 (802.11ax) 5G
        {"AAX", WIFI_MODE_AAX},
        {"ANAX", WIFI_MODE_ANAX},
        {"AACAX", WIFI_MODE_AACAX},
        {"ANACAX", WIFI_MODE_ANACAX},
    };
    int mode_count = sizeof(allmode) / sizeof(wifimode_t);

    for (int i = 0; i < mode_count; i++)
    {
        if (allmode[i].mode == mode) {
            return allmode[i].mode_str;
        }
    }

    return "Unknown";
}

char *wifi_mgmr_auth_to_str(uint8_t auth)
{
    switch (auth) {
        case WIFI_EVENT_BEACON_IND_AUTH_WPA3_SAE:
        {
            return "WPA3-SAE";
        }
        break;
        case WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK_WPA3_SAE:
        {
            return "WPA2-PSK/WPA3-SAE";
        }
        break;
        case WIFI_EVENT_BEACON_IND_AUTH_OPEN:
        {
            return "Open";
        }
        break;
        case WIFI_EVENT_BEACON_IND_AUTH_WEP:
        {
            return "WEP";
        }
        break;
        case WIFI_EVENT_BEACON_IND_AUTH_WPA_PSK:
        {
            return "WPA-PSK";
        }
        break;
        case WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK:
        {
            return "WPA2-PSK";
        }
        break;
        case WIFI_EVENT_BEACON_IND_AUTH_WPA_WPA2_PSK:
        {
            return "WPA2-PSK/WPA-PSK";
        }
        break;
        case WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT:
        {
            return "WPAx-Enterprise";
        }
        break;
        case WIFI_EVENT_BEACON_IND_AUTH_UNKNOWN:
        {
            return "Unknown";
        }
        break;
        default:
        {
            return "Unknown";
        }
    }
}

char *wifi_mgmr_cipher_to_str(uint8_t cipher)
{
    if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_NONE) {
        return "NONE";
    } else if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_WEP) {
        return "WEP";
    } else if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_AES) {
        return "AES";
    } else if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_TKIP) {
        return "TKIP";
    } else if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_TKIP_AES) {
        return "TKIP/AES";
    } else {
        return "Unknown";
    }
}

static const struct reason_code sm_reason_list[] = {
    {WLAN_FW_SUCCESSFUL, "sm connect ind ok"},
    {WLAN_FW_TX_AUTH_FRAME_ALLOCATE_FAIILURE, "tx auth frame alloc failure"},
    {WLAN_FW_AUTHENTICATION_FAIILURE, "Authentication failure"},
    {WLAN_FW_AUTH_ALGO_FAIILURE, "Auth response but auth algo failure"},
    {WLAN_FW_TX_ASSOC_FRAME_ALLOCATE_FAIILURE, "tx assoc frame alloc failure"},
    {WLAN_FW_ASSOCIATE_FAIILURE, "Association failure"},
    {WLAN_FW_DEAUTH_BY_AP_WHEN_NOT_CONNECTION, "deauth by AP when connecting"},
    {WLAN_FW_DEAUTH_BY_AP_WHEN_CONNECTION, "deauth by AP when connected"},
    {WLAN_FW_4WAY_HANDSHAKE_ERROR_PSK_TIMEOUT_FAILURE, "Passwd error, 4-way handshake timeout"},
    {WLAN_FW_4WAY_HANDSHAKE_TX_DEAUTH_FRAME_TRANSMIT_FAILURE, "Passwd error, tx deauth frame transmit failure"},
    {WLAN_FW_4WAY_HANDSHAKE_TX_DEAUTH_FRAME_ALLOCATE_FAIILURE, "Passwd error, tx deauth frame allocate failure"},
    {WLAN_FW_AUTH_OR_ASSOC_RESPONSE_TIMEOUT_FAILURE, "auth or associate frame response timeout failure"},
    {WLAN_FW_SCAN_NO_BSSID_AND_CHANNEL, "SSID error, scan no bssid and channel"},
    {WLAN_FW_CREATE_CHANNEL_CTX_FAILURE_WHEN_JOIN_NETWORK, "create channel context failure when join network"},
    {WLAN_FW_JOIN_NETWORK_FAILURE, "join network failure"},
    {WLAN_FW_ADD_STA_FAILURE, "add sta failure"},
    {WLAN_FW_BEACON_LOSS, "ap beacon loss"},
    {WLAN_FW_NETWORK_SECURITY_NOMATCH, "network security no match"},
    {WLAN_FW_NETWORK_WEPLEN_ERROR, "wep network psk len error"},
    {WLAN_FW_DISCONNECT_BY_USER_WITH_DEAUTH, "user disconnect and send deauth"},
    {WLAN_FW_DISCONNECT_BY_USER_NO_DEAUTH, "user disconnect but no send deauth"},
    {WLAN_FW_DISCONNECT_BY_FW_PS_TX_NULLFRAME_FAILURE, "fw disconnect(tx nullframe failures)"},
    {WLAN_FW_TRAFFIC_LOSS, "fw disconnect(traffic loss)"},
    {WLAN_FW_SWITCH_CHANNEL_FAILURE, "fw switch channel failure"},
    {WLAN_FW_AUTH_OR_ASSOC_RESPONSE_CFM_FAILURE, "auth or associate frame response cfm failure"},
    {WLAN_FW_CONNECT_PARAMS_ERROR, "connect params error"},
};

static const char* _get_status_code_str(const struct reason_code list[], uint16_t len, uint16_t status_code)
{
    int i;

    for (i = 0; i < len; i++) {
        if (list[i].reason_code == status_code) {
            break;
        }
    }
    if (len == i) {
        return "Unknown Code";
    }

    return list[i].action;
}

const char* wifi_mgmr_get_sm_status_code_str(uint16_t status_code)
{
    return _get_status_code_str(sm_reason_list, sizeof(sm_reason_list)/sizeof(sm_reason_list[0]), status_code);
}
