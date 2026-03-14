/**
 * @file bflbwifid_ota.h
 * @brief Daemon-local OTA watchdog helpers
 */

#ifndef BFLBWIFID_OTA_H
#define BFLBWIFID_OTA_H

#include <stdbool.h>
#include <time.h>

bool bflbwifid_ota_in_progress(void);
time_t bflbwifid_ota_get_start_time(void);
void bflbwifid_ota_reset(void);

#endif /* BFLBWIFID_OTA_H */
