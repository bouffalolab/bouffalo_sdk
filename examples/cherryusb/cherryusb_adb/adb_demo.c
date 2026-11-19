/*
 * Copyright (c) 2024, Bouffalo Lab
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file adb_demo.c
 * @brief CherryUSB ADB device demo: descriptors, USB glue and RX/TX task.
 *
 * The CherryUSB ADB class only implements the ADB transport. This file
 * supplies the USB descriptors, the application hooks used by the class and a
 * small FreeRTOS task that turns the "shell:" service into a line based shell.
 *
 * Endpoint callbacks (including the usbd_adb_notify_* hooks) run in USB
 * interrupt context, so they only push data into FreeRTOS primitives. All
 * command execution and all IN transfers are done from the worker task.
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "usbd_core.h"
#include "usbd_adb.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "stream_buffer.h"
#include "task.h"

#include "adb_demo.h"

#define DBG_TAG "ADB"
#include "log.h"

/** @name USB endpoint addresses used by the ADB interface
 * @{ */
#define ADB_IN_EP  0x81
#define ADB_OUT_EP 0x02
/** @} */

/** @name USB device identification
 * @{ */
#define USBD_VID       0x18D1
#define USBD_PID       0x4EE7
#define USBD_MAX_POWER 100
/** @} */

#ifdef CONFIG_USB_HS
#define ADB_MAX_MPS 512
#else
#define ADB_MAX_MPS 64
#endif

/** @brief Size of the interface descriptor set emitted by ADB_DESCRIPTOR_INIT. */
#define ADB_INTERFACE_DESC_LEN (9 + 2 * 7)
/** @brief Total configuration descriptor length. */
#define USB_CONFIG_SIZE (9 + ADB_INTERFACE_DESC_LEN)

#define ADB_RX_STREAM_SIZE    1024
#define ADB_OPEN_QUEUE_DEPTH  4
#define ADB_TX_TIMEOUT_MS     1000
#define ADB_TASK_STACK_SIZE   1024
#define ADB_TASK_PRIO         15
#define ADB_COMMAND_LINE_SIZE 128

/** @brief One accepted A_OPEN request, handed from ISR to the worker task. */
struct adb_open_event {
    uint32_t localid;
    uint32_t len;
    uint8_t service[ADB_MAX_SERVICE_LEN];
};

static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0100, 0x01)
};

static const uint8_t config_descriptor[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    ADB_DESCRIPTOR_INIT(0x00, ADB_IN_EP, ADB_OUT_EP, ADB_MAX_MPS)
};

static const uint8_t device_quality_descriptor[] = {
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x00,
    0x00,
};

static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "Bouffalo Lab",               /* Manufacturer */
    "CherryUSB ADB Demo",         /* Product */
    "2024111701",                 /* Serial Number */
};

static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    (void)speed;
    return device_descriptor;
}

static const uint8_t *config_descriptor_callback(uint8_t speed)
{
    (void)speed;
    return config_descriptor;
}

static const uint8_t *device_quality_descriptor_callback(uint8_t speed)
{
    (void)speed;
    return device_quality_descriptor;
}

static const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    (void)speed;
    if (index >= (sizeof(string_descriptors) / sizeof(string_descriptors[0]))) {
        return NULL;
    }
    return string_descriptors[index];
}

static const struct usb_descriptor adb_usb_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback,
};

static struct usbd_interface adb_interface;

static StreamBufferHandle_t adb_rx_stream;
static SemaphoreHandle_t adb_tx_done;
static QueueHandle_t adb_open_queue;
static TaskHandle_t adb_task_handle;

static volatile uint32_t adb_cmd_line_len;
static char adb_cmd_line[ADB_COMMAND_LINE_SIZE];
static bool adb_last_was_cr;

/**
 * @brief Wake the ADB worker task from task or interrupt context.
 * @param[in,out] woken Set to pdTRUE when a context switch is required.
 */
static void adb_task_wake(BaseType_t *woken)
{
    if (adb_task_handle) {
        vTaskNotifyGiveFromISR(adb_task_handle, woken);
    }
}

/**
 * @brief Forward received shell bytes to the worker task.
 * @param[in] data Received bytes.
 * @param[in] len Number of bytes.
 * @note Runs in USB interrupt context.
 */
void usbd_adb_notify_shell_read(uint8_t *data, uint32_t len)
{
    BaseType_t woken = pdFALSE;

    if (adb_rx_stream && data && len) {
        (void)xStreamBufferSendFromISR(adb_rx_stream, data, len, &woken);
    }
    adb_task_wake(&woken);
    portYIELD_FROM_ISR(woken);
}

/**
 * @brief File-sync channel hook, intentionally unused in this demo.
 * @param[in] data Received bytes; unused.
 * @param[in] len Number of bytes; unused.
 * @note Runs in USB interrupt context.
 */
void usbd_adb_notify_file_read(uint8_t *data, uint32_t len)
{
    (void)data;
    (void)len;
}

