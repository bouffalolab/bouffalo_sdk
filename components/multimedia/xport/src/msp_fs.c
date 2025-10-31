/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include "avutil/av_config.h"

#if defined(CONFIG_STREAMER_FILE) && CONFIG_STREAMER_FILE
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <xutils/debug.h>
#include <msp/kernel.h>
#include <msp_fs.h>

typedef enum {
    FS_UNKNOW = 0,
    FS_FATFS,
    FS_ROMFS,
    FS_LTFS,
} FS_TYPE;

int get_fs_type(const char *path)
{
    if (0 == strncmp(path, "/romfs", 6)) {
        return FS_ROMFS;
    } else if (0 == strncmp(path, "/sd", 3)) {
        return FS_FATFS;
    } else if (0 == strncmp(path, "/ltfs", 5)) {
        return FS_LTFS;
    } else {
        return FS_UNKNOW;
    }
}

int msp_open(const char *path, int flags)
{
    int ret = 0;
    int fs_type = 0;

    fs_type = get_fs_type(path);

    switch(fs_type)
    {
        case FS_FATFS:
#if defined(CONFIG_FATFS)
            return msp_fatfs_open(path, msp_fatfs_flags(flags));
#else
            return ret;
#endif
        case FS_ROMFS:
#if defined(CONFIG_ROMFS)
            return msp_romfs_open(path, msp_romfs_flags(flags));
#else
            return ret;
#endif
        case FS_LTFS:
#if defined(CONFIG_LTFS)
            return msp_ltfs_open(path, msp_ltfs_flags(flags));
#else
            return ret;
#endif
        default:
            return ret;
    }
}

int msp_stat(const char *path, struct stat *st)
{
    int ret = -1;
    int fs_type = 0;

    fs_type = get_fs_type(path);

    switch(fs_type)
    {
        case FS_FATFS:
#if defined(CONFIG_FATFS)
            return msp_fatfs_stat(path, st);
#else
            return ret;
#endif
        case FS_ROMFS:
#if defined(CONFIG_ROMFS)
            return msp_romfs_stat(path, st);
#else
            return ret;
#endif
        case FS_LTFS:
#if defined(CONFIG_LTFS)
            return msp_ltfs_stat(path, st);
#else
            return ret;
#endif
        default:
            return ret;
    }
}

int msp_read(const char *path, int fd, void *buf, size_t nbytes)
{
    int ret = -1;
    int fs_type = 0;

    fs_type = get_fs_type(path);

    switch(fs_type)
    {
        case FS_FATFS:
#if defined(CONFIG_FATFS)
            return msp_fatfs_read(fd, buf, nbytes);
#else
            return ret;
#endif
        case FS_ROMFS:
#if defined(CONFIG_ROMFS)
            return msp_romfs_read(fd, buf, nbytes);
#else
            return ret;
#endif
        case FS_LTFS:
#if defined(CONFIG_LTFS)
            return msp_ltfs_read(fd, buf, nbytes);
#else
            return ret;
#endif
        default:
            return ret;
    }
}

int msp_write(const char *path, int fd, const void *buf, size_t nbytes)
{
    // TBD

    return 0;
}

int msp_close(const char *path, int fd)
{
    int ret = -1;
    int fs_type = 0;

    fs_type = get_fs_type(path);

    switch(fs_type)
    {
        case FS_FATFS:
#if defined(CONFIG_FATFS)
            return msp_fatfs_close(fd);
#else
            return ret;
#endif
        case FS_ROMFS:
#if defined(CONFIG_ROMFS)
            return msp_romfs_close(fd);
#else
            return ret;
#endif
        case FS_LTFS:
#if defined(CONFIG_LTFS)
            return msp_ltfs_close(fd);
#else
            return ret;
#endif
        default:
            return ret;
    }
}

int msp_lseek(const char *path, int fd, off_t offset, int whence)
{
    int ret = -1;
    int fs_type = 0;

    fs_type = get_fs_type(path);

    switch(fs_type)
    {
        case FS_FATFS:
#if defined(CONFIG_FATFS)
            return msp_fatfs_lseek(fd, offset, whence);
#else
            return ret;
#endif
        case FS_ROMFS:
#if defined(CONFIG_ROMFS)
            return msp_romfs_lseek(fd, offset, whence);
#else
            return ret;
#endif
        case FS_LTFS:
#if defined(CONFIG_LTFS)
            return msp_ltfs_lseek(fd, offset, whence);
#else
            return ret;
#endif
        default:
            return ret;
    }
}
#endif 
