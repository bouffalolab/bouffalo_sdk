
#ifndef __AT_OTA_H__
#define __AT_OTA_H__

#include <utils_sha256.h>
#include "partition.h"

#define OTA_SLICE_SIZE    (4096)

typedef struct ota_header {
    union {
        struct {
            uint8_t header[16];

            uint8_t type[4]; //RAW XZ
            uint32_t len;    //body len
            uint8_t pad0[8];

            uint8_t ver_hardware[16];
            uint8_t ver_software[16];
    
            uint8_t sha256[32];
        } s;
        uint8_t _pad[512];
    } u;
} at_ota_header_t;

typedef struct at_ota_handle {
    pt_table_stuff_config pt_table_stuff[2];
    pt_table_entry_config pt_fw_entry;
    pt_table_id_type active_id;
    uint32_t *sector_erased;
    uint32_t sector_erased_size;
    uint8_t use_xz;
    uint32_t file_size;
    uint32_t total_size; 
    uint32_t part_size; // part_size;
    uint32_t ota_addr;
    uint32_t active_addr;
    uint8_t sha256_img[32];
    uint8_t sha256_result[32];
    sha256_context ctx_sha256;
    uint8_t check_buf[OTA_SLICE_SIZE];
} *at_ota_handle_t;


at_ota_handle_t at_ota_start(at_ota_header_t *ota_header);
int at_ota_update(at_ota_handle_t handle, uint32_t offset, uint8_t *buf, uint32_t buf_len);
int at_ota_finish(at_ota_handle_t handle, uint8_t check_hash, uint8_t reboot);
int at_ota_abort(at_ota_handle_t handle);

#endif

