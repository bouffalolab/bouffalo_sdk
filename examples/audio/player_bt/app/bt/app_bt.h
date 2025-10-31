/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef __APP_BT_H__
#define __APP_BT_H__

void app_bt_init(void);
void app_bt_adv_enable(int enable);
int app_bt_set_device_name(char *name);

#endif

