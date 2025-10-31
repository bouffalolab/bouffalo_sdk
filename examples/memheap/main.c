#include "bflb_mtimer.h"
#include "board.h"
#include "stdlib.h"
#include "string.h"
#include "mem.h"

int memheap_test(void)
{
    char *ptr = NULL;

    for (int i = 1;; i++) {
        ptr = malloc(i * 128);
        printf("remain %d byte\r\n", kfree_size());

        if (ptr != NULL) {
            memcpy(ptr, "hello123456789123456789123456789", 33);
            printf("ptr :%s\r\n", ptr);
            printf("get memory :%d byte\r\n", i * 128);
            free(ptr);
            printf("free memory :%d byte\r\n", i * 128);
            ptr = NULL;
            bflb_mtimer_delay_ms(100);
        } else {
            printf("try to get %d byte memory failed!\r\n", i * 128);

            return -1;
        }
    }
    return 0;
}

int main(void)
{
    board_init();

    if (memheap_test() == -1) {
        printf("memheap test fail\r\n");
        while (1) {
        }
    }
    printf("memheap test success\r\n");
    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
