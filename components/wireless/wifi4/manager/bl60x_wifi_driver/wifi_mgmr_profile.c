/**
 ****************************************************************************************
 *
 * @file wifi_mgmr_profile.c
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

#include <string.h>

#include "wifi_mgmr.h"
#include "wifi_mgmr_profile.h"

int wifi_mgmr_profile_add(wifi_mgmr_t *mgmr, wifi_mgmr_profile_msg_t *profile_msg, int index, uint8_t isActive)
{
    int i;
    wifi_mgmr_profile_t *profile;

    profile = NULL;

    if (-1 == index) {
        /*use default index for profile*/
        profile = &(mgmr->profiles[0]);
    } else {
        /*scan free index for profile*/
        for (i = 0; i < sizeof(mgmr->profiles)/sizeof(mgmr->profiles[0]); i++) {
            if (0 == mgmr->profiles[i].isUsed) {
                profile = &(mgmr->profiles[i]);
                if (isActive) {                 
                    mgmr->profile_active_index = i;
                    bflb_os_printf("[WF][PF] Adding and Using profile, idx is @%d\r\n", i);
                } else {
                    bflb_os_printf("[WF][PF] Only Adding profile, idx is @%d\r\n", i);
                }
                break;
            }
        }
    }
    if (NULL == profile) {
        return -1;
    }
    memset(profile, 0, sizeof(*profile));
    profile->isUsed = 1;//Set isUsed firstly
    profile->isActive = isActive;
    profile->ssid_len = profile_msg->ssid_len;
    profile->passphr_len = profile_msg->passphr_len;
    profile->psk_len = profile_msg->psk_len;
    profile->band = profile_msg->band;
    profile->freq = profile_msg->freq;
    profile->ap_info_ttl = profile_msg->ap_info_ttl;
    profile->priority = 255;
    memcpy(profile->ssid, profile_msg->ssid, sizeof(profile->ssid));
    memcpy(profile->psk, profile_msg->psk, sizeof(profile->psk));
    memcpy(profile->passphr, profile_msg->passphr, sizeof(profile->passphr));
    memcpy(profile->bssid, profile_msg->bssid, sizeof(profile->bssid));
    profile->dhcp_use = profile_msg->dhcp_use;
    profile->flags = profile_msg->flags;

    return 0;
}

int wifi_mgmr_profile_add_by_idx(wifi_mgmr_t *mgmr, wifi_mgmr_profile_msg_t *profile_msg, int index, uint8_t isActive)
{
    wifi_mgmr_profile_t *profile;

    profile = NULL;

    if (-1 == index) {
        /*use default index for profile*/
        profile = &(mgmr->profiles[0]);
    } else if (index < WIFI_MGMR_PROFILES_MAX) {
        profile = &(mgmr->profiles[index]);
        if (isActive) {                 
            mgmr->profile_active_index = index;
            bflb_os_printf("[WF][PF] Adding and Using profile, idx is @%d\r\n", index);
        } else {
            bflb_os_printf("[WF][PF] Only Adding profile, idx is @%d\r\n", index);
        }
    }
    if (NULL == profile) {
        return -1;
    }
    memset(profile, 0, sizeof(*profile));
    profile->isUsed = 1;//Set isUsed firstly
    profile->isActive = isActive;
    profile->ssid_len = profile_msg->ssid_len;
    profile->passphr_len = profile_msg->passphr_len;
    profile->psk_len = profile_msg->psk_len;
    profile->band = profile_msg->band;
    profile->freq = profile_msg->freq;
    profile->ap_info_ttl = profile_msg->ap_info_ttl;
    profile->priority = 255;
    memcpy(profile->ssid, profile_msg->ssid, sizeof(profile->ssid));
    memcpy(profile->psk, profile_msg->psk, sizeof(profile->psk));
    memcpy(profile->passphr, profile_msg->passphr, sizeof(profile->passphr));
    memcpy(profile->bssid, profile_msg->bssid, sizeof(profile->bssid));
    profile->dhcp_use = profile_msg->dhcp_use;
    profile->flags = profile_msg->flags;

    return 0;
}

int wifi_mgmr_profile_del(wifi_mgmr_t *mgmr, char *ssid, int len)
{
    int i;
    wifi_mgmr_profile_t *profile;

    profile = NULL;
    for (i = 0; i < sizeof(mgmr->profiles)/sizeof(mgmr->profiles[0]); i++) {
        if (1 == mgmr->profiles[i].isUsed &&
                len == mgmr->profiles[i].ssid_len &&
                0 == memcmp(mgmr->profiles[i].ssid, ssid, len)) {
            profile = &(mgmr->profiles[i]);
            if (i == mgmr->profile_active_index) {
                mgmr->profile_active_index = -1;
            }
            bflb_os_printf("[WF][PF] Free profile, idx is @%d\r\n", i);
            break;
        }
    }
    if (NULL == profile) {
        return -1;
    }
    memset(profile, 0, sizeof(*profile));

    return 0;
}

int wifi_mgmr_profile_set_active_by_idx(wifi_mgmr_t *mgmr, uint8_t index, uint8_t isActive)
{
    wifi_mgmr_profile_t *profile = NULL;

    if (index < WIFI_MGMR_PROFILES_MAX && 1 == mgmr->profiles[index].isUsed) {
        profile = &(mgmr->profiles[index]);
        profile->isActive = isActive;
        mgmr->profile_active_index = index;
        bflb_os_printf("[WF][PF] Set profile isActive = %u, idx is @%d\r\n", isActive, index);
    } else {
        return -1;
    }

    return 0;
}


