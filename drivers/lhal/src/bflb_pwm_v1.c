#include "bflb_pwm_v1.h"
#include "bflb_clock.h"
#include "hardware/pwm_v1_reg.h"

void bflb_pwm_v1_channel_init(struct bflb_device_s *dev, uint8_t ch, const struct bflb_pwm_v1_channel_config_s *config)
{
    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_PWM_CLK_SOURCE(config->clk_source));
    LHAL_PARAM_ASSERT(IS_PWM_CLK_DIV(config->clk_div));
    LHAL_PARAM_ASSERT(IS_PWM_PERIOD(config->period));

#ifdef romapi_bflb_pwm_v1_channel_init
    romapi_bflb_pwm_v1_channel_init(dev, ch, config);
#else
    uint32_t reg_base;
    uint32_t regval;
    uint64_t start_time;

    reg_base = dev->reg_base;
    /* stop pwm */
    regval = getreg32(reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);
    regval |= PWM_STOP_EN;
    putreg32(regval, reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);

    start_time = bflb_mtimer_get_time_ms();
    do {
        regval = getreg32(reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);
        regval &= PWM_STS_TOP;
        if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
            return;
        }
    } while (regval == 0);

    /* config clock source and dividor */
    regval = getreg32(reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);
    regval &= ~PWM_REG_CLK_SEL_MASK;
    if (config->clk_source == BFLB_SYSTEM_XCLK) {
        regval |= (0 << PWM_REG_CLK_SEL_SHIFT);
    } else if (config->clk_source == BFLB_SYSTEM_PBCLK) {
        regval |= (1 << PWM_REG_CLK_SEL_SHIFT);
    } else if (config->clk_source == BFLB_SYSTEM_32K_CLK) {
        regval |= (2 << PWM_REG_CLK_SEL_SHIFT);
    } else {
    }
    putreg32(regval, reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);

    regval = getreg32(reg_base + PWM0_CLKDIV_OFFSET + ch * 0x20);
    regval &= ~PWM_CLK_DIV_MASK;
    regval |= (uint32_t)config->clk_div << PWM_CLK_DIV_SHIFT;
    putreg32(regval, reg_base + PWM0_CLKDIV_OFFSET + ch * 0x20);

    /* config pwm period */
    regval = getreg32(reg_base + PWM0_PERIOD_OFFSET + ch * 0x20);
    regval &= ~PWM_PERIOD_MASK;
    regval |= (uint32_t)config->period << PWM_PERIOD_SHIFT;
    putreg32(regval, reg_base + PWM0_PERIOD_OFFSET + ch * 0x20);
#endif
}

void bflb_pwm_v1_channel_deinit(struct bflb_device_s *dev, uint8_t ch)
{
#ifdef romapi_bflb_pwm_v1_channel_deinit
    romapi_bflb_pwm_v1_channel_deinit(dev, ch);
#else
    uint32_t reg_base;
    uint32_t regval;
    uint64_t start_time;

    reg_base = dev->reg_base;
    /* stop pwmx */
    regval = getreg32(reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);
    regval |= PWM_STOP_EN;
    putreg32(regval, reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);

    start_time = bflb_mtimer_get_time_ms();
    do {
        regval = getreg32(reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);
        regval &= PWM_STS_TOP;
        if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
            return;
        }
    } while (regval == 0);

    /* restore pwmx_clkdiv register with default value */
    putreg32(0, reg_base + PWM0_CLKDIV_OFFSET + ch * 0x20);

    /* restore pwmx_thre1 register with default value */
    putreg32(0, reg_base + PWM0_THRE1_OFFSET + ch * 0x20);

    /* restore pwmx_thre2 register with default value */
    putreg32(0, reg_base + PWM0_THRE2_OFFSET + ch * 0x20);

    /* restore pwmx_period register with default value */
    putreg32(0, reg_base + PWM0_PERIOD_OFFSET + ch * 0x20);

    /* restore pwmx_config register with default value */
    regval = PWM_STOP_MODE;
    putreg32(regval, reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);

    /* restore pwmx_interrupt register with default value */
    putreg32(0, reg_base + PWM0_INTERRUPT_OFFSET + ch * 0x20);

    /* clear all interrupt */
    putreg32(0xFFFFFFFF, reg_base + PWM_INT_CONFIG_OFFSET);
#endif
}

