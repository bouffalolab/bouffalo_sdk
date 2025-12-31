/****************************************************************************
 * Flash RPC Master (AP Core)
 *
 * AP core (Master) receives Flash operation commands from NP core (Remote)
 * and executes them using bflb_flash APIs.
 *
 * Uses XRAM shared memory for ringbuffer communication with NP core.
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "flash_rpc_protocol.h"
#include "flash_rpc_master.h"
#include "ipm.h"
#include "bflb_flash.h"
#include "bl616d_glb.h"
#include "bl616d_hbn.h"
#include "bl616d_aon.h"
#include "bl_sys.h"

#define DBG_TAG "FLASH_RPC"
#include "log.h"

/****************************************************************************
 * Types and Variables
 ****************************************************************************/
static SemaphoreHandle_t flash_op_mutex;            /* Flash operation mutex */
static TaskHandle_t g_flash_rpc_task_handle = NULL; /* RPC task handle */
static volatile bool g_rpc_running = true;          /* RPC task running flag */
static bool g_flash_rpc_master_initialized = false; /* Initialization flag */

/****************************************************************************
 * Flash Operation Handlers
 ****************************************************************************/

/* Handle Flash Erase Command */
static uint8_t handle_flash_erase(flash_cmd_header_t *header, flash_resp_t *resp)
{
    int ret;

    LOG_I("erase entry, addr=0x%08lx, len=%lu\r\n", header->addr, header->len);

    ret = bflb_flash_erase(header->addr, header->len);

    if (ret != 0) {
        LOG_E("erase failed, ret=%d\r\n", ret);
        resp->error_code = ret;
        return FLASH_STATUS_ERROR;
    }

    LOG_I("erase success\r\n");
    resp->actual_len = header->len;
    return FLASH_STATUS_OK;
}

/* Handle Flash Write Command */
static uint8_t handle_flash_write(flash_cmd_header_t *header, uint8_t *data, uint32_t data_len, flash_resp_t *resp)
{
    int ret;
    uint8_t *read_back;

    if (data_len != header->len) {
        LOG_E("write data length mismatch, expected=%lu, got=%lu\r\n", header->len, data_len);
        resp->error_code = -1;
        return FLASH_STATUS_ERROR;
    }

    LOG_I("write addr=0x%08lx, len=%lu\r\n", header->addr, header->len);

    ret = bflb_flash_write(header->addr, data, header->len);
    if (ret != 0) {
        LOG_E("write failed, ret=%d\r\n", ret);
        resp->error_code = ret;
        return FLASH_STATUS_ERROR;
    }

    /* Verify write by reading back */
    read_back = pvPortMalloc(header->len);
    if (read_back != NULL) {
        ret = bflb_flash_read(header->addr, read_back, header->len);
        if (ret != 0) {
            LOG_E("read back failed, ret=%d\r\n", ret);
            vPortFree(read_back);
            resp->error_code = ret;
            return FLASH_STATUS_ERROR;
        }
        vPortFree(read_back);
    }

    resp->actual_len = header->len;
    return FLASH_STATUS_OK;
}

/* Handle Flash Read Command */
static uint8_t handle_flash_read(flash_cmd_header_t *header, flash_resp_t *resp, uint8_t *data_out)
{
    int ret;
    uint8_t *read_data;

    LOG_I("read addr=0x%08lx, len=%lu\r\n", header->addr, header->len);

    read_data = pvPortMalloc(header->len);
    if (read_data == NULL) {
        LOG_E("malloc failed for read\r\n");
        resp->error_code = -1;
        return FLASH_STATUS_ERROR;
    }

    ret = bflb_flash_read(header->addr, read_data, header->len);
    if (ret != 0) {
        LOG_E("read failed, ret=%d\r\n", ret);
        vPortFree(read_data);
        resp->error_code = ret;
        return FLASH_STATUS_ERROR;
    }

    if (data_out != NULL) {
        memcpy(data_out, read_data, header->len);
    }
    vPortFree(read_data);

    resp->actual_len = header->len;
    return FLASH_STATUS_OK;
}

/* Handle System Reboot Command */
static uint8_t handle_flash_reboot(flash_cmd_header_t *header, flash_resp_t *resp, uint8_t *send_buf,
                                   uint32_t *send_len)
{
    (void)header;
    int ret;

    LOG_I("reboot command received\r\n");

    resp->status = FLASH_STATUS_OK;
    resp->actual_len = 0;
    resp->error_code = 0;
    *send_len = sizeof(flash_resp_t);
    memcpy(send_buf, resp, sizeof(flash_resp_t));

    LOG_I("sending reboot response...\r\n");
    ret = simple_rpc_send(send_buf, *send_len);
    if (ret != 0) {
        LOG_E("reboot response send failed, ret=%d\r\n", ret);
    } else {
        LOG_I("reboot response sent OK\r\n");
    }
    *((volatile uint32_t *)0x24000020) |= ((1 << 8));
    bl_sys_reset_por();
    for (volatile int i = 0; i < 10000; i++) {
        __asm__ volatile("nop");
    }

    LOG_I("executing system reset...1\r\n");
    while (1) {}
}

