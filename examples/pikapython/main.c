#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "bflb_uart.h"
#include "board.h"
#include "log.h"
#include "lwip/init.h"
#include "pikaScript.h"

struct bflb_device_s* uartx = NULL;
static uint8_t freertos_heap[configTOTAL_HEAP_SIZE];

static HeapRegion_t xHeapRegions[] = {
    {(uint8_t*)freertos_heap, 0},
    {NULL, 0}, /* Terminates the array. */
    {NULL, 0}  /* Terminates the array. */
};

char pika_platform_getchar(void) {
    while (1) {
        int c = bflb_uart_getchar(uartx);
        if (c != -1) {
            return c;
        }
    }
}

int pika_platform_putchar(char ch) {
    bflb_uart_putchar(uartx, ch);
    return 0;
}

static void consumer_task(void* pvParameters) {
    uartx = bflb_device_get_by_name("uart0");
    bflb_uart_feature_control(uartx, UART_CMD_SET_BAUD_RATE, 115200);
    PikaObj* root = pikaScriptInit();
    pikaScriptShell(root);
    while (1) {
    }
}

int main(void) {
    board_init();
    xHeapRegions[0].xSizeInBytes = configTOTAL_HEAP_SIZE;
    vPortDefineHeapRegions(xHeapRegions);
    printf("Heap size: %d", configTOTAL_HEAP_SIZE);

    xTaskCreate(consumer_task, (char*)"consumer_task", 512, NULL,
                configMAX_PRIORITIES - 2, NULL);
    vTaskStartScheduler();

    while (1) {
    }
}
