/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include "avutil/av_config.h"

#if defined(CONFIG_LTFS) && CONFIG_LTFS
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <msp/kernel.h>
#include <xutils/debug.h>
#include <msp_fs.h>
#include "lfs.h"
#include "lfs_port.h"

lfs_t *lfs;

int msp_ltfs_flags(int flags)
{
    int true_flags = 0;

    if (flags & MSP_FS_CREAT) {
        true_flags |= LFS_O_CREAT;
    }

    if (flags & MSP_FS_RDWR) {
        true_flags |= LFS_O_RDWR;
    } else if (flags & MSP_FS_WRONLY) {
        true_flags |= LFS_O_WRONLY;
    } else if (flags & MSP_FS_RDONLY) {
        true_flags |= LFS_O_RDONLY;
    } else {
        MSP_LOGE("lfs flags type not support!\r\n");
        return -1;
    }

    return true_flags;
}

char* extractSubstr(const char* str, const char* prefix) {
    if (str == NULL || prefix == NULL) {
        return NULL;
    }

    const char* substr = strstr(str, prefix);
    if (substr != NULL) {
        return substr + strlen(prefix);
    }

    return NULL;
}

int msp_ltfs_open(const char *path, int flags)
{
    int err = 0;
    lfs_file_t *file = msp_malloc(sizeof(lfs_file_t));

    if (file == NULL) {
        MSP_LOGE("lfs_file_open failed\r\n");
        return -1;
    }

    if(!lfs)
    {
        lfs = lfs_xip_init();
        if (lfs == NULL) {
            MSP_LOGE("lfs_xip_init failed\r\n");
            return -1;
        }
    }
    
    err = lfs_file_open(lfs, file, extractSubstr(path, "/ltfs/"), flags);
    
    if (err != LFS_ERR_OK) {
        return -1;
    }

    return (int)file;
}

int msp_ltfs_stat(const char *path, struct stat *st)
{
    int err = 0;
    struct lfs_info info;

    if(!lfs)
    {
        lfs = lfs_xip_init();
        if (lfs == NULL) {
            MSP_LOGE("lfs_xip_init failed\r\n");
            return -1;
        }
    }

    err = lfs_stat(lfs, extractSubstr(path, "/ltfs/"), &info);
    if (err != LFS_ERR_OK) {
        return -1;
    } else {
        st->st_size = info.size;
        return 0;
    }
}

int msp_ltfs_close(int fd)
{
    int ret = lfs_file_close(lfs, (lfs_file_t *)fd);
    msp_free((void *)fd);
    return ret;
}

int msp_ltfs_read(int fd, void *buf, size_t nbytes)
{
    int ret = 0;
    
    ret = lfs_file_read(lfs, (lfs_file_t *)fd, buf, nbytes);
    
    if (!ret) {
        return -1;
    } else {
        return ret;
    }
}

int msp_ltfs_write(int fd, const void *buf, size_t nbytes)
{
    return 0;
}

int msp_ltfs_lseek(int fd, off_t offset, int whence)
{
    return lfs_file_seek(lfs, (lfs_file_t *)fd, offset, whence);
}
#endif 

