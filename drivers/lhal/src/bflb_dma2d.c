#include "bflb_dma2d.h"
#include "hardware/dma2d_reg.h"

#define DMA2D_CHANNEL_MAX 2

struct bflb_dma2d_irq_callback {
    void (*handler)(void *arg);
    void *arg;
};

const uint32_t dma2d_base[] = { 0x20005000 };
struct bflb_dma2d_irq_callback dma2d_callback[1][DMA2D_CHANNEL_MAX];

void dma2d_isr(int irq, void *arg)
{
    uint32_t regval;

    regval = getreg32(dma2d_base[0] + DMA2D_DMA_INTSTATUS_OFFSET);
    putreg32(regval, dma2d_base[0] + DMA2D_DMA_INTTCCLEAR_OFFSET);

    for (uint8_t i = 0; i < DMA2D_CHANNEL_MAX; i++) {
        if (regval & (1 << i)) {
            dma2d_callback[0][i].handler(dma2d_callback[0][i].arg);
        }
    }
}

void bflb_dma2d_channel_init(struct bflb_device_s *dev, struct bflb_dma2d_channel_config_s *config)
{
    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_DMA2D_BURST_COUNT(config->control.bits.src_burst));
    LHAL_PARAM_ASSERT(IS_DMA2D_BURST_COUNT(config->control.bits.dst_burst));
    LHAL_PARAM_ASSERT(IS_DMA2D_DATA_WIDTH(config->control.bits.src_data_width));
    LHAL_PARAM_ASSERT(IS_DMA2D_DATA_WIDTH(config->control.bits.dst_data_width));

#ifdef romapi_bflb_dma2d_channel_init
    romapi_bflb_dma2d_channel_init(dev, config);
#else
    uint32_t regval;
    uint32_t channel_base;

    channel_base = dev->reg_base;

    /* dma2d global enable */
    regval = getreg32(dma2d_base[dev->idx] + DMA2D_DMA_CONFIG_OFFSET);
    regval |= DMA2D_E;
    putreg32(regval, dma2d_base[dev->idx] + DMA2D_DMA_CONFIG_OFFSET);

    /* dma2d channel disable */
    regval = getreg32(channel_base + DMA2D_Cx_CFG_OFFSET);
    regval &= ~DMA2D_CH_EN;
    putreg32(regval, channel_base + DMA2D_Cx_CFG_OFFSET);

    putreg32(config->src_addr, channel_base + DMA2D_CxSRCADDR_OFFSET);
    putreg32(config->dst_addr, channel_base + DMA2D_CxDSTADDR_OFFSET);
    putreg32(config->next_lli_addr, channel_base + DMA2D_CxLLI_OFFSET);
    putreg32(config->control.word, channel_base + DMA2D_Cx_BUS_OFFSET);
    if (config->control.bits.transfer_size) {
        /* normal DMA mode */
        regval &= ~DMA2D_REG_DMA_2D_EN;
        putreg32(regval, channel_base + DMA2D_Cx_CFG_OFFSET);
    } else {
        /* DMA2D mode */
        regval |= DMA2D_REG_DMA_2D_EN;
        putreg32(regval, channel_base + DMA2D_Cx_CFG_OFFSET);
        putreg32(config->loop_step.src_cnt_x | (config->loop_step.src_cnt_y << 16), channel_base + DMA2D_Cx_SRC_CNT_OFFSET);
        putreg32(config->loop_step.src_incr_x, channel_base + DMA2D_Cx_SRC_XIC_OFFSET);
        putreg32(config->loop_step.src_incr_y, channel_base + DMA2D_Cx_SRC_YIC_OFFSET);
        putreg32(config->loop_step.dst_cnt_x, channel_base + DMA2D_Cx_DST_CNT_OFFSET);
        putreg32(config->loop_step.dst_incr_x, channel_base + DMA2D_Cx_DST_XIC_OFFSET);
        putreg32(config->loop_step.dst_incr_y, channel_base + DMA2D_Cx_DST_YIC_OFFSET);
    }

    /* clear irq status */
    putreg32(1 << dev->sub_idx, dma2d_base[dev->idx] + DMA2D_DMA_INTTCCLEAR_OFFSET);
#endif
}

