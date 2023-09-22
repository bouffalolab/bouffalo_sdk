/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdio.h>
#include <stdlib.h>
#include <smart_audio.h>
#include <msp_app_bt.h>
#include <bt.h>
// #include <ulog/ulog.h>

// #include "wifi/app_wifi.h"
// #include "event_mgr/app_event.h"

#if defined(CONFIG_BT_A2DP) && (CONFIG_BT_A2DP == 1)
static void bt_callback(msp_app_bt_event_t event, msp_app_bt_param_t *param)
{
    switch (event) {
        case MSP_APP_BT_A2DP_CONNECTED:
            printf("MSP_APP_BT_A2DP_CONNECTE\r\n");
            // app_event_update(EVENT_STATUS_BT_CONNECTED);
            break;
        case MSP_APP_BT_A2DP_DISCONNECTED:
            printf("MSP_APP_BT_A2DP_DISCONNECTED\r\n");
            // app_event_update(EVENT_STATUS_BT_DISCONNECTED);
            break;
        case MSP_APP_BT_A2DP_LINK_LOSS:
            printf("MSP_APP_BT_A2DP_LINK_LOSS\r\n");
            break;
        case MSP_APP_BT_A2DP_PLAY_STATUS_STOPPED:
            printf("MSP_APP_BT_A2DP_PLAY_STATUS_STOPPED\r\n");
            break;
        case MSP_APP_BT_A2DP_PLAY_STATUS_PLAYING:
            printf("MSP_APP_BT_A2DP_PLAY_STATUS_PLAYING\r\n");
            break;
        case MSP_APP_BT_A2DP_PLAY_STATUS_PAUSEED:
            printf("MSP_APP_BT_A2DP_PLAY_STATUS_PAUSEED\r\n");
            break;
        case MSP_APP_BT_AVRCP_STATUS_PAUSEED:
            printf("MSP_APP_BT_AVRCP_STATUS_PAUSEED\r\n");
            break;
        case MSP_APP_BT_AVRCP_STATUS_STOPPED:
            printf("MSP_APP_BT_AVRCP_STATUS_STOPPED\r\n");
            break;
        case MSP_APP_BT_AVRCP_STATUS_PLAYING:
            printf("MSP_APP_BT_AVRCP_STATUS_PLAYING\r\n");
            break;
        default:
            break;
    }
}
#endif

void app_bt_init(void)
{
    msp_app_bt_init();

    msp_app_bt_gap_set_scan_mode(0);

#if defined(CONFIG_BT_A2DP) && (CONFIG_BT_A2DP == 1)
    msp_app_bt_a2dp_register_cb(bt_callback);
    smtaudio_register_bt_a2dp(0, NULL, 0, 1.0f, 0);
#endif

#if defined(CONFIG_BT_HFP) && (CONFIG_BT_HFP == 1)
    smtaudio_register_bt_hfp(0, NULL, 0, 1.0f, 0);
#endif
}

void app_bt_adv_enable(int enable)
{
    if (enable) {
        bt_dev_addr_t addr;
        char          dev_name[64];

        memset(dev_name, 0, sizeof(dev_name));
        ble_stack_get_local_addr(&addr);
        sprintf(dev_name,
                "BouffaloMSP[%02X:%02X:%02X]",
                addr.val[0], addr.val[1], addr.val[2]);
        printf("bt device_name: %s\r\n", dev_name);
        msp_app_bt_set_device_name(dev_name);

        msp_app_bt_gap_set_scan_mode(1);
    } else {
        msp_app_bt_gap_set_scan_mode(0);
        msp_app_bt_a2dp_disconnect();
    }
}

int app_bt_set_device_name(char *name)
{
    int ret;
    ret = msp_app_bt_set_device_name(name);
    return ret;
}

