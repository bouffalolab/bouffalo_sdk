#ifndef __HTTPS_H__
#define __HTTPS_H__

#include <stdint.h>

/**
 * @brief Create an encrypted TCP connection
 *
 * @par Description:
 * Create an encrypted TCP connection in non-blocking mode
 *
 * @param dst      [IN] Receiver's IP address.
 * @param port     [IN] Receiver's port number.
 *
 * @retval BL_TCP_ARG_INVALID           dst is NULL.
 * @retval BL_TCP_CREATE_CONNECT_ERR    Failed to create connection.
 * @retval Greater than 0 Connection successful, returns the TCP socket. Then use bl_TcpSslState to check if the connection is fully established.
 */
int32_t bl_TcpSslConnect(const char* dst, uint16_t port);

/**
 * @brief Encrypted TCP connection status
 *
 * @par Description:
 * Query the status of the encrypted TCP connection
 *
 * @param fd      [IN] TCP socket.
 *
 * @retval bl_TcpErrorCode   TCP error code.
 * @see bl_TcpErrorCode
 */
int32_t bl_TcpSslState(int32_t fd);

/**
 * @brief Disconnect the encrypted TCP connection.
 *
 * @par Description:
 * Disconnect the encrypted TCP connection.
 *
 * @param fd   [IN] The socket created by bl_TcpSslConnect.
 *
 * @retval None.
 */
void bl_TcpSslDisconnect(int32_t fd);

/**
 * @brief Send encrypted TCP data
 *
 * @par Description:
 * Send encrypted TCP data in non-blocking mode
 *
 * @param fd      [IN]  TCP socket.
 * @param buf     [IN] Pointer to the buffer of data to be sent.
 * @param len     [IN] Length of the data to be sent, ranging from [0, 512).
 *
 * @retval bl_TcpErrorCode  TCP error code.
 * @see bl_TcpErrorCode
 */
int32_t bl_TcpSslSend(int32_t fd, const uint8_t* buf, uint16_t len);

/**
 * @brief Read encrypted TCP data
 *
 * @par Description:
 * Read encrypted TCP data in non-blocking mode
 *
 * @param fd      [IN] TCP socket.
 * @param buf     [IN] Pointer to the buffer for storing received data.
 * @param len     [IN] Maximum length of the buffer for storing received data, ranging from [0, 512).
 *
 * @retval bl_TcpErrorCode  TCP error code.
 * @see bl_TcpErrorCode
 */
int32_t bl_TcpSslRead(int32_t fd, uint8_t* buf, uint16_t len);

#endif
