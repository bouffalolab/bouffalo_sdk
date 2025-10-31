#include "bflb_mtimer.h"
#include "board.h"
#include "log.h"
#include "bl616d_glb.h"
#include "bl616d_pds.h"
#include "bl616d_hbn.h"
#include "bl616d_aon.h"
#include "bl616d_pm.h"

#define PDS_RTC_WAKEUP_TIME (5 * 1000 * 1000 / 32)
#define PDS_PRINT_DELAY     200

void HBN_OUT0_IRQ_Handler(void)
{
    printf("HBN_OUT0_IRQ_Handler\r\n");
    if (SET == HBN_Get_INT_State(HBN_INT_GPIO0)) {
        printf("HBN_INT_GPIO0 wakeup\r\n");
        HBN_Clear_IRQ(HBN_INT_GPIO0);
        pm_irq_callback(PM_HBN_GPIO0_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO1)) {
        printf("HBN_INT_GPIO1 wakeup\r\n");
        HBN_Clear_IRQ(HBN_INT_GPIO1);
        pm_irq_callback(PM_HBN_GPIO1_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO2)) {
        printf("HBN_INT_GPIO2 wakeup\r\n");
        HBN_Clear_IRQ(HBN_INT_GPIO2);
        pm_irq_callback(PM_HBN_GPIO2_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO3)) {
        printf("HBN_INT_GPIO3 wakeup\r\n");
        HBN_Clear_IRQ(HBN_INT_GPIO3);
        pm_irq_callback(PM_HBN_GPIO3_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO4)) {
        printf("HBN_INT_GPIO4 wakeup\r\n");
        HBN_Clear_IRQ(HBN_INT_GPIO4);
        pm_irq_callback(PM_HBN_GPIO4_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO5)) {
        printf("HBN_INT_GPIO5 wakeup\r\n");
        HBN_Clear_IRQ(HBN_INT_GPIO5);
        pm_irq_callback(PM_HBN_GPIO5_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO6)) {
        printf("HBN_INT_GPIO6 wakeup\r\n");
        HBN_Clear_IRQ(HBN_INT_GPIO6);
        pm_irq_callback(PM_HBN_GPIO6_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO7)) {
        printf("HBN_INT_GPIO7 wakeup\r\n");
        HBN_Clear_IRQ(HBN_INT_GPIO7);
        pm_irq_callback(PM_HBN_GPIO7_WAKEUP_EVENT);
    } else {
        printf("HBN_INT_RTC wakeup\r\n");
        HBN_Clear_RTC_INT();
        HBN_Clear_IRQ(HBN_INT_RTC);
        pm_irq_callback(PM_HBN_RTC_WAKEUP_EVENT);
    }
}

int main(void)
{
    uint32_t tmpVal = 0;
    uint32_t num = 0;
    uint32_t rtc_val_l = 0;
    uint32_t rtc_val_h = 0;

    board_init();

    HBN_32K_Sel(HBN_32K_RC);

    while (1) {
        HBN_Get_RTC_Timer_Val(&rtc_val_l, &rtc_val_h);
        printf("RTC val: %u, %u\r\n", rtc_val_l, rtc_val_h);
        HBN_Clear_RTC_INT();
        HBN_Set_RTC_Timer(HBN_RTC_INT_DELAY_0T, rtc_val_l + PDS_RTC_WAKEUP_TIME, rtc_val_h, HBN_RTC_COMP_BIT0_39);
        HBN_Enable_RTC_Counter();
        arch_delay_ms(1000);
        HBN_Get_RTC_Timer_Val(&rtc_val_l, &rtc_val_h);
        printf("RTC val: %u, %u\r\n", rtc_val_l, rtc_val_h);
        bflb_irq_attach(HBN_OUT0_IRQn, (irq_callback)HBN_OUT0_IRQ_Handler, NULL);
        bflb_irq_enable(HBN_OUT0_IRQn);

        PDS_Mask_All_Wakeup_Src();
        HBN_Pin_WakeUp_Mask(0xF);
        PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_HBN_IRQ_OUT, UNMASK);

        tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV0);
        num = tmpVal;
        BL_WR_REG(AON_BASE, HBN_RSV0, (num + 1));
        printf("num is %d\r\n", num);

        if (num == 0) {
            pm_pds_irq_register();
            printf("enter pds1 mode\r\n");
            arch_delay_us(PDS_PRINT_DELAY);
            pm_pds_mode_enter(PM_PDS_LEVEL_1, 0);
            printf("pds1 wakeup\r\n");
            /* will excute delay */
            arch_delay_ms(10000);
        } else if (num == 1) {
            printf("enter pds2 mode\r\n");
            arch_delay_us(PDS_PRINT_DELAY);
            pm_pds_mode_enter(PM_PDS_LEVEL_2, 0);
            printf("pds2 wakeup\r\n");
            /* will NOT excute delay */
            arch_delay_ms(10000);
        } else if (num == 2) {
            printf("enter pds3 mode\r\n");
            arch_delay_us(PDS_PRINT_DELAY);
            pm_pds_mode_enter(PM_PDS_LEVEL_3, 0);
            printf("pds3 wakeup\r\n");
            /* will NOT excute delay */
            arch_delay_ms(10000);
        } else if (num == 3) {
            printf("enter pds7 mode\r\n");
            arch_delay_us(PDS_PRINT_DELAY);
            pm_pds_mode_enter(PM_PDS_LEVEL_7, 0);
            printf("pds7 wakeup\r\n");
            /* will NOT excute delay */
            arch_delay_ms(10000);
        } else if (num == 4) {
            printf("enter pds15 mode\r\n");
            arch_delay_us(PDS_PRINT_DELAY);
            pm_pds_mode_enter(PM_PDS_LEVEL_15, 0);
            printf("pds15 wakeup\r\n");
            /* will NOT excute delay */
            arch_delay_ms(10000);
        } else {
            printf("SUCCESS\r\n");
            BL_WR_REG(AON_BASE, HBN_RSV0, 0);
            while (1) {
            }
        }
    }
}
