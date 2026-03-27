/**
 * @file fatfs_show.c
 * @brief FatFS 文件显示命令 - 用于查看 gcov 数据文件
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

#include "shell.h"
#include "ff.h"
#include "fatfs_diskio_register.h"
#include "board.h"
#include <stdio.h>
#include <string.h>

#define DBG_TAG "FATFS_SHOW"
#include "log.h"

/* 文件系统状态 */
static uint8_t fatfs_initialized = 0;

/* FATFS filesystem data */
static FATFS fs;
static uint8_t fatfs_mounted = 0;
__attribute((aligned(64))) static uint32_t workbuf[FF_MAX_SS * 4];

static MKFS_PARM fs_para = {
    .fmt = FM_FAT32,     /* Format option (FM_FAT, FM_FAT32, FM_EXFAT and FM_SFD) */
    .n_fat = 0,          /* Number of FATs */
    .align = 0,          /* Data area alignment (sector) */
    .n_root = 0,         /* Number of root directory entries */
    .au_size = 512 * 32, /* Cluster size (byte) */
};

/**
 * @brief Initialize and mount FATFS filesystem
 *
 * @return int 0 on success, negative on failure
 */
int filesystem_init(void)
{
    FRESULT ret;

    LOG_I("Initializing FATFS filesystem...\r\n");

    /* Initialize SD card GPIO pins */
    board_sdh_gpio_init();

    /* Register FATFS SDH driver */
    fatfs_sdh_driver_register();

    /* Mount filesystem */
    ret = f_mount(&fs, "/sd", 1);

    if (ret == FR_NO_FILESYSTEM) {
        LOG_W("No filesystem found, formatting SD card...\r\n");

        ret = f_mkfs("/sd", &fs_para, workbuf, sizeof(workbuf));
        if (ret != FR_OK) {
            LOG_E("Failed to format filesystem (error=%d)\r\n", ret);
            return -1;
        }

        LOG_I("Format successful. Mounting again...\r\n");

        ret = f_mount(&fs, "/sd", 1);
    }

    if (ret == FR_OK) {
        fatfs_mounted = 1;
        LOG_I("FATFS mounted successfully!\r\n");
        LOG_I("  Type: %s\r\n", fs.fs_type == 1 ? "FAT12" :
                                  fs.fs_type == 2 ? "FAT16" :
                                  fs.fs_type == 3 ? "FAT32" :
                                  fs.fs_type == 4 ? "exFAT" :
                                                    "unknown");
        LOG_I("  Volume size: %d MB\r\n",
              (uint32_t)((uint64_t)(fs.n_fatent - 2) * fs.csize * 512 / 1024 / 1024));
        LOG_I("  Cluster size: %d sectors (%d bytes)\r\n", fs.csize, fs.csize * 512);
        return 0;
    } else {
        LOG_E("Failed to mount filesystem (error=%d)\r\n", ret);
        LOG_E("SD card might not be inserted or failed to initialize.\r\n");
        return -1;
    }
}

/**
 * @brief Unmount FATFS filesystem
 */
void filesystem_unmount(void)
{
    if (fatfs_mounted) {
        f_unmount("/sd");
        fatfs_mounted = 0;
        LOG_I("FATFS unmounted.\r\n");
    }
}

/**
 * @brief 检查文件是否为 gcov 相关文件
 *
 * @param filename 文件名
 * @return int 1-是gcov文件, 0-不是
 */
static int is_gcov_file(const char *filename)
{
    if (strstr(filename, ".gcda") || strstr(filename, ".gcno") ||
        strstr(filename, ".gcov")) {
        return 1;
    }
    return 0;
}

/**
 * @brief 格式化文件大小显示
 *
 * @param size 文件大小（字节）
 * @param buf 输出缓冲区
 * @param buf_size 缓冲区大小
 */
static void format_file_size(uint64_t size, char *buf, size_t buf_size)
{
    if (size >= 1024 * 1024) {
        snprintf(buf, buf_size, "%.2f MB", (double)size / (1024 * 1024));
    } else if (size >= 1024) {
        snprintf(buf, buf_size, "%.2f KB", (double)size / 1024);
    } else {
        snprintf(buf, buf_size, "%u B", (unsigned int)size);
    }
}

/**
 * @brief 初始化 FatFS 文件系统命令
 *
 * 用法: fatfs_init
 *
 * @param argc 参数个数
 * @param argv 参数数组
 * @return int 0-成功, 负值-失败
 */
