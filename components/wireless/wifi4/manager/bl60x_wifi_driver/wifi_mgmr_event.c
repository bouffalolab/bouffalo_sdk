/**
 ****************************************************************************************
 *
 * @file wifi_mgmr_event.c
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include <string.h>
//#include <aos/yloop.h>

#include "bl_main.h"
#include "wifi_mgmr.h"
#include "wifi_mgmr_api.h"
#include "wifi_mgmr_event.h"
#include <wifi_hosal.h>

void wifi_mgmr_diagnose_tlv_free(struct sm_tlv_list* list);
static void cb_connect_ind(void *env, struct wifi_event_sm_connect_ind *ind)
{
    struct sm_tlv_list last_unused = {NULL, NULL};

    wifiMgmr.wifi_mgmr_stat_info.status_code = ind->status_code;
    wifiMgmr.wifi_mgmr_stat_info.reason_code = ind->reason_code;
    bflb_os_mutex_lock(wifiMgmr.wifi_mgmr_stat_info.diagnose_lock);
    last_unused = wifiMgmr.wifi_mgmr_stat_info.connect_diagnose;
    wifiMgmr.wifi_mgmr_stat_info.connect_diagnose = ind->connect_diagnose;
    bflb_os_mutex_unlock(wifiMgmr.wifi_mgmr_stat_info.diagnose_lock);
    if (last_unused.first)
    {
        wifi_mgmr_diagnose_tlv_free(&last_unused);
    }
    wifi_mgmr_set_connect_stat_info(ind, WIFI_MGMR_CONNECT_IND_STAT_INFO_TYPE_IND_CONNECTION);
    wifi_mgmr_api_common_msg(
            (ind->status_code ? WIFI_MGMR_EVENT_FW_IND_DISCONNECT : WIFI_MGMR_EVENT_FW_IND_CONNECTED),
            (void*)0x1, (void*)0x2);
}

static void cb_disconnect_ind(void *env, struct wifi_event_sm_disconnect_ind *ind)
{
    struct sm_tlv_list last_unused = {NULL, NULL};

    bflb_os_printf("sending disconnect\r\n");
    wifiMgmr.wifi_mgmr_stat_info.type_ind = WIFI_MGMR_CONNECT_IND_STAT_INFO_TYPE_IND_DISCONNECTION;
    wifiMgmr.wifi_mgmr_stat_info.status_code = ind->status_code;
    wifiMgmr.wifi_mgmr_stat_info.reason_code = ind->reason_code;
    bflb_os_mutex_lock(wifiMgmr.wifi_mgmr_stat_info.diagnose_lock);
    last_unused = wifiMgmr.wifi_mgmr_stat_info.connect_diagnose;
    wifiMgmr.wifi_mgmr_stat_info.connect_diagnose = ind->connect_diagnose;
    bflb_os_mutex_unlock(wifiMgmr.wifi_mgmr_stat_info.diagnose_lock);
    if (last_unused.first)
    {
        wifi_mgmr_diagnose_tlv_free(&last_unused);
    }
    wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_FW_IND_DISCONNECT, (void*)0x1, (void*)0x2);

    wifi_hosal_pm_post_event(WLAN_PM_EVENT_CONTROL, WLAN_CODE_PM_NOTIFY_STOP, NULL);
}

static void cb_beacon_ind(void *env, struct wifi_event_beacon_ind *ind)
{
    wifi_mgmr_api_scan_item_beacon(ind->channel, ind->rssi, ind->auth, ind->bssid, ind->ssid, ind->ssid_len, ind->ppm_abs, ind->ppm_rel, ind->cipher, ind->wps, ind->mode, ind->group_cipher);
}

static void cb_probe_resp_ind(void *env, long long timestamp)
{
    bflb_os_printf("timestamp = 0x%llx\r\n", timestamp);
}

static void cb_rssi_ind(void *env, int8_t rssi)
{
    wifiMgmr.wlan_sta.sta.rssi = rssi;
}

static void cb_event_ind(void *env, struct wifi_event *event)
{
    switch (event->id) {
        case WIFI_EVENT_ID_IND_CHANNEL_SWITCH:
        {
            struct wifi_event_data_ind_channel_switch *ind;

            ind = (struct wifi_event_data_ind_channel_switch*)event->data;
            wifiMgmr.channel = ind->channel;
            //TODO it seems channel is strange got from fw. Fixit
            bflb_os_printf("[WIFI] [IND] Channel is %d\r\n", wifiMgmr.channel);
        }
        break;
        case WIFI_EVENT_ID_IND_SCAN_DONE:
        {
            struct wifi_event_data_ind_scan_done *ind;

            ind = (struct wifi_event_data_ind_scan_done*)event->data;
            (void) ind;
            bflb_os_puts("[WIFI] [IND] SCAN Done\r\n");
            wifi_mgmr_scan_complete_notify();
            platform_post_event(EV_WIFI, CODE_WIFI_ON_SCAN_DONE, WIFI_SCAN_DONE_EVENT_OK);
        }
        break;
        case WIFI_EVENT_ID_IND_SCAN_DONE_ONJOIN:
        {
            platform_post_event(EV_WIFI, CODE_WIFI_ON_SCAN_DONE_ONJOIN, 0);
        }
        break;
        default:
        {
            bflb_os_printf("----------------UNKNOWN WIFI EVENT %d-------------------\r\n", (int)event->id);
        }
    }
}

int wifi_mgmr_event_init(void)
{
    bl_rx_sm_connect_ind_cb_register(NULL, cb_connect_ind);
    bl_rx_sm_disconnect_ind_cb_register(NULL, cb_disconnect_ind);
    bl_rx_beacon_ind_cb_register(NULL, cb_beacon_ind);
    bl_rx_probe_resp_ind_cb_register(NULL, cb_probe_resp_ind);
    bl_rx_rssi_cb_register(NULL, cb_rssi_ind);
    bl_rx_event_register(NULL, cb_event_ind);
    return 0;
}
