#include "bflb_dpi.h"
#include "hardware/mm_misc_reg.h"
#include "hardware/dvp_tsrc_reg.h"

#if defined(BL616D)
#define DTSRC_BASE ((uint32_t)0x20045000)
#else
#error "not support dpi"
#endif

void bflb_dpi_init(struct bflb_device_s *dev, const struct bflb_dpi_config_s *config)
{
    LHAL_PARAM_ASSERT(dev);

#ifdef romapi_bflb_dpi_init
    romapi_bflb_dpi_init(dev, config);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    regval = config->hsw << MM_MISC_RG_DISP_DPI_HS_W_SHIFT;
    regval |= (config->hfp << MM_MISC_RG_DISP_DPI_HFP_W_SHIFT);
    regval |= (config->vsw << MM_MISC_RG_DISP_DPI_VS_W_SHIFT);
    regval |= (config->vfp << MM_MISC_RG_DISP_DPI_VFP_W_SHIFT);
    putreg32(regval, reg_base + MM_MISC_DISP_DPI_CONFIG_OFFSET);

    regval = getreg32(reg_base + MM_MISC_DISP_CONFIG_OFFSET);
    regval &= ~MM_MISC_RG_DISP_DPI_ICC_MASK;
    regval |= (config->interface << MM_MISC_RG_DISP_DPI_ICC_SHIFT);
    putreg32(regval, reg_base + MM_MISC_DISP_CONFIG_OFFSET);

    reg_base = DTSRC_BASE;
    regval = getreg32(reg_base + DVP_TSRC_CONFIG_OFFSET);
    regval &= ~DVP_TSRC_CR_ENABLE;
    putreg32(regval, reg_base + DVP_TSRC_CONFIG_OFFSET);

    regval &= ~(DVP_TSRC_CR_AXI_DVP_DATA_MODE_MASK | DVP_TSRC_CR_AXI_B0_SEL_MASK | DVP_TSRC_CR_AXI_B1_SEL_MASK | DVP_TSRC_CR_AXI_B2_SEL_MASK);
    regval |= ((config->data_format % 4) << DVP_TSRC_CR_AXI_DVP_DATA_MODE_SHIFT);
    switch (config->data_format) {
        case DPI_DATA_FORMAT_YUYV:
            regval |= (1 << DVP_TSRC_CR_AXI_B1_SEL_SHIFT);
            break;

        case DPI_DATA_FORMAT_RGB888:
            regval |= (2 << DVP_TSRC_CR_AXI_B0_SEL_SHIFT | 1 << DVP_TSRC_CR_AXI_B1_SEL_SHIFT);
            break;

        case DPI_DATA_FORMAT_RGB565:
            regval |= (1 << DVP_TSRC_CR_AXI_B0_SEL_SHIFT);
            break;

        case DPI_DATA_FORMAT_NRGB8888:
            regval |= (2 << DVP_TSRC_CR_AXI_B0_SEL_SHIFT | 1 << DVP_TSRC_CR_AXI_B1_SEL_SHIFT);
            break;

        case DPI_DATA_FORMAT_UYVY:
            regval |= (1 << DVP_TSRC_CR_AXI_B0_SEL_SHIFT);
            break;

        case DPI_DATA_FORMAT_BGR888:
            regval |= (1 << DVP_TSRC_CR_AXI_B1_SEL_SHIFT | 2 << DVP_TSRC_CR_AXI_B2_SEL_SHIFT);
            break;

        case DPI_DATA_FORMAT_BGR565:
            regval |= (1 << DVP_TSRC_CR_AXI_B2_SEL_SHIFT);
            break;

        case DPI_DATA_FORMAT_NBGR8888:
            regval |= (1 << DVP_TSRC_CR_AXI_B1_SEL_SHIFT | 2 << DVP_TSRC_CR_AXI_B2_SEL_SHIFT);
            break;

        case DPI_DATA_FORMAT_TEST_PATTERN:
            regval |= (1 << DVP_TSRC_CR_AXI_B1_SEL_SHIFT);
            break;

        default:
            regval |= (1 << DVP_TSRC_CR_AXI_B1_SEL_SHIFT | 2 << DVP_TSRC_CR_AXI_B2_SEL_SHIFT);
            break;
    }
    regval |= DVP_TSRC_CR_AXI_SWAP_MODE;
    regval |= DVP_TSRC_CR_AXI_SWAP_IDX_SWM;
    putreg32(regval, reg_base + DVP_TSRC_CONFIG_OFFSET);

    regval = config->hsw + config->hbp + config->hfp;
    regval = (regval << DVP_TSRC_CR_BLANK_H_SHIFT) | ((regval + config->width) & 0xffff);
    putreg32(regval, reg_base + DVP_TSRC_FRAME_SIZE_H_OFFSET);

    regval = config->vsw + config->vbp + config->vfp;
    regval = (regval << DVP_TSRC_CR_BLANK_V_SHIFT) | ((regval + config->height) & 0xffff);
    putreg32(regval, reg_base + DVP_TSRC_FRAME_SIZE_V_OFFSET);

    putreg32(2, reg_base + DVP_TSRC_AXI2DVP_PREFETCH_OFFSET);
    putreg32(config->framebuffer_addr, reg_base + DVP_TSRC_AXI2DVP_SWAP_ADDR_BY_OFFSET);

    switch (config->test_pattern) {
        case DPI_TEST_PATTERN_NULL:
            break;

        case DPI_TEST_PATTERN_BLACK:
            putreg32(0xffff0000, reg_base + DVP_TSRC_PIX_DATA_RANGE_OFFSET);
            putreg32(0, reg_base + DVP_TSRC_PIX_DATA_STEP_OFFSET);
            break;

        case DPI_TEST_PATTERN_RED:
            putreg32(0xffff00ff, reg_base + DVP_TSRC_PIX_DATA_RANGE_OFFSET);
            putreg32(0, reg_base + DVP_TSRC_PIX_DATA_STEP_OFFSET);
            break;

        case DPI_TEST_PATTERN_GREE:
            putreg32(0xffffff00, reg_base + DVP_TSRC_PIX_DATA_RANGE_OFFSET);
            putreg32(0, reg_base + DVP_TSRC_PIX_DATA_STEP_OFFSET);
            break;

        case DPI_TEST_PATTERN_YELLOW:
            putreg32(0xffffffff, reg_base + DVP_TSRC_PIX_DATA_RANGE_OFFSET);
            putreg32(0, reg_base + DVP_TSRC_PIX_DATA_STEP_OFFSET);
            break;

        default:
            break;
    }

    switch (config->input_sel) {
        case DPI_INPUT_SEL_TEST_PATTERN_WITHOUT_OSD:
            regval = getreg32(reg_base + DVP_TSRC_CONFIG_OFFSET);
            regval &= ~DVP_TSRC_CR_AXI_EN;
            putreg32(regval, reg_base + DVP_TSRC_CONFIG_OFFSET);
            regval = getreg32(dev->reg_base + MM_MISC_DVP_MUX_SEL_REG2_OFFSET);
            regval &= ~MM_MISC_RG_DISP_MUXO_SEL_MASK;
            regval |= 2 << MM_MISC_RG_DISP_MUXO_SEL_SHIFT;
            putreg32(regval, dev->reg_base + MM_MISC_DVP_MUX_SEL_REG2_OFFSET);
            break;

        case DPI_INPUT_SEL_TEST_PATTERN_WITH_OSD:
            regval = getreg32(reg_base + DVP_TSRC_CONFIG_OFFSET);
            regval &= ~DVP_TSRC_CR_AXI_EN;
            putreg32(regval, reg_base + DVP_TSRC_CONFIG_OFFSET);
            regval = getreg32(dev->reg_base + MM_MISC_DVP_MUX_SEL_REG2_OFFSET);
            regval &= ~(MM_MISC_RG_DISP_MUXO_SEL_MASK | MM_MISC_RG_DISP_OSD_SEL_MASK);
            regval |= 1 << MM_MISC_RG_DISP_MUXO_SEL_SHIFT;
            regval |= 2 << MM_MISC_RG_DISP_OSD_SEL_SHIFT;
            putreg32(regval, dev->reg_base + MM_MISC_DVP_MUX_SEL_REG2_OFFSET);

            putreg32(0, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_0_OFFSET);
            putreg32(128 << MM_MISC_CR_DISP_R2Y_POS_1_SHIFT, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_1_OFFSET);
            putreg32(128 << MM_MISC_CR_DISP_R2Y_POS_2_SHIFT, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_2_OFFSET);
            putreg32(0x99 | 0x12d << MM_MISC_CR_DISP_R2Y_MTX_01_SHIFT, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_3_OFFSET);
            putreg32(0x3a | 0xfa9 << MM_MISC_CR_DISP_R2Y_MTX_10_SHIFT, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_4_OFFSET);
            putreg32(0xf57 | 0x100 << MM_MISC_CR_DISP_R2Y_MTX_12_SHIFT, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_5_OFFSET);
            putreg32(0x100 | 0xf29 << MM_MISC_CR_DISP_R2Y_MTX_21_SHIFT, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_6_OFFSET);
            putreg32(0xfd7, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_7_OFFSET);
            putreg32(MM_MISC_CR_DISP_R2Y_EN, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_0_OFFSET);

            regval = getreg32(dev->reg_base + MM_MISC_DISP_Y2R_CONFIG_0_OFFSET);
            regval |= MM_MISC_CR_DISP_Y2R_EN;
            putreg32(regval, dev->reg_base + MM_MISC_DISP_Y2R_CONFIG_0_OFFSET);
            break;

        case DPI_INPUT_SEL_FRAMEBUFFER_WITHOUT_OSD:
            regval = getreg32(reg_base + DVP_TSRC_CONFIG_OFFSET);
            regval |= DVP_TSRC_CR_AXI_EN;
            putreg32(regval, reg_base + DVP_TSRC_CONFIG_OFFSET);
            regval = getreg32(dev->reg_base + MM_MISC_DVP_MUX_SEL_REG2_OFFSET);
            regval &= ~MM_MISC_RG_DISP_MUXO_SEL_MASK;
            regval |= 2 << MM_MISC_RG_DISP_MUXO_SEL_SHIFT;
            putreg32(regval, dev->reg_base + MM_MISC_DVP_MUX_SEL_REG2_OFFSET);
            break;

        case DPI_INPUT_SEL_FRAMEUFFER_WITH_OSD:
            regval = getreg32(reg_base + DVP_TSRC_CONFIG_OFFSET);
            regval |= DVP_TSRC_CR_AXI_EN;
            putreg32(regval, reg_base + DVP_TSRC_CONFIG_OFFSET);
            regval = getreg32(dev->reg_base + MM_MISC_DVP_MUX_SEL_REG2_OFFSET);
            regval &= ~(MM_MISC_RG_DISP_MUXO_SEL_MASK | MM_MISC_RG_DISP_OSD_SEL_MASK);
            regval |= 1 << MM_MISC_RG_DISP_MUXO_SEL_SHIFT;

            if (config->data_format == DPI_DATA_FORMAT_YUYV || config->data_format == DPI_DATA_FORMAT_UYVY) {
                regval |= 1 << MM_MISC_RG_DISP_OSD_SEL_SHIFT;
            } else {
                putreg32(0, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_0_OFFSET);
                putreg32(128 << MM_MISC_CR_DISP_R2Y_POS_1_SHIFT, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_1_OFFSET);
                putreg32(128 << MM_MISC_CR_DISP_R2Y_POS_2_SHIFT, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_2_OFFSET);
                putreg32(0x99 | 0x12d << MM_MISC_CR_DISP_R2Y_MTX_01_SHIFT, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_3_OFFSET);
                putreg32(0x3a | 0xfa9 << MM_MISC_CR_DISP_R2Y_MTX_10_SHIFT, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_4_OFFSET);
                putreg32(0xf57 | 0x100 << MM_MISC_CR_DISP_R2Y_MTX_12_SHIFT, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_5_OFFSET);
                putreg32(0x100 | 0xf29 << MM_MISC_CR_DISP_R2Y_MTX_21_SHIFT, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_6_OFFSET);
                putreg32(0xfd7, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_7_OFFSET);
                putreg32(MM_MISC_CR_DISP_R2Y_EN, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_0_OFFSET);
                regval |= 2 << MM_MISC_RG_DISP_OSD_SEL_SHIFT;
            }
            putreg32(regval, dev->reg_base + MM_MISC_DVP_MUX_SEL_REG2_OFFSET);

            regval = getreg32(dev->reg_base + MM_MISC_DISP_Y2R_CONFIG_0_OFFSET);
            regval |= MM_MISC_CR_DISP_Y2R_EN;
            putreg32(regval, dev->reg_base + MM_MISC_DISP_Y2R_CONFIG_0_OFFSET);
            break;

        default:
            regval = getreg32(reg_base + DVP_TSRC_CONFIG_OFFSET);
            regval &= ~DVP_TSRC_CR_AXI_EN;
            putreg32(regval, reg_base + DVP_TSRC_CONFIG_OFFSET);
            regval = getreg32(dev->reg_base + MM_MISC_DVP_MUX_SEL_REG2_OFFSET);
            regval &= ~MM_MISC_RG_DISP_MUXO_SEL_MASK;
            regval |= 2 << MM_MISC_RG_DISP_MUXO_SEL_SHIFT;
            putreg32(regval, dev->reg_base + MM_MISC_DVP_MUX_SEL_REG2_OFFSET);
            break;
    }

    putreg32(DVP_TSRC_CR_SW_SHD, reg_base + DVP_TSRC_AXI_PUSH_MODE_OFFSET);
    regval = getreg32(reg_base + DVP_TSRC_CONFIG_OFFSET);
    regval |= DVP_TSRC_CR_ENABLE;
    putreg32(regval, reg_base + DVP_TSRC_CONFIG_OFFSET);
#endif
}

