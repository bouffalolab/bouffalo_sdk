#include "bflb_adc_v2.h"
#include "bflb_efuse.h"
#include "hardware/adc_v2_reg.h"

#define ADC_GPIP_BASE  ((uint32_t)0x20002000)
#define ADC_GPIP2_BASE ((uint32_t)0x20002400)

#if defined(BL616L_VERSION_A0)
volatile uint32_t tsen_offset = 1500;
#else
volatile uint32_t tsen_offset;
#endif
volatile int adc_reference_channel = -1;

void bflb_adc_init(struct bflb_device_s *dev, const struct bflb_adc_config_s *config)
{
#ifdef romapi_bflb_adc_init
    romapi_bflb_adc_init(dev, config);
#else
    uint32_t regval;
    uint32_t reg_base;

    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_ADC_RESOLUTION(config->resolution));
    LHAL_PARAM_ASSERT(IS_ADC_VREF(config->vref));

    reg_base = dev->reg_base;

    /* adc disable */
    regval = getreg32(reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);
    regval &= ~GLB_GPADC1_GLOBAL_EN;
    putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);

    regval = getreg32(reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);
    regval |= GLB_GPADC1_GLOBAL_EN;
    putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);

    /* adc reset */
    regval = getreg32(reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);
    regval |= GLB_GPADC1_SOFT_RST;
    putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);

    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");

    regval = getreg32(reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);
    regval &= ~GLB_GPADC1_SOFT_RST;
    putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);

    regval = 0;
    regval |= (1 << GLB_GPADC_DVDD_SEL_SHIFT);
    regval |= (0xF << GLB_GPADC_INT_POS_SEL_SHIFT);
    regval |= (0xF << GLB_GPADC_INT_NEG_SEL_SHIFT);
    putreg32(regval, reg_base + GLB_GPADC_REG_CMD1_OFFSET);

    regval = 0;
    regval |= (3 << GLB_GPADC1_PGA_VCM_SHIFT); /* 0.9V */
    regval |= (1 << GLB_GPADC1_PGA_GAIN_SHIFT); /* gain 1 */
    regval |= GLB_GPADC1_PGA_EN;
    regval |= (2 << GLB_GPADC1_CHOP_MODE_SHIFT); /* Vref AZ and PGA chop on */
    if (config->differential_mode) {
        regval |= GLB_GPADC1_DIFF_MODE;
        regval &= ~GLB_GPADC1_NEG_GND;
    } else {
        regval &= ~GLB_GPADC1_DIFF_MODE;
        regval |= GLB_GPADC1_NEG_GND;
    }
