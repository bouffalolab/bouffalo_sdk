#include "bflb_touch.h"
#include "hardware/touch_reg.h"

int bflb_touch_init(struct bflb_device_s *dev, const struct bflb_touch_config_s *config)
{
#ifdef romapi_bflb_touch_init
    romapi_bflb_touch_init(dev, config);
#else
    uint32_t reg_base;
    uint32_t regval;
    uint32_t clock = 0;
    uint32_t ratio = 0;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_TOUCH_CLK_SEL_TYPE(config->clk_sel));
    LHAL_PARAM_ASSERT(IS_TOUCH_CLK_DIV_TYPE(config->clk_div));
    LHAL_PARAM_ASSERT(IS_TOUCH_CUR_SEL_TYPE(config->cur_sel));
    LHAL_PARAM_ASSERT(IS_TOUCH_WORK_MODE_TYPE(config->work_mode));
    LHAL_PARAM_ASSERT(IS_TOUCH_RUN_MODE_TYPE(config->run_mode));
    LHAL_PARAM_ASSERT(IS_TOUCH_PCHARGE_LOW_TYPE(config->pcharge_low));
    LHAL_PARAM_ASSERT(IS_TOUCH_PCHARGE_HIGH_TYPE(config->pcharge_high));
    LHAL_PARAM_ASSERT(IS_TOUCH_LDO_VOLTAGE_TYPE(config->ldo_volt));
    LHAL_PARAM_ASSERT(IS_TOUCH_VREF_VOLTAGE_TYPE(config->vref_volt));
    LHAL_PARAM_ASSERT(IS_TOUCH_HYSTERSIS_VOLTAGE_TYPE(config->hystersis_volt));
    LHAL_PARAM_ASSERT(IS_TOUCH_COMPENSATION_CAP_TYPE(config->compensation_cap));
    LHAL_PARAM_ASSERT(IS_TOUCH_FILTER_ORDER_CTRL_TYPE(config->filter_ctrl));
    LHAL_PARAM_ASSERT(IS_TOUCH_LTA_ORDER_CTRL_TYPE(config->lta_ctrl));
    LHAL_PARAM_ASSERT((config->data_hys_ctrl <= 0x1FF));

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TOUCH1_OFFSET);

    if (config->cur_sel == TOUCH_CUR_SEL_AON_BG) {
        /* power from AON bandgap */
        regval &= ~TOUCH_CURRENT_SEL;
    } else {
        /* power from internal bandgap */
        regval |= TOUCH_CURRENT_SEL;
    }

    if (config->ulp_en) {
        regval |= TOUCH_ULP_EN;
    } else {
        regval &= ~TOUCH_ULP_EN;
    }

    if (config->cycle_en) {
        regval |= TOUCH_CYCLE_EN;
    } else {
        regval &= ~TOUCH_CYCLE_EN;
    }

    if (config->run_mode == TOUCH_RUN_MODE_CONTINUE) {
        regval |= TOUCH_CONT_EN;
    } else {
        regval &= ~TOUCH_CONT_EN;
    }

    regval &= ~TOUCH_PCHARGE_LOW_MASK;
    regval |= config->pcharge_low << TOUCH_PCHARGE_LOW_SHIFT;

    regval &= ~TOUCH_PCHARGE_HIGH_MASK;
    regval |= config->pcharge_high << TOUCH_PCHARGE_HIGH_SHIFT;

    if (config->clk_sel == TOUCH_CLK_SEL_32M) {
        regval |= TOUCH_CLK_SEL;
        clock = 32000;
        ratio = 1;
    } else {
        regval &= ~TOUCH_CLK_SEL;
        clock = 32000;
        ratio = 1000;
    }

    regval &= ~TOUCH_CLK_DIV_RATIO_MASK;
    regval |= config->clk_div << TOUCH_CLK_DIV_RATIO_SHIFT;

    clock = clock >> config->clk_div;

    if (config->hystersis_volt == TOUCH_HYSTERSIS_VOLTAGE_80MV) {
        regval |= TOUCH_COMP_HYS_SEL;
    } else {
        regval &= ~TOUCH_COMP_HYS_SEL;
    }

    regval &= ~TOUCH_VLDO_SEL_MASK;
    regval |= config->ldo_volt << TOUCH_VLDO_SEL_SHIFT;

    regval &= ~TOUCH_VREF_SEL_MASK;
    regval |= config->vref_volt << TOUCH_VREF_SEL_SHIFT;

    putreg32(regval, reg_base + TOUCH1_OFFSET);

    /* touch3 */

    regval = getreg32(reg_base + TOUCH3_OFFSET);

    regval &= ~TOUCH_VLDO_CCSEL_MASK;
    regval |= config->compensation_cap << TOUCH_VLDO_CCSEL_SHIFT;

    if (config->work_mode == TOUCH_WORK_MODE_MUTUAL) {
        regval |= TOUCH_SELF_MUTUAL_SEL;
    } else {
        regval &= ~TOUCH_SELF_MUTUAL_SEL;
    }

    regval &= ~TOUCH_FLT_ORDER_MASK;
    regval |= config->filter_ctrl << TOUCH_FLT_ORDER_SHIFT;

    if (config->filter_en) {
        regval |= TOUCH_FLT_EN;
    } else {
        regval &= ~TOUCH_FLT_EN;
    }

    regval &= ~TOUCH_LTA_ORDER_MASK;
    regval |= config->filter_ctrl << TOUCH_LTA_ORDER_SHIFT;

    if (config->lta_en) {
        regval |= TOUCH_LTA_EN;
    } else {
        regval &= ~TOUCH_LTA_EN;
    }

    if (config->data_hystersis_en) {
        regval |= TOUCH_DATA_HYS_EN;
    } else {
        regval &= ~TOUCH_DATA_HYS_EN;
    }

    if (config->cal_en) {
        regval |= TOUCH_CHANNEL_CAL_EN;
    } else {
        regval &= ~TOUCH_CHANNEL_CAL_EN;
    }

    putreg32(regval, reg_base + TOUCH3_OFFSET);

    /* touch_sleep_time_ms */
    regval = getreg32(reg_base + TOUCH_SLEEP_TIME_OFFSET);
    regval &= ~TOUCH_SLEEP_CYCLE_MASK;
    /* use ratio just to avoid overflow */
    regval |= (config->sleep_time_ms * clock / ratio) << TOUCH_SLEEP_CYCLE_SHIFT;
    putreg32(regval, reg_base + TOUCH_SLEEP_TIME_OFFSET);

    /* touch_data_hystersis */
    regval = getreg32(reg_base + TOUCH_DATA_HYSTERSIS_OFFSET);
    regval &= ~TOUCH_DATA_HYS_MASK;
    regval |= config->data_hys_ctrl << TOUCH_DATA_HYS_SHIFT;
    putreg32(regval, reg_base + TOUCH_DATA_HYSTERSIS_OFFSET);

    return 0;
