/**
 ****************************************************************************************
 *
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "msp/kernel.h"

#if 1
#define SPEED_MEASURE(t_name, l_name, TAG, size, interv_ms)                                          \
do {                                                                                                 \
    static uint32_t t_name##mea = 0;                                                                 \
    static uint32_t l_name##mea = 0;                                                                 \
    uint32_t now = msp_now_ms();                                                                     \
    t_name##mea += size;                                                                             \
    if (l_name##mea == 0) {                                                                          \
        l_name##mea = now;                                                                           \
    } else if (now - l_name##mea >= interv_ms) {                                                     \
        printf("%ld %s %.3f Kbits/sec\r\n",                                                          \
               now, TAG, (float)8*t_name##mea/1024/(((float)now - l_name##mea)/1000));               \
        l_name##mea = now;                                                                           \
        t_name##mea = 0;                                                                             \
    }                                                                                                \
} while(0)
#else
#define SPEED_MEASURE(t_name, l_name, TAG, size, interv_ms)
#endif
