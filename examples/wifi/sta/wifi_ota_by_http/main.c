/****************************************************************************
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include "wifi_mgmr_ext.h"

#include "bflb_irq.h"
#include "bflb_uart.h"
#include "bflb_flash.h"

#include "rfparam_adapter.h"

#include "board.h"
#include "shell.h"
#include "partition.h"

#include "bflb_wdg.h"
#include "ring_buffer.h"

#define DBG_TAG "MAIN"
#include "log.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define WIFI_STACK_SIZE  (1536)
#define TASK_PRIORITY_FW (16)

#define AUTO_CONNECT_WIFI (0)

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

struct bflb_device_s *gpio;
static struct bflb_device_s *uart0;

#if AUTO_CONNECT_WIFI
struct bflb_device_s *wdg;
static TaskHandle_t ota_task_handle;
#endif

#if AUTO_CONNECT_WIFI
// clang-format off
/* config your wifi ssid and password */
static const uint8_t wifi_sta_connet[] = "wifi_sta_connect 21a 12344321\r";
/* config your ota server and port */
static const uint8_t wifi_ota_test[] = "wifi_ota_test 192.168.123.120 3365 /wifi_ota_bl616.bin.ota\r";
/* reboot command */
// static const uint8_t send_buf1[] = "reboot\r";
// clang-format on

extern Ring_Buffer_Type shell_rb;
extern void shell_release_sem(void);

SemaphoreHandle_t sem_wifi_init_done;
SemaphoreHandle_t sem_wifi_connect_done;
SemaphoreHandle_t sem_wifi_disconnect;
#endif

extern void shell_init_with_task(struct bflb_device_s *shell);

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Functions
 ****************************************************************************/

void wifi_start_firmware_task(void *param)
{
    LOG_I("Starting wifi ...\r\n");

    wifi_task_create();

    LOG_I("Starting fhost ...\r\n");
    fhost_init();

    vTaskDelete(NULL);
}

void wifi_event_handler(uint32_t code)
{
    switch (code) {
        case CODE_WIFI_ON_INIT_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_INIT_DONE\r\n", __func__);
            wifi_mgmr_task_start();
        } break;
        case CODE_WIFI_ON_MGMR_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_MGMR_DONE\r\n", __func__);
        } break;
        case CODE_WIFI_ON_SCAN_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_SCAN_DONE\r\n", __func__);
            wifi_mgmr_sta_scanlist();
        } break;
        case CODE_WIFI_ON_CONNECTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_CONNECTED\r\n", __func__);
            void mm_sec_keydump();
            mm_sec_keydump();
        } break;
        case CODE_WIFI_ON_GOT_IP: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_GOT_IP\r\n", __func__);
            LOG_I("[SYS] Memory left is %d Bytes\r\n", kfree_size());
        } break;
        case CODE_WIFI_ON_DISCONNECT: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_DISCONNECT\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STARTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STARTED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STOPPED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STOPPED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STA_ADD: {
            LOG_I("[APP] [EVT] [AP] [ADD] %lld\r\n", xTaskGetTickCount());
        } break;
        case CODE_WIFI_ON_AP_STA_DEL: {
            LOG_I("[APP] [EVT] [AP] [DEL] %lld\r\n", xTaskGetTickCount());
        } break;
        default: {
            LOG_I("[APP] [EVT] Unknown code %u \r\n", code);
        }
    }
}


#if AUTO_CONNECT_WIFI
void ota_task(void *param)
{
    if (xSemaphoreTake(sem_wifi_init_done, portMAX_DELAY) == pdTRUE) {
        Ring_Buffer_Write(&shell_rb, wifi_sta_connet, sizeof(wifi_sta_connet) - 1);
        shell_release_sem();
    }

    if (xSemaphoreTake(sem_wifi_connect_done, portMAX_DELAY) == pdTRUE) {
        Ring_Buffer_Write(&shell_rb, wifi_ota_test, sizeof(wifi_ota_test) - 1);
        shell_release_sem();
    }

    while (1) {
        if (xSemaphoreTake(sem_wifi_disconnect, portMAX_DELAY) == pdTRUE) {
            Ring_Buffer_Write(&shell_rb, wifi_sta_connet, sizeof(wifi_sta_connet) - 1);
            shell_release_sem();
        }
        vTaskDelay(10000);
    }
}
#endif

