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
#include "wifi_ota_internal.h"
#include "wifi_ota.h"
#include "bflb_flash.h"
#include "bflb_multi_core_sync.h"
#include "partition.h"
#include "bl_sys.h"

#ifdef CONFIG_IPC
#include <flash_ops_rpmsg.h>
#endif

#define DBG_TAG "OTA"
#include "log.h"

bool g_ota_initialized = false;
bool g_ota_in_progress = false;
static uint8_t *g_recv_buffer = NULL;

static pt_table_id_type wifi_ota_init_partition(pt_table_stuff_config pt_table_stuff[2]);

static int wifi_ota_prepare_flash(pt_table_id_type active_id, pt_table_stuff_config pt_table_stuff[],
                                  wifi_ota_context_t *ctx);

static int wifi_ota_connect_server(const char *server_ip, const char *server_port, int *sock);

static int wifi_ota_download_header(int sock, uint32_t *bin_size,

                                    uint8_t sha256_img[32], int *use_xz, unsigned int *p_buffer_offset);

static int wifi_ota_download_firmware(int sock, uint32_t ota_addr, uint32_t bin_size, const uint8_t sha256_img[32],
                                      unsigned int initial_buffer_offset);

static int wifi_ota_update_partition_table(pt_table_id_type active_id, pt_table_stuff_config pt_table_stuff[],
                                           const wifi_ota_context_t *ctx);

static void wifi_ota_cleanup(int sock, bool free_buffer);

int wifi_ota_check_header(ota_header_t *ota_header, uint32_t *ota_len, int *use_xz)
{
    char str[33];

    memcpy(str, ota_header->u.s.header, sizeof(ota_header->u.s.header));
    str[sizeof(ota_header->u.s.header)] = '\0';
    LOG_I("ota header is %s\r\n", str);

    memcpy(str, ota_header->u.s.type, sizeof(ota_header->u.s.type));
    str[sizeof(ota_header->u.s.type)] = '\0';
    LOG_I("file type is %s\r\n", str);

    if (strstr(str, "XZ")) {
        *use_xz = 1;
    } else {
        *use_xz = 0;
    }

    if ((!strcmp(str, "RAW")) && (*use_xz != 1)) {
        return 1;
    }

    memcpy(ota_len, &(ota_header->u.s.len), 4);
    LOG_I("file length (exclude ota header) is %lu\r\n", *ota_len);

    memcpy(str, ota_header->u.s.ver_hardware, sizeof(ota_header->u.s.ver_hardware));
    str[sizeof(ota_header->u.s.ver_hardware)] = '\0';
    LOG_I("ver_hardware is %s\r\n", str);

    memcpy(str, ota_header->u.s.ver_software, sizeof(ota_header->u.s.ver_software));
    str[sizeof(ota_header->u.s.ver_software)] = '\0';
    LOG_I("ver_software is %s\r\n", str);

    memcpy(str, ota_header->u.s.sha256, sizeof(ota_header->u.s.sha256));
    str[sizeof(ota_header->u.s.sha256)] = '\0';
    LOG_I("sha256 is ");
    for (int i = 0; i < sizeof(ota_header->u.s.sha256); i++) {
        printf("%02X", str[i]);
    }
    printf("\r\n");

    return 0;
}

