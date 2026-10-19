#ifndef NETHUB_LOG_H
#define NETHUB_LOG_H

#include <stdio.h>

#define NETHUB_LOGE(fmt, ...) nethub_log_write("E", __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define NETHUB_LOGW(fmt, ...) nethub_log_write("W", __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define NETHUB_LOGI(fmt, ...) nethub_log_write("I", __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define NETHUB_LOGD(fmt, ...) nethub_log_debug(fmt, ##__VA_ARGS__)

int nethub_log_init(const char *path);
void nethub_log_deinit(void);
void nethub_log_write(const char *level, const char *file, int line, const char *fmt, ...);
void nethub_log_debug(const char *fmt, ...);

#endif /* NETHUB_LOG_H */
