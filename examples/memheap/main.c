#include "bflb_core.h"
#include "bflb_mtimer.h"
#include "board.h"
#include "stdlib.h"
#include "string.h"
#include "mm.h"

#if defined(CONFIG_FREERTOS)
#include <FreeRTOS.h>
#include "task.h"
#endif

#if defined(CONFIG_SHELL)
#include "shell.h"
#endif

#define DBG_TAG "MAIN"
#include "log.h"

#if defined(CONFIG_FREERTOS)
static TaskHandle_t heap_test_hd;
#endif

void memheap_test(void *param)
{
    char *ptr = NULL;

    LOG_I("memheap test start\r\n");

    for (int i = 1;; i++) {
        ptr = malloc(i * 1024 * 10);

        if (ptr != NULL) {
            printf("ptr :%p\r\n", ptr);
            printf("get memory :%d byte\r\n", i * 1024 * 10);

            bflb_mtimer_delay_ms(100);

            free(ptr);
            ptr = NULL;
        } else {
            printf("try to get %d byte memory failed!\r\n", i * 1024 * 10);
            break;
        }
    }

    LOG_I("memheap test end\r\n");

#if defined(CONFIG_SHELL)
    extern int cmd_mm_info(int argc, char **argv);
    cmd_mm_info(1, NULL);
#endif

    while (1) {
#if defined(CONFIG_FREERTOS)
        vTaskDelay(1000 / portTICK_PERIOD_MS);
#else
        bflb_mtimer_delay_ms(1000);
#endif
    }
}

int main(void)
{
    board_init();

    LOG_I("mem heap case\r\n");

#if defined(CONFIG_SHELL)
    extern int cmd_mm_info(int argc, char **argv);
    cmd_mm_info(1, NULL);
#endif

#if defined(CONFIG_FREERTOS)

#if defined(CONFIG_SHELL)
    /* shell init */
    struct bflb_device_s *uart0 = bflb_device_get_by_name("uart0");
    extern void shell_init_with_task(struct bflb_device_s * shell);
    shell_init_with_task(uart0);
    LOG_I("Shell ready\r\n");
#endif

    xTaskCreate(memheap_test, (char *)"memheap_test", 2048, NULL, 2, &heap_test_hd);
    vTaskStartScheduler();

#else

    memheap_test(NULL);

#endif
}
