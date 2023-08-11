#include "usbd_core.h"
#include "usbd_msc.h"
#include "bflb_mtimer.h"
#include "board.h"

extern void msc_ram_init(void);

int main(void)
{
    board_init();

    msc_ram_init();
    while (1) {
        if (usbd_msc_set_popup()) {
            bflb_mtimer_delay_ms(200);
            usbd_deinitialize();
        }
    }
}
