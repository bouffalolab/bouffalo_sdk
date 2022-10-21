#include "bflb_mtimer.h"
#include "board.h"
#include "log.h"

int main(void)
{
    board_init();
    while (1) {
        LOG_F("hello world\r\n");
        LOG_E("hello world\r\n");
        LOG_W("hello world\r\n");
        LOG_I("hello world\r\n");
        LOG_D("hello world\r\n");
        LOG_T("hello world\r\n");
        bflb_mtimer_delay_ms(1000);
    }
}
