/**
 * @file bflbwifid_ota.c
 * @brief Daemon-local OTA watchdog helpers
 */

#include "bflbwifid_ota.h"

#include "../src/bflbwifi_internal.h"

bool bflbwifid_ota_in_progress(void)
{
    return bflbwifi_is_ota_in_progress();
}

time_t bflbwifid_ota_get_start_time(void)
{
    return bflbwifi_ota_get_start_time();
}

void bflbwifid_ota_reset(void)
{
    bflbwifi_ota_reset();
}
