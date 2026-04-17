/**
 * @file main.c
 * @brief BLE Beacon Demo - full lifecycle cycle test
 *
 * Runs 3 cycles of:
 *   btble_controller_init -> btble_adv_init -> btble_adv_set_parameter ->
 *   btble_adv_set_data -> btble_adv_start -> (wait) ->
 *   btble_adv_stop -> btble_adv_deinit -> btble_controller_deinit
 *
 * Requires CONFIG_BLE_BEACON_ONLY=y and BL616CL chip.
 */

#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include "task.h"
#include "board.h"
#include "mm.h"

#include "btble_lib_api.h"
#include "bl616cl_glb.h"
#include "rfparam_adapter.h"

#include "bflb_mtd.h"
#include "easyflash.h"

/**************************************************************************************************
 * Beacon Configuration
 **************************************************************************************************/

#define BEACON_COMPANY_ID    0x004C  /* Apple, Inc. */
#define BEACON_TYPE          0x0215  /* iBeacon type */
#define BEACON_MAJOR         0x0001
#define BEACON_MINOR         0x0002
#define BEACON_TX_POWER      0xC5    /* -59 dBm */

static const uint8_t beacon_uuid[16] = {
    0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78,
    0x89, 0x9A, 0xAB, 0xBC, 0xCD, 0xDE, 0xEF, 0xF0
};

/**************************************************************************************************
 * Beacon Data Builder
 **************************************************************************************************/

static int build_ibeacon_data(uint8_t *buf, size_t buf_size)
{
    uint8_t *p = buf;

    if (buf_size < 28) {
        return -1;
    }

    /* Flags */
    *p++ = 0x02;
    *p++ = 0x01;
    *p++ = 0x06;

    /* Manufacturer Specific Data */
    *p++ = 26;
    *p++ = 0xFF;
    *p++ = (BEACON_COMPANY_ID & 0xFF);
    *p++ = ((BEACON_COMPANY_ID >> 8) & 0xFF);
    *p++ = ((BEACON_TYPE >> 8) & 0xFF);
    *p++ = (BEACON_TYPE & 0xFF);
    *p++ = 0x15;
    memcpy(p, beacon_uuid, 16);
    p += 16;
    *p++ = ((BEACON_MAJOR >> 8) & 0xFF);
    *p++ = (BEACON_MAJOR & 0xFF);
    *p++ = ((BEACON_MINOR >> 8) & 0xFF);
    *p++ = (BEACON_MINOR & 0xFF);
    *p++ = BEACON_TX_POWER;

    return (int)(p - buf);
}

/**************************************************************************************************
 * Heap Helper
 **************************************************************************************************/

static size_t heap_free(void)
{
    return kfree_size(0);
}

/**************************************************************************************************
 * Beacon Cycle Test Task
 **************************************************************************************************/

