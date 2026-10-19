#ifndef __BLCTL_LOG_H
#define __BLCTL_LOG_H

#include <stdio.h>
#include <stdarg.h>

enum {
    BL_LOG_LEVEL_FATAL = 0,
    BL_LOG_LEVEL_ERR,
    BL_LOG_LEVEL_WARN,
    BL_LOG_LEVEL_INFO,
    BL_LOG_LEVEL_DEBUG,
    BL_LOG_LEVEL_VERBOSE,
};

/* this can be overridden by other source files */
#define BL_TAG  " blctl "

#define bl_logf(fmt, ...) bl_log_print(BL_LOG_LEVEL_FATAL,   " F " BL_TAG fmt, ##__VA_ARGS__)
#define bl_loge(fmt, ...) bl_log_print(BL_LOG_LEVEL_ERR,     " E " BL_TAG fmt, ##__VA_ARGS__)
#define bl_logw(fmt, ...) bl_log_print(BL_LOG_LEVEL_WARN,    " W " BL_TAG fmt, ##__VA_ARGS__)
#define bl_logi(fmt, ...) bl_log_print(BL_LOG_LEVEL_INFO,    " I " BL_TAG fmt, ##__VA_ARGS__)
#define bl_logd(fmt, ...) bl_log_print(BL_LOG_LEVEL_DEBUG,   " D " BL_TAG fmt, ##__VA_ARGS__)
#define bl_logv(fmt, ...) bl_log_print(BL_LOG_LEVEL_VERBOSE, " V " BL_TAG fmt, ##__VA_ARGS__)


int bl_log_level_get(void);

int bl_log_level_set(int level);

void bl_log_print(int level, const char *fmt, ...);

/* __BLCTL_LOG_H */
#endif
