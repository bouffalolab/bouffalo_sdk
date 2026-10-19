#include "bl_lp_internal.h"

static bl_lp_soft_irq_callback_t lp_soft_callback = { NULL };

static void bl_lp_soft_irq(void)
{
    uint64_t wakeup_io_bits = iot2lp_para->wake_io_bits;
    uint32_t wakeup_acmp_bits = iot2lp_para->wake_acomp_bits;

    bflb_irq_disable(MSOFT_IRQn);
    BL_LP_SOFT_INT_CLEAR;

    if ((iot2lp_para->wakeup_reason & LPFW_WAKEUP_IO) && lp_soft_callback.wakeup_io_callback) {
        lp_soft_callback.wakeup_io_callback(wakeup_io_bits);
    }

    if ((iot2lp_para->wakeup_reason & LPFW_WAKEUP_ACOMP) && lp_soft_callback.wakeup_acomp_callback) {
        lp_soft_callback.wakeup_acomp_callback(wakeup_acmp_bits);
    }

    iot2lp_para->wake_io_bits = 0;
    iot2lp_para->wake_acomp_bits = 0;
}

void bl616_lp_soft_irq_trigger(void)
{
    bflb_irq_attach(MSOFT_IRQn, (irq_callback)bl_lp_soft_irq, NULL);
    BL_LP_SOFT_INT_TRIG;
}

void bl_lp_wakeup_io_int_register(void (*wakeup_io_callback)(uint64_t wake_up_io_bits))
{
    lp_soft_callback.wakeup_io_callback = wakeup_io_callback;
}

void bl_lp_wakeup_acomp_int_register(void (*wakeup_acomp_callback)(uint32_t wake_up_acomp))
{
    lp_soft_callback.wakeup_acomp_callback = wakeup_acomp_callback;
}
