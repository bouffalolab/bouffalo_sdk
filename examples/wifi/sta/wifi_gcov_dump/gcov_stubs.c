/**
 * @file gcov_stubs.c
 * @brief GCOV 桩函数 - 为嵌入式系统提供 libgcov 所需的最小实现
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

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include "ff.h"

/* ============================================================================
 * GCOV 文件桥接层 - 将 FILE* 桥接到 FatFS FIL
 * ============================================================================ */

#define MAX_GCOV_FILES  32
#define MAX_PATH_LEN    512

/* 文件桥接结构体 */
typedef struct {
    FILE       *std_file;    /* 标准 FILE* 指针 */
    FIL         fatfs_file;  /* FatFS 文件对象 */
    uint8_t     in_use;      /* 是否已使用 */
} gcov_file_bridge_t;

/* 文件桥接表 */
static gcov_file_bridge_t g_file_bridge[MAX_GCOV_FILES];

/**
 * @brief 通过 FILE* 查找对应的桥接结构
 */
static gcov_file_bridge_t* find_bridge(FILE *stream)
{
    int i;
    for (i = 0; i < MAX_GCOV_FILES; i++) {
        if (g_file_bridge[i].in_use && g_file_bridge[i].std_file == stream) {
            return &g_file_bridge[i];
        }
    }
    return NULL;
}

/**
 * @brief 释放文件桥接
 */
static void free_bridge(FILE *stream)
{
    int i;
    for (i = 0; i < MAX_GCOV_FILES; i++) {
        if (g_file_bridge[i].in_use && g_file_bridge[i].std_file == stream) {
            g_file_bridge[i].in_use = 0;
            g_file_bridge[i].std_file = NULL;
            return;
        }
    }
}

/* ============================================================================
 * GCOV 桩函数实现 - 使用链接器包装
 * ============================================================================ */

/**
 * @brief 清理旧的 gcov 文件（带 # 的长路径文件）
 *
 * 这个函数应该在 FatFS 初始化后调用一次
 */
void gcov_cleanup_old_files(void)
{
    DIR dir;
    FILINFO fno;
    FRESULT res;
    int deleted_count = 0;

    /* 打开根目录 */
    res = f_opendir(&dir, "/sd");
    if (res != FR_OK) {
        return;
    }

    /* 遍历目录，删除所有带 # 或以 # 开头的文件 */
    while (1) {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0) {
            break;
        }

        /* 跳过 . 和 .. */
        if (strcmp(fno.fname, ".") == 0 || strcmp(fno.fname, "..") == 0) {
            continue;
        }

        /* 只删除文件，不删除目录 */
        if (!(fno.fattrib & AM_DIR)) {
            /* 检查文件名中是否包含 # 或以 # 开头 */
            if (strchr(fno.fname, '#') != NULL || fno.fname[0] == '#') {
                /* 删除文件 */
                char filepath[MAX_PATH_LEN];
                strncpy(filepath, "/sd/", sizeof(filepath) - 1);
                size_t len = strlen(filepath);
                size_t i;
                for (i = 0; fno.fname[i] != '\0' && len < sizeof(filepath) - 1; i++) {
                    filepath[len++] = fno.fname[i];
                }
                filepath[len] = '\0';

                res = f_unlink(filepath);
                if (res == FR_OK) {
                    deleted_count++;
                }
            }
        }
    }

    f_closedir(&dir);
}

/**
 * @brief __wrap_fopen 实现 - 桥接到 FatFS f_open
 */
FILE* __wrap_fopen(const char *path, const char *mode)
{
    static FILE file_handle[MAX_GCOV_FILES];
    char fatfs_path[MAX_PATH_LEN];
    gcov_file_bridge_t *bridge;
    int bridge_idx = -1;
    BYTE fatfs_mode;
    int i;

    if (path == NULL || mode == NULL) {
        return NULL;
    }

    /* 直接使用原始路径，不做任何修改 */
    strncpy(fatfs_path, path, sizeof(fatfs_path) - 1);
    fatfs_path[sizeof(fatfs_path) - 1] = '\0';

    /* 转换文件模式 */
    if (strchr(mode, 'w') != NULL) {
        fatfs_mode = FA_WRITE | FA_CREATE_ALWAYS;
    } else if (strchr(mode, 'a') != NULL) {
        fatfs_mode = FA_WRITE | FA_OPEN_APPEND;
    } else if (strchr(mode, 'r') != NULL) {
        fatfs_mode = FA_READ;
    } else {
        fatfs_mode = FA_READ | FA_WRITE;
    }

    /* 分配桥接槽 */
    for (i = 0; i < MAX_GCOV_FILES; i++) {
        if (!g_file_bridge[i].in_use) {
            bridge_idx = i;
            break;
        }
    }

    if (bridge_idx == -1) {
        return NULL;
    }

    bridge = &g_file_bridge[bridge_idx];

    /* 打开 FatFS 文件 */
    FRESULT res = f_open(&bridge->fatfs_file, fatfs_path, fatfs_mode);
    if (res != FR_OK) {
        return NULL;
    }

    /* 设置桥接 */
    bridge->std_file = &file_handle[bridge_idx];
    bridge->in_use = 1;

    return bridge->std_file;
}

