/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <FreeRTOS.h>
#include "task.h"

#include <app_wifi.h>
#if defined(CONFIG_BLUETOOTH)
#include <app_bt.h>
#endif
#include <app_player.h>

#include "rfparam_adapter.h"
#include "bflb_mtd.h"
#include "easyflash.h"
#include "bflb_romfs.h"

#include <sys/time.h>
#include <reent.h>

extern int _gettimeofday_r(struct _reent *, struct timeval *, void *);
int gettimeofday(struct timeval *tv, void *tz)
{
    return _gettimeofday_r(_REENT, tv, tz);
}

void app_main_entry(void *arg)
{
    /* Set ble controller EM Size */
#if defined(CONFIG_BLUETOOTH)
    extern int bl_sys_em_config(void);
    bl_sys_em_config();
#endif

    /* Init rf */
    if (0 != rfparam_init(0, NULL, 0)) {
        printf("PHY RF init failed!\r\n");
        return;
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
#if CONFIG_BLUETOOTH
    app_bt_init();
#endif

#if defined(CONFIG_CODEC_USE_I2S)
    extern msp_i2s_port_init(void);
    msp_i2s_port_init();
#endif
    extern int app_play_fifo_music();
    app_play_fifo_music();
    vTaskDelete(NULL);
}
