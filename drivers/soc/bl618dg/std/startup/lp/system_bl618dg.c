#include "bl618dg.h"

#include "sysinit.h"

void SystemInit(void)
{
    uint32_t i = 0;

    /* get interrupt level from info */
    CLIC->CLICCFG = (((CLIC->CLICINFO & CLIC_INFO_CLICINTCTLBITS_Msk) >> CLIC_INFO_CLICINTCTLBITS_Pos) << CLIC_CLICCFG_NLBIT_Pos);

    /* Every interrupt should be clear by software*/
    for (i = 0; i < IRQn_LAST; i++) {
        CLIC->CLICINT[i].IE = 0;
        CLIC->CLICINT[i].IP = 0;
#ifdef CONFIG_IRQ_USE_VECTOR
        CLIC->CLICINT[i].ATTR = 1; /* use vector interrupt */
#else
        CLIC->CLICINT[i].ATTR = 0; /* use no vector interrupt */
#endif
    }

    /* tspend interrupt will be clear auto*/
    /* tspend use positive interrupt */
#ifdef CONFIG_IRQ_USE_VECTOR
    CLIC->CLICINT[MSOFT_IRQn].ATTR = 0x3;
    //CLIC->CLICINT[SDU_SOFT_RST_IRQn].ATTR = 0x3;
#else
    CLIC->CLICINT[MSOFT_IRQn].ATTR = 0x2;
    //CLIC->CLICINT[SDU_SOFT_RST_IRQn].ATTR = 0x2;
#endif

}

#if defined(CONFIG_SYSINIT) && defined(CONFIG_FREERTOS)
/* Kept after the service records and before the scheduler barrier. */
static void soc_post_init(const void *arg)
{
    (void)arg;
    __enable_irq();
}
SYSINIT(soc_post, SI_SUB_PRE_SCHED, SI_ORDER_FIRST, soc_post_init, NULL);
#endif

void System_Post_Init(void)
{
    sysinit_run_pre_schedule();

#if !defined(CONFIG_SYSINIT) || !defined(CONFIG_FREERTOS)
    /* global IRQ enable */
    __enable_irq();
#endif
}