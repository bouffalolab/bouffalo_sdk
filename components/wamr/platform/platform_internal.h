#ifndef WAMR_BOUFFALO_PLATFORM_INTERNAL_H
#define WAMR_BOUFFALO_PLATFORM_INTERNAL_H

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

typedef TaskHandle_t korp_tid;
typedef SemaphoreHandle_t korp_mutex;
typedef void *korp_cond;
typedef void *korp_rwlock;
typedef unsigned int korp_sem;
typedef int os_file_handle;
typedef void *os_dir_stream;
typedef int os_raw_file_handle;

static inline int os_getpagesize(void)
{
    return 4096;
}

static inline os_file_handle os_get_invalid_handle(void)
{
    return -1;
}

#endif
