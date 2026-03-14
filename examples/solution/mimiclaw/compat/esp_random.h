#ifndef MIMICLAW_COMPAT_ESP_RANDOM_H
#define MIMICLAW_COMPAT_ESP_RANDOM_H

#include <stdint.h>

#include "bflb_mtimer.h"

static inline uint32_t esp_random(void)
{
    uint64_t now = bflb_mtimer_get_time_us();
    return (uint32_t)(now ^ (now >> 32) ^ 0x9E3779B9u);
}

#endif
