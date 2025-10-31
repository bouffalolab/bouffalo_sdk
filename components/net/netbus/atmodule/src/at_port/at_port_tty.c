/**
  ******************************************************************************
  * @file    at_port.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <at_main.h>

#include <netbus_tty.h>

#define AT_PORT_PRINTF printf

extern netbus_tty_ctx_t at_tty;

int at_port_init(void)
{
    return 1;
}

int at_port_deinit(void)
{
    return 0;
}

int at_port_read_data(uint8_t*data, int len)
{
    int nBytes = 0;

    nBytes = netbus_tty_receive(&at_tty, data, len, portMAX_DELAY);
#if 1
    if (nBytes) {
        printf("<---at_port_read_data reaturn :%d, data: 0x%02X-%c\r\n", nBytes, data[0], data[0]);// fixme.
    }
#endif
    if (nBytes <= 0) {
        return 0;
    }

    return nBytes;
}

#define AT_PORT_WRITE_TIMEOUT      (10000)// 10s
int at_port_write_data(uint8_t *data, int len)
{
    int msg_len;

    if (at->fakeoutput) {
        for (int i = 0; i < len; i++) {
            printf("%c", data[i]);
        }
    }

    if (at->fakeoutput) {
        return len;
    }
    if (!data) {
    	return 0;
    }

    msg_len = netbus_tty_send(&at_tty, data, len, AT_PORT_WRITE_TIMEOUT);
    if (msg_len > 0) {
        return msg_len;
    }
    return 0;
}

int at_port_para_set(int baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity, uint8_t flow_control)
{
    return 0;
}

int at_port_para_get(int *baudrate, uint8_t *databits, uint8_t *stopbits, uint8_t *parity, uint8_t *flow_control)
{
    return 0;
}

int at_port_netmode_set(int mode)
{
    return 0;
}

int at_port_netmode_get()
{
    return 1;
}

