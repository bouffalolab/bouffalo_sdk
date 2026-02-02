#include <string.h>
#include <stdbool.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "bflb_multi_core_sync.h"
#include <rpmsg_service.h>
#include "flash_ops_rpmsg.h"

#include "log.h"
#define DBG_TAG "RPMSG_MASTER"

/****************************************************************************
 * Global Variables (defined in rpmsg_master_init.c, declared extern here)
 ****************************************************************************/
extern SemaphoreHandle_t g_rpmsg_mutex;
extern TaskHandle_t g_rpmsg_task_handle;
extern volatile bool g_rpmsg_running;
extern bool g_rpmsg_initialized;
extern int g_flash_endpoint_id;
extern QueueHandle_t g_command_queue;

/* Send response to SLAVE */
int rpmsg_send_response(uint8_t cmd, uint8_t seq, uint8_t status)
{
    rpmsg_response_t resp;

    resp.cmd = cmd;
    resp.seq = seq;
    resp.status = status;

    int ret = rpmsg_service_send(g_flash_endpoint_id, (uint8_t *)&resp, sizeof(resp));
    return ret;
}

static int handle_flash_erase(uint8_t *recv_buf, uint32_t recv_len)
{
    int ret;
    rpmsg_flash_op_t *erase_cmd = (rpmsg_flash_op_t *)recv_buf;

    if (recv_len < sizeof(rpmsg_flash_op_t)) {
        return -EINVAL;
    }

    LOG_I("FLASH ERASE: addr=0x%08lx, len=%lu\r\n", erase_cmd->addr, erase_cmd->len);

    uint32_t erase_addr = erase_cmd->addr;
    uint32_t remaining = erase_cmd->len;
    uint32_t chunk_size;
    uint32_t total_erased = 0;

    while (remaining > 0) {
        chunk_size = CALC_CHUNK_SIZE(remaining, RPMSG_FLASH_ERASE_CHUNK_SIZE);

        LOG_I("Erasing chunk: addr=0x%08lx, len=%lu, total=%lu/%lu\r\n", erase_addr, chunk_size, total_erased,
              erase_cmd->len);

        ret = bflb_flash_erase_mcs(erase_addr, chunk_size);
        if (ret != 0) {
            LOG_E("Flash erase failed at addr=0x%08lx: ret=%d\r\n", erase_addr, ret);
            rpmsg_send_response(erase_cmd->cmd, erase_cmd->seq, RPMSG_STATUS_ERROR);
            return ret;
        }

        erase_addr += chunk_size;
        remaining -= chunk_size;
        total_erased += chunk_size;
    }

    LOG_I("Flash erase completed: total=%lu bytes\r\n", total_erased);

    rpmsg_send_response(erase_cmd->cmd, erase_cmd->seq, RPMSG_STATUS_OK);
    return 0;
}

/* Handle Flash READ command */
static int handle_flash_read(uint8_t *recv_buf, uint32_t recv_len)
{
    int ret;
    rpmsg_flash_op_t *read_cmd = (rpmsg_flash_op_t *)recv_buf;

    if (recv_len < sizeof(rpmsg_flash_op_t)) {
        return -EINVAL;
    }

    uint8_t *read_data = pvPortMalloc(read_cmd->len);
    if (read_data == NULL) {
        rpmsg_send_response(read_cmd->cmd, read_cmd->seq, RPMSG_STATUS_ERROR);
        return -ENOMEM;
    }
    ret = bflb_flash_read_mcs(read_cmd->addr, read_data, read_cmd->len);
    if (ret != 0) {
        vPortFree(read_data);
        rpmsg_send_response(read_cmd->cmd, read_cmd->seq, RPMSG_STATUS_ERROR);
        return ret;
    }

    /* Send response with data (max 489 bytes per RPMsg message) */
    uint8_t send_buf[4];
    rpmsg_response_t *resp = (rpmsg_response_t *)send_buf;
    uint32_t max_data = RPMSG_PAYLOAD_MAX_SIZE - sizeof(rpmsg_response_t);
    uint32_t data_len = (read_cmd->len > max_data) ? max_data : read_cmd->len;

    resp->cmd = read_cmd->cmd;
    resp->seq = read_cmd->seq;
    resp->status = RPMSG_STATUS_OK;
    memcpy(send_buf + sizeof(rpmsg_response_t), read_data, data_len);

    ret = rpmsg_service_send(g_flash_endpoint_id, send_buf, sizeof(rpmsg_response_t) + data_len);
    if (ret < 0) {
        LOG_E("Failed to send read response, ret=%d\r\n", ret);
        vPortFree(read_data);
        return ret;
    }

    vPortFree(read_data);
    return 0;
}

