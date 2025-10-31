#include "bflb_mtimer.h"
#include "board.h"

#define DBG_TAG "MAIN"
#include "log.h"

int main(void)
{
    board_init();
    while (1) {
        LOG_F("hello world d0\r\n");
        LOG_E("hello world d0\r\n");
        LOG_W("hello world d0\r\n");
        LOG_I("hello world d0\r\n");
        LOG_D("hello world d0\r\n");
        LOG_T("hello world d0\r\n");
        bflb_mtimer_delay_ms(1000);
    }
}
