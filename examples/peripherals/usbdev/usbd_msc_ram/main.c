#include "usbh_core.h"
#include "bflb_mtimer.h"
#include "board.h"

extern void msc_ram_init(void);

int main(void)
{
    board_init();

    msc_ram_init();
    while (1) {
    }
}
