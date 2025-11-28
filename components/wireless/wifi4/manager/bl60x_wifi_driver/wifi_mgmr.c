/**
 ****************************************************************************************
 *
 * @file wifi_mgmr.c
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <lwip/netifapi.h>
#include <lwip/dns.h>

#ifdef BL602_MATTER_SUPPORT
#include <lwip/dhcp6.h>
#endif

#include <utils_tlv_bl.h>
#include <bl60x_fw_api.h>
//#include <dns_server.h>
#include <dhcp_server.h>
#include "bl_main.h"
#include "wifi_mgmr.h"
#include "wifi_mgmr_profile.h"
#include "wifi_mgmr_api.h"
#include "wifi_mgmr_event.h"
#include "wifi_netif.h"
#include "include/wifi_mgmr_ext.h"

#include "wifi_hosal.h"

#include <bl_defs.h>

#include <bflb_os_private.h>
#define USER_UNUSED(a) ((void)(a))

#define DEBUG_HEADER "[WF][SM] "
#define mgmr_TASK_PRIORITY     (28)
#define TSEN_RELOAD_MS         (10000)

static wifi_conf_t conf = {
    .country_code = "CN",
};

wifi_mgmr_t wifiMgmr;
extern struct bl_hw wifi_hw;
static void dump_connect_param(const wifi_mgmr_profile_msg_t *profile_msg, int band, int freq, const uint8_t *bssid);
const static struct state
    stateGlobal,
    stateIdle,
    stateConnecting,
    stateConnectedIPNo, stateConnectedIPYes,
    stateDisconnect,
    stateIfaceDown,
    stateSniffer,
    stateError;

static void printErrMsg( void *stateData, struct event *event )
{
   bflb_os_printf( "ENTERED ERROR STATE!" );
}

static void _pending_task_set_safely(uint32_t bit)
{
    bflb_os_enter_critical();
    wifiMgmr.pending_task.val |= bit;
    bflb_os_exit_critical();
}

static void _pending_task_clr_safely(uint32_t bit)
{
    bflb_os_enter_critical();
    wifiMgmr.pending_task.val &= (~bit);
    bflb_os_exit_critical();
}

static int _pending_task_is_set(uint32_t bit)
{
    return (wifiMgmr.pending_task.val & bit) ? 1 : 0;
}

static int _features_is_set(uint32_t bit)
{
    return (wifiMgmr.features & bit) ? 1 : 0;
}

char *wifi_mgmr_mode_to_str(uint32_t mode)
{
    switch (mode)
    {
    case (WIFI_MODE_802_11B):
        return "B";
    case (WIFI_MODE_802_11B | WIFI_MODE_802_11G):
        return "BG";
    case (WIFI_MODE_802_11B | WIFI_MODE_802_11G | WIFI_MODE_802_11N_2_4):
        return "BGN";
    default:
        return "Unknown";
    }
}

char *wifi_mgmr_auth_to_str(uint8_t auth)
{
    switch (auth) {
        case WIFI_EVENT_BEACON_IND_AUTH_WPA3_SAE:
        {
            return "WPA3-SAE";
        }
        break;
        case WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK_WPA3_SAE:
        {
            return "WPA2-PSK/WPA3-SAE";
        }
        break;
        case WIFI_EVENT_BEACON_IND_AUTH_OPEN:
        {
            return "Open";
        }
        break;
        case WIFI_EVENT_BEACON_IND_AUTH_WEP:
        {
            return "WEP";
        }
        break;
        case WIFI_EVENT_BEACON_IND_AUTH_WPA_PSK:
        {
            return "WPA-PSK";
        }
        break;
        case WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK:
        {
            return "WPA2-PSK";
        }
        break;
        case WIFI_EVENT_BEACON_IND_AUTH_WPA_WPA2_PSK:
        {
            return "WPA2-PSK/WPA-PSK";
        }
        break;
        case WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT:
        {
            return "WPA/WPA2-Enterprise";
        }
        break;
        case WIFI_EVENT_BEACON_IND_AUTH_UNKNOWN:
        {
            return "Unknown";
        }
        break;
        default:
        {
            return "Unknown";
        }
    }
}

char *wifi_mgmr_cipher_to_str(uint8_t cipher)
{
    if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_NONE) {
        return "NONE";
    } else if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_WEP) {
        return "WEP";
    } else if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_AES) {
        return "AES";
    } else if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_TKIP) {
        return "TKIP";
    } else if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_TKIP_AES) {
        return "TKIP/AES";
    } else{
        return "Unknown";
    }
}

int wifi_mgmr_scan_beacon_save( wifi_mgmr_scan_item_t *scan )
{
#define SCAN_UPDATE_LIMIT_TIME_MS (3000)

    int i, j, pos_empty = -1, ret = 0, flag = 0;
    uint32_t counter = 0;
    int8_t lowest_rssi = 0, pos_rssi = -1;
    int8_t pos_same_ssid = -1;
    wifi_mgmr_scan_item_t tmp_item;

    bflb_os_mutex_lock(wifiMgmr.scan_items_lock);
    if (scan->channel > wifiMgmr.channel_nums || !scan->channel){
        ret = -1;
        goto __exit;
    }
    if (0 == scan->ssid[0] && (!_features_is_set(WIFI_MGMR_FEATURES_SCAN_SAVE_HIDDEN_SSID))) {
        ret = -1;
        goto __exit;
    }
    /*update scan_items, we just store the newly found item, or update exsiting one*/
    counter = bflb_os_get_time_ms();
    for (i = 0; i < sizeof(wifiMgmr.scan_items)/sizeof(wifiMgmr.scan_items[0]); i++) {
        if(wifiMgmr.scan_items[i].channel > wifiMgmr.channel_nums){
            memset(&wifiMgmr.scan_items[i], 0, sizeof(wifi_mgmr_scan_item_t));
        }

        if(0 == wifiMgmr.scan_items[i].is_used) {
            pos_empty = i;
            continue;
        }

        /* delete old item */
        if(counter - wifiMgmr.scan_items[i].timestamp_lastseen > SCAN_UPDATE_LIMIT_TIME_MS) {
            // printf("ssid:%s rssi:%d is old\r\n",wifiMgmr.scan_items[i].ssid,wifiMgmr.scan_items[i].rssi);
            memset(&wifiMgmr.scan_items[i], 0, sizeof(wifi_mgmr_scan_item_t));
            pos_empty = i;
            continue;
        }

        if(0 == flag) {
            lowest_rssi = wifiMgmr.scan_items[i].rssi;
            pos_rssi = i;
            flag = 1;
        }

        /* find the weakest rssi. If rssi is samed, update to the older position */
        if(lowest_rssi > wifiMgmr.scan_items[i].rssi) {
            lowest_rssi = wifiMgmr.scan_items[i].rssi;
            pos_rssi = i; 
        }else if(lowest_rssi == wifiMgmr.scan_items[i].rssi && wifiMgmr.scan_items[i].timestamp_lastseen < wifiMgmr.scan_items[pos_rssi].timestamp_lastseen) {
            pos_rssi = i;
        }

        if (0 == memcmp(wifiMgmr.scan_items[i].bssid, scan->bssid, sizeof(scan->bssid)) && 0 == strcmp(scan->ssid, wifiMgmr.scan_items[i].ssid)) {
            pos_same_ssid = i;
        }
    }

    if (i == sizeof(wifiMgmr.scan_items)/sizeof(wifiMgmr.scan_items[0])) {
        /* The oldest item is deleted if exist. Just need update the same ssid */
        if(-1 != pos_same_ssid) {
            if(scan->rssi >= wifiMgmr.scan_items[pos_same_ssid].rssi){
                wifiMgmr.scan_items[pos_same_ssid].channel = scan->channel;
                wifiMgmr.scan_items[pos_same_ssid].rssi = scan->rssi;
                wifiMgmr.scan_items[pos_same_ssid].ppm_abs = scan->ppm_abs;
                wifiMgmr.scan_items[pos_same_ssid].ppm_rel = scan->ppm_rel;
                wifiMgmr.scan_items[pos_same_ssid].timestamp_lastseen = counter;
                wifiMgmr.scan_items[pos_same_ssid].auth = scan->auth;
                wifiMgmr.scan_items[pos_same_ssid].cipher = scan->cipher;
                wifiMgmr.scan_items[pos_same_ssid].wps = scan->wps;
                wifiMgmr.scan_items[pos_same_ssid].mode = scan->mode;
            }
        }else {
            /* find empty item */
            if(-1 != pos_empty) {
                memset(&wifiMgmr.scan_items[pos_empty], 0, sizeof(wifiMgmr.scan_items[0]));
                strncpy(wifiMgmr.scan_items[pos_empty].ssid, scan->ssid, sizeof(wifiMgmr.scan_items[0].ssid));
                wifiMgmr.scan_items[pos_empty].ssid_tail[0] = '\0';
                wifiMgmr.scan_items[pos_empty].ssid_len = strlen(wifiMgmr.scan_items[pos_empty].ssid);
                memcpy(wifiMgmr.scan_items[pos_empty].bssid, scan->bssid, sizeof(wifiMgmr.scan_items[pos_empty].bssid));
                wifiMgmr.scan_items[pos_empty].channel = scan->channel;
                wifiMgmr.scan_items[pos_empty].rssi = scan->rssi;
                wifiMgmr.scan_items[pos_empty].timestamp_lastseen = counter;
                wifiMgmr.scan_items[pos_empty].auth = scan->auth;
                wifiMgmr.scan_items[pos_empty].cipher = scan->cipher;
                wifiMgmr.scan_items[pos_empty].wps = scan->wps;
                wifiMgmr.scan_items[pos_empty].mode = scan->mode;
                wifiMgmr.scan_items[pos_empty].is_used = 1;
            }else {
                /* if no empty item, copy to low rssi item */
                if(scan->rssi > lowest_rssi) {
                    memset(&wifiMgmr.scan_items[pos_rssi], 0, sizeof(wifiMgmr.scan_items[0]));
                    strncpy(wifiMgmr.scan_items[pos_rssi].ssid, scan->ssid, sizeof(wifiMgmr.scan_items[0].ssid));
                    wifiMgmr.scan_items[pos_rssi].ssid_tail[0] = '\0';
                    wifiMgmr.scan_items[pos_rssi].ssid_len = strlen(wifiMgmr.scan_items[pos_rssi].ssid);
                    memcpy(wifiMgmr.scan_items[pos_rssi].bssid, scan->bssid, sizeof(wifiMgmr.scan_items[pos_rssi].bssid));
                    wifiMgmr.scan_items[pos_rssi].channel = scan->channel;
                    wifiMgmr.scan_items[pos_rssi].rssi = scan->rssi;
                    wifiMgmr.scan_items[pos_rssi].timestamp_lastseen = counter;
                    wifiMgmr.scan_items[pos_rssi].auth = scan->auth;
                    wifiMgmr.scan_items[pos_rssi].cipher = scan->cipher;
                    wifiMgmr.scan_items[pos_rssi].wps = scan->wps;
                    wifiMgmr.scan_items[pos_rssi].mode = scan->mode;
                    wifiMgmr.scan_items[pos_rssi].is_used = 1;
                }
            }
        }
    }

    /* sort scan results according to rssi. The empty item placed in the end of the list*/
    for(i = 0; i < WIFI_MGMR_SCAN_ITEMS_MAX - 1; ++i) {
        for(j = 0; j < WIFI_MGMR_SCAN_ITEMS_MAX-i-1; ++j) {
            if((wifiMgmr.scan_items[j].rssi < wifiMgmr.scan_items[j+1].rssi && wifiMgmr.scan_items[j+1].is_used)
                    || ((!wifiMgmr.scan_items[j].is_used) && (wifiMgmr.scan_items[j+1].is_used))) {
                memcpy(&tmp_item, &wifiMgmr.scan_items[j], sizeof(wifi_mgmr_scan_item_t));
                memcpy(&wifiMgmr.scan_items[j], &wifiMgmr.scan_items[j+1], sizeof(wifi_mgmr_scan_item_t));
                memcpy(&wifiMgmr.scan_items[j+1], &tmp_item, sizeof(wifi_mgmr_scan_item_t));
            }
        }
    }
