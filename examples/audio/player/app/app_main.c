/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <FreeRTOS.h>
#include "task.h"

#include <app_wifi.h>
#include <app_bt.h>
#include <app_player.h>

#include "rfparam_adapter.h"
#include "bl616_glb.h"

void vAssertCalled(void)
{
    printf("vAssertCalled\r\n");
    __disable_irq();

    while (1);
}

static int btblecontroller_em_config(void)
{
    extern uint8_t __LD_CONFIG_EM_SEL;
    volatile uint32_t em_size;

    em_size = (uint32_t)&__LD_CONFIG_EM_SEL;

    if (em_size == 0) {
        GLB_Set_EM_Sel(GLB_WRAM160KB_EM0KB);
    } else if (em_size == 32*1024) {
        GLB_Set_EM_Sel(GLB_WRAM128KB_EM32KB);
    } else if (em_size == 64*1024) {
        GLB_Set_EM_Sel(GLB_WRAM96KB_EM64KB);
    } else {
        GLB_Set_EM_Sel(GLB_WRAM96KB_EM64KB);
    }

    return 0;
}

void app_main_entry(void *arg)
{
    /* Set ble controller EM Size */
    btblecontroller_em_config();

    /* Init rf */
    if (0 != rfparam_init(0, NULL, 0)) {
        printf("PHY RF init failed!\r\n");
        return 0;
    }

    /* For bt status save */
    bflb_mtd_init();
    easyflash_init();

    /* romsfs init mount use media factory*/
    romfs_mount(0x378000);

    // filesystem_init();

    /* Init wifi */
    app_wifi_init();

    /* Init player */
    app_player_init();

    /* Init bt */
    app_bt_init();

#if CONFIG_CODEC_USE_I2S_RX || CONFIG_CODEC_USE_I2S_TX
    extern msp_i2s_port_init(void);
    msp_i2s_port_init();
#endif
    extern int app_play_fifo_music();
    app_play_fifo_music();
    vTaskDelete(NULL);
}

