/**
 * @file bflbwifi_internal.h
 * @brief Internal libbflbwifi declarations shared by daemon and library sources
 */

#ifndef BFLBWIFI_INTERNAL_H
#define BFLBWIFI_INTERNAL_H

#include <time.h>

#include "../include/bflbwifi.h"

typedef struct {
    char mac[18];
    char ip[16];
} bflbwifi_sta_info_t;

typedef enum {
    WIFI_MODE_IDLE = 0,
    WIFI_MODE_STA,
    WIFI_MODE_AP,
    WIFI_MODE_STA_AP,
} bflbwifi_mode_t;

int bflbwifi_set_mode(bflbwifi_mode_t mode);
int bflbwifi_get_mode(bflbwifi_mode_t *mode);
bool bflbwifi_is_ota_in_progress(void);
time_t bflbwifi_ota_get_start_time(void);
void bflbwifi_ota_reset(void);
int bflbwifi_set_station_dhcpc(bool enable);
int bflbwifi_set_static_ip(const char *ip, const char *gateway, const char *netmask);
int bflbwifi_ap_config(const bflbwifi_ap_config_t *config);
int bflbwifi_ap_set_dhcp_range(const char *start_ip, const char *end_ip, int lease_min);
int bflbwifi_ap_get_sta_list(bflbwifi_sta_info_t *sta_list, int max_count, int *actual_count);
int bflbwifi_ap_disconnect_sta(const char *mac);

#endif /* BFLBWIFI_INTERNAL_H */
