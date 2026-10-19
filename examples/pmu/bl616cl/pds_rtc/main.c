#include "bflb_mtimer.h"
#include "board.h"
#include "log.h"
#include "bl616cl_glb.h"
#include "bl616cl_pds.h"
#include "bl616cl_hbn.h"
#include "bl616cl_aon.h"
#include "bl616cl_pm.h"
#include "bl_lp.h"
#ifdef CONFIG_PSRAM
#include "psram_test.h"
#endif

#define PDS_RTC_WAKEUP_TIME (5*1000*1000/32)

static PM_LOWPOWER_CFG_Type app_lowpower_cfg;

static int app_lowpower_mode_load(uint8_t mode)
{
    const PM_LOWPOWER_CFG_Type *mode_cfg = pm_power_mode_cfg_get(mode);

    if (mode_cfg == NULL) {
        return -1;
    }

    app_lowpower_cfg = *mode_cfg;
    return 0;
}

void HBN_OUT0_IRQ_Handler(void)
{
    printf("HBN_OUT0_IRQ_Handler\r\n");
    if (SET == HBN_Get_INT_State(HBN_INT_GPIO0)) {
        HBN_Clear_IRQ(HBN_INT_GPIO0);
        pm_irq_callback(PM_HBN_GPIO0_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO1)) {
        HBN_Clear_IRQ(HBN_INT_GPIO1);
        pm_irq_callback(PM_HBN_GPIO1_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO2)) {
        HBN_Clear_IRQ(HBN_INT_GPIO2);
        pm_irq_callback(PM_HBN_GPIO2_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO3)) {
        HBN_Clear_IRQ(HBN_INT_GPIO3);
        pm_irq_callback(PM_HBN_GPIO3_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO4)) {
        HBN_Clear_IRQ(HBN_INT_GPIO4);
        pm_irq_callback(PM_HBN_GPIO4_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO5)) {
        HBN_Clear_IRQ(HBN_INT_GPIO5);
        pm_irq_callback(PM_HBN_GPIO5_WAKEUP_EVENT);
    } else {
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
    if (app_lowpower_mode_load(PM_LDO13_LDO07_PDSLDO07) != 0) {
        printf("load default power mode failed\r\n");
        while (1) {
        }
    }

    HBN_32K_Sel(HBN_32K_RC);

    while (1) {
        HBN_Get_RTC_Timer_Val(&rtc_val_l, &rtc_val_h);
        printf("RTC val: %u, %u\r\n", rtc_val_l, rtc_val_h);
        HBN_Clear_RTC_INT();
        HBN_Set_RTC_Timer(HBN_RTC_INT_DELAY_0T,rtc_val_l+PDS_RTC_WAKEUP_TIME, rtc_val_h, HBN_RTC_COMP_BIT0_47);
        HBN_Enable_RTC_Counter();
        arch_delay_ms(1000);
        HBN_Get_RTC_Timer_Val(&rtc_val_l, &rtc_val_h);
        printf("RTC val: %u, %u\r\n", rtc_val_l, rtc_val_h);
        bflb_irq_attach(HBN_OUT0_IRQn, (irq_callback)HBN_OUT0_IRQ_Handler, NULL);
        // bflb_irq_enable(HBN_OUT0_IRQn); //if enable this IRQ, CPU will continue run after HBN interrupt but before resuming PDS state

        PDS_Mask_All_Wakeup_Src();
        HBN_Pin_WakeUp_Mask(0xF);
        PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_HBN_IRQ_OUT, UNMASK);

        tmpVal = HBN_Get_Version();
        num = tmpVal;
        HBN_Set_Version(num + 1);
        printf("num is %d\r\n", num);

#ifdef CONFIG_PSRAM
        psram_test_prepare();
#endif

        if (num == 0) {
            pm_pds_irq_register();
            printf("enter pds1 mode\r\n");
            arch_delay_ms(10);  /* Wait for log output */
            bl_lp_pds_enter_with_restore(PM_PDS_LEVEL_1, 0, &app_lowpower_cfg);
            printf("pds1 wakeup\r\n");
        } else if (num == 1) {
            printf("enter pds2 mode\r\n");
            arch_delay_ms(10);  /* Wait for log output */
            bl_lp_pds_enter_with_restore(PM_PDS_LEVEL_2, 0, &app_lowpower_cfg);
            printf("pds2 wakeup\r\n");
        } else if (num == 2) {
            printf("enter pds3 mode\r\n");
            arch_delay_ms(10);  /* Wait for log output */
            bl_lp_pds_enter_with_restore(PM_PDS_LEVEL_3, 0, &app_lowpower_cfg);
            printf("pds3 wakeup\r\n");
        } else if (num == 3) {
            printf("enter pds7 mode\r\n");
            arch_delay_ms(10);  /* Wait for log output */
            bl_lp_pds_enter_with_restore(PM_PDS_LEVEL_7, 0, &app_lowpower_cfg);
            printf("pds7 wakeup\r\n");
        } else if (num == 4) {
            printf("enter pds15 mode\r\n");
            arch_delay_ms(10);  /* Wait for log output */
            bl_lp_pds_enter_with_restore(PM_PDS_LEVEL_15, 0, &app_lowpower_cfg);
            printf("pds15 wakeup\r\n");
        } else {
            printf("SUCCESS\r\n");
            BL_WR_REG(AON_BASE, HBN_RSV0, 0);
            while (1) {
            }
        }

#ifdef CONFIG_PSRAM
        psram_test_verify();
#endif
        /* will NOT excute delay */
        arch_delay_ms(10000);
    }
}
