#include "bflb_osd.h"
#include "hardware/osd_blend_reg.h"
#include "hardware/osd_draw_reg.h"

#define OSD_DRAW_REG_OFFSET (0x400)
#define OSD_NOP                  \
    do {                         \
        __asm__ volatile("nop"); \
    } while (0)

void bflb_osd_blend_init(struct bflb_device_s *dev, struct bflb_osd_blend_config_s *config)
{
    LHAL_PARAM_ASSERT(dev);

#ifdef romapi_bflb_osd_blend_init
    romapi_bflb_osd_blend_init(dev, config);
#else
    uint32_t regval;
    uint32_t reg_base;
    uint32_t width;

    reg_base = dev->reg_base;

    /* Disable osd blend */
    regval = getreg32(reg_base + OSD_BLEND_OBND_MEM_CONFIG0_OFFSET);
    regval &= ~(OSD_BLEND_OBND_LAYER_EN);
    putreg32(regval, reg_base + OSD_BLEND_OBND_MEM_CONFIG0_OFFSET);

    regval = getreg32(reg_base + OSD_BLEND_OBND_LAYER_CONFIG0_OFFSET);
    regval &= ~(OSD_BLEND_OBND_COLOR_FORMAT_MASK | OSD_BLEND_OBND_ORDER_A_MASK | OSD_BLEND_OBND_ORDER_RV_MASK | OSD_BLEND_OBND_ORDER_GY_MASK | OSD_BLEND_OBND_ORDER_BU_MASK);
    regval |= ((config->blend_format << OSD_BLEND_OBND_COLOR_FORMAT_SHIFT) & OSD_BLEND_OBND_COLOR_FORMAT_MASK);
    regval |= ((config->order_a << OSD_BLEND_OBND_ORDER_A_SHIFT) & OSD_BLEND_OBND_ORDER_A_MASK);
    regval |= ((config->order_rv << OSD_BLEND_OBND_ORDER_RV_SHIFT) & OSD_BLEND_OBND_ORDER_RV_MASK);
    regval |= ((config->order_gy << OSD_BLEND_OBND_ORDER_GY_SHIFT) & OSD_BLEND_OBND_ORDER_GY_MASK);
    regval |= ((config->order_bu << OSD_BLEND_OBND_ORDER_BU_SHIFT) & OSD_BLEND_OBND_ORDER_BU_MASK);
    putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG0_OFFSET);

    putreg32(((config->coor.end_x - 1) << OSD_BLEND_OBND_X_MAX_SHIFT) | (config->coor.start_x), reg_base + OSD_BLEND_OBND_LAYER_XCONFIG_OFFSET);
    putreg32(((config->coor.end_y - 1) << OSD_BLEND_OBND_Y_MAX_SHIFT) | (config->coor.start_y), reg_base + OSD_BLEND_OBND_LAYER_YCONFIG_OFFSET);
    putreg32(config->layer_buffer_addr, reg_base + OSD_BLEND_OBND_MEM_CONFIG1_OFFSET);
    putreg32(200, reg_base + OSD_BLEND_OBND_SH_OFFSET);
    putreg32(OSD_BLEND_REG_RFIFO_DRAIN_MASK_W, reg_base + OSD_BLEND_OBND_ERROR_OFFSET);

    width = config->coor.end_x - config->coor.start_x;
    switch (config->blend_format) {
        case OSD_BLEND_FORMAT_ARGB8888:
        /* same as OSD_BLEND_FORMAT_AYUV8888 */
        case OSD_BLEND_FORMAT_AYUV8888:
            width = width * 4;
            break;

        case OSD_BLEND_FORMAT_ARGB4444:
        /* same as OSD_BLEND_FORMAT_YUV655 */
        case OSD_BLEND_FORMAT_AYUV4444:
        /* same as OSD_BLEND_FORMAT_YUV655 */
        case OSD_BLEND_FORMAT_ARGB1555:
        /* same as OSD_BLEND_FORMAT_YUV655 */
        case OSD_BLEND_FORMAT_AYUV1555:
        /* same as OSD_BLEND_FORMAT_YUV655 */
        case OSD_BLEND_FORMAT_RGB565:
        /* same as OSD_BLEND_FORMAT_YUV655 */
        case OSD_BLEND_FORMAT_YUV655:
            width = width * 2;
            break;

        case OSD_BLEND_FORMAT_A8_GLOBAL_RGB:
        /* same as OSD_BLEND_FORMAT_AYUV_PALETTE_8BIT */
        case OSD_BLEND_FORMAT_A8_GLOBAL_YUV:
        /* same as OSD_BLEND_FORMAT_AYUV_PALETTE_8BIT */
        case OSD_BLEND_FORMAT_ARGB_PALETTE_8BIT:
        /* same as OSD_BLEND_FORMAT_AYUV_PALETTE_8BIT */
        case OSD_BLEND_FORMAT_AYUV_PALETTE_8BIT:
            break;

        case OSD_BLEND_FORMAT_ARGB_PALETTE_4BIT:
        /* same as OSD_BLEND_FORMAT_AYUV_PALETTE_4BIT */
        case OSD_BLEND_FORMAT_AYUV_PALETTE_4BIT:
            width = width / 2;
            break;

        case OSD_BLEND_FORMAT_ARGB_PALETTE_2BIT:
        /* same as OSD_BLEND_FORMAT_AYUV_PALETTE_2BIT */
        case OSD_BLEND_FORMAT_AYUV_PALETTE_2BIT:
            width = width / 4;
            break;

        case OSD_BLEND_FORMAT_ARGB_PALETTE_1BIT:
        /* same as OSD_BLEND_FORMAT_AYUV_PALETTE_1BIT */
        case OSD_BLEND_FORMAT_AYUV_PALETTE_1BIT:
            width = width / 8;
            break;

        default:
            width = width * 2;
            break;
    }
    regval = getreg32(reg_base + OSD_BLEND_OBND_MEM_CONFIG2_OFFSET);
    regval &= ~OSD_BLEND_REG_FRAME_WIDTH_BYTE_X8_MASK;
    regval |= (width / 8) << OSD_BLEND_REG_FRAME_WIDTH_BYTE_X8_SHIFT;
    putreg32(regval, reg_base + OSD_BLEND_OBND_MEM_CONFIG2_OFFSET);
    regval = getreg32(reg_base + OSD_BLEND_OBND_MEM_CONFIG3_OFFSET);
    regval &= ~OSD_BLEND_REG_FRAME_HEIGHT_MASK;
    regval |= (config->coor.end_y - config->coor.start_y) << OSD_BLEND_REG_FRAME_HEIGHT_SHIFT;
    putreg32(regval, reg_base + OSD_BLEND_OBND_MEM_CONFIG3_OFFSET);
