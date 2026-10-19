/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include "avutil/av_config.h"

#if defined(CONFIG_FATFS) && CONFIG_FATFS
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <msp/kernel.h>
#include <xutils/debug.h>
#include <msp_fs.h>
#include "fatfs_diskio_register.h"
#include "ff.h"
#include "log.h"
// #include "bl616_glb.h"

#define f_unmount(path) f_mount(0, path, 0)

FATFS fs;
__attribute((aligned(64))) static uint32_t workbuf[4096];

MKFS_PARM fs_para = {
    .fmt = FM_FAT32,     /* Format option (FM_FAT, FM_FAT32, FM_EXFAT and FM_SFD) */
    .n_fat = 1,          /* Number of FATs */
    .align = 0,          /* Data area alignment (sector) */
    .n_root = 1,         /* Number of root directory entries */
    .au_size = 512 * 32, /* Cluster size (byte) */
};

int filesystem_init(void)
{
    FRESULT ret;

    board_sdh_gpio_init();

    fatfs_sdh_driver_register();

    ret = f_mount(&fs, "/sd", 1);

    if (ret == FR_NO_FILESYSTEM) {
        MSP_LOGW("No filesystem yet, try to be formatted...");

        ret = f_mkfs("/sd", &fs_para, workbuf, sizeof(workbuf));

        if (ret != FR_OK) {
            MSP_LOGE("fail to make filesystem %d", ret);
            // _CALL_ERROR();
            return ret;
        }

        if (ret == FR_OK) {
            MSP_LOGI("done with formatting.");
            MSP_LOGI("first start to unmount.");
            ret = f_unmount("/sd");
            MSP_LOGI("then start to remount.");
        }
    } else if (ret != FR_OK) {
        MSP_LOGE("fail to mount filesystem,error= %d", ret);
        MSP_LOGE("SD card might fail to initialise.");
        // _CALL_ERROR();
        return ret;
    } else {
        MSP_LOGD("Succeed to mount filesystem");
    }

    if (ret == FR_OK) {
        MSP_LOGI("FileSystem cluster size:%d-sectors (%d-Byte)", fs.csize, fs.csize * 512);
    }

    return ret;
}

int msp_fatfs_flags(int flags)
{
    int true_flags = 0;

    if (flags & MSP_FS_CREAT) {
        true_flags |= FA_CREATE_ALWAYS;
    }

    if (flags & MSP_FS_RDWR) {
        true_flags |= (FA_READ | FA_WRITE);
    } else if (flags & MSP_FS_WRONLY) {
        true_flags |= FA_WRITE;
    } else if (flags & MSP_FS_RDONLY) {
        true_flags |= FA_READ;
    } else {
        MSP_LOGE("fs flags type not support!\r\n");
        return -1;
    }

    return true_flags;
}

int msp_fatfs_open(const char *path, int flags)
{
    int ret = 0;

    FIL *fnew = msp_malloc(sizeof(FIL));
    if (NULL == fnew)
    {
        return ret;
    }

    ret = f_open(fnew, path, flags);
    if (ret != FR_OK) {
        return 0;
    } else {
        return (int)fnew;
    }
}

int msp_fatfs_stat(const char *path, struct stat *st)
{
    FRESULT fresult;
    FILINFO fno;

    fresult = f_stat(path, &fno);
    if (fresult != FR_OK) {
        return -1;
    } else {
        st->st_size = fno.fsize;
        return 0;
    }
}

int msp_fatfs_close(int fd)
{
    int ret = f_close((FIL *)fd);
    msp_free((void *)fd);
    return ret;
}

__attribute((aligned(64))) BYTE RW_Buffer[CONFIG_AV_STREAM_INNER_BUF_SIZE] = { 0 };
int msp_fatfs_read(int fd, void *buf, size_t nbytes)
{
    UINT fnum;
    int ret;
    memset(RW_Buffer, 0, CONFIG_AV_STREAM_INNER_BUF_SIZE);

    ret = f_read((FIL *)fd, RW_Buffer, nbytes, &fnum);

    memcpy(buf, RW_Buffer, fnum);
    if (ret != FR_OK) {
        return -1;
    } else {
        return fnum;
    }
}

int msp_fatfs_write(int fd, const void *buf, size_t nbytes)
{
    return 0;
}

int msp_fatfs_lseek(int fd, off_t offset, int whence)
{
    return f_lseek((FIL *)fd, offset);
}
#endif 

