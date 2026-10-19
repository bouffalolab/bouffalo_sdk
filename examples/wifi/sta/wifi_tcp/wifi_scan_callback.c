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

#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "async_event.h"
#include "shell.h"
#include "wifi_mgmr_ext.h"

#define DBG_TAG "SCAN_CB"
#include "log.h"

#define WIFI_SCAN_FRAME_MAX_LEN     2304
#define WIFI_SCAN_FRAME_QUEUE_DEPTH 4

struct wifi_scan_frame_copy {
    uint16_t length;
    uint16_t frequency;
    int16_t rssi;
    uint8_t data[WIFI_SCAN_FRAME_MAX_LEN];
};

struct wifi_scan_frame_demo {
    QueueHandle_t free_queue;
    QueueHandle_t frame_queue;
    TaskHandle_t task;
    volatile uint32_t queued_count;
    volatile uint32_t dropped_count;
    volatile bool scan_active;
};

static struct wifi_scan_frame_copy scan_frame_pool[WIFI_SCAN_FRAME_QUEUE_DEPTH];
static struct wifi_scan_frame_demo scan_frame_demo;

static void wifi_scan_frame_callback(void *arg, void *frame_queue, const wifi_mgmr_scan_frame_t *frame)
{
    struct wifi_scan_frame_demo *demo = arg;
    struct wifi_scan_frame_copy *frame_copy;
    uint8_t frame_type;

    if (!demo || !frame_queue || !frame || !frame->data || frame->length < 24 ||
        frame->length > WIFI_SCAN_FRAME_MAX_LEN) {
        return;
    }

    frame_type = frame->data[0] & 0xfc;
    if (frame_type != 0x80 && frame_type != 0x50) {
        return;
    }

    if (xQueueReceive(demo->free_queue, &frame_copy, 0) != pdTRUE) {
        demo->dropped_count++;
        return;
    }

    frame_copy->length = frame->length;
    frame_copy->frequency = frame->frequency;
    frame_copy->rssi = frame->rssi;
    memcpy(frame_copy->data, frame->data, frame->length);

    if (xQueueSend((QueueHandle_t)frame_queue, &frame_copy, 0) != pdTRUE) {
        demo->dropped_count++;
        xQueueSend(demo->free_queue, &frame_copy, 0);
        return;
    }

    demo->queued_count++;
}

static void wifi_scan_frame_task(void *param)
{
    struct wifi_scan_frame_demo *demo = param;
    struct wifi_scan_frame_copy *frame;
    const uint8_t *bssid;
    const char *type;

    while (1) {
        if (xQueueReceive(demo->frame_queue, &frame, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        type = ((frame->data[0] & 0xfc) == 0x80) ? "Beacon" : "Probe Response";
        bssid = &frame->data[16];
        LOG_I("%s, bssid %02X:%02X:%02X:%02X:%02X:%02X, freq %u MHz, rssi %d dBm, len %u\r\n",
              type, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5],
              frame->frequency, frame->rssi, frame->length);

        xQueueSend(demo->free_queue, &frame, portMAX_DELAY);
    }
}

static void wifi_scan_frame_event_handler(async_input_event_t event, void *priv)
{
    struct wifi_scan_frame_demo *demo = priv;

    if (!event || event->code != CODE_WIFI_ON_SCAN_DONE || !demo->scan_active) {
        return;
    }

    demo->scan_active = false;
    LOG_I("scan done, queued %lu, dropped %lu\r\n",
          (unsigned long)demo->queued_count,
          (unsigned long)demo->dropped_count);
}

static int wifi_scan_frame_demo_init(void)
{
    struct wifi_scan_frame_copy *frame;

    if (scan_frame_demo.frame_queue) {
        return 0;
    }

    scan_frame_demo.free_queue = xQueueCreate(WIFI_SCAN_FRAME_QUEUE_DEPTH, sizeof(frame));
    scan_frame_demo.frame_queue = xQueueCreate(WIFI_SCAN_FRAME_QUEUE_DEPTH, sizeof(frame));
    if (!scan_frame_demo.free_queue || !scan_frame_demo.frame_queue) {
        goto fail;
    }

    for (size_t i = 0; i < WIFI_SCAN_FRAME_QUEUE_DEPTH; i++) {
        frame = &scan_frame_pool[i];
        if (xQueueSend(scan_frame_demo.free_queue, &frame, 0) != pdTRUE) {
            goto fail;
        }
    }

    if (xTaskCreate(wifi_scan_frame_task, "scan frame", 512, &scan_frame_demo, 12,
                    &scan_frame_demo.task) != pdPASS) {
        goto fail;
    }

    if (async_register_event_filter(EV_WIFI, wifi_scan_frame_event_handler, &scan_frame_demo) < 0) {
        vTaskDelete(scan_frame_demo.task);
        scan_frame_demo.task = NULL;
        goto fail;
    }

    return 0;

fail:
    if (scan_frame_demo.free_queue) {
        vQueueDelete(scan_frame_demo.free_queue);
    }
    if (scan_frame_demo.frame_queue) {
        vQueueDelete(scan_frame_demo.frame_queue);
    }
    scan_frame_demo.free_queue = NULL;
    scan_frame_demo.frame_queue = NULL;
    return -1;
}

static int cmd_wifi_scan_callback(int argc, char **argv)
{
    wifi_mgmr_scan_params_t config = { 0 };

    (void)argc;
    (void)argv;

    if (wifi_scan_frame_demo_init() < 0) {
        LOG_E("Failed to initialize scan frame queues\r\n");
        return -1;
    }

    if (scan_frame_demo.scan_active) {
        LOG_W("A callback scan is already running\r\n");
        return -1;
    }

    scan_frame_demo.queued_count = 0;
    scan_frame_demo.dropped_count = 0;
    scan_frame_demo.scan_active = true;
    config.frame_cb = wifi_scan_frame_callback;
    config.frame_cb_arg = &scan_frame_demo;
    config.frame_queue = scan_frame_demo.frame_queue;

    if (wifi_mgmr_sta_scan(&config) < 0) {
        scan_frame_demo.scan_active = false;
        LOG_E("Failed to start callback scan\r\n");
        return -1;
    }

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_wifi_scan_callback, wifi_scan_callback,
                       scan and process raw Beacon/Probe Response frames);