void bflb_dma2d_channel_deinit(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_dma2d_channel_deinit
    romapi_bflb_dma2d_channel_deinit(dev);
#else
    bflb_dma2d_channel_stop(dev);
    bflb_dma2d_channel_tcint_mask(dev, true);
#endif
}

void bflb_dma2d_channel_start(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_dma2d_channel_start
    romapi_bflb_dma2d_channel_start(dev);
#else
    uint32_t regval;
    uint32_t channel_base;

    channel_base = dev->reg_base;

    /* dma2d channel enable */
    regval = getreg32(channel_base + DMA2D_Cx_CFG_OFFSET);
    regval |= DMA2D_CH_EN;
    putreg32(regval, channel_base + DMA2D_Cx_CFG_OFFSET);
#endif
}

void bflb_dma2d_channel_stop(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_dma2d_channel_stop
    romapi_bflb_dma2d_channel_stop(dev);
#else
    uint32_t regval;
    uint32_t channel_base;

    channel_base = dev->reg_base;

    /* dma2d channel disable */
    regval = getreg32(channel_base + DMA2D_Cx_CFG_OFFSET);
    regval &= ~DMA2D_CH_EN;
    putreg32(regval, channel_base + DMA2D_Cx_CFG_OFFSET);
#endif
}

bool bflb_dma2d_channel_isbusy(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_dma2d_channel_isbusy
    return romapi_bflb_dma2d_channel_isbusy(dev);
#else
    uint32_t regval;
    uint32_t channel_base;

    channel_base = dev->reg_base;

    regval = getreg32(channel_base + DMA2D_Cx_CFG_OFFSET);
    if (regval & DMA2D_CH_EN) {
        return true;
    } else {
        return false;
    }
#endif
}

void bflb_dma2d_channel_tcint_mask(struct bflb_device_s *dev, bool mask)
{
#ifdef romapi_bflb_dma2d_channel_tcint_mask
    romapi_bflb_dma2d_channel_tcint_mask(dev, mask);
#else
    uint32_t regval;
    uint32_t channel_base;

    channel_base = dev->reg_base;

    if (mask) {
        regval = getreg32(channel_base + DMA2D_Cx_BUS_OFFSET);
        regval &= ~DMA2D_I;
        putreg32(regval, channel_base + DMA2D_Cx_BUS_OFFSET);
    } else {
        regval = getreg32(channel_base + DMA2D_Cx_BUS_OFFSET);
        regval |= DMA2D_I;
        putreg32(regval, channel_base + DMA2D_Cx_BUS_OFFSET);
    }
#endif
}

void bflb_dma2d_channel_irq_attach(struct bflb_device_s *dev, void (*callback)(void *arg), void *arg)
{
    dma2d_callback[dev->idx][dev->sub_idx].handler = callback;
    dma2d_callback[dev->idx][dev->sub_idx].arg = arg;

    if (dev->idx == 0) {
        bflb_irq_attach(dev->irq_num, dma2d_isr, NULL);
        bflb_irq_enable(dev->irq_num);
    }
}

void bflb_dma2d_channel_irq_detach(struct bflb_device_s *dev)
{
    dma2d_callback[dev->idx][dev->sub_idx].handler = NULL;
    dma2d_callback[dev->idx][dev->sub_idx].arg = NULL;
}

bool bflb_dma2d_channel_get_tcint_status(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_dma2d_channel_get_tcint_status
    return romapi_bflb_dma2d_channel_get_tcint_status(dev);
#else
    uint32_t regval;

    regval = getreg32(dma2d_base[dev->idx] + DMA2D_DMA_INTSTATUS_OFFSET);
    if (regval & (1 << dev->sub_idx)) {
        return true;
    } else {
        return false;
    }
#endif
}

void bflb_dma2d_channel_tcint_clear(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_dma2d_channel_tcint_clear
    romapi_bflb_dma2d_channel_tcint_clear(dev);
#else
    putreg32(1 << dev->sub_idx, dma2d_base[dev->idx] + DMA2D_DMA_INTTCCLEAR_OFFSET);
#endif
}