/**
 * @brief Signal the worker task that an IN transfer has completed.
 * @note Runs in USB interrupt context.
 */
void usbd_adb_notify_write_done(void)
{
    BaseType_t woken = pdFALSE;

    if (adb_tx_done) {
        (void)xSemaphoreGiveFromISR(adb_tx_done, &woken);
    }
    portYIELD_FROM_ISR(woken);
}

/**
 * @brief Queue an accepted service open request for the worker task.
 * @param[in] localid ADB_SHELL_LOALID or ADB_FILE_LOALID.
 * @param[in] service Service text after "shell:"/"sync:".
 * @param[in] len Length of @p service.
 * @note Runs in USB interrupt context.
 */
void usbd_adb_notify_open(uint32_t localid, const uint8_t *service, uint32_t len)
{
    struct adb_open_event event;
    BaseType_t woken = pdFALSE;

    memset(&event, 0, sizeof(event));
    event.localid = localid;
    if (len > sizeof(event.service) - 1) {
        len = sizeof(event.service) - 1;
    }
    if (service && len) {
        memcpy(event.service, service, len);
    }
    event.len = len;

    if (adb_open_queue) {
        (void)xQueueSendFromISR(adb_open_queue, &event, &woken);
    }
    adb_task_wake(&woken);
    portYIELD_FROM_ISR(woken);
}

/**
 * @brief USB device event handler.
 * @param[in] busid USB device-controller index; unused.
 * @param[in] event CherryUSB device event identifier.
 * @note Runs in USB interrupt context, so it must stay short.
 */
static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    (void)busid;

    switch (event) {
        case USBD_EVENT_RESET:
        case USBD_EVENT_DISCONNECTED:
            adb_cmd_line_len = 0;
            adb_last_was_cr = false;
            adb_shell_set_interactive(false);
            break;
        default:
            break;
    }
}

/**
 * @brief Send one buffer over the ADB shell channel, splitting large payloads.
 * @param[in] data Buffer to send.
 * @param[in] len Number of bytes.
 */
void adb_demo_write(const void *data, uint32_t len)
{
    const uint8_t *cursor = (const uint8_t *)data;

    if (!cursor || !len || !adb_tx_done) {
        return;
    }

    while (len) {
        uint32_t chunk = (len > ADB_DEMO_MAX_PAYLOAD) ? ADB_DEMO_MAX_PAYLOAD : len;

        /* Drop completion signals left over from an earlier transfer. */
        while (xSemaphoreTake(adb_tx_done, 0) == pdTRUE) {
        }

        {
            uint32_t retry = 0;

            /* The class may be busy sending its own ADB response; retry. */
            while (usbd_abd_write(ADB_SHELL_LOALID, cursor, chunk) != 0) {
                if (++retry > 100) {
                    LOG_E("ADB write busy\r\n");
                    return;
                }
                vTaskDelay(pdMS_TO_TICKS(1));
            }
        }

        if (xSemaphoreTake(adb_tx_done, pdMS_TO_TICKS(ADB_TX_TIMEOUT_MS)) != pdTRUE) {
            LOG_E("ADB write timeout, host may be gone\r\n");
            return;
        }

        cursor += chunk;
        len -= chunk;
    }
}

/**
 * @brief printf-style output helper for the ADB shell.
 * @param[in] fmt printf format string.
 * @return Number of bytes formatted, or a negative value on error.
 */
int adb_demo_printf(const char *fmt, ...)
{
    char buffer[512];
    va_list args;
    int len;

    va_start(args, fmt);
    len = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    if (len <= 0) {
        return len;
    }
    if (len > (int)sizeof(buffer) - 1) {
        len = (int)sizeof(buffer) - 1;
    }

    adb_demo_write(buffer, (uint32_t)len);
    return len;
}

/**
 * @brief Execute the command currently stored in adb_cmd_line.
 */
static void adb_handle_shell_line(void)
{
    int ret;

    adb_cmd_line[adb_cmd_line_len] = '\0';
    adb_demo_printf("\r\n");
    ret = adb_shell_execute(adb_cmd_line);
    adb_cmd_line_len = 0;

    if (ret == ADB_SHELL_EXIT) {
        adb_demo_printf("bye\r\n");
        adb_shell_set_interactive(false);
        usbd_adb_close(ADB_SHELL_LOALID);
    } else if (adb_shell_is_interactive()) {
        adb_demo_printf(ADB_SHELL_PROMPT);
    }
}

/**
 * @brief Consume shell input, echoing printable characters and running lines.
 * @param[in] data Received bytes.
 * @param[in] len Number of bytes.
 */
