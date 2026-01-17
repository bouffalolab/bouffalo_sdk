/**
 * @file bflbwifi_log.h
 * @brief BFLB WiFi module log interface
 */

#ifndef BFLBWIFI_LOG_H
#define BFLBWIFI_LOG_H

#include <stdio.h>

/**
 * @brief Log level
 */
typedef enum {
    LOG_ERROR = 0,    /* Error: Issues that must be handled */
    LOG_WARN,         /* Warning: Issues that need attention but do not affect operation */
    LOG_INFO,         /* Info: Normal status changes */
    LOG_DEBUG,        /* Debug: Detailed debug information */
} log_level_t;

/**
 * @brief Set log level
 * @param level Log level, only logs >= this level will be output
 * @note Default level: LOG_INFO
 */
void bflbwifi_log_set_level(log_level_t level);

/**
 * @brief Get current log level
 * @return Current log level
 */
log_level_t bflbwifi_log_get_level(void);

/* Global log level (defined in bflbwifi_log.c) */
extern log_level_t g_bflbwifi_log_level;

/* ========== Log Macros (recommended to use) ========== */

#define BFLBWIFI_LOGE(fmt, ...) \
    do { \
        if (LOG_ERROR <= g_bflbwifi_log_level) { \
            fprintf(stderr, "[E] %s:%d " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
        } \
    } while(0)

#define BFLBWIFI_LOGW(fmt, ...) \
    do { \
        if (LOG_WARN <= g_bflbwifi_log_level) { \
            fprintf(stderr, "[W] %s:%d " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
        } \
    } while(0)

#define BFLBWIFI_LOGI(fmt, ...) \
    do { \
        if (LOG_INFO <= g_bflbwifi_log_level) { \
            fprintf(stderr, "[I] " fmt "\n", ##__VA_ARGS__); \
        } \
    } while(0)

#define BFLBWIFI_LOGD(fmt, ...) \
    do { \
        if (LOG_DEBUG <= g_bflbwifi_log_level) { \
            fprintf(stderr, "[D] %s:%d " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
        } \
    } while(0)

/* Backward compatible macro aliases */
#define BFLB_LOGE BFLBWIFI_LOGE
#define BFLB_LOGW BFLBWIFI_LOGW
#define BFLB_LOGI BFLBWIFI_LOGI
#define BFLB_LOGD BFLBWIFI_LOGD
#define bflb_log_set_level bflbwifi_log_set_level
#define bflb_log_get_level bflbwifi_log_get_level

#endif /* BFLBWIFI_LOG_H */