static int cmd_fatfs_init(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    LOG_I("\r\n");
    LOG_I("=================================================\r\n");
    LOG_I("Initializing FatFS filesystem...\r\n");
    LOG_I("=================================================\r\n");

    if (fatfs_initialized) {
        LOG_W("FatFS already initialized.\r\n");
        LOG_I("Use 'fatfs_unmount' to unmount first if needed.\r\n");
        return 0;
    }

    if (filesystem_init() < 0) {
        LOG_E("Failed to initialize FatFS.\r\n");
        LOG_E("Please check:\r\n");
        LOG_E("  1. SD card is inserted properly\r\n");
        LOG_E("  2. SD card is formatted (FAT32/exFAT)\r\n");
        LOG_E("  3. GPIO pins are configured correctly\r\n");
        return -1;
    }

    fatfs_initialized = 1;
    LOG_I("=================================================\r\n");
    LOG_I("FatFS initialized successfully!\r\n");
    LOG_I("Use 'show' command to list files.\r\n");
    LOG_I("=================================================\r\n");
    LOG_I("\r\n");

    return 0;
}

/**
 * @brief 卸载 FatFS 文件系统命令
 *
 * 用法: fatfs_unmount
 *
 * @param argc 参数个数
 * @param argv 参数数组
 * @return int 0-成功, 负值-失败
 */
static int cmd_fatfs_unmount(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    LOG_I("\r\n");
    LOG_I("=================================================\r\n");
    LOG_I("Unmounting FatFS filesystem...\r\n");
    LOG_I("=================================================\r\n");

    if (!fatfs_initialized) {
        LOG_W("FatFS not initialized.\r\n");
        return 0;
    }

    filesystem_unmount();
    fatfs_initialized = 0;

    LOG_I("FatFS unmounted successfully.\r\n");
    LOG_I("=================================================\r\n");
    LOG_I("\r\n");

    return 0;
}

/**
 * @brief 显示 FatFS 目录内容命令
 *
 * 用法: show [path]
 *
 * @param argc 参数个数
 * @param argv 参数数组
 * @return int 0-成功, 负值-失败
 */
static int cmd_fatfs_show(int argc, char **argv)
{
    FRESULT res;
    DIR dir;
    FILINFO fno;
    char *path = "/sd";  /* 默认路径 */
    uint32_t total_files = 0;
    uint32_t total_size = 0;
    uint32_t gcov_files = 0;
    uint32_t gcov_size = 0;

    /* 解析路径参数 */
    if (argc >= 2) {
        path = argv[1];
    }

    LOG_I("\r\n");
    LOG_I("=================================================\r\n");
    LOG_I("FatFS Directory: %s\r\n", path);
    LOG_I("=================================================\r\n");

    /* 打开目录 */
    res = f_opendir(&dir, path);
    if (res != FR_OK) {
        LOG_E("Failed to open directory '%s' (error=%d)\r\n", path, res);
        LOG_E("Please ensure:\r\n");
        LOG_E("  1. SD card is inserted\r\n");
        LOG_E("  2. FatFS is mounted\r\n");
        LOG_E("  3. Path is correct\r\n");
        return -1;
    }

    LOG_I("%-30s %-12s %-10s\r\n", "Filename", "Size", "Type");
    LOG_I("-------------------------------------------------\r\n");

    /* 遍历目录 */
    while (1) {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0) {
            break;  /* 结束或错误 */
        }

        /* 跳过 . 和 .. */
        if (strcmp(fno.fname, ".") == 0 || strcmp(fno.fname, "..") == 0) {
            continue;
        }

        total_files++;

        /* 判断文件类型 */
        if (fno.fattrib & AM_DIR) {
            LOG_I("%-30s %-12s <DIR>\r\n", fno.fname, "-");
        } else {
            char size_str[32];
            format_file_size(fno.fsize, size_str, sizeof(size_str));
            total_size += fno.fsize;

            /* 检查是否为 gcov 文件 */
            if (is_gcov_file(fno.fname)) {
                gcov_files++;
                gcov_size += fno.fsize;
                LOG_I("%-30s %-12s [GCOV]\r\n", fno.fname, size_str);
            } else {
                LOG_I("%-30s %-12s\r\n", fno.fname, size_str);
            }
        }
    }

    /* 关闭目录 */
    f_closedir(&dir);

    LOG_I("-------------------------------------------------\r\n");
    LOG_I("Summary:\r\n");
    LOG_I("  Total files: %u\r\n", total_files);
    LOG_I("  Total size: ");

    /* 显示总大小 */
    if (total_size >= 1024 * 1024) {
        LOG_I("%.2f MB\r\n", (double)total_size / (1024 * 1024));
    } else if (total_size >= 1024) {
        LOG_I("%.2f KB\r\n", (double)total_size / 1024);
    } else {
        LOG_I("%u B\r\n", (unsigned int)total_size);
    }

    /* 显示 gcov 统计 */
    if (gcov_files > 0) {
        LOG_I("\r\n");
        LOG_I("GCOV Files:\r\n");
        LOG_I("  Count: %u\r\n", gcov_files);
        LOG_I("  Size: ");

        if (gcov_size >= 1024 * 1024) {
            LOG_I("%.2f MB\r\n", (double)gcov_size / (1024 * 1024));
        } else if (gcov_size >= 1024) {
            LOG_I("%.2f KB\r\n", (double)gcov_size / 1024);
        } else {
            LOG_I("%u B\r\n", (unsigned int)gcov_size);
        }
    } else {
        LOG_I("\r\n");
        LOG_I("No GCOV files found in directory.\r\n");
    }

    LOG_I("=================================================\r\n");
    LOG_I("\r\n");

    return 0;
}

