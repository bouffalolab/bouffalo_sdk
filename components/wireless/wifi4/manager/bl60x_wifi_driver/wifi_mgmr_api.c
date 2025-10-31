#include <string.h>
#include <stdio.h>

#include "wifi_mgmr_api.h"
#include "bl_defs.h"

#define MAX_SSID_LEN_CHECK 32
#define MAX_PSK_LEN_CHECK 64

static int wifi_mgmr_api_common(WIFI_MGMR_EVENT_T ev, void *data1, void *data2, uint8_t *data_in_heap)
{
    wifi_mgmr_msg_t msg;

    msg.ev = ev;
    msg.data1 = data1;
    msg.data2 = data2;
    msg.data = data_in_heap;

    if (0 != wifi_mgmr_event_notify(&msg, 0)) {
        if (data_in_heap) {
            bflb_os_free(data_in_heap);
        }
        return -1;
    }

    return 0;
}

int wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_T ev, void *data1, void *data2)
{
    return wifi_mgmr_api_common(ev, data1, data2, NULL);
}

int wifi_mgmr_api_connect(char *ssid, char *passphr, const ap_connect_adv_t *ext_param)
{
    wifi_mgmr_profile_msg_t *profile = NULL;

    profile = (wifi_mgmr_profile_msg_t *)bflb_os_zalloc(sizeof(wifi_mgmr_profile_msg_t));
    if (!profile) {
        goto failed;
    }

    profile->ssid_len = strlen(ssid);//ssid should never be NULL
    memcpy(profile->ssid, ssid, profile->ssid_len);
    profile->ssid_tail[0] = '\0';

    profile->passphr_len = passphr ? strlen(passphr) : 0;//passphr can be NULL
    if (profile->passphr_len > sizeof(profile->passphr)) {
        goto failed;
    } else if (profile->passphr_len > 0) {
        memcpy(profile->passphr, passphr, profile->passphr_len);
    }
    profile->passphr_tail[0] = '\0';

    profile->psk_len = ext_param->psk ? strlen(ext_param->psk) : 0; //psk can be NULL
    if (0 != profile->psk_len && sizeof(profile->psk) != profile->psk_len) {
        goto failed;
    } else if (sizeof(profile->psk) == profile->psk_len) {
        memcpy(profile->psk, ext_param->psk, profile->psk_len);
    } else if (0 == profile->psk_len && profile->passphr_len && profile->passphr_len >= 8) {
        // Put PSK calculation here, Otherwise it will influence FW Response performance
        if (wifi_mgmr_psk_cal(profile->passphr, profile->ssid, profile->ssid_len, profile->psk)){
            return -1;
        }
        profile->psk_len = sizeof(profile->psk);
    }
    profile->psk_tail[0] = '\0';

    if (ext_param->ap_info.bssid) {
        memcpy(profile->bssid, ext_param->ap_info.bssid, sizeof(profile->bssid));
    }

    if (ext_param->ap_info.freq > 0) {
        //define the freq
        profile->band = ext_param->ap_info.band;
        profile->freq = ext_param->ap_info.freq;
        bflb_os_printf("wifi mgmr band:%d freq: %d\r\n", profile->band, profile->freq);
    }

    if (ext_param->ap_info.type == AP_INFO_TYPE_PRESIST) {
        profile->ap_info_ttl = -1;
    } else if (ext_param->ap_info.time_to_live >= 0){
        profile->ap_info_ttl = ext_param->ap_info.time_to_live;
    } else {
        profile->ap_info_ttl = -1;
        bflb_os_printf("invalid ap info type or time_to_live value!\r\n");
    }

    profile->dhcp_use = ext_param->ap_info.use_dhcp;
    profile->flags = ext_param->flags;

    return wifi_mgmr_api_common(
        WIFI_MGMR_EVENT_APP_CONNECT,
        (void*)0x1,
        (void*)0x2,
        (void*)profile
    );

failed:
    if (profile) {
        bflb_os_printf("%s malloc profile failed!\r\n", __FUNCTION__);
        bflb_os_free(profile);
    } else {
        bflb_os_printf("%s send profile failed!\r\n", __FUNCTION__);
    }
    return -1;
}

