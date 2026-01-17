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
    nxspi_init();
#if NXSPI_NET
    spinet_init();
#endif
    return 1;
}

int at_port_deinit(void)
{
    return 0;
}

int at_port_read_data(uint8_t*data, int len)
{
    int nBytes = 0;
    nBytes = nxspi_read(NXSPI_TYPE_AT, data, len, portMAX_DELAY);
    return nBytes;
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
    return 0;
}

int at_port_para_get(int *baudrate, uint8_t *databits, uint8_t *stopbits, uint8_t *parity, uint8_t *flow_control)
{
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

