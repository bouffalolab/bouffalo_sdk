
#ifndef __AT_OTA_H__
#define __AT_OTA_H__

#include <stdint.h>
#include "ota/ota.h"

#define AT_OTA_SLICE_SIZE    OTA_SLICE_SIZE

typedef ota_header_t at_ota_header_t;

typedef struct at_ota_handle {
    ota_handle_t ota;           /* fota core handle */
    uint32_t file_size;         /* firmware size (for at_base_cmd.c access) */
} *at_ota_handle_t;


at_ota_handle_t at_ota_start(at_ota_header_t *ota_header);
int at_ota_update(at_ota_handle_t handle, uint32_t offset, uint8_t *buf, uint32_t buf_len);
int at_ota_finish(at_ota_handle_t handle, uint8_t check_hash, uint8_t reboot);
int at_ota_abort(at_ota_handle_t handle);

#endif