__exit:
    bflb_os_mutex_unlock(wifiMgmr.scan_items_lock);

    return ret;
}

static bool stateGlobalGuard_disable_autoreconnect( void *ch, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ch != (void*)msg->ev) {
        return false;
    }

    if (&stateDisconnect == wifiMgmr.m.currentState) {
        bflb_os_printf("Disable Autoreconnect in Disconnec State\r\n");
        bflb_os_printf(DEBUG_HEADER "Removing STA interface...\r\n");
        bl_main_if_remove(BL_VIF_STA);
        return true;
    }
    /*we need set disable now for future use*/
    bflb_os_printf("Disable Auto Reconnect\r\n");
    wifi_mgmr_profile_autoreconnect_disable(&wifiMgmr, -1);

    /*we always return false, since we only store the info from beacon frame*/
    return false;
}

static bool stateGlobalGuard_enable_autoreconnect( void *ch, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ch != (void*)msg->ev) {
        return false;
    }

    /*we need set enable now for future use*/
    bflb_os_printf("Enable Auto Reconnect\r\n");
    wifi_mgmr_profile_autoreconnect_enable(&wifiMgmr, -1);

    /*we always return false, since we only store the info from beacon frame*/
    return false;
}

static bool stateGlobalGuard_fw_disconnect(void *ch, struct event *event)
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (WIFI_MGMR_EVENT_FW_DISCONNECT == msg->ev) {
        bflb_os_printf("Disconnect CMD\r\n");
        bl_main_disconnect();
    }

    return false;
}

static bool stateGlobalGuard_fw_powersaving(void *ch, struct event *event)
{
    wifi_mgmr_msg_t *msg;
    int mode, ret = -1;

    msg = event->data;
    if (WIFI_MGMR_EVENT_FW_POWERSAVING == msg->ev) {
        bflb_os_printf("------>>>>>> Powersaving CMD, mode: %u\r\n", (unsigned int)msg->data1);
        mode = (int)msg->data1;
        if (mode >= PS_MODE_OFF && mode <= PS_MODE_ON_DYN) {
            if ((mode == PS_MODE_OFF) || ((mode > PS_MODE_OFF) && bl_main_sta_is_connected()))
            if (!bl_main_powersaving(mode)) {
                ret = 0;
            }
        }
    }

    platform_post_event(EV_WIFI, CODE_WIFI_ON_SET_PS_DONE, (ret==0)?WIFI_PS_SET_DONE_EVENT_OK:
                                                               WIFI_PS_SET_DONE_EVENT_FAIL);
    return false;
}

static bool stateGlobalGuard_fw_scan(void *ch, struct event *event)
{
    wifi_mgmr_msg_t *msg;
    uint16_t channel_num = 0;
    wifi_mgmr_scan_params_t *ch_req;
    struct mac_ssid *ssid = NULL;
    struct mac_addr bssid;
    uint8_t scan_mode;
    uint32_t duration_scan;

    msg = event->data;

    /*only check wifi scan command*/
    if (WIFI_MGMR_EVENT_FW_SCAN != msg->ev) {
        return false;
    }

#ifndef CFG_NETBUS_WIFI_ENABLE
    /*pending wifi scan command*/
    if (&stateConnecting == wifiMgmr.m.currentState ||
            &stateConnectedIPNo == wifiMgmr.m.currentState ||
            &stateDisconnect == wifiMgmr.m.currentState) {
            bflb_os_printf("------>>>>>> Scan CMD Pending\r\n");
            //FIXME TODO we should keep scan channel list for PENDING scan
            _pending_task_set_safely(WIFI_MGMR_PENDING_TASK_SCAN_BIT);
            return false;
    }
#endif

    ch_req = (wifi_mgmr_scan_params_t *)msg->data;
    channel_num = ch_req->channel_num;
    ssid = &(ch_req->ssid);
    memcpy((uint8_t *)&bssid, ch_req->bssid, ETH_ALEN);
    scan_mode = ch_req->scan_mode;
    duration_scan = ch_req->duration_scan;
#if 0
    if (channel_num) {
        bflb_os_printf("%s len:%d \r\n",__func__, channel_num);
        for(uint8_t i = 0; i < channel_num ; i ++) {
            bflb_os_printf("%d  ", ch_req->channels[i]);
        }
        bflb_os_printf("\r\n");
    }
#endif

#ifndef CFG_NETBUS_WIFI_ENABLE
    /*Forbidden other cases*/
    if (&stateIdle != wifiMgmr.m.currentState &&
            &stateConnectedIPYes != wifiMgmr.m.currentState &&
            &stateSniffer != wifiMgmr.m.currentState) {
            bflb_os_printf("------>>>>>> FW busy\r\n");
            platform_post_event(EV_WIFI, CODE_WIFI_ON_SCAN_DONE, WIFI_SCAN_DONE_EVENT_BUSY);
            return false;
    }
#endif

    if (channel_num) {
        bflb_os_printf("------>>>>>> Scan CMD fixed channels_num:%u\r\n", channel_num);
        bl_main_scan(&wifiMgmr.wlan_sta.netif, ch_req->channels, channel_num, &bssid, ssid, scan_mode, duration_scan);
    } else {
        /*normal scan command*/
        bflb_os_printf("------>>>>>> Scan CMD\r\n");
        bl_main_scan(&wifiMgmr.wlan_sta.netif, NULL, 0, &bssid, ssid, scan_mode, duration_scan);
    }

    return false;
}

static void stateGlobalAction( void *oldStateData, struct event *event,
      void *newStateData )
{
    bflb_os_printf(DEBUG_HEADER "Global Action\r\n");
}

static bool stateGuard( void *ch, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ch == (void*)msg->ev) {
        return true;
    }
    return false;
}

static void stateAction( void *oldStateData, struct event *event,
      void *newStateData )
{
    bflb_os_printf(DEBUG_HEADER "State Action ###%s### --->>> ###%s###\r\n",
            (char*)oldStateData,
            (char*)newStateData
    );
}


static bool stateSnifferGuard_idle(void *ev, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ev != (void*)msg->ev) {
        return false;
    }

    bl_main_monitor_disable();
    return true;
}

/*function for state sniffer*/
static bool stateSnifferGuard_ChannelSet( void *ch, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ch != (void*)msg->ev) {
        return false;
    }

    bl_main_monitor_channel_set((int)msg->data1, (int)msg->data2);
    /*will never trigger state change, since we just want to trigger the guard*/
    return false;
}

