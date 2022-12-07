#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "shell.h"
#include <FreeRTOS.h>
#include "semphr.h"
#include "board.h"

static uint8_t freertos_heap[configTOTAL_HEAP_SIZE];

static HeapRegion_t xHeapRegions[] = {
    { (uint8_t *)freertos_heap, 0 },
    { NULL, 0 }, /* Terminates the array. */
    { NULL, 0 }  /* Terminates the array. */
};

static struct bflb_device_s *uart0;

extern void shell_init_with_task(struct bflb_device_s *shell);

int main(void)
{
    board_init();
    xHeapRegions[0].xSizeInBytes = configTOTAL_HEAP_SIZE;
    vPortDefineHeapRegions(xHeapRegions);

    configASSERT((configMAX_PRIORITIES > 4));

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    vTaskStartScheduler();

    while (1) {
    }
}
