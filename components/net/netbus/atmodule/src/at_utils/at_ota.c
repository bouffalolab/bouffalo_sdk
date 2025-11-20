#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <partition.h>
#include <bflb_flash.h>
#include "at_ota.h"
#include "at_pal.h"

#define OTA_PARTITION_NAME_TYPE_FW    "FW"
#define OTA_UPGRADE_RETRY 1
#define OTA_DEBUG_IMG 0
#define OTA_ERASE_BLOCK_SIZE (4*1024)

static int at_ota_erase(at_ota_handle_t handle, uint32_t offset, uint32_t len)
{
    uint32_t start_index, end_index, index;
    int ret;
    
    start_index = offset / OTA_ERASE_BLOCK_SIZE;
    end_index = (offset + len - 1) / OTA_ERASE_BLOCK_SIZE;
    
    for (index = start_index; index <= end_index; index++) {
        if (index / 32 >= handle->sector_erased_size) {
            printf("index error:%d sector_size:%d\r\n", index, handle->sector_erased_size);
            return -1;
        }
        if ((handle->sector_erased[index / 32] & (1U << (index % 32))) == 0) {
            ret = bflb_flash_erase(handle->ota_addr + index * OTA_ERASE_BLOCK_SIZE, OTA_ERASE_BLOCK_SIZE);
            if (ret) {
                printf("mtd erase failed\r\n");
                return -1;
            }
            handle->sector_erased[index / 32] |= (1U << (index % 32));
        }
    }
    
    return 0;
}

static int ota_upgrade_slice(at_ota_handle_t handle, uint32_t offset, char *buf, uint32_t slice_size)
{
    int ret = 0;
    uint32_t retry;
    
#if (!CONFIG_AT_FAST_OTA)
    ret = at_ota_erase(handle, offset, slice_size);
    if (ret) {
        printf("erase failed\r\n");
        return -1;
    }
#endif

    for (retry = 0; retry < OTA_UPGRADE_RETRY; retry++) {

        ret = bflb_flash_write((handle->ota_addr + offset), buf, slice_size);

        if (ret != 0) {
            printf("flash write fail! %d\r\n", ret);
            return -1;
        }

#if (!CONFIG_AT_FAST_OTA)
        ret = bflb_flash_read((handle->ota_addr + offset), handle->check_buf, slice_size);
        if (ret != 0) {
            printf("flash read fail! %d\r\n", ret);
            return -1;
        }

        if (memcmp(handle->check_buf, buf, slice_size) != 0) {
            printf("flash check fail! 0x%08x\r\n", offset);
            return -1;
        }
#endif

#if OTA_DEBUG_IMG
        ret = bflb_flash_read((handle->active_addr + offset), handle->check_buf, slice_size);
        if (ret != 0) {
            printf("flash read fail! %d\r\n", ret);
            return -1;
        }

        if (memcmp(handle->check_buf, buf, slice_size) != 0) {
            printf("flash check fail! 0x%08x\r\n", offset);
            while(1);
        }
#endif 
    }
    return 0;
}

