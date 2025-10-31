/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include "avutil/av_config.h"

#if defined(CONFIG_ROMFS)
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "bflb_romfs.h"
#include <msp/kernel.h>
#include <xutils/debug.h>
#include <msp_fs.h>

int msp_romfs_flags(int flags)
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

int msp_romfs_open(const char *path, int flags)
{
    int ret;
    romfs_file_t *fp = msp_malloc(sizeof(romfs_file_t));
    ret = romfs_open(fp, path, flags);
    if (ret != 0) {
        msp_free(fp);
        return 0;
    }
    return (int)fp;
}

int msp_romfs_stat(const char *path, struct stat *st)
{
    int ret;
    romfs_stat_t stat = {0};
    ret = romfs_stat(path, &stat);
    st->st_size = stat.st_size;
    return ret;
}

int msp_romfs_close(int fd)
{
    int ret = romfs_close((romfs_file_t *)fd);
    msp_free((void *)fd);
    return ret;
}

int msp_romfs_read(int fd, void *buf, size_t nbytes)
{
    return romfs_read((romfs_file_t *)fd, buf, nbytes);
}

int msp_romfs_write(int fd, const void *buf, size_t nbytes)
{
    return 0;
}

int msp_romfs_lseek(int fd, off_t offset, int whence)
{
    return romfs_lseek((romfs_file_t *)fd, offset, whence);
}
#endif 

