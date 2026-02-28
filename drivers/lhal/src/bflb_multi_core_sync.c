#include "bflb_flash.h"
#include <stdio.h>
#ifdef CONFIG_IPC
#include "ipm.h"
#if defined(BL618DG)
#include "bl618dg_sys.h"
#endif

int bflb_flash_erase_mcs(uint32_t erase_addr, uint32_t len)
{
    int ret;

    if (ipc_sync_send_wait_ack(IPC_SYNC_SUSPEND_CMD, IPC_SYNC_SUSPEND_ACK, 3000) != 0) {
        printf("[FLASH ERASE] Timeout waiting for IPC_SYNC_SUSPEND_ACK\n");
        return -1;
    }

    ret = bflb_flash_erase(erase_addr, len);
    if (ret != 0) {
        ipc_sync_send_wait_ack(IPC_SYNC_RESUME_CMD, IPC_SYNC_RESUME_ACK, 1000);
        return ret;
    }

    if (ipc_sync_send_wait_ack(IPC_SYNC_RESUME_CMD, IPC_SYNC_RESUME_ACK, 3000) != 0) {
        printf("[FLASH ERASE] Timeout waiting for IPC_SYNC_RESUME_ACK\n");
        return -1;
    }

    return 0;
}

int bflb_flash_write_mcs(uint32_t write_addr, const uint8_t *data, uint32_t len)
{
    int ret;

    if (ipc_sync_send_wait_ack(IPC_SYNC_SUSPEND_CMD, IPC_SYNC_SUSPEND_ACK, 3000) != 0) {
        printf("[FLASH WRITE] Timeout waiting for IPC_SYNC_SUSPEND_ACK\n");
        return -1;
    }

    ret = bflb_flash_write(write_addr, data, len);
    if (ret != 0) {
        ipc_sync_send_wait_ack(IPC_SYNC_RESUME_CMD, IPC_SYNC_RESUME_ACK, 1000);
        return ret;
    }

    if (ipc_sync_send_wait_ack(IPC_SYNC_RESUME_CMD, IPC_SYNC_RESUME_ACK, 3000) != 0) {
        printf("[FLASH WRITE] Timeout waiting for IPC_SYNC_RESUME_ACK\n");
        return -1;
    }

    return 0;
}

int bflb_flash_read_mcs(uint32_t addr, uint8_t *data, uint32_t len)
{
    int ret;

    if (ipc_sync_send_wait_ack(IPC_SYNC_SUSPEND_CMD, IPC_SYNC_SUSPEND_ACK, 3000) != 0) {
        printf("[FLASH READ] Timeout waiting for IPC_SYNC_SUSPEND_ACK\n");
        return -1;
    }

    ret = bflb_flash_read(addr, data, len);
    if (ret != 0) {
        ipc_sync_send_wait_ack(IPC_SYNC_RESUME_CMD, IPC_SYNC_RESUME_ACK, 1000);
        return ret;
    }

    if (ipc_sync_send_wait_ack(IPC_SYNC_RESUME_CMD, IPC_SYNC_RESUME_ACK, 3000) != 0) {
        printf("[FLASH READ] Timeout waiting for IPC_SYNC_RESUME_ACK\n");
        return -1;
    }

    return 0;
}

void bflb_sys_reboot_mcs(void)
{

    if (ipc_sync_send_wait_ack(IPC_SYNC_SUSPEND_CMD, IPC_SYNC_SUSPEND_ACK, 3000) != 0) {
        printf("[SYSTEM REBOOT] Timeout waiting for IPC_SYNC_SUSPEND_ACK\n");
    }

    bl_sys_reset_por();

    if (ipc_sync_send_wait_ack(IPC_SYNC_RESUME_CMD, IPC_SYNC_RESUME_ACK, 3000) != 0) {
        printf("[SYSTEM REBOOT] Timeout waiting for IPC_SYNC_RESUME_ACK\n");
    }

}
#endif