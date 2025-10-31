#include "bflb_touch_v2.h"
#include "hardware/touch_v2_reg.h"

/**
 * @brief Initialize touch V2 with configuration
 */
int bflb_touch_v2_init(struct bflb_device_s *dev, const struct bflb_touch_v2_config_s *config)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    /* Configure touch_ctrl_0 register */
    regval = 0;
    regval |= TOUCH_EN;
    regval |= (config->clk_sel ? TOUCH_CLK_SEL : 0);
    regval |= (config->clk_div << TOUCH_CLK_DIV_RATIO_SHIFT) & TOUCH_CLK_DIV_RATIO_MASK;
    regval |= (config->touch_32m_clk_sel ? TOUCH_32M_CLK_SEL : 0);
    regval |= (config->scn_lst_chl_cnt << TOUCH_SCN_LST_CHL_CNT_SHIFT) & TOUCH_SCN_LST_CHL_CNT_MASK;
    regval |= (config->hys_sel ? TOUCH_HYS_SEL : 0);
    //regval |= (config->scan_mode_en ? TOUCH_SCN_EN : 0);
    regval |= (config->vldo_sel << TOUCH_VLDO_SEL_SHIFT) & TOUCH_VLDO_SEL_MASK;
    regval |= (config->vref_sel << TOUCH_VREF_SEL_SHIFT) & TOUCH_VREF_SEL_MASK;
    putreg32(regval, reg_base + TOUCH_CTRL_0_OFFSET);

    /* Configure touch_ctrl_1 register */
    regval = 0;
    regval |= (config->det_int_mask << TOUCH_DET_INT_MASK_SHIFT) & TOUCH_DET_INT_MASK_MASK;
    regval |= (config->data_hys_en ? TOUCH_DATA_HYS_EN : 0);
    regval |= (config->force_val_en ? TOUCH_FORCE_VAL_EN : 0);
    regval |= (config->channel_cal_en ? TOUCH_CHANNEL_CAL_EN : 0);
    regval |= (config->lta_order << TOUCH_LTA_ORDER_SHIFT) & TOUCH_LTA_ORDER_MASK;
    regval |= (config->lta_en ? TOUCH_LTA_EN : 0);
    regval |= (config->lta_sel ? TOUCH_LTA_SEL : 0);
    regval |= (config->filter_order << TOUCH_FLT_ORDER_SHIFT) & TOUCH_FLT_ORDER_MASK;
    regval |= (config->filter_en ? TOUCH_FLT_EN : 0);
    regval |= (config->filter_sel ? TOUCH_FLT_SEL : 0);
    regval |= (config->hop_int_mask << TOUCH_HOP_FREQ_TH_INT_MASK_SHIFT) & TOUCH_HOP_FREQ_TH_INT_MASK_MASK;
    regval |= (config->scan_int_mask ? TOUCH_CHL_SCN_LST_INT_MASK : 0);
    putreg32(regval, reg_base + TOUCH_CTRL_1_OFFSET);

    /* Configure touch_chl_cfg register */
    regval = 0;
    regval |= (config->ldo_settle_cyc << TOUCH_LDO_SETTLE_CYC_SHIFT) & TOUCH_LDO_SETTLE_CYC_MASK;
    regval |= (config->cont_mode_en ? TOUCH_CONT_MODE_EN : 0);
    regval |= (config->wakeup_mode_en ? TOUCH_ULP2_EN : 0);
    regval |= (config->as_chl_sel ? TOUCH_AS_CHL_SEL : 0);
    regval |= (config->as_channel_sel << TOUCH_AS_CH_SEL_SHIFT) & TOUCH_AS_CH_SEL_MASK;
    regval |= (config->rx_channel_en << TOUCH_CH_RX_EN_SHIFT) & TOUCH_CH_RX_EN_MASK;
    regval |= (config->tx_channel_en << TOUCH_CH_TX_EN_SHIFT) & TOUCH_CH_TX_EN_MASK;
    putreg32(regval, reg_base + TOUCH_CHL_CFG_OFFSET);

    /* Configure touch_ctrl_2 register */
    regval = 0;
    regval |= (config->sleep_cycle << TOUCH_SLEEP_CYCLE_SHIFT) & TOUCH_SLEEP_CYCLE_MASK;
    regval |= (config->pcharge_low << TOUCH_PCHARGE_LOW_SHIFT) & TOUCH_PCHARGE_LOW_MASK;
    regval |= (config->pcharge_high << TOUCH_PCHARGE_HIGH_SHIFT) & TOUCH_PCHARGE_HIGH_MASK;
    regval |= (config->det_keep ? TOUCH_DET_KEEP : 0);
    regval |= (config->det_on ? TOUCH_DET_TOUCH_ON : 0);
    putreg32(regval, reg_base + TOUCH_CTRL_2_OFFSET);

    /* Configure frequency hopping if enabled */
    if (config->hop_freq_en) {
        bflb_touch_v2_config_freq_hopping(dev, true, config->hop_freq_th,
                                          config->hop_freq_div0, config->hop_freq_div1, config->hop_freq_div2);
    }

    /* Set detection duration */
    bflb_touch_v2_set_detection_duration(dev, config->det_dur);

    return 0;
}