void bflb_dpi_enable(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_dpi_enable
    romapi_bflb_dpi_enable(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + MM_MISC_DISP_CONFIG_OFFSET);
    regval |= MM_MISC_RG_DISP_DPI_EN;
    putreg32(regval, reg_base + MM_MISC_DISP_CONFIG_OFFSET);
#endif
}

void bflb_dpi_disable(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_dpi_disable
    romapi_bflb_dpi_disable(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + MM_MISC_DISP_CONFIG_OFFSET);
    regval &= ~MM_MISC_RG_DISP_DPI_EN;
    putreg32(regval, reg_base + MM_MISC_DISP_CONFIG_OFFSET);
#endif
}

void bflb_dpi_framebuffer_switch(struct bflb_device_s *dev, uint32_t addr)
{
#ifdef romapi_bflb_dpi_framebuffer_switch
    romapi_bflb_dpi_framebuffer_switch(dev, addr);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = DTSRC_BASE;
    regval = getreg32(reg_base + DVP_TSRC_CONFIG_OFFSET);
    if (regval & DVP_TSRC_CR_AXI_SWAP_IDX_SWV) {
        putreg32(addr, reg_base + DVP_TSRC_AXI2DVP_SWAP_ADDR_BY_OFFSET);
        regval &= ~(DVP_TSRC_CR_AXI_SWAP_IDX_SWV);
    } else {
        putreg32(addr, reg_base + DVP_TSRC_AXI2DVP_START_ADDR_BY_OFFSET);
        regval |= DVP_TSRC_CR_AXI_SWAP_IDX_SWV;
    }
    putreg32(regval, reg_base + DVP_TSRC_CONFIG_OFFSET);
#endif
}