#endif
}

int bflb_touch_channel_init(struct bflb_device_s *dev, const struct bflb_touch_chan_config_s *config, uint8_t lp_chan_sel)
{
#ifdef romapi_bflb_touch_channel_init
    romapi_bflb_touch_channel_init(dev, config, chan_sel);
#else
    uint32_t reg_base;
    uint32_t regval;
    int32_t i;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_TOUCH_CHANNEL_TYPE(chan_sel));

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TOUCH2_OFFSET);

    for (i = 0; i < TOUCH_CHANNEL_MAX; i++) {
        if (config->ch[i] == TOUCH_CHANNEL_HIGHZ_CFG_GND) {
            regval &= ~(1 << (i + 4));
        } else {
            regval |= (1 << (i + 4));
        }
    }

    regval &= ~TOUCH_CHANNEL_SEL_MASK;
    regval |= lp_chan_sel << TOUCH_CHANNEL_SEL_SHIFT;

    putreg32(regval, reg_base + TOUCH2_OFFSET);

    return 0;
#endif
}

void bflb_touch_enable(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_touch_enable
    romapi_bflb_touch_enable(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TOUCH1_OFFSET);
    regval |= TOUCH_PU_TOUCH;
    putreg32(regval, reg_base + TOUCH1_OFFSET);
#endif
}

