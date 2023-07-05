#include "usbd_core.h"
#include "bflb_mtimer.h"
#include "board.h"

extern void video_init(void);
extern void video_test(void);

int main(void)
{
    board_init();

    video_init();
    while (1) {
        video_test();
    }
}
