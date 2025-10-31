#include "bflb_adc_v3.h"
#include "bflb_efuse.h"
#include "hardware/adc_v3_reg.h"

#if defined(BL616D)
#define ADC_GPIP_BASE ((uint32_t)0x20002000)
#define ADC_AON_BASE  ((uint32_t)0x2008F000)
#else
#error "Please select correct chip that support adc_v3"
#endif

#if defined(BL616D_VERSION_A0)
volatile uint32_t tsen_offset = 2400;
#else
volatile uint32_t tsen_offset = 0;
#endif
volatile int adc_reference_channel = -1;

__UNUSED static int bflb_adc_fifo1_is_enabled(void)
{
    uint32_t regval;

    regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC2_FIFO_ENABLE_OFFSET);
    if (regval & GPIP_GPADC2_FIFO_EN) {
        return 1;
    } else {
        return 0;
    }
}

void bflb_adc_common_init(const struct bflb_adc_common_config_s *config)
{
#ifdef romapi_bflb_adc_common_init
    romapi_bflb_adc_common_init(config);
#else
    uint32_t regval;

    LHAL_PARAM_ASSERT(IS_ADC_CLK_DIV(config->clk_div));
    LHAL_PARAM_ASSERT(IS_ADC_MODE(config->mode));

    regval = getreg32(ADC_AON_BASE + AON_GPADC_REG_CMD2_OFFSET);
    regval &= ~AON_GPADC_CLK_DIV_RATIO_MASK;
    regval |= (config->clk_div << AON_GPADC_CLK_DIV_RATIO_SHIFT);
    regval &= ~AON_GPADC_MODE_MASK;
    regval |= (config->mode << AON_GPADC_MODE_SHIFT);
    putreg32(regval, ADC_AON_BASE + AON_GPADC_REG_CMD2_OFFSET);

    regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC2_FIFO_ENABLE_OFFSET);
    if (config->fifo1_enable) {
        regval |= GPIP_GPADC2_FIFO_EN;
    } else {
        regval &= ~GPIP_GPADC2_FIFO_EN;
    }
    putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC2_FIFO_ENABLE_OFFSET);

#if defined(BL616D_VERSION_A0)
    regval = getreg32(ADC_AON_BASE + AON_GPADC1_REG_CONFIG2_OFFSET);
    regval |= AON_GPADC1_GLOBAL_EN;
    putreg32(regval, ADC_AON_BASE + AON_GPADC1_REG_CONFIG2_OFFSET);
    regval = getreg32(ADC_AON_BASE + AON_GPADC2_REG_CONFIG2_OFFSET);
    regval |= AON_GPADC2_GLOBAL_EN;
    putreg32(regval, ADC_AON_BASE + AON_GPADC2_REG_CONFIG2_OFFSET);
#endif

    bflb_adc_int_watchdog_mask(NULL, ADC_INT_WATCHDOG_ALL, true);
    bflb_adc_int_watchdog_clear(NULL, ADC_INT_WATCHDOG_ALL);
#endif
}

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
    regval = getreg32(reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);
    regval &= ~AON_GPADC1_GLOBAL_EN;
    putreg32(regval, reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);

    regval = getreg32(reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);
    regval |= AON_GPADC1_GLOBAL_EN;
    putreg32(regval, reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);

    /* adc reset */
    regval = getreg32(reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);
    regval |= AON_GPADC1_SOFT_RST;
    putreg32(regval, reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);

    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");

    regval = getreg32(reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);
    regval &= ~AON_GPADC1_SOFT_RST;
    putreg32(regval, reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);

    regval = 0;
    regval |= (3 << AON_GPADC1_PGA_VCM_SHIFT); /* 0.9V */
    regval |= (1 << AON_GPADC1_PGA_GAIN_SHIFT); /* gain 1 */
    regval |= AON_GPADC1_PGA_EN;
    if (config->resolution == ADC_RESOLUTION_12B) {
        regval |= (1 << AON_GPADC1_CHOP_MODE_SHIFT); /* Vref AZ and PGA chop */
    } else {
        regval |= (2 << AON_GPADC1_CHOP_MODE_SHIFT); /* Vref AZ and PGA chop */
    }
#if defined(BL616D_VERSION_A0)
    regval |= (16 << AON_GPADC1_VREF_TRIM_SHIFT);
#endif
    if (config->differential_mode) {
        regval |= AON_GPADC1_DIFF_MODE;
        regval &= ~AON_GPADC1_NEG_GND;
    } else {
        regval &= ~AON_GPADC1_DIFF_MODE;
        regval |= AON_GPADC1_NEG_GND;
    }