void bflb_touch_disable(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_touch_disable
    romapi_bflb_touch_disable(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TOUCH1_OFFSET);
    regval &= ~TOUCH_PU_TOUCH;
    putreg32(regval, reg_base + TOUCH1_OFFSET);
#endif
}

void bflb_touch_set_vth_data(struct bflb_device_s *dev, uint8_t channel, uint32_t vth)
{
#ifdef romapi_bflb_touch_set_vth_data
    romapi_bflb_touch_set_vth_data(dev, channel, vth);
#else
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_TOUCH_CHANNEL_TYPE(channel));

    reg_base = dev->reg_base;

    if (channel < 4) {
        regval = getreg32(reg_base + TOUCH_CHANNEL_VTH_DATA_0_OFFSET);
        regval &= ~(0xFF << (channel * 8));
        regval |= (vth & 0xFF) << (channel * 8);
        putreg32(regval, reg_base + TOUCH_CHANNEL_VTH_DATA_0_OFFSET);
    } else if (channel < 8) {
        channel = channel - 4;
        regval = getreg32(reg_base + TOUCH_CHANNEL_VTH_DATA_1_OFFSET);
        regval &= ~(0xFF << (channel * 8));
        regval |= (vth & 0xFF) << (channel * 8);
        putreg32(regval, reg_base + TOUCH_CHANNEL_VTH_DATA_1_OFFSET);
    } else if (channel < 12) {
        channel = channel - 8;
        regval = getreg32(reg_base + TOUCH_CHANNEL_VTH_DATA_2_OFFSET);
        regval &= ~(0xFF << (channel * 8));
        regval |= (vth & 0xFF) << (channel * 8);
        putreg32(regval, reg_base + TOUCH_CHANNEL_VTH_DATA_2_OFFSET);
    }
#endif
}

uint32_t bflb_touch_get_vth_data(struct bflb_device_s *dev, uint8_t channel)
{
#ifdef romapi_bflb_touch_get_vth_data
    return romapi_bflb_touch_get_vth_data(dev, channel);
#else
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_TOUCH_CHANNEL_TYPE(channel));

    reg_base = dev->reg_base;

    if (channel < 4) {
        regval = getreg32(reg_base + TOUCH_CHANNEL_VTH_DATA_0_OFFSET);
        return ((regval >> (channel * 8)) & 0xFF);
    } else if (channel < 8) {
        channel = channel - 4;
        regval = getreg32(reg_base + TOUCH_CHANNEL_VTH_DATA_1_OFFSET);
        return ((regval >> (channel * 8)) & 0xFF);
    } else if (channel < 12) {
        channel = channel - 8;
        regval = getreg32(reg_base + TOUCH_CHANNEL_VTH_DATA_2_OFFSET);
        return ((regval >> (channel * 8)) & 0xFF);
    }
    return 0;
#endif
}

uint32_t bflb_touch_get_raw_data(struct bflb_device_s *dev, uint8_t channel)
{
#ifdef romapi_bflb_touch_get_raw_data
    return romapi_bflb_touch_get_raw_data(dev, channel);
#else
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_TOUCH_CHANNEL_TYPE(channel));

    reg_base = dev->reg_base;
    if (channel < TOUCH_CHANNEL_MAX) {
        regval = getreg32(reg_base + TOUCH_CHANNEL_RAW_DATA_0_OFFSET + channel * 4);
        return (regval & 0xFFFF);
    }
    return 0;
#endif
}

