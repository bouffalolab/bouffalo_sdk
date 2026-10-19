/*
 * wpa_supplicant/hostapd / Internal implementation of OS specific functions
 * Copyright (c) 2005-2006, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 *
 * This file is an example of operating system specific  wrapper functions.
 * This version implements many of the functions internally, so it can be used
 * to fill in missing functions from the target system C libraries.
 *
 * Some of the functions are using standard C library calls in order to keep
 * this file in working condition to allow the functions to be tested on a
 * Linux target. Please note that OS_NO_C_LIB_DEFINES needs to be defined for
 * this file to work correctly. Note that these implementations are only
 * examples and are not optimized for speed.
 */

#include "os.h"
#include <stdlib.h>
/* #include <time.h> */
/* #include <unistd.h> */
/* #include <sys/time.h> */
/* #include "utils/common.h" */
#include <FreeRTOS.h>
#include <task.h>
#include "bflb_sec_trng.h"

int os_get_time(struct os_time *t)
{
#if 0
    struct timeval tv;
    int ret = gettimeofday(&tv, NULL);
    t->sec = (os_time_t) tv.tv_sec;
    t->usec = tv.tv_usec;
    return ret;
#endif
    return -1;
}

unsigned long os_random(void)
{
    return (unsigned long)random();
}

int os_get_random(unsigned char *buf, size_t len)
{
    bflb_trng_readlen(buf, len);
    return 0;
}

void os_sleep(os_time_t sec, os_time_t usec)
{
    size_t time_ms = sec * 1000 + usec / 1000;
    time_ms = time_ms ? time_ms : 1;
    vTaskDelay(pdMS_TO_TICKS(time_ms));
}

 typedef struct memoryStats {
        size_t totalAllocs;     /* number of allocations */
        size_t totalBytes;      /* total number of bytes allocated */
        size_t peakBytes;       /* concurrent max bytes */
        size_t currentBytes;    /* total current bytes in use */
    } memoryStats;

static memoryStats ourMemStats;

typedef struct memHint {
    size_t thisSize;      /* size of this memory */
    void*  thisMemory;    /* actual memory for user */
} memHint;

void *wpa_supplicant_malloc(size_t size)
{
#if 0
    return pvPortMalloc(size);
#else
	memHint* mt;
	if (size == 0)
		return NULL;

	mt = (memHint*)pvPortMalloc(sizeof(memHint) + size);
	if (mt == NULL)
	{
		printf("failed to allocate mem[%d], have malloced [%d]\r\n", size, ourMemStats.currentBytes);
		return NULL;
	}
	mt->thisSize   = size;
	mt->thisMemory = (char *)mt + sizeof(memHint);

	ourMemStats.totalAllocs++;
	ourMemStats.totalBytes   += size;
	ourMemStats.currentBytes += size;
	if (ourMemStats.currentBytes > ourMemStats.peakBytes)
		ourMemStats.peakBytes = ourMemStats.currentBytes;

	return mt->thisMemory;
#endif
}

void *wpa_supplicant_realloc(void *ptr, size_t size)
{
#if 0
    return pvPortRealloc(ptr, size);
#else
	 void* ret = wpa_supplicant_malloc(size);
	 memHint* mt;

        if (ptr) {
            /* if realloc is bigger, don't overread old ptr */
            mt = (memHint*)((char*)ptr - sizeof(memHint));

            if (mt->thisSize < size)
                size = mt->thisSize;
        }

        if (ret && ptr)
            memcpy(ret, ptr, size);

	if (ret)
		wpa_supplicant_free(ptr);

	return ret;
#endif
}

void *wpa_supplicant_zalloc(size_t nmemb, size_t size)
{
#if 0
    return pvPortCalloc(nmemb, size);
#else
	void *ptr = wpa_supplicant_malloc(nmemb * size);
	if (ptr)
		memset(ptr, 0, nmemb * size);

	return ptr;
#endif
}

void wpa_supplicant_free(void *ptr)
{
  if (ptr == NULL) {
    return;
  }
#if 0
    vPortFree(ptr);
#else
	memHint* mt;
	mt = (memHint*)((char *)ptr - sizeof(memHint));
	ourMemStats.currentBytes -= mt->thisSize;

	vPortFree(mt);
#endif
}

void wpa_supplicant_bzero(void *s, size_t n)
{
    memset(s, 0, n);
}

void InitMemoryTracker(void)
{
	ourMemStats.totalAllocs  = 0;
	ourMemStats.totalBytes   = 0;
	ourMemStats.peakBytes    = 0;
	ourMemStats.currentBytes = 0;
}

void ShowMemoryTracker(void)
{
	printf("total     	Allocs 	= %9lu\r\n",(unsigned long)ourMemStats.totalAllocs);
	printf("total     	Bytes  	= %9lu\r\n",(unsigned long)ourMemStats.totalBytes);
	printf("peak    	Bytes  	= %9lu\r\n",(unsigned long)ourMemStats.peakBytes);
	printf("current 	ytes  	= %9lu\r\n",(unsigned long)ourMemStats.currentBytes);
}
