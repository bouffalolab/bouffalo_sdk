/**
  ******************************************************************************
  * @file    ota.c
  * @version V1.0
  * @date
  * @brief   This file is OTA feature
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <partition.h>
#include <bflb_flash.h>
#include <bflb_core.h>
#include <bflb_l1c.h>
#include <bflb_sec_mutex.h>
#include "bl_sys.h"
#include "ota.h"

#ifdef CONFIG_IPC
#include <bflb_multi_core_sync.h>
#include <flash_ops_rpmsg.h>
#endif

#ifdef BL618DG
#define OTA_PARTITION_NAME_TYPE_FW    "FW0"
#else
#define OTA_PARTITION_NAME_TYPE_FW    "FW"
#endif
#define OTA_UPGRADE_RETRY 1
#define OTA_DEBUG_IMG 0
#define OTA_ERASE_BLOCK_SIZE (4*1024)

static ATTR_NOCACHE_NOINIT_RAM_SECTION __attribute__((aligned(32))) struct bflb_sha256_link_ctx_s _sha_ctx;
    
#ifdef CONFIG_OTA_VERSION_CHECK
#ifdef CONFIG_OTA_VERSION_PREFIX
#define OTA_VERSION_PREFIX CONFIG_OTA_VERSION_PREFIX
#else
#define OTA_VERSION_PREFIX "EVENT_V"
#endif

static int ota_extract_version_triplet(const uint8_t *field, uint32_t field_len,
                                       uint32_t *major, uint32_t *minor, uint32_t *patch)
{
    static const char version_prefix[] = OTA_VERSION_PREFIX;
    char version_str[17];
    uint32_t copy_len;
    char *cursor;
    char *endptr;
    unsigned long parsed[3];
    int index;

    copy_len = field_len < (sizeof(version_str) - 1U) ? field_len : (sizeof(version_str) - 1U);
    memcpy(version_str, field, copy_len);
    version_str[copy_len] = '\0';

    if (strncmp(version_str, version_prefix, sizeof(version_prefix) - 1U) != 0) {
        return -1;
    }

    cursor = version_str + sizeof(version_prefix) - 1U;

    for (index = 0; index < 3; index++) {
        if (*cursor < '0' || *cursor > '9') {
            return -1;
        }

        parsed[index] = strtoul(cursor, &endptr, 10);
        if (endptr == cursor || parsed[index] > 255UL) {
            return -1;
        }

        cursor = endptr;
        if (index < 2) {
            if (*cursor != '.') {
                return -1;
            }
            cursor++;
        }
    }

    if (*cursor != '\0') {
        return -1;
    }

    *major = (uint32_t)parsed[0];
    *minor = (uint32_t)parsed[1];
    *patch = (uint32_t)parsed[2];
    return 0;
}

static int ota_check_header_software_version(const uint8_t *field, uint32_t field_len)
{
    uint32_t incoming_major;
    uint32_t incoming_minor;
    uint32_t incoming_patch;

    if (ota_extract_version_triplet(field, field_len, &incoming_major, &incoming_minor, &incoming_patch) != 0) {
        LOG_E("[OTA] [HEADER] invalid software version format, expect %sx.y.z\r\n", OTA_VERSION_PREFIX);
        return -1;
    }

    LOG_I("[OTA] [HEADER] current version %u.%u.%u, incoming version %u.%u.%u\r\n",
          APP_VER_X, APP_VER_Y, APP_VER_Z,
          incoming_major, incoming_minor, incoming_patch);

    if ((incoming_major < APP_VER_X) ||
        (incoming_major == APP_VER_X && incoming_minor < APP_VER_Y) ||
        (incoming_major == APP_VER_X && incoming_minor == APP_VER_Y && incoming_patch <= APP_VER_Z)) {
        LOG_E("[OTA] [HEADER] incoming version is not newer than current firmware\r\n");
        return -1;
    }

    return 0;
}
#endif

static int ota_sha256_update(ota_handle_t handle, const uint8_t *buf, uint32_t len)
{
    int ret;

    if (len == 0) {
        return 0;
    }

    bflb_l1c_dcache_clean_range((void *)buf, len);
    bflb_sec_sha_mutex_take();
    ret = bflb_sha256_link_update(handle->sha_dev, handle->ctx_sha256, buf, len);
    bflb_sec_sha_mutex_give();
    if (ret != 0) {
        LOG_E("[OTA] sha256 update fail! %d\r\n", ret);
        return -1;
    }

    return 0;
}

static int ota_erase(ota_handle_t handle, uint32_t offset, uint32_t len)
{
    uint32_t start_index, end_index, index;
    int ret;

    start_index = offset / OTA_ERASE_BLOCK_SIZE;
    end_index = (offset + len - 1) / OTA_ERASE_BLOCK_SIZE;

    for (index = start_index; index <= end_index; index++) {
        if (index / 32 >= handle->sector_erased_size) {
            LOG_E("index error:%d sector_size:%d\r\n", index, handle->sector_erased_size);
            return -1;
        }
        if ((handle->sector_erased[index / 32] & (1U << (index % 32))) == 0) {
#ifdef CONFIG_IPC
#ifdef CONFIG_RPMSG_SERVICE_MODE_MASTER
            ret = bflb_flash_erase_mcs(handle->ota_addr + index * OTA_ERASE_BLOCK_SIZE, OTA_ERASE_BLOCK_SIZE);
#else
            ret = bflb_flash_erase_rpmsg(handle->ota_addr + index * OTA_ERASE_BLOCK_SIZE, OTA_ERASE_BLOCK_SIZE);
#endif
#else
            ret = bflb_flash_erase(handle->ota_addr + index * OTA_ERASE_BLOCK_SIZE, OTA_ERASE_BLOCK_SIZE);
#endif
            if (ret) {
                LOG_E("mtd erase failed\r\n");
                return -1;
            }
            handle->sector_erased[index / 32] |= (1U << (index % 32));
        }
    }

    return 0;
}

static int ota_upgrade_slice(ota_handle_t handle, uint32_t offset, char *buf, uint32_t slice_size)
{
    int ret = 0;
    uint32_t retry;

#if (!CONFIG_FAST_OTA)
    ret = ota_erase(handle, offset, slice_size);
    if (ret) {
        LOG_E("erase failed\r\n");
        return -1;
    }
#endif

    for (retry = 0; retry < OTA_UPGRADE_RETRY; retry++) {
#ifdef CONFIG_IPC
#ifdef CONFIG_RPMSG_SERVICE_MODE_MASTER
        ret = bflb_flash_write_mcs((handle->ota_addr + offset), buf, slice_size);
#else
        ret = bflb_flash_write_rpmsg((handle->ota_addr + offset), buf, slice_size);
#endif
#else
        ret = bflb_flash_write((handle->ota_addr + offset), (uint8_t *)buf, slice_size);
#endif

        if (ret != 0) {
            LOG_E("flash write fail! %d\r\n", ret);
            return -1;
        }


#if OTA_DEBUG_IMG
        ret = bflb_flash_read((handle->ota_addr + offset), handle->check_buf, slice_size);
        if (ret != 0) {
            LOG_E("flash read fail! %d\r\n", ret);
            return -1;
        }

        if (memcmp(handle->check_buf, buf, slice_size) != 0) {
            LOG_E("flash check fail! 0x%08x\r\n", offset);
            while(1);
        }
#endif
    }
    return 0;
}

static int _check_ota_header(ota_header_t *ota_header, uint32_t *ota_len, int *use_xz)
{
    char str[33]; //assume max segment size
    int i;

    memcpy(str, ota_header->u.s.header, sizeof(ota_header->u.s.header));
    str[sizeof(ota_header->u.s.header)] = '\0';
    LOG_I("[OTA] [HEADER] ota header is %s\r\n", str);

    if (strstr(str, "BL60X_OTA") == NULL) {
        return -1;
    }

    memcpy(str, ota_header->u.s.type, sizeof(ota_header->u.s.type));
    str[sizeof(ota_header->u.s.type)] = '\0';
    LOG_I("[OTA] [HEADER] file type is %s\r\n", str);

    if (strstr(str, "XZ")) {
        *use_xz = 1;
    } else {
        *use_xz = 0;
    }

    if ((!strcmp(str, "RAW")) && (*use_xz != 1)) {
        return 1;
    }

    memcpy(ota_len, &(ota_header->u.s.len), 4);
    LOG_I("[OTA] [HEADER] file length (exclude ota header) is %lu\r\n", *ota_len);

    memcpy(str, ota_header->u.s.ver_hardware, sizeof(ota_header->u.s.ver_hardware));
    str[sizeof(ota_header->u.s.ver_hardware)] = '\0';
    LOG_I("[OTA] [HEADER] ver_hardware is %s\r\n", str);

    memcpy(str, ota_header->u.s.ver_software, sizeof(ota_header->u.s.ver_software));
    str[sizeof(ota_header->u.s.ver_software)] = '\0';
    LOG_I("[OTA] [HEADER] ver_software is %s\r\n", str);

#ifdef CONFIG_OTA_VERSION_CHECK
    if (ota_check_header_software_version(ota_header->u.s.ver_software,
                                          sizeof(ota_header->u.s.ver_software))) {
        return -1;
    }
#endif

    memcpy(str, ota_header->u.s.sha256, sizeof(ota_header->u.s.sha256));
    str[sizeof(ota_header->u.s.sha256)] = '\0';
    LOG_D("[OTA] [HEADER] sha256 is ");
    for (i = 0; i < sizeof(ota_header->u.s.sha256); i++) {
        LOG_D("%02X", str[i]);
    }
    LOG_D("\r\n");

    return 0;
}

 ota_handle_t ota_start(void)
{
    ota_handle_t ota_handle = malloc(sizeof(struct ota_handle));
    if (!ota_handle) {
        LOG_E("[OTA] Error: malloc failed\r\n");
        return NULL;
    }
    memset(ota_handle, 0, sizeof(struct ota_handle));

    /* Set flash operation function, read via xip */