#endif
}

void bflb_osd_blend_enable(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_osd_blend_enable
    romapi_bflb_osd_blend_enable(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    /* Set force shadow before enable */
    regval = getreg32(reg_base + OSD_BLEND_OBND_MEM_CONFIG0_OFFSET);
    putreg32(regval | OSD_BLEND_OBND_FORCE_SH, reg_base + OSD_BLEND_OBND_MEM_CONFIG0_OFFSET);
    OSD_NOP;
    regval |= OSD_BLEND_OBND_LAYER_EN;
    putreg32(regval, reg_base + OSD_BLEND_OBND_MEM_CONFIG0_OFFSET);

    /* Enable main switch of osd */
    regval = getreg32(reg_base + OSD_DRAW_REG_OFFSET + OSD_DRAW_OSD_BLEND_EN_OFFSET);
    regval |= OSD_DRAW_CR_BLEND_EN;
    putreg32(regval, reg_base + OSD_DRAW_REG_OFFSET + OSD_DRAW_OSD_BLEND_EN_OFFSET);
#endif
}

void bflb_osd_blend_disable(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_osd_blend_disable
    romapi_bflb_osd_blend_disable(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + OSD_BLEND_OBND_MEM_CONFIG0_OFFSET);
    regval &= ~OSD_BLEND_OBND_LAYER_EN;
    putreg32(regval, reg_base + OSD_BLEND_OBND_MEM_CONFIG0_OFFSET);
#endif
}

void bflb_osd_blend_set_coordinate(struct bflb_device_s *dev, struct bflb_osd_coordinate_s *coor)
{
#ifdef romapi_bflb_osd_blend_set_coordinate
    romapi_bflb_osd_blend_set_coordinate(dev, coor);
#else
    putreg32(((coor->end_x - 1) << OSD_BLEND_OBND_X_MAX_SHIFT) | (coor->start_x), dev->reg_base + OSD_BLEND_OBND_LAYER_XCONFIG_OFFSET);
    putreg32(((coor->end_y - 1) << OSD_BLEND_OBND_Y_MAX_SHIFT) | (coor->start_y), dev->reg_base + OSD_BLEND_OBND_LAYER_YCONFIG_OFFSET);
#endif
}

void bflb_osd_blend_set_global_a(struct bflb_device_s *dev, bool enable, uint8_t value)
{
#ifdef romapi_bflb_osd_blend_set_global_a
    romapi_bflb_osd_blend_set_global_a(dev, enable, value);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + OSD_BLEND_OBND_LAYER_CONFIG0_OFFSET);
    regval &= ~OSD_BLEND_OBND_GLOBAL_A_MASK;
    regval |= value << OSD_BLEND_OBND_GLOBAL_A_SHIFT;
    if (enable) {
        regval |= OSD_BLEND_OBND_GLOBAL_A_EN;
    } else {
        regval &= ~OSD_BLEND_OBND_GLOBAL_A_EN;
    }
    putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG0_OFFSET);
