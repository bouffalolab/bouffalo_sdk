/**
 * @file lfs_gcov_flush.c
 * @brief GCOV 数据刷新命令 - 手动触发覆盖率数据保存（LittleFS 版本）
 *
 * Copyright (c) 2026 Bouffalolab team
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
 */

#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include "shell.h"
#include "lfs.h"

#define DBG_TAG "GCOV_FLUSH"
#include "log.h"

/* GCOV flush 函数声明 */
extern void __gcov_dump(void);

/* GCOV 全局变量声明 */
extern void *__gcov_root;
extern void *__gcov_master;

/* LittleFS 句柄（在 lfs_show.c 中定义） */
extern lfs_t *lfs_p;
extern uint8_t lfs_mounted;

/* 清理旧的 gcov 文件 */
extern void gcov_cleanup_old_files_lfs(void);

/* 任务参数配置 */
#define GCOV_FLUSH_TASK_PRIORITY      1
#define GCOV_FLUSH_TASK_STACK_SIZE    2048

/**
 * @brief GCOV flush 一次性执行函数 - 手动触发
 */
static void gcov_flush_once(void)
{
    LOG_I("\r\n");
    LOG_I("=================================================\r\n");
    LOG_I("GCOV Flush Started\r\n");
    LOG_I("=================================================\r\n");

    /* 检查文件系统是否可访问 */
    if (!lfs_mounted || lfs_p == NULL) {
        LOG_E("=================================================\r\n");
        LOG_E("[GCOV] ERROR: LittleFS not accessible!\r\n");
        LOG_E("=================================================\r\n");
        LOG_E("Please run filesystem_init first.\r\n");
        LOG_E("\r\n");
        return;
    }

    LOG_I("GCOV Data Structures:\r\n");
    LOG_I("  __gcov_root     = %p\r\n", (void*)__gcov_root);
    LOG_I("  __gcov_master   = %p\r\n", (void*)__gcov_master);
    LOG_I("=================================================\r\n");

    /* 注意：不再自动清理旧的 gcov 文件，保留历史数据 */

    /* 检查 GCOV 是否已启用 */
    if (__gcov_root == NULL) {
        LOG_E("\r\n");
        LOG_E("=================================================\r\n");
        LOG_E("[GCOV] ERROR: GCOV coverage is NOT enabled!\r\n");
        LOG_E("=================================================\r\n");
        LOG_E("Reason: __gcov_root is NULL\r\n");
        LOG_E("\r\n");
        return;
    }

    LOG_I("\r\n");
    LOG_I("=================================================\r\n");
    LOG_I("[GCOV] Flushing coverage data...\r\n");
    LOG_I("[GCOV] __gcov_root = %p\r\n", (void*)__gcov_root);

    if (__gcov_root != NULL) {
        __gcov_dump();
        LOG_I("[GCOV] Flush completed successfully\r\n");
    }

    LOG_I("=================================================\r\n");
    LOG_I("\r\n");
}

/**
 * @brief GCOV flush 任务主函数 - 一次性执行
 */
static void gcov_flush_task(void *param)
{
    (void)param;

    gcov_flush_once();
    vTaskDelete(NULL);
}

/**
 * @brief GCOV dump 命令 - 手动触发 GCOV 数据刷新
 */
static int cmd_gcov_dump(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    gcov_flush_once();
    return 0;
}

/* 注册 shell 命令 */
SHELL_CMD_EXPORT_ALIAS(cmd_gcov_dump, gcov_dump, Dump GCOV coverage data to Flash);
