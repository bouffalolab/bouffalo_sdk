#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "flash_ops_rpmsg.h"
#include <rpmsg_service.h>
#include <openamp/rpmsg_virtio.h>
#include "log.h"

/****************************************************************************
 * RPMsg Buffer Size Calculations
 ****************************************************************************/
/* Maximum data chunk size for read operations (with response header) */
#define RPMSG_READ_MAX_DATA     (RPMSG_PAYLOAD_MAX_SIZE - sizeof(rpmsg_response_t))

/* Maximum data chunk size for write operations (with flash op header) */
#define RPMSG_WRITE_MAX_DATA    (RPMSG_PAYLOAD_MAX_SIZE - sizeof(rpmsg_flash_op_t))

/****************************************************************************
 * Types and Variables
 ****************************************************************************/
/* Variables defined in rpmsg_slave_init.c (extern declarations) */
extern SemaphoreHandle_t g_rpmsg_mutex;
extern SemaphoreHandle_t g_response_sem;
extern bool g_rpmsg_initialized;
extern int g_flash_endpoint_id;
extern int g_user_ep1_id;
extern int g_user_ep2_id;

/* Local static variables */
static uint8_t g_seq_counter = 0;

/* Endpoint IDs - defined in rpmsg_slave_init.c */

/* Last response received from MASTER */
static rpmsg_response_t g_last_response = { 0 };

/* Buffer for READ response data (up to RPMSG_READ_MAX_DATA bytes) */
static uint8_t g_read_data_buf[RPMSG_READ_MAX_DATA];
static uint32_t g_read_data_len = 0;

/* Track if SLAVE has already sent handshake response */
static bool g_handshake_response_sent = false;
static bool g_handshake_received_from_master = false;

/****************************************************************************
 * Function Declarations
 ****************************************************************************/
static int wait_and_check_response(rpmsg_flash_op_t *tx_cmd, bool check_data_len_flag, uint32_t expected_data_len);

/****************************************************************************
 * RPMsg Flash Operations (SLAVE calls these to access MASTER's flash)
 ****************************************************************************/

int bflb_flash_read_rpmsg(uint32_t addr, uint8_t *data, uint32_t len)
{
    if (!g_rpmsg_initialized) {
        return -EINVAL;
    }

    uint8_t rpmsg_send_buf[sizeof(rpmsg_flash_op_t)];

    /* Available for payload: RPMSG_BUFFER_SIZE - sizeof(rpmsg_hdr)
     * We need space for our response header: sizeof(rpmsg_response_t)
     * Max data per chunk: RPMSG_READ_MAX_DATA (currently 489 bytes)
     */
    uint32_t offset = 0;
    uint32_t max_chunk = RPMSG_READ_MAX_DATA;

    while (offset < len) {
        uint32_t chunk_size = CALC_CHUNK_SIZE(len - offset, max_chunk);

        /* Get mutex */
        if (xSemaphoreTake(g_rpmsg_mutex, pdMS_TO_TICKS(5000)) != pdTRUE) {
            return -EBUSY;
        }

        rpmsg_flash_op_t *tx_cmd = (rpmsg_flash_op_t *)rpmsg_send_buf;

        tx_cmd->cmd = RPMSG_CMD_READ;
        tx_cmd->seq = ++g_seq_counter;
        tx_cmd->addr = addr + offset;
        tx_cmd->len = chunk_size;

        /* Send command */
        int ret = rpmsg_service_send(g_flash_endpoint_id, rpmsg_send_buf, sizeof(rpmsg_flash_op_t));
        if (ret < 0) {
            xSemaphoreGive(g_rpmsg_mutex);
            return ret;
        }

        /* Wait for response and validate */
        ret = wait_and_check_response(tx_cmd, 1, chunk_size);
        if (ret < 0) {
            xSemaphoreGive(g_rpmsg_mutex);
            return ret;
        }

        memcpy(data + offset, g_read_data_buf, g_read_data_len);

        xSemaphoreGive(g_rpmsg_mutex);

        offset += chunk_size;
    }

    return 0;
}

/****************************************************************************
 * RPMsg Flash Operations for SLAVE Core
 *
 * SLAVE calls these functions to execute flash operations on MASTER side
 ****************************************************************************/

/**
 * @brief Erase flash on MASTER via RPMsg
 * @param addr Flash address to erase
 * @param len Length to erase in bytes
 * @return 0 on success, negative error code on failure
 */
