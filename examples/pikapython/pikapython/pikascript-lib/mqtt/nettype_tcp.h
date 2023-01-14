/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 13:39:00
 * @LastEditTime: 2020-10-17 14:17:10
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _NETTYPE_TCP_H_
#define _NETTYPE_TCP_H_

#include "network.h"
#include "mqtt_error.h"

#ifdef __cplusplus
extern "C" {
#endif

int nettype_tcp_read(network_t *n, unsigned char *buf, int len, int timeout);
int nettype_tcp_write(network_t *n, unsigned char *buf, int len, int timeout);
int nettype_tcp_connect(network_t* n);
void nettype_tcp_disconnect(network_t* n);

#ifdef __cplusplus
}
#endif

#endif
