/**
 * at_ota.c — thin wrapper around fota/ota core
 *
 * The heavy lifting (partition table, flash erase/write, SHA-256 verify) is
 * delegated to ota_start / ota_update / ota_finish / ota_abort in fota/ota.
 * This file only adapts the AT-module calling convention so that
 * at_base_cmd.c does not need any changes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "at_ota.h"
#include "at_pal.h"

at_ota_handle_t at_ota_start(at_ota_header_t *ota_header)
{
    if (!ota_header) {
        printf("[OTA] Error: ota_header is NULL\r\n");
        return NULL;
    }

    at_ota_handle_t handle = at_malloc(sizeof(struct at_ota_handle));
    if (!handle) {
        printf("[OTA] Error: at_malloc failed\r\n");
        return NULL;
    }
    memset(handle, 0, sizeof(struct at_ota_handle));

    /* Initialise the fota core (partition table, flash ops) */
    handle->ota = ota_start();
    if (!handle->ota) {
        printf("[OTA] ota_start failed\r\n");
        at_free(handle);
        return NULL;
    }

    /* Feed the 512-byte header; ota_update() parses it internally */
    if (ota_update(handle->ota, (uint8_t *)ota_header, sizeof(at_ota_header_t)) != 0) {
        printf("[OTA] header feed failed\r\n");
        ota_abort(handle->ota);
        at_free(handle);
        return NULL;
    }

    /* Mirror file_size so at_base_cmd.c can read handle->file_size */
    handle->file_size = handle->ota->file_size;

    return handle;
}

int at_ota_update(at_ota_handle_t handle, uint32_t offset, uint8_t *buf, uint32_t buf_len)
{
    if (!handle || !buf) {
        printf("[OTA] Error: handle or buf is NULL\r\n");
        return -1;
    }

    /* offset is ignored — fota core tracks position internally */
    return ota_update(handle->ota, buf, buf_len);
}

int at_ota_finish(at_ota_handle_t handle, uint8_t check_hash, uint8_t reboot)
{
    int ret;

    if (!handle) {
        printf("[OTA] Error: handle is NULL\r\n");
        return -1;
    }

    ret = ota_finish(handle->ota, check_hash, reboot);
    /* ota_finish frees the fota handle internally on success path,
       but we still own the wrapper */
    if (ret == 0) {
        at_free(handle);
    }
    return ret;
}

int at_ota_abort(at_ota_handle_t handle)
{
    if (!handle) {
        printf("[OTA] Error: handle is NULL\r\n");
        return -1;
    }

    ota_abort(handle->ota);
    at_free(handle);
    return 0;
}