static pt_table_id_type wifi_ota_init_partition(pt_table_stuff_config pt_table_stuff[2])
{
    pt_table_id_type active_id;

    LOG_I("Initializing partition table\r\n");

    /* Set flash operations based on configuration */
#ifdef CONFIG_IPC
#ifdef CONFIG_RPMSG_SERVICE_MODE_MASTER
    /* AP Core: Use bflb_flash_mcs_* with suspend/resume */
    pt_table_set_flash_operation(bflb_flash_erase_mcs, bflb_flash_write_mcs, bflb_flash_read_mcs);
#else
    /* NP Core: Use RPMsg to communicate with AP */
    pt_table_set_flash_operation(bflb_flash_erase_rpmsg, bflb_flash_write_rpmsg, bflb_flash_read_rpmsg);
#endif
#else
    /* Single-core mode: Direct flash operations */
    pt_table_set_flash_operation(bflb_flash_erase, bflb_flash_write, bflb_flash_read);
#endif

    LOG_I("Getting active partition\r\n");
    active_id = pt_table_get_active_partition_need_lock(pt_table_stuff);
    if (PT_TABLE_ID_INVALID == active_id) {
        LOG_E("No valid PT\r\n");
        LOG_E("PT0 magicCode: 0x%08x (expected: 0x%08x)\r\n", pt_table_stuff[0].pt_table.magicCode, 0x54504642);
        LOG_E("PT1 magicCode: 0x%08x (expected: 0x%08x)\r\n", pt_table_stuff[1].pt_table.magicCode, 0x54504642);
        return PT_TABLE_ID_INVALID;
    }

    LOG_I("Active partition ID: %d\r\n", active_id);
    return active_id;
}

static int wifi_ota_prepare_flash(pt_table_id_type active_id, pt_table_stuff_config pt_table_stuff[],
                                  wifi_ota_context_t *ctx)
{
    if (pt_table_get_active_entries_by_name(&pt_table_stuff[active_id], (uint8_t *)OTA_PARTITION_NAME_TYPE_FW,
                                            &ctx->entry)) {
        if (pt_table_get_active_entries_by_name(&pt_table_stuff[active_id], (uint8_t *)OTA_PARTITION_NAME_TYPE_FW0,
                                                &ctx->entry)) {
            LOG_E("PtTable get active entry fail!\r\n");
            return OTA_ERR_PARTITION;
        }
    }

    ctx->ota_addr = ctx->entry.start_address[!ctx->entry.active_index];
    ctx->bin_size = ctx->entry.max_len[!ctx->entry.active_index];
    ctx->active_addr = ctx->entry.start_address[ctx->entry.active_index];

    LOG_I("activeIndex is %u, use OTA address=%08lx\r\n", ctx->entry.active_index, ctx->ota_addr);
    LOG_I("Starting OTA test. OTA size is %lu\r\n", ctx->bin_size);

    if (ctx->ota_addr == ctx->active_addr) {
        LOG_E("OTA target is same as active partition!\r\n");
        return OTA_ERR_PARTITION;
    }

    if (!g_recv_buffer) {
        g_recv_buffer = pvPortMalloc(OTA_PROGRAM_SIZE);
        if (!g_recv_buffer) {
            LOG_E("Failed to allocate OTA buffer\r\n");
            return OTA_ERR_FLASH;
        }
    } else {
        memset(g_recv_buffer, 0, OTA_PROGRAM_SIZE);
    }

    return OTA_OK;
}

