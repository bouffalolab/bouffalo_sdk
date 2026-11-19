/**
  ******************************************************************************
  * @file    at_through.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <FreeRTOS.h>

#include "at_main.h"
#include "at_core.h"
#include "at_through.h"
#include "at_net_main.h"
#if defined(CONFIG_ATMODULE_UART) && defined(CONFIG_ATMODULE_BASE)
#include "at_base_config.h"
#endif

static int g_through_linkid = -1;
static int g_through_max_size = 0;
static int g_through_report_size = 0;
static int g_through_transmit_size = 0;
static int g_through_send_size = 0;
static int g_through_recv_size = 0;
static int g_through_report_tag = 0;

#define AT_THROUGH_GUARD_MS 20

static int g_through_escape_count;
static uint64_t g_through_last_input_ms;

#if defined(CONFIG_ATMODULE_NETWORK) && defined(CONFIG_ATMODULE_UART) && defined(CONFIG_ATMODULE_BASE)
static uint8_t g_through_buffer[AT_THROUGH_MAX_LEN];
static int g_through_buffer_len;
static uint64_t g_through_buffer_deadline;

static int at_through_flush(void)
{
    int len = g_through_buffer_len;

    g_through_buffer_len = 0;
    return len > 0 ? at_net_client_send(0, g_through_buffer, len) : 0;
}
#endif

#ifdef CONFIG_ATMODULE_NETWORK
static int at_through_send(uint8_t *data, int32_t len)
{
    int linkid = 0;

    if (g_through_max_size > 0) {
        int unit_size = 0;
        int send_size = 0;
        int remaining = g_through_max_size - g_through_recv_size;
        len = (len > remaining) ? remaining : len;
        while (len - send_size > 0) {
            if (len - send_size >= g_through_transmit_size)
                unit_size = g_through_transmit_size;
            else
                unit_size = len - send_size;

            if (at_net_client_send(g_through_linkid, data + send_size, unit_size) < unit_size)
                break;
            else
                send_size += unit_size;
        }

        g_through_send_size += send_size;
        g_through_recv_size += len;

        if (send_size < len) {//send failed
            at_report_string("+CIPSENDL:%d,%d\r\n", g_through_send_size, g_through_recv_size);
        }
        else
        {
            if (g_through_send_size - g_through_report_tag >= g_through_report_size)
            {
                g_through_report_tag = (g_through_send_size/g_through_report_size)*g_through_report_size;
                at_report_string("+CIPSENDL:%d,%d\r\n", g_through_report_tag, g_through_recv_size);
            }
        }
        return send_size;
    }
    else {
#if defined(CONFIG_ATMODULE_UART) && defined(CONFIG_ATMODULE_BASE)
        if (at_base_config->trans_interval_ms > 0) {
            int received = len;

            while (len > 0) {
                int count = AT_THROUGH_MAX_LEN - g_through_buffer_len;

                if (g_through_buffer_len == 0) {
                    g_through_buffer_deadline = at_current_ms_get() + at_base_config->trans_interval_ms;
                }
                if (count > len) {
                    count = len;
                }
                memcpy(g_through_buffer + g_through_buffer_len, data, count);
                g_through_buffer_len += count;
                data += count;
                len -= count;
                if (g_through_buffer_len == AT_THROUGH_MAX_LEN) {
                    int ret = at_through_flush();

                    if (ret < 0) {
                        return ret;
                    }
                }
            }
            return received;
        }
#endif
        return at_net_client_send(linkid, data, len);
    }
}
#endif

void at_through_escape_reset(void)
{
    g_through_escape_count = 0;
    g_through_last_input_ms = at_current_ms_get();
#if defined(CONFIG_ATMODULE_NETWORK) && defined(CONFIG_ATMODULE_UART) && defined(CONFIG_ATMODULE_BASE)
    g_through_buffer_len = 0;
#endif
}

uint32_t at_through_read_timeout(void)
{
    uint64_t now = at_current_ms_get();
    uint32_t wait_ms = portMAX_DELAY;

    if (at_get_work_mode() == AT_WORK_MODE_CMD) {
        return portMAX_DELAY;
    }
    if (g_through_escape_count > 0) {
        uint64_t idle_ms = now - g_through_last_input_ms;

        wait_ms = idle_ms < AT_THROUGH_GUARD_MS ? AT_THROUGH_GUARD_MS - idle_ms : 1;
    }
#if defined(CONFIG_ATMODULE_NETWORK) && defined(CONFIG_ATMODULE_UART) && defined(CONFIG_ATMODULE_BASE)
    if (g_through_buffer_len > 0) {
        uint32_t buffer_wait = now < g_through_buffer_deadline ? g_through_buffer_deadline - now : 1;

        if (buffer_wait < wait_ms) {
            wait_ms = buffer_wait;
        }
    }
#endif
    if (wait_ms == portMAX_DELAY) {
        return portMAX_DELAY;
    }
    return (uint32_t)(((uint64_t)wait_ms * configTICK_RATE_HZ + 999) / 1000);
}

int at_through_input(uint8_t *data, int32_t len, int *remaining)
{
    *remaining = 0;
#ifdef CONFIG_ATMODULE_NETWORK
    uint64_t now = at_current_ms_get();
    uint64_t idle_ms = now - g_through_last_input_ms;
    int plus_count = 0;
    int ret;

#if defined(CONFIG_ATMODULE_UART) && defined(CONFIG_ATMODULE_BASE)
    if (g_through_buffer_len > 0 &&
        (now >= g_through_buffer_deadline ||
         (g_through_escape_count == 3 && idle_ms >= AT_THROUGH_GUARD_MS))) {
        ret = at_through_flush();
        if (ret < 0) {
            return ret;
        }
    }
#endif
    if (g_through_escape_count == 3 && idle_ms >= AT_THROUGH_GUARD_MS) {
        g_through_escape_count = 0;
        *remaining = len;
        if (g_through_max_size > 0) {
            g_through_linkid = -1;
            g_through_max_size = 0;
            g_through_report_size = 0;
            g_through_transmit_size = 0;
            g_through_send_size = 0;
            g_through_recv_size = 0;
            g_through_report_tag = 0;
            return -2;
        }
        else {
            return -1;
        }
    }

    while (plus_count < len && data[plus_count] == '+') {
        plus_count++;
    }
    if (g_through_escape_count > 0 &&
        (idle_ms > AT_THROUGH_GUARD_MS || plus_count != len ||
         g_through_escape_count + len > 3)) {
        int pending = g_through_escape_count;

        g_through_escape_count = 0;
        ret = at_through_send((uint8_t *)AT_THROUGH_EXIT_CMD, pending);
        if (ret < 0) {
            return ret;
        }
        if (g_through_max_size > 0 && g_through_recv_size == g_through_max_size) {
            return 0;
        }
    }
    if (len == 0) {
        return 0;
    }

    g_through_last_input_ms = now;
    if ((g_through_max_size == 0 || g_through_max_size - g_through_recv_size >= 3) &&
        (g_through_escape_count > 0 || idle_ms >= AT_THROUGH_GUARD_MS) &&
        plus_count == len && g_through_escape_count + len <= 3) {
        g_through_escape_count += len;
        return len;
    }
    return at_through_send(data, len);
#else
    return 0;
#endif
}

int at_through_max_length(void)
{
    if (g_through_max_size > 0)
    {
        int unit_size = g_through_max_size - g_through_recv_size - g_through_escape_count;
        /* Reserve a read until the trailing guard expires. */
        if (unit_size == 0 && g_through_escape_count > 0)
            return 1;
        if (unit_size > AT_THROUGH_MAX_LEN)
            unit_size = AT_THROUGH_MAX_LEN;
        if (unit_size <= 0) { //end
            if (g_through_send_size < g_through_recv_size)
                unit_size = -1;
        }
        return unit_size;
    }
    else
        return AT_THROUGH_MAX_LEN;
}

int at_through_set_length(int linkid, int max_size, int report_size, int transmit_size)
{
    g_through_linkid = linkid;
    g_through_max_size = max_size;
    g_through_report_size = report_size;
    g_through_transmit_size = transmit_size;
    g_through_send_size = 0;
    g_through_recv_size = 0;
    g_through_report_tag = 0;
    return 0;
}