#ifdef BL602_MATTER_SUPPORT
static struct dhcp6 bl_dhcp6;
#endif
static bool stateSnifferGuard_raw_send(void *ch, struct event *event)
{
    wifi_mgmr_msg_t *msg;
    uint8_t *pkt;
    int len;

    if ((&stateIdle) == wifiMgmr.m.currentState || (&stateIfaceDown) == wifiMgmr.m.currentState) {
        /* NO Raw Send in IDLE mode */
        return false;
    }

    msg = event->data;
    if (WIFI_MGMR_EVENT_FW_DATA_RAW_SEND == msg->ev) {
        pkt = msg->data1;
        len = (int)msg->data2;
        bflb_os_log_info("------>>>>>> RAW Send CMD, pkt %p, len %d\r\n", pkt, len);
        bl_main_raw_send(pkt, len);
    }
    #ifdef BL602_MATTER_SUPPORT
    dhcp6_set_struct(&(wifiMgmr.wlan_sta.netif), &bl_dhcp6);
    netifapi_netif_common(&(wifiMgmr.wlan_sta.netif), dhcp6_enable_stateless, NULL);
    #endif

    return false;
}

static bool stateGlobal_cfg_req(void *ch, struct event *event)
{
    wifi_mgmr_msg_t *msg;
    wifi_mgmr_cfg_element_msg_t *cfg_req;

    msg = event->data;
    if (WIFI_MGMR_EVENT_FW_CFG_REQ == msg->ev) {
        cfg_req = (wifi_mgmr_cfg_element_msg_t*)msg->data;
        bl_main_cfg_task_req(cfg_req->ops, cfg_req->task, cfg_req->element, cfg_req->type, cfg_req->buf, NULL);
    }

    return false;
}

static void stateConnectingEnter( void *stateData, struct event *event )
{
   bflb_os_printf(DEBUG_HEADER "Entering %s state\r\n", (char *)stateData);
   platform_post_event(EV_WIFI, CODE_WIFI_ON_CONNECTING, 0);
}

static bool stateGlobalGuard_AP(void *ev, struct event *event )
{
    wifi_mgmr_msg_t *msg;
    wifi_mgmr_ap_msg_t *ap;

    if (wifiMgmr.inf_ap_enabled) {
        bflb_os_printf(DEBUG_HEADER "%s: AP iface has started!\r\n", __func__);
        return false;
    }

    msg = event->data;
    if (WIFI_MGMR_EVENT_APP_AP_START != msg->ev) {
        return false;
    }

    if (bl_main_if_add(0, &(wifiMgmr.wlan_ap.netif), &(wifiMgmr.wlan_ap.vif_index))) {
        bflb_os_printf(DEBUG_HEADER "%s: add AP iface failed\r\n", __func__);
        return false;
    }
    ap = (wifi_mgmr_ap_msg_t*)msg->data;

    if (ap->use_dhcp_server) {
        dhcpd_start(&(wifiMgmr.wlan_ap.netif), -1, -1);
    }

    if (ap->max_sta_supported >= 0) {
        bflb_os_printf(DEBUG_HEADER "Conf max sta supported %d;\r\n", ap->max_sta_supported);
        bl_main_conf_max_sta(ap->max_sta_supported);
    }

    bflb_os_printf(DEBUG_HEADER "start AP with ssid %s;\r\n", ap->ssid);
    bflb_os_printf(DEBUG_HEADER "              pwd  %s;\r\n", ap->psk);
    bflb_os_printf(DEBUG_HEADER "              channel  %ld;\r\n", ap->channel);
    bl_main_apm_start(ap->ssid, ap->psk, ap->channel, ap->hidden_ssid, wifiMgmr.ap_bcn_int);
    wifiMgmr.inf_ap_enabled = 1;
    // DNS Server should only need to be started once during lwip initialization
    //if (ap->use_dhcp_server) {
    //    wifiMgmr.dns_server = dns_server_init();
    //}
    platform_post_event(EV_WIFI, CODE_WIFI_ON_AP_STARTED, 0);

    return false;
}

static bool stateGlobalGuard_stop(void *ev, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (WIFI_MGMR_EVENT_APP_AP_STOP != msg->ev) {
        return false;
    }

    //dns_server_deinit(wifiMgmr.dns_server);
    //wifiMgmr.dns_server = NULL;

    bflb_os_printf(DEBUG_HEADER "Removing and deauth all sta client...\r\n");
    bl_main_apm_remove_all_sta();
    bflb_os_printf(DEBUG_HEADER "Stoping AP interface...\r\n");
    bl_main_apm_stop();
    bflb_os_printf(DEBUG_HEADER "Removing AP interface...\r\n");
    bl_main_if_remove(BL_VIF_AP);
    bflb_os_printf(DEBUG_HEADER "Stopping DHCP on AP interface...\r\n");

    // netifapi_netif_set_addr(&(wifiMgmr.wlan_ap.netif), NULL, NULL, NULL);
err_t dhcp_server_stop(struct netif *netif);
    netifapi_netif_common(&(wifiMgmr.wlan_ap.netif), NULL, dhcp_server_stop);
    // bflb_os_printf(DEBUG_HEADER "Removing ETH interface ...\r\n");
    // netifapi_netif_remove(&(wifiMgmr.wlan_ap.netif));
    wifiMgmr.inf_ap_enabled = 0;
    platform_post_event(EV_WIFI, CODE_WIFI_ON_AP_STOPPED, 0);

    return false;
}

static bool stateGlobalGuard_ap_chan_switch(void *ev, struct event *event)
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ev != (void *)msg->ev) {
        return false;
    }

    if (!wifiMgmr.inf_ap_enabled) {
        return false;
    }

    bl_main_apm_chan_switch((int)(intptr_t)msg->data1, (uint8_t)(uintptr_t)msg->data2);

    return false;
}

static bool stateGlobalGuard_conf_max_sta(void *ev, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (WIFI_MGMR_EVENT_APP_CONF_MAX_STA != msg->ev) {
        return false;
    }

    bflb_os_printf(DEBUG_HEADER "Conf max sta supported %lu...\r\n", (uint32_t)msg->data1);
    bl_main_conf_max_sta((uint32_t)msg->data1);

    return false;
}

//FIXME TODO ugly hack
static int auto_repeat = 0;
static void trigger_auto_denoise(void* arg)
{
    //Continuously check the microwave and try to denoise
    if (auto_repeat) {
int wifi_mgmr_api_denoise_enable(void);
        wifi_mgmr_api_denoise_enable();
        platform_post_delayed_action(85, trigger_auto_denoise, NULL);
    }
}

static bool stateGlobalGuard_denoise(void *ev, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (WIFI_MGMR_EVENT_APP_DENOISE != msg->ev) {
        return false;
    }

    if (msg->data1) {
        /*TODO no more magic here*/
        //enable denoise
        if (&stateConnectedIPYes == wifiMgmr.m.currentState) {
            /*only enable denoise with IP OK*/
            if (auto_repeat) {
                /*Continuously detect*/
                bl_main_denoise(3);
            } else {
                /*Initial detect*/
                auto_repeat = 1;
                bl_main_denoise(1);
                platform_post_event(EV_WIFI, CODE_WIFI_ON_MGMR_DENOISE, 0);
            }
        }
    } else {
        //disable denoise
        auto_repeat = 0;
        bl_main_denoise(0);
    }

    return false;
}

static bool stateGlobalGuard_connect(void *ev, struct event *event )
{
    wifi_mgmr_msg_t *msg;
    wifi_mgmr_profile_msg_t *profile_msg;
    uint8_t status;

    msg = event->data;
    if (ev != (void*)msg->ev) {
        return false;
    }

    bflb_os_printf(DEBUG_HEADER "currentState is %s\r\n", (char*)wifiMgmr.m.currentState->data);

    /* pending wifi connect command */
    if (&stateConnecting == wifiMgmr.m.currentState ||
            &stateConnectedIPNo == wifiMgmr.m.currentState ||
            &stateConnectedIPYes == wifiMgmr.m.currentState) {
        bflb_os_printf(DEBUG_HEADER "Connect CMD Pending\r\n");

        if (&stateConnectedIPNo == wifiMgmr.m.currentState ||
            &stateConnectedIPYes == wifiMgmr.m.currentState ) {
            wifi_mgmr_sta_disconnect();
        } else {
            bl_main_connect_abort(&status);
            bflb_os_printf(DEBUG_HEADER "connect abort status : %u\r\n", status);
        }
        bflb_os_msleep(WIFI_MGMR_STA_DISCONNECT_DELAY);

        profile_msg = (wifi_mgmr_profile_msg_t*)msg->data;
        profile_msg->ssid_tail[0] = '\0';
        profile_msg->psk_tail[0] = '\0';
        /* if pending, use profiles[1] */
        wifi_mgmr_profile_add_by_idx(&wifiMgmr, profile_msg, 1, 0);
        _pending_task_set_safely(WIFI_MGMR_PENDING_TASK_CONNECT_BIT);
        return false;
    } else if (&stateDisconnect == wifiMgmr.m.currentState) {
        /* continue connecting*/
        return true;
    } else if (&stateIdle != wifiMgmr.m.currentState) {
        return false;
    }

    if (bl_main_if_add(1, &wifiMgmr.wlan_sta.netif, &wifiMgmr.wlan_sta.vif_index)) {
        bflb_os_printf(DEBUG_HEADER "%s: add STA iface failed\r\n", __func__);
        return false;
    }
    return true;
}

