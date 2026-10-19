#include "bl_lp_internal.h"

static ATTR_NOCACHE_NOINIT_RAM_SECTION lp_fw_bcn_loss_level_t bcn_loss_cfg_buff[20];

void bl_lp_fw_bcn_loss_cfg(lp_fw_bcn_loss_level_t *cfg_table, uint16_t table_num, uint16_t loop_start,
                           uint16_t loss_max)
{
    if (cfg_table == NULL || table_num == 0) {
        return;
    }

    if (loop_start >= table_num) {
        loop_start = table_num;
    }

    bflb_l1c_dcache_clean_range(cfg_table, sizeof(lp_fw_bcn_loss_level_t) * table_num);
    cfg_table = (void *)(((uint32_t)cfg_table) & 0x2fffffff);

    iot2lp_para->bcn_loss_info->continuous_loss_cnt_max = loss_max;
    iot2lp_para->bcn_loss_info->bcn_loss_cfg_table = cfg_table;
    iot2lp_para->bcn_loss_info->bcn_loss_loop_start = loop_start;
    iot2lp_para->bcn_loss_info->bcn_loss_level_max = table_num;
    iot2lp_para->bcn_loss_info->bcn_loss_level = 0;
    iot2lp_para->bcn_loss_info->continuous_loss_cnt = 0;
}

void bl_lp_fw_bcn_loss_cfg_dtim_default_common(uint8_t dtim_num, const struct bl_lp_bcn_loss_cfg_desc *dtim1_cfg,
                                               const struct bl_lp_bcn_loss_cfg_desc *dtim3_cfg,
                                               const struct bl_lp_bcn_loss_cfg_desc *dtim6_cfg,
                                               const struct bl_lp_bcn_loss_cfg_desc *dtim10_cfg)
{
    const struct bl_lp_bcn_loss_cfg_desc *cfg;

    if (dtim_num == 0) {
        dtim_num = 10;
    }

    if (dtim_num < 3) {
        cfg = dtim1_cfg;
        BL_LP_LOG("bcn_loss_cfg: dtim-1\r\n");
    } else if (dtim_num < 6) {
        cfg = dtim3_cfg;
        BL_LP_LOG("bcn_loss_cfg: dtim-3\r\n");
    } else if (dtim_num < 9) {
        cfg = dtim6_cfg;
        BL_LP_LOG("bcn_loss_cfg: dtim-6\r\n");
    } else {
        cfg = dtim10_cfg;
        BL_LP_LOG("bcn_loss_cfg: dtim-10\r\n");
    }

    if (cfg == NULL || cfg->cfg_table == NULL || cfg->table_num <= 0) {
        return;
    }

    memcpy(bcn_loss_cfg_buff, cfg->cfg_table, sizeof(lp_fw_bcn_loss_level_t) * cfg->table_num);
    bl_lp_fw_bcn_loss_cfg(bcn_loss_cfg_buff, cfg->table_num, cfg->loop_start, cfg->loss_max);
}

int bl_lp_get_bcn_delay_ready(void)
{
    if (iot2lp_para->bcn_delay_info->bcn_delay_sliding_win_status <
        iot2lp_para->bcn_delay_info->bcn_delay_sliding_win_size) {
        return 0;
    }

    return 1;
}

ATTR_TCM_SECTION void bl_lp_bcn_loss_cnt_clear(void)
{
    if (!iot2lp_para->bcn_loss_info) {
        return;
    }

    iot2lp_para->bcn_loss_info->continuous_loss_cnt = 0;
    iot2lp_para->bcn_loss_info->bcn_loss_level = 0;
}

ATTR_TCM_SECTION void bl_lp_bcn_timestamp_update(uint64_t beacon_timestamp_us, uint64_t rtc_timestamp_us, uint32_t mode)
{
    iot2lp_para->last_beacon_stamp_rtc_valid = mode;
    iot2lp_para->last_beacon_stamp_rtc_us = rtc_timestamp_us;
    iot2lp_para->last_beacon_stamp_beacon_us = beacon_timestamp_us;
}

ATTR_TCM_SECTION uint32_t bl_lp_get_beacon_interval_tu(void)
{
    return iot2lp_para->wifi_parameter->beacon_interval_tu;
}

ATTR_TCM_SECTION uint32_t bl_lp_get_dtim_num(void)
{
    return iot2lp_para->wifi_parameter->dtim_num;
}
