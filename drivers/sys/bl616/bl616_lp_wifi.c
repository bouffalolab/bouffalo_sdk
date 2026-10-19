#include "bl_lp_internal.h"

static ATTR_NOCACHE_RAM_SECTION lp_fw_wifi_para_t wifi_param = { 0 };
static ATTR_NOCACHE_RAM_SECTION lp_fw_bcn_delay_t bcn_delay_info = { 0 };
static ATTR_NOCACHE_RAM_SECTION lp_fw_bcn_loss_t beacon_loss_info = { 0 };

void bl_lp_fw_wifi_init(void)
{
    memset(&wifi_param, 0, sizeof(wifi_param));
    iot2lp_para->wifi_parameter = &wifi_param;
    iot2lp_para->wifi_parameter->dtim_num = 10;
    iot2lp_para->wifi_parameter->beacon_interval_tu = 100;
}

void bl_lp_fw_bcn_delay_init(void)
{
    static ATTR_NOCACHE_RAM_SECTION int32_t bcn_delay_buff[16] = { 0 };

    iot2lp_para->bcn_delay_info = &bcn_delay_info;
    iot2lp_para->bcn_delay_info->bcn_delay_sliding_win_buff = bcn_delay_buff;
    iot2lp_para->bcn_delay_info->bcn_delay_sliding_win_size = sizeof(bcn_delay_buff) / sizeof(int32_t);
    iot2lp_para->bcn_delay_info->bcn_delay_sliding_win_point = 0;
    iot2lp_para->bcn_delay_info->bcn_delay_sliding_win_status = 0;
}

void bl_lp_fw_bcn_loss_init(void)
{
    memset(&beacon_loss_info, 0, sizeof(beacon_loss_info));
    iot2lp_para->bcn_loss_info = &beacon_loss_info;
    iot2lp_para->bcn_loss_info->bcn_loss_level = 0;
    iot2lp_para->bcn_loss_info->continuous_loss_cnt = 0;
    bl_lp_fw_bcn_loss_cfg_dtim_default(iot2lp_para->wifi_parameter->dtim_num);
}

void bl_lp_fw_bcn_loss_cfg_dtim_default(uint8_t dtim_num)
{
    static const lp_fw_bcn_loss_level_t dtim10_cfg_table[18] = {
        { 10, 0, 2000, 4000 },  { 8, 0, 2000, 6000 },   { 6, 0, 3000, 10000 }, { 6, 0, 4000, 10000 },
        { 3, 0, 6000, 14000 },  { 3, 1, 15000, 40000 }, { 9, 0, 2000, 5000 },  { 8, 0, 3000, 8000 },
        { 8, 0, 4000, 12000 },  { 6, 0, 5000, 12000 },  { 6, 0, 6000, 12000 }, { 6, 1, 20000, 50000 },
        { 9, 0, 2000, 4000 },   { 8, 0, 4000, 8000 },   { 8, 0, 6000, 12000 }, { 6, 0, 6000, 12000 },
        { 6, 0, 6000, 12000 },  { 6, 1, 50000, 110000 },
    };
    static const struct bl_lp_bcn_loss_cfg_desc dtim10_cfg = {
        .cfg_table = dtim10_cfg_table,
        .table_num = 18,
        .loop_start = 6,
        .loss_max = 6 + 12 * 20,
    };

    static const lp_fw_bcn_loss_level_t dtim6_cfg_table[20] = {
        { 6, 0, 2000, 4000 },   { 6, 0, 2000, 6000 },   { 5, 0, 3000, 10000 },  { 4, 0, 4000, 8000 },
        { 3, 0, 6000, 14000 },  { 3, 1, 15000, 40000 }, { 6, 0, 2000, 4000 },   { 6, 0, 3000, 8000 },
        { 5, 0, 4000, 8000 },   { 5, 0, 4000, 10000 },  { 4, 0, 3000, 10000 },  { 4, 0, 5000, 10000 },
        { 3, 1, 20000, 50000 }, { 6, 0, 2000, 4000 },   { 6, 0, 3000, 8000 },   { 5, 0, 4000, 8000 },
        { 5, 0, 4000, 10000 },  { 4, 0, 3000, 10000 },  { 4, 0, 5000, 10000 },  { 3, 1, 50000, 110000 },
    };
    static const struct bl_lp_bcn_loss_cfg_desc dtim6_cfg = {
        .cfg_table = dtim6_cfg_table,
        .table_num = 20,
        .loop_start = 6,
        .loss_max = 6 + 14 * 20,
    };

    static const lp_fw_bcn_loss_level_t dtim3_cfg_table[20] = {
        { 3, 0, 2000, 4000 },   { 3, 0, 2000, 6000 },   { 3, 0, 3000, 10000 },  { 3, 0, 4000, 8000 },
        { 2, 0, 3000, 8000 },   { 3, 0, 3000, 10000 },  { 2, 0, 6000, 14000 },  { 3, 1, 15000, 40000 },
        { 3, 0, 2000, 5000 },   { 3, 0, 3000, 6000 },   { 3, 0, 4000, 8000 },   { 3, 0, 10000, 20000 },
        { 3, 0, 3000, 6000 },   { 2, 0, 3000, 6000 },   { 3, 0, 4000, 8000 },   { 3, 0, 4000, 8000 },
        { 3, 0, 4000, 10000 },  { 2, 0, 4000, 10000 },  { 3, 0, 10000, 20000 }, { 3, 1, 50000, 110000 },
    };
    static const struct bl_lp_bcn_loss_cfg_desc dtim3_cfg = {
        .cfg_table = dtim3_cfg_table,
        .table_num = 20,
        .loop_start = 8,
        .loss_max = 8 + 12 * 20,
    };

    static const lp_fw_bcn_loss_level_t dtim1_cfg_table[20] = {
        { 1, 0, 2000, 4000 },   { 1, 0, 2000, 6000 },   { 1, 0, 3000, 10000 },  { 1, 0, 4000, 8000 },
        { 2, 0, 3000, 8000 },   { 3, 0, 3000, 8000 },   { 2, 0, 6000, 14000 },  { 1, 1, 15000, 40000 },
        { 1, 0, 2000, 4000 },   { 2, 0, 3000, 6000 },   { 3, 0, 4000, 8000 },   { 1, 0, 10000, 20000 },
        { 2, 0, 3000, 6000 },   { 3, 0, 3000, 6000 },   { 1, 0, 4000, 8000 },   { 2, 0, 4000, 8000 },
        { 3, 0, 4000, 8000 },   { 1, 0, 4000, 8000 },   { 2, 0, 10000, 20000 }, { 3, 1, 50000, 110000 },
    };
    static const struct bl_lp_bcn_loss_cfg_desc dtim1_cfg = {
        .cfg_table = dtim1_cfg_table,
        .table_num = 20,
        .loop_start = 8,
        .loss_max = 8 + 12 * 20,
    };

    bl_lp_fw_bcn_loss_cfg_dtim_default_common(dtim_num, &dtim1_cfg, &dtim3_cfg, &dtim6_cfg, &dtim10_cfg);
}

