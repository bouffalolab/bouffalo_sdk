#include "usbh_core.h"
#include "bflb_mtimer.h"
#include "board.h"

extern void audio_init(void);
extern void audio_test(void);

int main(void)
{
    board_init();

    audio_init();
    while (1) {
        audio_test();
    }
}