#if defined(BL616L_VERSION_A0)
    if (config->vref == ADC_VREF_INTERNAL_1P25) {
#else
    if (config->vref == ADC_VREF_EXTERNAL_1P25) {
#endif
        regval |= GLB_GPADC1_VREF_EX_SEL;
    }
    putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG1_OFFSET);

    regval = getreg32(reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);
    if (config->scan_conv_mode && config->continuous_conv_mode) {
        regval |= GLB_GPADC1_SCN_LOOP_EN;
        regval |= GLB_GPADC1_SCN_EN;
        regval &= ~GLB_GPADC1_CONT_CONV_EN;
    } else if (config->scan_conv_mode && !(config->continuous_conv_mode)) {
        regval &= ~GLB_GPADC1_SCN_LOOP_EN;
        regval |= GLB_GPADC1_SCN_EN;
        regval &= ~GLB_GPADC1_CONT_CONV_EN;
    } else if (!(config->scan_conv_mode) && config->continuous_conv_mode) {
        regval &= ~GLB_GPADC1_SCN_LOOP_EN;
        regval &= ~GLB_GPADC1_SCN_EN;
        regval |= GLB_GPADC1_CONT_CONV_EN;
    } else if (!(config->scan_conv_mode) && !(config->continuous_conv_mode)) {
        regval &= ~GLB_GPADC1_SCN_LOOP_EN;
        regval &= ~GLB_GPADC1_SCN_EN;
        regval &= ~GLB_GPADC1_CONT_CONV_EN;
    }
    regval &= ~GLB_GPADC1_RES_SEL_MASK;
    regval |= (config->resolution << GLB_GPADC1_RES_SEL_SHIFT);    /* resolution */
    regval &= ~GLB_GPADC1_DLY_SEL_MASK;
    regval |= (2 << GLB_GPADC1_DLY_SEL_SHIFT);
    regval &= ~GLB_GPADC1_DITHER_EN;
    regval &= ~GLB_GPADC1_DWA_EN;
    regval &= ~GLB_GPADC1_CHL_SEL_NUM_MASK;
    regval |= (32 << GLB_GPADC1_CHL_SEL_NUM_SHIFT);
    putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);

    /* disable regular int and clear status */
    regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
    regval |= (GPIP_GPADC_FIFO_UNDERRUN_MASK | GPIP_GPADC_FIFO_OVERRUN_MASK | GPIP_GPADC_RDY_MASK |
               GPIP_GPADC_FIFO_UNDERRUN_CLR | GPIP_GPADC_FIFO_OVERRUN_CLR | GPIP_GPADC_RDY_CLR);

    regval |= GPIP_GPADC_FIFO_CLR;
    regval &= ~GPIP_GPADC_FIFO_THL_MASK;
    regval &= ~GPIP_GPADC_DMA_EN;
    putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

    regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
    regval &= ~(GPIP_GPADC_FIFO_UNDERRUN_CLR |
                GPIP_GPADC_FIFO_OVERRUN_CLR |
                GPIP_GPADC_RDY_CLR |
                GPIP_GPADC_FIFO_CLR);
    putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

    /* disable inject int and clear status */
    regval = getreg32(ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);
    regval |= (GPIP_GPADC_FIFO_UNDERRUN_MASK | GPIP_GPADC_FIFO_OVERRUN_MASK | GPIP_GPADC_RDY_MASK |
               GPIP_GPADC_FIFO_UNDERRUN_CLR | GPIP_GPADC_FIFO_OVERRUN_CLR | GPIP_GPADC_RDY_CLR);

    regval |= GPIP_GPADC_FIFO_CLR;
    regval &= ~GPIP_GPADC_FIFO_THL_MASK;
    regval &= ~GPIP_GPADC_DMA_EN;
    putreg32(regval, ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);

    regval = getreg32(ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);
    regval &= ~(GPIP_GPADC_FIFO_UNDERRUN_CLR |
                GPIP_GPADC_FIFO_OVERRUN_CLR |
                GPIP_GPADC_RDY_CLR |
                GPIP_GPADC_FIFO_CLR);
    putreg32(regval, ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);
#endif
}

void bflb_adc_deinit(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_adc_deinit
    romapi_bflb_adc_deinit(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = 0;
    regval |= (1 << GLB_GPADC_DVDD_SEL_SHIFT);
    regval |= (0xF << GLB_GPADC_INT_POS_SEL_SHIFT);
    regval |= (0xF << GLB_GPADC_INT_NEG_SEL_SHIFT);
    putreg32(regval, reg_base + GLB_GPADC_REG_CMD1_OFFSET);

    regval = 0;
    regval |= (0xF << GLB_GPADC1_EXT_POS_SEL_SHIFT);
    regval |= (0xF << GLB_GPADC1_EXT_NEG_SEL_SHIFT);
    putreg32(regval, reg_base + GLB_GPADC1_REG_CMD1_OFFSET);

    regval = 0;
    regval |= (3 << GLB_GPADC1_PGA_VCM_SHIFT); /* 0.9V */
    regval |= (2 << GLB_GPADC1_CHOP_MODE_SHIFT); /* Vref AZ and PGA chop on */
    regval |= (32 << GLB_GPADC1_VREF_TRIM_SHIFT);
    putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG1_OFFSET);

    regval = 0;
    regval |= (1 << GLB_GPADC1_SCN_LEN_SHIFT);
    regval |= (2 << GLB_GPADC1_DLY_SEL_SHIFT);
    putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);

    putreg32(0, reg_base + GLB_GPADC_TEN_OFFSET);

    /* disable regular & inject int and clear status */
    regval = GPIP_GPADC_FIFO_UNDERRUN_CLR | GPIP_GPADC_FIFO_OVERRUN_CLR | GPIP_GPADC_RDY_CLR;
    regval |= GPIP_GPADC_FIFO_CLR;
    regval |= (1 << GPIP_GPADC_INT_RDY_NUM_SHIFT);
    putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
    putreg32(regval, ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);

    regval = (1 << GPIP_GPADC_INT_RDY_NUM_SHIFT);
    putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
    putreg32(regval, ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);
#endif
}

void bflb_adc_link_rxdma(struct bflb_device_s *dev, bool enable)
{
#ifdef romapi_bflb_adc_link_rxdma
    romapi_bflb_adc_link_rxdma(dev, enable);
#else
    uint32_t regval;

    regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
    if (enable) {
        regval |= GPIP_GPADC_DMA_EN;
    } else {
        regval &= ~GPIP_GPADC_DMA_EN;
    }
    putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
#endif
}

