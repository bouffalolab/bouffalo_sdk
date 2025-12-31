/****************************************************************************
 * Flash RPC Protocol for Dual-Core OTA Communication
 *
 * This protocol enables NP core to request AP core to execute Flash operations
 * (erase/write/read) via XRAM shared memory ringbuffer.
 *
 * Uses BL616D's 16KB XRAM as shared memory between AP and NP cores.
 ****************************************************************************/

#ifndef __FLASH_RPC_PROTOCOL_H__
#define __FLASH_RPC_PROTOCOL_H__

#include <stdint.h>
#include "ipm.h"  /* For RB_ENTRY_SIZE */

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * Flash Command Types
 ****************************************************************************/
#define FLASH_CMD_ERASE    0x01
#define FLASH_CMD_WRITE    0x02
#define FLASH_CMD_READ     0x03
#define FLASH_CMD_REBOOT   0x04

/* Response status codes */
#define FLASH_STATUS_OK    0x00
#define FLASH_STATUS_ERROR 0xFF
#define FLASH_MAX_DATA_SIZE  (RB_ENTRY_SIZE - sizeof(uint32_t) - sizeof(flash_cmd_header_t))

/****************************************************************************
 * Command Message Structure
 ****************************************************************************/
typedef struct __attribute__((packed)) {
    uint8_t  cmd;        /* Command type: FLASH_CMD_* */
    uint8_t  seq;        /* Sequence number for tracking requests/responses */
    uint32_t addr;       /* Flash address */
    uint32_t len;        /* Data length */
    uint32_t offset;     /* Offset for multi-packet operations */
} flash_cmd_header_t;

/****************************************************************************
 * Response Message Structure
 ****************************************************************************/
typedef struct __attribute__((packed)) {
    uint8_t  status;     /* FLASH_STATUS_OK or FLASH_STATUS_ERROR */
    uint8_t  seq;        /* Echo back the sequence number */
    uint32_t actual_len; /* Actual bytes processed */
    int32_t  error_code; /* Error code if status != OK */
} flash_resp_t;

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_RPC_PROTOCOL_H__ */
