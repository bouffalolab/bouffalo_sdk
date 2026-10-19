/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <msp/kernel.h>
#include <sys/select.h>

__attribute__((weak)) int select2(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                                  struct timeval *timeout, void *semaphore)
{
    uint32_t tomeout_ms;

    if (timeout) {
        tomeout_ms = timeout->tv_sec * 1000 + timeout->tv_usec / 1000;
    } else {
        tomeout_ms = MSP_WAIT_FOREVER;
    }

    if (semaphore && msp_sem_is_valid(semaphore)) {
        msp_sem_wait(semaphore, tomeout_ms);
    } else {
        msp_msleep(tomeout_ms);
    }

    return  0;
}


/* dummy select, fix compile error with no net evn */
__attribute__((weak)) int select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                                 struct timeval *timeout)
{
    return select2(maxfdp1, readset, writeset, exceptset, timeout, NULL);
}
