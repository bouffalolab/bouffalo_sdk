#include "bflb_dvp_raster.h"
#include "hardware/mm_misc_reg.h"
#include "hardware/dvp2sram_reg.h"
#include "hardware/raster2block_reg.h"
#include "hardware/block2raster_reg.h"

#if defined(BL616D)
#define MM_MISC_BASE      ((uint32_t)0x20040000)
#define DVP2SRAM0_BASE    ((uint32_t)0x20048000)
#define DVP2SRAM1_BASE    ((uint32_t)0x20049000)
#define BLOCK2RASTER_BASE ((uint32_t)0x20047000)
#else
#error "not support dvp raster"
#endif

void bflb_dvp_raster_init(struct bflb_device_s *dev, struct bflb_dvp_raster_config_s *config)
{
    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(config);
    LHAL_PARAM_ASSERT(config->resolution_x % 16 == 0);
    LHAL_PARAM_ASSERT(config->resolution_y % 16 == 0);

#ifdef romapi_bflb_dvp_raster_init
    romapi_bflb_dvp_raster_init(dev, config);
#else
    uint32_t reg_base;
    uint32_t regval;

    /* config dvp2sram0 */
    reg_base = DVP2SRAM0_BASE;
    putreg32((config->resolution_x * 8) << DVP2SRAM_REG_SRAM_ADDR_FEND_SHIFT, reg_base + DVP2SRAM_D2S_SRAM_ADDR_OFFSET);
    putreg32((config->resolution_y << DVP2SRAM_REG_FRAM_LINE_CNT_SHIFT) | config->resolution_x, reg_base + DVP2SRAM_D2S_MEM_BCNT_OFFSET);
    putreg32(config->resolution_x * config->resolution_y, reg_base + DVP2SRAM_D2S_FRAME_BCNT_OFFSET);

    /* config dvp2sram1 */
    reg_base = DVP2SRAM1_BASE;
    putreg32((config->resolution_x * 4) << DVP2SRAM_REG_SRAM_ADDR_FEND_SHIFT, reg_base + DVP2SRAM_D2S_SRAM_ADDR_OFFSET);
    putreg32(((config->resolution_y / 2) << DVP2SRAM_REG_FRAM_LINE_CNT_SHIFT) | config->resolution_x, reg_base + DVP2SRAM_D2S_MEM_BCNT_OFFSET);
    putreg32(config->resolution_x * config->resolution_y / 2, reg_base + DVP2SRAM_D2S_FRAME_BCNT_OFFSET);
    regval = getreg32(reg_base + DVP2SRAM_D2S_CONFIGUE_OFFSET);
    regval |= DVP2SRAM_REG_DVP_DATA_BSEL | DVP2SRAM_REG_V_SUBSAMPLE_EN;
    putreg32(regval, reg_base + DVP2SRAM_D2S_CONFIGUE_OFFSET);

    /* config mm_misc */
    reg_base = MM_MISC_BASE;
    regval = getreg32(reg_base + MM_MISC_DVP_MUX_SEL_REG_OFFSET);
    regval &= ~MM_MISC_REG_D2XA_IN_SEL_MASK;
    regval |= 2 << MM_MISC_REG_D2XA_IN_SEL_SHIFT;
    putreg32(regval, reg_base + MM_MISC_DVP_MUX_SEL_REG_OFFSET);

    regval = getreg32(reg_base + MM_MISC_OFFSET);
    regval &= ~(MM_MISC_CR_R2B_Y_SEL_MASK | MM_MISC_CR_R2B_C_SEL_MASK);
    regval |= (2 << MM_MISC_CR_R2B_Y_SEL_SHIFT) | (3 << MM_MISC_CR_R2B_C_SEL_SHIFT) | MM_MISC_CR_BLOCK_SRAM_EN;
    putreg32(regval, reg_base + MM_MISC_OFFSET);

    /* config raster2block */
    reg_base = dev->reg_base;
    putreg32(((config->resolution_y / 16) << RASTER2BLOCK_REG_FRAME_HBLK_SHIFT) | (config->resolution_x / 16), reg_base + RASTER2BLOCK_R2B_CFG1_OFFSET);
    putreg32(config->y_frame_addr, reg_base + RASTER2BLOCK_R2B_NENC_CFG0_OFFSET);
    putreg32(config->uv_frame_addr, reg_base + RASTER2BLOCK_R2B_NENC_CFG1_OFFSET);
    putreg32(config->uv_frame_cnt << RASTER2BLOCK_REG_C_MEM_FRAME_CNT_SHIFT | config->y_frame_cnt, reg_base + RASTER2BLOCK_R2B_NENC_CFG2_OFFSET);
    regval = getreg32(reg_base + RASTER2BLOCK_R2B_CFG0_OFFSET);
    regval &= ~(RASTER2BLOCK_REG_MIRROR_EN | RASTER2BLOCK_REG_FLIP_EN | RASTER2BLOCK_REG_FRAME_MODE_MASK);
    regval |= (config->mode & 0xf0);
    putreg32(regval, reg_base + RASTER2BLOCK_R2B_CFG0_OFFSET);

    /* config block2raster */
    reg_base = BLOCK2RASTER_BASE;
    regval = getreg32(reg_base + BLOCK2RASTER_B2R_BASIC_CFG0_OFFSET);
    regval |= BLOCK2RASTER_REG_B2R_DVP_MODE | BLOCK2RASTER_REG_B2R_BP_WAIT;
    putreg32(regval, reg_base + BLOCK2RASTER_B2R_BASIC_CFG0_OFFSET);
    if (config->mode & (1 << 6)) {
        putreg32(((config->resolution_x / 16) << BLOCK2RASTER_REG_H_BLK_SHIFT) | (config->resolution_y / 16), reg_base + BLOCK2RASTER_B2R_FRAME_CFG0_OFFSET);
    } else {
        putreg32(((config->resolution_y / 16) << BLOCK2RASTER_REG_H_BLK_SHIFT) | (config->resolution_x / 16), reg_base + BLOCK2RASTER_B2R_FRAME_CFG0_OFFSET);
    }
    regval = getreg32(reg_base + BLOCK2RASTER_B2R_FRAME_CFG1_OFFSET);
    regval &= ~BLOCK2RASTER_REG_W_BLANK_MASK;
    if (config->resolution_x <= 640) {
        putreg32(regval | (0xaff << BLOCK2RASTER_REG_W_BLANK_SHIFT), reg_base + BLOCK2RASTER_B2R_FRAME_CFG1_OFFSET);
    } else {
        putreg32(regval | (0xfff << BLOCK2RASTER_REG_W_BLANK_SHIFT), reg_base + BLOCK2RASTER_B2R_FRAME_CFG1_OFFSET);
    }
    putreg32(config->y_frame_addr, reg_base + BLOCK2RASTER_B2R_MEM_LAYOUT_CFG0_OFFSET);
    putreg32(config->uv_frame_addr, reg_base + BLOCK2RASTER_B2R_MEM_LAYOUT_CFG1_OFFSET);
#endif
}

