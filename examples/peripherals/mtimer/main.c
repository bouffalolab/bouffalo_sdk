#include "bflb_mtimer.h"

void systick_isr()
{
    static uint32_t tick = 0;
    tick++;
    printf("tick:%d\r\n", tick);
}

extern void board_init(void);

int main(void)
{
    board_init();
    bflb_mtimer_config(1000000, systick_isr);
    while (1) {
    }
}