#if defined(BL616D_VERSION_A0)
    if (config->vref == ADC_VREF_INTERNAL_1P25) {
#else
    if (config->vref == ADC_VREF_EXTERNAL_1P25) {
#endif
        regval |= AON_GPADC1_VREF_EX_SEL;
    }
    putreg32(regval, reg_base + AON_GPADC1_REG_CONFIG1_OFFSET);

    regval = getreg32(reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);
    regval |= AON_GPADC1_EXT_NEG_SEL_MASK;
    regval |= AON_GPADC1_EXT_POS_SEL_MASK;
    regval &= ~AON_GPADC1_DLY_SEL_MASK;
    regval |= (2 << AON_GPADC1_DLY_SEL_SHIFT);
    regval &= ~AON_GPADC1_DITHER_EN;
    regval &= ~AON_GPADC1_DWA_EN;
    putreg32(regval, reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);

    regval = 0;
    regval |= (config->resolution << AON_GPADC1_RES_SEL_SHIFT);    /* resolution */
    if (config->scan_conv_mode) {
        regval |= AON_GPADC1_SCAN_EN;
    }
    if (config->continuous_conv_mode) {
        regval |= AON_GPADC1_CONT_CONV_EN;
    }
#if defined(BL616D_VERSION_A0)
    regval |= (1 << AON_GPADC1_CONV_DELAY_SHIFT); /* delay 2(=1+1) clk */
#endif
    putreg32(regval, reg_base + AON_GPADC1_REG_CTRL_0_OFFSET);

    /* calibration offset */
    regval = getreg32(reg_base + AON_GPADC1_REG_OS_CAL_DATA_OFFSET);
    regval &= ~AON_GPADC1_OS_CAL_DATA_MASK;
    putreg32(regval, reg_base + AON_GPADC1_REG_OS_CAL_DATA_OFFSET);

    /* disable int and clear status */
    bflb_adc_int_saturation_mask(dev, ADC_INT_SATURATION_ALL, true);
    bflb_adc_int_done_mask(dev, ADC_INT_DONE_ALL, true);
    bflb_adc_int_fifo_mask(dev, ADC_INT_FIFO_ALL, true);
    bflb_adc_int_saturation_clear(dev, ADC_INT_SATURATION_ALL);
    bflb_adc_int_done_clear(dev, ADC_INT_DONE_ALL);
    bflb_adc_int_fifo_clear(dev, ADC_INT_FIFO_ALL);

    if ((dev->idx == 1) && bflb_adc_fifo1_is_enabled()) {
        putreg32(0xF002, ADC_GPIP_BASE + GPIP_GPADC_FIFO2_CONFIG_OFFSET);
    } else {
        putreg32(0xF002, ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
    }
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

    putreg32(0x03002320, reg_base + AON_GPADC1_REG_CONFIG1_OFFSET);
    putreg32(0x00FF2000, reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);
    putreg32(0x00000041, reg_base + AON_GPADC1_REG_CTRL_0_OFFSET);
    putreg32(0, reg_base + AON_GPADC1_REG_OS_CAL_DATA_OFFSET);
    putreg32(0xFFFFFFFF, reg_base + AON_GPADC1_REG_SCAN_0_OFFSET);
    putreg32(0xFFFFFFFF, reg_base + AON_GPADC1_REG_SCAN_1_OFFSET);
    putreg32(0xFFFFFFFF, reg_base + AON_GPADC1_REG_SCAN_2_OFFSET);
    putreg32(0xFFFFFFFF, reg_base + AON_GPADC1_REG_SCAN_3_OFFSET);
    putreg32(0xFFFFFFFF, reg_base + AON_GPADC1_REG_IJCT_EXT_0_OFFSET);
    putreg32(0x04040404, reg_base + AON_GPADC1_REG_IJCT_INT_0_OFFSET);
    putreg32(0x04040404, reg_base + AON_GPADC1_REG_IJCT_INT_1_OFFSET);
    if (dev->idx == 0) {
        putreg32(AON_GPADC1_POS_SATUR_CLR | AON_GPADC1_NEG_SATUR_CLR, reg_base + AON_GPADC1_REG_SATUR_OFFSET);
        putreg32(0, reg_base + AON_GPADC1_REG_SATUR_OFFSET);
        putreg32(0xF002, ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
        regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_DMA_INT_MASK_OFFSET);
        regval &= (0x3 << 17);
        putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_DMA_INT_MASK_OFFSET);
        putreg32(0x0001EDB6, ADC_GPIP_BASE + GPIP_GPADC_DMA_INT_CLEAR_OFFSET);
    } else {
        putreg32(AON_GPADC1_POS_SATUR_CLR | AON_GPADC1_NEG_SATUR_CLR, reg_base + AON_GPADC1_REG_SATUR_OFFSET - AON_GPADC2_OFFSET_COMPENSATION);
        putreg32(0, reg_base + AON_GPADC1_REG_SATUR_OFFSET - AON_GPADC2_OFFSET_COMPENSATION);
        regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_DMA_INT_MASK_OFFSET);
        regval &= ~(0x3 << 17);
        putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_DMA_INT_MASK_OFFSET);
        putreg32(0x3 << 17, ADC_GPIP_BASE + GPIP_GPADC_DMA_INT_CLEAR_OFFSET);
        putreg32(GPIP_GPADC2_FIFO_EN, ADC_GPIP_BASE + GPIP_GPADC2_FIFO_ENABLE_OFFSET);
        putreg32(0xF002, ADC_GPIP_BASE + GPIP_GPADC_FIFO2_CONFIG_OFFSET);
        putreg32(0, ADC_GPIP_BASE + GPIP_GPADC2_DMA_INT_MASK_OFFSET);
        putreg32(0x6DB6, ADC_GPIP_BASE + GPIP_GPADC2_DMA_INT_CLEAR_OFFSET);
    }
#endif
}

void bflb_adc_link_rxdma(struct bflb_device_s *dev, bool enable)
{
#ifdef romapi_bflb_adc_link_rxdma
    romapi_bflb_adc_link_rxdma(dev, enable);
#else
    uint32_t regval;
    uint32_t reg_addr;

    if ((dev->idx == 1) && bflb_adc_fifo1_is_enabled()) {
        reg_addr = ADC_GPIP_BASE + GPIP_GPADC_FIFO2_CONFIG_OFFSET;
    } else {
        reg_addr = ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET;
    }
    regval = getreg32(reg_addr);
    if (enable) {
        regval |= GPIP_GPADC_FIFO_DMA_EN;
    } else {
        regval &= ~GPIP_GPADC_FIFO_DMA_EN;
    }
    putreg32(regval, reg_addr);
#endif
}