static int _check_ota_header(at_ota_header_t *ota_header, uint32_t *ota_len, int *use_xz)
{
    char str[33]; //assume max segment size
    int i;

    if (!ota_header || !ota_len || !use_xz) {
        return -1;
    }

    memcpy(str, ota_header->u.s.header, sizeof(ota_header->u.s.header));
    str[sizeof(ota_header->u.s.header)] = '\0';
    printf("[OTA] [HEADER] ota header is %s\r\n", str);

    if (strstr(str, "BFLB_OTA") == NULL) {
        return -1;
    }

    memcpy(str, ota_header->u.s.type, sizeof(ota_header->u.s.type));
    str[sizeof(ota_header->u.s.type)] = '\0';
    printf("[OTA] [HEADER] file type is %s\r\n", str);

    if (strstr(str, "XZ")) {
        *use_xz = 1;
    } else {
        *use_xz = 0;
    }

    if ((!strcmp(str, "RAW")) && (*use_xz != 1)) {
        return 1;
    }

    memcpy(ota_len, &(ota_header->u.s.len), 4);
    printf("[OTA] [HEADER] file length (exclude ota header) is %lu\r\n", *ota_len);

    memcpy(str, ota_header->u.s.ver_hardware, sizeof(ota_header->u.s.ver_hardware));
    str[sizeof(ota_header->u.s.ver_hardware)] = '\0';
    printf("[OTA] [HEADER] ver_hardware is %s\r\n", str);

    memcpy(str, ota_header->u.s.ver_software, sizeof(ota_header->u.s.ver_software));
    str[sizeof(ota_header->u.s.ver_software)] = '\0';
    printf("[OTA] [HEADER] ver_software is %s\r\n", str);

    memcpy(str, ota_header->u.s.sha256, sizeof(ota_header->u.s.sha256));
    str[sizeof(ota_header->u.s.sha256)] = '\0';
    printf("[OTA] [HEADER] sha256 is ");
    for (i = 0; i < sizeof(ota_header->u.s.sha256); i++) {
        printf("%02X", str[i]);
    }
    printf("\r\n");

    return 0;
}

at_ota_handle_t at_ota_start(at_ota_header_t *ota_header)
{
    if (!ota_header) {
        printf("[OTA] Error: ota_header is NULL\r\n");
        return NULL;
    }
    at_ota_handle_t ota_handle = at_malloc(sizeof(struct at_ota_handle));
    if (!ota_handle) {
        printf("[OTA] Error: at_malloc failed\r\n");
        return NULL;
    }
    memset(ota_handle, 0, sizeof(struct at_ota_handle));

    /* Set flash operation function, read via xip */
    pt_table_set_flash_operation(bflb_flash_erase, bflb_flash_write, bflb_flash_read);

    ota_handle->active_id = pt_table_get_active_partition_need_lock(ota_handle->pt_table_stuff);
    if (PT_TABLE_ID_INVALID == ota_handle->active_id) {
        printf("No valid PT\r\n");
        goto _fail;
    }
    printf("Active PT:%d,Age %d\r\n", ota_handle->active_id, ota_handle->pt_table_stuff[ota_handle->active_id].pt_table.age);

    if (pt_table_get_active_entries_by_name(&ota_handle->pt_table_stuff[ota_handle->active_id], (uint8_t *)OTA_PARTITION_NAME_TYPE_FW, &ota_handle->pt_fw_entry)) {
        printf("PtTable get active entry fail!\r\n");
        goto _fail;
    }

    if (_check_ota_header(ota_header, &ota_handle->file_size, &ota_handle->use_xz)) {
        /*ota header is NOT OK*/
        printf("[OTA] header check fail!\r\n");
        goto _fail;
    }
    ota_handle->ota_addr = ota_handle->pt_fw_entry.start_address[!ota_handle->pt_fw_entry.active_index];
    ota_handle->active_addr = ota_handle->pt_fw_entry.start_address[ota_handle->pt_fw_entry.active_index];
    ota_handle->part_size = ota_handle->pt_fw_entry.max_len[!ota_handle->pt_fw_entry.active_index];
    memcpy(ota_handle->sha256_img, ota_header->u.s.sha256, sizeof(ota_handle->sha256_img));
    
    if (ota_handle->part_size < ota_handle->file_size) {
        printf("[OTA] file_size overflow:0x%08x part_size:0x%08x\r\n", ota_handle->file_size, ota_handle->part_size);
        goto _fail;
    }
    ota_handle->sector_erased_size = ((ota_handle->part_size + (OTA_ERASE_BLOCK_SIZE - 1))/OTA_ERASE_BLOCK_SIZE + 31)/32;
    ota_handle->sector_erased = at_malloc(4 * ota_handle->sector_erased_size);
    if (!ota_handle->sector_erased) {
        printf("[OTA] Error: failed to allocate\r\n");
        at_free(ota_handle);
        return NULL;
    }
    memset(ota_handle->sector_erased, 0, 4 * ota_handle->sector_erased_size);

    printf("[OTA] [TEST] activeIndex is %u, use OTA address=%08x part_size=%08x\r\n", ota_handle->pt_fw_entry.active_index, (unsigned int)ota_handle->ota_addr, ota_handle->part_size);
    
    utils_sha256_init(&ota_handle->ctx_sha256);
    utils_sha256_starts(&ota_handle->ctx_sha256);

#if CONFIG_AT_FAST_OTA
    at_ota_erase(ota_handle, 0, ota_handle->part_size);
#endif
    return ota_handle;
_fail:
    at_free(ota_handle);
    return NULL;
}

