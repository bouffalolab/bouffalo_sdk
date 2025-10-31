#include "bflb_mtimer.h"
#include "bflb_core.h"
#if defined(BL602) || defined(BL702) || defined(BL702L)
#include <risc-v/e24/clic.h>
#else
#if !defined(BL616D) || defined(CPU_MODEL_A0) || defined(CPU_LP)
#include <csi_core.h>
#else
#include <nmsis_core.h>
#endif
#endif

#if defined(BL616L)
/* MCU_MISC reg */
#define BFLB_MISC_BASE                      (0x20009000)
#define MCU_MISC_MCU_E907_RTC_LOAD_L_OFFSET (0x08)
#define MCU_MISC_MCU_E907_RTC_LOAD_H_OFFSET (0x0C)
#define MCU_MISC_MCU_E907_RTC_OFFSET        (0x14)
/* 0x14 : mcu_e907_rtc */
#define MCU_MISC_MCU_RTC_EN                 (1 << 0U)
#define MCU_MISC_MCU_RTC_RST                (1 << 1U)
#define MCU_MISC_MCU_LOAD_P                 (1 << 28U)
#endif

static void (*systick_callback)(void);
static uint64_t current_set_ticks = 0;

static void systick_isr(int irq, void *arg)
{
#if defined(BL702) || defined(BL602) || defined(BL702L)
    *(volatile uint64_t *)(CLIC_CTRL_BASE + CLIC_MTIMECMP_OFFSET) += current_set_ticks;
#else
#if !defined(BL616D) || defined(CPU_MODEL_A0) || defined(CPU_LP)
    csi_coret_config(current_set_ticks, 7);
#else
    SysTimer_SetCompareValue(SysTimer_GetCompareValue() + current_set_ticks);
#endif
#endif
    systick_callback();
}

void bflb_mtimer_config(uint64_t ticks, void (*interruptfun)(void))
{
    bflb_irq_disable(7);

    current_set_ticks = ticks;
    systick_callback = interruptfun;
#if defined(BL702) || defined(BL602) || defined(BL702L)
    *(volatile uint64_t *)(CLIC_CTRL_BASE + CLIC_MTIMECMP_OFFSET) = (*(volatile uint64_t *)(CLIC_CTRL_BASE + CLIC_MTIME_OFFSET)) + ticks;
#else
#if !defined(BL616D) || defined(CPU_MODEL_A0) || defined(CPU_LP)
    csi_coret_config_use(ticks, 7);
#else
    SysTimer_SetCompareValue(SysTimer_GetLoadValue() + ticks);
#endif
#endif

    bflb_irq_attach(7, systick_isr, NULL);
    bflb_irq_enable(7);
}

__WEAK uint32_t ATTR_TCM_SECTION bflb_mtimer_get_freq(void)
{
    return 1 * 1000 * 1000;
}

uint64_t ATTR_TCM_SECTION bflb_mtimer_get_time_us(void)
{
#ifdef romapi_bflb_mtimer_get_time_us
    return romapi_bflb_mtimer_get_time_us();
#else
#if !defined(BL616D) || defined(CPU_MODEL_A0) || defined(CPU_LP)
    volatile uint32_t timeout = 0;
    volatile uint64_t tmp_low, tmp_high, tmp_low1, tmp_high1;

    do {
#if defined(BL702) || defined(BL602) || defined(BL702L)
        tmp_high = getreg32(CLIC_CTRL_BASE + CLIC_MTIME_OFFSET + 4);
        tmp_low = getreg32(CLIC_CTRL_BASE + CLIC_MTIME_OFFSET);
        tmp_low1 = getreg32(CLIC_CTRL_BASE + CLIC_MTIME_OFFSET);
        tmp_high1 = getreg32(CLIC_CTRL_BASE + CLIC_MTIME_OFFSET + 4);
#else
        tmp_high = (uint64_t)csi_coret_get_valueh();
        tmp_low = (uint64_t)csi_coret_get_value();
        tmp_low1 = (uint64_t)csi_coret_get_value();
        tmp_high1 = (uint64_t)csi_coret_get_valueh();
#endif
        timeout++;
        if (timeout == 1000) {
            return 0;
        }
    } while (tmp_low > tmp_low1 || tmp_high != tmp_high1);
#ifdef CONFIG_MTIMER_CUSTOM_FREQUENCE
    return ((uint64_t)(((tmp_high1 << 32) + tmp_low1)) * ((uint64_t)(1 * 1000 * 1000)) / bflb_mtimer_get_freq());
#else
    return (uint64_t)(((tmp_high1 << 32) + tmp_low1));
#endif
#else
#ifdef CONFIG_MTIMER_CUSTOM_FREQUENCE
    return (SysTimer_GetLoadValue() * ((uint64_t)(1 * 1000 * 1000)) / bflb_mtimer_get_freq());
#else
    return (SysTimer_GetLoadValue());
#endif
#endif
#endif
}

uint32_t ATTR_TCM_SECTION __attribute__((weak)) __div64_32(uint64_t *n, uint32_t base)
{
    uint64_t rem = *n;
    uint64_t b = base;
    uint64_t res, d = 1;
    uint32_t high = rem >> 32;

    res = 0;
    if (high >= base) {
        high /= base;
        res = (uint64_t)high << 32;
        rem -= (uint64_t)(high * base) << 32;
    }
    while ((int64_t)b > 0 && b < rem) {
        b = b + b;
        d = d + d;
    }

    do {
        if (rem >= b) {
            rem -= b;
            res += d;
        }
        b >>= 1;
        d >>= 1;
    } while (d);

    *n = res;
    return rem;
}

uint64_t ATTR_TCM_SECTION bflb_mtimer_get_time_ms(void)
{
#ifdef romapi_bflb_mtimer_get_time_ms
    return romapi_bflb_mtimer_get_time_ms();
#else
#ifdef BFLB_BOOT2
    uint64_t ret = bflb_mtimer_get_time_us();
    __div64_32(&ret, 1000);
    return ret;
#else
    return bflb_mtimer_get_time_us() / 1000;
#endif
#endif
}

void ATTR_TCM_SECTION bflb_mtimer_delay_us(uint32_t time)
{
#ifdef romapi_bflb_mtimer_delay_us
    return romapi_bflb_mtimer_delay_us(time);
#else
    uint64_t start_time = bflb_mtimer_get_time_us();

    while (bflb_mtimer_get_time_us() - start_time < time) {
    }
#endif
}

void ATTR_TCM_SECTION bflb_mtimer_delay_ms(uint32_t time)
{
#ifdef romapi_bflb_mtimer_delay_ms
    return romapi_bflb_mtimer_delay_ms(time);
#else
    uint64_t start_time = bflb_mtimer_get_time_us();

    while (bflb_mtimer_get_time_us() - start_time < ((uint64_t)time * 1000)) {
    }
#endif
}

#if defined(BL616L)
void ATTR_TCM_SECTION bflb_mtimer_set_val(uint64_t val)
{
    putreg32((val & 0xFFFFFFFF), (BFLB_MISC_BASE + MCU_MISC_MCU_E907_RTC_LOAD_L_OFFSET));
    putreg32(((val >> 32) & 0xFFFFFFFF), (BFLB_MISC_BASE + MCU_MISC_MCU_E907_RTC_LOAD_H_OFFSET));

    uint32_t regval = getreg32(BFLB_MISC_BASE + MCU_MISC_MCU_E907_RTC_OFFSET);
    regval |= MCU_MISC_MCU_LOAD_P;
    putreg32(regval, (BFLB_MISC_BASE + MCU_MISC_MCU_E907_RTC_OFFSET));
}
#endif
