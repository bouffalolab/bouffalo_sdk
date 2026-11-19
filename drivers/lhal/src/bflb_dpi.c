#include "bflb_dpi.h"
#include "hardware/mm_misc_reg.h"
#include "hardware/dvp_tsrc_reg.h"

#if defined(BL618DG)
#define DTSRC_BASE ((uint32_t)0x20045000)
#else
#error "not support dpi"
#endif

void bflb_dpi_set_pclk_sample_edge(struct bflb_device_s *dev, uint8_t edge)
{
    LHAL_PARAM_ASSERT(dev);

#ifdef romapi_bflb_dpi_set_pclk_sample_edge
    romapi_bflb_dpi_set_pclk_sample_edge(dev, edge);
#else
    uint32_t regval;

    regval = getreg32(dev->reg_base + MM_MISC_DISP_CONFIG_OFFSET);
    if (edge == DPI_PCLK_SAMPLE_EDGE_RISING) {
        regval |= MM_MISC_RG_DISP_16;
    } else {
        regval &= ~MM_MISC_RG_DISP_16;
    }
    putreg32(regval, dev->reg_base + MM_MISC_DISP_CONFIG_OFFSET);
#endif
}

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

    regval = getreg32(reg_base + DVP_TSRC_AXI2DVP_SETTING_OFFSET);
    regval &= ~DVP_TSRC_CR_AXI_420_SP_MODE;
    putreg32(regval, reg_base + DVP_TSRC_AXI2DVP_SETTING_OFFSET);

    regval &= ~(DVP_TSRC_CR_AXI_DVP_DATA_MODE_MASK | DVP_TSRC_CR_AXI_B0_SEL_MASK | DVP_TSRC_CR_AXI_B1_SEL_MASK |
                DVP_TSRC_CR_AXI_B2_SEL_MASK);
    regval |= ((config->data_format % 5) << DVP_TSRC_CR_AXI_DVP_DATA_MODE_SHIFT);
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

        case DPI_DATA_FORMAT_Y_UV_PLANAR:
            putreg32(getreg32(reg_base + DVP_TSRC_AXI2DVP_SETTING_OFFSET) | DVP_TSRC_CR_AXI_420_SP_MODE,
                     reg_base + DVP_TSRC_AXI2DVP_SETTING_OFFSET);
            putreg32(config->uv_framebuffer_addr, reg_base + DVP_TSRC_AXI2DVP_SWAP_ADDR_UV_OFFSET);
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
            putreg32(0x99 | 0x12d << MM_MISC_CR_DISP_R2Y_MTX_01_SHIFT,
                     dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_3_OFFSET);
            putreg32(0x3a | 0xfa9 << MM_MISC_CR_DISP_R2Y_MTX_10_SHIFT,
                     dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_4_OFFSET);
            putreg32(0xf57 | 0x100 << MM_MISC_CR_DISP_R2Y_MTX_12_SHIFT,
                     dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_5_OFFSET);
            putreg32(0x100 | 0xf29 << MM_MISC_CR_DISP_R2Y_MTX_21_SHIFT,
                     dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_6_OFFSET);
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

        case DPI_INPUT_SEL_FRAMEBUFFER_WITH_OSD:
            regval = getreg32(reg_base + DVP_TSRC_CONFIG_OFFSET);
            regval |= DVP_TSRC_CR_AXI_EN;
            putreg32(regval, reg_base + DVP_TSRC_CONFIG_OFFSET);
            regval = getreg32(dev->reg_base + MM_MISC_DVP_MUX_SEL_REG2_OFFSET);
            regval &= ~(MM_MISC_RG_DISP_MUXO_SEL_MASK | MM_MISC_RG_DISP_OSD_SEL_MASK);
            regval |= 1 << MM_MISC_RG_DISP_MUXO_SEL_SHIFT;

            if (config->data_format == DPI_DATA_FORMAT_YUYV || config->data_format == DPI_DATA_FORMAT_UYVY) {
                regval |= 1 << MM_MISC_RG_DISP_OSD_SEL_SHIFT;
            } else if (config->data_format == DPI_DATA_FORMAT_Y_UV_PLANAR) {
                regval &= ~MM_MISC_RG_DISP_OSD_SEL_MASK;
            } else {
                putreg32(0, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_0_OFFSET);
                putreg32(128 << MM_MISC_CR_DISP_R2Y_POS_1_SHIFT, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_1_OFFSET);
                putreg32(128 << MM_MISC_CR_DISP_R2Y_POS_2_SHIFT, dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_2_OFFSET);
                putreg32(0x99 | 0x12d << MM_MISC_CR_DISP_R2Y_MTX_01_SHIFT,
                         dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_3_OFFSET);
                putreg32(0x3a | 0xfa9 << MM_MISC_CR_DISP_R2Y_MTX_10_SHIFT,
                         dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_4_OFFSET);
                putreg32(0xf57 | 0x100 << MM_MISC_CR_DISP_R2Y_MTX_12_SHIFT,
                         dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_5_OFFSET);
                putreg32(0x100 | 0xf29 << MM_MISC_CR_DISP_R2Y_MTX_21_SHIFT,
                         dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_6_OFFSET);
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

    bflb_dpi_set_pclk_sample_edge(dev, DPI_PCLK_SAMPLE_EDGE_RISING);
}