/**
 * @brief Deinitialize touch V2
 */
void bflb_touch_v2_deinit(struct bflb_device_s *dev)
{
    uint32_t reg_base;

    LHAL_PARAM_ASSERT(dev);

    reg_base = dev->reg_base;

    /* Perform software reset */
    bflb_touch_v2_reset(dev);

    /* Disable touch module */
    bflb_touch_v2_disable(dev);

    /* Clear all registers */
    putreg32(0, reg_base + TOUCH_CTRL_0_OFFSET);
    putreg32(0, reg_base + TOUCH_CTRL_1_OFFSET);
    putreg32(0, reg_base + TOUCH_CHL_CFG_OFFSET);
    putreg32(0, reg_base + TOUCH_CTRL_2_OFFSET);
    putreg32(0, reg_base + TOUCH_HOP_FREQ_CFG_OFFSET);
    putreg32(0, reg_base + TOUCH_DET_DUR_OFFSET);
}

/**
 * @brief Enable touch V2 scan
 */
void bflb_touch_v2_scan_start(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TOUCH_CTRL_0_OFFSET);
    regval &= ~TOUCH_SCN_EN;
    putreg32(regval, reg_base + TOUCH_CTRL_0_OFFSET);

    arch_delay_us(2);

    regval = getreg32(reg_base + TOUCH_CTRL_0_OFFSET);
    regval |= TOUCH_SCN_EN;
    putreg32(regval, reg_base + TOUCH_CTRL_0_OFFSET);
}

/**
 * @brief Disable touch V2 scan
 */
void bflb_touch_v2_scan_stop(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TOUCH_CTRL_0_OFFSET);
    regval &= ~TOUCH_SCN_EN;
    putreg32(regval, reg_base + TOUCH_CTRL_0_OFFSET);
}
/**
 * @brief Perform software reset
 */
void bflb_touch_v2_reset(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TOUCH_CTRL_1_OFFSET);
    regval |= TOUCH_SWRST;
    putreg32(regval, reg_base + TOUCH_CTRL_1_OFFSET);

    /* Wait a bit for reset to complete */
    bflb_mtimer_delay_us(10);

    regval &= ~TOUCH_SWRST;
    putreg32(regval, reg_base + TOUCH_CTRL_1_OFFSET);
}

/**
 * @brief Set channel threshold data
 */
