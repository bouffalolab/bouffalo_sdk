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

#include <netbus_uart.h>

#define AT_PORT_PRINTF printf

static int at_serial_fd = -1;
static int at_serial_baudrate = 2000000;
static uint8_t at_serial_databits = 8;
static uint8_t at_serial_stopbits = 1;
static uint8_t at_serial_parity = 0;
static uint8_t at_serial_flow_control = 0;

extern netbus_uart_ctx_t at_uart;

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

    nBytes = netbus_uart_receive(&at_uart, data, len, portMAX_DELAY);
#if 0
    if (nBytes) {
        printf("<---at_port_read_data reaturn :%d\r\n", nBytes);
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

    msg_len = netbus_uart_send(&at_uart, data, len, AT_PORT_WRITE_TIMEOUT);
    if (msg_len > 0) {
        return msg_len;
    }
    return 0;
}

int at_port_para_set(int baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity, uint8_t flow_control)
{
#if 0
    uint32_t uart_baudrate;
    hosal_uart_data_width_t uart_databits;
    hosal_uart_stop_bits_t uart_stopbits;
    hosal_uart_parity_t uart_parity;
    hosal_uart_flow_control_t uart_flow_control;

    if (at_serial_fd < 0) {
        AT_PORT_PRINTF("uart is not open!\r\n");
        return -1;
    }

    uart_baudrate = (uint32_t)baudrate;
    if (databits == 5)
        uart_databits = HOSAL_DATA_WIDTH_5BIT;
    else if (databits == 6)
        uart_databits = HOSAL_DATA_WIDTH_6BIT;
    else if (databits == 7)
        uart_databits = HOSAL_DATA_WIDTH_7BIT;
    else if (databits == 8)
        uart_databits = HOSAL_DATA_WIDTH_8BIT;
    else
        return -1;
    if (stopbits == 1)
        uart_stopbits = HOSAL_STOP_BITS_1;
    else if (stopbits == 2)
        uart_stopbits = HOSAL_STOP_BITS_1_5;
    else  if (stopbits == 3)
        uart_stopbits = HOSAL_STOP_BITS_2;
    else
        return -1;
    if (parity == 0)
        uart_parity = HOSAL_NO_PARITY;
    else if (parity == 1)
        uart_parity = HOSAL_ODD_PARITY;
    else  if (parity == 2)
        uart_parity = HOSAL_EVEN_PARITY;
    else
        return -1;
    if (flow_control == 0)
        uart_flow_control = HOSAL_FLOW_CONTROL_DISABLED;
    else if (flow_control == 1)
        uart_flow_control = HOSAL_FLOW_CONTROL_RTS;
    else  if (flow_control == 2)
        uart_flow_control = HOSAL_FLOW_CONTROL_CTS;
    else  if (flow_control == 3)
        uart_flow_control = HOSAL_FLOW_CONTROL_CTS_RTS;
    else
        return -1;
    
    if (aos_ioctl(at_serial_fd, IOCTL_UART_IOC_BAUD_MODE, uart_baudrate) != 0) {
        AT_PORT_PRINTF("uart set baudrate failed!\r\n");
        return -1;
    }
    if (aos_ioctl(at_serial_fd, IOCTL_UART_IOC_DATABITS_SET, uart_databits) != 0) {
        AT_PORT_PRINTF("uart set databits failed!\r\n");
        return -1;
    }
    if (aos_ioctl(at_serial_fd, IOCTL_UART_IOC_STOPBITS_SET, uart_stopbits) != 0) {
        AT_PORT_PRINTF("uart set stopbits failed!\r\n");
        return -1;
    }
    if (aos_ioctl(at_serial_fd, IOCTL_UART_IOC_PARITY_SET, uart_parity) != 0) {
        AT_PORT_PRINTF("uart set parity failed!\r\n");
        return -1;
    }
    if (aos_ioctl(at_serial_fd, IOCTL_UART_IOC_HWFC_SET, uart_flow_control) != 0) {
        AT_PORT_PRINTF("uart set flow_control failed!\r\n");
        return -1;
    }

    at_serial_baudrate = baudrate;
    at_serial_databits = databits;
    at_serial_stopbits = stopbits;
    at_serial_parity = parity;
    at_serial_flow_control = flow_control;
#endif
    return 0;
}

int at_port_para_get(int *baudrate, uint8_t *databits, uint8_t *stopbits, uint8_t *parity, uint8_t *flow_control)
{
#if 0
    if (at_serial_fd < 0) {
        AT_PORT_PRINTF("uart is not open!\r\n");
        return -1;
    }

    *baudrate = at_serial_baudrate;
    *databits = at_serial_databits;
    *stopbits = at_serial_stopbits;
    *parity = at_serial_parity;
    *flow_control = at_serial_flow_control;
#endif
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

