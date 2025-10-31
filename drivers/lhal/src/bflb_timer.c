#include "bflb_timer.h"
#include "bflb_gpio.h"
#include "hardware/timer_reg.h"
#if defined(BL616L)
#include "bl616l_glb.h"
#elif defined(BL616)
#include "bl616_glb.h"
#elif defined(BL616D)
#include "bl616d_glb.h"
#endif

void bflb_timer_init(struct bflb_device_s *dev, const struct bflb_timer_config_s *config)
{
    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_TIMER_COUNTER_MODE(config->counter_mode));
    LHAL_PARAM_ASSERT(IS_TIMER_CLK_SOURCE(config->clock_source));
    LHAL_PARAM_ASSERT(IS_TIMER_CLOCK_DIV(config->clock_div));
    LHAL_PARAM_ASSERT(IS_TIMER_COMP_ID(config->trigger_comp_id));

    if (!IS_TIMER_COMP_VAL(config->comp0_val) ||
        !IS_TIMER_COMP_VAL(config->comp1_val) ||
        !IS_TIMER_COMP_VAL(config->comp2_val)) {
        bflb_lhal_assert_func(__FILE__, __LINE__, __func__, "comp value is error");
    }

#ifdef romapi_bflb_timer_init
    romapi_bflb_timer_init(dev, config);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    /* Disable timer */
    regval = getreg32(reg_base + TIMER_TCER_OFFSET);
    regval &= ~(1 << (dev->idx + 1));
    putreg32(regval, reg_base + TIMER_TCER_OFFSET);

    /* Timer interrupr clear */
    bflb_timer_compint_clear(dev, TIMER_COMP_ID_0);
    bflb_timer_compint_clear(dev, TIMER_COMP_ID_1);
    bflb_timer_compint_clear(dev, TIMER_COMP_ID_2);

    /* Configure clock source */
    regval = getreg32(reg_base + TIMER_TCCR_OFFSET);
    if (dev->idx == 0) {
        regval &= ~TIMER_CS_0_MASK;
        regval |= (config->clock_source << TIMER_CS_0_SHIFT);
    } else {
        regval &= ~TIMER_CS_1_MASK;
        regval |= (config->clock_source << TIMER_CS_1_SHIFT);
    }
    putreg32(regval, reg_base + TIMER_TCCR_OFFSET);

    /* Configure clock div */
    regval = getreg32(reg_base + TIMER_TCDR_OFFSET);
    regval &= ~(0xff << (TIMER_TCDR0_SHIFT + 8 * dev->idx));
    regval |= (config->clock_div << (TIMER_TCDR0_SHIFT + 8 * dev->idx));
    putreg32(regval, reg_base + TIMER_TCDR_OFFSET);

    /* Configure counter mode */
    regval = getreg32(reg_base + TIMER_TCMR_OFFSET);
    if (config->counter_mode == TIMER_COUNTER_MODE_PROLOAD) {
        regval &= ~(1 << (1 + dev->idx));
    } else {
        regval |= (1 << (1 + dev->idx));
    }
    putreg32(regval, reg_base + TIMER_TCMR_OFFSET);

    /* Configure preload trigger source */
    regval = getreg32(reg_base + TIMER_TPLCR0_OFFSET + 4 * dev->idx);
    regval &= ~TIMER_TPLCR0_MASK;
    if (config->trigger_comp_id != TIMER_COMP_NONE) {
        regval |= ((config->trigger_comp_id + 1) << TIMER_TPLCR0_SHIFT);
    }
    putreg32(regval, reg_base + TIMER_TPLCR0_OFFSET + 4 * dev->idx);

    if (config->counter_mode == TIMER_COUNTER_MODE_PROLOAD) {
        bflb_timer_set_preloadvalue(dev, config->preload_val);
    }

    if (config->trigger_comp_id < TIMER_COMP_ID_1) {
        bflb_timer_compint_mask(dev, TIMER_COMP_ID_0, false);
        bflb_timer_compint_mask(dev, TIMER_COMP_ID_1, true);
        bflb_timer_compint_mask(dev, TIMER_COMP_ID_2, true);
        bflb_timer_set_compvalue(dev, TIMER_COMP_ID_0, config->comp0_val - 2);
        bflb_timer_set_compvalue(dev, TIMER_COMP_ID_1, 0xffffffff);
        bflb_timer_set_compvalue(dev, TIMER_COMP_ID_2, 0xffffffff);
    } else if (config->trigger_comp_id < TIMER_COMP_ID_2) {
        bflb_timer_compint_mask(dev, TIMER_COMP_ID_0, false);
        bflb_timer_compint_mask(dev, TIMER_COMP_ID_1, false);
        bflb_timer_compint_mask(dev, TIMER_COMP_ID_2, true);
        bflb_timer_set_compvalue(dev, TIMER_COMP_ID_0, config->comp0_val - 2);
        bflb_timer_set_compvalue(dev, TIMER_COMP_ID_1, config->comp1_val - 2);
        bflb_timer_set_compvalue(dev, TIMER_COMP_ID_2, 0xffffffff);
    } else if (config->trigger_comp_id < TIMER_COMP_NONE) {
        bflb_timer_compint_mask(dev, TIMER_COMP_ID_0, false);
        bflb_timer_compint_mask(dev, TIMER_COMP_ID_1, false);
        bflb_timer_compint_mask(dev, TIMER_COMP_ID_2, false);
        bflb_timer_set_compvalue(dev, TIMER_COMP_ID_0, config->comp0_val - 2);
        bflb_timer_set_compvalue(dev, TIMER_COMP_ID_1, config->comp1_val - 2);
        bflb_timer_set_compvalue(dev, TIMER_COMP_ID_2, config->comp2_val - 2);
    } else {
        bflb_timer_compint_mask(dev, TIMER_COMP_ID_0, true);
        bflb_timer_compint_mask(dev, TIMER_COMP_ID_1, true);
        bflb_timer_compint_mask(dev, TIMER_COMP_ID_2, true);
        bflb_timer_set_compvalue(dev, TIMER_COMP_ID_0, 0xffffffff);
        bflb_timer_set_compvalue(dev, TIMER_COMP_ID_1, 0xffffffff);
        bflb_timer_set_compvalue(dev, TIMER_COMP_ID_2, 0xffffffff);
    }
