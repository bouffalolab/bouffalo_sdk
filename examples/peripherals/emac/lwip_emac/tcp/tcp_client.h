/**
 * @file tcp_client.h
 * @brief Shell-controlled TCP loopback client.
 */

#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <stdint.h>

#include "lwip/ip4_addr.h"

#define TCP_CLIENT_DEFAULT_PORT 3365

/**
 * @brief Start the TCP loopback client.
 *
 * @param[in] address Remote TCP server IPv4 address.
 * @param[in] port Remote TCP server port.
 * @retval 0 The client task was created successfully.
 * @retval -1 The client is already running or its task could not be created.
 */
int tcp_client_start(const ip4_addr_t *address, uint16_t port);

/**
 * @brief Stop the TCP loopback client.
 *
 * @retval 0 The client stopped successfully.
 * @retval -1 The client is not running or did not stop in time.
 */
int tcp_client_stop(void);

#endif /* TCP_CLIENT_H */