void bflb_touch_v2_set_channel_threshold(struct bflb_device_s *dev, uint8_t channel, uint8_t threshold)
{
    uint32_t reg_base;
    uint32_t regval;
    uint32_t reg_offset;
    uint32_t shift;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_TOUCH_CHANNEL_TYPE(channel));

    reg_base = dev->reg_base;

    /* Calculate register offset and bit shift based on channel */
    if (channel < 4) {
        reg_offset = TOUCH_VTH_DAT_0_3_OFFSET;
        shift = (channel % 4) * 8;
    } else if (channel < 8) {
        reg_offset = TOUCH_VTH_DAT_4_7_OFFSET;
        shift = (channel % 4) * 8;
    } else if (channel < 12) {
        reg_offset = TOUCH_VTH_DAT_8_B_OFFSET;
        shift = (channel % 4) * 8;
    } else {
        reg_offset = TOUCH_VTH_DAT_C_F_OFFSET;
        shift = (channel % 4) * 8;
    }

    regval = getreg32(reg_base + reg_offset);
    regval &= ~(0xFF << shift);
    regval |= (threshold << shift);
    putreg32(regval, reg_base + reg_offset);
}

/**
 * @brief Set channel hysteresis data
 */
void bflb_touch_v2_set_channel_hysteresis(struct bflb_device_s *dev, uint8_t channel, uint8_t hys)
{
    uint32_t reg_base;
    uint32_t regval;
    uint32_t reg_offset;
    uint32_t shift;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_TOUCH_CHANNEL_TYPE(channel));
    LHAL_PARAM_ASSERT(hys <= 0xF);

    reg_base = dev->reg_base;

    /* Calculate register offset and bit shift based on channel */
    if (channel < 8) {
        reg_offset = TOUCH_HYS_DAT_0_7_OFFSET;
        shift = (channel % 8) * 4;
    } else {
        reg_offset = TOUCH_HYS_DAT_8_F_OFFSET;
        shift = (channel % 8) * 4;
    }

    regval = getreg32(reg_base + reg_offset);
    regval &= ~(0xF << shift);
    regval |= (hys << shift);
    putreg32(regval, reg_base + reg_offset);
}

/**
 * @brief Set channel force data
 */
void bflb_touch_v2_set_channel_force_data(struct bflb_device_s *dev, uint8_t channel, uint16_t force_data)
{
    uint32_t reg_base;
    uint32_t regval;
    uint32_t reg_offset;
    uint32_t shift;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_TOUCH_CHANNEL_TYPE(channel));

    reg_base = dev->reg_base;

    /* Calculate register offset and bit shift based on channel */
    if (channel < 2) {
        reg_offset = TOUCH_FORCE_DAT_01_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 4) {
        reg_offset = TOUCH_FORCE_DAT_23_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 6) {
        reg_offset = TOUCH_FORCE_DAT_45_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 8) {
        reg_offset = TOUCH_FORCE_DAT_67_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 10) {
        reg_offset = TOUCH_FORCE_DAT_89_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 12) {
        reg_offset = TOUCH_FORCE_DAT_AB_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 14) {
        reg_offset = TOUCH_FORCE_DAT_CD_OFFSET;
        shift = (channel % 2) * 16;
    } else {
        reg_offset = TOUCH_FORCE_DAT_EF_OFFSET;
        shift = (channel % 2) * 16;
    }

    regval = getreg32(reg_base + reg_offset);
    regval &= ~(0xFFFF << shift);
    regval |= (force_data << shift);
    putreg32(regval, reg_base + reg_offset);
}

/**
 * @brief Latch force data to force_data registers
 */
void bflb_touch_v2_latch_force_data(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TOUCH_CTRL_1_OFFSET);
    regval |= TOUCH_FORCE_VAL_EN;
    putreg32(regval, reg_base + TOUCH_CTRL_1_OFFSET);

    regval = getreg32(reg_base + TOUCH_CTRL_0_OFFSET);
    if (regval & TOUCH_CLK_SEL) {
        arch_delay_us(2);
    } else {
        arch_delay_us(32 * 4);
    }

    regval = getreg32(reg_base + TOUCH_CTRL_1_OFFSET);
    regval &= (~TOUCH_FORCE_VAL_EN);
    putreg32(regval, reg_base + TOUCH_CTRL_1_OFFSET);
}

/**
 * @brief Get channel data
 */
