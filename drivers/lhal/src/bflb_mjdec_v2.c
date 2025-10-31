#include "bflb_mjdec_v2.h"
#include "hardware/mjdec_v2_reg.h"

void bflb_mjdec_init(struct bflb_device_s *dev, const struct bflb_mjdec_config_s *config)
{
    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_MJDEC_FORMAT(config->format));
    LHAL_PARAM_ASSERT(IS_MJDEC_ADDR(config->output_bufaddr0));
    LHAL_PARAM_ASSERT(IS_MJDEC_ADDR(config->output_bufaddr1));

#ifdef romapi_bflb_mjdec_init
    romapi_bflb_mjdec_init(dev, config);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJDEC_CONFIG_OFFSET);
    regval |= MJDEC_EN;
    regval &= ~MJDEC_START_DEC;
    if (config->parse_header) {
        regval &= ~MJDEC_SKIP_HDER;
    } else {
        regval |= MJDEC_SKIP_HDER;
    }
    if (config->has_header) {
        regval &= ~MJDEC_STREAM_NO_HDER;
    } else {
        regval |= MJDEC_STREAM_NO_HDER;
    }
    regval |= MJDEC_IN_BURST_TYPE_MASK;
    regval |= MJDEC_OUT_BURST_TYPE_MASK;
    regval &= ~MJDEC_PIXEL_FORMAT_MASK;
    regval &= ~MJDEC_SEMI_PLAN_TYPE;
    regval &= ~MJDEC_YUV_INTLV_TYPE_MASK;
    regval &= ~MJDEC_RGB_TYPE_MASK;
    regval &= ~MJDEC_ALPHA_VALUE_MASK;
    regval |= (config->alpha << MJDEC_ALPHA_VALUE_SHIFT);
    switch (config->format) {
        case MJDEC_FORMAT_GRAY:
            regval &= ~MJDEC_PIXEL_FORMAT_MASK;
            break;

        case MJDEC_FORMAT_YUYV:
            regval |= (1 << MJDEC_PIXEL_FORMAT_SHIFT);
            regval |= (0 << MJDEC_YUV_INTLV_TYPE_SHIFT);
            break;

        case MJDEC_FORMAT_YVYU:
            regval |= (1 << MJDEC_PIXEL_FORMAT_SHIFT);
            regval |= (1 << MJDEC_YUV_INTLV_TYPE_SHIFT);
            break;

        case MJDEC_FORMAT_UYVY:
            regval |= (1 << MJDEC_PIXEL_FORMAT_SHIFT);
            regval |= (2 << MJDEC_YUV_INTLV_TYPE_SHIFT);
            break;

        case MJDEC_FORMAT_VYUY:
            regval |= (1 << MJDEC_PIXEL_FORMAT_SHIFT);
            regval |= (3 << MJDEC_YUV_INTLV_TYPE_SHIFT);
            break;

        case MJDEC_FORMAT_NV16_OR_NV12:
            regval |= (2 << MJDEC_PIXEL_FORMAT_SHIFT);
            regval &= ~MJDEC_SEMI_PLAN_TYPE;
            break;

        case MJDEC_FORMAT_NV61_OR_NV21:
            regval |= (2 << MJDEC_PIXEL_FORMAT_SHIFT);
            regval |= MJDEC_SEMI_PLAN_TYPE;
            break;

        case MJDEC_FORMAT_RGB888:
            regval |= (3 << MJDEC_PIXEL_FORMAT_SHIFT);
            regval |= (0 << MJDEC_RGB_TYPE_SHIFT);
            break;

        case MJDEC_FORMAT_RGB565:
            regval |= (3 << MJDEC_PIXEL_FORMAT_SHIFT);
            regval |= (5 << MJDEC_RGB_TYPE_SHIFT);
            break;

        case MJDEC_FORMAT_NRGB8888:
            regval |= (3 << MJDEC_PIXEL_FORMAT_SHIFT);
            regval |= (2 << MJDEC_RGB_TYPE_SHIFT);
            break;

        case MJDEC_FORMAT_BGR888:
            regval |= (3 << MJDEC_PIXEL_FORMAT_SHIFT);
            regval |= (4 << MJDEC_RGB_TYPE_SHIFT);
            break;

        case MJDEC_FORMAT_BGR565:
            regval |= (3 << MJDEC_PIXEL_FORMAT_SHIFT);
            regval |= (1 << MJDEC_RGB_TYPE_SHIFT);
            break;

        case MJDEC_FORMAT_NBGR8888:
            regval |= (3 << MJDEC_PIXEL_FORMAT_SHIFT);
            regval |= (6 << MJDEC_RGB_TYPE_SHIFT);
            break;

        default:
            break;
    }
    putreg32(regval, reg_base + MJDEC_CONFIG_OFFSET);

    putreg32(config->input_bufaddr, reg_base + MJDEC_JPEG_STREAM_BASE_OFFSET);
    putreg32(config->output_bufaddr0, reg_base + MJDEC_OUT_Y_BASE_OFFSET);
    putreg32(config->output_bufaddr1, reg_base + MJDEC_OUT_UV_BASE_OFFSET);

    regval = getreg32(reg_base + MJDEC_INT_CFG_OFFSET);
    regval &= ~MJDEC_INT_ROW_NUM_MASK;
    if (config->row_of_interrupt == 0) {
        regval |= MJDEC_INT_ROW_NUM_MASK;
    } else {
        regval |= (config->row_of_interrupt << MJDEC_INT_ROW_NUM_SHIFT);
    }
    putreg32(regval, reg_base + MJDEC_INT_CFG_OFFSET);

    regval = getreg32(reg_base + MJDEC_PAUSE_ROW_OFFSET);
    regval &= ~MJDEC_PAUSE_ROW_NUM_MASK;
    regval |= (config->row_of_pause << MJDEC_PAUSE_ROW_NUM_SHIFT);
    putreg32(regval, reg_base + MJDEC_PAUSE_ROW_OFFSET);

    regval = getreg32(reg_base + MJDEC_LOOP_BUFFER_OFFSET);
    regval &= ~MJDEC_OUT_Y_END_ROW_MASK;
    regval |= (config->row_of_addr_loop << MJDEC_OUT_Y_END_ROW_SHIFT);
    putreg32(regval, reg_base + MJDEC_LOOP_BUFFER_OFFSET);
