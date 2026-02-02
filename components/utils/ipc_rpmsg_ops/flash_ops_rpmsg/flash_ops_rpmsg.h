#ifndef __RPMSG_OPS_H__
#define __RPMSG_OPS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <openamp/rpmsg_virtio.h>

#define CALC_CHUNK_SIZE(remaining, max)   ((remaining) > (max) ? (max) : (remaining))

#define RPMSG_FLASH_ERASE_CHUNK_SIZE      (64 * 1024) /* 64KB erase chunk */

/****************************************************************************
 * RPMsg Protocol Constants
 ****************************************************************************/
/* RPMsg protocol header size: src(4) + dst(4) + reserved(4) + len(2) + flags(2) = 16 bytes 
    components/ipc/open-amp/open-amp/lib/rpmsg/rpmsg_internal.h sizeof(rpmsg_hdr)*/
#define RPMSG_HDR_SIZE                    16

/* Available payload space in RPMsg buffer (after RPMsg protocol header) */
#define RPMSG_PAYLOAD_MAX_SIZE             (RPMSG_BUFFER_SIZE - RPMSG_HDR_SIZE)

/****************************************************************************
 * RPMsg FLASH Command Types
 ****************************************************************************/
#define RPMSG_CMD_READ                    0x01 /* SLAVE -> MASTER: Read flash */
#define RPMSG_CMD_ERASE                   0x02 /* SLAVE -> MASTER: Erase flash */
#define RPMSG_CMD_WRITE                   0x03 /* SLAVE -> MASTER: Write flash */
#define RPMSG_CMD_REBOOT                  0x04 /* SLAVE -> MASTER: Reboot system */

/****************************************************************************
 * RPMsg FLASH Status Codes
 ****************************************************************************/
#define RPMSG_STATUS_OK                   0x00
#define RPMSG_STATUS_ERROR                0xFF

#define RPMSG_WAIT_TIMEOUT_MS             50000
/****************************************************************************
 * RPMsg FLASH Message Structures
 ****************************************************************************/

/* Flash Operation Command (used for ERASE, WRITE, READ) */
typedef struct __attribute__((packed)) {
    uint8_t cmd;   /* RPMSG_CMD_READ/WRITE/ERASE */
    uint8_t seq;   /* Sequence number */
    uint32_t addr; /* Flash address */
    uint32_t len;  /* Length */
} rpmsg_flash_op_t;

/* Flash Response */
typedef struct __attribute__((packed)) {
    uint8_t cmd;    /* Echo back command type */
    uint8_t seq;    /* Echo back sequence number */
    uint8_t status; /* RPMSG_STATUS_* */
} rpmsg_response_t;


typedef struct {
    uint8_t data[RPMSG_PAYLOAD_MAX_SIZE];
    uint32_t len;
} command_queue_item_t;

/****************************************************************************
 * SLAVE Core: RPMsg Flash Operations
 *
 * These functions allow SLAVE to call MASTER's flash operations via RPMsg
 ****************************************************************************/
int bflb_flash_erase_rpmsg(uint32_t addr, uint32_t len);
int bflb_flash_write_rpmsg(uint32_t addr, const uint8_t *data, uint32_t len);
int bflb_flash_read_rpmsg(uint32_t addr, uint8_t *data, uint32_t len);
int bflb_reboot_rpmsg(void);

/**
 * @brief Initialize RPMsg on Flash Core
 *
 * Sets up RPMsg communication and flash operation handlers.
 * Flash Core receives flash operation requests from SLAVE.
 *
 * @return 0 on success, negative error code on failure
 */
int rpmsg_master_init(void);

/**
 * @brief Deinitialize RPMsg on Flash Core
 *
 * @return 0 on success, negative error code on failure
 */
int rpmsg_master_deinit(void);

/**
 * @brief Initialize RPMsg on Receiver Core (Slave)
 *
 * @return 0 on success, negative error code on failure
 */
int rpmsg_slave_init(void);

/**
 * @brief Deinitialize RPMsg on Receiver Core (Slave)
 *
 * @return 0 on success, negative error code on failure
 */
int rpmsg_slave_deinit(void);

/**
 * @brief Read from flash via RPMsg
 *
 * @param addr Flash address to read
 * @param data Buffer to store read data
 * @param len Length to read in bytes
 * @return 0 on success, negative error code on failure
 */
int bflb_flash_read_rpmsg(uint32_t addr, uint8_t *data, uint32_t len);

/**
 * @brief Erase flash via RPMsg
 *
 * @param addr Flash address to erase
 * @param len Length to erase in bytes
 * @return 0 on success, negative error code on failure
 */
int bflb_flash_erase_rpmsg(uint32_t addr, uint32_t len);

/**
 * @brief Write to flash via RPMsg
 *
 * @param addr Flash address to write
 * @param data Data buffer
 * @param len Length to write in bytes
 * @return 0 on success, negative error code on failure
 */
int bflb_flash_write_rpmsg(uint32_t addr, const uint8_t *data, uint32_t len);

/**
 * @brief Reboot system via RPMsg (SLAVE calls MASTER to reboot)
 *
 * SLAVE sends reboot command to MASTER, MASTER suspends SLAVE and reboots the system.
 *
 * @return 0 on success, negative error code on failure
 */
int bflb_reboot_rpmsg(void);

/****************************************************************************
 * User Endpoints for Customer Use
 *
 * These endpoints are registered on both MASTER and SLAVE cores for
 * custom application communication. Customers can implement their own
 * callback handlers in rpmsg_user1_cb() and rpmsg_user2_cb().
 ****************************************************************************/

/**
 * @brief Send data via user endpoint 1
 *
 * This function is available on both MASTER and SLAVE cores.
 * The endpoint name is "user_ep1" on both sides.
 *
 * @param data Data buffer to send
 * @param len Length of data
 * @return Number of bytes sent on success, negative error code on failure
 */
int rpmsg_send_user_ep1(const void *data, size_t len);

/**
 * @brief Send data via reserved endpoint 2
 *
 * This function is available on both MASTER and SLAVE cores.
 * The endpoint name is "reserved_ep2" on both sides.
 *
 * @param data Data buffer to send
 * @param len Length of data
 * @return Number of bytes sent on success, negative error code on failure
 */
int rpmsg_send_user_ep2(const void *data, size_t len);

#endif
#ifdef __cplusplus
}
#endif