static void stateGlobalAction_connect( void *oldStateData, struct event *event,
      void *newStateData )
{
    wifi_mgmr_msg_t *msg;
    wifi_mgmr_profile_msg_t *profile_msg;

    msg = event->data;
    profile_msg = (wifi_mgmr_profile_msg_t*)msg->data;
    profile_msg->ssid_tail[0] = '\0';
    profile_msg->psk_tail[0] = '\0';
    dump_connect_param(profile_msg, profile_msg->band, profile_msg->freq, profile_msg->bssid);
    /* if not pending, use profiles[0] */
    wifi_mgmr_profile_add_by_idx(&wifiMgmr, profile_msg, 0, 1);
    /* set profiles[1] inactive */
    wifi_mgmr_profile_set_active_by_idx(&wifiMgmr, 1, 0);

    bflb_os_printf(DEBUG_HEADER "State Action ###%s### --->>> ###%s###\r\n",
            (char*)oldStateData,
            (char*)newStateData
    );

    wifiMgmr.ap_info_ttl_curr = -1;

    //TODO Other security support
    bl_main_connect((const uint8_t *)profile_msg->ssid, profile_msg->ssid_len,
            (const uint8_t *)profile_msg->passphr, profile_msg->passphr_len,
            (const uint8_t *)profile_msg->psk, profile_msg->psk_len,
            (const uint8_t *)profile_msg->bssid,
            (const uint8_t)profile_msg->band,
            (const uint16_t)profile_msg->freq,
            (const uint32_t)profile_msg->flags
    );
}

static void stateExit( void *stateData, struct event *event )
{
    if (_pending_task_is_set(WIFI_MGMR_PENDING_TASK_SCAN_BIT)) {
        bflb_os_printf(DEBUG_HEADER "Pending Scan Sent\r\n");
        bl_main_scan(&wifiMgmr.wlan_sta.netif, NULL, 0, (struct mac_addr *)&mac_addr_bcst, NULL, 0, 0);
        _pending_task_clr_safely(WIFI_MGMR_PENDING_TASK_SCAN_BIT);
    }

   bflb_os_printf(DEBUG_HEADER "Exiting %s state\r\n", (char *)stateData);
}

static void stateEnter( void *stateData, struct event *event )
{
   bflb_os_printf(DEBUG_HEADER "Entering %s state\r\n", (char *)stateData);
}

const static struct state stateGlobal = {
   .parentState = NULL,
   .entryState = NULL,
   .transitions = (struct transition[])
   {
      {EVENT_TYPE_GLB, (void*)WIFI_MGMR_EVENT_GLB_DISABLE_AUTORECONNECT, &stateGlobalGuard_disable_autoreconnect, &stateGlobalAction, &stateIdle},
      {EVENT_TYPE_GLB, (void*)WIFI_MGMR_EVENT_GLB_ENABLE_AUTORECONNECT, &stateGlobalGuard_enable_autoreconnect, &stateGlobalAction, &stateIdle},
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_AP_START, &stateGlobalGuard_AP, &stateGlobalAction, &stateIdle},
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_AP_STOP, &stateGlobalGuard_stop, &stateGlobalAction, &stateIdle},
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_AP_CHAN_SWITCH, &stateGlobalGuard_ap_chan_switch, &stateGlobalAction, &stateIdle},
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_CONF_MAX_STA, &stateGlobalGuard_conf_max_sta, &stateGlobalAction, &stateIdle},
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_DENOISE, &stateGlobalGuard_denoise, &stateGlobalAction, &stateIdle},
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_CONNECT, &stateGlobalGuard_connect, &stateGlobalAction_connect, &stateConnecting},
      {EVENT_TYPE_FW, (void*)WIFI_MGMR_EVENT_FW_DISCONNECT, &stateGlobalGuard_fw_disconnect, &stateGlobalAction, &stateIdle},
      {EVENT_TYPE_FW, (void*)WIFI_MGMR_EVENT_FW_POWERSAVING, &stateGlobalGuard_fw_powersaving, &stateGlobalAction, &stateIdle},
      {EVENT_TYPE_FW, (void*)WIFI_MGMR_EVENT_FW_SCAN, &stateGlobalGuard_fw_scan, &stateGlobalAction, &stateIdle},
      {EVENT_TYPE_FW,  (void*)WIFI_MGMR_EVENT_FW_DATA_RAW_SEND, &stateSnifferGuard_raw_send, &stateGlobalAction, &stateIdle},
      {EVENT_TYPE_FW,  (void*)WIFI_MGMR_EVENT_FW_CFG_REQ, &stateGlobal_cfg_req, &stateGlobalAction, &stateIdle},
   },
   .numTransitions = 13,
   .data = "group",
   .entryAction = &stateEnter,
   .exitAction = &stateExit,
};

const static struct state stateSniffer = {
   .parentState = &stateGlobal,
   .entryState = NULL,
   .transitions = (struct transition[])
   {
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_IDLE, &stateSnifferGuard_idle, &stateAction, &stateIdle},
      /*Will NOT transfer state*/
      {EVENT_TYPE_FW,  (void*)WIFI_MGMR_EVENT_FW_CHANNEL_SET, &stateSnifferGuard_ChannelSet, &stateAction, &stateIdle},
   },
   .numTransitions = 2,
   .data = "sniffer",
   .entryAction = &stateEnter,
   .exitAction = &stateExit,
};

static bool stateConnectingGuard_disconnect(void *ev, struct event *event )
{
    wifi_mgmr_msg_t *msg;
    uint8_t status;

    msg = event->data;
    if (ev != (void*)msg->ev) {
        return false;
    }

    bl_main_connect_abort(&status);
    bflb_os_msleep(WIFI_MGMR_STA_DISCONNECT_DELAY);
    return false;
}

const static struct state stateConnecting = {
   .parentState = &stateGlobal,
   .entryState = NULL,
   .transitions = (struct transition[])
   {
      {EVENT_TYPE_FW, (void*)WIFI_MGMR_EVENT_FW_IND_CONNECTED, &stateGuard, &stateAction, &stateConnectedIPNo},
      {EVENT_TYPE_FW, (void*)WIFI_MGMR_EVENT_FW_IND_DISCONNECT, &stateGuard, &stateAction, &stateDisconnect},
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_DISCONNECT, &stateConnectingGuard_disconnect, &stateAction, &stateDisconnect},
   },
   .numTransitions = 3,
   .data = "connecting",
   .entryAction = &stateConnectingEnter,
   .exitAction = &stateExit,
};

/********************section for ilde *************************/
static bool stateIdleGuard_sniffer(void *ev, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ev != (void*)msg->ev) {
        return false;
    }

    bl_main_monitor();
    return true;
}

static void dump_connect_param(const wifi_mgmr_profile_msg_t *profile_msg, int band, int freq, const uint8_t *bssid)
{
    bflb_os_printf(DEBUG_HEADER "Action Connect\r\n");
    bflb_os_printf("\tssid %s\r\n", profile_msg->ssid);
    bflb_os_printf("\tssid len %u\r\n", (unsigned int)profile_msg->ssid_len);
    bflb_os_printf("\tpassphr %s\r\n", profile_msg->passphr);
    bflb_os_printf("\tpassphr len %u\r\n", (unsigned int)profile_msg->passphr_len);
    bflb_os_printf("\tpsk %s\r\n", profile_msg->psk);
    bflb_os_printf("\tpsk len %u\r\n", (unsigned int)profile_msg->psk_len);
    bflb_os_printf("\tband %d\r\n", band);
    bflb_os_printf("\tfreq %d\r\n", freq);
    bflb_os_printf("\tbssid %02X:%02X:%02X:%02X:%02X:%02X\r\n", MAC_ADDR_LIST(bssid));
    bflb_os_printf("\tdhcp status: %s\r\n", profile_msg->dhcp_use ? "true" : "false");
    bflb_os_printf("\tflags: %u\r\n", (unsigned)profile_msg->flags);
}

const static struct state stateIdle = {
   .parentState = &stateGlobal,
   .entryState = NULL,
   .transitions = (struct transition[])
   {
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_SNIFFER, &stateIdleGuard_sniffer, &stateAction, &stateSniffer},
   },
   .numTransitions = 1,
   .data = "idle",
   .entryAction = &stateEnter,
   .exitAction = &stateExit,
};
/*==================================================================================================*/


/********************section for ifacedown *************************/
static bool stateIfaceDownGuard_phyup(void *ev, struct event *event )
{
    int error;
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ev != (void*)msg->ev) {
        bflb_os_printf(DEBUG_HEADER "state mismatch\r\n");
        return false;
    }

    //TODO no such usage for function call
    error = bl_main_phy_up();
    if (error) {
        return false;
    }
    return true;
}

const static struct state stateIfaceDown = {
   .parentState = &stateGlobal,
   .entryState = NULL,
   .transitions = (struct transition[])
   {
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_PHY_UP, &stateIfaceDownGuard_phyup, &stateAction, &stateIdle},
   },
   .numTransitions = 1,
   .data = "ifaceDown",
   .entryAction = &stateEnter,
   .exitAction = &stateExit,
};
/*==================================================================================================*/

/********************section for connected but still no IP address*************************/
typedef struct connectedIPNoData {
    char name[32];//all the state data must start with name field
    BL_Timer_t timer;//used for timeout detect on obtain IP address
} connectedIPNoData_t;
static connectedIPNoData_t stateConnectedIPNo_data = {
    .name = "wifiConnected_ipObtaining",
};

typedef struct tsen_reload_data {
    char name[32];//all the state data must start with name field
    BL_Timer_t timer;//used for timeout detect on obtain IP address
} tsen_reload_data_t;
static tsen_reload_data_t state_tsen_reload_data = {
    .name = "wifi tsen reload",
};