#endif
}

void bflb_mjdec_deinit(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_mjdec_deinit
    romapi_bflb_mjdec_deinit(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJDEC_CONFIG_OFFSET);
    regval &= ~MJDEC_EN;
    putreg32(regval, reg_base + MJDEC_CONFIG_OFFSET);
#endif
}

void bflb_mjdec_start(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_mjdec_start
    romapi_bflb_mjdec_start(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJDEC_CONFIG_OFFSET);
    regval |= MJDEC_START_DEC;
    putreg32(regval, reg_base + MJDEC_CONFIG_OFFSET);
#endif
}

void bflb_mjdec_get_parse_result(struct bflb_device_s *dev, struct bflb_mjdec_parse_s *parse)
{
#ifdef romapi_bflb_mjdec_get_parse_result
    romapi_bflb_mjdec_get_parse_result(dev, parse);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJDEC_HDER_IMAGE_SIZE_OFFSET);
    parse->resolution_x = (regval & MJDEC_IMAGE_WIDTH_MASK) >> MJDEC_IMAGE_WIDTH_SHIFT;
    parse->resolution_y = (regval & MJDEC_IMAGE_HEIGHT_MASK) >> MJDEC_IMAGE_HEIGHT_SHIFT;

    regval = getreg32(reg_base + MJDEC_HDER_INFO_OFFSET);
    parse->header_length = (regval & MJDEC_HDER_LENGTH_MASK) >> MJDEC_HDER_LENGTH_SHIFT;
    parse->samp_yy_horizon = (regval & MJDEC_IMAGE_Y_H_SAMP_MASK) >> MJDEC_IMAGE_Y_H_SAMP_SHIFT;
    parse->samp_yy_vertical = (regval & MJDEC_IMAGE_Y_V_SAMP_MASK) >> MJDEC_IMAGE_Y_V_SAMP_SHIFT;
    parse->samp_cb_horizon = (regval & MJDEC_IMAGE_CB_H_SAMP_MASK) >> MJDEC_IMAGE_CB_H_SAMP_SHIFT;
    parse->samp_cb_vertical = (regval & MJDEC_IMAGE_CB_V_SAMP_MASK) >> MJDEC_IMAGE_CB_V_SAMP_SHIFT;
    parse->samp_cr_horizon = (regval & MJDEC_IMAGE_CR_H_SAMP_MASK) >> MJDEC_IMAGE_CR_H_SAMP_SHIFT;
    parse->samp_cr_vertical = (regval & MJDEC_IMAGE_CR_V_SAMP_MASK) >> MJDEC_IMAGE_CR_V_SAMP_SHIFT;
