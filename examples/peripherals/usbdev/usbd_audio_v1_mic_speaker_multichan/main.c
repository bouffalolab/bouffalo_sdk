#include "usbd_core.h"
#include "bflb_mtimer.h"
#include "board.h"

extern void audio_v1_init(void);
extern void audio_v1_test(void);

int main(void)
{
    board_init();

    audio_v1_init();
    while (1) {
        audio_v1_test();
    }
}