/****************************************************************************
 * RPC Task - Processes incoming commands from NP (via simple_rpc)
 ****************************************************************************/
static void flash_rpc_task(void *param)
{
    (void)param;
    uint8_t recv_buf[512];
    uint32_t recv_len;
    int ret;

    LOG_I("Master task started\r\n");

    while (g_rpc_running) {
        ret = simple_rpc_recv(recv_buf, &recv_len);
        if (ret == 0) {
            flash_cmd_header_t *header;
            flash_resp_t response = { 0 };
            uint8_t status = FLASH_STATUS_ERROR;
            uint8_t send_buf[512];
            uint32_t send_len;
            bool response_sent = false;

            if (recv_len < sizeof(flash_cmd_header_t)) {
                LOG_E("invalid message length %lu\r\n", recv_len);
                continue;
            }

            header = (flash_cmd_header_t *)recv_buf;
            response.seq = header->seq;

            LOG_I("received cmd=%d, seq=%d\r\n", header->cmd, header->seq);

            if (xSemaphoreTake(flash_op_mutex, pdMS_TO_TICKS(5000)) != pdTRUE) {
                LOG_E("failed to get mutex\r\n");
                response.status = FLASH_STATUS_ERROR;
                response.error_code = -EBUSY;
                send_len = sizeof(flash_resp_t);
                memcpy(send_buf, &response, send_len);
                simple_rpc_send(send_buf, send_len);
                continue;
            }

            switch (header->cmd) {
                case FLASH_CMD_ERASE:
                    status = handle_flash_erase(header, &response);
                    send_len = sizeof(flash_resp_t);
                    break;
                case FLASH_CMD_WRITE:
                    status = handle_flash_write(header, recv_buf + sizeof(flash_cmd_header_t),
                                                recv_len - sizeof(flash_cmd_header_t), &response);
                    send_len = sizeof(flash_resp_t);
                    break;
                case FLASH_CMD_READ:
                    status = handle_flash_read(header, &response, send_buf + sizeof(flash_resp_t));
                    send_len = sizeof(flash_resp_t) + header->len;
                    response.status = status;
                    memcpy(send_buf, &response, sizeof(flash_resp_t));
                    break;
                case FLASH_CMD_REBOOT: {
                    handle_flash_reboot(header, &response, send_buf, &send_len);
                    response_sent = true;
                    break;
                }
                default:
                    LOG_E("unknown command %d\r\n", header->cmd);
                    response.error_code = -EINVAL;
                    send_len = sizeof(flash_resp_t);
                    break;
            }
            if (header->cmd != FLASH_CMD_READ && !response_sent) {
                response.status = status;
                memcpy(send_buf, &response, send_len);
            }

            xSemaphoreGive(flash_op_mutex);

            if (!response_sent) {
                LOG_I("sending response, status=%d, seq=%d, len=%lu\r\n", status, response.seq, send_len);
                ret = simple_rpc_send(send_buf, send_len);
                if (ret != 0) {
                    LOG_E("send failed, ret=%d\r\n", ret);
                } else {
                    LOG_I("send OK, ret=%d\r\n", ret);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    LOG_I("Master task exiting\r\n");
    vTaskDelete(NULL);
}

/****************************************************************************
 * Public API - Initialize Flash RPC Master (AP Core)
 ****************************************************************************/

int flash_rpc_master_init(void)
{
    int ret;
    BaseType_t task_ret;

    if (g_flash_rpc_master_initialized) {
        return 0;
    }

    LOG_I("Initializing with XRAM simple_rpc...\n");

    ret = simple_rpc_init();
    if (ret != 0) {
        LOG_E("simple_rpc_init failed, ret=%d\n", ret);
        return ret;
    }

    flash_op_mutex = xSemaphoreCreateMutex();
    if (flash_op_mutex == NULL) {
        LOG_E("failed to create mutex\r\n");
        return -1;
    }

    task_ret =
        xTaskCreate(flash_rpc_task, "flash_rpc_task", 1024, NULL, configMAX_PRIORITIES - 2, &g_flash_rpc_task_handle);
    if (task_ret != pdPASS) {
        LOG_E("failed to create task\r\n");
        vSemaphoreDelete(flash_op_mutex);
        flash_op_mutex = NULL;
        return -1;
    }

    g_flash_rpc_master_initialized = true;
    LOG_I("Master started on AP core (XRAM @ 0x%08lx)\r\n", BL616D_XRAM_BASE);
    return 0;
}

int flash_rpc_master_deinit(void)
{
    LOG_I("Master deinit...\r\n");

    g_rpc_running = false;

    if (flash_op_mutex) {
        vSemaphoreDelete(flash_op_mutex);
        flash_op_mutex = NULL;
    }

    LOG_I("Master deinitialized\r\n");
    return 0;
}