void bflb_touch_v2_get_channel_data(struct bflb_device_s *dev, uint8_t channel, struct bflb_touch_v2_channel_data_s *data)
{
    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_TOUCH_CHANNEL_TYPE(channel));
    LHAL_PARAM_ASSERT(data);

    data->raw_data = bflb_touch_v2_get_raw_data(dev, channel);
    data->flt_data = bflb_touch_v2_get_flt_data(dev, channel);
    data->lta_data = bflb_touch_v2_get_lta_data(dev, channel);

    /* Get force data, threshold and hysteresis from corresponding registers */
    uint32_t reg_base = dev->reg_base;
    uint32_t regval;

    /* Get force data */
    uint32_t force_reg_offset, force_shift;
    if (channel < 2) {
        force_reg_offset = TOUCH_FORCE_DAT_01_OFFSET;
        force_shift = (channel % 2) * 16;
    } else if (channel < 4) {
        force_reg_offset = TOUCH_FORCE_DAT_23_OFFSET;
        force_shift = (channel % 2) * 16;
    } else if (channel < 6) {
        force_reg_offset = TOUCH_FORCE_DAT_45_OFFSET;
        force_shift = (channel % 2) * 16;
    } else if (channel < 8) {
        force_reg_offset = TOUCH_FORCE_DAT_67_OFFSET;
        force_shift = (channel % 2) * 16;
    } else if (channel < 10) {
        force_reg_offset = TOUCH_FORCE_DAT_89_OFFSET;
        force_shift = (channel % 2) * 16;
    } else if (channel < 12) {
        force_reg_offset = TOUCH_FORCE_DAT_AB_OFFSET;
        force_shift = (channel % 2) * 16;
    } else if (channel < 14) {
        force_reg_offset = TOUCH_FORCE_DAT_CD_OFFSET;
        force_shift = (channel % 2) * 16;
    } else {
        force_reg_offset = TOUCH_FORCE_DAT_EF_OFFSET;
        force_shift = (channel % 2) * 16;
    }
    regval = getreg32(reg_base + force_reg_offset);
    data->force_data = (regval >> force_shift) & 0xFFFF;

    /* Get threshold data */
    uint32_t vth_reg_offset, vth_shift;
    if (channel < 4) {
        vth_reg_offset = TOUCH_VTH_DAT_0_3_OFFSET;
        vth_shift = (channel % 4) * 8;
    } else if (channel < 8) {
        vth_reg_offset = TOUCH_VTH_DAT_4_7_OFFSET;
        vth_shift = (channel % 4) * 8;
    } else if (channel < 12) {
        vth_reg_offset = TOUCH_VTH_DAT_8_B_OFFSET;
        vth_shift = (channel % 4) * 8;
    } else {
        vth_reg_offset = TOUCH_VTH_DAT_C_F_OFFSET;
        vth_shift = (channel % 4) * 8;
    }
    regval = getreg32(reg_base + vth_reg_offset);
    data->vth_data = (regval >> vth_shift) & 0xFF;

    /* Get hysteresis data */
    uint32_t hys_reg_offset, hys_shift;
    if (channel < 8) {
        hys_reg_offset = TOUCH_HYS_DAT_0_7_OFFSET;
        hys_shift = (channel % 8) * 4;
    } else {
        hys_reg_offset = TOUCH_HYS_DAT_8_F_OFFSET;
        hys_shift = (channel % 8) * 4;
    }
    regval = getreg32(reg_base + hys_reg_offset);
    data->hys_data = (regval >> hys_shift) & 0xF;
}

/**
 * @brief Get raw data for a specific channel
 */
uint16_t bflb_touch_v2_get_raw_data(struct bflb_device_s *dev, uint8_t channel)
{
    uint32_t reg_base;
    uint32_t regval;
    uint32_t reg_offset;
    uint32_t shift;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_TOUCH_CHANNEL_TYPE(channel));

    reg_base = dev->reg_base;

    /* Calculate register offset and bit shift based on channel */
    if (channel < 2) {
        reg_offset = TOUCH_RAW_DAT_01_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 4) {
        reg_offset = TOUCH_RAW_DAT_23_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 6) {
        reg_offset = TOUCH_RAW_DAT_45_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 8) {
        reg_offset = TOUCH_RAW_DAT_67_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 10) {
        reg_offset = TOUCH_RAW_DAT_89_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 12) {
        reg_offset = TOUCH_RAW_DAT_AB_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 14) {
        reg_offset = TOUCH_RAW_DAT_CD_OFFSET;
        shift = (channel % 2) * 16;
    } else {
        reg_offset = TOUCH_RAW_DAT_EF_OFFSET;
        shift = (channel % 2) * 16;
    }

    regval = getreg32(reg_base + reg_offset);
    return (regval >> shift) & 0xFFFF;
}