static void adb_handle_shell_data(const uint8_t *data, uint32_t len)
{
    char echo[64];
    uint32_t echo_len = 0;

    for (uint32_t i = 0; i < len; i++) {
        char c = (char)data[i];

        /* Interactive terminals send CR for Enter, one-shot commands are
         * executed from the A_OPEN service string. Accept CR, LF and CRLF. */
        if (c == '\r' || c == '\n') {
            if ((c == '\n') && adb_last_was_cr) {
                adb_last_was_cr = false;
                continue;
            }
            adb_last_was_cr = (c == '\r');
            if (echo_len) {
                adb_demo_write(echo, echo_len);
                echo_len = 0;
            }
            adb_handle_shell_line();
            continue;
        }

        adb_last_was_cr = false;

        if (c == 0x08 || c == 0x7f) { /* Backspace */
            if (adb_cmd_line_len) {
                adb_cmd_line_len--;
                if (echo_len + 3 <= sizeof(echo)) {
                    echo[echo_len++] = '\b';
                    echo[echo_len++] = ' ';
                    echo[echo_len++] = '\b';
                }
            }
            continue;
        }

        if (c == 0x03) { /* Ctrl-C: abandon the current line */
            if (echo_len) {
                adb_demo_write(echo, echo_len);
                echo_len = 0;
            }
            adb_cmd_line_len = 0;
            adb_demo_printf("^C\r\n" ADB_SHELL_PROMPT);
            continue;
        }

        if (c == 0x04) { /* Ctrl-D: close the shell */
            if (echo_len) {
                adb_demo_write(echo, echo_len);
                echo_len = 0;
            }
            adb_cmd_line_len = 0;
            adb_demo_printf("\r\nbye\r\n");
            adb_shell_set_interactive(false);
            usbd_adb_close(ADB_SHELL_LOALID);
            return;
        }

        if (c >= 0x20 && c < 0x7f) {
            if (adb_cmd_line_len < sizeof(adb_cmd_line) - 1) {
                adb_cmd_line[adb_cmd_line_len++] = c;
                if (echo_len < sizeof(echo)) {
                    echo[echo_len++] = c;
                }
            }
        }
    }

    if (echo_len) {
        adb_demo_write(echo, echo_len);
    }
}

/**
 * @brief Handle one accepted ADB service open request.
 * @param[in] event Open request from the USB interrupt handler.
 */
static void adb_handle_open(const struct adb_open_event *event)
{
    if (event->localid == ADB_SHELL_LOALID) {
        adb_cmd_line_len = 0;
        adb_last_was_cr = false;

        if (event->len == 0) {
            /* Bare "shell:" means the host wants an interactive shell. */
            adb_shell_enter_interactive();
        } else {
            char command[ADB_MAX_SERVICE_LEN];
            uint32_t len = event->len;

            if (len >= sizeof(command)) {
                len = sizeof(command) - 1;
            }
            memcpy(command, event->service, len);
            command[len] = '\0';

            /* "adb shell <command>": run it and close the stream afterwards,
             * which tells the host that the command has finished. */
            adb_shell_set_interactive(false);
            (void)adb_shell_execute(command);
            usbd_adb_close(ADB_SHELL_LOALID);
        }
    } else if (event->localid == ADB_FILE_LOALID) {
        /* File sync (adb push/pull) is not implemented by this demo. Close
         * the stream so the host fails fast instead of waiting forever. */
        usbd_adb_close(ADB_FILE_LOALID);
    }
}

/**
 * @brief ADB worker task: dispatch service opens and shell input.
 * @param[in] param Unused FreeRTOS task parameter.
 */
static void adb_task(void *param)
{
    struct adb_open_event event;
    uint8_t data[64];

    (void)param;

    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        while (adb_open_queue && xQueueReceive(adb_open_queue, &event, 0) == pdTRUE) {
            adb_handle_open(&event);
        }

        if (!adb_shell_is_interactive()) {
            continue;
        }

        for (;;) {
            size_t n = xStreamBufferReceive(adb_rx_stream, data, sizeof(data), 0);

            if (n == 0) {
                break;
            }
            adb_handle_shell_data(data, (uint32_t)n);
        }
    }
}

/**
 * @brief Register the ADB device and start its worker task.
 */
void adb_demo_init(void)
{
    adb_rx_stream = xStreamBufferCreate(ADB_RX_STREAM_SIZE, 1);
    adb_tx_done = xSemaphoreCreateBinary();
    adb_open_queue = xQueueCreate(ADB_OPEN_QUEUE_DEPTH, sizeof(struct adb_open_event));

    if (!adb_rx_stream || !adb_tx_done || !adb_open_queue) {
        LOG_E("Not enough heap for the ADB demo\r\n");
        return;
    }

    if (xTaskCreate(adb_task, (char *)"adb", ADB_TASK_STACK_SIZE, NULL,
                    ADB_TASK_PRIO, &adb_task_handle) != pdPASS) {
        LOG_E("Failed to create the ADB task\r\n");
        return;
    }

    usbd_desc_register(0, &adb_usb_descriptor);
    usbd_add_interface(0, usbd_adb_init_intf(0, &adb_interface, ADB_IN_EP, ADB_OUT_EP));
    usbd_initialize(0, 0, usbd_event_handler);

    LOG_I("ADB device ready, VID:PID=%04x:%04x\r\n", USBD_VID, USBD_PID);
    LOG_I("Connect USB to a host and run: adb devices\r\n");
}
