#include "bflb_irq.h"
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

#if defined(BL616D)
#include "bl616d_irq.h"
#include "bl616d_glb.h"
#endif

#ifndef BL_IOT_SDK
extern struct bflb_irq_info_s g_irqvector[];

static void irq_unexpected_isr(int irq, void *arg)
{
    printf("irq :%d unregistered\r\n", irq);
}
#endif
void bflb_irq_initialize(void)
{
#ifndef BL_IOT_SDK
    int i;

    /* Point all interrupt vectors to the unexpected interrupt */
    for (i = 0; i < CONFIG_IRQ_NUM; i++) {
        g_irqvector[i].handler = irq_unexpected_isr;
        g_irqvector[i].arg = NULL;
    }
#endif
}

ATTR_TCM_SECTION uintptr_t bflb_irq_save(void)
{
#ifdef romapi_bflb_irq_save
    return romapi_bflb_irq_save();
#else
    uintptr_t oldstat;

    /* Read mstatus & clear machine interrupt enable (MIE) in mstatus */

    asm volatile("csrrc %0, mstatus, %1"
                 : "=r"(oldstat)
                 : "r"(MSTATUS_MIE));
    return oldstat;
#endif
}

ATTR_TCM_SECTION void bflb_irq_restore(uintptr_t flags)
{
#ifdef romapi_bflb_irq_restore
    romapi_bflb_irq_restore(flags);
#else
    /* Write flags to mstatus */

    asm volatile("csrw mstatus, %0"
                 : /* no output */
                 : "r"(flags));
#endif
}

#if (defined(BL616D) && defined(CPU_LP))
void bl616d_level2_irq_handler(int irq, void *arg0)
{
    struct bflb_irq_info_s *vector;
    uint64_t level2_irq_state = 0;

    level2_irq_state = GLB_Get_NP2MINI_Interrupt_Status();

    const uint32_t base = BL616D_IRQ_LEVEL2_BASE + 1;
    while (level2_irq_state) {
        uint32_t i = __builtin_ctzll(level2_irq_state);
        level2_irq_state &= ~(1UL << i);
        const uint32_t irq_num = base + i;
        vector = &g_irqvector[irq_num];
        if (vector->handler) {
            vector->handler(irq_num, vector->arg);
            GLB_Set_NP2MINI_Interrupt_Clear(irq_num);
        } else {
            bflb_lhal_assert_func(__FILE__, __LINE__, __FUNCTION__, "handler is NULL");
        }
    }
}
#endif

int bflb_irq_attach(int irq, irq_callback isr, void *arg)
{
    if (irq >= CONFIG_IRQ_NUM) {
        return -EINVAL;
    }
#ifndef BL_IOT_SDK
#if (defined(BL616D) && defined(CPU_LP))
    if (irq >= BL616D_IRQ_LEVEL2_BASE) {
        g_irqvector[BL616D_IRQ_LEVEL2_BASE].handler = bl616d_level2_irq_handler;
        g_irqvector[BL616D_IRQ_LEVEL2_BASE].arg = arg;
    }
#endif
    g_irqvector[irq].handler = isr;
    g_irqvector[irq].arg = arg;
#else
    extern void bl_irq_register_with_ctx(int irqnum, void *handler, void *ctx);
    bl_irq_register_with_ctx(irq, (void *)isr, arg);
#endif
    return 0;
}

int bflb_irq_detach(int irq)
{
    if (irq >= CONFIG_IRQ_NUM) {
        return -EINVAL;
    }
#ifndef BL_IOT_SDK
    g_irqvector[irq].handler = irq_unexpected_isr;
    g_irqvector[irq].arg = NULL;
#endif
    return 0;
}

void bflb_irq_enable(int irq)
{
#ifdef romapi_bflb_irq_enable
    romapi_bflb_irq_enable(irq);
#else
#if defined(BL702) || defined(BL602) || defined(BL702L)
    putreg8(1, CLIC_HART0_BASE + CLIC_INTIE_OFFSET + irq);
#else
#if (defined(BL808) || defined(BL606P)) && defined(CPU_D0)
    if (csi_vic_get_prio(irq) == 0) {
        csi_vic_set_prio(irq, 1);
    }
#endif
#if (defined(BL616D) && defined(CPU_LP))
    if (irq >= BL616D_IRQ_LEVEL2_BASE) {
        csi_vic_enable_irq(BL616D_IRQ_LEVEL2_BASE);
    } else {
        csi_vic_enable_irq(irq);
    }
#elif (defined(BL616D) && !defined(CPU_MODEL_A0))
    __ECLIC_EnableIRQ(irq);
#else
    csi_vic_enable_irq(irq);
#endif
#endif
#endif
}