#ifdef CONFIG_IPC
#ifdef CONFIG_RPMSG_SERVICE_MODE_MASTER
    pt_table_set_flash_operation(bflb_flash_erase_mcs, bflb_flash_write_mcs, bflb_flash_read_mcs);
#else
    pt_table_set_flash_operation(bflb_flash_erase_rpmsg, bflb_flash_write_rpmsg, bflb_flash_read_rpmsg);
#endif
#else
    pt_table_set_flash_operation(bflb_flash_erase, bflb_flash_write, bflb_flash_read);
#endif

    ota_handle->active_id = pt_table_get_active_partition_need_lock(ota_handle->pt_table_stuff);
    if (PT_TABLE_ID_INVALID == ota_handle->active_id) {
        LOG_E("No valid PT\r\n");
        goto _fail;
    }
    LOG_D("Active PT:%d,Age %d\r\n", ota_handle->active_id, ota_handle->pt_table_stuff[ota_handle->active_id].pt_table.age);

    if (pt_table_get_active_entries_by_name(&ota_handle->pt_table_stuff[ota_handle->active_id], (uint8_t *)OTA_PARTITION_NAME_TYPE_FW, &ota_handle->pt_fw_entry)) {
        LOG_E("PtTable get active entry fail!\r\n");
        goto _fail;
    }

    ota_handle->ota_addr = ota_handle->pt_fw_entry.start_address[!ota_handle->pt_fw_entry.active_index];
    ota_handle->active_addr = ota_handle->pt_fw_entry.start_address[ota_handle->pt_fw_entry.active_index];
    ota_handle->part_size = ota_handle->pt_fw_entry.max_len[!ota_handle->pt_fw_entry.active_index];

    ota_handle->sector_erased_size = ((ota_handle->part_size + (OTA_ERASE_BLOCK_SIZE - 1))/OTA_ERASE_BLOCK_SIZE + 31)/32;
    ota_handle->sector_erased = malloc(4 * ota_handle->sector_erased_size);
    if (!ota_handle->sector_erased) {
        LOG_E("[OTA] Error: malloc failed\r\n");
        goto _fail;
    }
    memset(ota_handle->sector_erased, 0, 4 * ota_handle->sector_erased_size);

    ota_handle->check_buf_raw = malloc(OTA_SLICE_SIZE + BFLB_CACHE_LINE_SIZE - 1U);
    if (!ota_handle->check_buf_raw) {
        LOG_E("[OTA] Error: malloc failed\r\n");
        goto _fail;
    }
    ota_handle->check_buf = (uint8_t *)(((uintptr_t)ota_handle->check_buf_raw + BFLB_CACHE_LINE_SIZE - 1U) &
                                        ~((uintptr_t)BFLB_CACHE_LINE_SIZE - 1U));
    ota_handle->sha_dev = bflb_device_get_by_name(BFLB_NAME_SEC_SHA);
    if (!ota_handle->sha_dev) {
        LOG_E("[OTA] Error: sha device not found\r\n");
        goto _fail;
    }
    ota_handle->ctx_sha256 = &_sha_ctx;
    bflb_group0_request_sha_access(ota_handle->sha_dev);
    bflb_sha_link_init(ota_handle->sha_dev);