/**
 * @brief 清除 FatFS 中的文件
 *
 * 用法: rm <filename>
 *
 * @param argc 参数个数
 * @param argv 参数数组
 * @return int 0-成功, 负值-失败
 */
static int cmd_fatfs_rm(int argc, char **argv)
{
    FRESULT res;
    char *path = "/sd";  /* 默认路径 */
    char filepath[256];

    if (argc < 2) {
        LOG_E("Usage: rm <filename>\r\n");
        LOG_E("Example: rm test.gcda\r\n");
        return -1;
    }

    /* 构建完整路径 */
    if (argv[1][0] == '/') {
        /* 绝对路径 */
        snprintf(filepath, sizeof(filepath), "%s", argv[1]);
    } else {
        /* 相对路径 */
        snprintf(filepath, sizeof(filepath), "%s/%s", path, argv[1]);
    }

    /* 删除文件 */
    res = f_unlink(filepath);
    if (res != FR_OK) {
        LOG_E("Failed to delete '%s' (error=%d)\r\n", filepath, res);
        return -1;
    }

    LOG_I("Successfully deleted: %s\r\n", filepath);
    return 0;
}

/**
 * @brief 显示文件内容
 *
 * 用法: cat <filename>
 *
 * @param argc 参数个数
 * @param argv 参数数组
 * @return int 0-成功, 负值-失败
 */
static int cmd_fatfs_cat(int argc, char **argv)
{
    FRESULT res;
    FIL file;
    char *path = "/sd";  /* 默认路径 */
    char filepath[256];
    char buffer[128];
    UINT bytes_read;
    UINT total_bytes = 0;

    if (argc < 2) {
        LOG_E("Usage: cat <filename>\r\n");
        LOG_E("Example: cat test.txt\r\n");
        return -1;
    }

    /* 构建完整路径 */
    if (argv[1][0] == '/') {
        snprintf(filepath, sizeof(filepath), "%s", argv[1]);
    } else {
        snprintf(filepath, sizeof(filepath), "%s/%s", path, argv[1]);
    }

    /* 打开文件 */
    res = f_open(&file, filepath, FA_READ);
    if (res != FR_OK) {
        LOG_E("Failed to open '%s' (error=%d)\r\n", filepath, res);
        return -1;
    }

    LOG_I("\r\n");
    LOG_I(">>> %s <<<\r\n", filepath);
    LOG_I("-------------------------------------------------\r\n");

    /* 读取并显示文件内容 */
    while (1) {
        res = f_read(&file, buffer, sizeof(buffer) - 1, &bytes_read);
        if (res != FR_OK || bytes_read == 0) {
            break;
        }

        buffer[bytes_read] = '\0';
        printf("%s", buffer);
        total_bytes += bytes_read;
    }

    LOG_I("\r\n");
    LOG_I("-------------------------------------------------\r\n");
    LOG_I("Total bytes: %u\r\n", total_bytes);
    LOG_I("\r\n");

    f_close(&file);
    return 0;
}

/* 注册 shell 命令 */
SHELL_CMD_EXPORT_ALIAS(cmd_fatfs_init, fatfs_init, Initialize FatFS filesystem);
SHELL_CMD_EXPORT_ALIAS(cmd_fatfs_unmount, fatfs_unmount, Unmount FatFS filesystem);
SHELL_CMD_EXPORT_ALIAS(cmd_fatfs_show, show, Show FatFS directory contents);
SHELL_CMD_EXPORT_ALIAS(cmd_fatfs_rm, rm, Remove file from FatFS);
SHELL_CMD_EXPORT_ALIAS(cmd_fatfs_cat, cat, Display file contents);
