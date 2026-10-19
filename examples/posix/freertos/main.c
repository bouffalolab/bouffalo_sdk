#include <FreeRTOS.h>
#include "semphr.h"
#include "board.h"

static void proc_hellow_entry(void *pvParameters)
{
    extern int mqueues_test(void);
    mqueues_test();
    extern int semaphores_test(void);
    semaphores_test();

    vTaskDelete(NULL);
}

int main(void)
{
    board_init();


    puts("[OS] Starting proc_hellow_entry task...\r\n");
    xTaskCreate(proc_hellow_entry, (char*)"hellow", 2048, NULL, 6, NULL);

    vTaskStartScheduler();

    while (1) {
    }
}
