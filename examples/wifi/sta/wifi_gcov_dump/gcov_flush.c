/**
 * @file gcov_flush.c
 * @brief GCOV 数据刷新命令 - 手动触发覆盖率数据保存
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

#define DBG_TAG "GCOV_FLUSH"
#include "log.h"

/* GCOV flush 函数声明（由编译器在启用覆盖率时自动生成） */
extern void __gcov_dump(void);

/* GCOV 全局变量声明（来自 libgcc/libgcov） */
extern void *__gcov_root;   /* GCOV 数据结构根指针 */
extern void *__gcov_master; /* GCOV 主控制结构 */

/* 清理旧的 gcov 文件（在 gcov_stubs.c 中实现） */
extern void gcov_cleanup_old_files(void);

/* 任务参数配置 */
#define GCOV_FLUSH_TASK_PRIORITY      1  /* 低优先级，不影响关键任务 */
#define GCOV_FLUSH_TASK_STACK_SIZE    2048 /* 栈大小 - 增加 FatFS LFN 缓冲区需求 */

/**
 * @brief GCOV flush 一次性执行函数 - 手动触发
 *
 * 执行一次 GCOV 数据刷新并退出
 */
static void gcov_flush_once(void)
{
    LOG_I("\r\n");
    LOG_I("=================================================\r\n");
    LOG_I("GCOV Flush Started\r\n");
    LOG_I("=================================================\r\n");
    LOG_I("GCOV Data Structures:\r\n");
    LOG_I("  __gcov_root     = %p\r\n", (void*)__gcov_root);
    LOG_I("  __gcov_master   = %p\r\n", (void*)__gcov_master);
    LOG_I("=================================================\r\n");

    /* 清理旧的 gcov 文件（带 # 的长路径文件） */
    LOG_I("Cleaning up old gcov files...\r\n");
    gcov_cleanup_old_files();
    LOG_I("Cleanup completed.\r\n");
    LOG_I("=================================================\r\n");

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
 *
 * @param param 任务参数（未使用）
 */
static void gcov_flush_task(void *param)
{
    (void)param;  /* 未使用的参数 */

    /* 执行一次性的 GCOV flush */
    gcov_flush_once();

    /* 任务完成，删除自己 */
    vTaskDelete(NULL);
}

/**
 * @brief GCOV dump 命令 - 手动触发 GCOV 数据刷新
 *
 * 用法: gcov_dump
 *
 * @param argc 参数个数
 * @param argv 参数数组
 * @return int 0-成功, 负值-失败
 */
static int cmd_gcov_dump(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    gcov_flush_once();

    return 0;
}

/* 注册 shell 命令 */
SHELL_CMD_EXPORT_ALIAS(cmd_gcov_dump, gcov_dump, Dump GCOV coverage data to SD card);
