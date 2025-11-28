/**
  ******************************************************************************
  * @file    ota.h
  * @version V1.0
  * @date
  * @brief   This file is OTA feature
  ******************************************************************************
  */

#ifndef __OTA_H__
#define __OTA_H__

#include <utils_sha256.h>
#include "partition.h"
#include "log.h"

#define CONFIG_FAST_OTA   (1)
#define OTA_SLICE_SIZE    (4096)

typedef struct ota_handle {
    pt_table_stuff_config pt_table_stuff[2];
    pt_table_entry_config pt_fw_entry;
    pt_table_id_type active_id;
    uint32_t *sector_erased;
    uint32_t sector_erased_size;
    uint8_t use_xz;
    uint8_t head_checked;
    uint32_t file_size;
    uint32_t total_size;
    uint32_t part_size; // part_size;
    uint32_t ota_addr;
    uint32_t active_addr;
    uint8_t sha256_img[32];
    uint8_t sha256_result[32];
    sha256_context ctx_sha256;
    uint8_t check_buf[OTA_SLICE_SIZE];
    uint32_t offset;
} *ota_handle_t;


 ota_handle_t ota_start(void);
int ota_update(ota_handle_t handle, uint8_t *buf, uint32_t buf_len);
int ota_finish(ota_handle_t handle, uint8_t check_hash, uint8_t reboot);
int ota_abort(ota_handle_t handle);
int ota_rollback(void);

#endif

