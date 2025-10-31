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

#include <nxspi.h>

#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "bflb_gpio.h"
#ifdef NXSPI_NET
#include "nxspi_net.h"
#endif
#if 0
#include <vfs.h>
#include <device/vfs_uart.h>
#include <aos/kernel.h>
#include <hosal_uart.h>
#endif

//#include <spisync.h>

#define AT_PORT_DEVICE "/dev/ttyS1"
#define AT_PORT_PRINTF printf

static int at_serial_fd = -1;
static int at_serial_baudrate = 115200;
static uint8_t at_serial_databits = 8;
static uint8_t at_serial_stopbits = 1;
static uint8_t at_serial_parity = 0;
static uint8_t at_serial_flow_control = 0;

//extern spisync_t *at_spisync;
struct bflb_device_s *gpio;
int at_port_init(void)
{
#if 0
    
    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);

    return 1;
#endif
}

int at_port_deinit(void)
{
#if 0
    if (at_serial_fd < 0) {
        AT_PORT_PRINTF("uart is not open!\r\n");
        return -1;
    }

    aos_close(at_serial_fd);
    at_serial_fd = -1;
#endif
    return 0;
}

int at_port_read_data(uint8_t*data, int len)
{
#if 0
    spisync_msg_t msg;
    int nBytes = 0;

    SPISYNC_MSGINIT(&msg,
                    SPISYNC_TYPESTREAM_AT,  /* type */
                    0,                      /* copy */
                    portMAX_DELAY,          /* timeout */
                    data, len,              /* buf buf_len */
                    NULL,                   /* cb */
                    NULL);                  /* cb_arg */
    nBytes = spisync_read(at_spisync, &msg, 0);
#if 0
    if (nBytes) {
        printf("[AT_READ]:%d return %d-->", len, nBytes);
        for (int i = 0; i < nBytes; i++) {
            putchar(data[i]);
        }
        printf("\r\n");
    }
    
#endif
    if (nBytes <= 0) {
        return 0;
    }

    return nBytes;
#else
    int nBytes = 0;
#if 1
    nBytes = nxspi_read(NXSPI_TYPE_AT, data, len, portMAX_DELAY);
#else
    vTaskDelay(portMAX_DELAY);
#endif
#if 0
    if (nBytes) {
        printf("[AT_READ]:%d return %d-->", len, nBytes);
        for (int i = 0; i < nBytes; i++) {
            putchar(data[i]);
        }
        printf("\r\n");
    }
#endif
    return nBytes;
#endif
}

void at_port_debug_gpio_set(uint8_t val)
{
    bflb_gpio_reset(gpio, GPIO_PIN_0);
    bflb_gpio_set(gpio, GPIO_PIN_0);
}

#define AT_PORT_WRITE_TIMEOUT      (30000)// 30s
int at_port_write_data(uint8_t *data, int len)
{
    uint32_t remain_len = len;
    uint32_t write_len;
    
    if (at->fakeoutput) {
        for (int i = 0; i < len; i++) {
            putchar(data[i]);
        }
        return len;
    }
    if (!data) {
        return 0;
    }
    do {
        write_len = remain_len > NXBD_MTU ? NXBD_MTU : remain_len;
        nxspi_write(NXSPI_TYPE_AT, data + len - remain_len, write_len, portMAX_DELAY);
        remain_len -= write_len;
    } while (remain_len > 0);
    return (len - remain_len);
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
#ifdef NXSPI_NET
    extern spinet_t g_spinet;
    g_spinet.netstream = mode;
#endif 
    return 0;
}

int at_port_netmode_get()
{
#ifdef NXSPI_NET
    extern spinet_t g_spinet;
    return g_spinet.netstream;
#else 
    return 1;
#endif
}