static void beacon_cycle_task(void *pvParameters)
{
    const int NUM_CYCLES = 3;
    const int ADV_DURATION_MS = 3000;

    uint8_t adv_data[BTBLE_ADV_DATA_MAX_LEN];
    int adv_data_len;
    struct btble_adv_params params;
    int err;

    printf("\r\n========================================\r\n");
    printf("  BLE Beacon Lifecycle Cycle Test\r\n");
    printf("  Sequence: ctrl_init -> adv_init -> set_params\r\n");
    printf("            -> set_data -> adv_start -> adv_stop\r\n");
    printf("            -> adv_deinit -> ctrl_deinit\r\n");
    printf("  Cycles: %d, ADV duration: %dms\r\n", NUM_CYCLES, ADV_DURATION_MS);
    printf("========================================\r\n\r\n");

    /* Build beacon data once (same for all cycles) */
    adv_data_len = build_ibeacon_data(adv_data, sizeof(adv_data));
    if (adv_data_len < 0) {
        printf("[ERR] Failed to build beacon data\r\n");
        vTaskDelete(NULL);
        return;
    }

    memset(&params, 0, sizeof(params));
    params.adv_interval_min  = 0x00A0;  /* 100 ms */
    params.adv_interval_max  = 0x00F0;  /* 150 ms */
    params.adv_type          = BTBLE_ADV_TYPE_NON_CONNECTABLE_UNDIRECTED;
    params.own_addr_type     = BTBLE_ADDR_TYPE_PUBLIC;
    params.adv_channel_map   = BTBLE_ADV_CHANNEL_ALL;
    params.adv_filter_policy = BTBLE_ADV_FILTER_ALLOW_ALL;

    size_t heap_baseline = heap_free();
    printf("[BASELINE] heap free: %u bytes\r\n\r\n", (unsigned)heap_baseline);

    for (int cycle = 1; cycle <= NUM_CYCLES; cycle++) {
        size_t h_prev, h_cur;

        printf("--- Cycle %d/%d ---\r\n", cycle, NUM_CYCLES);

        /* Step 1: controller init */
        h_prev = heap_free();
        btble_controller_init(configMAX_PRIORITIES - 1);
        h_cur = heap_free();
        printf("  [ctrl_init]  heap used: %u  free: %u\r\n",
               (unsigned)(h_prev - h_cur), (unsigned)h_cur);

        {
            uint8_t *em_addr = NULL;
            int em_size = 0;
            btble_controller_remaining_mem(&em_addr, &em_size);
            printf("  [EM]         remaining: %d bytes\r\n", em_size);
        }

        /* Step 2: adv init (HCI_Reset + Set_Event_Mask + queue alloc) */
        h_prev = h_cur;
        err = btble_adv_init();
        h_cur = heap_free();
        printf("  [adv_init]   err=%d  heap used: %u  free: %u\r\n",
               err, (unsigned)(h_prev - h_cur), (unsigned)h_cur);
        if (err != 0) {
            printf("  [ERR] adv_init failed, skipping cycle\r\n");
            btble_controller_deinit();
            continue;
        }

        /* Step 3: set parameters */
        err = btble_adv_set_parameter(&params);
        printf("  [set_params] err=%d\r\n", err);

        /* Step 4: set adv data */
        err = btble_adv_set_data(adv_data, (uint8_t)adv_data_len);
        printf("  [set_data]   err=%d  len=%d\r\n", err, adv_data_len);

        /* Step 5: start advertising */
        err = btble_adv_start();
        printf("  [adv_start]  err=%d  -> %s\r\n",
               err, err == 0 ? "OK" : "FAILED");

        /* Advertise for ADV_DURATION_MS */
        vTaskDelay(pdMS_TO_TICKS(ADV_DURATION_MS));

        /* Step 6: stop advertising */
        err = btble_adv_stop();
        printf("  [adv_stop]   err=%d\r\n", err);

        /* Step 7: adv deinit (queue free + state reset) */
        h_prev = h_cur;
        btble_adv_deinit();
        h_cur = heap_free();
        printf("  [adv_deinit] heap recovered: %u  free: %u\r\n",
               (unsigned)(h_cur - h_prev), (unsigned)h_cur);

        /* Step 8: controller deinit */
        h_prev = h_cur;
        btble_controller_deinit();
        h_cur = heap_free();
        printf("  [ctrl_deinit] heap recovered: %u  free: %u\r\n",
               (unsigned)(h_cur - h_prev), (unsigned)h_cur);

        /* Check for leaks */
        if (h_cur >= heap_baseline) {
            printf("  [CHECK] heap fully recovered\r\n");
        } else {
            printf("  [CHECK] heap leak: %u bytes (baseline=%u, now=%u)\r\n",
                   (unsigned)(heap_baseline - h_cur),
                   (unsigned)heap_baseline, (unsigned)h_cur);
        }

        printf("\r\n");
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    printf("========================================\r\n");
    printf("  Beacon lifecycle test DONE (%d cycles)\r\n", NUM_CYCLES);
    printf("  Final heap free: %u bytes\r\n", (unsigned)heap_free());
    printf("========================================\r\n");

    vTaskDelete(NULL);
}

/**************************************************************************************************
 * Main
 **************************************************************************************************/

int main(void)
{
    board_init();

    bflb_mtd_init();
    easyflash_init();

    if (0 != rfparam_init(0, NULL, 0)) {
        printf("PHY RF init failed!\r\n");
        return 0;
    }

    xTaskCreate(beacon_cycle_task, "beacon_cycle", 1024, NULL,
                configMAX_PRIORITIES - 2, NULL);

    vTaskStartScheduler();

    while (1) {}
}