int wifi_mgmr_profile_del_by_idx(wifi_mgmr_t *mgmr, uint8_t index)
{
    wifi_mgmr_profile_t *profile;

    profile = NULL;
    if (index < WIFI_MGMR_PROFILES_MAX && 1 == mgmr->profiles[index].isUsed) {
        profile = &(mgmr->profiles[index]);
        if (index == mgmr->profile_active_index) {
            mgmr->profile_active_index = -1;
        }
        bflb_os_printf("[WF][PF] Free profile by index, idx is @%d\r\n", index);
    }
    if (NULL == profile) {
        return -1;
    }
    memset(profile, 0, sizeof(*profile));

    return 0;
}

int wifi_mgmr_profile_get_by_idx(wifi_mgmr_t *mgmr, wifi_mgmr_profile_msg_t *profile_msg, uint8_t index)
{
    wifi_mgmr_profile_t *profile;

    profile = NULL;
    if (index < WIFI_MGMR_PROFILES_MAX && 1 == mgmr->profiles[index].isUsed) {
        profile = &(mgmr->profiles[index]);
        bflb_os_printf("[WF][PF] Getting profile by index, idx is @%d\r\n", index);
    }
    if (NULL == profile) {
        bflb_os_printf("[WF][PF] Getting profile by index, ret -1\r\n");
        return -1;
    }

    memset(profile_msg, 0, sizeof(*profile_msg));
    profile_msg->ssid_len = profile->ssid_len;
    profile_msg->psk_len = profile->psk_len;
    profile_msg->passphr_len = profile->passphr_len;
    profile_msg->band = profile->band;
    profile_msg->freq = profile->freq;
    profile_msg->ap_info_ttl = profile->ap_info_ttl;
    profile_msg->dhcp_use = profile->dhcp_use;
    profile_msg->flags = profile->flags;
    memcpy(profile_msg->ssid, profile->ssid, sizeof(profile->ssid));
    memcpy(profile_msg->psk, profile->psk, sizeof(profile->psk));
    memcpy(profile_msg->passphr, profile->passphr, sizeof(profile->passphr));
    memcpy(profile_msg->bssid, profile->bssid, sizeof(profile->bssid));

    return 0;
}

int wifi_mgmr_profile_get(wifi_mgmr_t *mgmr, wifi_mgmr_profile_msg_t *profile_msg, uint8_t isActive)
{
    int i;
    wifi_mgmr_profile_t *profile;

    profile = NULL;
    for (i = 0; i < sizeof(mgmr->profiles)/sizeof(mgmr->profiles[0]); i++) {
        if (1 == mgmr->profiles[i].isUsed && isActive == mgmr->profiles[i].isActive) {
            profile = &(mgmr->profiles[i]);
            bflb_os_printf("[WF][PF] Getting profile, idx is @%d\r\n", i);
            break;
        }
    }
    if (NULL == profile) {
        bflb_os_printf("[WF][PF] Getting  ret is -1\r\n");
        return -1;
    }

    memset(profile_msg, 0, sizeof(*profile_msg));
    profile_msg->ssid_len = profile->ssid_len;
    profile_msg->psk_len = profile->psk_len;
    profile_msg->passphr_len = profile->passphr_len;
    profile_msg->band = profile->band;
    profile_msg->freq = profile->freq;
    profile_msg->ap_info_ttl = profile->ap_info_ttl;
    profile_msg->dhcp_use = profile->dhcp_use;
    profile_msg->flags = profile->flags;
    memcpy(profile_msg->ssid, profile->ssid, sizeof(profile->ssid));
    memcpy(profile_msg->psk, profile->psk, sizeof(profile->psk));
    memcpy(profile_msg->passphr, profile->passphr, sizeof(profile->passphr));
    memcpy(profile_msg->bssid, profile->bssid, sizeof(profile->bssid));

    return i;
}

wifi_mgmr_profile_t* __lookup_profile(wifi_mgmr_t *mgmr, int index)
{
    wifi_mgmr_profile_t *profile = NULL;

    if (-1 == index) {
        /*default profile*/
        profile = &(mgmr->profiles[0]);
    } else {
        if (index >= 0 && index < sizeof(mgmr->profiles)/sizeof(mgmr->profiles[0])) {
            profile = &(mgmr->profiles[index]);
        }
    }
    return profile;
}

int wifi_mgmr_profile_autoreconnect_is_enabled(wifi_mgmr_t *mgmr, int index)
{
#if 0
    wifi_mgmr_profile_t *profile;
    
    profile = __lookup_profile(mgmr, index);
    if (NULL == profile) {
        return -1;
    }

    return profile->no_autoconnect ? 0 : 1;
#else
    return mgmr->disable_autoreconnect ? 0 : 1;
#endif
}

int wifi_mgmr_profile_autoreconnect_disable(wifi_mgmr_t *mgmr, int index)
{
#if 0
    wifi_mgmr_profile_t *profile;

    profile = __lookup_profile(mgmr, index);
    if (NULL == profile) {
        return -1;
    }
    profile->no_autoconnect = 1;
    return 0;
#else
    mgmr->disable_autoreconnect = 1;

    return 0;
#endif
}

int wifi_mgmr_profile_autoreconnect_enable(wifi_mgmr_t *mgmr, int index)
{
#if 0
    wifi_mgmr_profile_t *profile;

    profile = __lookup_profile(mgmr, index);
    if (NULL == profile) {
        return -1;
    }
    profile->no_autoconnect = 0;

    return 0;
#else
    mgmr->disable_autoreconnect = 0;

    return 0;
#endif
}