void bflb_adc_link_rxdma_inject(struct bflb_device_s *dev, bool enable)
{
#ifdef romapi_bflb_adc_link_rxdma_inject
    romapi_bflb_adc_link_rxdma_inject(dev, enable);
#else
    uint32_t regval;

    regval = getreg32(ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);
    if (enable) {
        regval |= GPIP_GPADC_DMA_EN;
    } else {
        regval &= ~GPIP_GPADC_DMA_EN;
    }
    putreg32(regval, ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);
#endif
}

int bflb_adc_channel_config_internal(struct bflb_device_s *dev, struct bflb_adc_channel_s *chan, uint8_t channels)
{
#ifdef romapi_bflb_adc_channel_config_internal
    return romapi_bflb_adc_channel_config_internal(dev, chan, channels);
#else
    uint32_t regval;
    uint32_t reg_base;

    LHAL_PARAM_ASSERT(IS_ADC_INTERNAL_CHANNEL(chan->pos_chan));
    LHAL_PARAM_ASSERT(IS_ADC_INTERNAL_CHANNEL(chan->neg_chan));

    reg_base = dev->reg_base;

    if (channels != 1) {
        return -EINVAL;
    }
    /* disable external channel */
    regval = getreg32(reg_base + GLB_GPADC1_REG_CMD1_OFFSET);
    regval &= ~GLB_GPADC1_EXT_NEG_SEL_MASK;
    regval |= (0xF << GLB_GPADC1_EXT_NEG_SEL_SHIFT);
    regval &= ~GLB_GPADC1_EXT_POS_SEL_MASK;
    regval |= (0xF << GLB_GPADC1_EXT_POS_SEL_SHIFT);
    putreg32(regval, reg_base + GLB_GPADC1_REG_CMD1_OFFSET);
    /* config internal channel */
    regval = getreg32(reg_base + GLB_GPADC_REG_CMD1_OFFSET);
    regval &= ~GLB_GPADC_INT_POS_SEL_MASK;
    regval &= ~GLB_GPADC_INT_NEG_SEL_MASK;
    regval |= (chan->pos_chan << GLB_GPADC_INT_POS_SEL_SHIFT);
    regval |= (chan->neg_chan << GLB_GPADC_INT_NEG_SEL_SHIFT);
    putreg32(regval, reg_base + GLB_GPADC_REG_CMD1_OFFSET);

    return 0;
#endif
}

int bflb_adc_channel_config_external(struct bflb_device_s *dev, struct bflb_adc_channel_s *chan, uint8_t channels)
{
#ifdef romapi_bflb_adc_channel_config_external
    return romapi_bflb_adc_channel_config_external(dev, chan, channels);
#else
    uint32_t regval;
    uint32_t regval2;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    if (!(getreg32(reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET) & GLB_GPADC1_SCN_EN)) {
        if (channels > 1) {
            return -EINVAL;
        }
        regval = getreg32(reg_base + GLB_GPADC1_REG_CMD1_OFFSET);
        regval &= ~GLB_GPADC1_EXT_POS_SEL_MASK;
        regval &= ~GLB_GPADC1_EXT_NEG_SEL_MASK;
        regval |= (chan->pos_chan << GLB_GPADC1_EXT_POS_SEL_SHIFT);
        regval |= (chan->neg_chan << GLB_GPADC1_EXT_NEG_SEL_SHIFT);
        putreg32(regval, reg_base + GLB_GPADC1_REG_CMD1_OFFSET);
    } else {
        if (channels == 0) {
            return -EINVAL;
        } else if (channels <= 6) {
            regval = 0;
            regval2 = 0;
            for (uint8_t i = 0; i < channels; i++) {
                regval |= (chan[i].pos_chan << (i * 5));
                regval2 |= (chan[i].neg_chan << (i * 5));
            }
            for (uint8_t i = channels; i < 6; i++) {
                regval |= (0xF << (i * 5));
                regval2 |= (0xF << (i * 5));
            }
            putreg32(regval, reg_base + GLB_GPADC_REG_SCN_POS1_OFFSET);
            putreg32(regval2, reg_base + GLB_GPADC_REG_SCN_NEG1_OFFSET);
        } else if (channels <= 12) {
            regval = 0;
            regval2 = 0;
            for (uint8_t i = 0; i < 6; i++) {
                regval |= (chan[i].pos_chan << (i * 5));
                regval2 |= (chan[i].neg_chan << (i * 5));
            }
            putreg32(regval, reg_base + GLB_GPADC_REG_SCN_POS1_OFFSET);
            putreg32(regval2, reg_base + GLB_GPADC_REG_SCN_NEG1_OFFSET);

            regval = 0;
            regval2 = 0;
            for (uint8_t i = 0; i < (channels - 6); i++) {
                regval |= (chan[i + 6].pos_chan << (i * 5));
                regval2 |= (chan[i + 6].neg_chan << (i * 5));
            }
            for (uint8_t i = channels - 6; i < 6; i++) {
                regval |= (0xF << (i * 5));
                regval2 |= (0xF << (i * 5));
            }
            putreg32(regval, reg_base + GLB_GPADC_REG_SCN_POS2_OFFSET);
            putreg32(regval2, reg_base + GLB_GPADC_REG_SCN_NEG2_OFFSET);
        } else {
            return -EINVAL;
        }
        regval = getreg32(reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);
        regval &= ~GLB_GPADC1_SCN_LEN_MASK;
        regval |= (channels << GLB_GPADC1_SCN_LEN_SHIFT);
        putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);
    }
    return 0;