/**
 * @brief __wrap_fclose 实现 - 桥接到 FatFS f_close
 */
int __wrap_fclose(FILE *stream)
{
    gcov_file_bridge_t *bridge;

    if (stream == NULL) {
        return EOF;
    }

    bridge = find_bridge(stream);
    if (bridge == NULL) {
        return EOF;
    }

    /* 关闭 FatFS 文件 */
    FRESULT res = f_close(&bridge->fatfs_file);

    /* 释放桥接 */
    free_bridge(stream);

    return (res == FR_OK) ? 0 : EOF;
}

/**
 * @brief __wrap_fwrite 实现 - 桥接到 FatFS f_write
 */
size_t __wrap_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    gcov_file_bridge_t *bridge;
    UINT bytes_written;
    size_t total_size;

    if (ptr == NULL || stream == NULL) {
        return 0;
    }

    total_size = size * nmemb;
    if (total_size == 0) {
        return 0;
    }

    /* 查找对应的 FatFS 文件 */
    bridge = find_bridge(stream);
    if (bridge == NULL) {
        return 0;
    }

    /* 调用 FatFS 写入 */
    FRESULT res = f_write(&bridge->fatfs_file, ptr, total_size, &bytes_written);
    if (res != FR_OK) {
        return 0;
    }

    /* 返回写入的对象数量 */
    return bytes_written / size;
}

void __wrap___gcov_lock()
{
    return;
}

void __wrap___gcov_unlock()
{
    return;
}


/**
 * @brief __wrap_fflush 实现 - 桥接到 FatFS f_sync
 */
int __wrap_fflush(FILE *stream)
{
    gcov_file_bridge_t *bridge;
    int i;

    if (stream == NULL) {
        /* fflush(NULL) - 刷新所有打开的流 */
        for (i = 0; i < MAX_GCOV_FILES; i++) {
            if (g_file_bridge[i].in_use) {
                f_sync(&g_file_bridge[i].fatfs_file);
            }
        }
        return 0;
    }

    /* 查找对应的 FatFS 文件 */
    bridge = find_bridge(stream);
    if (bridge == NULL) {
        return EOF;
    }

    /* 调用 FatFS 同步 */
    FRESULT res = f_sync(&bridge->fatfs_file);
    if (res != FR_OK) {
        return EOF;
    }

    return 0;
}

/**
 * @brief getpid 桩函数
 *
 * 嵌入式系统通常没有进程概念，返回固定值 1
 */
int getpid(void)
{
    return 1;
}

char *getenv(const char *name)
{
    //printf("getenv %s \r\n", name);
    (void)name;

    /* Embedded systems typically don't have environment variables */
    return NULL;
}

/**
 * @brief fread 桩函数
 *
 * libgcov 需要这个函数，但实际不读取数据
 */
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    (void)ptr;
    (void)size;
    (void)nmemb;
    (void)stream;
    return 0;
}

/**
 * @brief fseek 桩函数
 */
int fseek(FILE *stream, long offset, int whence)
{
    (void)stream;
    (void)offset;
    (void)whence;
    return 0;
}

/**
 * @brief fputs 桩函数
 */
int fputs(const char *s, FILE *stream)
{
    (void)s;
    (void)stream;
    return 0;
}

/**
 * @brief vfprintf 桩函数
 *
 * libgcov 用这个打印错误信息，嵌入式环境中忽略
 */
//int vfprintf(FILE *stream, const char *format, va_list ap)
//{
//    (void)stream;
//    (void)format;
//    (void)ap;
//    return 0;
//}

