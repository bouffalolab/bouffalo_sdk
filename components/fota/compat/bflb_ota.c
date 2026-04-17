#include <stdlib.h>
#include <string.h>
#include <bflb_flash.h>
#include <bflb_sec_sha.h>
#include <bflb_core.h>

#define DBG_TAG "BFLB_OTA"
#include "../ota/ota.h"
#include "bflb_ota.h"

static ota_handle_t handle = NULL;

static void _cleanup(void)
{
    if (handle) {
        ota_abort(handle);
        handle = NULL;
    }
}

int bflb_ota_start(uint32_t file_size)
{
    if (handle) {
        LOG_W("ota had start\r\n");
        return 0;
    }

    handle = ota_start();
    if (!handle) {
        return -1;
    }

    /* bflb_ota data format: no 512-byte header, SHA256 appended at tail */
    handle->head_checked = 1;
    handle->file_size = file_size;
    handle->tail_hash_size = 32;  /* bflb_ota format: [raw_fw][SHA256(32)] */

    if (file_size > handle->part_size) {
        LOG_E("file size is more than partition size\r\n");
        _cleanup();
        return -1;
    }

    LOG_I("Starting OTA. OTA size is %lu\r\n", handle->part_size);
    LOG_I("[OTA] activeIndex is %u, use OTA address=%08x\r\n",
          handle->pt_fw_entry.active_index, (unsigned int)handle->ota_addr);
    return 0;
}

int bflb_ota_update(uint32_t filesize, uint32_t offset, uint8_t *buf, uint32_t buf_len)
{
    if (!handle) {
        LOG_E("please start ota first\r\n");
        return -1;
    }

    if ((filesize > handle->file_size) || !buf ||
        ((offset + buf_len) > handle->file_size) || (buf_len == 0)) {
        LOG_E("parm is error!\r\n");
        return -1;
    }

    return ota_update(handle, buf, buf_len);
}

int bflb_ota_finish(uint8_t check_hash)
{
    if (!handle || handle->file_size <= 32) {
        _cleanup();
        return -1;
    }

    int ret = ota_finish(handle, check_hash, 0);
    if (ret == 0) {
        handle = NULL;  /* freed by ota_finish */
    } else {
        _cleanup();
    }
    return ret;
}

int bflb_ota_read(uint32_t offset, uint8_t *buf, uint32_t buf_len)
{
    if (!handle || !buf || ((offset + buf_len) > handle->file_size) || (buf_len == 0)) {
        LOG_E("parm is error!\r\n");
        return -1;
    }

    return bflb_flash_read(handle->ota_addr + offset, buf, buf_len);
}

int bflb_ota_check(void)
{
    if (!handle || handle->file_size <= handle->tail_hash_size) {
        _cleanup();
        return -1;
    }

    return ota_verify_hash(handle);
}

int bflb_ota_apply(void)
{
    if (!handle || handle->file_size <= 32) {
        _cleanup();
        return -1;
    }

    int ret = ota_finish(handle, 0, 0);
    if (ret == 0) {
        handle = NULL;
    } else {
        _cleanup();
    }
    return ret;
}

void bflb_ota_abort(void)
{
    _cleanup();
}
