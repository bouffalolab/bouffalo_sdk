/**
 * @file lfs_show.c
 * @brief LittleFS 文件管理命令 - 用于 gcov 数据文件管理
 *
 * 使用 Flash 的 media 分区 (0x2E9000 - 0x3E9000, 1MB)
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
#include "lfs.h"
#include "lfs_port.h"
#include "bflb_mtd.h"
#include "bflb_flash.h"
#include "bflb_boot2.h"
#include "board.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "FreeRTOS.h"
#include "task.h"
#include "mbedtls/ssl.h"
#include "https_client.h"
#include "https_wrapper.h"
#include "http/parser.h"

#define DBG_TAG "LFS_SHOW"
#include "log.h"

/* LittleFS filesystem data */
static struct lfs_context lfs_ctx;
lfs_t *lfs_p = NULL;
uint8_t lfs_mounted = 0;

/* Flash 分区配置 */
#define LFS_PARTITION_NAME    "media"
/* Flash 参数 - 需要根据实际 Flash 调整 */
#define LFS_READ_SIZE         16
#define LFS_PROG_SIZE         16
#define LFS_BLOCK_SIZE        4096
#define LFS_BLOCK_COUNT       256   /* 1MB / 4KB = 256 */
#define LFS_CACHE_SIZE        256
#define LFS_LOOKAHEAD_SIZE    16
#define UPLOAD_SEND_RETRY_DELAY_MS 10
#define UPLOAD_SEND_MAX_RETRIES    3000

static int gcov_is_encoded_gcda_file(const char *name)
{
    size_t len;

    if (name == NULL) {
        return 0;
    }

    len = strlen(name);
    return strchr(name, '#') != NULL &&
           len > 5 &&
           strcmp(name + len - 5, ".gcda") == 0;
}

static int url_encode_component(char *dst, size_t dst_len, const char *src)
{
    static const char hex[] = "0123456789ABCDEF";
    size_t out = 0;

    if (dst_len == 0) {
        return -1;
    }

    while (*src != '\0') {
        unsigned char c = (unsigned char)*src++;
        int safe = (c >= 'A' && c <= 'Z') ||
                   (c >= 'a' && c <= 'z') ||
                   (c >= '0' && c <= '9') ||
                   c == '-' || c == '_' || c == '.' || c == '~';

        if (safe) {
            if (out + 1 >= dst_len) {
                return -1;
            }
            dst[out++] = (char)c;
        } else {
            if (out + 3 >= dst_len) {
                return -1;
            }
            dst[out++] = '%';
            dst[out++] = hex[c >> 4];
            dst[out++] = hex[c & 0x0f];
        }
    }

    dst[out] = '\0';
    return 0;
}

/* LittleFS 配置 */
static struct lfs_config lfs_cfg = {
    .context = &lfs_ctx,
    .read_size = LFS_READ_SIZE,
    .prog_size = LFS_PROG_SIZE,
    .block_size = LFS_BLOCK_SIZE,
    .block_count = LFS_BLOCK_COUNT,
    .cache_size = LFS_CACHE_SIZE,
    .lookahead_size = LFS_LOOKAHEAD_SIZE,
    .block_cycles = 500,
};

/**
 * @brief Initialize and mount LittleFS filesystem on Flash media partition
 *
 * @return int 0 on success, negative on failure
 */