int bflb_flash_erase_rpmsg(uint32_t addr, uint32_t len)
{
    if (!g_rpmsg_initialized) {
        return -EINVAL;
    }

    uint8_t rpmsg_send_buf[sizeof(rpmsg_flash_op_t)];

    LOG_I("RPMsg flash erase: addr=0x%08lx, len=%lu\r\n", addr, len);

    if (xSemaphoreTake(g_rpmsg_mutex, pdMS_TO_TICKS(5000)) != pdTRUE) {
        return -EBUSY;
    }

    rpmsg_flash_op_t *tx_cmd = (rpmsg_flash_op_t *)rpmsg_send_buf;

    tx_cmd->cmd = RPMSG_CMD_ERASE;
    tx_cmd->seq = ++g_seq_counter;
    tx_cmd->addr = addr;
    tx_cmd->len = len;

    /* Send erase command */
    int ret = rpmsg_service_send(g_flash_endpoint_id, rpmsg_send_buf, sizeof(rpmsg_flash_op_t));
    if (ret < 0) {
        xSemaphoreGive(g_rpmsg_mutex);
        return ret;
    }

    /* Wait for response and validate */
    ret = wait_and_check_response(tx_cmd, 0, 0);
    if (ret < 0) {
        xSemaphoreGive(g_rpmsg_mutex);
        return ret;
    }

    xSemaphoreGive(g_rpmsg_mutex);

    LOG_I("RPMsg flash erase completed\r\n");
    return 0;
}

/**
 * @brief Write flash on MASTER via RPMsg
 * @param addr Flash address to write
 * @param data Data to write
 * @param len Length to write in bytes
 * @return 0 on success, negative error code on failure
 */
int bflb_flash_write_rpmsg(uint32_t addr, const uint8_t *data, uint32_t len)
{
    if (!g_rpmsg_initialized) {
        return -EINVAL;
    }

    /* Max payload per RPMsg message: RPMSG_PAYLOAD_MAX_SIZE bytes
     * rpmsg_flash_op_t = 10 bytes (cmd+seq+addr+len)
     * Max data: RPMSG_WRITE_MAX_DATA bytes (currently 486 with 512-byte buffer)
     */
    uint8_t rpmsg_send_buf[RPMSG_PAYLOAD_MAX_SIZE];

    LOG_I("RPMsg flash write: addr=0x%08lx, len=%lu\r\n", addr, len);

    /* Split into chunks if needed */
    uint32_t max_data = RPMSG_WRITE_MAX_DATA;
    uint32_t offset = 0;

    while (offset < len) {
        uint32_t chunk_size = CALC_CHUNK_SIZE(len - offset, max_data);

        if (xSemaphoreTake(g_rpmsg_mutex, pdMS_TO_TICKS(5000)) != pdTRUE) {
            return -EBUSY;
        }

        rpmsg_flash_op_t *tx_cmd = (rpmsg_flash_op_t *)rpmsg_send_buf;

        tx_cmd->cmd = RPMSG_CMD_WRITE;
        tx_cmd->seq = ++g_seq_counter;
        tx_cmd->addr = addr + offset;
        tx_cmd->len = chunk_size;

        memcpy(rpmsg_send_buf + sizeof(rpmsg_flash_op_t), data + offset, chunk_size);

        /* Send write command */
        uint32_t send_len = sizeof(rpmsg_flash_op_t) + chunk_size;
        int ret = rpmsg_service_send(g_flash_endpoint_id, rpmsg_send_buf, send_len);
        if (ret < 0) {
            xSemaphoreGive(g_rpmsg_mutex);
            return ret;
        }

        /* Wait for response and validate */
        ret = wait_and_check_response(tx_cmd, 0, 0);
        if (ret < 0) {
            xSemaphoreGive(g_rpmsg_mutex);
            return ret;
        }

        xSemaphoreGive(g_rpmsg_mutex);

        offset += chunk_size;
    }

    LOG_I("RPMsg flash write completed\r\n");
    return 0;
}

/***************************************************************************** * RPMsg Endpoint Callback - Receives responses from MASTER
 ****************************************************************************/