#endif
}

int bflb_adc_channel_config_internal_inject(struct bflb_device_s *dev, struct bflb_adc_channel_s *chan, uint8_t channels)
{
#ifdef romapi_bflb_adc_channel_config_internal_inject
    return romapi_bflb_adc_channel_config_internal_inject(dev, chan, channels);
#else
    uint32_t regval;
    uint32_t regval2;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    if (channels == 0) {
        return -EINVAL;
    } else if (channels <= 4) {
        regval = 0;
        regval2 = 0;
        for (uint8_t i = 0; i < channels; i++) {
            regval |= (chan[i].pos_chan << (i * 5));
            regval2 |= (chan[i].neg_chan << (i * 5));
        }
        for (uint8_t i = channels; i < 4; i++) {
            regval |= (0x4 << (i * 5));
            regval2 |= (0x4 << (i * 5));
        }
        putreg32(regval, reg_base + GLB_GPADC_INT_INSERT_POS_OFFSET);
        putreg32(regval2, reg_base + GLB_GPADC_INT_INSERT_NEG_OFFSET);

        regval = getreg32(reg_base + GLB_GPADC1_REG_CONFIG1_OFFSET);
        regval &= ~GLB_GPADC1_INT_INSERT_LEN_MASK;
        regval |= (channels << GLB_GPADC1_INT_INSERT_LEN_SHIFT);
        regval |= GLB_GPADC1_INT_EXT_INSERT_SEL;
        putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG1_OFFSET);
    } else {
        return -EINVAL;
    }
    return 0;
#endif
}

int bflb_adc_channel_config_external_inject(struct bflb_device_s *dev, struct bflb_adc_channel_s *chan, uint8_t channels)
{
#ifdef romapi_bflb_adc_channel_config_external_inject
    return romapi_bflb_adc_channel_config_external_inject(dev, chan, channels);
#else
    uint32_t regval;
    uint32_t regval2;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    if (channels == 0) {
        return -EINVAL;
    } else if (channels <= 6) {
        regval = 0;
        regval2 = 0;
        for (uint8_t i = 0; i < channels; i++) {
            regval |= (chan[i].pos_chan << (i * 5));
            regval2 |= (chan[i].neg_chan << (i * 5));
        }
        for (uint8_t i = channels; i < 6; i++) {
            regval |= (0xF << (i * 5));
            regval2 |= (0xF << (i * 5));
        }
        putreg32(regval, reg_base + GLB_GPADC_REG_INSERT_POS1_OFFSET);
        putreg32(regval2, reg_base + GLB_GPADC_REG_INSERT_NEG1_OFFSET);
    } else if (channels <= 12) {
        regval = 0;
        regval2 = 0;
        for (uint8_t i = 0; i < 6; i++) {
            regval |= (chan[i].pos_chan << (i * 5));
            regval2 |= (chan[i].neg_chan << (i * 5));
        }
        putreg32(regval, reg_base + GLB_GPADC_REG_INSERT_POS1_OFFSET);
        putreg32(regval2, reg_base + GLB_GPADC_REG_INSERT_NEG1_OFFSET);

        regval = 0;
        regval2 = 0;
        for (uint8_t i = 0; i < (channels - 6); i++) {
            regval |= (chan[i + 6].pos_chan << (i * 5));
            regval2 |= (chan[i + 6].neg_chan << (i * 5));
        }
        for (uint8_t i = channels - 6; i < 6; i++) {
            regval |= (0xF << (i * 5));
            regval2 |= (0xF << (i * 5));
        }
        putreg32(regval, reg_base + GLB_GPADC_REG_INSERT_POS2_OFFSET);
        putreg32(regval2, reg_base + GLB_GPADC_REG_INSERT_NEG2_OFFSET);
    } else {
        return -EINVAL;
    }
    regval = getreg32(reg_base + GLB_GPADC1_REG_CONFIG1_OFFSET);
    regval &= ~GLB_GPADC1_EXT_INSERT_LEN_MASK;
    regval |= (channels << GLB_GPADC1_EXT_INSERT_LEN_SHIFT);
    regval &= ~GLB_GPADC1_INT_EXT_INSERT_SEL;
    putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG1_OFFSET);

    return 0;
