/**
 * @file sdh_sdcard.c
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

#include "ff.h"     /* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include "sdh_sd.h"

struct sd_card_s sd_card;
struct sdh_host_s sdh_host;

int MMC_disk_status()
{
    return 0;
}

int MMC_disk_initialize()
{
    static bool inited = false;

    if (inited) {
        sdh_sd_deinit(&sd_card);
    }

    if (sdh_sd_card_init(&sd_card, &sdh_host) < 0) {
        sdh_sd_deinit(&sd_card);
        return -1;
    }
    inited = true;

    return 0;
}

int MMC_disk_read(BYTE *buff, LBA_t sector, UINT count)
{
    if (sdh_sd_read_blocks(&sd_card, (void *)buff, sector, count) < 0) {
        return -1;
    }

    return 0;
}

int MMC_disk_write(const BYTE *buff, LBA_t sector, UINT count)
{
    if (sdh_sd_write_blocks(&sd_card, (void *)buff, sector, count) < 0) {
        return -1;
    }

    return 0;
}

int MMC_disk_ioctl(BYTE cmd, void *buff)
{
    switch (cmd) {
        // Get R/W sector size (WORD)
        case GET_SECTOR_SIZE:
            *(WORD *)buff = sd_card.block_size;
            break;

        // Get erase block size in unit of sector (DWORD)
        case GET_BLOCK_SIZE:
            *(DWORD *)buff = 1;
            break;

        case GET_SECTOR_COUNT:
            *(DWORD *)buff = sd_card.block_count;
            break;

        case CTRL_SYNC:
            break;

        default:
            break;
    }

    return 0;
}

DSTATUS Translate_Result_Code(int result)
{
    return result;
}

void fatfs_sdh_driver_register(void)
{
    FATFS_DiskioDriverTypeDef SDH_DiskioDriver = { NULL };

    SDH_DiskioDriver.disk_status = MMC_disk_status;
    SDH_DiskioDriver.disk_initialize = MMC_disk_initialize;
    SDH_DiskioDriver.disk_write = MMC_disk_write;
    SDH_DiskioDriver.disk_read = MMC_disk_read;
    SDH_DiskioDriver.disk_ioctl = MMC_disk_ioctl;
    SDH_DiskioDriver.error_code_parsing = Translate_Result_Code;

    disk_driver_callback_init(DEV_SD, &SDH_DiskioDriver);
}
