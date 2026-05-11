#ifndef NETHUB_WIFI_SERVICE_H
#define NETHUB_WIFI_SERVICE_H

#include "nethub_wifi_types.h"

int nethub_wifi_service_init(void);
void nethub_wifi_service_deinit(void);

int nethub_wifi_service_get_status(nethub_wifi_status_t *status);
int nethub_wifi_service_get_version(char *buf, size_t len);
int nethub_wifi_service_reboot(void);
int nethub_wifi_service_sta_connect(const nethub_wifi_sta_config_t *cfg, int timeout_ms);
int nethub_wifi_service_sta_disconnect(void);
int nethub_wifi_service_scan(nethub_wifi_ap_info_t *aps, int max_count, int *actual_count, int timeout_ms);
int nethub_wifi_service_ap_start(const nethub_wifi_ap_config_t *cfg, int timeout_ms);
int nethub_wifi_service_ap_stop(void);
int nethub_wifi_service_ota_update(const char *fw_path, int timeout_ms);

#endif /* NETHUB_WIFI_SERVICE_H */