uint32_t bflb_touch_get_lta_data(struct bflb_device_s *dev, uint8_t channel)
{
#ifdef romapi_bflb_touch_get_lta_data
    return romapi_bflb_touch_get_lta_data(dev, channel);
#else
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_TOUCH_CHANNEL_TYPE(channel));

    reg_base = dev->reg_base;
    if (channel < TOUCH_CHANNEL_MAX) {
        regval = getreg32(reg_base + TOUCH_CHANNEL_LTA_DATA_0_OFFSET + channel * 4);
        return (regval & 0xFFFF);
    }
    return 0;
#endif
}

uint32_t bflb_touch_get_flt_data(struct bflb_device_s *dev, uint8_t channel)
{
#ifdef romapi_bflb_touch_get_flt_data
    return romapi_bflb_touch_get_flt_data(dev, channel);
#else
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_TOUCH_CHANNEL_TYPE(channel));

    reg_base = dev->reg_base;
    if (channel < TOUCH_CHANNEL_MAX) {
        regval = getreg32(reg_base + TOUCH_CHANNEL_FLT_DATA_0_OFFSET + channel * 4);
        return (regval & 0xFFFF);
    }
    return 0;
#endif
}

bool bflb_touch_get_end_status(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_touch_get_end_status
    return romapi_bflb_touch_get_end_status(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TOUCH_INT_STATUS_OFFSET);
    if (regval & TOUCH_END_FLAG) {
        return true;
    } else {
        return false;
    }
#endif
}

void bflb_touch_enable_int(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_touch_enable_int
    romapi_bflb_touch_enable_int(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TOUCH_INT_SETTING_OFFSET);
    regval |= TOUCH_INT_EN;
    putreg32(regval, reg_base + TOUCH_INT_SETTING_OFFSET);
#endif
}

void bflb_touch_disable_int(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_touch_disable_int
    romapi_bflb_touch_disable_int(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TOUCH_INT_SETTING_OFFSET);
    regval &= ~TOUCH_INT_EN;
    putreg32(regval, reg_base + TOUCH_INT_SETTING_OFFSET);
#endif
}

void bflb_touch_int_mask(struct bflb_device_s *dev, uint32_t int_type, bool mask)
{
#ifdef romapi_bflb_touch_int_mask
    romapi_bflb_touch_int_mask(dev, int_type, mask);
#else
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_TOUCH_INT_TYPE(int_type));

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TOUCH_INT_SETTING_OFFSET);

    int_type &= 0xFFF;
    if (mask) {
        regval |= (int_type << 16);
    } else {
        regval &= ~(int_type << 16);
    }
    putreg32(regval, reg_base + TOUCH_INT_SETTING_OFFSET);
#endif
}

uint32_t bflb_touch_get_intstatus(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_touch_get_intstatus
    return romapi_bflb_touch_get_intstatus(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TOUCH_INT_STATUS_OFFSET);
    return (regval & 0xFFF);
#endif
}

void bflb_touch_int_clear(struct bflb_device_s *dev, uint32_t int_type)
{
#ifdef romapi_bflb_touch_int_clear
    romapi_bflb_touch_int_clear(dev, int_type);
#else
    uint32_t reg_base;
    uint32_t regval;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_TOUCH_INT_TYPE(int_type));

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TOUCH_INT_SETTING_OFFSET);
    int_type &= 0xFFF;
    regval |= int_type;
    putreg32(regval, reg_base + TOUCH_INT_SETTING_OFFSET);

    regval = getreg32(reg_base + TOUCH_INT_SETTING_OFFSET);
    regval &= ~int_type;
    putreg32(regval, reg_base + TOUCH_INT_SETTING_OFFSET);
#endif
}

int bflb_touch_feature_control(struct bflb_device_s *dev, int cmd, size_t arg)
{
#ifdef romapi_bflb_touch_feature_control
    return romapi_bflb_touch_feature_control(dev, cmd, arg);
#else
    LHAL_PARAM_ASSERT(dev);

    switch (cmd) {
        case TOUCH_CMD_START:
            bflb_touch_enable(dev);
            break;
        case TOUCH_CMD_STOP:
            bflb_touch_disable(dev);
            break;
        default:
            return -1;
    }
    return 0;
#endif
}