/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include "avutil/av_config.h"

#if CONFIG_STREAMER_FILE
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "bflb_romfs.h"
#include <msp/kernel.h>
#include <xutils/debug.h>
#include <msp_fs.h>

#define MSP_FS_RDONLY                0x0000
#define MSP_FS_WRONLY                0x0001
#define MSP_FS_RDWR                  0x0002
#define MSP_FS_CREAT                 0x0200

int msp_fs_flags(int flags)
{
    switch(flags) {
    case MSP_FS_RDONLY:
        return O_RDONLY;

    case MSP_FS_WRONLY:
        return O_WRONLY;

    case MSP_FS_RDWR:
        return O_RDWR;

    case MSP_FS_CREAT:
        return O_CREAT;

    default:
        MSP_LOGE("fs flags type not support!\r\n");
    }

    return -1;
}

int msp_open(const char *path, int flags)
{
    int ret;
    romfs_file_t *fp = msp_malloc(sizeof(romfs_file_t));
    ret = romfs_open(fp, path, flags);
    if (ret != 0) {
        msp_free(fp);
        return -1;
    }
    return (int)fp;
}

int msp_stat(const char *path, struct stat *st)
{
    int ret;
    romfs_stat_t stat = {0};
    ret = romfs_stat(path, &stat);
    st->st_size = stat.st_size;
    return ret;
}

int msp_close(int fd)
{
    int ret = romfs_close((romfs_file_t *)fd);
    msp_free((void *)fd);
    return ret;
}

int msp_read(int fd, void *buf, size_t nbytes)
{
    return romfs_read((romfs_file_t *)fd, buf, nbytes);
}

int msp_write(int fd, const void *buf, size_t nbytes)
{
    return 0;
}

int msp_lseek(int fd, off_t offset, int whence)
{
    return romfs_lseek((romfs_file_t *)fd, offset, whence);
}
#endif 

