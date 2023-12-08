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
#include "bflb_mtd.h"

#include "log.h"
#include "lfs.h"
#include "lfs_port.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct bflb_device_s *uart0;
lfs_t *lfs;
lfs_file_t file;

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
    bflb_mtd_init();

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    lfs = lfs_xip_init();
    if (lfs == NULL) {
        LOG_F("lfs_xip_init failed. errno: %d\r\n", errno);
        while (1)
            ;
    }

    // read current count
    uint32_t boot_count = 0;
    lfs_file_open(lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(lfs, &file, &boot_count, sizeof(boot_count));

    // update boot count
    boot_count += 1;
    lfs_file_rewind(lfs, &file);
    lfs_file_write(lfs, &file, &boot_count, sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(lfs, &file);

    // release any resources we were using
    // lfs_unmount(&lfs);

    // print the boot count
    printf("boot_count: %d\n", boot_count);

    vTaskStartScheduler();

    while (1) {
    }
}