int at_ota_update(at_ota_handle_t handle, uint32_t offset, uint8_t *buf, uint32_t buf_len)
{
    if (!handle || !buf) {
        printf("[OTA] Error: handle or buf is NULL\r\n");
        return -1;
    }

    uint32_t write_size, slice_size;
    int ret;

    if (handle->part_size < buf_len + handle->total_size) {
        printf("[OTA] file_size overflow file_size:%d total_size:%d!!!!!!\r\n", handle->part_size, handle->total_size);
        return -1;
    }
   
    if (handle->file_size < buf_len + handle->total_size) {
        printf("[OTA] file_size overflow file_size:%d total_size:%d buf_len:%d!!!!!!\r\n", handle->file_size, handle->total_size, buf_len);
        buf_len = handle->file_size - handle->total_size;
    }

    for (write_size = 0; write_size < buf_len; write_size += OTA_SLICE_SIZE) {
        slice_size = (buf_len - write_size < OTA_SLICE_SIZE)?(buf_len - write_size):OTA_SLICE_SIZE;
        ret = ota_upgrade_slice(handle, offset + write_size, buf, slice_size);
        if (ret != 0) {
            return ret;
        }
    }


    utils_sha256_update(&handle->ctx_sha256, buf, buf_len);
    handle->total_size += buf_len;
    return ret;
}

int at_ota_finish(at_ota_handle_t handle, uint8_t check_hash, uint8_t reboot)
{
    if (!handle) {
        printf("[OTA] Error: handle is NULL\r\n");
        return -1;
    }

    int status;
    if (handle->file_size != handle->total_size) {
        printf("[OTA] file_size error file_size:%d total_size:%d\r\n", handle->file_size, handle->total_size);
        return -1;
    }
   
    if (check_hash) {
        utils_sha256_finish(&handle->ctx_sha256, handle->sha256_result);
        if (memcmp(handle->sha256_img, handle->sha256_result, sizeof(handle->sha256_img))) {
            printf("[OTA] check SHA256 NOT Correct\r\n");
            return -1;
        }
    }

    printf("[OTA] prepare OTA partition info\r\n");
    handle->pt_fw_entry.len = handle->file_size;
    printf("[OTA] Update PARTITION, partition len is %lu\r\n", handle->pt_fw_entry.len);
    handle->pt_fw_entry.active_index = !(handle->pt_fw_entry.active_index & 0x01);
    handle->pt_fw_entry.age++;
    status = pt_table_update_entry(!handle->active_id, &handle->pt_table_stuff[handle->active_id], &handle->pt_fw_entry);
    if (status != 0) {
        printf("pt table update fail! %d\r\n", status);
        return -1;
    }
    printf("[OTA] Rebooting\r\n");
    if (reboot) {
        bl_sys_reset_por();
    }
    return 0;
}

int at_ota_abort(at_ota_handle_t handle)
{
    if (!handle) {
        printf("[OTA] Error: handle is NULL\r\n");
        return -1;
    }
    utils_sha256_free(&handle->ctx_sha256);
    if (handle->sector_erased) {
        at_free(handle->sector_erased);
    }
    at_free(handle);
    return 0;
}

