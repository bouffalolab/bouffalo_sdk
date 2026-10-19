#include "bflb_mtimer.h"
#include "board.h"
#include "log.h"
#include "bl616_glb.h"
#include "bl616_pds.h"
#include "bl616_hbn.h"
#include "bl616_aon.h"
#include "bl616_pm.h"
#include "bflb_uart.h"
#include "bflb_acomp.h"

int main(void)
{
    board_init();

    printf("init acomp\r\n");
    board_acomp_init();

    printf("enable acomp\r\n");
    bflb_acomp_enable(AON_ACOMP0_ID);
    bflb_acomp_enable(AON_ACOMP1_ID);

    HBN_Clear_IRQ(HBN_INT_ACOMP0);
    HBN_Clear_IRQ(HBN_INT_ACOMP1);

    HBN_Enable_AComp_IRQ(AON_ACOMP0_ID, HBN_ACOMP_INT_EDGE_POSEDGE);
    HBN_Enable_AComp_IRQ(AON_ACOMP0_ID, HBN_ACOMP_INT_EDGE_NEGEDGE);
    HBN_Enable_AComp_IRQ(AON_ACOMP1_ID, HBN_ACOMP_INT_EDGE_POSEDGE);
    HBN_Enable_AComp_IRQ(AON_ACOMP1_ID, HBN_ACOMP_INT_EDGE_NEGEDGE);

    printf("enter hbn mode\r\n");
    bflb_mtimer_delay_ms(100);

    /* sleep time must set zero to avoid using rtc */
    pm_hbn_mode_enter(PM_HBN_LEVEL_0, 0);

    while (1) {
    }
}
