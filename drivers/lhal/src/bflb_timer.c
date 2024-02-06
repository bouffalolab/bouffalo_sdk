#include "bflb_timer.h"
#include "bflb_gpio.h"
#include "hardware/timer_reg.h"

void bflb_timer_init(struct bflb_device_s *dev, const struct bflb_timer_config_s *config)
{
    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_TIMER_COUNTER_MODE(config->counter_mode));
    LHAL_PARAM_ASSERT(IS_TIMER_CLK_SOURCE(config->clock_source));
    LHAL_PARAM_ASSERT(IS_TIMER_CLOCK_DIV(config->clock_div));
    LHAL_PARAM_ASSERT(IS_TIMER_COMP_ID(config->trigger_comp_id));

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
    romapi_bflb_timer_set_preloadvalue(dev);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;

    putreg32(val, reg_base + TIMER_TPLVR0_OFFSET + 4 * dev->idx);
#endif
}

void bflb_timer_set_compvalue(struct bflb_device_s *dev, uint8_t cmp_no, uint32_t val)
{
#ifdef romapi_bflb_timer_set_compvalue
    romapi_bflb_timer_set_compvalue(dev);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;

    putreg32(val, reg_base + TIMER_TMR0_0_OFFSET + 0x0c * dev->idx + 4 * cmp_no);
#endif
}

uint32_t bflb_timer_get_compvalue(struct bflb_device_s *dev, uint8_t cmp_no)
{
#ifdef romapi_bflb_timer_get_compvalue
    romapi_bflb_timer_get_compvalue(dev);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;

    return getreg32(reg_base + TIMER_TMR0_0_OFFSET + 0x0c * dev->idx + 4 * cmp_no);
#endif
}

uint32_t bflb_timer_get_countervalue(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_timer_get_countervalue
    romapi_bflb_timer_get_countervalue(dev);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;

    return getreg32(reg_base + TIMER_TCR0_OFFSET + 4 * dev->idx);
#endif
}

void bflb_timer_compint_mask(struct bflb_device_s *dev, uint8_t cmp_no, bool mask)
{
#ifdef romapi_bflb_timer_compint_mask
    romapi_bflb_timer_compint_mask(dev);
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
    romapi_bflb_timer_get_compint_status(dev);
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
    romapi_bflb_timer_compint_clear(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + TIMER_TICR0_OFFSET + 4 * dev->idx);
    regval |= (1 << cmp_no);
    putreg32(regval, reg_base + TIMER_TICR0_OFFSET + 4 * dev->idx);
#endif
}