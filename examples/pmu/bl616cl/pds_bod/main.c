#include "bflb_mtimer.h"
#include "board.h"
#include "bl616cl_glb.h"
#include "bl616cl_pds.h"
#include "bl616cl_hbn.h"
#include "bl616cl_aon.h"
#include "bl616cl_pm.h"

uint32_t wakeup_count;
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
    if (app_lowpower_mode_load(PM_LDO13_LDO07_PDSLDO07) != 0) {
        printf("load default power mode failed\r\n");
        while (1) {
        }
    }
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
        pm_pds_mode_enter(PM_PDS_LEVEL_15, 0, &app_lowpower_cfg);
        bflb_mtimer_delay_ms(1000);
    }
}
