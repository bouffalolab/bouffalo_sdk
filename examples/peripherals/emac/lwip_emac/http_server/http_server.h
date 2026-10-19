/**
 * @file http_server.h
 * @brief Built-in lwIP HTTP server integration.
 */

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

/**
 * @brief Initialize the long-running built-in lwIP HTTP server.
 *
 * @retval 0 Initialization was queued in the TCP/IP thread.
 * @retval -1 Initialization could not be queued.
 */
int http_server_init(void);

#endif /* HTTP_SERVER_H */