int bflb_adc_channel_config_internal(struct bflb_device_s *dev, struct bflb_adc_channel_s *chan, uint8_t channels)
{
#ifdef romapi_bflb_adc_channel_config_internal
    return romapi_bflb_adc_channel_config_internal(dev, chan, channels);
#else
    uint32_t regval;
    uint32_t reg_base;

    LHAL_PARAM_ASSERT(IS_ADC0_INTERNAL_CHANNEL(chan->pos_chan));
    LHAL_PARAM_ASSERT(IS_ADC0_INTERNAL_CHANNEL(chan->neg_chan));

    reg_base = dev->reg_base;

    /* only ADC0 support internal channel */
    if (dev->idx != 0) {
        return -EINVAL;
    }
    /* internal channel do not support scan mode */
    if (channels != 1) {
        return -EINVAL;
    }
    if (getreg32(reg_base + AON_GPADC1_REG_CTRL_0_OFFSET) & AON_GPADC1_SCAN_EN) {
        return -EINVAL;
    }
    /* disable external regular one-shot channel */
    regval = getreg32(ADC_AON_BASE + AON_GPADC1_REG_CONFIG2_OFFSET);
    regval &= ~AON_GPADC1_EXT_POS_SEL_MASK;
    regval |= (0xF << AON_GPADC1_EXT_POS_SEL_SHIFT);
    regval &= ~AON_GPADC1_EXT_NEG_SEL_MASK;
    regval |= (0xF << AON_GPADC1_EXT_NEG_SEL_SHIFT);
    putreg32(regval, ADC_AON_BASE + AON_GPADC1_REG_CONFIG2_OFFSET);
    /* config internal channel */
    regval = getreg32(reg_base + AON_GPADC_REG_CMD1_OFFSET);
    regval &= ~AON_GPADC_INT_POS_SEL_MASK;
    regval &= ~AON_GPADC_INT_NEG_SEL_MASK;
    regval |= (chan->pos_chan << AON_GPADC_INT_POS_SEL_SHIFT);
    regval |= (chan->neg_chan << AON_GPADC_INT_NEG_SEL_SHIFT);
    putreg32(regval, reg_base + AON_GPADC_REG_CMD1_OFFSET);

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

    if (dev->idx == 0) {
        /* disable internal regular one-shot channel */
        regval = getreg32(ADC_AON_BASE + AON_GPADC_REG_CMD1_OFFSET);
        regval &= ~AON_GPADC_INT_POS_SEL_MASK;
        regval |= (4 << AON_GPADC_INT_POS_SEL_SHIFT);
        regval &= ~AON_GPADC_INT_NEG_SEL_MASK;
        regval |= (4 << AON_GPADC_INT_NEG_SEL_SHIFT);
        putreg32(regval, ADC_AON_BASE + AON_GPADC_REG_CMD1_OFFSET);
    }

    if (!(getreg32(reg_base + AON_GPADC1_REG_CTRL_0_OFFSET) & AON_GPADC1_SCAN_EN)) {
        if (channels > 1) {
            return -EINVAL;
        }
        regval = getreg32(reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);
        regval &= ~AON_GPADC1_EXT_POS_SEL_MASK;
        regval &= ~AON_GPADC1_EXT_NEG_SEL_MASK;
#if defined(BL616D_VERSION_A0)
        if (dev->idx == 0) {
            regval |= (chan->pos_chan << AON_GPADC1_EXT_POS_SEL_SHIFT);
            regval |= (chan->neg_chan << AON_GPADC1_EXT_NEG_SEL_SHIFT);
        } else {
            regval |= (chan->pos_chan << AON_GPADC1_EXT_NEG_SEL_SHIFT);
            regval |= (chan->neg_chan << AON_GPADC1_EXT_POS_SEL_SHIFT);
        }
#else
        regval |= (chan->pos_chan << AON_GPADC1_EXT_POS_SEL_SHIFT);
        regval |= (chan->neg_chan << AON_GPADC1_EXT_NEG_SEL_SHIFT);
#endif
        putreg32(regval, reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);
    } else {
        if (channels == 0) {
            return -EINVAL;
        } else if (channels <= 8) {
            regval = 0;
            regval2 = 0;
            for (uint8_t i = 0; i < channels; i++) {
                regval |= (chan[i].pos_chan << (i * 4));
                regval2 |= (chan[i].neg_chan << (i * 4));
            }
            for (uint8_t i = channels; i < 8; i++) {
                regval |= (0xF << (i * 4));
                regval2 |= (0xF << (i * 4));
            }
#if defined(BL616D_VERSION_A0)
            if (dev->idx == 0) {
                putreg32(regval, reg_base + AON_GPADC1_REG_SCAN_0_OFFSET);
                putreg32(regval2, reg_base + AON_GPADC1_REG_SCAN_2_OFFSET);
            } else {
                putreg32(regval, reg_base + AON_GPADC1_REG_SCAN_2_OFFSET);
                putreg32(regval2, reg_base + AON_GPADC1_REG_SCAN_0_OFFSET);
            }
#else
            putreg32(regval, reg_base + AON_GPADC1_REG_SCAN_0_OFFSET);
            putreg32(regval2, reg_base + AON_GPADC1_REG_SCAN_2_OFFSET);
#endif
        } else if (channels <= 16) {
            regval = 0;
            regval2 = 0;
            for (uint8_t i = 0; i < 8; i++) {
                regval |= (chan[i].pos_chan << (i * 4));
                regval2 |= (chan[i].neg_chan << (i * 4));
            }
#if defined(BL616D_VERSION_A0)
            if (dev->idx == 0) {
                putreg32(regval, reg_base + AON_GPADC1_REG_SCAN_0_OFFSET);
                putreg32(regval2, reg_base + AON_GPADC1_REG_SCAN_2_OFFSET);
            } else {
                putreg32(regval, reg_base + AON_GPADC1_REG_SCAN_2_OFFSET);
                putreg32(regval2, reg_base + AON_GPADC1_REG_SCAN_0_OFFSET);
            }
#else
            putreg32(regval, reg_base + AON_GPADC1_REG_SCAN_0_OFFSET);
            putreg32(regval2, reg_base + AON_GPADC1_REG_SCAN_2_OFFSET);
#endif

            regval = 0;
            regval2 = 0;
            for (uint8_t i = 0; i < (channels - 8); i++) {
                regval |= (chan[i + 8].pos_chan << (i * 4));
                regval2 |= (chan[i + 8].neg_chan << (i * 4));
            }
            for (uint8_t i = channels - 8; i < 8; i++) {
                regval |= (0xF << (i * 4));
                regval2 |= (0xF << (i * 4));
            }
#if defined(BL616D_VERSION_A0)
            if (dev->idx == 0) {
                putreg32(regval, reg_base + AON_GPADC1_REG_SCAN_1_OFFSET);
                putreg32(regval2, reg_base + AON_GPADC1_REG_SCAN_3_OFFSET);
            } else {
                putreg32(regval, reg_base + AON_GPADC1_REG_SCAN_3_OFFSET);
                putreg32(regval2, reg_base + AON_GPADC1_REG_SCAN_1_OFFSET);
            }
#else
            putreg32(regval, reg_base + AON_GPADC1_REG_SCAN_1_OFFSET);
            putreg32(regval2, reg_base + AON_GPADC1_REG_SCAN_3_OFFSET);
#endif
        } else {
            return -EINVAL;
        }
        regval = getreg32(reg_base + AON_GPADC1_REG_CTRL_0_OFFSET);
        regval &= ~AON_GPADC1_SCAN_LENGTH_MASK;
        regval |= ((channels - 1) << AON_GPADC1_SCAN_LENGTH_SHIFT);
        putreg32(regval, reg_base + AON_GPADC1_REG_CTRL_0_OFFSET);
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

    /* only ADC0 support internal channel */
    if (dev->idx != 0) {
        return -EINVAL;
    }

    if (channels <= 4) {
        /* disable external inject channels */
        putreg32(0xFFFFFFFF, ADC_AON_BASE + AON_GPADC1_REG_IJCT_EXT_0_OFFSET);

        regval = 0;
        regval2 = 0;
        for (uint8_t i = 0; i < channels; i++) {
            regval |= (chan[i].pos_chan << (i * 8));
            regval2 |= (chan[i].neg_chan << (i * 8));
        }
        for (uint8_t i = channels; i < 4; i++) {
            regval |= (0x4 << (i * 8));
            regval2 |= (0x4 << (i * 8));
        }
        putreg32(regval, reg_base + AON_GPADC1_REG_IJCT_INT_0_OFFSET);
        putreg32(regval2, reg_base + AON_GPADC1_REG_IJCT_INT_1_OFFSET);

        regval = getreg32(reg_base + AON_GPADC1_REG_CTRL_0_OFFSET);
        regval &= ~AON_GPADC1_IJCT_LENGTH_MASK;
        regval |= ((channels - 1) << AON_GPADC1_IJCT_LENGTH_SHIFT);
        putreg32(regval, reg_base + AON_GPADC1_REG_CTRL_0_OFFSET);
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
    uint32_t reg_base;

    reg_base = dev->reg_base;

    if (channels <= 4) {
        if (dev->idx == 0) {
            /* disable internal inject channels */
            putreg32(0x04040404, ADC_AON_BASE + AON_GPADC1_REG_IJCT_INT_0_OFFSET);
            putreg32(0x04040404, ADC_AON_BASE + AON_GPADC1_REG_IJCT_INT_1_OFFSET);
        }

        regval = 0;
        for (uint8_t i = 0; i < channels; i++) {
#if defined(BL616D_VERSION_A0)
            if (dev->idx == 0) {
                regval |= (chan[i].pos_chan << (i * 4));
                regval |= (chan[i].neg_chan << (i * 4 + 16));
            } else {
                regval |= (chan[i].neg_chan << (i * 4));
                regval |= (chan[i].pos_chan << (i * 4 + 16));
            }
#else
            regval |= (chan[i].pos_chan << (i * 4));
            regval |= (chan[i].neg_chan << (i * 4 + 16));
#endif
        }
        for (uint8_t i = channels; i < 4; i++) {
            regval |= (0xF << (i * 4));
            regval |= (0xF << (i * 4 + 16));
        }
        putreg32(regval, reg_base + AON_GPADC1_REG_IJCT_EXT_0_OFFSET);

        regval = getreg32(reg_base + AON_GPADC1_REG_CTRL_0_OFFSET);
        regval &= ~AON_GPADC1_IJCT_LENGTH_MASK;
        regval |= ((channels - 1) << AON_GPADC1_IJCT_LENGTH_SHIFT);
        putreg32(regval, reg_base + AON_GPADC1_REG_CTRL_0_OFFSET);
    } else {
        return -EINVAL;
    }
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

    regval = getreg32(reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);
    regval &= ~AON_GPADC1_CONV_START;
    putreg32(regval, reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);

    bflb_mtimer_delay_us(100);

    regval = getreg32(reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);
    regval |= AON_GPADC1_CONV_START;
    putreg32(regval, reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);
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

    regval = getreg32(reg_base + AON_GPADC1_REG_CTRL_0_OFFSET);
    regval &= ~AON_GPADC1_IJCT_START;
    putreg32(regval, reg_base + AON_GPADC1_REG_CTRL_0_OFFSET);

    bflb_mtimer_delay_us(100);

    regval = getreg32(reg_base + AON_GPADC1_REG_CTRL_0_OFFSET);
    regval |= AON_GPADC1_IJCT_START;
    putreg32(regval, reg_base + AON_GPADC1_REG_CTRL_0_OFFSET);
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
    regval = getreg32(reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);
    regval &= ~AON_GPADC1_CONV_START;
    putreg32(regval, reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);
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
    regval = getreg32(reg_base + AON_GPADC1_REG_CTRL_0_OFFSET);
    regval &= ~AON_GPADC1_IJCT_START;
    putreg32(regval, reg_base + AON_GPADC1_REG_CTRL_0_OFFSET);
#endif
}

uint8_t bflb_adc_get_count(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_adc_get_count
    return romapi_bflb_adc_get_count(dev);
#else
    if ((dev->idx == 1) && bflb_adc_fifo1_is_enabled()) {
        return ((getreg32(ADC_GPIP_BASE + GPIP_GPADC_FIFO2_CONFIG_OFFSET) & GPIP_GPADC_FIFO2_DATA_COUNT_MASK) >> GPIP_GPADC_FIFO2_DATA_COUNT_SHIFT);
    } else {
        return ((getreg32(ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET) & GPIP_GPADC_FIFO_DATA_COUNT_MASK) >> GPIP_GPADC_FIFO_DATA_COUNT_SHIFT);
    }
#endif
}

uint8_t bflb_adc_get_count_inject(struct bflb_device_s *dev, uint8_t idx)
{
#ifdef romapi_bflb_adc_get_count_inject
    return romapi_bflb_adc_get_count_inject(dev, idx);
#else
    uint32_t regval;

    if ((dev->idx == 1) && bflb_adc_fifo1_is_enabled()) {
        regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_FIFO2_CONFIG_OFFSET);
    } else {
        regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
    }
    regval >>= (idx * 4);
    regval &= GPIP_GPADC_IJCT_FIFO0_DATA_COUNT_MASK;
    regval >>= GPIP_GPADC_IJCT_FIFO0_DATA_COUNT_SHIFT;
    return (uint8_t)regval;
#endif
}

uint32_t bflb_adc_read_raw(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_adc_read_raw
    return romapi_bflb_adc_read_raw(dev);
#else
#if defined(BL616D_VERSION_A0)
    uint32_t data, pos_sel, neg_sel;
    if ((dev->idx == 1) && bflb_adc_fifo1_is_enabled()) {
        data = getreg32(ADC_GPIP_BASE + GPIP_GPADC_REGULAR_FIFO2_OFFSET) & GPIP_GPADC_RGLR_FIFO2_RDATA_MASK;
    } else {
        data = getreg32(ADC_GPIP_BASE + GPIP_GPADC_REGULAR_FIFO_OFFSET) & GPIP_GPADC_RGLR_FIFO_RDATA_MASK;
    }
    if (data & (1 << 29)) {
        pos_sel = (data >> 20) & 0xF;
        neg_sel = (data >> 16) & 0xF;
        data &= ~((0xF << 20) | (0xF << 16));
        data |= (pos_sel << 16) | (neg_sel << 20);
    }
    return data;
#else
    if ((dev->idx == 1) && bflb_adc_fifo1_is_enabled()) {
        return getreg32(ADC_GPIP_BASE + GPIP_GPADC_REGULAR_FIFO2_OFFSET) & GPIP_GPADC_RGLR_FIFO2_RDATA_MASK;
    } else {
        return getreg32(ADC_GPIP_BASE + GPIP_GPADC_REGULAR_FIFO_OFFSET) & GPIP_GPADC_RGLR_FIFO_RDATA_MASK;
    }
#endif
#endif
}

uint32_t bflb_adc_read_raw_inject(struct bflb_device_s *dev, uint8_t idx)
{
#ifdef romapi_bflb_adc_read_raw_inject
    return romapi_bflb_adc_read_raw_inject(dev, idx);
#else
    uint32_t reg_addr;

    if ((dev->idx == 1) && bflb_adc_fifo1_is_enabled()) {
        reg_addr = ADC_GPIP_BASE + GPIP_GPADC_INJECTION_FIFO4_OFFSET;
    } else {
        reg_addr = ADC_GPIP_BASE + GPIP_GPADC_INJECTION_FIFO0_OFFSET;
    }
#if defined(BL616D_VERSION_A0)
    uint32_t data, pos_sel, neg_sel;
    data = getreg32(reg_addr + idx * 4) & GPIP_GPADC_IJCT_FIFO0_RDATA_MASK;
    if (data & (1 << 29)) {
        pos_sel = (data >> 20) & 0xF;
        neg_sel = (data >> 16) & 0xF;
        data &= ~((0xF << 20) | (0xF << 16));
        data |= (pos_sel << 16) | (neg_sel << 20);
    }
    return data;
#else
    return getreg32(reg_addr + idx * 4) & GPIP_GPADC_IJCT_FIFO0_RDATA_MASK;
#endif
#endif
}

void bflb_adc_clear_fifo(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_adc_clear_fifo
    romapi_bflb_adc_clear_fifo(dev);
#else
    uint32_t regval;
    uint32_t reg_addr;

    if ((dev->idx == 1) && bflb_adc_fifo1_is_enabled()) {
        reg_addr = ADC_GPIP_BASE + GPIP_GPADC_FIFO2_CONFIG_OFFSET;
    } else {
        reg_addr = ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET;
    }
    regval = getreg32(reg_addr);
    regval |= GPIP_GPADC_FIFO_CLR;
    putreg32(regval, reg_addr);
#endif
}

void bflb_adc_clear_fifo_inject(struct bflb_device_s *dev, uint8_t idx)
{
#ifdef romapi_bflb_adc_clear_fifo_inject
    romapi_bflb_adc_clear_fifo_inject(dev, idx);
#else
    uint32_t regval;
    uint32_t reg_addr;

    if ((dev->idx == 1) && bflb_adc_fifo1_is_enabled()) {
        reg_addr = ADC_GPIP_BASE + GPIP_GPADC_FIFO2_CONFIG_OFFSET;
    } else {
        reg_addr = ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET;
    }
    regval = getreg32(reg_addr);
    regval |= (GPIP_GPADC_IJCT_FIFO0_CLR << idx);
    putreg32(regval, reg_addr);
#endif
}

void bflb_adc_int_saturation_mask(struct bflb_device_s *dev, uint32_t int_type, bool mask)
{
#ifdef romapi_bflb_adc_int_saturation_mask
    romapi_bflb_adc_int_saturation_mask(dev, int_type, mask);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    int_type &= ADC_INT_SATURATION_ALL;
    int_type <<= 8;
    if (dev->idx == 0) {
        regval = getreg32(reg_base + AON_GPADC1_REG_SATUR_OFFSET);
    } else {
        regval = getreg32(reg_base + AON_GPADC1_REG_SATUR_OFFSET - AON_GPADC2_OFFSET_COMPENSATION);
    }
    if (mask) {
        regval |= int_type;
    } else {
        regval &= ~int_type;
    }
    if (dev->idx == 0) {
        putreg32(regval, reg_base + AON_GPADC1_REG_SATUR_OFFSET);
    } else {
        putreg32(regval, reg_base + AON_GPADC1_REG_SATUR_OFFSET - AON_GPADC2_OFFSET_COMPENSATION);
    }
#endif
}

void bflb_adc_int_watchdog_mask(struct bflb_device_s *dev, uint32_t int_type, bool mask)
{
#ifdef romapi_bflb_adc_int_watchdog_mask
    romapi_bflb_adc_int_watchdog_mask(dev, int_type, mask);
#else
    uint32_t regval;

    int_type &= ADC_INT_WATCHDOG_ALL;
    regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_DMA_INT_MASK_OFFSET);
    if (mask) {
        regval |= int_type;
    } else {
        regval &= ~int_type;
    }
    putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_DMA_INT_MASK_OFFSET);
