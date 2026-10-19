#ifndef NETHUB_WIFI_H
#define NETHUB_WIFI_H

#include "nethub_wifi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

const char *nethub_wifi_strerror(int err);
const char *nethub_wifi_sta_state_name(nethub_wifi_sta_state_t state);
const char *nethub_wifi_link_state_name(nethub_wifi_link_state_t state);

int nethub_wifi_client_open(nethub_wifi_client_t **client, const char *socket_path);
void nethub_wifi_client_close(nethub_wifi_client_t *client);

int nethub_wifi_get_status(nethub_wifi_client_t *client, nethub_wifi_status_t *status);
int nethub_wifi_get_version(nethub_wifi_client_t *client, char *buf, size_t len);
int nethub_wifi_reboot(nethub_wifi_client_t *client);

int nethub_wifi_sta_connect(nethub_wifi_client_t *client,
                            const nethub_wifi_sta_config_t *cfg,
                            int timeout_ms);
int nethub_wifi_sta_disconnect(nethub_wifi_client_t *client);

int nethub_wifi_scan(nethub_wifi_client_t *client,
                     nethub_wifi_ap_info_t *aps,
                     int max_count,
                     int *actual_count,
                     int timeout_ms);

int nethub_wifi_ap_start(nethub_wifi_client_t *client,
                         const nethub_wifi_ap_config_t *cfg,
                         int timeout_ms);
int nethub_wifi_ap_stop(nethub_wifi_client_t *client);

int nethub_wifi_ota_update(nethub_wifi_client_t *client,
                           const char *fw_path,
                           int timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* NETHUB_WIFI_H */
