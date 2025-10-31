#include "bflb_mtimer.h"
#include "board.h"
#include "log.h"
#include "bl616_pm.h"

int main(void)
{
    board_init();

    printf("enter hbn mode\r\n");
    bflb_mtimer_delay_ms(100);

    /* Wake up every 1 seconds by hbn0 */
    pm_hbn_mode_enter(PM_HBN_LEVEL_0, 32768*1);

    while (1) {
    }
}