void bflb_irq_disable(int irq)
{
#ifdef romapi_bflb_irq_disable
    romapi_bflb_irq_disable(irq);
#else
#if defined(BL702) || defined(BL602) || defined(BL702L)
    putreg8(0, CLIC_HART0_BASE + CLIC_INTIE_OFFSET + irq);
#else
#if (defined(BL616D) && defined(CPU_LP))
    if (irq >= BL616D_IRQ_LEVEL2_BASE) {
        csi_vic_disable_irq(BL616D_IRQ_LEVEL2_BASE);
    } else {
        csi_vic_disable_irq(irq);
    }
#elif (defined(BL616D) && !defined(CPU_MODEL_A0))
    __ECLIC_DisableIRQ(irq);
#else
    csi_vic_disable_irq(irq);
#endif
#endif
#endif
}

void bflb_irq_set_pending(int irq)
{
#ifdef romapi_bflb_irq_set_pending
    romapi_bflb_irq_set_pending(irq);
#else
#if defined(BL702) || defined(BL602) || defined(BL702L)
    putreg8(1, CLIC_HART0_BASE + CLIC_INTIP_OFFSET + irq);
#else
#if (defined(BL616D) && defined(CPU_LP))
    if (irq >= BL616D_IRQ_LEVEL2_BASE) {
        csi_vic_set_pending_irq(BL616D_IRQ_LEVEL2_BASE);
    } else {
        csi_vic_set_pending_irq(irq);
    }
#elif (defined(BL616D) && !defined(CPU_MODEL_A0))
    __ECLIC_SetPendingIRQ(irq);
#else
    csi_vic_set_pending_irq(irq);
#endif
#endif
#endif
}

void bflb_irq_clear_pending(int irq)
{
#ifdef romapi_bflb_irq_clear_pending
    romapi_bflb_irq_clear_pending(irq);
#else
#if defined(BL702) || defined(BL602) || defined(BL702L)
    putreg8(0, CLIC_HART0_BASE + CLIC_INTIP_OFFSET + irq);
#else
#if (defined(BL616D) && defined(CPU_LP))
    if (irq >= BL616D_IRQ_LEVEL2_BASE) {
        csi_vic_clear_pending_irq(BL616D_IRQ_LEVEL2_BASE);
    } else {
        csi_vic_clear_pending_irq(irq);
    }
#elif (defined(BL616D) && !defined(CPU_MODEL_A0))
    __ECLIC_ClearPendingIRQ(irq);
#else
    csi_vic_clear_pending_irq(irq);
#endif
#endif
#endif
}

void bflb_irq_set_nlbits(uint8_t nlbits)
{
#ifdef romapi_bflb_irq_set_nlbits
    romapi_bflb_irq_set_nlbits(nlbits);
#else
#if defined(BL702) || defined(BL602) || defined(BL702L)
    uint8_t clicCfg = getreg8(CLIC_HART0_BASE + CLIC_CFG_OFFSET);
    putreg8((clicCfg & 0xe1) | ((nlbits & 0xf) << 1), CLIC_HART0_BASE + CLIC_CFG_OFFSET);
#else
#if (defined(BL616D) && !defined(CPU_MODEL_A0) && !defined(CPU_LP))
    __ECLIC_SetCfgNlbits(nlbits);
#else
#if !defined(CPU_D0)
    CLIC->CLICCFG = ((nlbits & 0xf) << 1) | 1;
#endif
#endif
#endif
#endif
}

void bflb_irq_set_priority(int irq, uint8_t preemptprio, uint8_t subprio)
{
    if (irq >= CONFIG_IRQ_NUM) {
        return;
    }
#ifdef romapi_bflb_irq_set_priority
    romapi_bflb_irq_set_priority(irq, preemptprio, subprio);
#else
#if defined(BL702) || defined(BL602) || defined(BL702L)
    uint8_t nlbits = getreg8(CLIC_HART0_BASE + CLIC_CFG_OFFSET) >> 1 & 0xf;
    uint8_t clicIntCfg = getreg8(CLIC_HART0_BASE + CLIC_INTCFG_OFFSET + irq);
    putreg8((clicIntCfg & 0xf) | (preemptprio << (8 - nlbits)) | ((subprio & (0xf >> nlbits)) << 4), CLIC_HART0_BASE + CLIC_INTCFG_OFFSET + irq);
#else
#if (defined(BL616D) && defined(CPU_LP))
    if (irq >= BL616D_IRQ_LEVEL2_BASE) {
        csi_vic_set_prio(BL616D_IRQ_LEVEL2_BASE, preemptprio);
    } else {
        csi_vic_set_prio(irq, preemptprio);
    }
#elif (defined(BL616D) && !defined(CPU_MODEL_A0))
    __ECLIC_SetLevelIRQ(irq, preemptprio);
    __ECLIC_SetPriorityIRQ(irq, subprio);
#else
    csi_vic_set_prio(irq, preemptprio);
#endif
#endif
#endif
}