/**
 * @brief Get filtered data for a specific channel
 */
uint16_t bflb_touch_v2_get_flt_data(struct bflb_device_s *dev, uint8_t channel)
{
    uint32_t reg_base;
    uint32_t regval;
    uint32_t reg_offset;
    uint32_t shift;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_TOUCH_CHANNEL_TYPE(channel));

    reg_base = dev->reg_base;

    /* Calculate register offset and bit shift based on channel */
    if (channel < 2) {
        reg_offset = TOUCH_FLT_DAT_01_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 4) {
        reg_offset = TOUCH_FLT_DAT_23_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 6) {
        reg_offset = TOUCH_FLT_DAT_45_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 8) {
        reg_offset = TOUCH_FLT_DAT_67_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 10) {
        reg_offset = TOUCH_FLT_DAT_89_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 12) {
        reg_offset = TOUCH_FLT_DAT_AB_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 14) {
        reg_offset = TOUCH_FLT_DAT_CD_OFFSET;
        shift = (channel % 2) * 16;
    } else {
        reg_offset = TOUCH_FLT_DAT_EF_OFFSET;
        shift = (channel % 2) * 16;
    }

    regval = getreg32(reg_base + reg_offset);
    return (regval >> shift) & 0xFFFF;
}

/**
 * @brief Get LTA data for a specific channel
 */
uint16_t bflb_touch_v2_get_lta_data(struct bflb_device_s *dev, uint8_t channel)
{
    uint32_t reg_base;
    uint32_t regval;
    uint32_t reg_offset;
    uint32_t shift;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_TOUCH_CHANNEL_TYPE(channel));

    reg_base = dev->reg_base;

    /* Calculate register offset and bit shift based on channel */
    if (channel < 2) {
        reg_offset = TOUCH_LTA_DAT_01_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 4) {
        reg_offset = TOUCH_LTA_DAT_23_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 6) {
        reg_offset = TOUCH_LTA_DAT_45_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 8) {
        reg_offset = TOUCH_LTA_DAT_67_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 10) {
        reg_offset = TOUCH_LTA_DAT_89_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 12) {
        reg_offset = TOUCH_LTA_DAT_AB_OFFSET;
        shift = (channel % 2) * 16;
    } else if (channel < 14) {
        reg_offset = TOUCH_LTA_DAT_CD_OFFSET;
        shift = (channel % 2) * 16;
    } else {
        reg_offset = TOUCH_LTA_DAT_EF_OFFSET;
        shift = (channel % 2) * 16;
    }

    regval = getreg32(reg_base + reg_offset);
    return (regval >> shift) & 0xFFFF;
}

/**
 * @brief Enable/disable channel TX
 */
void bflb_touch_v2_set_tx_channel(struct bflb_device_s *dev, uint16_t channels)
{
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TOUCH_CHL_CFG_OFFSET);
    regval &= ~TOUCH_CH_TX_EN_MASK;
    regval |= (channels << TOUCH_CH_TX_EN_SHIFT) & TOUCH_CH_TX_EN_MASK;
    putreg32(regval, reg_base + TOUCH_CHL_CFG_OFFSET);
}

/**
 * @brief Enable/disable channel RX
 */
void bflb_touch_v2_set_rx_channel(struct bflb_device_s *dev, uint16_t channels)
{
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TOUCH_CHL_CFG_OFFSET);
    regval &= ~TOUCH_CH_RX_EN_MASK;
    regval |= (channels << TOUCH_CH_RX_EN_SHIFT) & TOUCH_CH_RX_EN_MASK;
    putreg32(regval, reg_base + TOUCH_CHL_CFG_OFFSET);
}