#endif
}

void bflb_adc_start_conversion(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_adc_start_conversion
    romapi_bflb_adc_start_conversion(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);
    regval &= ~GLB_GPADC1_CONV_START;
    putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);

    bflb_mtimer_delay_us(100);

    regval = getreg32(reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);
    regval |= GLB_GPADC1_CONV_START;
    putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);
#endif
}

void bflb_adc_start_conversion_inject(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_adc_start_conversion_inject
    romapi_bflb_adc_start_conversion_inject(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + GLB_GPADC1_REG_CONFIG1_OFFSET);
    regval &= ~GLB_GPADC1_INSERT;
    putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG1_OFFSET);

    bflb_mtimer_delay_us(1);

    regval = getreg32(reg_base + GLB_GPADC1_REG_CONFIG1_OFFSET);
    regval |= GLB_GPADC1_INSERT;
    putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG1_OFFSET);

    bflb_mtimer_delay_us(1);

    regval = getreg32(reg_base + GLB_GPADC1_REG_CONFIG1_OFFSET);
    regval &= ~GLB_GPADC1_INSERT;
    putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG1_OFFSET);
#endif
}

void bflb_adc_stop_conversion(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_adc_stop_conversion
    romapi_bflb_adc_stop_conversion(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);
    regval &= ~GLB_GPADC1_CONV_START;
    putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);
#endif
}

void bflb_adc_stop_conversion_inject(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_adc_stop_conversion_inject
    romapi_bflb_adc_stop_conversion_inject(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + GLB_GPADC1_REG_CONFIG1_OFFSET);
    regval &= ~GLB_GPADC1_INSERT;
    putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG1_OFFSET);
#endif
}

uint8_t bflb_adc_get_count(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_adc_get_count
    return romapi_bflb_adc_get_count(dev);
#else
    return ((getreg32(ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET) & GPIP_GPADC_FIFO_DATA_COUNT_MASK) >> GPIP_GPADC_FIFO_DATA_COUNT_SHIFT);
#endif
}

uint8_t bflb_adc_get_count_inject(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_adc_get_count_inject
    return romapi_bflb_adc_get_count_inject(dev);
#else
    return ((getreg32(ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET) & GPIP_GPADC_FIFO_DATA_COUNT_MASK) >> GPIP_GPADC_FIFO_DATA_COUNT_SHIFT);
#endif
}

uint32_t bflb_adc_read_raw(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_adc_read_raw
    return romapi_bflb_adc_read_raw(dev);
#else
    return getreg32(ADC_GPIP_BASE + GPIP_GPADC_DMA_RDATA_OFFSET) & GPIP_GPADC_DMA_RDATA_MASK;
#endif
}

uint32_t bflb_adc_read_raw_inject(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_adc_read_raw_inject
    return romapi_bflb_adc_read_raw_inject(dev);
#else
    return getreg32(ADC_GPIP2_BASE + GPIP_GPADC_DMA_RDATA_OFFSET) & GPIP_GPADC_DMA_RDATA_MASK;
#endif
}

void bflb_adc_clear_fifo(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_adc_clear_fifo
    romapi_bflb_adc_clear_fifo(dev);
#else
    uint32_t regval;

    regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
    regval |= GPIP_GPADC_FIFO_CLR;
    putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
#endif
}

void bflb_adc_clear_fifo_inject(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_adc_clear_fifo_inject
    romapi_bflb_adc_clear_fifo_inject(dev);
#else
    uint32_t regval;

    regval = getreg32(ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);
    regval |= GPIP_GPADC_FIFO_CLR;
    putreg32(regval, ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);
#endif
}

void bflb_adc_int_mask(struct bflb_device_s *dev, uint32_t int_type, bool mask)
{
#ifdef romapi_bflb_adc_int_mask
    romapi_bflb_adc_int_mask(dev, int_type, mask);
#else
    uint32_t regval;
    uint32_t int_mask;

    int_mask = int_type & ADC_INT_REGULAR_FIFO;
    if (int_mask) {
        int_mask <<= 12;
        regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
        if (mask) {
            regval |= int_mask;
        } else {
            regval &= ~int_mask;
        }
        putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
    }

    int_mask = int_type & ADC_INT_INJECT_FIFO;
    if (int_mask) {
        int_mask >>= 3;
        int_mask <<= 12;
        regval = getreg32(ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);
        if (mask) {
            regval |= int_mask;
        } else {
            regval &= ~int_mask;
        }
        putreg32(regval, ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);
    }
#endif
}

