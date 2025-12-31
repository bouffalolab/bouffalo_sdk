/****************************************************************************
 * Flash RPC Remote (NP Core)
 *
 * NP core (Remote) sends Flash operation commands to AP core (Master)
 * and receives results via XRAM shared memory ringbuffer.
 *
 * Uses XRAM shared memory for communication - no OpenAMP/RPMsg needed.
 *
 * IMPORTANT: When AP is erasing flash, NP cannot execute code from Flash (XIP).
 * Solution: Poll IPC interrupt status register directly to detect when AP completes
 * the operation. This avoids calling functions that may be in Flash during erase.
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <bflb_ipc.h>
#include <compiler/compiler_ld.h>

#ifdef CONFIG_WIFI_OTA_DUAL_CORE
#include <hardware/bl616d.h>
#endif

#include "flash_rpc_protocol.h"
#include "flash_rpc_remote.h"
#include "ipm.h"

#define DBG_TAG "FLASH_RPC"
#include "log.h"

#define IPC_AP2NP_STATUS_OFFSET 0x3C /* NP reads this to see AP's triggers */
#define IPC_AP2NP_ACK_OFFSET    0x28 /* NP writes here to clear AP's triggers */

/* IPC base address for BL616D */
#define IPC0_BASE               0x20013000

/* Direct IPC register access macros (inline, no function calls) */
#define IPC_GET_INTSTA()        (*(volatile uint32_t *)(IPC0_BASE + IPC_AP2NP_STATUS_OFFSET))
#define IPC_CLEAR_INT(bits)     (*(volatile uint32_t *)(IPC0_BASE + IPC_AP2NP_ACK_OFFSET) = (bits))
/* Polling for AP erase the flash eventually */
ATTR_TCM_SECTION
int wait_for_ipc_response(volatile uint32_t *reg_addr, uint32_t mask, uint32_t expected, uintptr_t *out_irq_flag)
{
    uint32_t timeout_count = 0;
    uint32_t value;
    uintptr_t irq_flag;

    irq_flag = bflb_irq_save();

    while (timeout_count < 200000) {
        value = *reg_addr;

        if ((value & mask) == expected) {
            *out_irq_flag = irq_flag;
            return 0;
        }

        timeout_count++;

        for (volatile int i = 0; i < 50000; i++) {
            __asm__ volatile("nop");
        }
    }

    bflb_irq_restore(irq_flag);
    return -ETIMEDOUT;
}

/****************************************************************************
 * Restore interrupts after reading ringbuffer
 ****************************************************************************/
ATTR_TCM_SECTION
void wait_for_ipc_response_restore(uintptr_t irq_flag)
{
    bflb_irq_restore(irq_flag);
}

static int read_flash_response(uint8_t expected_seq, uint8_t *recv_buf, uint32_t *recv_len)
{
    volatile ringbuffer_t *ap2np_rb = XRAM_AP2NP_RINGBUF;
    uint32_t idx;
    ringbuffer_entry_t *entry;
    uint8_t *src;
    flash_resp_t *resp;

    uint32_t poll_count = 0;
    while (poll_count < 1000) {
        if (ap2np_rb->tail - ap2np_rb->head > 0) {
            idx = ap2np_rb->head & RB_INDEX_MASK;
            entry = (ringbuffer_entry_t *)&ap2np_rb->entries[idx];
            *recv_len = entry->len;

            src = (uint8_t *)entry->data;
            for (uint32_t i = 0; i < *recv_len; i++) {
                recv_buf[i] = src[i];
            }

            resp = (flash_resp_t *)recv_buf;
            if (resp->seq == expected_seq) {
                ap2np_rb->head++;
                return 0;
            }

            LOG_I("Flash RPC NP: skipping old seq=%u, waiting for seq=%u\r\n", resp->seq, expected_seq);
            ap2np_rb->head++;
        }

        poll_count++;

        for (volatile int i = 0; i < 10; i++) {
            __asm__ volatile("nop");
        }
    }

    LOG_E("Flash RPC NP: ringbuffer empty after %u polls\r\n", poll_count);
    return -ENODATA;
}

/****************************************************************************
 * Types and Variables
 ****************************************************************************/
static SemaphoreHandle_t flash_rpc_mutex;                         /* Mutex for RPC call serialization */
static flash_resp_t g_last_response;                              /* Last received response */
static uint8_t g_read_data_buf[RB_ENTRY_SIZE - sizeof(uint32_t)]; /* Buffer for read data */
static uint32_t g_read_data_len = 0;                              /* Length of read data */
static uint8_t g_seq_counter = 0;                                 /* Sequence number counter */
static bool g_flash_rpc_initialized = false;                      /* Initialization flag */

/****************************************************************************
 * Generic RPC Call Function (with sync protection)
 *
 * CRITICAL: This function runs from RAM because when AP is erasing flash,
 * NP cannot execute code from Flash (XIP). All code called from this function
 * must also be in RAM or use XRAM for data.
 ****************************************************************************/

