#include "bflb_mtimer.h"
#include "board.h"

extern void core_mark();

int main(void)
{
    board_init();

    core_mark();

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