#endif
}

void bflb_osd_blend_set_global_rgb_yuv(struct bflb_device_s *dev, bool enable, uint8_t rv, uint8_t gy, uint8_t bu)
{
#ifdef romapi_bflb_osd_blend_set_global_rgb_yuv
    romapi_bflb_osd_blend_set_global_rgb_yuv(dev, enable, rv, gy, bu);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + OSD_BLEND_OBND_LAYER_CONFIG1_OFFSET);
    regval &= ~(OSD_BLEND_OBND_GLOBAL_RV_MASK | OSD_BLEND_OBND_GLOBAL_GY_MASK | OSD_BLEND_OBND_GLOBAL_BU_MASK);
    regval |= rv << OSD_BLEND_OBND_GLOBAL_RV_SHIFT;
    regval |= gy << OSD_BLEND_OBND_GLOBAL_GY_SHIFT;
    regval |= bu << OSD_BLEND_OBND_GLOBAL_BU_SHIFT;
    if (enable) {
        regval |= OSD_BLEND_OBND_GLOBAL_COLOR_EN;
    } else {
        regval &= ~OSD_BLEND_OBND_GLOBAL_COLOR_EN;
    }
    putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG1_OFFSET);
#endif
}

void bflb_osd_blend_set_palette_color(struct bflb_device_s *dev, uint8_t index, uint32_t value)
{
#ifdef romapi_bflb_osd_blend_set_palette_color
    romapi_bflb_osd_blend_set_palette_color(dev, index, value);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    /* Disable blend before setting palette */
    regval = getreg32(reg_base + OSD_BLEND_OBND_MEM_CONFIG0_OFFSET);
    regval &= ~OSD_BLEND_OBND_LAYER_EN;
    putreg32(regval, reg_base + OSD_BLEND_OBND_MEM_CONFIG0_OFFSET);

    putreg32(value, reg_base + OSD_BLEND_OBND_LAYER_CONFIG7_OFFSET);
    regval = getreg32(reg_base + OSD_BLEND_OBND_LAYER_CONFIG6_OFFSET);
    regval &= ~OSD_BLEND_OBND_UPDATE_INDEX_MASK;
    regval |= (index << OSD_BLEND_OBND_UPDATE_INDEX_SHIFT);
    putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG6_OFFSET);
    OSD_NOP;
    regval |= OSD_BLEND_OBND_UPDATE_TRIGGER;
    putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG6_OFFSET);
#endif
}

