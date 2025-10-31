#include "bflb_mtimer.h"
#include "board.h"
#include "log.h"
#include "bl616_glb.h"
#include "bl616_pds.h"
#include "bl616_hbn.h"
#include "bl616_aon.h"
#include "bl616_pm.h"

int main(void)
{
    uint32_t tmpVal = 0;
    board_init();

    /* rf808_usb20_psw_rref output off */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_USB_PHY_CTRL);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_REG_PU_USB20_PSW);
    BL_WR_REG(PDS_BASE, PDS_USB_PHY_CTRL, tmpVal);

    pm_pds_mask_all_wakeup_src();
    AON_Output_Float_LDO15_RF();
    HBN_Pin_WakeUp_Mask(0xF);

    printf("enter pds mode\r\n");
    bflb_mtimer_delay_ms(100);

    /* Wake up every 1 seconds by pds15 */
    pm_pds_mode_enter(PM_PDS_LEVEL_15, 32768 * 1);

    while (1) {
    }
}
