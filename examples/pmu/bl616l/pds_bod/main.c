#include "bflb_mtimer.h"
#include "board.h"
#include "bl616l_glb.h"
#include "bl616l_pds.h"
#include "bl616l_hbn.h"
#include "bl616l_aon.h"
#include "bl616l_pm.h"

uint32_t wakeup_count;

HBN_BOD_CFG_Type bod_cfg = {
    .enableBod = ENABLE,               /*!< Enable BOD or not */
    .enableBodInt = ENABLE,            /*!< Enable BOD interrupt or not */
    .bodThreshold = HBN_BOD_THRES_2P7, /*!< BOD threshold */
    .enablePorInBod = DISABLE,         /*!< Enable POR when BOD occure or not */
};

void pm_irq_callback(uint32_t event)
{
    if (event == PM_PDS_FROM_HBN_WAKEUP_EVENT) {
        printf("bod interrupt\r\n");
        HBN_Clear_IRQ(HBN_INT_BOD);
    } else {
        printf("unknow interrupt\r\n");
    }
}

int main(void)
{
    board_init();
    HBN_32K_Sel(HBN_32K_RC);

    while (1) {
        wakeup_count = HBN_Get_Version();
        printf("pds wake up count = %u\r\n", wakeup_count);
        pm_pds_irq_register();
        bflb_mtimer_delay_us(200);
        HBN_Set_Version(wakeup_count + 1);
        HBN_Set_BOD_Cfg(&bod_cfg);
        PDS_Mask_All_Wakeup_Src();
        HBN_Pin_WakeUp_Mask(0xF);
        PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_HBN_IRQ_OUT, UNMASK);
        pm_pds_mode_enter(PM_PDS_LEVEL_15, 0);
        bflb_mtimer_delay_ms(1000);
    }
}