void bl_lp_fw_disconnection(void)
{
    iot2lp_para->wifi_parameter->dtim_num = 10;
    iot2lp_para->wifi_parameter->beacon_interval_tu = 100;
    iot2lp_para->rc32k_trim_parameter->rc32k_fr_ext = (*((volatile uint32_t *)0x2000F200)) >> 22;
    iot2lp_para->bcn_loss_info->bcn_loss_level = 0;
    iot2lp_para->bcn_loss_info->continuous_loss_cnt = 0;
    iot2lp_para->bcn_delay_info->bcn_delay_sliding_win_point = 0;
    iot2lp_para->bcn_delay_info->bcn_delay_sliding_win_status = 0;
    iot2lp_para->bcn_delay_info->last_beacon_delay_us = 0;
    iot2lp_para->bcn_delay_info->bcn_delay_offset = 0;
    iot2lp_para->last_beacon_stamp_rtc_valid = 0;
    iot2lp_para->rc32k_trim_parameter->last_rc32trim_stamp_valid = 0;
    iot2lp_para->rc32k_trim_parameter->rc32k_trim_ready = 0;
    iot2lp_para->rc32k_trim_parameter->last_rc32trim_stamp_rtc_us = 0;
}

int bl_lp_get_next_beacon_time(uint8_t mode)
{
    uint64_t last_beacon_rtc_us, rtc_now_us, rtc_cnt;
    uint32_t dtim_period_us, beacon_interval_us;
    int32_t next_time;

    if (iot2lp_para->last_beacon_stamp_rtc_valid == 0) {
        return -1;
    }

    beacon_interval_us = iot2lp_para->wifi_parameter->beacon_interval_tu * 1024;
    last_beacon_rtc_us = iot2lp_para->last_beacon_stamp_rtc_us;
    last_beacon_rtc_us -= iot2lp_para->bcn_delay_info->last_beacon_delay_us;
    HBN_Get_RTC_Timer_Val((uint32_t *)&rtc_cnt, (uint32_t *)&rtc_cnt + 1);
    rtc_now_us = BL_PDS_CNT_TO_US(rtc_cnt);

    if (mode) {
        dtim_period_us = beacon_interval_us;
    } else {
        if (iot2lp_para->bcn_loss_info->continuous_loss_cnt) {
            uint32_t bcn_loss_level = iot2lp_para->bcn_loss_info->bcn_loss_level;
            lp_fw_bcn_loss_level_t *bcn_loss_cfg = &(iot2lp_para->bcn_loss_info->bcn_loss_cfg_table[bcn_loss_level]);
            dtim_period_us = bcn_loss_cfg->dtim_num * beacon_interval_us;
        } else {
            dtim_period_us = iot2lp_para->wifi_parameter->dtim_num * beacon_interval_us;
        }
    }

    if (last_beacon_rtc_us + dtim_period_us >= rtc_now_us) {
        next_time = (last_beacon_rtc_us + dtim_period_us - rtc_now_us);
    } else {
        next_time = beacon_interval_us - ((rtc_now_us - last_beacon_rtc_us) % beacon_interval_us);
    }

    return next_time / 1000;
}
