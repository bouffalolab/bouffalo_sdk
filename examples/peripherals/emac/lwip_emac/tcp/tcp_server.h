/**
 * @file tcp_server.h
 * @brief Shell-controlled TCP loopback server.
 */

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <stdint.h>

#define TCP_SERVER_DEFAULT_PORT 3365

/**
 * @brief Start the TCP loopback server on a local port.
 *
 * @param[in] port Local TCP port.
 * @retval 0 The server task was created successfully.
 * @retval -1 The server is already running or its task could not be created.
 */
int tcp_server_start(uint16_t port);

/**
 * @brief Stop the TCP loopback server.
 *
 * @retval 0 The server stopped successfully.
 * @retval -1 The server is not running or did not stop in time.
 */
int tcp_server_stop(void);

#endif /* TCP_SERVER_H */