static void wifi_ota_dump_partition()
{
    pt_table_stuff_config pt_table_stuff[2];
    pt_table_id_type active_id;
    pt_table_stuff_config *pt_stuff;

    /* Set flash operation function, read via xip */
    pt_table_set_flash_operation(bflb_flash_erase, bflb_flash_write, bflb_flash_read);

    active_id = pt_table_get_active_partition_from_ram(pt_table_stuff);
    if (PT_TABLE_ID_INVALID == active_id) {
        printf("No valid PT\r\n");
        return;
    }
    printf("Active PT:%d,Age %d\r\n", active_id, pt_table_stuff[active_id].pt_table.age);

    pt_stuff = &pt_table_stuff[active_id];

    printf("======= PtTable_Config @%p=======\r\n", pt_stuff);
    printf("magicCode 0x%08X;", (unsigned int)(pt_stuff->pt_table.magicCode));
    printf(" version 0x%04X;", pt_stuff->pt_table.version);
    printf(" entryCnt %u;", pt_stuff->pt_table.entryCnt);
    printf(" age %lu;", pt_stuff->pt_table.age);
    printf(" crc32 0x%08X\r\n", (unsigned int)pt_stuff->pt_table.crc32);
    printf(" idx  type device active_index    name    address[0]   address[1]   length[0]    length[1]   age \r\n");
    for (uint32_t i = 0; i < pt_stuff->pt_table.entryCnt; i++) {
        printf("[%02d] ", i);
        printf("  %02u", (pt_stuff->pt_entries[i].type));
        printf("     %u", (pt_stuff->pt_entries[i].device));
        printf("        %u", (pt_stuff->pt_entries[i].active_index));
        printf("       %8s", (pt_stuff->pt_entries[i].name));
        printf("   0x%08lx", (pt_stuff->pt_entries[i].start_address[0]));
        printf("   0x%08lx", (pt_stuff->pt_entries[i].start_address[1]));
        printf("   0x%08lx", (pt_stuff->pt_entries[i].max_len[0]));
        printf("   0x%08lx", (pt_stuff->pt_entries[i].max_len[1]));
        printf("   %lu\r\n", (pt_stuff->pt_entries[i].age));
    }
}

int main(void)
{
    board_init();

#if AUTO_CONNECT_WIFI
    sem_wifi_init_done = xSemaphoreCreateBinary();
    sem_wifi_connect_done = xSemaphoreCreateBinary();
    sem_wifi_disconnect = xSemaphoreCreateBinary();
#endif

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    if (0 != rfparam_init(0, NULL, 0)) {
        LOG_I("PHY RF init failed!\r\n");
        return 0;
    }

    LOG_I("PHY RF init success!\r\n");

    wifi_ota_dump_partition();

    tcpip_init(NULL, NULL);
    xTaskCreate(wifi_start_firmware_task, "wifi init", 1024, NULL, 10, NULL);

#if AUTO_CONNECT_WIFI
    printf("Config Watchdog...\r\n");
    struct bflb_wdg_config_s wdg_cfg;
    wdg_cfg.clock_source = WDG_CLKSRC_32K;
    wdg_cfg.clock_div = 31;
    wdg_cfg.comp_val = 30000;
    wdg_cfg.mode = WDG_MODE_RESET;

    wdg = bflb_device_get_by_name("watchdog");
    bflb_wdg_init(wdg, &wdg_cfg);

    bflb_wdg_start(wdg);
    bflb_wdg_reset_countervalue(wdg);
    printf("Next delay 30s, wdg will reset it.\r\n");

    xTaskCreate(ota_task, "ota_task", 2048, NULL, 15, &ota_task_handle);
#endif

    vTaskStartScheduler();

    while (1) {
    }
}
