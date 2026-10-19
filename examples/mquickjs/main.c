/*
 * MQuickJS on Bouffalo SDK
 *
 * JavaScript engine for embedded systems running on BL616/BL618.
 * Uses FreeRTOS task for thread-safe JS VM execution.
 *
 * Copyright (c) 2017-2025 Fabrice Bellard
 * Copyright (c) 2017-2025 Charlie Gordon
 * Bouffalo Lab SDK integration
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "board.h"
#include "shell.h"
#include "bflb_core.h"
#include "lfs.h"
#include "newlib/port_file_littlefs.h"
#include "bouffalo_mquickjs.h"

#include <FreeRTOS.h>
#include "task.h"
#include "semphr.h"

/* ============== LittleFS Configuration ============== */

#define LFS_MOUNT_POINT         "/lfs"

static const char *lfs_partitions[] = {
    "media",
    "PSM",
};

/* LittleFS instances */
static lfs_t lfs_instance;
static struct lfs_port_context lfs_ctx;
static struct lfs_config lfs_cfg;

/* ============== FreeRTOS Hooks ============== */

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    printf("\r\n[STACK OVERFLOW] %s\r\n", pcTaskName);
    while (1);
}

void vApplicationMallocFailedHook(void)
{
    printf("\r\n[MALLOC FAILED]\r\n");
    while (1);
}

/* ============== LittleFS Helper Functions ============== */

/**
 * @brief Make full path from user input (supports CWD for relative paths)
 */
static void lfs_make_path(char *dest, size_t dest_size, const char *input)
{
    char cwd[256];

    if (strncmp(input, LFS_MOUNT_POINT, strlen(LFS_MOUNT_POINT)) == 0) {
        /* Already has mount point prefix - absolute path */
        strncpy(dest, input, dest_size - 1);
        dest[dest_size - 1] = '\0';
    } else if (input[0] == '/') {
        /* Absolute path within LFS - add mount point */
        snprintf(dest, dest_size, "%s%s", LFS_MOUNT_POINT, input);
    } else {
        /* Relative path - use current working directory */
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            snprintf(dest, dest_size, "%s/%s", cwd, input);
        } else {
            /* Fallback to mount point if getcwd fails */
            snprintf(dest, dest_size, "%s/%s", LFS_MOUNT_POINT, input);
        }
    }
}

/* ============== LittleFS Shell Commands ============== */

/**
 * @brief Shell command to list files using native LFS API
 */
static void cmd_lfs_ls(int argc, char **argv)
{
    lfs_dir_t dir;
    struct lfs_info info;
    char cwd[256];
    const char *path;

    if (argc > 1) {
        path = argv[1];
        /* Skip mount point prefix if provided */
        if (strncmp(path, LFS_MOUNT_POINT, strlen(LFS_MOUNT_POINT)) == 0) {
            path += strlen(LFS_MOUNT_POINT);
            if (*path == '\0') {
                path = "/";
            }
        }
    } else {
        /* Use current working directory */
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            /* Strip mount point prefix for native LFS API */
            if (strncmp(cwd, LFS_MOUNT_POINT, strlen(LFS_MOUNT_POINT)) == 0) {
                path = cwd + strlen(LFS_MOUNT_POINT);
                if (*path == '\0') {
                    path = "/";
                }
            } else {
                path = "/";
            }
        } else {
            path = "/";
        }
    }

    if (lfs_dir_open(&lfs_instance, &dir, path) != 0) {
        printf("Failed to open: %s\r\n", path);
        return;
    }

    printf("Listing %s:\r\n", path);
    while (lfs_dir_read(&lfs_instance, &dir, &info) > 0) {
        if (info.type == LFS_TYPE_DIR) {
            printf("  [DIR]  %s/\r\n", info.name);
        } else {
            printf("  [FILE] %s (%u bytes)\r\n", info.name, (unsigned)info.size);
        }
    }
    lfs_dir_close(&lfs_instance, &dir);
}
SHELL_CMD_EXPORT_ALIAS(cmd_lfs_ls, lfs_ls, List LittleFS files);

/**
 * @brief Shell command to read a file
 */
static void cmd_lfs_cat(int argc, char **argv)
{
    FILE *fp;
    char buf[128];
    char path[256];

    if (argc < 2) {
        printf("Usage: lfs_cat <filename>\r\n");
        return;
    }

    lfs_make_path(path, sizeof(path), argv[1]);
    fp = fopen(path, "r");
    if (fp == NULL) {
        printf("Failed to open: %s\r\n", path);
        return;
    }

    while (fgets(buf, sizeof(buf), fp) != NULL) {
        printf("%s", buf);
    }
    fclose(fp);
}
SHELL_CMD_EXPORT_ALIAS(cmd_lfs_cat, lfs_cat, Read LittleFS file);

/**
 * @brief Shell command to write a file
 */
