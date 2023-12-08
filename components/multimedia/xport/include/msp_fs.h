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

#define MSP_FS_RDONLY                0x0000
#define MSP_FS_WRONLY                0x0001
#define MSP_FS_RDWR                  0x0002
#define MSP_FS_CREAT                 0x0200

int msp_fs_flags(int flags);

int msp_open(const char *path, int flags);

int msp_stat(const char *path, struct stat *st);

int msp_close(int fd);

int msp_read(int fd, void *buf, size_t nbytes);

int msp_write(int fd, const void *buf, size_t nbytes);

int msp_lseek(int fd, off_t offset, int whence);

#endif