void bflb_dpi_set_input(struct bflb_device_s *dev, uint8_t input_sel, uint8_t data_format,
                       uint32_t y_addr, uint32_t uv_addr)
{
    uint32_t config;
    uint32_t setting;
    uint32_t mux;
    const bool frame = input_sel == DPI_INPUT_SEL_FRAMEBUFFER_WITH_OSD ||
                       input_sel == DPI_INPUT_SEL_FRAMEBUFFER_WITHOUT_OSD;
    const bool osd = input_sel == DPI_INPUT_SEL_TEST_PATTERN_WITH_OSD ||
                     input_sel == DPI_INPUT_SEL_FRAMEBUFFER_WITH_OSD;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(input_sel <= DPI_INPUT_SEL_FRAMEBUFFER_WITH_OSD);

    /* Use register access even when init/switch have ROM implementations: this
     * operation must preserve the range matrices and the running timing. */
    config = getreg32(DTSRC_BASE + DVP_TSRC_CONFIG_OFFSET);
    config &= ~(DVP_TSRC_CR_AXI_EN | DVP_TSRC_CR_AXI_DVP_DATA_MODE_MASK |
                DVP_TSRC_CR_AXI_B0_SEL_MASK | DVP_TSRC_CR_AXI_B1_SEL_MASK |
                DVP_TSRC_CR_AXI_B2_SEL_MASK | DVP_TSRC_CR_AXI_SWAP_IDX_SWV);
    config |= DVP_TSRC_CR_AXI_SWAP_MODE | DVP_TSRC_CR_AXI_SWAP_IDX_SWM;
    config |= (uint32_t)(data_format % 5u) << DVP_TSRC_CR_AXI_DVP_DATA_MODE_SHIFT;
    switch (data_format) {
        case DPI_DATA_FORMAT_RGB888:
        case DPI_DATA_FORMAT_NRGB8888:
            config |= (2u << DVP_TSRC_CR_AXI_B0_SEL_SHIFT) | (1u << DVP_TSRC_CR_AXI_B1_SEL_SHIFT);
            break;
        case DPI_DATA_FORMAT_RGB565:
        case DPI_DATA_FORMAT_UYVY:
            config |= 1u << DVP_TSRC_CR_AXI_B0_SEL_SHIFT;
            break;
        case DPI_DATA_FORMAT_BGR888:
        case DPI_DATA_FORMAT_NBGR8888:
            config |= (1u << DVP_TSRC_CR_AXI_B1_SEL_SHIFT) | (2u << DVP_TSRC_CR_AXI_B2_SEL_SHIFT);
            break;
        case DPI_DATA_FORMAT_BGR565:
            config |= 1u << DVP_TSRC_CR_AXI_B2_SEL_SHIFT;
            break;
        case DPI_DATA_FORMAT_Y_UV_PLANAR:
            break;
        default: /* YUYV or the two-byte R/G test pattern */
            config |= 1u << DVP_TSRC_CR_AXI_B1_SEL_SHIFT;
            break;
    }

    setting = getreg32(DTSRC_BASE + DVP_TSRC_AXI2DVP_SETTING_OFFSET);
    setting &= ~DVP_TSRC_CR_AXI_420_SP_MODE;
    if (frame && data_format == DPI_DATA_FORMAT_Y_UV_PLANAR) {
        setting |= DVP_TSRC_CR_AXI_420_SP_MODE;
    }
    putreg32(setting, DTSRC_BASE + DVP_TSRC_AXI2DVP_SETTING_OFFSET);
    if (frame) {
        /* SWV=0 selects SWAP, matching framebuffer_switch(). Both slots begin
         * with the same frame so no previously retired address is resurrected. */
        putreg32(y_addr, DTSRC_BASE + DVP_TSRC_AXI2DVP_START_ADDR_BY_OFFSET);
        putreg32(y_addr, DTSRC_BASE + DVP_TSRC_AXI2DVP_SWAP_ADDR_BY_OFFSET);
        putreg32(uv_addr, DTSRC_BASE + DVP_TSRC_AXI2DVP_START_ADDR_UV_OFFSET);
        putreg32(uv_addr, DTSRC_BASE + DVP_TSRC_AXI2DVP_SWAP_ADDR_UV_OFFSET);
        config |= DVP_TSRC_CR_AXI_EN;
    }

    mux = getreg32(dev->reg_base + MM_MISC_DVP_MUX_SEL_REG2_OFFSET);
    mux &= ~(MM_MISC_RG_DISP_MUXO_SEL_MASK | MM_MISC_RG_DISP_OSD_SEL_MASK);
    mux |= (osd ? 1u : 2u) << MM_MISC_RG_DISP_MUXO_SEL_SHIFT;
    if (osd) {
        if (frame && (data_format == DPI_DATA_FORMAT_YUYV || data_format == DPI_DATA_FORMAT_UYVY)) {
            mux |= 1u << MM_MISC_RG_DISP_OSD_SEL_SHIFT;
        } else if (!frame || data_format != DPI_DATA_FORMAT_Y_UV_PLANAR) {
            mux |= 2u << MM_MISC_RG_DISP_OSD_SEL_SHIFT;
            putreg32(getreg32(dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_0_OFFSET) | MM_MISC_CR_DISP_R2Y_EN,
                     dev->reg_base + MM_MISC_DISP_R2Y_CONFIG_0_OFFSET);
        }
        putreg32(getreg32(dev->reg_base + MM_MISC_DISP_Y2R_CONFIG_0_OFFSET) | MM_MISC_CR_DISP_Y2R_EN,
                 dev->reg_base + MM_MISC_DISP_Y2R_CONFIG_0_OFFSET);
    }
    putreg32(mux, dev->reg_base + MM_MISC_DVP_MUX_SEL_REG2_OFFSET);
    putreg32(DVP_TSRC_CR_SW_SHD, DTSRC_BASE + DVP_TSRC_AXI_PUSH_MODE_OFFSET);
    putreg32(config, DTSRC_BASE + DVP_TSRC_CONFIG_OFFSET);
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

void bflb_dpi_framebuffer_planar_switch(struct bflb_device_s *dev, uint32_t y_addr, uint32_t uv_addr)
{
#ifdef romapi_bflb_dpi_framebuffer_planar_switch
    romapi_bflb_dpi_framebuffer_planar_switch(dev, y_addr, uv_addr);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = DTSRC_BASE;
    regval = getreg32(reg_base + DVP_TSRC_CONFIG_OFFSET);
    if (regval & DVP_TSRC_CR_AXI_SWAP_IDX_SWV) {
        putreg32(y_addr, reg_base + DVP_TSRC_AXI2DVP_SWAP_ADDR_BY_OFFSET);
        putreg32(uv_addr, reg_base + DVP_TSRC_AXI2DVP_SWAP_ADDR_UV_OFFSET);
        regval &= ~(DVP_TSRC_CR_AXI_SWAP_IDX_SWV);
    } else {
        putreg32(y_addr, reg_base + DVP_TSRC_AXI2DVP_START_ADDR_BY_OFFSET);
        putreg32(uv_addr, reg_base + DVP_TSRC_AXI2DVP_START_ADDR_UV_OFFSET);
        regval |= DVP_TSRC_CR_AXI_SWAP_IDX_SWV;
    }
    putreg32(regval, reg_base + DVP_TSRC_CONFIG_OFFSET);
#endif
}

/* 12-bit two's complement field of the matrices of the converters */
#define DPI_MTX(v) ((uint32_t)(v) & 0xfffu)

void bflb_dpi_set_yuv_range(struct bflb_device_s *dev, uint8_t range)
{
    uint32_t regval;
    uint32_t reg_base = dev->reg_base;
    const bool limited = range == DPI_YUV_RANGE_LIMITED;

    /* RGB -> YUV (the frames of RGB format, 9-bit offset POS_0 of Y): coefficients with 512 as 1.0 */
    regval = getreg32(reg_base + MM_MISC_DISP_R2Y_CONFIG_0_OFFSET);
    regval &= ~MM_MISC_CR_DISP_R2Y_POS_0_MASK;
    regval |= (limited ? 16u : 0u) << MM_MISC_CR_DISP_R2Y_POS_0_SHIFT;
    putreg32(regval, reg_base + MM_MISC_DISP_R2Y_CONFIG_0_OFFSET);
    putreg32(limited ? (DPI_MTX(132) | DPI_MTX(258) << 16) : (DPI_MTX(153) | DPI_MTX(301) << 16),
             reg_base + MM_MISC_DISP_R2Y_CONFIG_3_OFFSET);
    putreg32(limited ? (DPI_MTX(50) | DPI_MTX(-76) << 16) : (DPI_MTX(58) | DPI_MTX(-87) << 16),
             reg_base + MM_MISC_DISP_R2Y_CONFIG_4_OFFSET);
    putreg32(limited ? (DPI_MTX(-149) | DPI_MTX(225) << 16) : (DPI_MTX(-169) | DPI_MTX(256) << 16),
             reg_base + MM_MISC_DISP_R2Y_CONFIG_5_OFFSET);
    putreg32(limited ? (DPI_MTX(225) | DPI_MTX(-188) << 16) : (DPI_MTX(256) | DPI_MTX(-215) << 16),
             reg_base + MM_MISC_DISP_R2Y_CONFIG_6_OFFSET);
    putreg32(limited ? DPI_MTX(-37) : DPI_MTX(-41), reg_base + MM_MISC_DISP_R2Y_CONFIG_7_OFFSET);

    /* YUV -> RGB (before the output): the offset of Y is -16 (9 bit) for the limited range */
    regval = getreg32(reg_base + MM_MISC_DISP_Y2R_CONFIG_0_OFFSET);
    regval &= ~MM_MISC_CR_DISP_Y2R_PRE_0_MASK;
    regval |= (limited ? 0x1f0u : 0u) << MM_MISC_CR_DISP_Y2R_PRE_0_SHIFT;
    putreg32(regval, reg_base + MM_MISC_DISP_Y2R_CONFIG_0_OFFSET);
    putreg32(limited ? DPI_MTX(596) : DPI_MTX(512), reg_base + MM_MISC_DISP_Y2R_CONFIG_3_OFFSET);
    putreg32(limited ? (DPI_MTX(817) | DPI_MTX(596) << 16) : (DPI_MTX(718) | DPI_MTX(512) << 16),
             reg_base + MM_MISC_DISP_Y2R_CONFIG_4_OFFSET);
    putreg32(limited ? (DPI_MTX(-201) | DPI_MTX(-416) << 16) : (DPI_MTX(-176) | DPI_MTX(-368) << 16),
             reg_base + MM_MISC_DISP_Y2R_CONFIG_5_OFFSET);
    putreg32(limited ? (DPI_MTX(596) | DPI_MTX(1033) << 16) : (DPI_MTX(512) | DPI_MTX(907) << 16),
             reg_base + MM_MISC_DISP_Y2R_CONFIG_6_OFFSET);
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
    uint32_t regval;
    uint32_t reg_base;

    reg_base = DTSRC_BASE;
    switch (cmd) {
        case DPI_CMD_SET_YUV420_UV_VALID_LINE:
            /* Set yuv420 uv valid line, arg use @ref DPI_YUV420 */
            regval = getreg32(reg_base + DVP_TSRC_AXI2DVP_SETTING_OFFSET);
            if (arg) {
                regval |= DVP_TSRC_CR_AXI_420_UD_SEL;
            } else {
                regval &= ~(DVP_TSRC_CR_AXI_420_UD_SEL);
            }
            putreg32(regval, reg_base + DVP_TSRC_AXI2DVP_SETTING_OFFSET);
            break;

        case DPI_CMD_SET_BURST:
            /* Set burst length, arg use @ref DPI_BURST */
            regval = getreg32(reg_base + DVP_TSRC_AXI2DVP_SETTING_OFFSET);
            regval &= ~DVP_TSRC_CR_AXI_XLEN_MASK;
            regval |= ((arg << DVP_TSRC_CR_AXI_XLEN_SHIFT) & DVP_TSRC_CR_AXI_XLEN_MASK);
            putreg32(regval, reg_base + DVP_TSRC_AXI2DVP_SETTING_OFFSET);
            break;

        default:
            ret = -EPERM;
            break;
    }

    return ret;
#endif
}
