/*
 *  Copyright (C) Bouffalo Lab 2016-2023
 *  SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <errno.h>
#include <semaphore.h>
#include <sys/time.h>
#include <time.h>

#ifndef UTILS_H_EELMNLVU
#define UTILS_H_EELMNLVU

#define MAC_FMT "%02X:%02X:%02X:%02X:%02X:%02X"
#define ARR_ELE_6(e) (e)[0], (e)[1], (e)[2], (e)[3], (e)[4], (e)[5]

static inline void my_timeval_to_timespec(struct timeval *tv, struct timespec *ts)
{
    ts->tv_sec = tv->tv_sec;
    ts->tv_nsec = tv->tv_usec * 1000;
}

long get_file_len(FILE *fp);
void dump_buf(const void *buf, const int buf_len);
int sem_timedwait_ms(sem_t *sem, const long ms);

#endif /* end of include guard:  */
