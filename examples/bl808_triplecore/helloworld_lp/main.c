#include "bflb_mtimer.h"
#include "board.h"

#define DBG_TAG "MAIN"
#include "log.h"

int main(void)
{
    board_init();
    while (1) {
        LOG_F("hello world lp\r\n");
        LOG_E("hello world lp\r\n");
        LOG_W("hello world lp\r\n");
        LOG_I("hello world lp\r\n");
        LOG_D("hello world lp\r\n");
        LOG_T("hello world lp\r\n");
        bflb_mtimer_delay_ms(1000);
    }
}
