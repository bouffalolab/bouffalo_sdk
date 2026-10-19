#include "bl_lp_internal.h"

#include "bflb_common.h"

int wifi_mgmr_sta_get_bssid(uint8_t bssid[6]);
int wifi_mgmr_sta_mac_get(uint8_t mac[6]);
int wifi_mgmr_sta_rssi_get(int *rssi);
int wifi_mgmr_sta_channel_get(uint8_t *channel);
int wifi_mgmr_sta_aid_get(void);
int wifi_mgmr_sta_state_get(void);
int wifi_mgmr_sta_ps_enter(void);
int wifi_mgmr_sta_ps_exit(void);
uint32_t mm_env_get(void);
void wifi_task_resume(bool isr);

static uint32_t ps_mamager_event = 0;

void bl_lp_turnoff_rf(void)
{
    PDS_Power_Off_WB();
}

void bl_lp_turnon_rf(void)
{
    PDS_Power_On_WB();
}

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

    ret = wifi_mgmr_sta_mac_get(pcfg->mac);
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

// #if defined(BL616)
int bl_lp_beacon_interval_update(uint16_t beacon_interval_tu)
{
    if (beacon_interval_tu < 20 || beacon_interval_tu > 1000) {
        return -1;
    }

    if (beacon_interval_tu == iot2lp_para->wifi_parameter->beacon_interval_tu) {
        return 0;
    }

    iot2lp_para->wifi_parameter->beacon_interval_tu = beacon_interval_tu;

    return 0;
}

int bl_lp_beacon_tim_update(uint8_t *tim, uint8_t mode)
{
    uint8_t tim_id = tim[0];
    uint8_t tim_len = tim[1];
    uint8_t dtim_count = tim[2];
    uint8_t dtim_period = tim[3];

    if (tim_id != 5 || tim_len < 4) {
        return -1;
    }

    if (dtim_count >= dtim_period) {
        return -1;
    }

    if (mode == BEACON_STAMP_LPFW) {
        if ((iot2lp_para->wifi_parameter->beacon_dtim_period != dtim_period) &&
            iot2lp_para->wifi_parameter->beacon_dtim_period) {
            return -1;
        }
    } else {
        iot2lp_para->wifi_parameter->beacon_dtim_period = dtim_period;
    }

    iot2lp_para->wifi_parameter->last_beacon_dtim_count = dtim_count;

    return 0;
}

