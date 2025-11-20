#include <assert.h>
#include "bl_lp.h"
#include "bl616_pds.h"
#include "macsw.h"
#include "bflb_common.h"
#include "wifi_mgmr.h"

static uint32_t ps_mamager_event = 0;

void bl_pm_event_bit_set(enum PSM_EVENT event_bit)
{
    ps_mamager_event |= 1 << event_bit;
}

void bl_pm_event_bit_clear(enum PSM_EVENT event_bit)
{
    ps_mamager_event &= ~(1 << event_bit);
}

uint32_t bl_pm_event_get(void)
{
    return ps_mamager_event;
}

void bl_pm_enter_ps(void)
{
    wifi_mgmr_sta_ps_enter();
    bl_pm_event_bit_clear(PSM_EVENT_PS);
}

void bl_pm_exit_ps(void)
{
    wifi_mgmr_sta_ps_exit();
    bl_pm_event_bit_set(PSM_EVENT_PS);
}

int bl_pm_wifi_config_get(bl_lp_fw_cfg_t *pcfg)
{
    int ret;
    uint8_t channel;
    #ifdef CFG_LPM
    int rssi;
    #endif

    ret = wifi_mgmr_sta_get_bssid(pcfg->bssid);
    if (ret) {
        return -1;
    }
    ret = wifi_mgmr_sta_channel_get(&channel);
    if (ret) {
        return -1;
    }
    #ifdef CFG_LPM
    ret = wifi_mgmr_sta_rssi_get(&rssi);
    if (ret) {
        return -1;
    }
    #endif

    pcfg->aid = wifi_mgmr_sta_aid_get();
    if (pcfg->aid < 0) {
        return -1;
    }

    pcfg->channel = channel;
    pcfg->pack_env = mm_env_get();
    #ifdef CFG_LPM
    pcfg->rssi = rssi;
    #endif

    return 0;
}

extern void resume_wifi_internal(int connected, uint8_t bssid[6], uint8_t mac[6]);
void bl_pm_resume_wifi(bool isr)
{
    uint8_t bssid[6];
    uint8_t mac[6];

    wifi_mgmr_sta_get_bssid(bssid);
    wifi_mgmr_sta_mac_get(mac);
    resume_wifi_internal(wifi_mgmr_sta_state_get(), bssid, mac);
    wifi_task_resume(isr);
}

void bl_pm_enter_pds(void *arg)
{
    return;
}

void bl_lp_turnoff_rf(void)
{
    PDS_Power_Off_WB();
}

void bl_lp_turnon_rf(void)
{
    PDS_Power_On_WB();
}