uint32_t bflb_adc_get_int_status(struct bflb_device_s *dev, uint32_t int_type)
{
#ifdef romapi_bflb_adc_get_int_status
    return romapi_bflb_adc_get_int_status(dev, int_type);
#else
    uint32_t regval;
    uint32_t int_mask;
    uint32_t status = 0;

    int_mask = int_type & ADC_INT_REGULAR_FIFO;
    if (int_mask) {
        regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
        regval >>= 4;
        status = regval & ADC_INT_REGULAR_FIFO;
    }

    int_mask = int_type & ADC_INT_INJECT_FIFO;
    if (int_mask) {
        regval = getreg32(ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);
        regval >>= 4;
        regval <<= 3;
        regval &= ADC_INT_INJECT_FIFO;
        status |= regval;
    }

    return status;
#endif
}

void bflb_adc_int_clear(struct bflb_device_s *dev, uint32_t int_type)
{
#ifdef romapi_bflb_adc_int_clear
    romapi_bflb_adc_int_clear(dev, int_type);
#else
    uint32_t regval;
    uint32_t int_mask;

    int_mask = int_type & ADC_INT_REGULAR_FIFO;
    if (int_mask) {
        int_mask <<= 8;
        /* write 1 to clear flag */
        regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
        regval |= int_mask;
        putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
        /* write 0 to release clear operation */
        regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
        regval &= ~int_mask;
        putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
    }

    int_mask = int_type & ADC_INT_INJECT_FIFO;
    if (int_mask) {
        int_mask >>= 3;
        int_mask <<= 8;
        /* write 1 to clear flag */
        regval = getreg32(ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);
        regval |= int_mask;
        putreg32(regval, ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);
        /* write 0 to release clear operation */
        regval = getreg32(ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);
        regval &= ~int_mask;
        putreg32(regval, ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);
    }
#endif
}

void bflb_adc_parse_result(struct bflb_device_s *dev, uint32_t *buffer, struct bflb_adc_result_s *result, uint16_t count)
{
#ifdef romapi_bflb_adc_parse_result
    romapi_bflb_adc_parse_result(dev, buffer, result, count);
#else
    uint32_t reg_base;
    uint32_t ref = 3200;
    uint32_t conv_result;
    uint32_t resolution;
    uint32_t diff_mode;
    uint32_t neg;

    reg_base = dev->reg_base;

    resolution = (getreg32(reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET) & GLB_GPADC1_RES_SEL_MASK) >> GLB_GPADC1_RES_SEL_SHIFT;
    diff_mode = (getreg32(reg_base + GLB_GPADC1_REG_CONFIG1_OFFSET) & GLB_GPADC1_DIFF_MODE);

    if (diff_mode == 0) {
        /* single-ended mode  */
        for (uint16_t i = 0; i < count; i++) {
            result[i].pos_chan = (buffer[i] >> 21) & 0x1F;
            result[i].neg_chan = (buffer[i] >> 16) & 0x1F;
            conv_result = buffer[i] & 0xFFFF;
            if (resolution == ADC_RESOLUTION_12B) {
                result[i].value = conv_result >> 4;
                result[i].millivolt = result[i].value * ref / 4096;
            } else if (resolution == ADC_RESOLUTION_14B) {
                result[i].value = conv_result >> 2;
                result[i].millivolt = result[i].value * ref / 16384;
            } else if (resolution == ADC_RESOLUTION_16B) {
                result[i].value = conv_result >> 0;
                result[i].millivolt = result[i].value * ref / 65536;
            } else {
                result[i].value = 0;
                result[i].millivolt = 0;
            }
        }
    } else {
        /* difference mode  */
        for (uint16_t i = 0; i < count; i++) {
            result[i].pos_chan = (buffer[i] >> 21) & 0x1F;
            result[i].neg_chan = (buffer[i] >> 16) & 0x1F;

            if (buffer[i] & (1 << 15)) {
                conv_result = ~buffer[i];
                conv_result += 1;
                neg = 1;
            } else {
                conv_result = buffer[i];
                neg = 0;
            }

            conv_result = conv_result & 0xFFFF;
            if (resolution == ADC_RESOLUTION_12B) {
                result[i].value = conv_result >> 4;
                result[i].millivolt = result[i].value * ref / 2048;
            } else if (resolution == ADC_RESOLUTION_14B) {
                result[i].value = conv_result >> 2;
                result[i].millivolt = result[i].value * ref / 8192;
            } else if (resolution == ADC_RESOLUTION_16B) {
                result[i].value = conv_result >> 0;
                result[i].millivolt = result[i].value * ref / 32768;
            } else {
                result[i].value = 0;
                result[i].millivolt = 0;
            }
            if (neg) {
                result[i].value = 0 - result[i].value;
                result[i].millivolt = 0 - result[i].millivolt;
            }
        }
    }
#endif
}