int wifi_mgmr_api_cfg_req(uint32_t ops, uint32_t task, uint32_t element, uint32_t type, uint32_t length, uint32_t *buf)
{
    if (length > MAX_LENGTH_LIMIT) {
        return -1;
    }

    wifi_mgmr_cfg_element_msg_t *cfg_req = NULL;
    cfg_req = (wifi_mgmr_cfg_element_msg_t *)bflb_os_zalloc(sizeof(wifi_mgmr_cfg_element_msg_t) + length);
    if (!cfg_req) {
        bflb_os_printf("%s malloc cfg_req failed!\r\n", __FUNCTION__);
        return -1;
    }

    cfg_req->ops = ops;
    cfg_req->task = task;
    cfg_req->element = element;
    cfg_req->type = type;
    cfg_req->length = length;
    if (length) {
        memcpy(cfg_req->buf, buf, length);
    }

    return wifi_mgmr_api_common(
        WIFI_MGMR_EVENT_FW_CFG_REQ,
        (void*)0x1,
        (void*)0x2,
        (void*)cfg_req
    );
}

int wifi_mgmr_api_ip_got(void)
{
    wifi_mgmr_pending_task_set(WIFI_MGMR_PENDING_TASK_IP_GOT_BIT);
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_GLB_MGMR_WAKEUP, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_ip_update(void)
{
    wifi_mgmr_pending_task_set(WIFI_MGMR_PENDING_TASK_IP_UPDATE_BIT);
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_GLB_MGMR_WAKEUP, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_reconnect(void)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_APP_RECONNECT, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_disable_autoreconnect(void)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_GLB_DISABLE_AUTORECONNECT, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_enable_autoreconnect(void)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_GLB_ENABLE_AUTORECONNECT, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_disconnect(void)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_APP_DISCONNECT, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_rate_config(uint16_t config)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_APP_RC_CONFIG, (void*)(intptr_t)config, (void*)0x2);
}

int wifi_mgmr_api_conf_max_sta(uint8_t max_sta_supported)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_APP_CONF_MAX_STA, (void*)(intptr_t)max_sta_supported, (void*)0x2);
}

int wifi_mgmr_api_ifaceup(void)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_APP_PHY_UP, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_sniffer_enable(void)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_APP_SNIFFER, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_scan_item_beacon(uint8_t channel, int8_t rssi, uint8_t auth, uint8_t mac[], uint8_t ssid[], int len, int8_t ppm_abs, int8_t ppm_rel, uint8_t cipher, uint8_t wps, uint32_t mode, uint8_t group_cipher)
{
    wifi_mgmr_scan_item_t scan;

    memset(&scan, 0, sizeof(scan));
    memcpy(scan.ssid, ssid, len);
    scan.ssid_tail[0] = '\0';
    scan.ssid_len = len;
    memcpy(scan.bssid, mac, sizeof(scan.bssid));
    scan.channel = channel;
    scan.rssi = rssi;
    scan.auth = auth;
    scan.cipher = cipher;
    scan.ppm_abs = ppm_abs;
    scan.ppm_rel = ppm_rel;
    scan.wps = wps;
    scan.mode = mode;
    scan.group_cipher = group_cipher;

    return wifi_mgmr_scan_beacon_save(&scan);
}

int wifi_mgmr_api_fw_disconnect(void)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_FW_DISCONNECT, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_fw_tsen_reload(void)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_APP_RELOAD_TSEN, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_fw_scan(wifi_mgmr_scan_params_t *ch_req)
{
    return wifi_mgmr_api_common(
        WIFI_MGMR_EVENT_FW_SCAN,
        (void*)0x1,
        (void*)0x2,
        (void*)ch_req
    );
}

