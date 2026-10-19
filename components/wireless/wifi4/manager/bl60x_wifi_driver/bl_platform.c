
/**
 ****************************************************************************************
 *
 * @file bl_platform.c
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

#include "bl_platform.h"
#include "bl_irqs.h"
#include "bl_utils.h"
#include "reg_access.h"
#include "reg_ipc_app.h"

/**
 * bl_platform_on - Start the platform
 *
 * @bl_hw Main driver data
 *
 * It starts the platform :
 * - initialize IPC
 * - boot the fw
 * - enable link communication/IRQ
 *
 * Called by 802.11 part
 */
struct ipc_shared_env_tag *ipc_shenv;
int bl_platform_on(struct bl_hw *bl_hw)
{
    int ret;

    ipc_shenv = (struct ipc_shared_env_tag *)(&ipc_shared_env);
    ret = bl_ipc_init(bl_hw, ipc_shenv);
    if (ret) {
        return ret;
    }

    /*clear any pending IRQ*/
    ipc_emb2app_ack_clear(0xFFFFFFFF);
#if 0
    RWNX_REG_WRITE(BOOTROM_ENABLE, BL606_WIFI_PLATFORM_ADDRESS, SYSCTRL_MISC_CNTL_ADDR);
#endif

    return 0;
}

void bl_platform_off(struct bl_hw *bl_hw)
{
    ipc_host_disable_irq(bl_hw->ipc_env, IPC_IRQ_E2A_ALL);
#if 0
    tasklet_kill(&bl_hw->task);
    bl_ipc_deinit(bl_hw);
#endif
}