void bflb_adc_tsen_init(struct bflb_device_s *dev, uint8_t tsen_mod)
{
#ifdef romapi_bflb_adc_tsen_init
    romapi_bflb_adc_tsen_init(dev, tsen_mod);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + GLB_GPADC_REG_CMD1_OFFSET);
    regval |= GLB_GPADC_TS_EN;
    regval &= ~GLB_GPADC_TSVBE_LOW;
    if (tsen_mod) {
        regval |= GLB_GPADC_TSEXT_SEL;
    } else {
        regval &= ~GLB_GPADC_TSEXT_SEL;
    }
    putreg32(regval, reg_base + GLB_GPADC_REG_CMD1_OFFSET);

    regval = getreg32(reg_base + GLB_GPADC1_REG_CONFIG1_OFFSET);
    regval &= ~GLB_GPADC1_PGA_VCMI_EN;
    regval &= ~GLB_GPADC1_CHOP_MODE_MASK;
    regval |= (1 << GLB_GPADC1_CHOP_MODE_SHIFT); /* Vref AZ */
    regval &= ~GLB_GPADC1_PGA_VCM_MASK;
    regval |= (1 << GLB_GPADC1_PGA_VCM_SHIFT);
    putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG1_OFFSET);

    regval = getreg32(reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);
    regval |= GLB_GPADC1_DWA_EN;
    regval &= ~GLB_GPADC1_DLY_SEL_MASK;
    regval |= (2 << GLB_GPADC1_DLY_SEL_SHIFT);
    regval |= GLB_GPADC1_DITHER_EN;
    putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);
#endif
}

float bflb_adc_tsen_get_temp(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_adc_tsen_get_temp
    return romapi_bflb_adc_tsen_get_temp(dev);
#else
    uint32_t regval;
    uint32_t reg_base;
    uint32_t v0 = 0, v1 = 0;
    float temp = 0;
    uint32_t raw_data;
    uint64_t start_time;
    uint32_t sum;

    reg_base = dev->reg_base;

    regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
    regval |= GPIP_GPADC_FIFO_CLR;
    putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

    regval = getreg32(reg_base + GLB_GPADC_REG_CMD1_OFFSET);
    regval &= ~GLB_GPADC_TSVBE_LOW;
    putreg32(regval, reg_base + GLB_GPADC_REG_CMD1_OFFSET);

    sum = 0;
    bflb_adc_start_conversion(dev);
    for (int i = 0; i < 8; i++) {
        start_time = bflb_mtimer_get_time_ms();
        while (bflb_adc_get_count(dev) == 0) {
            if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
                return -ETIMEDOUT;
            }
        }
        bflb_adc_read_raw(dev);
    }
    for (int i = 0; i < 16; i++) {
        start_time = bflb_mtimer_get_time_ms();
        while (bflb_adc_get_count(dev) == 0) {
            if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
                return -ETIMEDOUT;
            }
        }
        raw_data = bflb_adc_read_raw(dev);
        sum += (raw_data & 0xFFFF);
    }
    bflb_adc_stop_conversion(dev);
    v0 = (sum + 8) / 16;

    regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
    regval |= GPIP_GPADC_FIFO_CLR;
    putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);

    regval = getreg32(reg_base + GLB_GPADC_REG_CMD1_OFFSET);
    regval |= GLB_GPADC_TSVBE_LOW;
    putreg32(regval, reg_base + GLB_GPADC_REG_CMD1_OFFSET);

    sum = 0;
    bflb_adc_start_conversion(dev);
    for (int i = 0; i < 8; i++) {
        start_time = bflb_mtimer_get_time_ms();
        while (bflb_adc_get_count(dev) == 0) {
            if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
                return -ETIMEDOUT;
            }
        }
        bflb_adc_read_raw(dev);
    }
    for (int i = 0; i < 16; i++) {
        start_time = bflb_mtimer_get_time_ms();
        while (bflb_adc_get_count(dev) == 0) {
            if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
                return -ETIMEDOUT;
            }
        }
        raw_data = bflb_adc_read_raw(dev);
        sum += (raw_data & 0xFFFF);
    }
    bflb_adc_stop_conversion(dev);
    v1 = (sum + 8) / 16;

    if (v0 > v1) {
        temp = (((float)v0 - (float)v1) - (float)tsen_offset) / 5.18f;
    } else {
        temp = (((float)v1 - (float)v0) - (float)tsen_offset) / 5.18f;
    }

    return temp;
