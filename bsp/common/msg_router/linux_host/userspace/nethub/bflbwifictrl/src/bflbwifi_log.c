/**
 * @file bflbwifi_log.c
 * @brief BFLB WiFi module log implementation
 */

#include "bflbwifi_log.h"
#include <stdio.h>

/* Global log level (default INFO) */
log_level_t g_bflbwifi_log_level = LOG_INFO;

void bflbwifi_log_set_level(log_level_t level)
{
    g_bflbwifi_log_level = level;
}

log_level_t bflbwifi_log_get_level(void)
{
    return g_bflbwifi_log_level;
}