#endif
}

void bflb_adc_int_done_mask(struct bflb_device_s *dev, uint32_t int_type, bool mask)
{
#ifdef romapi_bflb_adc_int_done_mask
    romapi_bflb_adc_int_done_mask(dev, int_type, mask);
#else
    uint32_t regval;

    int_type &= ADC_INT_DONE_ALL;
    if (dev->idx == 1) {
        int_type <<= 2;
    }
    regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_DMA_INT_MASK_OFFSET);
    if (mask) {
        regval |= int_type;
    } else {
        regval &= ~int_type;
    }
    putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_DMA_INT_MASK_OFFSET);
#endif
}

void bflb_adc_int_fifo_mask(struct bflb_device_s *dev, uint32_t int_type, bool mask)
{
#ifdef romapi_bflb_adc_int_fifo_mask
    romapi_bflb_adc_int_fifo_mask(dev, int_type, mask);
#else
    uint32_t regval;
    uint32_t reg_addr;

    int_type &= ADC_INT_FIFO_ALL;
    if ((dev->idx == 1) && bflb_adc_fifo1_is_enabled()) {
        reg_addr = ADC_GPIP_BASE + GPIP_GPADC2_DMA_INT_MASK_OFFSET;
    } else {
        reg_addr = ADC_GPIP_BASE + GPIP_GPADC_DMA_INT_MASK_OFFSET;
    }
    regval = getreg32(reg_addr);
    if (mask) {
        regval |= int_type;
    } else {
        regval &= ~int_type;
    }
    putreg32(regval, reg_addr);
#endif
}

