/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef __MSP_FS_H__
#define __MSP_FS_H__

#include <stdint.h>
#include <stdio.h>

int msp_open(const char *path, int flags);

int msp_stat(const char *path, struct stat *st);

int msp_close(int fd);

int msp_read(int fd, void *buf, size_t nbytes);

int msp_write(int fd, const void *buf, size_t nbytes);

int msp_lseek(int fd, off_t offset, int whence);

#endif