#if CONFIG_FAST_OTA
    ota_erase(ota_handle, 0, ota_handle->part_size);
#endif
    return ota_handle;
_fail:
    if (ota_handle && ota_handle->sha_dev) {
        bflb_group0_release_sha_access(ota_handle->sha_dev);
    }
    if (ota_handle && ota_handle->sector_erased) {
        free(ota_handle->sector_erased);
    }
    if (ota_handle && ota_handle->check_buf_raw) {
        free(ota_handle->check_buf_raw);
    }
    free(ota_handle);
    return NULL;
}

int ota_update(ota_handle_t handle, uint8_t *buf, uint32_t buf_len)
{
    uint32_t write_size, slice_size;
    int use_xz = 0;
    int ret;

    if (!handle || !buf) {
        LOG_E("[OTA] Error: handle or buf is NULL\r\n");
        return -1;
    }

    if (handle->part_size < buf_len + handle->total_size) {
        LOG_E("[OTA] file_size overflow file_size:%d total_size:%d!!!!!!\r\n", handle->part_size, handle->total_size);
        return -1;
    }

    if (!handle->head_checked) {
        if (handle->offset >= sizeof(ota_header_t)) {
            LOG_E("[OTA] header check fail %d\r\n", handle->offset);
            return -1;
        }

        write_size = (sizeof(ota_header_t) - handle->offset) > buf_len ? buf_len : (sizeof(ota_header_t) - handle->offset);
    	memcpy(handle->check_buf + handle->offset, buf, write_size);
        handle->offset += write_size;
        LOG_D("[OTA] set header %d %d\r\n", handle->offset, write_size);
        if (handle->offset < sizeof(ota_header_t)) {
            return 0;
        }

        buf += write_size;
        buf_len -= write_size;

        if (_check_ota_header((ota_header_t *)handle->check_buf, &handle->file_size, &use_xz)) {
            /*ota header is NOT OK*/
            LOG_E("[OTA] header check fail!\r\n");
            return -1;
        }
        handle->use_xz = (uint8_t)use_xz;
        memcpy(handle->sha256_img, ((ota_header_t *)handle->check_buf)->u.s.sha256, sizeof(handle->sha256_img));
        handle->head_checked = 1;
        handle->offset -= sizeof(ota_header_t);
    }

    LOG_D("[OTA] ota_update offset:%lx len:%lx\r\n", handle->offset, buf_len);

    if (!handle->head_checked) {
        LOG_E("[OTA] header not check!\r\n");
        return -1;
    }

    if (handle->file_size < buf_len + handle->total_size) {
        LOG_E("[OTA] file_size overflow file_size:%d total_size:%d buf_len:%d!!!!!!\r\n", handle->file_size, handle->total_size, buf_len);
        buf_len = handle->file_size - handle->total_size;
    }

    for (write_size = 0; write_size < buf_len; write_size += OTA_SLICE_SIZE) {
        slice_size = (buf_len - write_size < OTA_SLICE_SIZE)?(buf_len - write_size):OTA_SLICE_SIZE;
        ret = ota_upgrade_slice(handle, handle->offset, (char *)buf + write_size, slice_size);
        if (ret != 0) {
            return ret;
        }
        handle->offset += slice_size;
    }

    handle->total_size += buf_len;
    return ret;
}