int filesystem_init(void)
{
    int ret;
    bflb_mtd_handle_t handle;
    bflb_mtd_info_t info;

    LOG_I("========================================\r\n");
    LOG_I("Partition Table Debug Information\r\n");
    LOG_I("========================================\r\n");

    /* 打印所有可用的分区 */
    LOG_I("Dumping all available partitions:\r\n");
    bflb_boot2_dump();
    LOG_I("========================================\r\n");

    LOG_I("Initializing LittleFS on Flash partition '%s'...\r\n", LFS_PARTITION_NAME);

    /* 打开 Flash 分区 */
    ret = bflb_mtd_open(LFS_PARTITION_NAME, &handle, BFLB_MTD_OPEN_FLAG_BUSADDR);
    if (ret < 0) {
        LOG_E("Failed to open partition '%s' (error=%d)\r\n", LFS_PARTITION_NAME, ret);
        LOG_E("Partition '%s' not found in partition table!\r\n", LFS_PARTITION_NAME);
        LOG_E("Available partition names are listed above.\r\n");
        return -1;
    }

    /* 获取分区信息 */
    memset(&info, 0, sizeof(info));
    ret = bflb_mtd_info(handle, &info);
    if (ret < 0) {
        LOG_E("Failed to get partition info (error=%d)\r\n", ret);
        bflb_mtd_close(handle);
        return -1;
    }

    LOG_I("Partition info:\r\n");
    LOG_I("  Name: %s\r\n", info.name);
    LOG_I("  Flash offset: 0x%08x\r\n", info.offset);
    LOG_I("  Size: %u KB\r\n", info.size / 1024);
    LOG_I("  XIP addr: %p\r\n", info.xip_addr);

    /* 设置分区名称和 Flash 地址 */
    lfs_ctx.partition_name = LFS_PARTITION_NAME;
    lfs_ctx.flash_addr = info.offset;

    /* 更新 block_count 根据实际分区大小 */
    lfs_cfg.block_count = info.size / LFS_BLOCK_SIZE;

    /* 关闭 MTD handle（lfs_xip_init 会自己打开） */
    bflb_mtd_close(handle);

    /* 初始化 LittleFS */
    lfs_p = lfs_xip_init(&lfs_ctx, &lfs_cfg);
    if (lfs_p == NULL) {
        LOG_E("Failed to initialize LittleFS\r\n");
        return -1;
    }

    lfs_mounted = 1;

    LOG_I("LittleFS mounted successfully!\r\n");
    LOG_I("  Block size: %d bytes\r\n", LFS_BLOCK_SIZE);
    LOG_I("  Block count: %d\r\n", lfs_cfg.block_count);
    LOG_I("  Total size: %d KB\r\n", (lfs_cfg.block_count * LFS_BLOCK_SIZE) / 1024);

    return 0;
}

/* 文件上传上下文 - 用于 payload_cb 回调 */
typedef struct {
    lfs_file_t *file;
    uint32_t file_size;
    uint32_t bytes_sent;
    char filename[64];
} file_upload_ctx_t;

static int upload_send_all(int sock, const char *buf, int len)
{
    int total_sent = 0;
    int retry_count = 0;

    while (total_sent < len) {
        errno = 0;
        int sent = https_wrapper_send((https_wrapper_handle_t)sock,
                                      buf + total_sent,
                                      len - total_sent,
                                      0);
        if (sent > 0) {
            total_sent += sent;
            retry_count = 0;
            continue;
        }

        if (sent == 0 ||
            sent == MBEDTLS_ERR_SSL_WANT_READ ||
            sent == MBEDTLS_ERR_SSL_WANT_WRITE ||
            errno == EAGAIN ||
            errno == EWOULDBLOCK) {
            if (++retry_count > UPLOAD_SEND_MAX_RETRIES) {
                LOG_E("send timeout after %d bytes\r\n", total_sent);
                return -1;
            }
            vTaskDelay(pdMS_TO_TICKS(UPLOAD_SEND_RETRY_DELAY_MS));
            continue;
        }

        LOG_E("send failed: %d errno=%d\r\n", sent, errno);
        return -1;
    }

    return total_sent;
}

/**
 * @brief Payload 回调函数 - 从文件读取数据并发送
 *
 * @param sock Socket 包装句柄
 * @param req HTTP 请求结构
 * @param user_data 用户数据
 * @return int 发送的字节数，<0 表示错误
 */
