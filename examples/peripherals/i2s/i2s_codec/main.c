#include <stdio.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <task.h>

#include "board.h"
#include "bflb_core.h"
#include "shell.h"
#include "wave_player.h"
#include "recorder.h"

static int cmd_cpu_diag(int argc, char **argv)
{
#if (configGENERATE_RUN_TIME_STATS == 1)
    uint32_t window_ms = 1000U;
    if (argc >= 2) {
        unsigned long v = strtoul(argv[1], NULL, 0);
        if (v < 50UL || v > 60000UL) {
            printf("usage: cpu_diag [window_ms:50..60000]\r\n");
            return -1;
        }
        window_ms = (uint32_t)v;
    }

    configRUN_TIME_COUNTER_TYPE total0 = (configRUN_TIME_COUNTER_TYPE)portGET_RUN_TIME_COUNTER_VALUE();
    configRUN_TIME_COUNTER_TYPE idle0 = ulTaskGetIdleRunTimeCounter();

    vTaskDelay(pdMS_TO_TICKS(window_ms));

    configRUN_TIME_COUNTER_TYPE total1 = (configRUN_TIME_COUNTER_TYPE)portGET_RUN_TIME_COUNTER_VALUE();
    configRUN_TIME_COUNTER_TYPE idle1 = ulTaskGetIdleRunTimeCounter();

    if (total1 <= total0 || idle1 < idle0) {
        printf("cpu_diag: invalid sample (counter wrap or too short window)\r\n");
        return -1;
    }

    uint64_t delta_total = (uint64_t)(total1 - total0);
    uint64_t delta_idle = (uint64_t)(idle1 - idle0);
    if (delta_idle > delta_total) {
        delta_idle = delta_total;
    }

    uint64_t cpu_pm = ((delta_total - delta_idle) * 1000ULL) / delta_total;
    uint64_t idle_pm = 1000ULL - cpu_pm;

    printf("cpu_diag: window=%lu ms cpu=%lu.%lu%% idle=%lu.%lu%% total=%llu idle_ticks=%llu\r\n",
           (unsigned long)window_ms, (unsigned long)(cpu_pm / 10ULL), (unsigned long)(cpu_pm % 10ULL),
           (unsigned long)(idle_pm / 10ULL), (unsigned long)(idle_pm % 10ULL), (unsigned long long)delta_total,
           (unsigned long long)delta_idle);
    return 0;
#else
    (void)argc;
    (void)argv;
    printf("cpu_diag: runtime stats disabled\r\n");
    return -1;
#endif
}

SHELL_CMD_EXPORT_ALIAS(cmd_cpu_diag, cpu_diag, "sample CPU usage: cpu_diag [window_ms]");

static void recorder_init_task(void *arg)
{
    (void)arg;

    if (recorder_storage_init() != 0) {
        printf("[WARN] recorder storage init failed, recording to SD disabled\r\n");
    }

    vTaskDelete(NULL);
}

void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    while (1) {}
}

void vAssertCalled(void)
{
    taskDISABLE_INTERRUPTS();
    while (1) {}
}

int main(void)
{
    board_init();

    struct bflb_device_s *uart0 = bflb_device_get_by_name("uart0");
    if (uart0 != NULL) {
        shell_init_with_task(uart0);
    } else {
        printf("[WARN] uart0 not found, shell disabled\r\n");
    }

    wave_player_hw_cfg_t audio_hw_cfg = {
        .i2s_dev_name = "i2s0",
        .i2c_dev_name = "i2c0",
        .dma_tx_dev_name = "dma0_ch0",
        .dma_rx_dev_name = "dma0_ch1",
    };
    if (wave_player_init(&audio_hw_cfg) != 0) {
        printf("[ERROR] wave_player_init failed\r\n");
        while (1) {}
    }

    if (xTaskCreate(recorder_init_task, (char *)"rec_init", 2048, NULL, 6, NULL) != pdPASS) {
        printf("[WARN] create rec_init task failed, recorder init skipped\r\n");
    }

    printf("i2s_codec ready. see audio command using help\r\n");
    vTaskStartScheduler();

    while (1) {}
}