#endif
}

void bflb_mjdec_set_row_count_pause(struct bflb_device_s *dev, uint16_t row)
{
#ifdef romapi_bflb_mjdec_set_row_count_pause
    romapi_bflb_mjdec_set_row_count_pause(dev, row);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + MJDEC_PAUSE_ROW_OFFSET);
    regval &= ~MJDEC_PAUSE_ROW_NUM_MASK;
    regval |= (row << MJDEC_PAUSE_ROW_NUM_SHIFT);
    putreg32(regval, reg_base + MJDEC_PAUSE_ROW_OFFSET);
#endif
}

uint16_t bflb_mjdec_get_row_count_pause(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_mjdec_get_row_count_pause
    return romapi_bflb_mjdec_get_row_count_pause(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + MJDEC_PAUSE_ROW_OFFSET);
    regval &= MJDEC_PAUSE_ROW_NUM_MASK;
    regval >>= MJDEC_PAUSE_ROW_NUM_SHIFT;

    return (uint16_t)regval;
#endif
}

void bflb_mjdec_int_enable(struct bflb_device_s *dev, uint32_t int_en, bool enable)
{
#ifdef romapi_bflb_mjdec_int_enable
    romapi_bflb_mjdec_int_enable(dev, int_en, enable);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    int_en &= MJDEC_INT_ALL;
    regval = getreg32(reg_base + MJDEC_INT_CFG_OFFSET);
    if (enable) {
        regval |= int_en;
    } else {
        regval &= ~int_en;
    }
    putreg32(regval, reg_base + MJDEC_INT_CFG_OFFSET);
#endif
}

uint32_t bflb_mjdec_get_intstatus(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_mjdec_get_intstatus
    return romapi_bflb_mjdec_get_intstatus(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJDEC_STAT_OFFSET);
    regval &= MJDEC_INT_ALL;

    return regval;
#endif
}

void bflb_mjdec_int_clear(struct bflb_device_s *dev, uint32_t int_clear)
{
#ifdef romapi_bflb_mjdec_int_clear
    romapi_bflb_mjdec_int_clear(dev, int_clear);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    int_clear &= MJDEC_INT_ALL;
    regval = getreg32(reg_base + MJDEC_STAT_OFFSET);
    regval |= int_clear;
    putreg32(regval, reg_base + MJDEC_STAT_OFFSET);
#endif
}

int bflb_mjdec_feature_control(struct bflb_device_s *dev, int cmd, size_t arg)
{
#ifdef romapi_bflb_mjdec_feature_control
    return romapi_bflb_mjdec_feature_control(dev, cmd, arg);
#else
    int ret = 0;
    __UNUSED uint32_t regval;
    __UNUSED uint32_t reg_base;

    reg_base = dev->reg_base;

    switch (cmd) {
        default:
            ret = -EPERM;
            break;
    }

    return ret;
#endif
}
