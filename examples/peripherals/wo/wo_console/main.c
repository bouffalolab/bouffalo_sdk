#include "bflb_mtimer.h"
#include "board.h"

int main(void)
{
    board_init();
    while (1) {
        static uint32_t i = 0;
        printf("i = %d\n", i++);
        if ((i & 3) == 0) {
            printf("abcdefghijklmnopqrstuvwxyz\n");
        } else if ((i & 3) == 1) {
            printf("ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
        } else if ((i & 3) == 2) {
            printf("0123456789\n");
        } else {
            printf("~!@#$%%^&*()_+-={}[]:\";'<>?,./'\n");
        }
        bflb_mtimer_delay_ms(500);
    }
}