#endif
}

void bflb_timer_deinit(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_timer_deinit
    romapi_bflb_timer_deinit(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TIMER_TCER_OFFSET);
    regval &= ~(1 << (dev->idx + 1));
    putreg32(regval, reg_base + TIMER_TCER_OFFSET);
#endif
}

void bflb_timer_start(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_timer_start
    romapi_bflb_timer_start(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TIMER_TCER_OFFSET);
    regval |= (1 << (dev->idx + 1));
    putreg32(regval, reg_base + TIMER_TCER_OFFSET);
#endif
}

void bflb_timer_stop(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_timer_stop
    romapi_bflb_timer_stop(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + TIMER_TCER_OFFSET);
    regval &= ~(1 << (dev->idx + 1));
    putreg32(regval, reg_base + TIMER_TCER_OFFSET);
#endif
}

void bflb_timer_set_preloadvalue(struct bflb_device_s *dev, uint32_t val)
{
#ifdef romapi_bflb_timer_set_preloadvalue
    romapi_bflb_timer_set_preloadvalue(dev, val);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;

    putreg32(val, reg_base + TIMER_TPLVR0_OFFSET + 4 * dev->idx);
#endif
}

void bflb_timer_set_compvalue(struct bflb_device_s *dev, uint8_t cmp_no, uint32_t val)
{
#ifdef romapi_bflb_timer_set_compvalue
    romapi_bflb_timer_set_compvalue(dev, cmp_no, val);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;

    putreg32(val, reg_base + TIMER_TMR0_0_OFFSET + 0x0c * dev->idx + 4 * cmp_no);
#endif
}

uint32_t bflb_timer_get_compvalue(struct bflb_device_s *dev, uint8_t cmp_no)
{
#ifdef romapi_bflb_timer_get_compvalue
    return romapi_bflb_timer_get_compvalue(dev, cmp_no);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;

    return getreg32(reg_base + TIMER_TMR0_0_OFFSET + 0x0c * dev->idx + 4 * cmp_no);
#endif
}

uint32_t bflb_timer_get_countervalue(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_timer_get_countervalue
    return romapi_bflb_timer_get_countervalue(dev);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;

    return getreg32(reg_base + TIMER_TCR0_OFFSET + 4 * dev->idx);
#endif
}

void bflb_timer_compint_mask(struct bflb_device_s *dev, uint8_t cmp_no, bool mask)
{
#ifdef romapi_bflb_timer_compint_mask
    return romapi_bflb_timer_compint_mask(dev, cmp_no, mask);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + TIMER_TIER0_OFFSET + 4 * dev->idx);
    if (mask) {
        regval &= ~(1 << cmp_no);
    } else {
        regval |= (1 << cmp_no);
    }
    putreg32(regval, reg_base + TIMER_TIER0_OFFSET + 4 * dev->idx);
#endif
}