static void cmd_lfs_write(int argc, char **argv)
{
    FILE *fp;
    char path[256];

    if (argc < 3) {
        printf("Usage: lfs_write <filename> <content>\r\n");
        return;
    }

    lfs_make_path(path, sizeof(path), argv[1]);
    fp = fopen(path, "w");
    if (fp == NULL) {
        printf("Failed to open: %s\r\n", path);
        return;
    }

    fprintf(fp, "%s\n", argv[2]);
    fclose(fp);
    printf("Written to %s\r\n", path);
}
SHELL_CMD_EXPORT_ALIAS(cmd_lfs_write, lfs_write, Write LittleFS file);

/**
 * @brief Shell command to delete a file
 */
static void cmd_lfs_rm(int argc, char **argv)
{
    char path[256];

    if (argc < 2) {
        printf("Usage: lfs_rm <filename>\r\n");
        return;
    }

    lfs_make_path(path, sizeof(path), argv[1]);
    if (remove(path) == 0) {
        printf("Deleted: %s\r\n", path);
    } else {
        printf("Failed to delete: %s\r\n", path);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_lfs_rm, lfs_rm, Delete LittleFS file);

/**
 * @brief Shell command to create directory
 */
static void cmd_lfs_mkdir(int argc, char **argv)
{
    char path[256];

    if (argc < 2) {
        printf("Usage: lfs_mkdir <dirname>\r\n");
        return;
    }

    lfs_make_path(path, sizeof(path), argv[1]);
    if (mkdir(path, 0755) == 0) {
        printf("Created: %s\r\n", path);
    } else {
        printf("Failed to create: %s\r\n", path);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_lfs_mkdir, lfs_mkdir, Create LittleFS directory);

/**
 * @brief Shell command to show filesystem info
 */
static void cmd_lfs_info(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("LittleFS Info:\r\n");
    printf("  Partition: %s\r\n", lfs_ctx.partition_name ? lfs_ctx.partition_name : "unknown");
    printf("  Mount point: %s\r\n", LFS_MOUNT_POINT);
    printf("  Flash address: 0x%08x\r\n", lfs_ctx.flash_addr);
    printf("  Partition size: %u bytes (%u KB)\r\n",
           lfs_ctx.flash_size, lfs_ctx.flash_size / 1024);
    printf("  Block size: %u bytes\r\n", lfs_cfg.block_size);
    printf("  Block count: %u\r\n", lfs_cfg.block_count);
}
SHELL_CMD_EXPORT_ALIAS(cmd_lfs_info, lfs_info, Show LittleFS info);

/**
 * @brief Shell command to change directory
 */
static void cmd_lfs_cd(int argc, char **argv)
{
    if (argc < 2) {
        printf("%s\r\n", LFS_MOUNT_POINT);
        return;
    }

    char path[256];
    if (argv[1][0] == '/') {
        /* Absolute path */
        strncpy(path, argv[1], sizeof(path) - 1);
    } else {
        /* Relative path - prepend mount point */
        snprintf(path, sizeof(path), "%s/%s", LFS_MOUNT_POINT, argv[1]);
    }

    if (chdir(path) == 0) {
        printf("Changed to: %s\r\n", path);
    } else {
        printf("Failed to change to: %s\r\n", path);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_lfs_cd, lfs_cd, Change LittleFS directory);

/**
 * @brief Shell command to print working directory
 */
static void cmd_lfs_pwd(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\r\n", cwd);
    } else {
        printf("Failed to get current directory\r\n");
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_lfs_pwd, lfs_pwd, Print LittleFS working directory);

/* ============== Main Entry Point ============== */

int main(void)
{
    int ret = -1;
    const char *mounted_partition = NULL;

    board_init();

    printf("\r\n========================================\r\n");
    printf("  MQuickJS on Bouffalo SDK\r\n");
    printf("  JavaScript for Embedded Systems\r\n");
    printf("========================================\r\n");

    /* Initialize LittleFS with partition and POSIX registration */
    printf("\r\nInitializing LittleFS...\r\n");

    for (size_t i = 0; i < sizeof(lfs_partitions) / sizeof(lfs_partitions[0]); i++) {
        ret = lfs_port_full_init(&lfs_instance, &lfs_ctx, &lfs_cfg,
                                 lfs_partitions[i], LFS_MOUNT_POINT);
        if (ret == 0) {
            mounted_partition = lfs_partitions[i];
            break;
        }
    }

    if (ret != 0) {
        printf("FATAL: LittleFS initialization failed\r\n");
        while (1) {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
    printf("LittleFS mounted at %s (partition: %s)\r\n", LFS_MOUNT_POINT, mounted_partition);

    /* Initialize MQuickJS (JS queue, VM task, JS commands) */
    bouffalo_mquickjs_init();

    printf("\r\nLFS Commands:\r\n");
    printf("  lfs_ls [path]  - List files\r\n");
    printf("  lfs_cat <file> - Read file\r\n");
    printf("  lfs_write <file> <content> - Write file\r\n");
    printf("  lfs_rm <file>  - Delete file\r\n");
    printf("  lfs_mkdir <dir> - Create directory\r\n");
    printf("  lfs_info       - Show filesystem info\r\n");
    printf("  lfs_cd <dir>   - Change directory\r\n");
    printf("  lfs_pwd        - Print working directory\r\n");
    printf("\r\n");

    /* Start shell */
    struct bflb_device_s *uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    vTaskStartScheduler();

    while (1);
}
