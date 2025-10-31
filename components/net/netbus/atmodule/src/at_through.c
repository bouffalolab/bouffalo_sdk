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

#include "at_main.h"
#include "at_core.h"
#include "at_through.h"
#include "at_net_main.h"

static int g_through_linkid = -1;
static int g_through_max_size = 0;
static int g_through_report_size = 0;
static int g_through_transmit_size = 0;
static int g_through_send_size = 0;
static int g_through_recv_size = 0;
static int g_through_report_tag = 0;

int at_through_input(uint8_t *data, int32_t len)
{
#if CONFIG_ATMODULE_NETWORK
    int linkid = 0;

    if (len == strlen(AT_THROUGH_EXIT_CMD) && memcmp(data, AT_THROUGH_EXIT_CMD, strlen(AT_THROUGH_EXIT_CMD)) == 0) {
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

    if (g_through_max_size > 0) {
        int unit_size = 0;
        int send_size = 0;
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
            at_response_string("+CIPSENDL:%d,%d\r\n", g_through_send_size, g_through_recv_size);
        }
        else
        {
            if (g_through_send_size - g_through_report_tag >= g_through_report_size)
            {
                g_through_report_tag = (g_through_send_size/g_through_report_size)*g_through_report_size;
                at_response_string("+CIPSENDL:%d,%d\r\n", g_through_report_tag, g_through_recv_size);
            }
        }
        return send_size;
    }
    else {
        return at_net_client_send(linkid, data, len);
    }
#else  
    return 0;
#endif
}

int at_through_max_length(void)
{
    if (g_through_max_size > 0)
    {
        int unit_size = g_through_max_size - g_through_recv_size;
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

