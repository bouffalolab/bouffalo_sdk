/* SPDX-License-Identifier: Apache-2.0 */

#include "os.h"

#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <sys/random.h>
#include <nuttx/kmalloc.h>

int os_get_time(struct os_time *t)
{
    struct timeval tv;
    int ret = gettimeofday(&tv, NULL);

    if (ret == 0) {
        t->sec = tv.tv_sec;
        t->usec = tv.tv_usec;
    }
    return ret;
}

unsigned long os_random(void)
{
    return (unsigned long)random();
}

int os_get_random(unsigned char *buf, size_t len)
{
    return getrandom(buf, len, 0) == (ssize_t)len ? 0 : -1;
}

void os_sleep(os_time_t sec, os_time_t usec)
{
    struct timespec delay = {
        .tv_sec = sec + usec / 1000000,
        .tv_nsec = (usec % 1000000) * 1000
    };

    while (nanosleep(&delay, &delay) < 0 && errno == EINTR) {
    }
}

void *wpa_supplicant_malloc(size_t size)
{
    return kmm_malloc(size);
}

void wpa_supplicant_free(void *ptr)
{
    kmm_free(ptr);
}

void *wpa_supplicant_realloc(void *ptr, size_t size)
{
    return kmm_realloc(ptr, size);
}

void *wpa_supplicant_zalloc(size_t nmemb, size_t size)
{
    return kmm_calloc(nmemb, size);
}

void wpa_supplicant_bzero(void *s, size_t n)
{
    explicit_bzero(s, n);
}
