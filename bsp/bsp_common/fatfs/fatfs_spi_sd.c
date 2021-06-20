/**
 * @file fatfs_spi_sd.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#include "diskio.h"
#include "bsp_spi_sd.h"
#include "string.h"

const char *FR_Table[] = {
    "FR_OK：成功",                                           /* (0) Succeeded */
    "FR_DISK_ERR：底层硬件错误",                             /* (1) A hard error occurred in the low level disk I/O layer */
    "FR_INT_ERR：断言失败",                                  /* (2) Assertion failed */
    "FR_NOT_READY：物理驱动没有工作",                        /* (3) The physical drive cannot work */
    "FR_NO_FILE：文件不存在",                                /* (4) Could not find the file */
    "FR_NO_PATH：路径不存在",                                /* (5) Could not find the path */
    "FR_INVALID_NAME：无效文件名",                           /* (6) The path name format is invalid */
    "FR_DENIED：由于禁止访问或者目录已满访问被拒绝",         /* (7) Access denied due to prohibited access or directory full */
    "FR_EXIST：文件已经存在",                                /* (8) Access denied due to prohibited access */
    "FR_INVALID_OBJECT：文件或者目录对象无效",               /* (9) The file/directory object is invalid */
    "FR_WRITE_PROTECTED：物理驱动被写保护",                  /* (10) The physical drive is write protected */
    "FR_INVALID_DRIVE：逻辑驱动号无效",                      /* (11) The logical drive number is invalid */
    "FR_NOT_ENABLED：卷中无工作区",                          /* (12) The volume has no work area */
    "FR_NO_FILESYSTEM：没有有效的FAT卷",                     /* (13) There is no valid FAT volume */
    "FR_MKFS_ABORTED：由于参数错误f_mkfs()被终止",           /* (14) The f_mkfs() aborted due to any parameter error */
    "FR_TIMEOUT：在规定的时间内无法获得访问卷的许可",        /* (15) Could not get a grant to access the volume within defined period */
    "FR_LOCKED：由于文件共享策略操作被拒绝",                 /* (16) The operation is rejected according to the file sharing policy */
    "FR_NOT_ENOUGH_CORE：无法分配长文件名工作区",            /* (17) LFN working buffer could not be allocated */
    "FR_TOO_MANY_OPEN_FILES：当前打开的文件数大于_FS_SHARE", /* (18) Number of open files > _FS_SHARE */
    "FR_INVALID_PARAMETER：参数无效"                         /* (19) Given parameter is invalid */
};

int sd_disk_status(void)
{
    return 0;
}
int sd_disk_initialize(void)
{
    return SD_Init(&SD_CardInfo);
}
int sd_disk_read(BYTE *buff, LBA_t sector, UINT count)
{
    return SD_ReadBlock(sector, buff, count);
}
int sd_disk_write(const BYTE *buff, LBA_t sector, UINT count)
{
    return SD_WriteBlock(sector, (BYTE *)buff, count);
}
int sd_disk_ioctl(BYTE cmd, void *buff)
{
    int result = 0;

    switch (cmd) {
        case CTRL_SYNC:
            result = RES_OK;
            break;

        case GET_SECTOR_SIZE:
            *(DWORD *)buff = SD_CardInfo.CardBlockSize;
            result = RES_OK;
            break;

        case GET_BLOCK_SIZE:
            *(WORD *)buff = SD_CardInfo.CardBlockSize;
            result = RES_OK;
            break;

        case GET_SECTOR_COUNT:
            *(DWORD *)buff = SD_CardInfo.CardCapacity / SD_CardInfo.CardBlockSize;
            result = RES_OK;
            break;

        default:
            result = RES_PARERR;
            break;
    }

    return result;
}

DSTATUS Translate_Result_Code(int result)
{
    // MSG("%s\r\n",FR_Table[result]);
    return result;
}
FATFS_DiskioDriverTypeDef pNewDiskioDriver;

void fatfs_sd_driver_register(void)
{
    pNewDiskioDriver.MMC_disk_status = sd_disk_status;
    pNewDiskioDriver.MMC_disk_initialize = sd_disk_initialize;
    pNewDiskioDriver.MMC_disk_write = sd_disk_write;
    pNewDiskioDriver.MMC_disk_read = sd_disk_read;
    pNewDiskioDriver.MMC_disk_ioctl = sd_disk_ioctl;
    pNewDiskioDriver.Translate_Result_Code = Translate_Result_Code;
    disk_driver_callback_init(&pNewDiskioDriver);
}

int shellGetcwd(char *path, unsigned int len)
{
    int err;
    err = f_getcwd(path, len);

    if (err) {
        strcpy(path, "unknow");
    }

    return 0;
}