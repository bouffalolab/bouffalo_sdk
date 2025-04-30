/*
 *  Copyright (C) Bouffalo Lab 2016-2023
 *  SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "utils.h"

long get_file_len(FILE *fp)
{
    long ret;

    if (fseek(fp, 0, SEEK_END)) {
        return -1;
    }
    ret = ftell(fp);
    if (fseek(fp, 0, SEEK_SET)) {
        return -1;
    }
    return ret;
}

void dump_buf(const void *buf, const int buf_len)
{
    int i;
    for (i = 0; i < buf_len; ++i) {
        printf("%02X ", ((uint8_t *)buf)[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
}

int sem_timedwait_ms(sem_t *sem, const long ms)
{
    struct timespec t;
#if 0 // Old system does NOT have this
    if (clock_gettime(CLOCK_REALTIME, &t)) {
        perror("clock_gettime");
        return -1;
    }
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    /* TIMEVAL_TO_TIMESPEC(&tv, &st); */
    my_timeval_to_timespec(&tv, &t);
#endif
    t.tv_sec += ms / 1000;
    t.tv_nsec += (ms % 1000) * 1000 * 1000;
    if (t.tv_nsec >= 1000 * 1000 * 1000) {
        t.tv_sec++;
        t.tv_nsec -= 1000 * 1000 * 1000;
    }
    while (sem_timedwait(sem, &t)) {
        if (errno == EINTR) {
            errno = 0;
        } else {
            return -1;
        }
    }
    return 0;
}