void bflb_dvp_raster_deinit(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_dvp_raster_deinit
    romapi_bflb_dvp_raster_deinit(dev);
#else
    uint32_t regval;

    bflb_dvp_raster_disable(dev);
    regval = getreg32(MM_MISC_BASE + MM_MISC_OFFSET);
    regval &= ~MM_MISC_CR_BLOCK_SRAM_EN;
    putreg32(regval, MM_MISC_BASE + MM_MISC_OFFSET);
#endif
}

void bflb_dvp_raster_enable(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_dvp_raster_enable
    romapi_bflb_dvp_raster_enable(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = DVP2SRAM0_BASE;
    regval = getreg32(reg_base + DVP2SRAM_D2S_CONFIGUE_OFFSET);
    regval |= DVP2SRAM_REG_DVP_ENABLE;
    putreg32(regval, reg_base + DVP2SRAM_D2S_CONFIGUE_OFFSET);
    reg_base = DVP2SRAM1_BASE;
    regval = getreg32(reg_base + DVP2SRAM_D2S_CONFIGUE_OFFSET);
    regval |= DVP2SRAM_REG_DVP_ENABLE;
    putreg32(regval, reg_base + DVP2SRAM_D2S_CONFIGUE_OFFSET);
    reg_base = dev->reg_base;
    regval = getreg32(reg_base + RASTER2BLOCK_R2B_CFG0_OFFSET);
    regval |= RASTER2BLOCK_REG_R2B_EN;
    putreg32(regval, reg_base + RASTER2BLOCK_R2B_CFG0_OFFSET);
    reg_base = BLOCK2RASTER_BASE;
    regval = getreg32(reg_base + BLOCK2RASTER_B2R_BASIC_CFG0_OFFSET);
    regval |= BLOCK2RASTER_REG_ENABLE;
    putreg32(regval, reg_base + BLOCK2RASTER_B2R_BASIC_CFG0_OFFSET);
#endif
}

void bflb_dvp_raster_disable(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_dvp_raster_disable
    romapi_bflb_dvp_raster_disable(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = BLOCK2RASTER_BASE;
    regval = getreg32(reg_base + BLOCK2RASTER_B2R_BASIC_CFG0_OFFSET);
    regval &= ~BLOCK2RASTER_REG_ENABLE;
    putreg32(regval, reg_base + BLOCK2RASTER_B2R_BASIC_CFG0_OFFSET);
    reg_base = dev->reg_base;
    regval = getreg32(reg_base + RASTER2BLOCK_R2B_CFG0_OFFSET);
    regval &= ~RASTER2BLOCK_REG_R2B_EN;
    putreg32(regval, reg_base + RASTER2BLOCK_R2B_CFG0_OFFSET);
    reg_base = DVP2SRAM0_BASE;
    regval = getreg32(reg_base + DVP2SRAM_D2S_CONFIGUE_OFFSET);
    regval &= ~DVP2SRAM_REG_DVP_ENABLE;
    putreg32(regval, reg_base + DVP2SRAM_D2S_CONFIGUE_OFFSET);
    reg_base = DVP2SRAM1_BASE;
    regval = getreg32(reg_base + DVP2SRAM_D2S_CONFIGUE_OFFSET);
    regval &= ~DVP2SRAM_REG_DVP_ENABLE;
    putreg32(regval, reg_base + DVP2SRAM_D2S_CONFIGUE_OFFSET);
#endif
}

void bflb_dvp_raster_int_mask(struct bflb_device_s *dev, uint32_t int_type, bool mask)
{
#ifdef romapi_bflb_dvp_raster_int_mask
    romapi_bflb_dvp_raster_int_mask(dev, int_type, mask);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + RASTER2BLOCK_R2B_NENC_CFG5_OFFSET);
    if (mask) {
        regval &= ~(int_type & DVP_RASTER_INT_ALL);
    } else {
        regval |= (int_type & DVP_RASTER_INT_ALL);
    }
    putreg32(regval, reg_base + RASTER2BLOCK_R2B_NENC_CFG5_OFFSET);
#endif
}

bool bflb_dvp_raster_get_intstatus(struct bflb_device_s *dev, uint32_t int_type)
{
#ifdef romapi_bflb_dvp_raster_get_intstatus
    return romapi_bflb_dvp_raster_get_intstatus(dev, int_type);
#else
    uint32_t regval;

    regval = getreg32(dev->reg_base + RASTER2BLOCK_R2B_NENC_CFG5_OFFSET);
    if (regval & ((int_type & DVP_RASTER_INT_ALL) << 8)) {
        return true;
    } else {
        return false;
    }
#endif
}

void bflb_dvp_raster_int_clear(struct bflb_device_s *dev, uint32_t int_type)
{
#ifdef romapi_bflb_dvp_raster_int_clear
    romapi_bflb_dvp_raster_int_clear(dev, int_type);
#else
    putreg32(int_type & DVP_RASTER_INT_ALL, dev->reg_base + RASTER2BLOCK_R2B_NENC_CFG6_OFFSET);
#endif
}

void bflb_dvp_raster_crop_vsync(struct bflb_device_s *dev, uint16_t start_line, uint16_t end_line)
{
#ifdef romapi_bflb_dvp_raster_crop_vsync
    romapi_bflb_dvp_raster_crop_vsync(dev, start_line, end_line);
#else
    /* Get start_line ~ (end_line - 1), not include end_line */
    putreg32(start_line << 16 | end_line, DVP2SRAM0_BASE + DVP2SRAM_D2S_VSYNC_CROP_OFFSET);
    putreg32(start_line << 16 | end_line, DVP2SRAM1_BASE + DVP2SRAM_D2S_VSYNC_CROP_OFFSET);
#endif
}

void bflb_dvp_raster_crop_hsync(struct bflb_device_s *dev, uint16_t start_pixel, uint16_t end_pixel)
{
#ifdef romapi_bflb_dvp_raster_crop_hsync
    romapi_bflb_dvp_raster_crop_hsync(dev, start_pixel, end_pixel);
#else
    /* Get start_pixel ~ (end_pixel - 1), not include end_pixel */
    putreg32(start_pixel << 16 | end_pixel, DVP2SRAM0_BASE + DVP2SRAM_D2S_HSYNC_CROP_OFFSET);
    putreg32(start_pixel << 16 | end_pixel, DVP2SRAM1_BASE + DVP2SRAM_D2S_HSYNC_CROP_OFFSET);
#endif
}

int bflb_dvp_raster_feature_control(struct bflb_device_s *dev, int cmd, size_t arg)
{
#ifdef romapi_bflb_dvp_raster_feature_control
    return romapi_bflb_dvp_raster_feature_control(dev, cmd, arg);
#else
    int ret = 0;
    uint32_t regval;

    switch (cmd) {
        case DVP_RASTER_CMD_SET_H_BLANK:
            regval = getreg32(BLOCK2RASTER_BASE + BLOCK2RASTER_B2R_FRAME_CFG1_OFFSET);
            regval &= ~BLOCK2RASTER_REG_W_BLANK_MASK;
            regval |= ((arg & 0xffff) << BLOCK2RASTER_REG_W_BLANK_SHIFT);
            putreg32(regval, BLOCK2RASTER_BASE + BLOCK2RASTER_B2R_FRAME_CFG1_OFFSET);
            break;

        case DVP_RASTER_CMD_SET_V_BLANK:
            regval = getreg32(BLOCK2RASTER_BASE + BLOCK2RASTER_B2R_FRAME_CFG1_OFFSET);
            regval &= ~BLOCK2RASTER_REG_H_BLANK_MASK;
            regval |= ((arg & 0xffff) << BLOCK2RASTER_REG_H_BLANK_SHIFT);
            putreg32(regval, BLOCK2RASTER_BASE + BLOCK2RASTER_B2R_FRAME_CFG1_OFFSET);
            break;

        default:
            ret = -EPERM;
            break;
    }

    return ret;
#endif
}
