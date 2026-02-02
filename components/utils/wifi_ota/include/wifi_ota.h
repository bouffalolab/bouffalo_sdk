#ifndef __WIFI_OTA_H__
#define __WIFI_OTA_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize WiFi OTA components */
int wifi_ota_init(void);

/* Deinitialize WiFi OTA component */
void wifi_ota_deinit(void);

/* Start OTA update */
int wifi_ota_start_update(const char *server_ip, const char *server_port);

#ifdef __cplusplus
}
#endif

#endif /* __WIFI_OTA_H__ */
