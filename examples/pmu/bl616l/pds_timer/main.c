#include "bflb_mtimer.h"
#include "board.h"
#include "bl616l_glb.h"
#include "bl616l_pds.h"
#include "bl616l_hbn.h"
#include "bl616l_aon.h"
#include "bl616l_pm.h"

#define SLEEP_TIME  (32768)

uint32_t pds_level = PM_PDS_LEVEL_15;
uint32_t wakeup_count;

int main(void)
{
    board_init();
    HBN_32K_Sel(HBN_32K_RC);
    printf("PDS wake up by timer start\r\n");

    while (1) {
        PDS_Mask_All_Wakeup_Src();
        HBN_Pin_WakeUp_Mask(0xF);
        wakeup_count = HBN_Get_Version();
        printf("wake up count = %u, ", wakeup_count);
        HBN_Set_Version(wakeup_count + 1);

        if (pds_level == PM_PDS_LEVEL_1) {
            pm_pds_irq_register();
        }
        printf("enter pds_%u mode\r\n", pds_level);
        arch_delay_us(200);
        pm_pds_mode_enter(pds_level, SLEEP_TIME);
    }
}