uint32_t bflb_adc_get_int_saturation_status(struct bflb_device_s *dev, uint32_t int_type)
{
#ifdef romapi_bflb_adc_get_int_saturation_status
    return romapi_bflb_adc_get_int_saturation_status(dev, int_type);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    if (dev->idx == 0) {
        regval = getreg32(reg_base + AON_GPADC1_REG_SATUR_OFFSET);
    } else {
        regval = getreg32(reg_base + AON_GPADC1_REG_SATUR_OFFSET - AON_GPADC2_OFFSET_COMPENSATION);
    }
    regval &= ADC_INT_SATURATION_ALL;
    return (regval & int_type);
#endif
}

uint32_t bflb_adc_get_int_watchdog_status(struct bflb_device_s *dev, uint32_t int_type)
{
#ifdef romapi_bflb_adc_get_int_watchdog_status
    return romapi_bflb_adc_get_int_watchdog_status(dev, int_type);
#else
    uint32_t regval;

    regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_DMA_INT_STS_OFFSET);
    regval &= ADC_INT_WATCHDOG_ALL;
    return (regval & int_type);
#endif
}

uint32_t bflb_adc_get_int_done_status(struct bflb_device_s *dev, uint32_t int_type)
{
#ifdef romapi_bflb_adc_get_int_done_status
    return romapi_bflb_adc_get_int_done_status(dev, int_type);
#else
    uint32_t regval;

    regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_DMA_INT_STS_OFFSET);
    if (dev->idx == 1) {
        regval >>= 2;
    }
    regval &= ADC_INT_DONE_ALL;
    return (regval & int_type);