void bflb_dma2d_image_geometric_transfor_calculate(struct bflb_device_s *dev, struct bflb_dma2d_channel_config_s *config,
                                                   struct bflb_dma2d_image_s *image, uint32_t mode)
{
    LHAL_PARAM_ASSERT(IS_DMA2D_IMAGE_GEOMETRIC_TRANSFOR(mode));

#ifdef romapi_bflb_dma2d_image_geometric_transfor_calculate
    romapi_bflb_dma2d_image_geometric_transfor_calculate(dev, config, image, mode);
#else
    uint32_t regval;
    uint32_t src_x_start = (image->src_x_start <= image->src_x_end) ? image->src_x_start : image->src_x_end;
    uint32_t src_x_end = (image->src_x_start <= image->src_x_end) ? image->src_x_end : image->src_x_start;
    uint32_t src_y_start = (image->src_y_start <= image->src_y_end) ? image->src_y_start : image->src_y_end;
    uint32_t src_y_end = (image->src_y_start <= image->src_y_end) ? image->src_y_end : image->src_y_start;
    uint32_t src_x_width = src_x_end - src_x_start;
    uint32_t src_y_width = src_y_end - src_y_start;
    uint32_t dst_image_addr = image->dst_image_addr;

    if (mode != DMA2D_IMAGE_TRANSLATE && mode != DMA2D_IMAGE_FLIP_VERTICAL && mode != DMA2D_IMAGE_FILL &&
        (image->pixel_data_width == 1 || image->pixel_data_width == 2 || image->pixel_data_width == 4)) {
        config->control.bits.src_data_width = image->pixel_data_width / 2;
        config->control.bits.dst_data_width = image->pixel_data_width / 2;
        regval = image->pixel_data_width;
    } else {
        config->control.bits.src_data_width = image->transfer_data_width;
        config->control.bits.dst_data_width = image->transfer_data_width;
        regval = 1 << image->transfer_data_width;
    }

    if (mode != DMA2D_IMAGE_ROTATE_DEGREE_90 && mode != DMA2D_IMAGE_ROTATE_DEGREE_180 && mode != DMA2D_IMAGE_ROTATE_DEGREE_270) {
        if (image->src_x_start > image->src_x_end && image->src_y_start <= image->src_y_end) {
            dst_image_addr = image->dst_image_addr - src_x_width * image->pixel_data_width;
        } else if (image->src_x_start <= image->src_x_end && image->src_y_start > image->src_y_end) {
            dst_image_addr = image->dst_image_addr - src_y_width * image->pixel_data_width * image->src_image_width;
        } else if (image->src_x_start > image->src_x_end && image->src_y_start > image->src_y_end) {
            dst_image_addr = image->dst_image_addr - src_y_width * image->pixel_data_width * image->src_image_width - src_x_width * image->pixel_data_width;
        }
    }

    config->src_addr = image->src_image_addr + image->src_image_width * image->pixel_data_width * src_y_start + image->pixel_data_width * src_x_start;
    config->dst_addr = dst_image_addr + image->dst_image_width * image->pixel_data_width * image->dst_y_start + image->pixel_data_width * image->dst_x_start;

    config->loop_step.src_cnt_x = src_x_width * image->pixel_data_width / regval;
    config->loop_step.src_incr_x = regval;
    config->loop_step.src_cnt_y = src_y_width;
    config->loop_step.src_incr_y = (image->src_image_width - src_x_width) * image->pixel_data_width + regval;
    config->loop_step.dst_cnt_x = src_x_width * image->pixel_data_width / regval;
    config->loop_step.dst_incr_x = regval;
    config->loop_step.dst_incr_y = (image->dst_image_width - src_x_width) * image->pixel_data_width + regval;