static bool stateConnectedIPNoGuard_disconnect(void *ch, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ch != (void*)msg->ev) {
        return false;
    }

    bl_main_disconnect();
    /*will never trigger state change, since we just want to trigger the guard*/
    return false;
}

static void stateConnectedIPNoAction_ipgot(void *oldStateData, struct event *event,
      void *newStateData )
{
    bflb_os_printf(DEBUG_HEADER "State Action ###%s### --->>> ###%s###\r\n",
            (char*)oldStateData,
            (char*)newStateData
    );
}

static void ip_obtaining_timeout(void *data)
{
    connectedIPNoData_t *stateData = (connectedIPNoData_t *)data;

    (void)stateData;

    bflb_os_printf(DEBUG_HEADER "IP obtaining timeout\r\n");
    wifi_mgmr_api_fw_disconnect();
}

static void __reload_tsen(void *data)
{
    uint32_t *stateData = (uint32_t *)data;

    (void)stateData;

    wifi_mgmr_api_fw_tsen_reload();
}

typedef volatile struct{
    uint16_t Tchannels[5];
    int16_t Tchannel_os[5];
    int16_t Tchannel_os_low[5];
    int16_t Troom_os;
    uint8_t en_tcal;
    uint8_t linear_or_follow;
} tcal_param_struct;
extern tcal_param_struct* tcal_param;
static void __run_reload_tsen(void)
{
#ifdef CFG_CHIP_BL602
    static int call_tcal_once = 0;
    int16_t temp = 0;
    extern void phy_tcal_callback(int16_t temperature);

    if (&stateConnecting == wifiMgmr.m.currentState || &stateDisconnect == wifiMgmr.m.currentState || &stateConnectedIPYes == wifiMgmr.m.currentState ||
            &stateSniffer == wifiMgmr.m.currentState || &stateConnectedIPNo == wifiMgmr.m.currentState) {

        if (tcal_param->en_tcal) {

#ifdef CONF_ADC_ENABLE_TSEN
            hosal_adc_dev_t *adc;
            adc = wifi_hosal_adc_device_get();
            if (NULL == adc) {
                bflb_os_log_warn("please init tsen\r\n");
                return;
            }
            temp = wifi_hosal_adc_tsen_value_get(adc);
#endif
            bflb_os_log_debug("temp is %u\r\n", temp);
            phy_tcal_callback(temp);
        } else {
            if (0 == call_tcal_once) {
                phy_tcal_callback(35);
                call_tcal_once = 1;
            }
        }
        return ;
    }
#endif
    return;

}


static void __sta_setup_ip(bool use_dhcp)
{
    uint32_t ip, mask, gw, dns1, dns2;
    ip4_addr_t addr_ipaddr;
    ip4_addr_t addr_netmask;
    ip4_addr_t addr_gw;
    ip_addr_t addr_val;

    bflb_os_enter_critical();
    ip = wifiMgmr.wlan_sta.ipv4.ip;
    mask = wifiMgmr.wlan_sta.ipv4.mask;
    gw = wifiMgmr.wlan_sta.ipv4.gw;
    dns1 = wifiMgmr.wlan_sta.ipv4.dns1;
    dns2 = wifiMgmr.wlan_sta.ipv4.dns2;
    bflb_os_exit_critical();

    if (ip) {
        /*use static IP address*/
        bflb_os_printf(DEBUG_HEADER "Static IP Starting...%p\r\n", &(wifiMgmr.wlan_sta.netif));

        ip4_addr_set_u32(&addr_ipaddr, dns1);
        ip_addr_copy_from_ip4(addr_val, addr_ipaddr);
        dns_setserver(0, &addr_val);
        ip4_addr_set_u32(&addr_ipaddr, dns2);
        ip_addr_copy_from_ip4(addr_val, addr_ipaddr);
        dns_setserver(1, &addr_val);

        ip4_addr_set_u32(&addr_ipaddr, ip);
        ip4_addr_set_u32(&addr_netmask, mask);
        ip4_addr_set_u32(&addr_gw, gw);
        wifi_netif_dhcp_stop(&(wifiMgmr.wlan_sta.netif));
        netifapi_netif_set_addr(&(wifiMgmr.wlan_sta.netif), &addr_ipaddr, &addr_netmask, &addr_gw);
    } else {
        /*use DHCP*/
        if (use_dhcp) {
            bflb_os_printf(DEBUG_HEADER "DHCP Starting...%p\r\n", &(wifiMgmr.wlan_sta.netif));
            wifi_netif_dhcp_start(&(wifiMgmr.wlan_sta.netif));
        }
    }
}

static void stateConnectedIPNoEnter(void *stateData, struct event *event )
{
    connectedIPNoData_t *stateConnectedIPNo_data;
    wifi_mgmr_profile_msg_t profile = {};
    uint8_t use_dhcp = 1;

    if (_pending_task_is_set(WIFI_MGMR_PENDING_TASK_CONNECT_BIT)) {
        //disconnect, not need to clear pending
        bflb_os_printf("IPNO enter, disconnect\r\n");
        wifi_mgmr_sta_disconnect();
        bflb_os_msleep(WIFI_MGMR_STA_DISCONNECT_DELAY);
        return; 
    }

    if (0 == wifi_mgmr_profile_get_by_idx(&wifiMgmr, &profile, wifiMgmr.profile_active_index)) {
        use_dhcp = profile.dhcp_use;
    }

    stateConnectedIPNo_data = stateData;
    bflb_os_printf(DEBUG_HEADER "Entering %s state\r\n", stateConnectedIPNo_data->name);
#ifdef DEBUG_CONNECT_ABORT
    unsigned long now = bflb_os_get_time_ms();
    bflb_os_printf("Entering %s state, up time is %.1fs, cost time is %.1fs\r\n", (char *)stateData, now/1000.0, (now - wifiMgmr.connect_time)/1000.0);
#endif

#ifndef CFG_NETBUS_WIFI_ENABLE
    /* timeout 15 seconds for ip obtaining */
    if (use_dhcp) {
        stateConnectedIPNo_data->timer = bflb_os_timer_create(ip_obtaining_timeout, stateConnectedIPNo_data);
        bflb_os_timer_start_once(stateConnectedIPNo_data->timer, 15, 0);
    } else {
        stateConnectedIPNo_data->timer = NULL;
    }

    __sta_setup_ip(use_dhcp);
#endif
    platform_post_event(EV_WIFI, CODE_WIFI_ON_CONNECTED, 0);
}

static void periodic_tsen_reload(void *stateData, struct event *event )
{
    tsen_reload_data_t *state_tsen_data;

    state_tsen_data = stateData;
    bflb_os_printf(DEBUG_HEADER "reload tsen \r\n");
 
    state_tsen_data->timer = bflb_os_timer_create(__reload_tsen,
                                                          state_tsen_data);
    /* timeout 15 seconds for ip obtaining */
    bflb_os_timer_start_periodic(state_tsen_data->timer,
                                         (TSEN_RELOAD_MS / 1000),
                                         ((TSEN_RELOAD_MS % 1000) * 1e6));
}


static void stateConnectedIPNoExit(void *stateData, struct event *event )
{
    connectedIPNoData_t *stateConnectedIPNo_data;

    stateConnectedIPNo_data = stateData;
    bflb_os_printf(DEBUG_HEADER "Exiting %s state\r\n", stateConnectedIPNo_data->name);
    if (stateConnectedIPNo_data->timer) {
        bflb_os_timer_delete(stateConnectedIPNo_data->timer, BL_OS_NO_WAITING);
        stateConnectedIPNo_data->timer = NULL;
    }
}

static void stateConnectedIPNoAction_disconn( void *oldStateData, struct event *event, void *newStateData)
{
    bflb_os_printf(DEBUG_HEADER "State Action ###%s### --->>> ###%s###\r\n",
            (char*)oldStateData,
            (char*)newStateData
    );

    wifiMgmr.wlan_sta.sta.rssi = 0;
    wifi_netif_dhcp_stop(&(wifiMgmr.wlan_sta.netif));
    netifapi_netif_set_addr(&(wifiMgmr.wlan_sta.netif), NULL, NULL, NULL);
}

const static struct state stateConnectedIPNo = {
   .parentState = &stateGlobal,
   .entryState = NULL,
   .transitions = (struct transition[])
   {
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_IP_GOT, &stateGuard, &stateConnectedIPNoAction_ipgot, &stateConnectedIPYes},
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_DISCONNECT, &stateConnectedIPNoGuard_disconnect, &stateAction, &stateDisconnect},
      {EVENT_TYPE_FW, (void*)WIFI_MGMR_EVENT_FW_IND_DISCONNECT, &stateGuard, &stateConnectedIPNoAction_disconn, &stateDisconnect},
   },
   .numTransitions = 3,
   .data = &stateConnectedIPNo_data,
   .entryAction = &stateConnectedIPNoEnter,
   .exitAction = &stateConnectedIPNoExit,
};
/*==================================================================================================*/




/********************section for connected with IP address*************************/
static bool stateConnectedIPYesGuard_ip_update(void *ch, struct event *event)
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ch != (void*)msg->ev) {
        return false;
    }

    return true;
}

static bool stateConnectedIPYesGuard_disconnect( void *ch, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ch != (void*)msg->ev) {
        return false;
    }

    bl_main_disconnect();
    /*will never trigger state change, since we just want to trigger the guard*/
    return false;
}

