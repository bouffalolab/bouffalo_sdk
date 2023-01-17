/****************************************************************************
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
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "FreeRTOS.h"
#include "task.h"
#include "shell.h"

#include "bflb_mtimer.h"
#include "bflb_flash.h"
#include "bflb_l1c.h"
#include "bflb_uart.h"
#include "board.h"

#include "log.h"
#include "lfs.h"
#include "lfs_port.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/*!< use memory area after 512k */
#define LITTLEFS_FLASH_SIZE        (0x71000)
#define LITTLEFS_FLASH_BLOCK_SIZE  4096
#define LITTLEFS_FLASH_BLOCK_COUNT (LITTLEFS_FLASH_SIZE / LITTLEFS_FLASH_BLOCK_SIZE)
#define LITTLEFS_FLASH_BLOCK_CYCLE 500

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct bflb_device_s *uart0;

lfs_t lfs;
lfs_file_t file;

/*!< configuration of the filesystem is provided by this struct */
const struct lfs_config cfg = {
    // block device operations
    .read = lfs_xip_flash_read,
    .prog = lfs_xip_flash_prog,
    .erase = lfs_xip_flash_erase,
    .sync = lfs_xip_flash_sync,

    // block device configuration
    .read_size = 16,
    .prog_size = 16,
    .lookahead_size = 16,
    .cache_size = 16,

    .block_size = LITTLEFS_FLASH_BLOCK_SIZE,
    .block_count = LITTLEFS_FLASH_BLOCK_COUNT,
    .block_cycles = LITTLEFS_FLASH_BLOCK_CYCLE,
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

extern void shell_init_with_task(struct bflb_device_s *shell);
extern void cmd_kv_test(int argc, char **argv);
SHELL_CMD_EXPORT_ALIAS(cmd_kv_test, kv_test, shell kv test);

/****************************************************************************
 * Functions
 ****************************************************************************/

int main(void)
{
    board_init();

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    // mount the filesystem
    int err = lfs_mount(&lfs, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        LOG_W("try to reformat \r\n");
        err = lfs_format(&lfs, &cfg);
        if (err) {
            LOG_F("reformat fail\r\n");
            _CALL_ERROR();
        }

        LOG_I("reformat success\r\n");
        err = lfs_mount(&lfs, &cfg);
        if (err) {
            LOG_F("mount fail\r\n");
            _CALL_ERROR();
        }
    }

    LOG_I("mount success\r\n");

    // read current count
    uint32_t boot_count = 0;
    lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

    // update boot count
    boot_count += 1;
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs, &file);

    // release any resources we were using
    // lfs_unmount(&lfs);

    // print the boot count
    printf("boot_count: %d\n", boot_count);

    vTaskStartScheduler();

    while (1) {
    }
}