#endif
}

uint32_t bflb_adc_get_int_fifo_status(struct bflb_device_s *dev, uint32_t int_type)
{
#ifdef romapi_bflb_adc_get_int_fifo_status
    return romapi_bflb_adc_get_int_fifo_status(dev, int_type);
#else
    uint32_t regval;
    uint32_t reg_addr;

    if ((dev->idx == 1) && bflb_adc_fifo1_is_enabled()) {
        reg_addr = ADC_GPIP_BASE + GPIP_GPADC2_DMA_INT_STS_OFFSET;
    } else {
        reg_addr = ADC_GPIP_BASE + GPIP_GPADC_DMA_INT_STS_OFFSET;
    }
    regval = getreg32(reg_addr);
    regval &= ADC_INT_FIFO_ALL;
    return (regval & int_type);
#endif
}

void bflb_adc_int_saturation_clear(struct bflb_device_s *dev, uint32_t int_type)
{
#ifdef romapi_bflb_adc_int_saturation_clear
    romapi_bflb_adc_int_saturation_clear(dev, int_type);
#else
    uint32_t regval;
    uint32_t reg_addr;

    if (dev->idx == 0) {
        reg_addr = dev->reg_base + AON_GPADC1_REG_SATUR_OFFSET;
    } else {
        reg_addr = dev->reg_base + AON_GPADC1_REG_SATUR_OFFSET - AON_GPADC2_OFFSET_COMPENSATION;
    }
    int_type &= ADC_INT_SATURATION_ALL;
    int_type <<= 4;
    /* write 1 to clear flag */
    regval = getreg32(reg_addr);
    regval |= int_type;
    putreg32(regval, reg_addr);
    /* write 0 to release clear operation */
    regval = getreg32(reg_addr);
    regval &= ~int_type;
    putreg32(regval, reg_addr);
#endif
}

