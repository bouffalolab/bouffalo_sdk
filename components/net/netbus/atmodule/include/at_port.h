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

int at_port_init(void);

int at_port_deinit(void);

int at_port_read_data(uint8_t*data, int len);

int at_port_write_data(uint8_t *data, int len);

int at_port_para_set(int baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity, uint8_t flow_control);

int at_port_para_get(int *baudrate, uint8_t *databits, uint8_t *stopbits, uint8_t *parity, uint8_t *flow_control);

int at_port_netmode_set(int mode);

int at_port_netmode_get();

#ifdef __cplusplus
}
#endif

#endif/* AT_PORT_H */