static int ota_verify_flash(ota_handle_t handle)
{
    uint32_t bin_size = handle->file_size - handle->tail_hash_size;
    uint32_t rd_offset = 0, read_size;
    uint8_t expected_hash[32];
    int ret;

    if (handle->tail_hash_size > handle->file_size) {
        LOG_E("[OTA] invalid tail hash size file_size:%lu tail_hash_size:%u\r\n",
              (unsigned long)handle->file_size, handle->tail_hash_size);
        return -1;
    }

    bflb_sec_sha_mutex_take();
    bflb_sha256_link_start(handle->sha_dev, handle->ctx_sha256, 0);
    bflb_sec_sha_mutex_give();

    while (rd_offset < bin_size) {
        read_size = (bin_size - rd_offset >= OTA_SLICE_SIZE) ? OTA_SLICE_SIZE : (bin_size - rd_offset);
        if (bflb_flash_read(handle->ota_addr + rd_offset, handle->check_buf, read_size)) {
            LOG_E("[OTA] flash read failed\r\n");
            return -1;
        }
        ret = ota_sha256_update(handle, handle->check_buf, read_size);
        if (ret != 0) {
            return ret;
        }
        rd_offset += read_size;
    }

    bflb_sec_sha_mutex_take();
    bflb_sha256_link_finish(handle->sha_dev, handle->ctx_sha256, handle->sha256_result);
    bflb_sec_sha_mutex_give();

    if (handle->tail_hash_size) {
        if (bflb_flash_read(handle->ota_addr + bin_size, expected_hash, sizeof(expected_hash))) {
            LOG_E("[OTA] flash read hash failed\r\n");
            return -1;
        }
    } else {
        memcpy(expected_hash, handle->sha256_img, sizeof(expected_hash));
    }

    if (memcmp(expected_hash, handle->sha256_result, sizeof(expected_hash)) != 0) {
        LOG_E("[OTA] check SHA256 NOT Correct\r\n");
        return -1;
    }
    return 0;
}

