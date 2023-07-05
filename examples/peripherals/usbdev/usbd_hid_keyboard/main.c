#include "usbd_core.h"
#include "bflb_mtimer.h"
#include "board.h"

extern void hid_keyboard_init(void);
extern void hid_keyboard_test(void);

int main(void)
{
    board_init();

    hid_keyboard_init();
    while (1) {
        hid_keyboard_test();
        bflb_mtimer_delay_ms(500);
    }
}