void bflb_osd_blend_replace_palette_index(struct bflb_device_s *dev, bool enable, struct bflb_osd_blend_replace_s *replace)
{
#ifdef romapi_bflb_osd_blend_replace_palette_index
    romapi_bflb_osd_blend_replace_palette_index(dev, enable, replace);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    if (replace != NULL) {
        /* Disable first */
        regval = getreg32(reg_base + OSD_BLEND_OBND_LAYER_CONFIG2_OFFSET);
        regval &= ~OSD_BLEND_OBND_KEY_PALETTE_EN;
        putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG2_OFFSET);

        regval = replace->min_value << OSD_BLEND_OBND_KEY_INDEX_MIN_SHIFT;
        regval |= replace->max_value << OSD_BLEND_OBND_KEY_INDEX_MAX_SHIFT;
        regval |= replace->target_value << OSD_BLEND_OBND_KEY_REPLACE_INDEX_SHIFT;
        if (replace->replace_between) {
            regval &= ~OSD_BLEND_OBND_KEY_INDEX_INV;
        } else {
            regval |= OSD_BLEND_OBND_KEY_INDEX_INV;
        }
        putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG2_OFFSET);
    }

    regval = getreg32(reg_base + OSD_BLEND_OBND_LAYER_CONFIG2_OFFSET);
    if (enable) {
        regval |= OSD_BLEND_OBND_KEY_PALETTE_EN;
    } else {
        regval &= ~OSD_BLEND_OBND_KEY_PALETTE_EN;
    }
    putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG2_OFFSET);
#endif
}

