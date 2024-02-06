#include "bflb_wdg.h"
#include "bflb_clock.h"
#include "hardware/timer_reg.h"

void bflb_wdg_init(struct bflb_device_s *dev, const struct bflb_wdg_config_s *config)
{
    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_WDG_CLKSRC(config->clock_source));
    LHAL_PARAM_ASSERT(IS_WDG_CLOCK_DIV(config->clock_div));

#ifdef romapi_bflb_wdg_init
    romapi_bflb_wdg_init(dev, config);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    putreg16(0xBABA, reg_base + TIMER_WFAR_OFFSET);
    putreg16(0xEB10, reg_base + TIMER_WSAR_OFFSET);

    regval = getreg32(reg_base + TIMER_WMER_OFFSET);
    regval &= ~TIMER_WE;
    if (config->mode == WDG_MODE_INTERRUPT) {
        regval &= ~TIMER_WRIE;
    } else {
        regval |= TIMER_WRIE;
    }
    putreg32(regval, reg_base + TIMER_WMER_OFFSET);

    /* Configure clock source */
    regval = getreg32(reg_base + TIMER_TCCR_OFFSET);
    regval &= ~TIMER_CS_WDT_MASK;
    regval |= (config->clock_source << TIMER_CS_WDT_SHIFT);
    putreg32(regval, reg_base + TIMER_TCCR_OFFSET);

    /* Configure clock div */
    regval = getreg32(reg_base + TIMER_TCDR_OFFSET);
    regval &= ~TIMER_WCDR_MASK;
    regval |= (config->clock_div << TIMER_WCDR_SHIFT);
    putreg32(regval, reg_base + TIMER_TCDR_OFFSET);

    putreg16(0xBABA, reg_base + TIMER_WFAR_OFFSET);
    putreg16(0xEB10, reg_base + TIMER_WSAR_OFFSET);
    regval = getreg32(reg_base + TIMER_WCR_OFFSET);
    regval |= TIMER_WCR;
    putreg32(regval, reg_base + TIMER_WCR_OFFSET);
    
    putreg16(0xBABA, reg_base + TIMER_WFAR_OFFSET);
    putreg16(0xEB10, reg_base + TIMER_WSAR_OFFSET);
    putreg16(config->comp_val, reg_base + TIMER_WMR_OFFSET);
#endif
}

void bflb_wdg_start(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_wdg_start
    romapi_bflb_wdg_start(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    putreg16(0xBABA, reg_base + TIMER_WFAR_OFFSET);
    putreg16(0xEB10, reg_base + TIMER_WSAR_OFFSET);

    regval = getreg32(reg_base + TIMER_WMER_OFFSET);
    regval |= TIMER_WE;
    putreg32(regval, reg_base + TIMER_WMER_OFFSET);
#endif
}

void bflb_wdg_stop(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_wdg_stop
    romapi_bflb_wdg_stop(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    putreg16(0xBABA, reg_base + TIMER_WFAR_OFFSET);
    putreg16(0xEB10, reg_base + TIMER_WSAR_OFFSET);

    regval = getreg32(reg_base + TIMER_WMER_OFFSET);
    regval &= ~TIMER_WE;
    putreg32(regval, reg_base + TIMER_WMER_OFFSET);
#endif
}

uint16_t bflb_wdg_get_countervalue(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_wdg_get_countervalue
    return romapi_bflb_wdg_get_countervalue(dev);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;

    return getreg16(reg_base + TIMER_WVR_OFFSET);
#endif
}

void bflb_wdg_set_countervalue(struct bflb_device_s *dev, uint16_t value)
{
#ifdef romapi_bflb_wdg_set_countervalue
    romapi_bflb_wdg_set_countervalue(dev, value);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;

    putreg16(0xBABA, reg_base + TIMER_WFAR_OFFSET);
    putreg16(0xEB10, reg_base + TIMER_WSAR_OFFSET);
    putreg16(value, reg_base + TIMER_WMR_OFFSET);
#endif
}

void bflb_wdg_reset_countervalue(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_wdg_reset_countervalue
    romapi_bflb_wdg_reset_countervalue(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    putreg16(0xBABA, reg_base + TIMER_WFAR_OFFSET);
    putreg16(0xEB10, reg_base + TIMER_WSAR_OFFSET);

    regval = getreg32(reg_base + TIMER_WCR_OFFSET);
    regval |= TIMER_WCR;
    putreg32(regval, reg_base + TIMER_WCR_OFFSET);
#endif
}

void bflb_wdg_compint_clear(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_wdg_compint_clear
    romapi_bflb_wdg_compint_clear(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    putreg16(0xBABA, reg_base + TIMER_WFAR_OFFSET);
    putreg16(0xEB10, reg_base + TIMER_WSAR_OFFSET);

    regval = getreg32(reg_base + TIMER_WICR_OFFSET);
    regval |= TIMER_WICLR;
    putreg32(regval, reg_base + TIMER_WICR_OFFSET);
#endif
}