static bool stateConnectedIPYesGuard_rcconfig( void *ch, struct event *event )
{
    wifi_mgmr_msg_t *msg;

    msg = event->data;
    if (ch != (void*)msg->ev) {
        return false;
    }

    bflb_os_printf(DEBUG_HEADER "rate config, use sta_idx 0, rate_config %04X\r\n", (unsigned int)(msg->data1));
    bl_main_rate_config(wifi_hw.vif_table[BL_VIF_STA].fixed_sta_idx, (uint32_t)msg->data1);
    /*will never trigger state change, since we just want to trigger the guard*/
    return false;
}

static void stateConnectedIPYes_action( void *oldStateData, struct event *event,
      void *newStateData )
{
    bflb_os_printf(DEBUG_HEADER "State Action ###%s### --->>> ###%s###\r\n",
            (char*)oldStateData,
            (char*)newStateData
    );
    wifiMgmr.wlan_sta.sta.rssi = 0;
    wifi_netif_dhcp_stop(&(wifiMgmr.wlan_sta.netif));
    netifapi_netif_set_addr(&(wifiMgmr.wlan_sta.netif), NULL, NULL, NULL);
}
static void sta_network_info_dump(void)
{
    uint8_t mac[6];
    char *state = "UP";
    char *connected = "CONNECTED";
    char *ifname = "wl1";
    char fmt[] = "%s: MAC=%02x:%02x:%02x:%02x:%02x:%02x ip=%d.%d.%d.%d/%d %s,%s\r\n";
    uint32_t ip, mask, gw;

    wifi_mgmr_sta_mac_get(mac);
    wifi_mgmr_sta_ip_get(&ip, &gw, &mask);

    bflb_os_printf(fmt, ifname,
                  mac[0], mac[1], mac[2],
                  mac[3], mac[4], mac[5],
                  ip & 0xff, (ip >> 8) & 0xff,
                  (ip >> 16) & 0xff, (ip >> 24) & 0xff,
                  32 - __builtin_clz(mask), state, connected);
}

static void stateConnectedIPYes_enter( void *stateData, struct event *event )
{
    // XXX: tell FW GOT IP
    // uint32_t trigger_flag = 0;
    // bl_main_cfg_task_req(CFG_ELEMENT_TYPE_OPS_SET, TASK_SM,
    //                      TASK_SM_CFG_RECONNECT_TRIGGER_FLAG,
    //                      CFG_ELEMENT_TYPE_UINT32, &trigger_flag, NULL);

    if (_pending_task_is_set(WIFI_MGMR_PENDING_TASK_CONNECT_BIT)) {
        //disconnect, not need to clear pending
        bflb_os_printf("IPYES enter, disconnect\r\n");
        wifi_mgmr_sta_disconnect();
        bflb_os_msleep(WIFI_MGMR_STA_DISCONNECT_DELAY);
        return; 
    }

    bflb_os_printf(DEBUG_HEADER "Entering %s state\r\n", (char *)stateData);
#ifdef DEBUG_CONNECT_ABORT
    unsigned long now = bflb_os_get_time_ms();
    bflb_os_printf("Entering %s state, up time is %.1fs, cost time is %.1fs\r\n", (char *)stateData, now/1000.0, (now - wifiMgmr.connect_time)/1000.0);
#endif
    sta_network_info_dump();
    platform_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0);
    if (_pending_task_is_set(WIFI_MGMR_PENDING_TASK_SCAN_BIT)) {
        bflb_os_printf(DEBUG_HEADER "Pending Scan Sent\r\n");
        bl_main_scan(&wifiMgmr.wlan_sta.netif, NULL, 0, (struct mac_addr *)&mac_addr_bcst, NULL, 0, 0);
        _pending_task_clr_safely(WIFI_MGMR_PENDING_TASK_SCAN_BIT);
    }
}

static void stateConnectedIPYes_exit( void *stateData, struct event *event )
{
    bflb_os_printf(DEBUG_HEADER "Exiting %s state\r\n", (char *)stateData);


    //FIXME TODO ugly hack
    if (auto_repeat) {
        auto_repeat = 0;
        bl_main_denoise(0);
    }
}

const static struct state stateConnectedIPYes = {
   .parentState = &stateGlobal,
   .entryState = NULL,
   .transitions = (struct transition[])
   {
      {EVENT_TYPE_GLB, (void*)WIFI_MGMR_EVENT_GLB_IP_UPDATE, &stateConnectedIPYesGuard_ip_update, &stateAction, &stateConnectedIPNo},
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_DISCONNECT, &stateConnectedIPYesGuard_disconnect, &stateConnectedIPYes_action, &stateDisconnect},
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_RC_CONFIG, &stateConnectedIPYesGuard_rcconfig, &stateConnectedIPYes_action, &stateDisconnect},
      {EVENT_TYPE_FW, (void*)WIFI_MGMR_EVENT_FW_IND_DISCONNECT, &stateGuard, &stateConnectedIPYes_action, &stateDisconnect},
   },
   .numTransitions = 4,
   .data = "wifiConnected_IPOK",
   .entryAction = &stateConnectedIPYes_enter,
   .exitAction = &stateConnectedIPYes_exit,
};
/*==================================================================================================*/

/********************section for disconnect *************************/
typedef struct disconnectData {
    char name[32];//all the state data must start with name field
    void *timer;
    uint8_t timer_started;
    wifi_mgmr_profile_msg_t profile_msg;
} disconnectData_t;
static disconnectData_t stateDisconnect_data = {
    .name = "disconnect",
};

static void stateDisconnect_action_reconnect( void *oldStateData, struct event *event,
      void *newStateData )
{
    disconnectData_t *stateDisconnect_data;
    wifi_mgmr_profile_msg_t *profile_msg;
    uint8_t band = 0;
    uint16_t freq = 0;
    uint8_t *bssid = NULL;
    uint8_t null_bssid[6] = {};


    stateDisconnect_data = (disconnectData_t*)oldStateData;
    profile_msg = &(stateDisconnect_data->profile_msg);
    bflb_os_printf(DEBUG_HEADER "State Action ###%s### --->>> ###%s###\r\n",
            (char*)oldStateData,
            (char*)newStateData
    );

    if (profile_msg->ap_info_ttl != -1) {
      /* if not ap info type presist */
      if (wifiMgmr.ap_info_ttl_curr == -1) {
        wifiMgmr.ap_info_ttl_curr = profile_msg->ap_info_ttl;
      }

      if (wifiMgmr.ap_info_ttl_curr == 0) {
        band = 0;
        freq = 0;
        bssid = NULL;
      } else {
        wifiMgmr.ap_info_ttl_curr--;
        //assert(wifiMgmr.ap_info_ttl_curr >= 0);

        band = profile_msg->band;
        freq = profile_msg->freq;
        bssid = profile_msg->bssid;
      }
    } else {
      band = profile_msg->band;
      freq = profile_msg->freq;
      bssid = profile_msg->bssid;
    }

    dump_connect_param(profile_msg, band, freq, bssid ? bssid : null_bssid);

    // XXX: tell FW reconnect happening
    // uint32_t trigger_flag = 1;
    // bl_main_cfg_task_req(CFG_ELEMENT_TYPE_OPS_SET, TASK_SM,
    //                      TASK_SM_CFG_RECONNECT_TRIGGER_FLAG,
    //                      CFG_ELEMENT_TYPE_UINT32, &trigger_flag, NULL);

    //TODO Other security support
    bl_main_connect((const uint8_t *)profile_msg->ssid, profile_msg->ssid_len,
            (const uint8_t *)profile_msg->passphr, profile_msg->passphr_len,
            (const uint8_t *)profile_msg->psk, profile_msg->psk_len,
            (const uint8_t *)bssid,
            (const uint8_t)band,
            (const uint16_t)freq,
            (const uint32_t)profile_msg->flags
    );
    platform_post_event(EV_WIFI, CODE_WIFI_CMD_RECONNECT, 0);
}

static void stateDisconnect_action_idle( void *oldStateData, struct event *event,
      void *newStateData )
{
    bflb_os_printf(DEBUG_HEADER "State Action ###%s### --->>> ###%s###\r\n",
            (char*)oldStateData,
            (char*)newStateData
    );
    bflb_os_printf(DEBUG_HEADER "Removing STA interface...\r\n");
    bl_main_if_remove(BL_VIF_STA);
}

static void disconnect_retry(void *data)
{
    disconnectData_t *stateData = (disconnectData_t *)data;
    int ret = 0;

    /*XXX may in the handler mode*/
    /*TODO use EVENT to copy profile*/
    if (_pending_task_is_set(WIFI_MGMR_PENDING_TASK_CONNECT_BIT)) {
        /* clear pending */
        _pending_task_clr_safely(WIFI_MGMR_PENDING_TASK_CONNECT_BIT);

        /* get profile[1] */
        ret = wifi_mgmr_profile_get_by_idx(&wifiMgmr, &(stateData->profile_msg), 1);
        if (ret >= 0) {
            /* copy profile[1] to profile[0] */
            wifi_mgmr_profile_del_by_idx(&wifiMgmr, 0);
            wifi_mgmr_profile_add_by_idx(&wifiMgmr, &(stateData->profile_msg), 0, 0);
            /* set profile[1] active */
            wifi_mgmr_profile_set_active_by_idx(&wifiMgmr, 1, 1);
            wifiMgmr.ap_info_ttl_curr = -1;
        }
    } else {
        ret = wifi_mgmr_profile_get_by_idx(&wifiMgmr, &(stateData->profile_msg), 0);
    }
    if (ret < 0) {
        bflb_os_printf(DEBUG_HEADER "Retry Again --->>> retry Abort, since profile copy failed, ret is %d, wifiMgmr.profile_active_index is %u\r\n", ret, wifiMgmr.profile_active_index);
    } else {
        bflb_os_printf(DEBUG_HEADER "Retry Again --->>> retry connect\r\n");
        wifi_mgmr_api_reconnect();
    }
}

