/**
 * @file lfs_gcov_stubs.c
 * @brief GCOV 桩函数 - 为嵌入式系统提供 libgcov 所需的最小实现（LittleFS 版本）
 *
 * 将 FILE* 桥接到 LittleFS lfs_file_t
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
#include <string.h>
#include "lfs.h"

/* ============================================================================
 * GCOV 文件桥接层 - 将 FILE* 桥接到 LittleFS lfs_file_t
 * ============================================================================ */

#define MAX_GCOV_FILES  32
#define MAX_PATH_LEN    512

/* 文件桥接结构体 */
typedef struct {
    FILE       *std_file;     /* 标准 FILE* 指针 */
    lfs_file_t lfs_file;      /* LittleFS 文件对象 */
    uint8_t     in_use;       /* 是否已使用 */
    int         open_flags;   /* 打开标志 */
} gcov_file_bridge_t;

/* 文件桥接表 */
static gcov_file_bridge_t g_file_bridge[MAX_GCOV_FILES];

/* LittleFS 句柄（在 lfs_show.c 中定义） */
extern lfs_t *lfs_p;

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
 * 这个函数应该在 LittleFS 初始化后调用一次
 */
void gcov_cleanup_old_files_lfs(void)
{
    lfs_dir_t dir;
    struct lfs_info info;
    int ret;
    int deleted_count = 0;

    if (lfs_p == NULL) {
        return;
    }

    /* 打开根目录 */
    ret = lfs_dir_open(lfs_p, &dir, "/");
    if (ret < 0) {
        return;
    }

    /* 遍历目录，删除所有带 # 或以 # 开头的文件 */
    while (1) {
        ret = lfs_dir_read(lfs_p, &dir, &info);
        if (ret <= 0) {
            break;
        }

        /* 跳过 . 和 .. */
        if (strcmp(info.name, ".") == 0 || strcmp(info.name, "..") == 0) {
            continue;
        }

        /* 只删除文件，不删除目录 */
        if (info.type == LFS_TYPE_REG) {
            /* 检查文件名中是否包含 # 或以 # 开头 */
            if (strchr(info.name, '#') != NULL || info.name[0] == '#') {
                /* 删除文件 */
                ret = lfs_remove(lfs_p, info.name);
                if (ret >= 0) {
                    deleted_count++;
                }
            }
        }
    }

    lfs_dir_close(lfs_p, &dir);
}

/**
 * @brief __wrap_fopen 实现 - 桥接到 LittleFS lfs_file_open
 */
FILE* __wrap_fopen(const char *path, const char *mode)
{
    static FILE file_handle[MAX_GCOV_FILES];
    char lfs_path[MAX_PATH_LEN];
    gcov_file_bridge_t *bridge;
    int bridge_idx = -1;
    int lfs_flags;
    int i;

    if (path == NULL || mode == NULL) {
        return NULL;
    }

    if (lfs_p == NULL) {
        return NULL;
    }

    /* 路径转换：简化文件名，避免长文件名限制 */
    const char *basename = NULL;

    /* 查找最后一个 '/' 或 '#' 后面的文件名 */
    const char *last_slash = strrchr(path, '/');
    const char *last_hash = strrchr(path, '#');

    if (last_hash != NULL && (last_slash == NULL || last_hash > last_slash)) {
        basename = last_hash + 1;
    } else if (last_slash != NULL) {
        basename = last_slash + 1;
    } else {
        basename = path;
    }

    /* 构建简化路径：简短文件名 */
    strncpy(lfs_path, basename, sizeof(lfs_path) - 1);
    lfs_path[sizeof(lfs_path) - 1] = '\0';

    /* 转换文件模式 */
    if (strchr(mode, 'w') != NULL) {
        lfs_flags = LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC;
    } else if (strchr(mode, 'a') != NULL) {
        lfs_flags = LFS_O_WRONLY | LFS_O_CREAT | LFS_O_APPEND;
    } else if (strchr(mode, 'r') != NULL) {
        lfs_flags = LFS_O_RDONLY;
    } else {
        lfs_flags = LFS_O_RDWR;
    }

    /* 如果包含 '+' 则添加读写标志 */
    if (strchr(mode, '+') != NULL) {
        lfs_flags |= LFS_O_RDWR;
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

    /* 打开 LittleFS 文件 */
    int ret = lfs_file_open(lfs_p, &bridge->lfs_file, lfs_path, lfs_flags);
    if (ret < 0) {
        return NULL;
    }

    /* 设置桥接 */
    bridge->std_file = &file_handle[bridge_idx];
    bridge->in_use = 1;
    bridge->open_flags = lfs_flags;

    return bridge->std_file;
}

/**
 * @brief __wrap_fclose 实现 - 桥接到 LittleFS lfs_file_close
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

    /* 关闭 LittleFS 文件 */
    int ret = lfs_file_close(lfs_p, &bridge->lfs_file);

    /* 释放桥接 */
    free_bridge(stream);

    return (ret >= 0) ? 0 : EOF;
}

/**
 * @brief __wrap_fwrite 实现 - 桥接到 LittleFS lfs_file_write
 */
size_t __wrap_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    gcov_file_bridge_t *bridge;
    lfs_ssize_t bytes_written;
    size_t total_size;

    if (ptr == NULL || stream == NULL) {
        return 0;
    }

    total_size = size * nmemb;
    if (total_size == 0) {
        return 0;
    }

    /* 查找对应的 LittleFS 文件 */
    bridge = find_bridge(stream);
    if (bridge == NULL) {
        return 0;
    }

    /* 调用 LittleFS 写入 */
    bytes_written = lfs_file_write(lfs_p, &bridge->lfs_file, ptr, total_size);
    if (bytes_written < 0) {
        return 0;
    }

    /* 返回写入的对象数量 */
    return bytes_written / size;
}

/**
 * @brief __wrap_fflush 实现 - 桥接到 LittleFS lfs_file_sync
 */
int __wrap_fflush(FILE *stream)
{
    gcov_file_bridge_t *bridge;
    int i;
    int ret;

    if (stream == NULL) {
        /* fflush(NULL) - 刷新所有打开的流 */
        for (i = 0; i < MAX_GCOV_FILES; i++) {
            if (g_file_bridge[i].in_use) {
                lfs_file_sync(lfs_p, &g_file_bridge[i].lfs_file);
            }
        }
        return 0;
    }

    /* 查找对应的 LittleFS 文件 */
    bridge = find_bridge(stream);
    if (bridge == NULL) {
        return EOF;
    }

    /* 调用 LittleFS 同步 */
    ret = lfs_file_sync(lfs_p, &bridge->lfs_file);
    if (ret < 0) {
        return EOF;
    }

    return 0;
}

/**
 * @brief __wrap___gcov_lock 实现
 */
void __wrap___gcov_lock()
{
    return;
}

/**
 * @brief __wrap___gcov_unlock 实现
 */
void __wrap___gcov_unlock()
{
    return;
}

/**
 * @brief getpid 桩函数
 */
int getpid(void)
{
    return 1;
}

/**
 * @brief getenv 桩函数
 */
char *getenv(const char *name)
{
    (void)name;
    return NULL;
}

/**
 * @brief fread 桩函数
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
