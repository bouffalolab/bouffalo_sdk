#ifndef __WIFI_OTA_INTERNAL_H__
#define __WIFI_OTA_INTERNAL_H__

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include "partition.h"

#define OTA_PROGRAM_SIZE            (512)
#define OTA_HEADER_SIZE             (512)
#define OTA_PARTITION_NAME_TYPE_FW  "FW"
#define OTA_PARTITION_NAME_TYPE_FW0 "FW0"

#include "mbedtls/sha256.h"
typedef struct ota_header {
    union {
        struct {
            uint8_t header[16];
            uint8_t type[4]; /* RAW XZ */
            uint32_t len;    /* body len */
            uint8_t pad0[8];
            uint8_t ver_hardware[16];
            uint8_t ver_software[16];
            uint8_t sha256[32];
        } __attribute__((packed)) s;
        uint8_t _pad[512];
    } u;
} __attribute__((packed)) ota_header_t;

/* Error codes */
#define OTA_OK                  0
#define OTA_ERR_NOT_INIT       -1
#define OTA_ERR_IN_PROGRESS    -2
#define OTA_ERR_INVALID_PARAM  -3
#define OTA_ERR_PARTITION      -4
#define OTA_ERR_FLASH          -5
#define OTA_ERR_NETWORK        -6
#define OTA_ERR_HEADER         -7
#define OTA_ERR_CHECKSUM       -8

/* OTA context structure */
typedef struct {
    uint32_t ota_addr;              /* OTA write address */
    uint32_t bin_size;              /* Firmware size */
    uint32_t active_addr;           /* Current active partition address */
    pt_table_entry_config entry;    /* Partition entry */
} wifi_ota_context_t;

/* Function declarations for internal use */
int wifi_ota_check_header(ota_header_t *ota_header, uint32_t *ota_len, int *use_xz);

extern bool g_ota_initialized;
extern bool g_ota_in_progress;

#endif /* __WIFI_OTA_INTERNAL_H__ */