void bflb_osd_blend_replace_color_value(struct bflb_device_s *dev, bool enable, struct bflb_osd_blend_replace_s *replace_a,
                                        struct bflb_osd_blend_replace_s *replace_rv, struct bflb_osd_blend_replace_s *replace_gy, struct bflb_osd_blend_replace_s *replace_bu)
{
#ifdef romapi_bflb_osd_blend_replace_color_value
    romapi_bflb_osd_blend_replace_color_value(dev, enable, replace_a, replace_rv, replace_gy, replace_bu);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    if (replace_a != NULL || replace_rv != NULL || replace_gy != NULL || replace_bu != NULL) {
        /* Disable first */
        regval = getreg32(reg_base + OSD_BLEND_OBND_LAYER_CONFIG6_OFFSET);
        regval &= ~OSD_BLEND_OBND_KEY_COLOR_EN;
        putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG6_OFFSET);
    }

    if (replace_a != NULL) {
        regval = getreg32(reg_base + OSD_BLEND_OBND_LAYER_CONFIG3_OFFSET);
        regval &= ~(OSD_BLEND_OBND_KEY_A_MIN_MASK | OSD_BLEND_OBND_KEY_A_MAX_MASK);
        regval |= replace_a->min_value << OSD_BLEND_OBND_KEY_A_MIN_SHIFT;
        regval |= replace_a->max_value << OSD_BLEND_OBND_KEY_A_MAX_SHIFT;
        putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG3_OFFSET);
        regval = getreg32(reg_base + OSD_BLEND_OBND_LAYER_CONFIG5_OFFSET);
        regval &= ~OSD_BLEND_OBND_KEY_REPLACE_A_MASK;
        regval |= replace_a->target_value << OSD_BLEND_OBND_KEY_REPLACE_A_SHIFT;
        putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG5_OFFSET);
        regval = getreg32(reg_base + OSD_BLEND_OBND_LAYER_CONFIG6_OFFSET);
        if (replace_a->replace_between) {
            regval &= ~OSD_BLEND_OBND_KEY_A_INV;
        } else {
            regval |= OSD_BLEND_OBND_KEY_A_INV;
        }
        putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG6_OFFSET);
    }

    if (replace_rv != NULL) {
        regval = getreg32(reg_base + OSD_BLEND_OBND_LAYER_CONFIG3_OFFSET);
        regval &= ~(OSD_BLEND_OBND_KEY_RV_MIN_MASK | OSD_BLEND_OBND_KEY_RV_MAX_MASK);
        regval |= replace_rv->min_value << OSD_BLEND_OBND_KEY_RV_MIN_SHIFT;
        regval |= replace_rv->max_value << OSD_BLEND_OBND_KEY_RV_MAX_SHIFT;
        putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG3_OFFSET);
        regval = getreg32(reg_base + OSD_BLEND_OBND_LAYER_CONFIG5_OFFSET);
        regval &= ~OSD_BLEND_OBND_KEY_REPLACE_RV_MASK;
        regval |= replace_rv->target_value << OSD_BLEND_OBND_KEY_REPLACE_RV_SHIFT;
        putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG5_OFFSET);
        regval = getreg32(reg_base + OSD_BLEND_OBND_LAYER_CONFIG6_OFFSET);
        if (replace_rv->replace_between) {
            regval &= ~OSD_BLEND_OBND_KEY_RV_INV;
        } else {
            regval |= OSD_BLEND_OBND_KEY_RV_INV;
        }
        putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG6_OFFSET);
    }

    if (replace_gy != NULL) {
        regval = getreg32(reg_base + OSD_BLEND_OBND_LAYER_CONFIG4_OFFSET);
        regval &= ~(OSD_BLEND_OBND_KEY_GY_MIN_MASK | OSD_BLEND_OBND_KEY_GY_MAX_MASK);
        regval |= replace_gy->min_value << OSD_BLEND_OBND_KEY_GY_MIN_SHIFT;
        regval |= replace_gy->max_value << OSD_BLEND_OBND_KEY_GY_MAX_SHIFT;
        putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG4_OFFSET);
        regval = getreg32(reg_base + OSD_BLEND_OBND_LAYER_CONFIG5_OFFSET);
        regval &= ~OSD_BLEND_OBND_KEY_REPLACE_GY_MASK;
        regval |= replace_gy->target_value << OSD_BLEND_OBND_KEY_REPLACE_GY_SHIFT;
        putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG5_OFFSET);
        regval = getreg32(reg_base + OSD_BLEND_OBND_LAYER_CONFIG6_OFFSET);
        if (replace_gy->replace_between) {
            regval &= ~OSD_BLEND_OBND_KEY_GY_INV;
        } else {
            regval |= OSD_BLEND_OBND_KEY_GY_INV;
        }
        putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG6_OFFSET);
    }

    if (replace_bu != NULL) {
        regval = getreg32(reg_base + OSD_BLEND_OBND_LAYER_CONFIG4_OFFSET);
        regval &= ~(OSD_BLEND_OBND_KEY_BU_MIN_MASK | OSD_BLEND_OBND_KEY_BU_MAX_MASK);
        regval |= replace_bu->min_value << OSD_BLEND_OBND_KEY_BU_MIN_SHIFT;
        regval |= replace_bu->max_value << OSD_BLEND_OBND_KEY_BU_MAX_SHIFT;
        putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG4_OFFSET);
        regval = getreg32(reg_base + OSD_BLEND_OBND_LAYER_CONFIG5_OFFSET);
        regval &= ~OSD_BLEND_OBND_KEY_REPLACE_BU_MASK;
        regval |= replace_bu->target_value << OSD_BLEND_OBND_KEY_REPLACE_BU_SHIFT;
        putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG5_OFFSET);
        regval = getreg32(reg_base + OSD_BLEND_OBND_LAYER_CONFIG6_OFFSET);
        if (replace_bu->replace_between) {
            regval &= ~OSD_BLEND_OBND_KEY_BU_INV;
        } else {
            regval |= OSD_BLEND_OBND_KEY_BU_INV;
        }
        putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG6_OFFSET);
    }

    regval = getreg32(reg_base + OSD_BLEND_OBND_LAYER_CONFIG6_OFFSET);
    if (enable) {
        regval |= OSD_BLEND_OBND_KEY_COLOR_EN;
    } else {
        regval &= ~OSD_BLEND_OBND_KEY_COLOR_EN;
    }
    putreg32(regval, reg_base + OSD_BLEND_OBND_LAYER_CONFIG6_OFFSET);
#endif
}