static int flash_rpc_call_sync(flash_cmd_header_t *cmd, const uint8_t *data, uint32_t data_len)
{
    int ret;
    uint8_t send_buf[512];
    uint8_t recv_buf[512];
    uint32_t recv_len;
    uint32_t send_len;
    uint8_t expected_seq;

    /* Get RPC mutex */
    if (flash_rpc_mutex == NULL) {
        LOG_E("Flash RPC: mutex is NULL!\r\n");
        return -EINVAL;
    }
    if (xSemaphoreTake(flash_rpc_mutex, pdMS_TO_TICKS(5000)) != pdTRUE) {
        LOG_E("Flash RPC: failed to get RPC mutex\r\n");
        return -EBUSY;
    }

    expected_seq = ++g_seq_counter;
    cmd->seq = expected_seq;

    send_len = sizeof(flash_cmd_header_t);
    memcpy(send_buf, cmd, sizeof(flash_cmd_header_t));
    if (data_len > 0) {
        memcpy(send_buf + sizeof(flash_cmd_header_t), data, data_len);
        send_len += data_len;
    }

    ret = simple_rpc_send(send_buf, send_len);
    if (ret != 0) {
        LOG_E("Flash RPC: simple_rpc_send failed, ret=%d\r\n", ret);
        xSemaphoreGive(flash_rpc_mutex);
        return ret;
    }

    uintptr_t irq_flag;
    ret = wait_for_ipc_response((volatile uint32_t *)(IPC0_BASE + IPC_AP2NP_STATUS_OFFSET), (1 << IPM_ID_FLASH_RESP),
                                (1 << IPM_ID_FLASH_RESP), &irq_flag);
    if (ret != 0) {
        LOG_E("Flash RPC: wait_for_ipc_response timeout, ret=%d\r\n", ret);
        xSemaphoreGive(flash_rpc_mutex);
        return ret;
    }

    IPC_CLEAR_INT((1 << IPM_ID_FLASH_RESP));

    ret = read_flash_response(expected_seq, recv_buf, &recv_len);

    wait_for_ipc_response_restore(irq_flag);

    if (ret != 0) {
        LOG_E("Flash RPC: read_flash_response failed, ret=%d\r\n", ret);
        xSemaphoreGive(flash_rpc_mutex);
        return ret;
    }

    {
        flash_resp_t *resp = (flash_resp_t *)recv_buf;

        memcpy(&g_last_response, resp, sizeof(flash_resp_t));

        if (recv_len > sizeof(flash_resp_t)) {
            g_read_data_len = recv_len - sizeof(flash_resp_t);
            if (g_read_data_len > RB_ENTRY_SIZE - sizeof(uint32_t)) {
                g_read_data_len = RB_ENTRY_SIZE - sizeof(uint32_t);
            }
            memcpy(g_read_data_buf, recv_buf + sizeof(flash_resp_t), g_read_data_len);
        }

        ret = (g_last_response.status == FLASH_STATUS_OK) ? 0 : g_last_response.error_code;
        xSemaphoreGive(flash_rpc_mutex);
        return ret;
    }
}

/****************************************************************************
 * Public API
 ****************************************************************************/

int flash_rpc_remote_init(void)
{
    int ret;

    if (g_flash_rpc_initialized) {
        return 0;
    }

    ret = simple_rpc_init();
    if (ret != 0) {
        LOG_E("Flash RPC Remote: simple_rpc_init failed, ret=%d\r\n", ret);
        return ret;
    }

    flash_rpc_mutex = xSemaphoreCreateMutex();
    if (flash_rpc_mutex == NULL) {
        LOG_E("Flash RPC Remote: failed to create flash_rpc_mutex\r\n");
        return -1;
    }

    g_flash_rpc_initialized = true;
    return 0;
}

int flash_rpc_erase(uint32_t addr, uint32_t len)
{
    flash_cmd_header_t cmd = {
        .cmd = FLASH_CMD_ERASE,
        .seq = 0,
        .addr = addr,
        .len = len,
        .offset = 0,
    };

    return flash_rpc_call_sync(&cmd, NULL, 0);
}

int flash_rpc_write(uint32_t addr, const uint8_t *data, uint32_t len)
{
    uint32_t offset = 0;
    int ret;
    while (offset < len) {
        uint32_t chunk_size = (len - offset) > FLASH_MAX_DATA_SIZE ? FLASH_MAX_DATA_SIZE : (len - offset);

        flash_cmd_header_t cmd = {
            .cmd = FLASH_CMD_WRITE,
            .seq = 0,
            .addr = addr + offset,
            .len = chunk_size,
            .offset = offset,
        };

        ret = flash_rpc_call_sync(&cmd, data + offset, chunk_size);
        if (ret != 0) {
            LOG_E("Flash RPC: write failed at offset=%lu, ret=%d\r\n", offset, ret);
            return ret;
        }

        offset += chunk_size;
    }

    return 0;
}

int flash_rpc_read(uint32_t addr, uint8_t *data, uint32_t len)
{
    uint32_t offset = 0;
    int ret;

    while (offset < len) {
        uint32_t chunk_size = (len - offset) > FLASH_MAX_DATA_SIZE ? FLASH_MAX_DATA_SIZE : (len - offset);

        flash_cmd_header_t cmd = {
            .cmd = FLASH_CMD_READ,
            .seq = 0,
            .addr = addr + offset,
            .len = chunk_size,
            .offset = offset,
        };

        ret = flash_rpc_call_sync(&cmd, NULL, 0);
        if (ret != 0) {
            LOG_E("Flash RPC: read failed at offset=%lu, ret=%d\r\n", offset, ret);
            return ret;
        }

        /* Copy data from response buffer to user buffer */
        if (g_read_data_len != chunk_size) {
            LOG_E("Flash RPC: read data length mismatch, expected=%lu, got=%lu\r\n", chunk_size, g_read_data_len);
            return -EIO;
        }
        memcpy(data + offset, g_read_data_buf, g_read_data_len);
        offset += chunk_size;
    }

    return 0;
}

int flash_rpc_reboot(void)
{
    flash_cmd_header_t cmd = {
        .cmd = FLASH_CMD_REBOOT,
        .seq = 0,
        .addr = 0,
        .len = 0,
        .offset = 0,
    };

    return flash_rpc_call_sync(&cmd, NULL, 0);
}

int flash_rpc_remote_deinit(void)
{
    if (flash_rpc_mutex) {
        vSemaphoreDelete(flash_rpc_mutex);
    }

    return 0;
}