void bflb_adc_int_watchdog_clear(struct bflb_device_s *dev, uint32_t int_type)
{
#ifdef romapi_bflb_adc_int_watchdog_clear
    romapi_bflb_adc_int_watchdog_clear(dev, int_type);
#else
    uint32_t regval;

    int_type &= ADC_INT_WATCHDOG_ALL;
    regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_DMA_INT_CLEAR_OFFSET);
    regval |= int_type;
    putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_DMA_INT_CLEAR_OFFSET);
#endif
}

void bflb_adc_int_done_clear(struct bflb_device_s *dev, uint32_t int_type)
{
#ifdef romapi_bflb_adc_int_done_clear
    romapi_bflb_adc_int_done_clear(dev, int_type);
#else
    uint32_t regval;

    int_type &= ADC_INT_DONE_ALL;
    if (dev->idx == 1) {
        int_type <<= 2;
    }
    regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_DMA_INT_CLEAR_OFFSET);
    regval |= int_type;
    putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_DMA_INT_CLEAR_OFFSET);
#endif
}

void bflb_adc_int_fifo_clear(struct bflb_device_s *dev, uint32_t int_type)
{
#ifdef romapi_bflb_adc_int_fifo_clear
    romapi_bflb_adc_int_fifo_clear(dev, int_type);
#else
    uint32_t regval;
    uint32_t reg_addr;

    int_type &= ADC_INT_FIFO_ALL;
    if ((dev->idx == 1) && bflb_adc_fifo1_is_enabled()) {
        reg_addr = ADC_GPIP_BASE + GPIP_GPADC2_DMA_INT_CLEAR_OFFSET;
    } else {
        reg_addr = ADC_GPIP_BASE + GPIP_GPADC_DMA_INT_CLEAR_OFFSET;
    }
    regval = getreg32(reg_addr);
    regval |= int_type;
    putreg32(regval, reg_addr);
#endif
}