int ota_verify_hash(ota_handle_t handle)
{
    return ota_verify_flash(handle);
}

int ota_finish(ota_handle_t handle, uint8_t check_hash, uint8_t reboot)
{
    if (!handle) {
        LOG_E("[OTA] Error: handle is NULL\r\n");
        return -1;
    }

    int status;
    if (handle->file_size != handle->total_size) {
        LOG_E("[OTA] file_size error file_size:%d total_size:%d\r\n", handle->file_size, handle->total_size);
        return -1;
    }

    if (check_hash) {
        if (ota_verify_flash(handle)) {
            return -1;
        }
    }

    LOG_D("[OTA] prepare OTA partition info\r\n");
    handle->pt_fw_entry.len = handle->file_size - handle->tail_hash_size;
    LOG_D("[OTA] Update PARTITION, partition len is %lu\r\n", handle->pt_fw_entry.len);
    handle->pt_fw_entry.active_index = !(handle->pt_fw_entry.active_index & 0x01);
    handle->pt_fw_entry.age++;
    status = pt_table_update_entry(!handle->active_id, &handle->pt_table_stuff[handle->active_id], &handle->pt_fw_entry);
    if (status != 0) {
        LOG_E("pt table update fail! %d\r\n", status);
        return -1;
    }

    bflb_group0_release_sha_access(handle->sha_dev);
    free(handle->sector_erased);
    free(handle->check_buf_raw);
    free(handle);

    if (reboot) {
        LOG_D("[OTA] Rebooting\r\n");
#ifdef CONFIG_IPC
#ifdef CONFIG_RPMSG_SERVICE_MODE_MASTER
        bflb_sys_reboot_mcs();
#else
        bflb_reboot_rpmsg();
#endif
#else
        bl_sys_reset_por();
#endif
    }
    return 0;
}

int ota_abort(ota_handle_t handle)
{
    if (!handle) {
        LOG_E("[OTA] Error: handle is NULL\r\n");
        return -1;
    }
    bflb_group0_release_sha_access(handle->sha_dev);
    free(handle->sector_erased);
    free(handle->check_buf_raw);
    free(handle);
    return 0;
}

int ota_rollback(void)
{
	int status;
    pt_table_entry_config pt_fw_entry;
    pt_table_stuff_config pt_table_stuff[2];
	pt_table_id_type active_id = pt_table_get_active_partition_need_lock(pt_table_stuff);

    if (pt_table_get_active_entries_by_name(&pt_table_stuff[active_id], (uint8_t *)OTA_PARTITION_NAME_TYPE_FW, &pt_fw_entry)) {
        LOG_E("PtTable get active entry fail!\r\n");
        return -1;
    }

    LOG_D("[OTA] active_index:%d partition len is %lu age:%d\r\n", pt_fw_entry.active_index, pt_fw_entry.len, pt_fw_entry.age);

    pt_fw_entry.active_index = !(pt_fw_entry.active_index & 0x01);
    pt_fw_entry.age++;
    status = pt_table_update_entry(!active_id, &pt_table_stuff[active_id], &pt_fw_entry);
    if (status != 0) {
        LOG_E("pt table update fail! %d\r\n", status);
        return -1;
    }
	return 0;
}