void bflb_osd_draw_init(struct bflb_device_s *dev, uint8_t layer, struct bflb_osd_draw_config_s *config)
{
    LHAL_PARAM_ASSERT(dev);

#ifdef romapi_bflb_osd_draw_init
    romapi_bflb_osd_draw_init(dev, layer, config);
#else
    uint32_t regval;
    uint32_t reg_base;

    if (layer > OSD_DRAW_LAYER_15) {
        layer = OSD_DRAW_LAYER_15;
    }

    reg_base = dev->reg_base + OSD_DRAW_REG_OFFSET;

    /* Disable osd draw layer */
    regval = getreg32(reg_base + OSD_DRAW_CONFIG_OFFSET);
    regval &= ~(1 << layer);
    putreg32(regval, reg_base + OSD_DRAW_CONFIG_OFFSET);

    if (config->is_solid) {
        regval |= (1 << (layer + OSD_DRAW_REG_DRAW_TYPE_L_SHIFT));
    } else {
        regval &= ~(1 << (layer + OSD_DRAW_REG_DRAW_TYPE_L_SHIFT));
    }
    putreg32(regval, reg_base + OSD_DRAW_CONFIG_OFFSET);

    regval = config->border_thickness + config->border_thickness % 2;
    regval |= config->color_y << OSD_DRAW_REG_DRAW_Y_00_SHIFT;
    regval |= config->color_u << OSD_DRAW_REG_DRAW_U_00_SHIFT;
    regval |= config->color_v << OSD_DRAW_REG_DRAW_V_00_SHIFT;
    putreg32(regval, reg_base + OSD_DRAW_DRAW_PARTIAL_YUV_CONFIG0_OFFSET + 0xC * layer);

    regval = config->coor.start_x + config->coor.start_x % 2;
    regval |= ((config->coor.end_x | 1) << OSD_DRAW_REG_DRAW_X_MAX_00_SHIFT);
    putreg32(regval, reg_base + OSD_DRAW_DRAW_PARTIAL_X_CONFIG0_OFFSET + 0xC * layer);

    regval = config->coor.start_y + config->coor.start_y % 2;
    regval |= ((config->coor.end_y | 1) << OSD_DRAW_REG_DRAW_Y_MAX_00_SHIFT);
    putreg32(regval, reg_base + OSD_DRAW_DRAW_PARTIAL_Y_CONFIG0_OFFSET + 0xC * layer);
#endif
}

void bflb_osd_draw_enable(struct bflb_device_s *dev, uint8_t layer)
{
#ifdef romapi_bflb_osd_draw_enable
    romapi_bflb_osd_draw_enable(dev, layer);
#else
    uint32_t regval;
    uint32_t reg_base;

    if (layer > OSD_DRAW_LAYER_15) {
        layer = OSD_DRAW_LAYER_15;
    }

    reg_base = dev->reg_base + OSD_DRAW_REG_OFFSET;

    /* Set force shadow before enable */
    regval = getreg32(reg_base + OSD_DRAW_OSD_SW_SHD_OFFSET);
    regval |= OSD_DRAW_CR_SW_SHD;
    putreg32(regval, reg_base + OSD_DRAW_OSD_SW_SHD_OFFSET);
    OSD_NOP;
    regval = getreg32(reg_base + OSD_DRAW_CONFIG_OFFSET);
    regval |= 1 << layer;
    putreg32(regval, reg_base + OSD_DRAW_CONFIG_OFFSET);

    /* Enable main switch of osd */
    regval = getreg32(reg_base + OSD_DRAW_OSD_BLEND_EN_OFFSET);
    regval |= OSD_DRAW_CR_BLEND_EN;
    putreg32(regval, reg_base + OSD_DRAW_OSD_BLEND_EN_OFFSET);
#endif
}

void bflb_osd_draw_disable(struct bflb_device_s *dev, uint8_t layer)
{
#ifdef romapi_bflb_osd_draw_disable
    romapi_bflb_osd_draw_disable(dev, layer);
#else
    uint32_t regval;
    uint32_t reg_base;

    if (layer > OSD_DRAW_LAYER_15) {
        layer = OSD_DRAW_LAYER_15;
    }

    reg_base = dev->reg_base + OSD_DRAW_REG_OFFSET;

    regval = getreg32(reg_base + OSD_DRAW_CONFIG_OFFSET);
    regval &= ~(1 << layer);
    putreg32(regval, reg_base + OSD_DRAW_CONFIG_OFFSET);
#endif
}

