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

#define MSP_FS_RDONLY                0x0001
#define MSP_FS_WRONLY                0x0002
#define MSP_FS_RDWR                  0x0003
#define MSP_FS_CREAT                 0x0200

#if defined(CONFIG_FATFS) && CONFIG_FATFS
int filesystem_init(void);
int msp_fatfs_flags(int flags);
int msp_fatfs_open(const char *path, int flags);
int msp_fatfs_stat(const char *path, struct stat *st);
int msp_fatfs_close(int fd);
int msp_fatfs_read(int fd, void *buf, size_t nbytes);
int msp_fatfs_write(int fd, const void *buf, size_t nbytes);
int msp_fatfs_lseek(int fd, off_t offset, int whence);
#endif

#if defined(CONFIG_ROMFS) && CONFIG_ROMFS
int msp_romfs_flags(int flags);
int msp_romfs_open(const char *path, int flags);
int msp_romfs_stat(const char *path, struct stat *st);
int msp_romfs_close(int fd);
int msp_romfs_read(int fd, void *buf, size_t nbytes);
int msp_romfs_write(int fd, const void *buf, size_t nbytes);
int msp_romfs_lseek(int fd, off_t offset, int whence);
#endif

#if defined(CONFIG_LTFS) && CONFIG_LTFS
int msp_ltfs_flags(int flags);
int msp_ltfs_open(const char *path, int flags);
int msp_ltfs_stat(const char *path, struct stat *st);
int msp_ltfs_close(int fd);
int msp_ltfs_read(int fd, void *buf, size_t nbytes);
int msp_ltfs_write(int fd, const void *buf, size_t nbytes);
int msp_ltfs_lseek(int fd, off_t offset, int whence);
#endif

int msp_open(const char *path, int flags);
int msp_stat(const char *path, struct stat *st);
int msp_close(const char *path, int fd);
int msp_read(const char *path, int fd, void *buf, size_t nbytes);
int msp_write(const char *path, int fd, const void *buf, size_t nbytes);
int msp_lseek(const char *path, int fd, off_t offset, int whence);

#endif