static void stateDisconnect_enter(void *stateData, struct event *event)
{
#ifdef CFG_CHIP_BL602
    int is_ok = 0;
#endif
    disconnectData_t *stateDisconnect_data;

    stateDisconnect_data = stateData;
    bflb_os_printf(DEBUG_HEADER "Entering %s state\r\n", (char *)stateData);
#ifdef DEBUG_CONNECT_ABORT
    unsigned long now = bflb_os_get_time_ms();
    bflb_os_printf("Entering %s state, up time is %.1fs, cost time is %.1fs\r\n", (char *)stateData, now/1000.0, (now - wifiMgmr.connect_time)/1000.0);
#endif
    if (wifi_mgmr_profile_autoreconnect_is_enabled(&wifiMgmr, -1) || _pending_task_is_set(WIFI_MGMR_PENDING_TASK_CONNECT_BIT)) {
        stateDisconnect_data->timer = bflb_os_timer_create(disconnect_retry,
                                                                   stateDisconnect_data);
        if (_pending_task_is_set(WIFI_MGMR_PENDING_TASK_CONNECT_BIT)) {
            bflb_os_timer_start_once(stateDisconnect_data->timer, 0, 1e6+1);
        } else {
            bflb_os_timer_start_once(stateDisconnect_data->timer, 2, 0);
        }
        stateDisconnect_data->timer_started = 1;
    } else {
        bflb_os_printf(DEBUG_HEADER "Will NOT retry connect\r\n");
        wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_APP_IDLE, (void*)0x1, (void*)0x2);
    }
    platform_post_event(EV_WIFI, CODE_WIFI_ON_DISCONNECT, wifiMgmr.wifi_mgmr_stat_info.status_code);

#ifdef CFG_CHIP_BL602
    if (0 == bl60x_check_mac_status(&is_ok) && 0 == is_ok) {
        platform_post_event(EV_WIFI, CODE_WIFI_ON_EMERGENCY_MAC, 0);
        //TODO Fix ugly header file hack
void helper_record_dump();
            helper_record_dump();
    }
#endif

    if (_pending_task_is_set(WIFI_MGMR_PENDING_TASK_SCAN_BIT)) {
        bflb_os_printf(DEBUG_HEADER "Pending Scan Sent\r\n");
        bl_main_scan(&wifiMgmr.wlan_sta.netif, NULL, 0, (struct mac_addr *)&mac_addr_bcst, NULL, 0, 0);
        _pending_task_clr_safely(WIFI_MGMR_PENDING_TASK_SCAN_BIT);
    }
}

static void stateDisconnect_exit(void *stateData, struct event *event)
{
    disconnectData_t *stateDisconnect_data = stateData;

    bflb_os_printf(DEBUG_HEADER "Exiting %s state\r\n", (char *)stateDisconnect_data);
    if (stateDisconnect_data->timer_started) {
        bflb_os_printf("Delete Timer.\r\n");
        bflb_os_timer_delete(stateDisconnect_data->timer, BL_OS_NO_WAITING);
        stateDisconnect_data->timer_started = 0;
    } else {
        bflb_os_printf("Delete Timer Skipped\r\n");
    }
}

const static struct state stateDisconnect = {
   .parentState = &stateGlobal,
   .entryState = NULL,
   .transitions = (struct transition[])
   {
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_RECONNECT, &stateGuard, &stateDisconnect_action_reconnect, &stateConnecting},
      {EVENT_TYPE_APP, (void*)WIFI_MGMR_EVENT_APP_IDLE, &stateGuard, &stateDisconnect_action_idle, &stateIdle},
   },
   .numTransitions = 2,
   .data = &stateDisconnect_data,
   .entryAction = &stateDisconnect_enter,
   .exitAction = &stateDisconnect_exit,
};
/*==================================================================================================*/


const static struct state stateError = {
   .entryAction = &printErrMsg
};

int wifi_mgmr_pending_task_set(uint32_t bits)
{
    _pending_task_set_safely(bits);
    return 0;
}

int wifi_mgmr_event_notify(wifi_mgmr_msg_t *msg, int use_block)
{
    int ret;
    uint8_t limit = 50;

    while (0 == wifiMgmr.ready) {
        bflb_os_printf("Waiting Wi-Fi Mgmr Start up...\r\n");
        bflb_os_msleep(20);

        if (0 == (limit--))
        {
            bflb_os_printf("Wi-Fi Mgmr NOT Start up! Start it first!\r\n");
            return -1;
        }
    }
    ret = use_block ? bflb_os_queue_send_wait(wifiMgmr.mq, msg, sizeof(wifi_mgmr_msg_t), BL_OS_WAITING_FOREVER, 0) :
                      bflb_os_queue_send(wifiMgmr.mq, msg, sizeof(wifi_mgmr_msg_t));
    if (ret) {
        bflb_os_printf("Failed when send msg 0x%p, ev :%d\r\n", msg, msg->ev);
        return -1;
    }
    return 0;
}

static void event_cb_wifi_event_mgmr(input_event_t *event, void *private_data)
{
    switch (event->code) {
        case CODE_WIFI_ON_MGMR_DENOISE:
        {
            platform_post_delayed_action(85, trigger_auto_denoise, NULL);
        }
        break;
        default:
        {
            /*nothing*/
        }
    }
}

static uint32_t handle_pending_task(wifi_mgmr_msg_t *msg)
{
    if (_pending_task_is_set(WIFI_MGMR_PENDING_TASK_CONNECT_BIT)) {
            /* clear pending */
            _pending_task_clr_safely(WIFI_MGMR_PENDING_TASK_IP_UPDATE_BIT);
            _pending_task_clr_safely(WIFI_MGMR_PENDING_TASK_IP_GOT_BIT);
            return 0;
    }

    if (_pending_task_is_set(WIFI_MGMR_PENDING_TASK_IP_UPDATE_BIT)) {
        _pending_task_clr_safely(WIFI_MGMR_PENDING_TASK_IP_UPDATE_BIT);
        msg->ev = WIFI_MGMR_EVENT_GLB_IP_UPDATE;
        msg->data1 = (void*)0x01;
        msg->data2 = (void*)0x02;
        return WIFI_MGMR_PENDING_TASK_IP_UPDATE_BIT;
    }

    if (_pending_task_is_set(WIFI_MGMR_PENDING_TASK_IP_GOT_BIT)) {
        _pending_task_clr_safely(WIFI_MGMR_PENDING_TASK_IP_GOT_BIT);
        msg->ev = WIFI_MGMR_EVENT_APP_IP_GOT;
        msg->data1 = (void*)0x01;
        msg->data2 = (void*)0x02;
        return WIFI_MGMR_PENDING_TASK_IP_GOT_BIT;
    }

    return 0;
}

void wifi_mgmr_start(void)
{
    struct event ev;
    wifi_mgmr_msg_t msg;

    ev.type = EVENT_TYPE_APP;
    ev.data = &msg;
    stateM_init(&(wifiMgmr.m), &stateIfaceDown, &stateError);

    wifiMgmr.scan_items_lock = bflb_os_mutex_create();
    wifiMgmr.wifi_mgmr_stat_info.diagnose_lock = bflb_os_mutex_create();
    wifiMgmr.wifi_mgmr_stat_info.diagnose_get_lock = bflb_os_mutex_create();
    /*register event cb for Wi-Fi Manager*/
    wifi_mgmr_event_init();

    /*register filter for call in event loop*/
    platform_register_event(EV_WIFI, event_cb_wifi_event_mgmr, NULL);
    /*Noitfy mgmr is ready*/
    platform_post_event(EV_WIFI, CODE_WIFI_ON_MGMR_DONE, 0);

    /*TODO: use another way based on event sys?*/
    platform_sys_capcode_update(255, 255);

    /*periodic reload tsen */
    periodic_tsen_reload(&state_tsen_reload_data, NULL);

    /*wifi mgmr pm ops register */
    wifi_mgmr_pm_ops_register();
    wifi_mgmr_fw_affair_ops();

    /*Run the event handler loop*/
    while (1) {
        if (0 == bflb_os_queue_recv(wifiMgmr.mq, &msg, sizeof(wifi_mgmr_msg_t), BL_OS_WAITING_FOREVER)) {

handle_msg:
            ev.type = msg.ev < WIFI_MGMR_EVENT_MAXAPP_MINIFW ? EVENT_TYPE_APP :
                (msg.ev < WIFI_MGMR_EVENT_MAXFW_MINI_GLOBAL ? EVENT_TYPE_FW : EVENT_TYPE_GLB);
            if (msg.ev == WIFI_MGMR_EVENT_APP_RELOAD_TSEN) {
                __run_reload_tsen();
            } else {
                stateM_handleEvent(&(wifiMgmr.m), &ev);
            }

            if (handle_pending_task(&msg)) {
                goto handle_msg;
            }

            if (msg.data) {
                bflb_os_free(msg.data);
            }
        }
    }
}

static void _wifi_mgmr_entry(void *pvParameters)
{
    wifi_conf_t *conf = (wifi_conf_t *)pvParameters;
    wifi_mgmr_drv_init(conf);
    wifi_mgmr_start();
}