void bflb_pwm_v1_start(struct bflb_device_s *dev, uint8_t ch)
{
#ifdef romapi_bflb_pwm_v1_start
    romapi_bflb_pwm_v1_start(dev, ch);
#else
    uint32_t reg_base;
    uint32_t regval;
    uint32_t start_time;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);
    regval &= ~PWM_STOP_EN;
    putreg32(regval, reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);

    start_time = bflb_mtimer_get_time_ms();
    do {
        regval = getreg32(reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);
        regval &= PWM_STS_TOP;
        if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
            return;
        }
    } while (regval != 0);
#endif
}

void bflb_pwm_v1_stop(struct bflb_device_s *dev, uint8_t ch)
{
#ifdef romapi_bflb_pwm_v1_stop
    romapi_bflb_pwm_v1_stop(dev, ch);
#else
    uint32_t reg_base;
    uint32_t regval;
    uint32_t start_time;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);
    regval |= PWM_STOP_EN;
    putreg32(regval, reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);

    start_time = bflb_mtimer_get_time_ms();
    do {
        regval = getreg32(reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);
        regval &= PWM_STS_TOP;
        if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
            return;
        }
    } while (regval == 0);
#endif
}

void bflb_pwm_v1_set_period(struct bflb_device_s *dev, uint8_t ch, uint16_t period)
{
#ifdef romapi_bflb_pwm_v1_set_period
    romapi_bflb_pwm_v1_set_period(dev, ch, period);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + PWM0_PERIOD_OFFSET + ch * 0x20);
    regval &= ~PWM_PERIOD_MASK;
    regval |= (uint32_t)period << PWM_PERIOD_SHIFT;
    putreg32(regval, reg_base + PWM0_PERIOD_OFFSET + ch * 0x20);
#endif
}

void bflb_pwm_v1_channel_set_threshold(struct bflb_device_s *dev, uint8_t ch, uint16_t low_threhold, uint16_t high_threhold)
{
#ifdef romapi_bflb_pwm_v1_channel_set_threshold
    romapi_bflb_pwm_v1_channel_set_threshold(dev, ch, low_threhold, high_threhold);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + PWM0_THRE1_OFFSET + ch * 0x20);
    regval &= ~PWM_THRE1_MASK;
    regval |= low_threhold;
    putreg32(regval, reg_base + PWM0_THRE1_OFFSET + ch * 0x20);

    regval = getreg32(reg_base + PWM0_THRE2_OFFSET + ch * 0x20);
    regval &= ~PWM_THRE2_MASK;
    regval |= high_threhold;
    putreg32(regval, reg_base + PWM0_THRE2_OFFSET + ch * 0x20);
#endif
}

uint32_t bflb_pwm_v1_get_frequency(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_pwm_v1_get_frequency
    return romapi_bflb_pwm_v1_get_frequency(dev);
#else
    uint32_t reg_base;
    uint32_t regval;
    uint32_t tmp;
    uint32_t src, div, period;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PWM0_CONFIG_OFFSET);
    /* get clock source frequency */
    tmp = (regval & PWM_REG_CLK_SEL_MASK) >> PWM_REG_CLK_SEL_SHIFT;
    switch (tmp) {
        case 0:
            src = bflb_clk_get_system_clock(BFLB_SYSTEM_XCLK);
            break;
        case 1:
            src = bflb_clk_get_system_clock(BFLB_SYSTEM_PBCLK);
            break;
        default:
            src = bflb_clk_get_system_clock(BFLB_SYSTEM_32K_CLK);
            break;
    }
    /* get clock divider */
    regval = getreg32(reg_base + PWM0_CLKDIV_OFFSET);
    tmp = (regval & PWM_CLK_DIV_MASK) >> PWM_CLK_DIV_SHIFT;
    div = tmp ? tmp : 1;
    /* get pwm period count */
    regval = getreg32(reg_base + PWM0_PERIOD_OFFSET);
    period = (regval & PWM_PERIOD_MASK) >> PWM_PERIOD_SHIFT;
    /* calculate frequency */
    tmp = div * period;
    return (src + tmp / 2) / tmp;
#endif
}

