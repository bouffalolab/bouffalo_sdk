#ifndef __WIFI_MGMR_PROFILE_H__
#define __WIFI_MGMR_PROFILE_H__
#include "wifi_mgmr.h"

int wifi_mgmr_profile_add(wifi_mgmr_t *mgmr, wifi_mgmr_profile_msg_t *profile_msg, int index, uint8_t isActive);
int wifi_mgmr_profile_add_by_idx(wifi_mgmr_t *mgmr, wifi_mgmr_profile_msg_t *profile_msg, int index, uint8_t isActive);
int wifi_mgmr_profile_del(wifi_mgmr_t *mgmr, char *ssid, int len);
int wifi_mgmr_profile_del_by_idx(wifi_mgmr_t *mgmr, uint8_t index);
int wifi_mgmr_profile_set_active_by_idx(wifi_mgmr_t *mgmr, uint8_t index, uint8_t isActive);
int wifi_mgmr_profile_get(wifi_mgmr_t *mgmr, wifi_mgmr_profile_msg_t *profile_msg, uint8_t isActive);
int wifi_mgmr_profile_get_by_idx(wifi_mgmr_t *mgmr, wifi_mgmr_profile_msg_t *profile_msg, uint8_t index);
int wifi_mgmr_profile_autoreconnect_is_enabled(wifi_mgmr_t *mgmr, int index);
int wifi_mgmr_profile_autoreconnect_disable(wifi_mgmr_t *mgmr, int index);
int wifi_mgmr_profile_autoreconnect_enable(wifi_mgmr_t *mgmr, int index);
#endif
