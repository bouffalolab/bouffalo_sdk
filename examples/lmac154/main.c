#include "FreeRTOS.h"
#include "task.h"
#include "board.h"

#if defined(BL616)
#include "rfparam_adapter.h"
#endif


#if ( configUSE_TICK_HOOK != 0 )
void vApplicationTickHook( void )
{
    extern void lmac154_monitor(void);
    lmac154_monitor();
}
#endif


int main(void)
{
    board_init();

    configASSERT((configMAX_PRIORITIES > 4));

#if defined(BL616)
    /* Init rf */
    if (0 != rfparam_init(0, NULL, 0)) {
        printf("PHY RF init failed!\r\n");
        return 0;
    }
#endif

    extern void lmac154_example_task(void *pvParameters);

    puts("[OS] Starting lmac154 example task...\r\n");
    xTaskCreate(lmac154_example_task, (char *)"lmac154", 512, NULL, configMAX_PRIORITIES - 1, NULL);

    puts("[OS] Starting OS Scheduler...\r\n");
    vTaskStartScheduler();

    while (1) {
    }
}