bool bflb_timer_get_compint_status(struct bflb_device_s *dev, uint8_t cmp_no)
{
#ifdef romapi_bflb_timer_get_compint_status
    return romapi_bflb_timer_get_compint_status(dev, cmp_no);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + TIMER_TSR0_OFFSET + 4 * dev->idx);

    if (regval & (1 << cmp_no)) {
        return true;
    } else {
        return false;
    }
#endif
}

void bflb_timer_compint_clear(struct bflb_device_s *dev, uint8_t cmp_no)
{
#ifdef romapi_bflb_timer_compint_clear
    romapi_bflb_timer_compint_clear(dev, cmp_no);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + TIMER_TICR0_OFFSET + 4 * dev->idx);
    regval |= (1 << cmp_no);
    putreg32(regval, reg_base + TIMER_TICR0_OFFSET + 4 * dev->idx);
#endif
}

int bflb_timer_feature_control(struct bflb_device_s *dev, int cmd, size_t arg)
{
#ifdef romapi_bflb_timer_feature_control
    return romapi_bflb_timer_feature_control(dev, cmd, arg);
#else
    int ret = 0;
#if defined(BL616L)
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
#endif

    switch (cmd) {
#if defined(BL616L)
        /* Select compare id to send dma request, arg use @ref TIMER_DMA_COMP_ID */
        case TIMER_CMD_DMA_REQUEST_SET_COMPARE_ID:
            regval = getreg32(reg_base + TIMER_TIER0_OFFSET + 4 * dev->idx);
            regval &= ~TIMER0_DMA_REQ_EN_MASK;
            regval |= arg << TIMER0_DMA_REQ_EN_SHIFT;
            putreg32(regval, reg_base + TIMER_TIER0_OFFSET + 4 * dev->idx);
            break;

        /* Enable or disable gpio pulse measure function, only timer0 support, arg use true or false */
        case TIMER_CMD_GPIO_PULSE_SET_ENABLE:
            if (dev->idx != 0) {
                ret = -EPERM;
            } else {
                regval = getreg32(reg_base + TIMER_GPIO_OFFSET);
                if (arg) {
                    regval |= TIMER0_GPIO_EN;
                } else {
                    regval &= ~TIMER0_GPIO_EN;
                }
                putreg32(regval, reg_base + TIMER_GPIO_OFFSET);
            }
            break;

        /* Set gpio pulse width measure interrupt trigger mode, only timer0 support, arg use @ref TIMER_GPIO_PULSE_INT_MODE */
        case TIMER_CMD_GPIO_PULSE_SET_INT_MODE:
            if (dev->idx != 0) {
                ret = -EPERM;
            } else {
                regval = getreg32(reg_base + TIMER_GPIO_OFFSET);
                if (arg) {
                    regval |= TIMER_GPIO_LAT_DET_MODE;
                } else {
                    regval &= ~TIMER_GPIO_LAT_DET_MODE;
                }
                putreg32(regval, reg_base + TIMER_GPIO_OFFSET);
            }
            break;

        case TIMER_CMD_GPIO_PULSE_GET_BUSY:
            if (dev->idx != 0) {
                ret = -EPERM;
            } else {
                regval = getreg32(reg_base + TIMER_GPIO_OFFSET);
                if (regval & TIMER_GPIO_LAT_OK) {
                    ret = 0;
                } else {
                    ret = 1;
                }
            }
            break;

        case TIMER_CMD_GPIO_PULSE_GET_VALUE:
            if (dev->idx != 0) {
                ret = -EPERM;
            } else {
                switch (arg) {
                    case TIMER_GPIO_PULSE_VALUE_0:
                        ret = (int)(getreg32(reg_base + TIMER_GPIO_LAT1_OFFSET));
                        break;

                    case TIMER_GPIO_PULSE_VALUE_1:
                        ret = (int)(getreg32(reg_base + TIMER_GPIO_LAT2_OFFSET));
                        break;

                    case TIMER_GPIO_PULSE_VALUE_2:
                        ret = (int)(getreg32(reg_base + TIMER_GPIO_LAT3_OFFSET));
                        break;

                    case TIMER_GPIO_PULSE_VALUE_3:
                        ret = (int)(getreg32(reg_base + TIMER_GPIO_LAT4_OFFSET));
                        break;

                    default:
                        break;
                }
            }
            break;

        case TIMER_CMD_GPIO_PULSE_SET_POLARITY:
            regval = getreg32(reg_base + TIMER_GPIO_OFFSET);
            if (arg) {
                regval |= 1 << (5 + dev->idx);
            } else {
                regval &= ~(1 << (5 + dev->idx));
            }
            putreg32(regval, reg_base + TIMER_GPIO_OFFSET);
            break;

        case TIMER_CMD_GPIO_PULSE_INT_MASK:
            if (dev->idx != 0) {
                ret = -EPERM;
            } else {
                regval = getreg32(reg_base + TIMER_TIER0_OFFSET);
                if (arg) {
                    regval &= ~TIMER_GPIO_LAT_IRQ_EN;
                } else {
                    regval |= TIMER_GPIO_LAT_IRQ_EN;
                }
                putreg32(regval, reg_base + TIMER_TIER0_OFFSET);
            }
            break;

        case TIMER_CMD_GPIO_PULSE_INT_CLEAR:
            if (dev->idx != 0) {
                ret = -EPERM;
            } else {
                regval = getreg32(reg_base + TIMER_TICR0_OFFSET);
                regval |= TIMER_GPIO_LAT_IRQ_CLR;
                putreg32(regval, reg_base + TIMER_TICR0_OFFSET);
            }
            break;

        case TIMER_CMD_READ_HW_VERSION:
            regval = getreg32(reg_base + TIMER_HW_VERSION_OFFSET);
            ret = (regval & TIMER_HW_VERSION_MASK) >> TIMER_HW_VERSION_SHIFT;
            break;

        case TIMER_CMD_READ_SW_USAGE:
            regval = getreg32(reg_base + TIMER_SW_USAGE_OFFSET);
            ret = (regval & TIMER_SW_USAGE_MASK) >> TIMER_SW_USAGE_SHIFT;
            break;

        case TIMER_CMD_WRITE_SW_USAGE:
            regval = getreg32(reg_base + TIMER_SW_USAGE_OFFSET);
            regval &= ~TIMER_SW_USAGE_MASK;
            regval |= ((arg << TIMER_SW_USAGE_SHIFT) & TIMER_SW_USAGE_MASK);
            putreg32(regval, reg_base + TIMER_SW_USAGE_OFFSET);
            break;
#endif

        default:
            ret = -EPERM;
            break;
    }

    return ret;
#endif
}