void bflb_pwm_v1_get_duty(struct bflb_device_s *dev, uint32_t *delta, uint32_t *period)
{
    LHAL_PARAM_ASSERT(delta);
    LHAL_PARAM_ASSERT(period);

#ifdef romapi_bflb_pwm_v1_get_duty
    romapi_bflb_pwm_v1_get_duty(dev, delta, period);
#else
    uint32_t reg_base;
    uint32_t regval;
    uint32_t val, val2;

    reg_base = dev->reg_base;
    /* get pwm period count */
    regval = getreg32(reg_base + PWM0_PERIOD_OFFSET);
    val = (regval & PWM_PERIOD_MASK) >> PWM_PERIOD_SHIFT;
    *period = val;
    /* get pwm threshold 1 count */
    regval = getreg32(reg_base + PWM0_THRE1_OFFSET);
    val = (regval & PWM_THRE1_MASK) >> PWM_THRE1_SHIFT;
    /* get pwm threshold 2 count */
    regval = getreg32(reg_base + PWM0_THRE2_OFFSET);
    val2 = (regval & PWM_THRE2_MASK) >> PWM_THRE2_SHIFT;
    /* calculate delta of threshold */
    if (val >= val2) {
        *delta = 0;
    } else {
        *delta = val2 - val;
    }
#endif
}

void bflb_pwm_v1_int_enable(struct bflb_device_s *dev, uint8_t ch, bool enable)
{
#ifdef romapi_bflb_pwm_v1_int_enable
    romapi_bflb_pwm_v1_int_enable(dev, ch, enable);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PWM0_INTERRUPT_OFFSET + ch * 0x20);
    if (enable) {
        regval |= PWM_INT_ENABLE;
    } else {
        regval &= ~PWM_INT_ENABLE;
    }
    putreg32(regval, reg_base + PWM0_INTERRUPT_OFFSET + ch * 0x20);
#endif
}

uint32_t bflb_pwm_v1_get_intstatus(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_pwm_v1_get_intstatus
    return romapi_bflb_pwm_v1_get_intstatus(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + PWM_INT_CONFIG_OFFSET);
    return (regval);
#endif
}

void bflb_pwm_v1_int_clear(struct bflb_device_s *dev, uint32_t int_clear)
{
#ifdef romapi_bflb_pwm_v1_int_clear
    romapi_bflb_pwm_v1_int_clear(dev, int_clear);
#else
    uint32_t reg_base;
    uint32_t regval;
    uint32_t timeout_count = 160 * 1000;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PWM_INT_CONFIG_OFFSET);
    regval |= int_clear;
    putreg32(regval, dev->reg_base + PWM_INT_CONFIG_OFFSET);
    do {
        regval = getreg32(reg_base + PWM_INT_CONFIG_OFFSET);
        timeout_count--;
        if (timeout_count == 0) {
            break;
        }
    } while (regval & (int_clear >> PWM_INT_CLEAR_SHIFT));

    regval &= ~int_clear;
    putreg32(regval, dev->reg_base + PWM_INT_CONFIG_OFFSET);
#endif
}

int bflb_pwm_v1_feature_control(struct bflb_device_s *dev, uint8_t ch, int cmd, size_t arg)
{
#ifdef romapi_bflb_pwm_v1_feature_control
    return romapi_bflb_pwm_v1_feature_control(dev, ch, cmd, arg);
#else
    int ret = 0;
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    switch (cmd) {
        case PWM_CMD_SET_STOP_MODE:
            regval = getreg32(reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);
            if (arg == PWM_STOP_MODE_ABRUPT) {
                regval &= ~PWM_STOP_MODE;
            } else {
                regval |= PWM_STOP_MODE;
            }
            putreg32(regval, reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);
            break;

        case PWM_CMD_SET_OUT_INVERT:
            regval = getreg32(reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);
            if (arg) {
                regval |= PWM_OUT_INV;
            } else {
                regval &= ~PWM_OUT_INV;
            }
            putreg32(regval, reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);
            break;

        case PWM_CMD_SET_SW_MODE:
            regval = getreg32(reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);
            if (arg) {
                regval |= PWM_SW_MODE;
            } else {
                regval &= ~PWM_SW_MODE;
            }
            putreg32(regval, reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);
            break;

        case PWM_CMD_SET_SW_FORCE_VALUE:
            regval = getreg32(reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);
            if (arg) {
                regval |= PWM_SW_FORCE_VAL;
            } else {
                regval &= ~PWM_SW_FORCE_VAL;
            }
            putreg32(regval, reg_base + PWM0_CONFIG_OFFSET + ch * 0x20);
            break;

        case PWM_CMD_SET_REPT_COUNT:
            regval = getreg32(reg_base + PWM0_INTERRUPT_OFFSET + ch * 0x20);
            regval &= ~(PWM_INT_PERIOD_CNT_MASK);
            regval |= (arg << PWM_INT_PERIOD_CNT_SHIFT);
            putreg32(regval, reg_base + PWM0_INTERRUPT_OFFSET + ch * 0x20);
            break;

        default:
            ret = -EPERM;
            break;
    }
    return ret;
#endif
}
