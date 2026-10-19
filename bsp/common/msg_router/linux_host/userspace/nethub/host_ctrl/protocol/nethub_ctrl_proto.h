#ifndef NETHUB_CTRL_PROTO_H
#define NETHUB_CTRL_PROTO_H

#include "nethub_wifi_types.h"

typedef struct {
    const char *name;
    int (*init)(void);
    void (*deinit)(void);

    int (*get_status)(nethub_wifi_status_t *status);
    int (*get_version)(char *buf, size_t len);
    int (*reboot)(void);

    int (*sta_connect)(const nethub_wifi_sta_config_t *cfg, int timeout_ms);
    int (*sta_disconnect)(void);
    int (*scan)(nethub_wifi_ap_info_t *aps, int max_count, int *actual_count, int timeout_ms);

    int (*ap_start)(const nethub_wifi_ap_config_t *cfg, int timeout_ms);
    int (*ap_stop)(void);

    int (*ota_update)(const char *fw_path, int timeout_ms);
} nethub_ctrl_proto_ops_t;

const nethub_ctrl_proto_ops_t *nethub_ctrl_proto_at(void);
const nethub_ctrl_proto_ops_t *nethub_ctrl_proto_private(void);

#endif /* NETHUB_CTRL_PROTO_H */