/* Handle Flash WRITE command */
static int handle_flash_write(uint8_t *recv_buf, uint32_t recv_len)
{
    int ret;
    rpmsg_flash_op_t *write_cmd = (rpmsg_flash_op_t *)recv_buf;

    uint8_t *data = recv_buf + sizeof(rpmsg_flash_op_t);
    uint32_t data_len = recv_len - sizeof(rpmsg_flash_op_t);
    if (recv_len < sizeof(rpmsg_flash_op_t)) {
        return -EINVAL;
    }

    LOG_I("FLASH WRITE: addr=0x%08lx, len=%lu\r\n", write_cmd->addr, (unsigned long)data_len);

    ret = bflb_flash_write_mcs(write_cmd->addr, data, data_len);
    if (ret != 0) {
        LOG_E("Flash write failed: ret=%d\r\n", ret);
        rpmsg_send_response(write_cmd->cmd, write_cmd->seq, RPMSG_STATUS_ERROR);
        return ret;
    }

    rpmsg_send_response(write_cmd->cmd, write_cmd->seq, RPMSG_STATUS_OK);
    return 0;
}



/* Handle REBOOT command */
static int handle_reboot(void)
{
    LOG_I("REBOOT: System rebooting...\r\n");
    bflb_sys_reboot_mcs();
    while (1) {}

    return 0;
}

/* RPMsg Endpoint Callback for Flash Operations */
int rpmsg_endpoint_flash_cb(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv)
{
    (void)ept;
    (void)src;
    (void)priv;

    if (len == 1 && *(uint8_t *)data == 0x5A) {
        LOG_I("Received handshake response from SLAVE - connection established\r\n");
        return RPMSG_SUCCESS;
    }

    uint8_t cmd_type = *(uint8_t *)data;

    bool is_valid_cmd = (cmd_type == RPMSG_CMD_READ) || (cmd_type == RPMSG_CMD_ERASE) ||
                        (cmd_type == RPMSG_CMD_WRITE) || (cmd_type == RPMSG_CMD_REBOOT);
    if (!is_valid_cmd) {
        return RPMSG_SUCCESS;
    }

    command_queue_item_t item;
    memcpy(item.data, data, len);
    item.len = len;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (xPortIsInsideInterrupt()) {
        if (xQueueSendFromISR(g_command_queue, &item, &xHigherPriorityTaskWoken) != pdTRUE) {
            LOG_E("Command queue full (ISR)\r\n");
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        if (xQueueSend(g_command_queue, &item, 0) != pdTRUE) {
            LOG_E("Command queue full\r\n");
        }
    }

    return RPMSG_SUCCESS;
}

void rpmsg_task(void *param)
{
    (void)param;
    command_queue_item_t queue_item;
    int ret;

    LOG_I("RPMsg task started on Master\r\n");

    vTaskDelay(pdMS_TO_TICKS(500));
    LOG_I("Sending handshake to SLAVE...\r\n");

    uint8_t shakehand = 0xA5;
    int handshake_retry = 0;
    const int max_handshake_retries = 3;

    for (handshake_retry = 0; handshake_retry < max_handshake_retries; handshake_retry++) {
        ret = rpmsg_service_send(g_flash_endpoint_id, &shakehand, sizeof(shakehand));
        if (ret >= 0) {
            LOG_I("Handshake sent to SLAVE successfully (attempt %d) \r\n", handshake_retry + 1);
            break;
        } else {
            LOG_W("Handshake failed, ret=%d, retrying in 1s...\r\n", ret);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

    if (handshake_retry >= max_handshake_retries) {
        LOG_E("Failed to send handshake after %d attempts\r\n", max_handshake_retries);
    }

    while (g_rpmsg_running) {
        if (xQueueReceive(g_command_queue, &queue_item, pdMS_TO_TICKS(100)) == pdTRUE) {
            uint8_t msg_type = *(uint8_t *)queue_item.data;
            if (xSemaphoreTake(g_rpmsg_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
                LOG_W("Failed to get mutex, skipping message\r\n");
                continue;
            }

            switch (msg_type) {
                case RPMSG_CMD_READ:
                    handle_flash_read(queue_item.data, queue_item.len);
                    break;

                case RPMSG_CMD_ERASE:
                    handle_flash_erase(queue_item.data, queue_item.len);
                    break;

                case RPMSG_CMD_WRITE:
                    handle_flash_write(queue_item.data, queue_item.len);
                    break;

                case RPMSG_CMD_REBOOT:
                    handle_reboot();
                    break;

                default:
                    LOG_E("Unknown command: %d\r\n", msg_type);
                    break;
            }

            xSemaphoreGive(g_rpmsg_mutex);
        }
    }

    LOG_I("RPMsg task exiting\r\n");

    vTaskDelete(NULL);
}
