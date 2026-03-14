#ifndef MIMICLAW_COMPAT_ESP_LOG_H
#define MIMICLAW_COMPAT_ESP_LOG_H

#include <stdio.h>

#define ESP_LOGI(tag, fmt, ...) \
    printf("[I][%s] " fmt "\r\n", (tag), ##__VA_ARGS__)
#define ESP_LOGW(tag, fmt, ...) \
    printf("[W][%s] " fmt "\r\n", (tag), ##__VA_ARGS__)
#define ESP_LOGE(tag, fmt, ...) \
    printf("[E][%s] " fmt "\r\n", (tag), ##__VA_ARGS__)

#endif