/**
 * @brief Get interrupt status
 */
uint32_t bflb_touch_v2_get_int_status(struct bflb_device_s *dev)
{
    uint32_t reg_base;

    LHAL_PARAM_ASSERT(dev);

    reg_base = dev->reg_base;

    return getreg32(reg_base + TOUCH_INT_STS_OFFSET);
}

/**
 * @brief Clear interrupt status
 */
void bflb_touch_v2_clear_int(struct bflb_device_s *dev, uint32_t int_mask)
{
    uint32_t reg_base;

    LHAL_PARAM_ASSERT(dev);

    reg_base = dev->reg_base;

    putreg32(int_mask, reg_base + TOUCH_INT_CLR_OFFSET);
}

/**
 * @brief Set interrupt mask
 */
void bflb_touch_v2_set_int_mask(struct bflb_device_s *dev, uint32_t int_mask)
{
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TOUCH_CTRL_1_OFFSET);
    regval &= ~(TOUCH_DET_INT_MASK_MASK | TOUCH_CHL_SCN_LST_INT_MASK | TOUCH_HOP_FREQ_TH_INT_MASK_MASK);
    regval |= int_mask;
    putreg32(regval, reg_base + TOUCH_CTRL_1_OFFSET);
}

/**
 * @brief Configure frequency hopping
 */
void bflb_touch_v2_config_freq_hopping(struct bflb_device_s *dev, bool enable, uint8_t threshold,
                                       uint8_t div0, uint8_t div1, uint8_t div2)
{
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(div0 <= 0xF);
    LHAL_PARAM_ASSERT(div1 <= 0xF);
    LHAL_PARAM_ASSERT(div2 <= 0xF);

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TOUCH_HOP_FREQ_CFG_OFFSET);
    regval &= ~(TOUCH_HOP_FREQ_EN | TOUCH_HOP_FREQ_TH_MASK |
                TOUCH_HOP_FREQ_DIV0_MASK | TOUCH_HOP_FREQ_DIV1_MASK | TOUCH_HOP_FREQ_DIV2_MASK);

    if (enable) {
        regval |= TOUCH_HOP_FREQ_EN;
        regval |= (threshold << TOUCH_HOP_FREQ_TH_SHIFT) & TOUCH_HOP_FREQ_TH_MASK;
        regval |= (div0 << TOUCH_HOP_FREQ_DIV0_SHIFT) & TOUCH_HOP_FREQ_DIV0_MASK;
        regval |= (div1 << TOUCH_HOP_FREQ_DIV1_SHIFT) & TOUCH_HOP_FREQ_DIV1_MASK;
        regval |= (div2 << TOUCH_HOP_FREQ_DIV2_SHIFT) & TOUCH_HOP_FREQ_DIV2_MASK;
    }

    putreg32(regval, reg_base + TOUCH_HOP_FREQ_CFG_OFFSET);
}

/**
 * @brief Set touch detection duration
 */
void bflb_touch_v2_set_detection_duration(struct bflb_device_s *dev, uint16_t duration)
{
    uint32_t reg_base;

    LHAL_PARAM_ASSERT(dev);

    reg_base = dev->reg_base;

    putreg32(duration & TOUCH_DET_DUR_MASK, reg_base + TOUCH_DET_DUR_OFFSET);
}

/**
 * @brief Configure sleep cycle for non-continuous mode
 */
void bflb_touch_v2_set_sleep_cycle(struct bflb_device_s *dev, uint32_t sleep_cycle)
{
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(sleep_cycle <= 0x7FFFFF);

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TOUCH_CTRL_2_OFFSET);
    regval &= ~TOUCH_SLEEP_CYCLE_MASK;
    regval |= (sleep_cycle << TOUCH_SLEEP_CYCLE_SHIFT) & TOUCH_SLEEP_CYCLE_MASK;
    putreg32(regval, reg_base + TOUCH_CTRL_2_OFFSET);
}
