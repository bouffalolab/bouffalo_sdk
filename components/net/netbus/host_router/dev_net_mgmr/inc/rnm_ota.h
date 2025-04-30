#pragma once

#include <rnm_server.h>

#include <utils_sha256.h>

#include "sdiowifi_platform_adapt.h"

typedef struct
{
    iot_sha256_context sha_ctx;
    uint8_t sha256_this[32];
    uint8_t sha256_that[32];     // ota header advertised checksum
    bl_mtd_handle_t mtd_hdl;
    uint32_t offset_written;
    uint32_t body_length;        // ota header advertised body size
    uint32_t tgt_part_size;      // target partition size
    uint8_t *write_buf;
    HALPartition_Entry_Config ptEntry;
} rnm_ota_t;

void rnms_handle_ota(rnms_t *rnm, rnm_msg_t *cmd);