#if defined(BL616L)
void bflb_timer_capture_init(struct bflb_device_s *dev, const struct bflb_timer_capture_config_s *config)
{
#ifdef romapi_bflb_timer_capture_init
    return romapi_bflb_timer_capture_init(dev, config);
#else
    uint32_t regval;
    uint32_t reg_base;
    struct bflb_device_s *gpio;

    reg_base = dev->reg_base;
    GLB_Sel_MCU_TMR_GPIO_Clock(config->pin);
    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, config->pin, GPIO_FUNC_CLKOUT | GPIO_ALTERNATE | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_1);

    regval = getreg32(reg_base + TIMER_GPIO_OFFSET);
    /* polarity: 1->neg, 0->pos */
    if (config->polarity == TIMER_GPIO_PULSE_POLARITY_NEGATIVE) {
        regval |= (1 << (5 + dev->idx));
    } else {
        regval &= ~(1 << (5 + dev->idx));
    }

    if (config->int_mode == TIMER_GPIO_PULSE_INT_EVEN_FALLING_EDGE) {
        regval |= TIMER_GPIO_LAT_DET_MODE;
    } else {
        regval &= ~TIMER_GPIO_LAT_DET_MODE;
    }
    putreg32(regval, reg_base + TIMER_GPIO_OFFSET);
#endif
}

