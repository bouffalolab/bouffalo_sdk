#include "bflb_mtimer.h"
#include "board.h"

#define MODEL_MNIST 1
#define MODEL_CIFAR10 2
#define MODEL_VWW 3
#define MODEL_MBNET 4

#define CONFIG_MODEL MODEL_MNIST

#define main benchmark_main
#if (CONFIG_MODEL == MODEL_MNIST)
#include "mnist/main.c"
#elif (CONFIG_MODEL == MODEL_CIFAR10)
#include "cifar10/main.c"
#elif (CONFIG_MODEL == MODEL_VWW)
#include "vww/main.c"
#elif (CONFIG_MODEL == MODEL_MBNET)
#include "mbnet/label.c"
#include "mbnet/main.c"
#endif
#undef main

int main(void)
{
    board_init();

    printf("tinymaix test ...\n");
    benchmark_main(0, NULL);

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
