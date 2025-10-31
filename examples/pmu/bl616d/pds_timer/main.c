#include "bflb_mtimer.h"
#include "board.h"
#include "log.h"
#include "bl616d_glb.h"
#include "bl616d_pds.h"
#include "bl616d_hbn.h"
#include "bl616d_aon.h"
#include "bl616d_pm.h"

#define PDS_TIMER_WAKEUP_TIME (10 * 1000 * 1000 / 32)
#define PDS_PRINT_DELAY       200

int main(void)
{
    uint32_t tmpVal = 0;
    uint32_t num = 0;

    board_init();

    HBN_32K_Sel(HBN_32K_RC);

    while (1) {
        PDS_Mask_All_Wakeup_Src();
        HBN_Pin_WakeUp_Mask(0xF);

        tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV0);
        num = tmpVal;
        BL_WR_REG(AON_BASE, HBN_RSV0, (num + 1));
        printf("num is %d\r\n", num);

        if (num == 0) {
            pm_pds_irq_register();
            printf("enter pds1 mode\r\n");
            arch_delay_us(PDS_PRINT_DELAY);
            pm_pds_mode_enter(PM_PDS_LEVEL_1, PDS_TIMER_WAKEUP_TIME);
            printf("pds1 wakeup\r\n");
            /* will excute delay */
            arch_delay_ms(10000);
        } else if (num == 1) {
            printf("enter pds2 mode\r\n");
            arch_delay_us(PDS_PRINT_DELAY);
            pm_pds_mode_enter(PM_PDS_LEVEL_2, PDS_TIMER_WAKEUP_TIME);
            printf("pds2 wakeup\r\n");
            /* will NOT excute delay */
            arch_delay_ms(10000);
        } else if (num == 2) {
            printf("enter pds3 mode\r\n");
            arch_delay_us(PDS_PRINT_DELAY);
            pm_pds_mode_enter(PM_PDS_LEVEL_3, PDS_TIMER_WAKEUP_TIME);
            printf("pds3 wakeup\r\n");
            /* will NOT excute delay */
            arch_delay_ms(10000);
        } else if (num == 3) {
            printf("enter pds7 mode\r\n");
            arch_delay_us(PDS_PRINT_DELAY);
            pm_pds_mode_enter(PM_PDS_LEVEL_7, PDS_TIMER_WAKEUP_TIME);
            printf("pds7 wakeup\r\n");
            /* will NOT excute delay */
            arch_delay_ms(10000);
        } else if (num == 4) {
            printf("enter pds15 mode\r\n");
            arch_delay_us(PDS_PRINT_DELAY);
            pm_pds_mode_enter(PM_PDS_LEVEL_15, PDS_TIMER_WAKEUP_TIME);
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
