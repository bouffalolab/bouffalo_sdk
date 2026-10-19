#include "bflb_mtimer.h"
#include "board.h"
#include "log.h"
#include "bl618dg_glb.h"
#include "bl618dg_pds.h"
#include "bl618dg_hbn.h"
#include "bl618dg_aon.h"
#include "bl618dg_pm.h"

#define PDS_RTC_WAKEUP_TIME (5 * 1000 * 1000 / 32)
#define PDS_PRINT_DELAY     200

volatile uint32_t bod_flag = 0;

void BOD_IRQ_Handler(void)
{
    printf("BOD_IRQ_Handler\r\n");
    printf("BOD Status:%d\r\n", AON_Get_BOD_OUT_State());
    bod_flag = 1;
    bflb_irq_disable(BOD_IRQn);
}

void BOD_Init_Reset(void)
{
    uint8_t enableBod = 1;
    /*0:BOD not trigger interrupt,1:trigger interrupt*/
    uint8_t enableBodInt = 1;
    /* 0:2.2v,1:2.3v,2:2.4v....7:2.9v */
    uint8_t bodThreshold = 7;

    AON_Set_BOD_Config(enableBod, bodThreshold, enableBodInt);
}

void BOD_Init_Interrupt(void)
{
    uint8_t enableBod = 1;
    /*0:BOD not trigger interrupt,1:trigger interrupt*/
    uint8_t enableBodInt = 1;
    /* 0:2.2v,1:2.3v,2:2.4v....7:2.9v */
    uint8_t bodThreshold = 7;

    AON_Set_BOD_Config(enableBod, bodThreshold, enableBodInt);
}

int main(void)
{
    uint32_t tmpVal = 0;

    board_init();
    HBN_32K_Sel(HBN_32K_RC);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV0);
    BL_WR_REG(AON_BASE, HBN_RSV0, (tmpVal + 1));

    bflb_irq_attach(BOD_IRQn, (irq_callback)BOD_IRQ_Handler, NULL);
    bflb_irq_enable(BOD_IRQn);

    printf("BOD Status:%d\r\n", AON_Get_BOD_OUT_State());

    if (0 == tmpVal) {
        BOD_Init_Reset();
        printf("Test BOD Reset\r\n");
        printf("Please reduce power voltage to make chip reset, then switch back to 3.3V\r\n");
#if 1
        printf("BL618DG not support BOD Reset, go to next test\r\n");
        bflb_mtimer_delay_ms(5000);
        GLB_SW_POR_Reset();
#else
        bflb_mtimer_delay_ms(1000);
        while (1) {
            /* wait for BOD reset */
            printf("wait for BOD reset\r\n");
            bflb_mtimer_delay_ms(1000);
        }
#endif
    } else if (1 == tmpVal) {
        BOD_Init_Interrupt();
        printf("Test BOD Interrupt\r\n");
        printf("Please reduce power voltage to make chip interrupt\r\n");
        while (0 == bod_flag) {
            /* wait for BOD reset */
            printf("wait for BOD interrupt\r\n");
            bflb_mtimer_delay_ms(1000);
            printf("BOD Status:%d\r\n", AON_Get_BOD_OUT_State());
        }
        printf("Chip will reset to test pds wake up, please switch back to 3.3V\r\n");
        bflb_mtimer_delay_ms(10000);
        GLB_SW_POR_Reset();
    } else if (2 == tmpVal) {
        BOD_Init_Interrupt();
        PDS_Mask_All_Wakeup_Src();
        HBN_Pin_WakeUp_Mask(0xF);
        PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_HBN_IRQ_OUT, UNMASK);
        printf("Test BOD Wakeup\r\n");
        printf("Please reduce power voltage to make chip wakeup\r\n");
        pm_pds_irq_register();
        printf("enter pds mode\r\n");
        arch_delay_us(PDS_PRINT_DELAY);
        pm_pds_mode_enter(PM_PDS_LEVEL_15, 0);
        printf("pds wakeup\r\n");
        /* will excute delay */
        arch_delay_ms(10000);
    } else {
        printf("case end\r\n");
    }

    while(1) {
        arch_delay_ms(10000);
    }
}