#endif
}

void bflb_adc_vbat_enable(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_adc_vbat_enable
    romapi_bflb_adc_vbat_enable(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + GLB_GPADC_REG_CMD1_OFFSET);
    regval |= GLB_GPADC_VBAT_EN;
    putreg32(regval, reg_base + GLB_GPADC_REG_CMD1_OFFSET);
#endif
}

void bflb_adc_vbat_disable(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_adc_vbat_disable
    romapi_bflb_adc_vbat_disable(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + GLB_GPADC_REG_CMD1_OFFSET);
    regval &= ~GLB_GPADC_VBAT_EN;
    putreg32(regval, reg_base + GLB_GPADC_REG_CMD1_OFFSET);
#endif
}

int bflb_adc_feature_control(struct bflb_device_s *dev, int cmd, size_t arg)
{
#ifdef bflb_adc_feature_control
    return bflb_adc_feature_control(dev, cmd, arg);
#else
    int ret = 0;
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    switch (cmd) {
        case ADC_CMD_VBAT_EN:
            if (arg) {
                regval = getreg32(reg_base + GLB_GPADC_REG_CMD1_OFFSET);
                regval |= GLB_GPADC_VBAT_EN;
                putreg32(regval, reg_base + GLB_GPADC_REG_CMD1_OFFSET);
            } else {
                regval = getreg32(reg_base + GLB_GPADC_REG_CMD1_OFFSET);
                regval &= ~GLB_GPADC_VBAT_EN;
                putreg32(regval, reg_base + GLB_GPADC_REG_CMD1_OFFSET);
            }
            break;
        case ADC_CMD_CONV_DELAY:
            /* register 0 means 256 */
            if (arg <= 0) {
                arg = 1;
            } else if (arg >= 256) {
                arg = 0;
            }
            regval = getreg32(reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);
            regval &= ~GLB_GPADC1_CHL_SEL_NUM_MASK;
            regval |= (arg << GLB_GPADC1_CHL_SEL_NUM_SHIFT);
            putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG2_OFFSET);
            break;
        case ADC_CMD_PWM_TRIG_EN:
            regval = getreg32(reg_base + GLB_GPADC1_REG_CONFIG1_OFFSET);
            if (arg) {
                regval |= GLB_GPADC1_PWM_TRG_EN;
            } else {
                regval &= ~GLB_GPADC1_PWM_TRG_EN;
            }
            putreg32(regval, reg_base + GLB_GPADC1_REG_CONFIG1_OFFSET);
            break;
        case ADC_CMD_SET_THRE_RDY_REGULAR:
            if (arg < 1) {
                arg = 1;
            } else if (arg > 31) {
                arg = 0;
            }
            regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            regval &= ~GPIP_GPADC_INT_RDY_NUM_MASK;
            regval |= (arg << GPIP_GPADC_INT_RDY_NUM_SHIFT);
            putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            break;
        case ADC_CMD_SET_THRE_DMA_REGULAR:
            regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            regval &= ~GPIP_GPADC_FIFO_THL_MASK;
            regval |= (arg << GPIP_GPADC_FIFO_THL_SHIFT);
            putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            break;
        case ADC_CMD_SET_THRE_RDY_INJECT:
            if (arg < 1) {
                arg = 1;
            } else if (arg > 31) {
                arg = 0;
            }
            regval = getreg32(ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);
            regval &= ~GPIP_GPADC_INT_RDY_NUM_MASK;
            regval |= (arg << GPIP_GPADC_INT_RDY_NUM_SHIFT);
            putreg32(regval, ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);
            break;
        case ADC_CMD_SET_THRE_DMA_INJECT:
            regval = getreg32(ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);
            regval &= ~GPIP_GPADC_FIFO_THL_MASK;
            regval |= (arg << GPIP_GPADC_FIFO_THL_SHIFT);
            putreg32(regval, ADC_GPIP2_BASE + GPIP_GPADC_CONFIG_OFFSET);
            break;

        default:
            ret = -EPERM;
            break;
    }
    return ret;
#endif
}