uint32_t bflb_dpi_get_framebuffer_using(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_dpi_get_framebuffer_using
    return romapi_bflb_dpi_get_framebuffer_using(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = DTSRC_BASE;
    regval = getreg32(reg_base + DVP_TSRC_CONFIG_OFFSET);
    if (regval & DVP_TSRC_CR_AXI_SWAP_IDX_SWV) {
        return (getreg32(reg_base + DVP_TSRC_AXI2DVP_START_ADDR_BY_OFFSET));
    } else {
        return (getreg32(reg_base + DVP_TSRC_AXI2DVP_SWAP_ADDR_BY_OFFSET));
    }
#endif
}

void bflb_dpi_set_test_pattern_custom(struct bflb_device_s *dev, uint16_t max, uint16_t value, uint8_t step)
{
#ifdef romapi_bflb_dpi_set_test_pattern_custom
    romapi_bflb_dpi_set_test_pattern_custom(dev, max, value, step);
#else
    putreg32(max << DVP_TSRC_CR_DATA_MAX_SHIFT | value, DTSRC_BASE + DVP_TSRC_PIX_DATA_RANGE_OFFSET);
    putreg32(step, DTSRC_BASE + DVP_TSRC_PIX_DATA_STEP_OFFSET);
#endif
}

int bflb_dpi_feature_control(struct bflb_device_s *dev, int cmd, size_t arg)
{
#ifdef romapi_bflb_dpi_feature_control
    return romapi_bflb_dpi_feature_control(dev, cmd, arg);
#else
    int ret = 0;

    switch (cmd) {
        default:
            ret = -EPERM;
            break;
    }

    return ret;
#endif
}