void wifi_mgmr_start_background(wifi_conf_t *conf)
{
    wifi_conf_t *conf_dup = bflb_os_zalloc(sizeof(wifi_conf_t));
    memcpy(conf_dup, conf, sizeof(wifi_conf_t));
    bflb_os_task_create("wifi_mgmr", _wifi_mgmr_entry, 1024 * 3, conf_dup, mgmr_TASK_PRIORITY, NULL);
}

int wifi_mgmr_init(wifi_conf_t *conf)
{
    wifi_mgmr_start_background(conf);
    return 0;
}

int wifi_mgmr_task_start(void)
{
    wifi_mgmr_init(&conf);
}

int wifi_mgmr_internel_init(void)
{
    int ret;

    wifiMgmr.mq = bflb_os_queue_create(WIFI_MGMR_MQ_MSG_COUNT, sizeof(wifi_mgmr_msg_t));
    assert((ret = (NULL != wifiMgmr.mq)));

    wifiMgmr.ready = 1;//TODO check ret
    wifiMgmr.ap_bcn_int = 100;
    wifiMgmr.ap_info_ttl_curr = -1;
    wifiMgmr.scan_item_timeout = WIFI_MGMR_CONFIG_SCAN_ITEM_TIMEOUT;
    return ret;
}

int wifi_mgmr_status_code_get_internal(int *s_code)
{
    (*s_code) = wifiMgmr.wifi_mgmr_stat_info.status_code;
    return 0;
}

int wifi_mgmr_status_code_clean_internal()
{
    wifiMgmr.wifi_mgmr_stat_info.status_code = WLAN_FW_SUCCESSFUL;
    return 0;
}

int wifi_mgmr_detailed_state_get_internal(int *state, int *state_detailed)
{
    const struct state *m_state;
    int s_code = 0;

    m_state = wifiMgmr.m.currentState;
    wifi_mgmr_status_code_get_internal(&s_code);

    if (m_state == &stateIdle) {
        if (wifiMgmr.inf_ap_enabled) {
            *state = WIFI_STATE_WITH_AP_IDLE;
        } else {
            *state = WIFI_STATE_IDLE;
            if (s_code == WLAN_FW_4WAY_HANDSHAKE_ERROR_PSK_TIMEOUT_FAILURE){
                *state_detailed = WIFI_STATE_PSK_ERROR;
            } else if (s_code == WLAN_FW_SCAN_NO_BSSID_AND_CHANNEL){
                *state_detailed = WIFI_STATE_NO_AP_FOUND;
            }
        }
    } else if (m_state == &stateConnecting) {
        if (wifiMgmr.inf_ap_enabled) {
            *state = WIFI_STATE_WITH_AP_CONNECTING;
        } else {
            *state = WIFI_STATE_CONNECTING;
            if (s_code == WLAN_FW_4WAY_HANDSHAKE_ERROR_PSK_TIMEOUT_FAILURE){
                *state_detailed = WIFI_STATE_PSK_ERROR;
            } else if (s_code == WLAN_FW_SCAN_NO_BSSID_AND_CHANNEL){
                *state_detailed = WIFI_STATE_NO_AP_FOUND;
            }
        }
    } else if (m_state == &stateConnectedIPNo) {
        if (wifiMgmr.inf_ap_enabled) {
            *state = WIFI_STATE_WITH_AP_CONNECTED_IP_GETTING;
        } else {
            *state = WIFI_STATE_CONNECTED_IP_GETTING;
        }
    } else if (m_state == &stateConnectedIPYes) {
        if (wifiMgmr.inf_ap_enabled) {
            *state = WIFI_STATE_WITH_AP_CONNECTED_IP_GOT;
        } else {
            *state = WIFI_STATE_CONNECTED_IP_GOT;
        }
    } else if (m_state == &stateDisconnect) {
        if (wifiMgmr.inf_ap_enabled) {
            *state = WIFI_STATE_WITH_AP_DISCONNECT;
        } else {
            *state = WIFI_STATE_DISCONNECT;
            if (s_code == WLAN_FW_4WAY_HANDSHAKE_ERROR_PSK_TIMEOUT_FAILURE){
                *state_detailed = WIFI_STATE_PSK_ERROR;
            } else if (s_code == WLAN_FW_SCAN_NO_BSSID_AND_CHANNEL){
                *state_detailed = WIFI_STATE_NO_AP_FOUND;
            }
        }
    } else if (m_state == &stateIfaceDown) {
        *state = WIFI_STATE_IFDOWN;
    } else if (m_state == &stateSniffer) {
        *state = WIFI_STATE_SNIFFER;
    } else {
        *state = WIFI_STATE_UNKNOWN;
    }

    return 0;
}

int wifi_mgmr_state_get_internal(int *state)
{
    return wifi_mgmr_detailed_state_get_internal(state, state);
}

void wifi_mgmr_set_connect_stat_info(struct wifi_event_sm_connect_ind *ind, uint8_t type_ind)
{
    int i;
    wifiMgmr.wifi_mgmr_stat_info.status_code = ind->status_code;
    wifiMgmr.wifi_mgmr_stat_info.reason_code = ind->reason_code;
    for(i = 0;i < 6; i++)
        wifiMgmr.wifi_mgmr_stat_info.bssid[i] = ind->bssid[i];
    wifiMgmr.wifi_mgmr_stat_info.chan_freq = ind->center_freq;
    wifiMgmr.wifi_mgmr_stat_info.chan_band = ind->band;
    wifiMgmr.wifi_mgmr_stat_info.type_ind = type_ind;

    switch (ind->band) {
        case PHY_BAND_2G4:
        {
            if (ind->center_freq == 2484) {
                wifiMgmr.channel = 14;
            } else if (ind->center_freq >= 2412 && ind->center_freq <= 2472) {
                wifiMgmr.channel = (ind->center_freq - 2412)/5 + 1;
            } else {
                bflb_os_log_info("err: chan is invalid\r\n");
                wifiMgmr.channel = 0;
            }
            break;
        }
        case PHY_BAND_5G:
            break;
        default:
            bflb_os_log_info("err: chan is invalid\r\n");
    }

    bflb_os_log_info("[RX] wifi_mgmr_set_connect_stat_info, wifiMgmr.wifi_mgmr_stat_info:\r\n");
    bflb_os_log_info("[RX]   status_code %u\r\n", wifiMgmr.wifi_mgmr_stat_info.status_code);
    bflb_os_log_info("[RX]   reason_code %u\r\n", wifiMgmr.wifi_mgmr_stat_info.reason_code);
    bflb_os_log_info("[RX]   MAC %02X:%02X:%02X:%02X:%02X:%02X\r\n",
             wifiMgmr.wifi_mgmr_stat_info.bssid[0],
             wifiMgmr.wifi_mgmr_stat_info.bssid[1],
             wifiMgmr.wifi_mgmr_stat_info.bssid[2],
             wifiMgmr.wifi_mgmr_stat_info.bssid[3],
             wifiMgmr.wifi_mgmr_stat_info.bssid[4],
             wifiMgmr.wifi_mgmr_stat_info.bssid[5]
    );
    bflb_os_log_info("[RX]   band %u\r\n", wifiMgmr.wifi_mgmr_stat_info.chan_band);
    bflb_os_log_info("[RX]   center_freq %u\r\n", wifiMgmr.wifi_mgmr_stat_info.chan_freq);
    bflb_os_log_info("[RX]   type_ind %u\r\n", wifiMgmr.wifi_mgmr_stat_info.type_ind);
}

int wifi_mgmr_set_country_code_internal(char *country_code)
{
    int ret;
    ret = bl_main_set_country_code(country_code);
    if (ret != 0) {
        return ret;
    }
    strncpy(wifiMgmr.country_code, country_code, sizeof(wifiMgmr.country_code));
    wifiMgmr.country_code[2] = '\0';
    wifiMgmr.channel_nums = bl_main_get_channel_nums();
    bflb_os_log_info("country code:%s, support channel nums:%d\r\n", wifiMgmr.country_code, wifiMgmr.channel_nums);

    return 0;
}

int wifi_mgmr_ap_sta_cnt_get_internal(uint8_t *sta_cnt)
{
    return bl_main_apm_sta_cnt_get(sta_cnt);
}

int wifi_mgmr_ap_sta_info_get_internal(wifi_mgmr_sta_basic_info_t *sta_info_internal, uint8_t idx)
{
    struct wifi_apm_sta_info apm_sta_info;
    memset(&apm_sta_info, 0, sizeof(struct wifi_apm_sta_info));
    bl_main_apm_sta_info_get(&apm_sta_info, idx);
    sta_info_internal->sta_idx = apm_sta_info.sta_idx;
    sta_info_internal->is_used = apm_sta_info.is_used;
    sta_info_internal->rssi = apm_sta_info.rssi;
    sta_info_internal->tsflo = apm_sta_info.tsflo;
    sta_info_internal->tsfhi = apm_sta_info.tsfhi;
    sta_info_internal->data_rate = apm_sta_info.data_rate;
    memcpy(sta_info_internal->sta_mac, apm_sta_info.sta_mac, 6);
    return 0;
}

int wifi_mgmr_ap_sta_delete_internal(uint8_t sta_idx)
{
    return bl_main_apm_sta_delete(sta_idx);
}

int wifi_mgmr_scan_complete_notify()
{
    return wifi_mgmr_scan_complete_callback();
}

int wifi_mgmr_api_fw_powersaving_get(void)
{
    return bl_main_powersaving_get();
}

int fhost_init(void)
{
    return 0;
}
