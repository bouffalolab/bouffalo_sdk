/**
 * @file bflbwifid_log.h
 * @brief Shared logging helpers for bflbwifid modules
 */

#ifndef BFLBWIFID_LOG_H
#define BFLBWIFID_LOG_H

#include <stddef.h>

int bflbwifid_log_init(const char *log_file);
void bflbwifid_log_deinit(void);
void bflbwifid_log_write(const char *level, const char *fmt, ...);

#define BFLBWIFID_LOGI(fmt, ...) bflbwifid_log_write("INFO", fmt, ##__VA_ARGS__)
#define BFLBWIFID_LOGW(fmt, ...) bflbwifid_log_write("WARN", fmt, ##__VA_ARGS__)
#define BFLBWIFID_LOGE(fmt, ...) bflbwifid_log_write("ERROR", fmt, ##__VA_ARGS__)

#endif /* BFLBWIFID_LOG_H */