static int file_upload_payload_cb(int sock, struct http_request *req, void *user_data)
{
    file_upload_ctx_t *ctx = (file_upload_ctx_t *)user_data;
    char io_buf[1024];
    lfs_ssize_t bytes_read;
    int total_sent = 0;

    (void)req;

    if (ctx == NULL || ctx->file == NULL) {
        LOG_E("Invalid upload context\r\n");
        return -1;
    }

    /* 循环读取并发送所有文件数据 */
    while (1) {
        bytes_read = lfs_file_read(lfs_p, ctx->file, io_buf, sizeof(io_buf));
        if (bytes_read <= 0) {
            /* 文件读取完毕或出错 */
            break;
        }

        /* 发送这一块数据，处理短写和 non-blocking socket 的临时不可写 */
        int sent = upload_send_all(sock, io_buf, bytes_read);
        if (sent < 0) {
            return -1;
        }

        total_sent += sent;
        ctx->bytes_sent += sent;

        /* 显示进度 */
        if (ctx->bytes_sent % (64 * 1024) == 0 || ctx->bytes_sent == ctx->file_size) {
            LOG_I("Uploading %s: %u / %u bytes (%.1f%%)\r\n",
                  ctx->filename, ctx->bytes_sent, ctx->file_size,
                  (ctx->bytes_sent * 100.0) / ctx->file_size);
        }
    }

    return total_sent;
}

/**
 * @brief Response 回调函数 - 处理服务器响应
 */
static void file_upload_response_cb(struct http_response *rsp,
                                    enum http_final_call final_data,
                                    void *user_data)
{
    (void)final_data;
    (void)user_data;

    if (rsp->http_status_code != 200) {
        LOG_E("Upload failed: HTTP %d %s\r\n",
              rsp->http_status_code, rsp->http_status);
    }
}

/**
 * @brief 使用 https_client API 上传单个文件
 *
 * @param url 服务器 URL
 * @param filename 文件名
 * @return int 0-成功, 负值-失败
 */
static int upload_one_file_https(const char *url, const char *filename)
{
    lfs_file_t file;
    char filepath[256];
    struct https_client_request req = {0};
    file_upload_ctx_t ctx = {0};
    char url_with_query[512];
    char encoded_name[512];
    const char *upload_name;
    int ret;
    lfs_ssize_t file_size;

    /* 构建文件路径 */
    snprintf(filepath, sizeof(filepath), "/%s", filename);

    /* 打开文件 */
    ret = lfs_file_open(lfs_p, &file, filepath, LFS_O_RDONLY);
    if (ret < 0) {
        LOG_E("Failed to open %s (error=%d)\r\n", filepath, ret);
        return -1;
    }

    /* 获取文件大小 */
    file_size = lfs_file_size(lfs_p, &file);
    if (file_size < 0) {
        LOG_E("Failed to get file size\r\n");
        lfs_file_close(lfs_p, &file);
        return -1;
    }

    upload_name = filename;

    /* 初始化上传上下文 */
    ctx.file = &file;
    ctx.file_size = file_size;
    ctx.bytes_sent = 0;
    strncpy(ctx.filename, upload_name, sizeof(ctx.filename) - 1);
    ctx.filename[sizeof(ctx.filename) - 1] = '\0';

    if (url_encode_component(encoded_name, sizeof(encoded_name), upload_name) < 0) {
        LOG_E("Encoded filename too long: %s\r\n", upload_name);
        lfs_file_close(lfs_p, &file);
        return -1;
    }

    /* 构建带查询参数的 URL */
    snprintf(url_with_query, sizeof(url_with_query),
             "%s%sfilename=%s",
             url,
             (strchr(url, '?') ? "&" : "?"),
             encoded_name);

    /* 设置 HTTP 请求 */
    memset(&req, 0, sizeof(req));
    req.method = HTTP_POST;
    req.url = url_with_query;
    req.protocol = "HTTP/1.1";
    req.payload_cb = file_upload_payload_cb;
    req.payload_len = ctx.file_size;
    req.response = file_upload_response_cb;

    /* 设置请求头 */
    const char *headers[] = {
        "Content-Type: application/octet-stream\r\n",
        "Connection: close\r\n",
        NULL
    };
    req.header_fields = headers;

    LOG_I("Starting upload: %s (%d bytes)\r\n", filename, ctx.file_size);

    /* 执行 HTTP 请求 */
    ret = https_client_request(&req, 30000, &ctx);
    lfs_file_close(lfs_p, &file);

    if (ret < 0) {
        LOG_E("https_client_request failed: %d\r\n", ret);
        return -1;
    }

    LOG_I("Upload complete: %s (%u bytes sent)\r\n", upload_name, ctx.bytes_sent);
    return 0;
}

