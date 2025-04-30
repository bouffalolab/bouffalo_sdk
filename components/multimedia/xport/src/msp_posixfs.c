/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <xutils/debug.h>
#include <msp_fs.h>

int msp_fs_flags(int flags)
{
    int true_flags = 0;

    if (flags & MSP_FS_CREAT) {
        true_flags |= O_CREAT;
    }

    if (flags & MSP_FS_RDWR) {
        true_flags |= O_RDWR;
    } else if (flags & MSP_FS_WRONLY) {
        true_flags |= O_WRONLY;
    } else if (flags & MSP_FS_RDONLY) {
        true_flags |= O_RDONLY;
    } else {
        MSP_LOGE("fs flags type not support!\r\n");
        return -1;
    }

    return true_flags;
}

int msp_open(const char *path, int flags)
{
    int ret = open(path, msp_fs_flags(flags));
    return (ret > 0) ? ret : 0;
}

int msp_stat(const char *path, struct stat *st)
{
    return stat(path, st);
}

int msp_close(const char *path, int fd)
{
    return close(fd);
}

int msp_read(const char *path, int fd, void *buf, size_t nbytes)
{
    return read(fd, buf, nbytes);
}

int msp_write(const char *path, int fd, const void *buf, size_t nbytes)
{
    return write(fd, buf, nbytes);
}

int msp_lseek(const char *path, int fd, off_t offset, int whence)
{
    return lseek(fd, offset, whence);
}
