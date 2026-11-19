/**
  ******************************************************************************
  * @file    at_port.h
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#ifndef AT_PORT_H
#define AT_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

const at_device_ops *at_devops_get(void);

int at_port_init(void);

/* TX/RX pin -1 leaves the pin unconfigured. */
int at_port_uart_config_set(int port, int baudrate, int tx_pin, int rx_pin);

void at_port_deep_sleep(uint32_t sleep_time_ms);

/* level: 0 = low, 1 = high. */
int at_port_sleep_wakeup_config(int gpio, int level);

int at_port_deinit(void);

int at_port_read_data(uint8_t*data, int len);

int at_port_write_data(uint8_t *data, int len);

int at_port_para_set(int baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity, uint8_t flow_control);

int at_port_para_get(int *baudrate, uint8_t *databits, uint8_t *stopbits, uint8_t *parity, uint8_t *flow_control);

#if defined(CONFIG_ATMODULE_UART) && CONFIG_ATMODULE_UART
int at_port_uart_cur_set(int baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity, uint8_t flow_control);
int at_port_uart_config_validate(int baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity, uint8_t flow_control);
#endif

int at_port_netmode_set(int mode);

int at_port_netmode_get();

#ifdef __cplusplus
}
#endif

#endif/* AT_PORT_H */