int rpmsg_endpoint_flash_cb(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv)
{
    (void)ept;
    (void)src;
    (void)priv;

    /* Handle handshake (single byte 0xA5) */
    if (len == 1 && *(uint8_t *)data == 0xA5) {
        LOG_I("Received handshake from MASTER\r\n");
        g_handshake_received_from_master = true;

        /* Check if endpoint is bound before sending response */
        if (rpmsg_service_endpoint_is_bound(g_flash_endpoint_id)) {
            uint8_t handshake_response = 0x5A;
            int ret = rpmsg_service_send(g_flash_endpoint_id, &handshake_response, sizeof(handshake_response));
            if (ret < 0) {
                LOG_E("Failed to send handshake response, ret=%d\r\n", ret);
            } else {
                LOG_I("Sent handshake response to MASTER\r\n");
                g_handshake_response_sent = true;
            }
        } else {
            LOG_W("Endpoint not bound yet, will send response later\r\n");
        }
        return RPMSG_SUCCESS;
    }

    if (len >= sizeof(rpmsg_response_t)) {
        rpmsg_response_t *resp = (rpmsg_response_t *)data;

        /* Validate command type - only READ, ERASE, WRITE, REBOOT are valid */
        if (resp->cmd != RPMSG_CMD_READ && resp->cmd != RPMSG_CMD_ERASE && resp->cmd != RPMSG_CMD_WRITE &&
            resp->cmd != RPMSG_CMD_REBOOT) {
            LOG_W("Ignoring invalid cmd=%u\r\n", resp->cmd);
            return RPMSG_SUCCESS;
        }

        memcpy(&g_last_response, resp, sizeof(rpmsg_response_t));

        if (resp->cmd == RPMSG_CMD_READ && len > sizeof(rpmsg_response_t)) {
            g_read_data_len = len - sizeof(rpmsg_response_t);
            uint8_t *data_ptr = (uint8_t *)data + sizeof(rpmsg_response_t);
            memcpy(g_read_data_buf, data_ptr, g_read_data_len);
        }

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        if (xPortIsInsideInterrupt()) {
            xSemaphoreGiveFromISR(g_response_sem, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } else {
            xSemaphoreGive(g_response_sem);
        }
    }

    return RPMSG_SUCCESS;
}

/****************************************************************************
 * Helper Functions
 ****************************************************************************/

/**
 * @brief Wait for RPMsg response and validate it
 * @param tx_cmd Pointer to the command that was sent (for seq check)
 * @param check_data_len Whether to validate data length (for READ operations)
 * @param expected_data_len Expected data length (if check_data_len is true)
 * @return 0 on success, negative error code on failure
 */
static int wait_and_check_response(rpmsg_flash_op_t *tx_cmd, bool check_data_len_flag, uint32_t expected_data_len)
{
    TickType_t timeout_ticks = pdMS_TO_TICKS(RPMSG_WAIT_TIMEOUT_MS);

    /* Wait for response */
    if (xSemaphoreTake(g_response_sem, timeout_ticks) != pdTRUE) {
        LOG_E("Timeout waiting for response (seq=%d)\r\n", tx_cmd->seq);
        return -ETIMEDOUT;
    }

    /* Check sequence number */
    if (g_last_response.seq != tx_cmd->seq) {
        LOG_E("Seq mismatch: expected=%d, got=%d\r\n", tx_cmd->seq, g_last_response.seq);
        return -EIO;
    }

    /* Check status */
    if (g_last_response.status != RPMSG_STATUS_OK) {
        LOG_E("Response status error: %d\r\n", g_last_response.status);
        return -g_last_response.status;
    }

    /* Check data length (for READ operations) */
    if (check_data_len_flag && g_read_data_len != expected_data_len) {
        LOG_E("Data len mismatch: expected=%lu, got=%lu\r\n", expected_data_len, g_read_data_len);
        return -EIO;
    }

    return 0;
}


/**
 * @brief Reboot system via RPMsg
 *
 * SLAVE sends reboot command to MASTER, which suspends SLAVE and reboots the system.
 * Note: This function may not return if reboot succeeds.
 *
 * @return 0 on success (reboot initiated), negative error code on failure
 */
int bflb_reboot_rpmsg(void)
{
    if (!g_rpmsg_initialized) {
        return -EINVAL;
    }

    static uint8_t rpmsg_send_buf[sizeof(rpmsg_flash_op_t)];

    LOG_I("SLAVE: Requesting reboot via RPMsg\r\n");

    if (xSemaphoreTake(g_rpmsg_mutex, pdMS_TO_TICKS(5000)) != pdTRUE) {
        return -EBUSY;
    }

    rpmsg_flash_op_t *tx_cmd = (rpmsg_flash_op_t *)rpmsg_send_buf;

    tx_cmd->cmd = RPMSG_CMD_REBOOT;
    tx_cmd->seq = ++g_seq_counter;
    tx_cmd->addr = 0;
    tx_cmd->len = 0;

    /* Send reboot command */
    int ret = rpmsg_service_send(g_flash_endpoint_id, rpmsg_send_buf, sizeof(rpmsg_flash_op_t));
    if (ret < 0) {
        xSemaphoreGive(g_rpmsg_mutex);
        return ret;
    }

    /* Wait for response (optional - reboot may happen before we get response) */
    TickType_t timeout_ticks = pdMS_TO_TICKS(RPMSG_WAIT_TIMEOUT_MS);
    if (xSemaphoreTake(g_response_sem, timeout_ticks) != pdTRUE) {
        LOG_W("SLAVE: Timeout waiting for reboot response, system may be rebooting\r\n");
        /* Continue anyway - system might be in process of rebooting */
    } else {
        /* Check response */
        if (g_last_response.seq == tx_cmd->seq && g_last_response.status == RPMSG_STATUS_OK) {
            LOG_I("SLAVE: Reboot command acknowledged by MASTER, system rebooting\r\n");
        }
    }

    xSemaphoreGive(g_rpmsg_mutex);

    LOG_I("SLAVE: Reboot request sent\r\n");
    return 0;
}

