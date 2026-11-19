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
#include <string.h>
#include "at_through.h"

#if defined(CONFIG_ATMODULE_BASE) && CONFIG_ATMODULE_BASE
#include "at_base_config.h"
#endif

#include <at_main.h>
#include "at_core.h"
#include "bflb_gpio.h"

#include <netbus_uart.h>

#define AT_PORT_PRINTF printf

static netbus_uart_config_t uart_config = {
    .name = "uart1",
    .speed = 2000000,
    .databits = 8,
    .stopbits = 1,
    .parity = 0,
    .flow_control =0,
};
static netbus_uart_ctx_t at_uart;

int at_port_uart_config_set(int port, int baudrate, int tx_pin, int rx_pin)
{
    static char uart_name[] = "uart1";
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    if (port < 0 || port > 1 || baudrate <= 0 ||
        tx_pin < -1 || tx_pin >= GPIO_PIN_MAX || rx_pin < -1 || rx_pin >= GPIO_PIN_MAX) {
        return -1;
    }
    uart_name[4] = '0' + port;
    uart_config.name = uart_name;
    uart_config.speed = baudrate;
    if (tx_pin >= 0) {
        bflb_gpio_uart_init(gpio, tx_pin, port * 4 + GPIO_UART_FUNC_UART0_TX);
    }
    if (rx_pin >= 0) {
        bflb_gpio_uart_init(gpio, rx_pin, port * 4 + GPIO_UART_FUNC_UART0_RX);
    }
    return 0;
}

int at_port_init(void)
{
    netbus_uart_init(&at_uart, &uart_config, 1024, 1024);
    return 1;
}

int at_port_deinit(void)
{
    return 0;
}

int at_port_read_data(uint8_t*data, int len)
{
    if (!data || len <= 0) {
        AT_PORT_PRINTF("Invalid arguments to at_port_read_data\r\n");
        return 0;
    }
    int nBytes = 0;

    nBytes = netbus_uart_receive(&at_uart, data, len, at_through_read_timeout());
    if (nBytes <= 0) {
        return 0;
    }

    return nBytes;
}

#define AT_PORT_WRITE_TIMEOUT      (10000)// 10s
int at_port_write_data(uint8_t *data, int len)
{
    if (!data || len <= 0) {
        AT_PORT_PRINTF("Invalid arguments to at_port_write_data\r\n");
        return 0;
    }
    int msg_len;

    if (at->fakeoutput) {
        for (int i = 0; i < len; i++) {
            printf("%c", data[i]);
        }
        return len;
    }
    msg_len = netbus_uart_send(&at_uart, data, len, AT_PORT_WRITE_TIMEOUT);
    if (msg_len > 0) {
        return msg_len;
    }
    return 0;
}

static int at_port_uart_reconfigure(int baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity, uint8_t flow_control, int send_reply)
{
    netbus_uart_config_t next;
    size_t offset = 0;
    size_t remaining = 0;
    const uint8_t *reply = NULL;

    if (netbus_uart_get_config(&at_uart, &next) != 0) {
        return -1;
    }

    next.speed = (uint32_t)baudrate;
    next.databits = databits;
    next.stopbits = stopbits;
    next.parity = parity;
    next.flow_control = flow_control;

    if (send_reply) {
        reply = (const uint8_t *)AT_CMD_MSG_OK;
        remaining = strlen(AT_CMD_MSG_OK);

#if defined(CONFIG_ATMODULE_BASE) && CONFIG_ATMODULE_BASE
        /* Preserve the existing result-message filtering semantics. */
        at_base_sysmsgfilter_apply(reply, remaining, &offset, &remaining);
#endif
    }

    return netbus_uart_reconfigure(&at_uart, &next, reply ? reply + offset : NULL, (uint32_t)remaining);
}

int at_port_para_set(int baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity, uint8_t flow_control)
{
    return at_port_uart_reconfigure(baudrate, databits, stopbits, parity, flow_control, 0);
}

int at_port_uart_config_validate(int baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity, uint8_t flow_control)
{
    netbus_uart_config_t next;

    if (netbus_uart_get_config(&at_uart, &next) != 0) {
        return -1;
    }

    next.speed = (uint32_t)baudrate;
    next.databits = databits;
    next.stopbits = stopbits;
    next.parity = parity;
    next.flow_control = flow_control;

    return netbus_uart_config_validate(&next);
}

int at_port_uart_cur_set(int baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity, uint8_t flow_control)
{
    return at_port_uart_reconfigure(baudrate, databits, stopbits, parity, flow_control, 1);
}

int at_port_para_get(int *baudrate, uint8_t *databits, uint8_t *stopbits, uint8_t *parity, uint8_t *flow_control)
{
    netbus_uart_config_t current;

    if (netbus_uart_get_config(&at_uart, &current) != 0) {
        return -1;
    }

    *baudrate = (int)current.speed;
    *databits = current.databits;
    *stopbits = current.stopbits;
    *parity = current.parity;
    *flow_control = current.flow_control;

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

int at_port_schedule_work(void)
{
    return netbus_uart_wakeup(&at_uart);
}

static const at_device_ops spidev_ops = {
    .init_device = at_port_init,
    .deinit_device = at_port_deinit,
    .read_data = at_port_read_data,
    .write_data = at_port_write_data,
    .read_zero_copy = NULL,
    .read_buffer_release = NULL,
    .schedule_work = at_port_schedule_work,
};

const at_device_ops *at_devops_get(void)
{
    return &spidev_ops;
}
