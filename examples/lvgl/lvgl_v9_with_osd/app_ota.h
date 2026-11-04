#ifndef LVGL_OSD_APP_OTA_H
#define LVGL_OSD_APP_OTA_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    APP_OTA_IDLE = 0,
    APP_OTA_CONNECTING,
    APP_OTA_DOWNLOADING,
    APP_OTA_VERIFYING,
    APP_OTA_SUCCESS,
    APP_OTA_FAILED,
    APP_OTA_REBOOTING,
} app_ota_phase_t;

typedef struct {
    app_ota_phase_t phase;
    uint32_t received;
    uint32_t total;
    int error;
    bool busy;
    bool reboot_pending;
} app_ota_status_t;

/* Shared by the OTA worker and shell; the UI reads progress. */
extern volatile app_ota_status_t g_ota_status;
const char *app_ota_phase_name(app_ota_phase_t phase);

#endif
