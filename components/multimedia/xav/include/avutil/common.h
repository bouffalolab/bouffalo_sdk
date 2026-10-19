/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
//#include <sys/stat.h>
//#include <sys/types.h>
//#include <stdarg.h>
//#include <stddef.h>
//#include <math.h>
#include <sys/time.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <sys/socket.h>
//#include <fcntl.h>

#ifdef __linux__
#include <errno.h>
#include <pthread.h>
#include "ulog/ulog.h"
#else
#include <xutils/types.h>
#include <xutils/debug.h>
#include <xutils/list.h>
#include <msp/kernel.h>
#endif

#include "avutil/av_errno.h"
#include "avutil/av_port.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int handle_t;

#ifndef INT_MAX
#define INT_MAX     (2147483647)
#endif

#ifndef UINT_MAX
#define UINT_MAX (INT_MAX * 2U + 1U)
#endif

#ifndef UNUSED
//#define UNUSED __attribute__((unused))
#define UNUSED(x)
#endif

#ifndef container_of
#define container_of(ptr, type, member) \
        ((type *)((char *)(ptr) - offsetof(type,member)))
#endif /* container_of */

#ifndef MIN
#define MIN(x,y)                ((x) < (y) ? (x) : (y))
#endif

#ifndef MAX
#define MAX(x,y)                ((x) > (y) ? (x) : (y))
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)           (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif

#ifndef SEEK_END
#define SEEK_END 2
#endif

#ifndef AV_EPSINON
#define AV_EPSINON  (0.000001)
#endif

#ifndef AV_ALIGN_SIZE
#define AV_ALIGN_SIZE(size, align) (((size) + align - 1U) & (~(align - 1U)))
#endif

#ifndef AV_ALIGN_BUFZ
#define AV_ALIGN_BUFZ(size, align) (((size) + align - 1U))
#endif

#ifndef AV_ALIGN
#define AV_ALIGN(addr, align) ((void *)(((unsigned long)(addr) + align - 1U) & (~(unsigned long)(align - 1U))))
#endif

#ifdef __linux__
#define av_malloc          malloc
#define av_realloc         realloc
#define av_calloc          calloc
#define av_zalloc(size)    calloc(1, size)
#define av_free            free
#define av_freep           freep
#else
#define av_malloc      msp_zalloc
#define av_realloc     msp_realloc
#define av_calloc      msp_calloc
#define av_zalloc      msp_zalloc
#define av_free        msp_free
#define av_freep       msp_freep
#endif

#ifdef __cplusplus
}
#endif

#endif /* __COMMON_H__ */