/**
 * @brief 使用 https_client API 上传所有 gcda 文件
 *
 * 用法: gcov_upload <url>
 * 示例: gcov_upload http://192.168.1.100:8080/upload
 *
 * @param argc 参数个数
 * @param argv 参数数组
 * @return int 0-成功, 负值-失败
 */
static int cmd_gcov_upload(int argc, char **argv)
{
    lfs_dir_t dir;
    struct lfs_info info;
    const char *url;
    int ok = 0;
    int fail = 0;
    int ret;

    if (argc < 2) {
        LOG_E("Usage: gcov_upload <url>\r\n");
        LOG_E("Example: gcov_upload http://192.168.1.100:8080/upload\r\n");
        return -1;
    }

    url = argv[1];

    /* 检查文件系统 */
    if (!lfs_mounted) {
        LOG_E("LittleFS not mounted. Please run initialization first.\r\n");
        return -1;
    }

    /* 打开根目录 */
    ret = lfs_dir_open(lfs_p, &dir, "/");
    if (ret < 0) {
        LOG_E("Failed to open root directory (error=%d)\r\n", ret);
        return -1;
    }

    LOG_I("\r\n");
    LOG_I("=================================================\r\n");
    LOG_I("GCOV Upload via HTTPS Client API\r\n");
    LOG_I("Target URL: %s\r\n", url);
    LOG_I("=================================================\r\n");

    /* 遍历目录，上传所有带 # 路径编码的 .gcda 文件 */
    while (1) {
        ret = lfs_dir_read(lfs_p, &dir, &info);
        if (ret <= 0) {
            break;
        }

        /* 跳过 . 和 .. */
        if (strcmp(info.name, ".") == 0 || strcmp(info.name, "..") == 0) {
            continue;
        }

        /* 只处理文件（跳过目录） */
        if (info.type == LFS_TYPE_REG) {
            /* 只上传带 # 路径编码的新格式 .gcda 文件 */
            if (gcov_is_encoded_gcda_file(info.name)) {
                /* 上传文件 */
                if (upload_one_file_https(url, info.name) == 0) {
                    ok++;
                } else {
                    fail++;
                }
            }
        }
    }

    lfs_dir_close(lfs_p, &dir);

    LOG_I("=================================================\r\n");
    LOG_I("Upload Summary:\r\n");
    LOG_I("  Success: %d\r\n", ok);
    LOG_I("  Failed:  %d\r\n", fail);
    LOG_I("=================================================\r\n");
    LOG_I("\r\n");

    return (ok > 0 && fail == 0) ? 0 : -1;
}

/* 注册 shell 命令 */
SHELL_CMD_EXPORT_ALIAS(cmd_gcov_upload, gcov_upload, Upload gcda files via HTTP);

/**
 * @brief 列出 LittleFS 根目录的文件
 *
 * @param argc 参数个数
 * @param argv 参数数组
 * @return int 0-成功, 负值-失败
 */
static int cmd_ls(int argc, char **argv)
{
    lfs_dir_t dir;
    struct lfs_info info;
    int ret;

    (void)argc;
    (void)argv;

    if (!lfs_mounted) {
        LOG_E("LittleFS not mounted\r\n");
        return -1;
    }

    ret = lfs_dir_open(lfs_p, &dir, "/");
    if (ret < 0) {
        LOG_E("Failed to open directory (error=%d)\r\n", ret);
        return -1;
    }

    LOG_I("\r\n");
    LOG_I("LittleFS Root Directory:\r\n");
    LOG_I("======================\r\n");

    while (1) {
        ret = lfs_dir_read(lfs_p, &dir, &info);
        if (ret <= 0) {
            break;
        }

        /* 跳过 . 和 .. */
        if (strcmp(info.name, ".") == 0 || strcmp(info.name, "..") == 0) {
            continue;
        }

        /* 显示文件/目录信息 */
        if (info.type == LFS_TYPE_DIR) {
            LOG_I("  %s/\r\n", info.name);
        } else {
            LOG_I("  %-30s %10u bytes\r\n", info.name, info.size);
        }
    }

    lfs_dir_close(lfs_p, &dir);
    LOG_I("======================\r\n\r\n");

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_ls, ls, List files in LittleFS);