void bflb_osd_draw_set_coordinate(struct bflb_device_s *dev, uint8_t layer, struct bflb_osd_coordinate_s *coor)
{
#ifdef romapi_bflb_osd_draw_set_coordinate
    romapi_bflb_osd_draw_set_coordinate(dev, layer, coor);
#else
    uint32_t regval;
    uint32_t reg_base;

    if (layer > OSD_DRAW_LAYER_15) {
        layer = OSD_DRAW_LAYER_15;
    }

    reg_base = dev->reg_base + OSD_DRAW_REG_OFFSET;

    regval = coor->start_x + coor->start_x % 2;
    regval |= ((coor->end_x | 1) << OSD_DRAW_REG_DRAW_X_MAX_00_SHIFT);
    putreg32(regval, reg_base + OSD_DRAW_DRAW_PARTIAL_X_CONFIG0_OFFSET + 0xC * layer);

    regval = coor->start_y + coor->start_y % 2;
    regval |= ((coor->end_y | 1) << OSD_DRAW_REG_DRAW_Y_MAX_00_SHIFT);
    putreg32(regval, reg_base + OSD_DRAW_DRAW_PARTIAL_Y_CONFIG0_OFFSET + 0xC * layer);
#endif
}

void bflb_osd_int_mask(struct bflb_device_s *dev, bool mask)
{
#ifdef romapi_bflb_osd_int_mask
    romapi_bflb_osd_int_mask(dev, mask);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base + OSD_DRAW_REG_OFFSET;
    regval = getreg32(reg_base + OSD_DRAW_OSD_INT_STS_OFFSET);
    if (mask) {
        regval |= OSD_DRAW_REG_OSD_SEOF_INT_MASK_W;
    } else {
        regval &= ~OSD_DRAW_REG_OSD_SEOF_INT_MASK_W;
    }
    putreg32(regval, reg_base + OSD_DRAW_OSD_INT_STS_OFFSET);
#endif
}

bool bflb_osd_get_intstatus(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_osd_get_intstatus
    return romapi_bflb_osd_get_intstatus(dev);
#else
    uint32_t regval;

    regval = getreg32(dev->reg_base + OSD_DRAW_REG_OFFSET + OSD_DRAW_OSD_INT_STS_OFFSET);
    if (regval & OSD_DRAW_STS_OSD_SEOF_INT_R) {
        return true;
    } else {
        return false;
    }
#endif
}

void bflb_osd_int_clear(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_osd_int_clear
    romapi_bflb_osd_int_clear(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base + OSD_DRAW_REG_OFFSET;
    regval = getreg32(reg_base + OSD_DRAW_OSD_INT_STS_OFFSET);
    regval |= OSD_DRAW_REG_OSD_SEOF_INT_CLR_W;
    putreg32(regval, reg_base + OSD_DRAW_OSD_INT_STS_OFFSET);
#endif
}

int bflb_osd_feature_control(struct bflb_device_s *dev, int cmd, size_t arg)
{
#ifdef romapi_bflb_osd_feature_control
    return romapi_bflb_osd_feature_control(dev, cmd, arg);
#else
    int ret = 0;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    switch (cmd) {
        /* Only for color format: ARGB1555, AYUV1555. arg[7:0]: value of alpha 0, arg[15:8]: value of alpha 1 */
        case OSD_CMD_BLEND_SET_1_BIT_ALPHA:
            putreg32(arg, reg_base + OSD_BLEND_OBND_LAYER_CONFIG8_OFFSET);
            break;

        /* Set preloading time of blend, unit: PCLK */
        case OSD_CMD_BLEND_SET_PRELOAD_TIME:
            putreg32(arg, reg_base + OSD_BLEND_OBND_SH_OFFSET);
            break;

        default:
            ret = -EPERM;
            break;
    }

    return ret;
#endif
}