int wifi_mgmr_api_fw_powersaving(int mode)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_FW_POWERSAVING, (void*)mode, (void*)0x2);
}

int wifi_mgmr_api_ap_start(char *ssid, char *passwd, int channel, uint8_t hidden_ssid, int8_t max_sta_supported, uint8_t use_dhcp_server)
{
    wifi_mgmr_ap_msg_t *ap = NULL;
    int ssid_len = ssid ? strlen(ssid) : 0;
    int psk_len = passwd ? strlen(passwd) : 0;

    if (!ssid || ssid_len > MAX_SSID_LEN_CHECK || (passwd && (psk_len < 8 || psk_len >= MAX_PSK_LEN_CHECK))) {
        bflb_os_printf("%s error start ap with wrong paramters!\r\n", __FUNCTION__);
        return -1;
    }

    ap = (wifi_mgmr_ap_msg_t *)bflb_os_zalloc(sizeof(wifi_mgmr_ap_msg_t));
    if (!ap) {
        bflb_os_printf("%s malloc ap failed!\r\n", __FUNCTION__);
        goto failed;
    }

    ap->ssid_len = ssid_len;
    memcpy(ap->ssid, ssid, ap->ssid_len);
    if (psk_len) {
        // Put PSK calculation here, Otherwise it will influence FW Response performance
        if (wifi_mgmr_psk_cal(passwd, ssid, ssid_len, ap->psk)) {
            goto failed;
        }
        ap->psk_len = sizeof(ap->psk);
    }
    ap->channel = channel;
    ap->hidden_ssid = hidden_ssid ? 1 : 0;
    ap->use_dhcp_server = use_dhcp_server ? 1 : 0;
    ap->max_sta_supported = max_sta_supported;

    return wifi_mgmr_api_common(
        WIFI_MGMR_EVENT_APP_AP_START,
        (void*)0x1,
        (void*)0x2,
        (void*)ap
    );

failed:
    if (ap) {
        bflb_os_printf("%s malloc ap failed!\r\n", __FUNCTION__);
        bflb_os_free(ap);
    } else {
        bflb_os_printf("%s send ap failed!\r\n", __FUNCTION__);
    }
    return -1;
}

int wifi_mgmr_api_ap_stop(void)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_APP_AP_STOP, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_chan_switch(int channel, uint8_t cs_count)
{
    if (cs_count == 0) {
        cs_count = WIFI_MGMR_AP_CHAN_SWITCH_COUNT_DEFAULT;
    }
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_APP_AP_CHAN_SWITCH, (void *)(intptr_t)channel, (void *)(uintptr_t)cs_count);
}

int wifi_mgmr_api_idle(void)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_APP_IDLE, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_denoise_enable(void)
{
    return wifi_mgmr_api_common_msg(
        WIFI_MGMR_EVENT_APP_DENOISE,
        (void*)0x1,//use non-zero for enable denoise
        (void*)0x2
    );
}

int wifi_mgmr_api_denoise_disable(void)
{
    return wifi_mgmr_api_common_msg(
        WIFI_MGMR_EVENT_APP_DENOISE,
        (void*)0x0,//use non-zero for enable denoise
        (void*)0x2
    );
}

int wifi_mgmr_api_channel_set(int channel, int use_40Mhz)
{
    return wifi_mgmr_api_common_msg(
        WIFI_MGMR_EVENT_FW_CHANNEL_SET,
        (void*)channel,
        (void*)use_40Mhz
    );
}

/*TODO callback for RAW data send*/
int wifi_mgmr_api_raw_send(uint8_t *pkt, int len)
{
    return wifi_mgmr_api_common_msg(
        WIFI_MGMR_EVENT_FW_DATA_RAW_SEND,
        (void*)pkt,
        (void*)len
    );
}

int wifi_mgmr_api_set_country_code(char *country_code)
{
    return wifi_mgmr_set_country_code_internal(country_code);
}