static int wifi_ota_connect_server(const char *server_ip, const char *server_port, int *sock)
{
    struct sockaddr_in remote_addr;
    struct hostent *hostinfo;
    int max_retry = 10;
    int retry_delay_ms = 1000;
    bool connected = false;

    if (!server_ip || !server_port) {
        LOG_E("Server IP and port not specified\r\n");
        return OTA_ERR_INVALID_PARAM;
    }

    hostinfo = gethostbyname(server_ip);
    if (!hostinfo) {
        LOG_E("gethostbyname Failed\r\n");
        return OTA_ERR_NETWORK;
    }

    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(atoi(server_port));
    remote_addr.sin_addr = *((struct in_addr *)hostinfo->h_addr);

    char *ip_str = inet_ntoa(remote_addr.sin_addr.s_addr);
    LOG_I("Server ip Address : %s:%s\r\n", ip_str, server_port);

    *sock = -1;

    for (int retry = 0; retry < max_retry; retry++) {
        LOG_I("Attempting connection %d/%d to %s:%s\r\n", retry + 1, max_retry, ip_str, server_port);
        *sock = socket(AF_INET, SOCK_STREAM, 0);
        if (*sock < 0) {
            LOG_E("Create socket failed, retry %d/%d\r\n", retry + 1, max_retry);
        } else {
            if (connect(*sock, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == 0) {
                LOG_I("ota test connect server success!\r\n");
                connected = true;
                break;
            }
            LOG_W("Connection attempt %d failed\r\n", retry + 1);
            closesocket(*sock);
            *sock = -1;
        }

        if (retry < max_retry - 1) {
            LOG_I("Retrying in %d ms\r\n", retry_delay_ms);
            vTaskDelay(retry_delay_ms);
        }
    }

    if (!connected) {
        LOG_E("Failed to connect after %d attempts\r\n", max_retry);
        return OTA_ERR_NETWORK;
    }

    return OTA_OK;
}

static int wifi_ota_download_header(int sock, uint32_t *bin_size, uint8_t sha256_img[32], int *use_xz,
                                    unsigned int *p_buffer_offset)
{
    int ret;
    ota_header_t *ota_header;

    while (*p_buffer_offset < OTA_HEADER_SIZE) {
        ret = read(sock, g_recv_buffer + *p_buffer_offset, OTA_PROGRAM_SIZE - *p_buffer_offset);
        if (ret < 0) {
            LOG_E("read error ret=%d, err=%d\r\n", ret, errno);
            return OTA_ERR_NETWORK;
        } else if (ret == 0) {
            LOG_E("Connection closed while reading header\r\n");
            return OTA_ERR_NETWORK;
        }
        *p_buffer_offset += ret;
    }

    ota_header = (ota_header_t *)g_recv_buffer;
    if (wifi_ota_check_header(ota_header, bin_size, use_xz)) {
        LOG_E("Invalid OTA header\r\n");
        return OTA_ERR_HEADER;
    }

    memcpy(sha256_img, ota_header->u.s.sha256, 32);

    if (*p_buffer_offset > OTA_HEADER_SIZE) {
        memmove(g_recv_buffer, g_recv_buffer + OTA_HEADER_SIZE, *p_buffer_offset - OTA_HEADER_SIZE);
        *p_buffer_offset -= OTA_HEADER_SIZE;
    } else {
        *p_buffer_offset = 0;
    }

    return OTA_OK;
}

/****************************************************************************
 * Download Firmware Data
 ****************************************************************************/
static int wifi_ota_download_firmware(int sock, uint32_t ota_addr, uint32_t bin_size, const uint8_t sha256_img[32],
                                      unsigned int initial_buffer_offset)
{
    int ret;
    mbedtls_sha256_context ctx_sha256;
    uint8_t sha256_result[32];
    unsigned int buffer_offset = initial_buffer_offset;
    unsigned int flash_offset = 0;
    int total_cnt = 0;

    mbedtls_sha256_init(&ctx_sha256);
    mbedtls_sha256_starts(&ctx_sha256, 0);
    memset(sha256_result, 0, sizeof(sha256_result));

    while (1) {
        ret = read(sock, g_recv_buffer + buffer_offset, OTA_PROGRAM_SIZE - buffer_offset);
        if (ret < 0) {
            LOG_E("ret = %d, err = %d\r\n", ret, errno);
            ret = OTA_ERR_NETWORK;
            break;
        } else {
            total_cnt += ret;
            if (ret == 0) {
                if (total_cnt < bin_size) {
                    LOG_E("Connection closed unexpectedly, received %u/%u\r\n", total_cnt, bin_size);
                    ret = OTA_ERR_NETWORK;
                } else {
                    ret = OTA_OK;
                }
                break;
            }
            LOG_I("total = %u, ret = %d\n\r", total_cnt, ret);
            buffer_offset += ret;

            if (buffer_offset > 0) {
                LOG_I("Will Write %u to 0x%08X left %u.\r\n", buffer_offset, ota_addr + flash_offset,
                      (bin_size - total_cnt) + buffer_offset);
                mbedtls_sha256_update(&ctx_sha256, g_recv_buffer, buffer_offset);
                /* Write data to flash based on configuration */
#ifdef CONFIG_IPC
#ifdef CONFIG_RPMSG_SERVICE_MODE_MASTER
                /* AP Core: Direct flash write with suspend/resume */
                ret = bflb_flash_write_mcs((ota_addr + flash_offset), g_recv_buffer, buffer_offset);
#else
                /* NP Core: Write via RPMsg */
                ret = bflb_flash_write_rpmsg((ota_addr + flash_offset), g_recv_buffer, buffer_offset);
#endif
#else
                /* Single-core mode: Direct flash write */
                ret = bflb_flash_write((ota_addr + flash_offset), g_recv_buffer, buffer_offset);
#endif
                if (ret != 0) {
                    LOG_E("flash write fail! %d\r\n", ret);
                    ret = OTA_ERR_FLASH;
                    break;
                }
                flash_offset += buffer_offset;
                buffer_offset = 0;
            }

            if (total_cnt >= bin_size) {
                ret = OTA_OK;
                break;
            }
        }
    }

    if (ret != OTA_OK) {
        LOG_E("Download failed with ret=%d\r\n", ret);
        mbedtls_sha256_free(&ctx_sha256);
        return ret;
    }

    LOG_I("Download complete, total=%u, verifying SHA256\r\n", total_cnt);

    mbedtls_sha256_finish(&ctx_sha256, sha256_result);
    mbedtls_sha256_free(&ctx_sha256);

    if (memcmp(sha256_img, sha256_result, 32)) {
        LOG_E("SHA256 NOT Correct\r\n");
        LOG_E("Expected: ");
        for (int i = 0; i < 32; i++) {
            printf("%02X", sha256_img[i]);
        }
        printf("\r\n");
        LOG_E("Got:      ");
        for (int i = 0; i < 32; i++) {
            printf("%02X", sha256_result[i]);
        }
        printf("\r\n");
        return OTA_ERR_CHECKSUM;
    }

    LOG_I("SHA256 verification passed\r\n");
    return OTA_OK;
}

/****************************************************************************
 * Update Partition Table and Reboot
 ****************************************************************************/

static int wifi_ota_update_partition_table(pt_table_id_type active_id, pt_table_stuff_config pt_table_stuff[],
                                           const wifi_ota_context_t *ctx)
{
    int ret;
    pt_table_entry_config pt_fw_entry = ctx->entry;

    LOG_I("Updating partition table\r\n");
    LOG_I("active_id=%d, bin_size=%lu\r\n", active_id, ctx->bin_size);

    pt_fw_entry.len = ctx->bin_size;
    pt_fw_entry.active_index = !(pt_fw_entry.active_index & 0x01);
    pt_fw_entry.age++;

    LOG_I("new active_index=%u, age=%u\r\n", pt_fw_entry.active_index, pt_fw_entry.age);
    LOG_I("target_table_id=%d (writing to %s)\r\n", !active_id, (!active_id == 0) ? "PT0" : "PT1");
    LOG_I("source pt_table age before write: %u\r\n", pt_table_stuff[active_id].pt_table.age);

    LOG_I("About to call pt_table_update_entry\r\n");

    ret = pt_table_update_entry(!active_id, &pt_table_stuff[active_id], &pt_fw_entry);
    if (ret != 0) {
        LOG_E("pt table update fail! %d\r\n", ret);
        return OTA_ERR_PARTITION;
    }

    LOG_I("Partition table updated successfully\r\n");
    LOG_I("Rebooting\r\n");
#ifdef CONFIG_IPC
#ifdef CONFIG_RPMSG_SERVICE_MODE_MASTER
    bflb_sys_reboot_mcs();
#else
    bflb_reboot_rpmsg();
#endif
#else
    bl_sys_reset_por();
#endif
    while (1)
        ;

    return OTA_OK;
}

static void wifi_ota_cleanup(int sock, bool free_buffer)
{
    if (sock >= 0) {
        closesocket(sock);
    }

    if (free_buffer && g_recv_buffer) {
        vPortFree(g_recv_buffer);
        g_recv_buffer = NULL;
    }
}

/****************************************************************************
 * Public API: Start OTA Update
 ****************************************************************************/
int wifi_ota_start_update(const char *server_ip, const char *server_port)
{
    pt_table_stuff_config pt_table_stuff[2];
    pt_table_id_type active_id;
    wifi_ota_context_t ctx;
    int sock = -1;
    int ret;
    uint8_t sha256_img[32];
    int use_xz = 0;

    LOG_I("wifi_ota_start_update called with ip=%s, port=%s\r\n", server_ip ? server_ip : "NULL",
          server_port ? server_port : "NULL");

    /* 1. Initialization check */
    if (!g_ota_initialized) {
        LOG_E("WiFi OTA not initialized\r\n");
        return OTA_ERR_NOT_INIT;
    }
    if (g_ota_in_progress) {
        LOG_E("OTA already in progress\r\n");
        return OTA_ERR_IN_PROGRESS;
    }
    g_ota_in_progress = true;

    /* 2. Initialize partition table */
    active_id = wifi_ota_init_partition(pt_table_stuff);
    if (active_id == PT_TABLE_ID_INVALID) {
        ret = OTA_ERR_PARTITION;
        goto cleanup;
    }

    /* 3. Prepare flash (get OTA address, allocate buffer) */
    ret = wifi_ota_prepare_flash(active_id, pt_table_stuff, &ctx);
    if (ret != OTA_OK) {
        goto cleanup;
    }

    /* 4. Connect to server */
    ret = wifi_ota_connect_server(server_ip, server_port, &sock);
    if (ret != OTA_OK) {
        goto cleanup;
    }

    /* 5. Download and verify OTA header */
    unsigned int buffer_offset = 0;
    ret = wifi_ota_download_header(sock, &ctx.bin_size, sha256_img, &use_xz, &buffer_offset);
    if (ret != OTA_OK) {
        goto cleanup;
    }

    /* 6. Download firmware data and verify (choose mode based on config) */
    LOG_I("Erasing flash with size %lu\r\n", ctx.bin_size);
#ifdef CONFIG_IPC
#ifdef CONFIG_RPMSG_SERVICE_MODE_MASTER
    /* AP Core: Erase flash with suspend/resume */
    ret = bflb_flash_erase_mcs(ctx.ota_addr, ctx.bin_size);
#else
    /* NP Core: Erase via RPMsg */
    ret = bflb_flash_erase_rpmsg(ctx.ota_addr, ctx.bin_size);
#endif
#else
    /* Single-core mode: Direct flash erase */
    ret = bflb_flash_erase(ctx.ota_addr, ctx.bin_size);
#endif
    if (ret != 0) {
        LOG_E("flash erase fail! %d\r\n", ret);
        ret = OTA_ERR_FLASH;
        goto cleanup;
    }
    LOG_I("Done\r\n");

    /* Download firmware data and verify */
    LOG_I("Starting firmware download\r\n");
    ret = wifi_ota_download_firmware(sock, ctx.ota_addr, ctx.bin_size, sha256_img, buffer_offset);
    if (ret != OTA_OK) {
        LOG_E("Firmware download failed with ret=%d\r\n", ret);
        goto cleanup;
    }
    LOG_I("Firmware download completed successfully\r\n");

    closesocket(sock);
    sock = -1;

    /* Update partition table and reboot */
    ret = wifi_ota_update_partition_table(active_id, pt_table_stuff, &ctx);

cleanup:
    wifi_ota_cleanup(sock, ret != OTA_OK);
    if (ret != OTA_OK) {
        wifi_ota_deinit();
    } else {
        g_ota_in_progress = false;
    }
    return ret;
}

int wifi_ota_init(void)
{
    int ret;

    if (g_ota_initialized) {
        LOG_W("WiFi OTA already initialized\r\n");
        return 0;
    }

    g_ota_initialized = true;
    return 0;
}

void wifi_ota_deinit(void)
{
    if (!g_ota_initialized) {
        return;
    }
    if (g_recv_buffer) {
        vPortFree(g_recv_buffer);
        g_recv_buffer = NULL;
    }
    g_ota_initialized = false;
    g_ota_in_progress = false;
}