int bflb_timer_capture_start(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_timer_capture_start
    return romapi_bflb_timer_capture_start(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    if (dev->idx != 0) {
        return -EPERM;
    }

    reg_base = dev->reg_base;
    /* Enable gpio pulse measure function */
    regval = getreg32(reg_base + TIMER_GPIO_OFFSET);
    regval |= TIMER0_GPIO_EN;
    putreg32(regval, reg_base + TIMER_GPIO_OFFSET);

    /* Gpio pulse width latch interrupt clear */
    regval = getreg32(reg_base + TIMER_TICR0_OFFSET);
    regval |= TIMER_GPIO_LAT_IRQ_CLR;
    putreg32(regval, reg_base + TIMER_TICR0_OFFSET);

    /* Gpio pulse width latch interrupt enable */
    regval = getreg32(reg_base + TIMER_TIER0_OFFSET);
    regval |= TIMER_GPIO_LAT_IRQ_EN;
    putreg32(regval, reg_base + TIMER_TIER0_OFFSET);

    return 0;
#endif
}

int bflb_timer_capture_stop(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_timer_capture_stop
    return romapi_bflb_timer_capture_stop(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    if (dev->idx != 0) {
        return -EPERM;
    }

    reg_base = dev->reg_base;
    /* Disable gpio pulse measure function */
    regval = getreg32(reg_base + TIMER_GPIO_OFFSET);
    regval &= ~TIMER0_GPIO_EN;
    putreg32(regval, reg_base + TIMER_GPIO_OFFSET);

    /* Gpio pulse width latch interrupt clear */
    regval = getreg32(reg_base + TIMER_TICR0_OFFSET);
    regval |= TIMER_GPIO_LAT_IRQ_CLR;
    putreg32(regval, reg_base + TIMER_TICR0_OFFSET);

    /* Gpio pulse width latch interrupt disable */
    regval = getreg32(reg_base + TIMER_TIER0_OFFSET);
    regval &= ~TIMER_GPIO_LAT_IRQ_EN;
    putreg32(regval, reg_base + TIMER_TIER0_OFFSET);

    return 0;
#endif
}

int bflb_timer_capture_get_latch_value(struct bflb_device_s *dev, struct bflb_timer_capture_value_s *gpio_lat)
{
#ifdef romapi_bflb_timer_capture_get_latch_value
    return romapi_bflb_timer_capture_get_latch_value(dev, gpio_lat);
#else
    uint32_t reg_base = dev->reg_base;
    uintptr_t flag;
    int ret = 0;

    /* Check timer gpio latch ok */
    if ((getreg32(reg_base + TIMER_GPIO_OFFSET) & TIMER_GPIO_LAT_OK) == 0) {
        return -EBUSY;
    }

    flag = bflb_irq_save();

    do {
        gpio_lat->gpio_lat1 = getreg32(reg_base + TIMER_GPIO_LAT1_OFFSET);
        gpio_lat->gpio_lat3 = getreg32(reg_base + TIMER_GPIO_LAT3_OFFSET);
    } while (gpio_lat->gpio_lat1 == gpio_lat->gpio_lat3);

    do {
        gpio_lat->gpio_lat2 = getreg32(reg_base + TIMER_GPIO_LAT2_OFFSET);
        gpio_lat->gpio_lat4 = getreg32(reg_base + TIMER_GPIO_LAT4_OFFSET);
    } while (gpio_lat->gpio_lat2 == gpio_lat->gpio_lat4);

    bflb_irq_restore(flag);

    return ret;
#endif
}

int bflb_timer_capture_get_pulsewidth(struct bflb_timer_capture_value_s *gpio_lat)
{
#ifdef romapi_bflb_timer_capture_get_pulsewidth
    return romapi_bflb_timer_capture_get_pulsewidth(gpio_lat);
#else
    uint32_t lat1 = gpio_lat->gpio_lat1;
    uint32_t lat2 = gpio_lat->gpio_lat2;
    uint32_t lat3 = gpio_lat->gpio_lat3;
    uint32_t lat4 = gpio_lat->gpio_lat4;

    if (lat1 > lat2 && lat3 >= lat4) {
        return lat2 - lat3;
    } else if (lat1 < lat2 && lat3 <= lat4) {
        return lat2 - lat1;
    } else if (lat1 < lat2 && lat3 > lat4) {
        if (lat3 > lat2) {
            return lat2 - lat1;
        } else {
            return lat2 - lat3;
        }
    } else if (lat1 > lat2 && lat3 < lat4) {
        if (lat3 > lat2) {
            return lat2 - lat1;
        } else {
            return lat2 - lat3;
        }
    }
    return 0;
#endif
}
#elif defined(BL616) || defined(BL616D)
void bflb_timer_capture_init(struct bflb_device_s *dev, const struct bflb_timer_capture_config_s *config)
{
#ifdef romapi_bflb_timer_capture_init
    romapi_bflb_timer_capture_init(dev, config);
#else
    uint32_t regval;
    uint32_t reg_base;
    struct bflb_device_s *gpio;

    reg_base = dev->reg_base;
    GLB_Sel_MCU_TMR_GPIO_Clock(config->pin);
    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, config->pin, GPIO_FUNC_CLKOUT | GPIO_ALTERNATE | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_1);

    regval = getreg32(reg_base + TIMER_GPIO_OFFSET);
    /* polarity: 1->neg, 0->pos */
    if (config->polarity == TIMER_GPIO_PULSE_POLARITY_NEGATIVE) {
        regval |= (1 << (5 + dev->idx));
    } else {
        regval &= ~(1 << (5 + dev->idx));
    }
    regval |= TIMER0_GPIO_EN;
    putreg32(regval, reg_base + TIMER_GPIO_OFFSET);
#endif
}
#endif