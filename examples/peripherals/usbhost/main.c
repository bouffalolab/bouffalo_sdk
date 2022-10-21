#include <FreeRTOS.h>
#include "semphr.h"
#include "usbh_core.h"
#include "board.h"

static uint8_t freertos_heap[configTOTAL_HEAP_SIZE];

static HeapRegion_t xHeapRegions[] = {
    { (uint8_t *)freertos_heap, 0 },
    { NULL, 0 }, /* Terminates the array. */
    { NULL, 0 }  /* Terminates the array. */
};

extern void usbh_class_test(void);

int main(void)
{
    board_init();

    xHeapRegions[0].xSizeInBytes = configTOTAL_HEAP_SIZE;
    vPortDefineHeapRegions(xHeapRegions);

    printf("Starting usb host task...\r\n");
    usbh_initialize();
    usbh_class_test();
    vTaskStartScheduler();

    while (1) {
    }
}