void bflb_adc_parse_result(struct bflb_device_s *dev, uint32_t *buffer, struct bflb_adc_result_s *result, uint16_t count)
{
#ifdef romapi_bflb_adc_parse_result
    romapi_bflb_adc_parse_result(dev, buffer, result, count);
#else
    uint32_t reg_base;
    uint32_t ref = 1800;
    uint32_t conv_result;
    uint32_t resolution;
    uint32_t diff_mode;
    uint32_t neg;

    reg_base = dev->reg_base;

    resolution = (getreg32(reg_base + AON_GPADC1_REG_CTRL_0_OFFSET) & AON_GPADC1_RES_SEL_MASK) >> AON_GPADC1_RES_SEL_SHIFT;
    diff_mode = getreg32(reg_base + AON_GPADC1_REG_CONFIG1_OFFSET) & AON_GPADC1_DIFF_MODE;

    if (diff_mode == 0) {
        /* single-ended mode  */
        for (uint16_t i = 0; i < count; i++) {
            result[i].adc_id = (buffer[i] >> 29) & 1;
            result[i].idx = (buffer[i] >> 24) & 0xF;
            result[i].pos_chan = (buffer[i] >> 20) & 0xF;
            result[i].neg_chan = (buffer[i] >> 16) & 0xF;
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
            result[i].adc_id = (buffer[i] >> 29) & 1;
            result[i].idx = (buffer[i] >> 24) & 0xF;
            result[i].pos_chan = (buffer[i] >> 20) & 0xF;
            result[i].neg_chan = (buffer[i] >> 16) & 0xF;

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

    if (dev->idx != 0) {
        return;
    }

    regval = getreg32(reg_base + AON_GPADC_REG_CMD1_OFFSET);
    regval |= AON_GPADC_TS_EN;
    regval &= ~AON_GPADC_TSVBE_LOW;
    if (tsen_mod) {
        regval |= AON_GPADC_TSEXT_SEL;
    } else {
        regval &= ~AON_GPADC_TSEXT_SEL;
    }
    putreg32(regval, reg_base + AON_GPADC_REG_CMD1_OFFSET);

    regval = getreg32(reg_base + AON_GPADC1_REG_CONFIG1_OFFSET);
    regval &= ~AON_GPADC1_PGA_VCMI_EN;
    regval &= ~AON_GPADC1_CHOP_MODE_MASK;
    regval |= (1 << AON_GPADC1_CHOP_MODE_SHIFT); /* Vref AZ */
    regval &= ~AON_GPADC1_PGA_VCM_MASK;
    regval |= (1 << AON_GPADC1_PGA_VCM_SHIFT);
    putreg32(regval, reg_base + AON_GPADC1_REG_CONFIG1_OFFSET);

    regval = getreg32(reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);
    regval |= AON_GPADC1_DWA_EN;
    regval &= ~AON_GPADC1_DLY_SEL_MASK;
    regval |= (2 << AON_GPADC1_DLY_SEL_SHIFT);
    regval |= AON_GPADC1_DITHER_EN;
    putreg32(regval, reg_base + AON_GPADC1_REG_CONFIG2_OFFSET);
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

    regval = getreg32(reg_base + AON_GPADC_REG_CMD1_OFFSET);
    regval &= ~AON_GPADC_TSVBE_LOW;
    putreg32(regval, reg_base + AON_GPADC_REG_CMD1_OFFSET);

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

    regval = getreg32(reg_base + AON_GPADC_REG_CMD1_OFFSET);
    regval |= AON_GPADC_TSVBE_LOW;
    putreg32(regval, reg_base + AON_GPADC_REG_CMD1_OFFSET);

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
        temp = (((float)v0 - (float)v1) - (float)tsen_offset) / 9.2f;
    } else {
        temp = (((float)v1 - (float)v0) - (float)tsen_offset) / 9.2f;
    }

    return temp;
#endif
}

void bflb_adc_watchdog_init(struct bflb_device_s *dev, struct bflb_adc_watchdog_config_s *config)
{
#ifdef romapi_bflb_adc_watchdog_init
    romapi_bflb_adc_watchdog_init(dev, config);
#else
    uint32_t regval;
    uint32_t ch_sel;

    /* disable watch dog */
    regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_WD_CTRL_0_OFFSET);
    regval &= ~GPIP_GPADC_WD_EN;
    putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_WD_CTRL_0_OFFSET);
    /* config GPADC_WD_CTRL_0 */
    regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_WD_CTRL_0_OFFSET);
    regval &= ~GPIP_GPADC_WD_CNTR_TH_MASK;
    regval |= (config->counter_threshold << GPIP_GPADC_WD_CNTR_TH_SHIFT);
    regval &= ~GPIP_GPADC_WD_CH_SEL_MASK;
    ch_sel = config->channel_id;
    if (config->channel_type) {
        ch_sel |= (1 << 4);
    }
    if (config->adc_id) {
        ch_sel |= (1 << 5);
    }
    regval |= (ch_sel << GPIP_GPADC_WD_CH_SEL_SHIFT);
    putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_WD_CTRL_0_OFFSET);
    /* config GPADC_WD_CTRL_1 */
    regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_WD_CTRL_1_OFFSET);
    regval &= ~GPIP_GPADC_WD_UPPER_TH_MASK;
    regval |= (config->upper_threshold << GPIP_GPADC_WD_UPPER_TH_SHIFT);
    regval &= ~GPIP_GPADC_WD_LOWER_TH_MASK;
    regval |= (config->lower_threshold << GPIP_GPADC_WD_LOWER_TH_SHIFT);
    putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_WD_CTRL_1_OFFSET);
    /* enable of disable watch dog */
    if (config->enable) {
        regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_WD_CTRL_0_OFFSET);
        regval |= GPIP_GPADC_WD_EN;
        putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_WD_CTRL_0_OFFSET);
    }
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
                regval = getreg32(ADC_AON_BASE + AON_GPADC_REG_CMD1_OFFSET);
                regval |= AON_GPADC_VBAT_EN;
                putreg32(regval, ADC_AON_BASE + AON_GPADC_REG_CMD1_OFFSET);
            } else {
                regval = getreg32(ADC_AON_BASE + AON_GPADC_REG_CMD1_OFFSET);
                regval &= ~AON_GPADC_VBAT_EN;
                putreg32(regval, ADC_AON_BASE + AON_GPADC_REG_CMD1_OFFSET);
            }
            break;
        case ADC_CMD_REGULAR_DATA_SHIFT:
            regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_PRE_PROC_OFFSET);
            regval &= ~GPIP_GPADC_RGLR_DATA_SHIFT_MASK;
            regval |= (arg << GPIP_GPADC_RGLR_DATA_SHIFT_SHIFT);
            putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_PRE_PROC_OFFSET);
            break;
        case ADC_CMD_INJECT_DATA_SHIFT:
            regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_PRE_PROC_OFFSET);
            regval &= GPIP_GPADC_IJCT_DATA_SHIFT_MASK;
            regval |= (arg << GPIP_GPADC_IJCT_DATA_SHIFT_SHIFT);
            putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_PRE_PROC_OFFSET);
            break;
        case ADC_CMD_CONV_DELAY:
            regval = getreg32(reg_base + AON_GPADC1_REG_CTRL_0_OFFSET);
            regval &= ~AON_GPADC1_CONV_DELAY_MASK;
            regval |= (arg << AON_GPADC1_CONV_DELAY_SHIFT);
            putreg32(regval, reg_base + AON_GPADC1_REG_CTRL_0_OFFSET);
            break;
        case ADC_CMD_PWM_TRIG_EN:
            regval = getreg32(reg_base + AON_GPADC1_REG_CTRL_0_OFFSET);
            if (arg) {
                regval |= AON_GPADC1_PWM_TRG_EN;
            } else {
                regval &= ~AON_GPADC1_PWM_TRG_EN;
            }
            putreg32(regval, reg_base + AON_GPADC1_REG_CTRL_0_OFFSET);
            break;
        case ADC_CMD_SET_FIFO_THRESHOLD:
            if (dev->idx == 0) {
                regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
                regval &= ~GPIP_GPADC_FIFO_THL_MASK;
                regval |= (arg << GPIP_GPADC_FIFO_THL_SHIFT);
                putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_CONFIG_OFFSET);
            } else {
                regval = getreg32(ADC_GPIP_BASE + GPIP_GPADC_FIFO2_CONFIG_OFFSET);
                regval &= ~GPIP_GPADC_FIFO2_THL_MASK;
                regval |= (arg << GPIP_GPADC_FIFO2_THL_SHIFT);
                putreg32(regval, ADC_GPIP_BASE + GPIP_GPADC_FIFO2_CONFIG_OFFSET);
            }
            break;
        case ADC_CMD_SET_OFFSET_CALI:
        /*adc os_cal_offset change, arg is offset value, pos value is down offset, neg value is up offset*/
            regval = getreg32(reg_base + AON_GPADC1_REG_OS_CAL_DATA_OFFSET);
            regval &= ~AON_GPADC1_OS_CAL_DATA_MASK;
            regval |= ((uint32_t)(arg) << AON_GPADC1_OS_CAL_DATA_SHIFT) & AON_GPADC1_OS_CAL_DATA_MASK;
            putreg32(regval, reg_base + AON_GPADC1_REG_OS_CAL_DATA_OFFSET);
            break;
        default:
            ret = -EPERM;
            break;
    }
    return ret;
#endif
}
