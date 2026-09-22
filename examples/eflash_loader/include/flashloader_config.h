#ifndef FLASHLOADER_CONFIG_H
#define FLASHLOADER_CONFIG_H

#include <stdint.h>

#define FLASHLOADER_DESCRIPTOR_MAGIC   0x4A4C4642UL /* "BFLJ" */
#define FLASHLOADER_PROTOCOL_VERSION   1UL

#define FLASHLOADER_CAP_FLASH          (1UL << 0)
#define FLASHLOADER_CAP_FLASH_PARAMETER (1UL << 1)
#define FLASHLOADER_CAP_FLASH_XZ       (1UL << 2)
#define FLASHLOADER_CAP_FLASH_OTP      (1UL << 3)
#define FLASHLOADER_CAP_EFUSE          (1UL << 4)
#define FLASHLOADER_CAP_RESET          (1UL << 5)
#define FLASHLOADER_CAP_FLASH_XIP      (1UL << 6)
#define FLASHLOADER_CAP_HOST_ACK       (1UL << 7)

#include "flashloader_port_config.h"

#if ((FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_HOST_ACK) != 0U) && \
    ((FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_RESET) == 0U)
#error "eFlash Loader host acknowledgement requires reset capability"
#endif

#ifndef FLASHLOADER_REQUEST_BUFFER_SIZE
#define FLASHLOADER_REQUEST_BUFFER_SIZE FLASHLOADER_SHARED_SIZE
#endif

#ifndef FLASHLOADER_RESPONSE_BUFFER_SIZE
#define FLASHLOADER_RESPONSE_BUFFER_SIZE FLASHLOADER_SHARED_SIZE
#endif

#define FLASHLOADER_MAX_PAYLOAD (FLASHLOADER_REQUEST_BUFFER_SIZE - 4U)

#define FLASHLOADER_HOST_READY         0x59445248UL /* "HRDY" */
#define FLASHLOADER_SERVER_ACK         0x4B434153UL /* "SACK" */

#ifndef FLASHLOADER_HOST_ACK_TIMEOUT_MS
#define FLASHLOADER_HOST_ACK_TIMEOUT_MS 1000U
#endif

#ifndef FLASHLOADER_SHA_CHUNK_SIZE
#define FLASHLOADER_SHA_CHUNK_SIZE     4096U
#endif

#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_FLASH_XZ) != 0U
#ifndef FLASHLOADER_XZ_OUTPUT_SIZE
#define FLASHLOADER_XZ_OUTPUT_SIZE     4096U
#endif
#ifndef FLASHLOADER_XZ_HEAP_SIZE
#define FLASHLOADER_XZ_HEAP_SIZE       (80U * 1024U)
#endif
#endif

#define FLASHLOADER_FLASH_SECTOR_SIZE  4096U

#if (FLASHLOADER_MAILBOX_ADDR & 3U) != 0U
#error "eFlash Loader mailbox must be word aligned"
#endif

#if (FLASHLOADER_DESCRIPTOR_ADDR & 3U) != 0U
#error "eFlash Loader descriptor must be word aligned"
#endif

#if (FLASHLOADER_DATA_ADDR & 3U) != 0U
#error "eFlash Loader data area must be word aligned"
#endif

#if FLASHLOADER_SHARED_SIZE < 4U
#error "eFlash Loader data area is too small"
#endif

#if (FLASHLOADER_REQUEST_BUFFER_SIZE < 4U) || \
    (FLASHLOADER_REQUEST_BUFFER_SIZE > FLASHLOADER_SHARED_SIZE)
#error "eFlash Loader request buffer size is invalid"
#endif

#if (FLASHLOADER_RESPONSE_BUFFER_SIZE < 4U) || \
    (FLASHLOADER_RESPONSE_BUFFER_SIZE > FLASHLOADER_SHARED_SIZE)
#error "eFlash Loader response buffer size is invalid"
#endif

#if FLASHLOADER_MAILBOX_ADDR < FLASHLOADER_RESERVED_RAM_START
#error "eFlash Loader mailbox is outside the reserved RAM region"
#endif

#if FLASHLOADER_DESCRIPTOR_ADDR < FLASHLOADER_RESERVED_RAM_START
#error "eFlash Loader descriptor is outside the reserved RAM region"
#endif

#if (FLASHLOADER_DESCRIPTOR_ADDR + 16U) > FLASHLOADER_RESERVED_RAM_END
#error "eFlash Loader descriptor exceeds the reserved RAM region"
#endif

#if (FLASHLOADER_MAILBOX_ADDR + 4U) > FLASHLOADER_RESERVED_RAM_END
#error "eFlash Loader mailbox exceeds the reserved RAM region"
#endif

#if FLASHLOADER_DATA_ADDR < FLASHLOADER_RESERVED_RAM_START
#error "eFlash Loader data area is outside the reserved RAM region"
#endif

#if (FLASHLOADER_DATA_ADDR + FLASHLOADER_SHARED_SIZE) > FLASHLOADER_RESERVED_RAM_END
#error "eFlash Loader data area exceeds the reserved RAM region"
#endif

#if ((FLASHLOADER_MAILBOX_ADDR < (FLASHLOADER_DATA_ADDR + FLASHLOADER_SHARED_SIZE)) && \
     ((FLASHLOADER_MAILBOX_ADDR + 4U) > FLASHLOADER_DATA_ADDR))
#error "eFlash Loader mailbox overlaps the data area"
#endif

#if ((FLASHLOADER_DESCRIPTOR_ADDR < (FLASHLOADER_MAILBOX_ADDR + 4U)) && \
     ((FLASHLOADER_DESCRIPTOR_ADDR + 16U) > FLASHLOADER_MAILBOX_ADDR))
#error "eFlash Loader descriptor overlaps the mailbox"
#endif

#if ((FLASHLOADER_DESCRIPTOR_ADDR < (FLASHLOADER_DATA_ADDR + FLASHLOADER_SHARED_SIZE)) && \
     ((FLASHLOADER_DESCRIPTOR_ADDR + 16U) > FLASHLOADER_DATA_ADDR))
#error "eFlash Loader descriptor overlaps the data area"
#endif

#endif