    if (mode == DMA2D_IMAGE_FLIP_HORIZONTAL) {
        config->dst_addr += src_x_width * image->pixel_data_width - regval;
        config->loop_step.dst_incr_x = -regval;
        config->loop_step.dst_incr_y = (image->dst_image_width + src_x_width) * image->pixel_data_width - regval;
    } else if (mode == DMA2D_IMAGE_FLIP_VERTICAL) {
        config->dst_addr += (src_y_width - 1) * image->pixel_data_width * image->dst_image_width;
        config->loop_step.dst_incr_y = regval - (image->dst_image_width + src_x_width) * image->pixel_data_width;
    } else if (mode == DMA2D_IMAGE_ROTATE_DEGREE_90) {
        config->dst_addr += (src_y_width - 1) * regval;
        config->loop_step.dst_incr_x = image->dst_image_width * image->pixel_data_width;
        config->loop_step.dst_incr_y = -(src_x_width * image->pixel_data_width / regval * image->pixel_data_width * image->dst_image_width) + image->pixel_data_width * image->dst_image_width - regval;
    } else if (mode == DMA2D_IMAGE_ROTATE_DEGREE_180) {
        config->dst_addr += (src_y_width - 1) * regval * image->dst_image_width + (src_x_width - 1) * regval;
        config->loop_step.dst_incr_x = -regval;
        config->loop_step.dst_incr_y = -image->dst_image_width * image->pixel_data_width + src_x_width * image->pixel_data_width - regval;
    } else if (mode == DMA2D_IMAGE_ROTATE_DEGREE_270) {
        config->dst_addr += (src_x_width - 1) * regval * image->dst_image_width;
        config->loop_step.dst_incr_x = -image->dst_image_width * image->pixel_data_width;
        config->loop_step.dst_incr_y = src_x_width * image->pixel_data_width / regval * image->pixel_data_width * image->dst_image_width - image->pixel_data_width * image->dst_image_width + regval;
    } else if (mode == DMA2D_IMAGE_FILL) {
        config->src_addr = image->src_image_addr;
        config->loop_step.src_incr_x = 0;
        config->loop_step.src_incr_y = 0;
    }
#endif
}

void bflb_dma2d_channel_enable_color_key(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_dma2d_channel_enable_color_key
    romapi_bflb_dma2d_channel_enable_color_key(dev);
#else
    uint32_t regval;
    uint32_t channel_base;

    channel_base = dev->reg_base;
    regval = getreg32(channel_base + DMA2D_Cx_KEY_EN_OFFSET);
    regval |= DMA2D_KEY_EN;
    putreg32(regval, channel_base + DMA2D_Cx_KEY_EN_OFFSET);
#endif
}

void bflb_dma2d_channel_disable_color_key(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_dma2d_channel_disable_color_key
    romapi_bflb_dma2d_channel_disable_color_key(dev);
#else
    uint32_t regval;
    uint32_t channel_base;

    channel_base = dev->reg_base;
    regval = getreg32(channel_base + DMA2D_Cx_KEY_EN_OFFSET);
    regval &= ~DMA2D_KEY_EN;
    putreg32(regval, channel_base + DMA2D_Cx_KEY_EN_OFFSET);
#endif
}

void bflb_dma2d_channel_set_color_key(struct bflb_device_s *dev, uint8_t mode, uint32_t value, uint8_t mask)
{
    LHAL_PARAM_ASSERT(IS_DMA2D_COLOR_KEY_MODE(mode));
    LHAL_PARAM_ASSERT(IS_DMA2D_COLOR_KEY_MASK(mask));

#ifdef romapi_bflb_dma2d_channel_set_color_key
    romapi_bflb_dma2d_channel_set_color_key(dev, mode, value, mask);
#else
    uint32_t regval;
    uint32_t channel_base;

    /* Set color key, for example:
       mode = DMA2D_COLOR_KEY_MODE_32BIT;
       value = 0x12345678;
       mask = DMA2D_COLOR_KEY_MASK_0X000000FF | DMA2D_COLOR_KEY_MASK_0X0000FF00;
       When the source address 32-bit value n & (0x000000ff | 0x0000ff00) = 0x12345678 & (0x000000ff | 0x0000ff00) = 0x5678,
       DMA2D will skip transferring this 32-bit value, meaning the corresponding destination address 32-bit value remains unchanged.
    */

    channel_base = dev->reg_base;
    putreg32(value, channel_base + DMA2D_Cx_KEY_OFFSET);
    regval = getreg32(channel_base + DMA2D_Cx_KEY_EN_OFFSET);
    regval &= ~(DMA2D_KEY_MODE_MASK | DMA2D_KEY_STRB_MASK);
    regval |= mode << DMA2D_KEY_MODE_SHIFT;
    regval |= mask << DMA2D_KEY_STRB_SHIFT;
    putreg32(regval, channel_base + DMA2D_Cx_KEY_EN_OFFSET);
#endif
}

int bflb_dma2d_feature_control(struct bflb_device_s *dev, int cmd, size_t arg)
{
#ifdef romapi_bflb_dma2d_feature_control
    return romapi_bflb_dma2d_feature_control(dev, cmd, arg);
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
