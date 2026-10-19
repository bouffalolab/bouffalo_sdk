/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef __SYS_SELECT_H
#define __SYS_SELECT_H

#include <sys/_timeval.h>

// #ifndef FD_SETSIZE
// #define FD_SETSIZE 1024
// #endif

// typedef unsigned long fd_mask;
// typedef struct {
//     unsigned long fds_bits[FD_SETSIZE / 8 / sizeof(long)];
// } fd_set;

// #define FD_ZERO(s) do { int __i; unsigned long *__b = (s)->fds_bits; for (__i = sizeof(fd_set) / sizeof(long); __i; __i--) *__b++ = 0; } while (0)
// #define FD_SET(d, s)   ((s)->fds_bits[(d) / (8 * sizeof(long))] |= (1UL << ((d) % (8 * sizeof(long)))))
// #define FD_CLR(d, s)   ((s)->fds_bits[(d) / (8 * sizeof(long))] &= ~(1UL << ((d) % (8 * sizeof(long)))))
// #define FD_ISSET(d, s) (!!((s)->fds_bits[(d) / (8 * sizeof(long))] & (1UL << ((d) % (8 * sizeof(long))))))

#ifdef __cplusplus
extern "C" {
#endif

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
                         struct timeval *timeout);

extern int select2(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
            struct timeval *timeout, void *semaphore);

#ifdef __cplusplus
}
#endif

#endif /*__SYS_SELECT_H*/
