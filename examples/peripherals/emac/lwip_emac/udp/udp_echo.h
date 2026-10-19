/**
 * @file udp_echo.h
 * @brief UDP echo service for the unified lwIP EMAC example.
 */

#ifndef UDP_ECHO_H
#define UDP_ECHO_H

#include <stdint.h>

#define UDP_ECHO_DEFAULT_PORT 3365

/**
 * @brief Start the UDP echo service task on a local port.
 *
 * @param[in] port Local UDP port.
 * @retval 0 The service task was created successfully.
 * @retval -1 The service task could not be created or was already started.
 */
int udp_echo_start(uint16_t port);

/**
 * @brief Stop the UDP echo service task.
 *
 * @retval 0 The service stopped successfully.
 * @retval -1 The service was not running or did not stop in time.
 */
int udp_echo_stop(void);

#endif /* UDP_ECHO_H */
