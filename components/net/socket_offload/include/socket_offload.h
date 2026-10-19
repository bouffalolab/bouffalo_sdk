#ifndef SOCKET_OFFLOAD_H
#define SOCKET_OFFLOAD_H

#include <lwip/sockets.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Socket RPC interface for client and service roles. Initialize the RPMsg
 * device before the role-specific socket layer on each peer. Socket operations
 * require task context and compatible RPC protocol versions on both peers.
 */

/**
 * @brief Initialize the socket RPC client on the SDK RPMsg device
 *
 * @return 0 on success, negative errno on failure
 */
int rpmsg_and_rpc_init(void);

/**
 * @brief Initialize the socket RPC service on the SDK RPMsg device
 *
 * @return 0 on success, negative errno on failure
 */
int rpc_service_init(void);

/**
 * @brief Initialize socket offload after rpmsg_and_rpc_init()
 *
 * @return 0 on success, negative errno on failure
 */
int socket_offload_init(void);

/**
 * @brief Create a socket via RPC to the socket service
 *
 * @param domain Address family, AF_INET only
 * @param type Socket type, SOCK_STREAM or SOCK_DGRAM
 * @param protocol Protocol, IPPROTO_TCP, IPPROTO_UDP or 0
 * @return Socket descriptor on success, -1 with errno on failure
 */
int socket_offload_socket(int domain, int type, int protocol);

/**
 * @brief Close a socket via RPC to the socket service
 *
 * @param fd Socket descriptor
 * @return 0 on success, -1 with errno on failure
 */
int socket_offload_close(int fd);

/**
 * @brief Bind a socket to a local IPv4 address via RPC
 *
 * @param fd Socket descriptor
 * @param addr sockaddr_in, network byte order
 * @param len Size of addr, sizeof(struct sockaddr_in)
 * @return 0 on success, -1 with errno on failure
 */
int socket_offload_bind(int fd, const struct sockaddr *addr, socklen_t len);

/**
 * @brief Connect a socket via RPC
 *
 * Blocking behavior follows the socket's configured mode.
 *
 * @param fd Socket descriptor
 * @param addr sockaddr_in of the peer, network byte order
 * @param len Size of addr
 * @return 0 on success, -1 with errno on failure
 */
int socket_offload_connect(int fd, const struct sockaddr *addr, socklen_t len);

/**
 * @brief Configure an offloaded socket
 *
 * @param fd Socket descriptor
 * @param cmd FIONBIO
 * @param argp Pointer to an unsigned long nonblocking mode value
 * @return 0 on success, -1 with errno on failure
 */
int socket_offload_ioctl(int fd, long cmd, void *argp);

/**
 * @brief Wait for readiness of offloaded sockets
 *
 * @param maxfdp1 Highest descriptor in the sets plus one
 * @param readset Descriptors to check for readability (input/output)
 * @param writeset Descriptors to check for writability (input/output)
 * @param exceptset Descriptors to check for exceptional conditions (input/output)
 * @param timeout Maximum wait time, NULL to wait indefinitely
 * @return Number of ready bits across all sets, 0 on timeout, -1 with errno on failure
 */
int socket_offload_select(int maxfdp1, fd_set *readset, fd_set *writeset,
                          fd_set *exceptset, struct timeval *timeout);

/**
 * @brief Mark a socket as passive via RPC
 *
 * @param fd Socket descriptor
 * @param backlog Pending connection queue length
 * @return 0 on success, -1 with errno on failure
 */
int socket_offload_listen(int fd, int backlog);

/**
 * @brief Accept a connection via RPC
 *
 * Blocking behavior follows the socket's configured mode.
 *
 * @param fd Listening socket descriptor
 * @param addr Optional peer address buffer, not populated by this protocol
 * @param len addr size in/out, set to zero when addr and len are provided
 * @return New socket descriptor on success, -1 with errno on failure
 */
int socket_offload_accept(int fd, struct sockaddr *addr, socklen_t *len);

/**
 * @brief Send data on a connected socket via RPC
 *
 * Requests larger than one RPC frame are fragmented.
 * Returns the underlying socket's accepted byte count without IPC clipping.
 *
 * @param fd Connected socket descriptor
 * @param buf Data to send
 * @param len Data length
 * @param flags MSG_DONTWAIT
 * @return Bytes sent on success, -1 with errno on failure
 */
ssize_t socket_offload_send(int fd, const void *buf, size_t len, int flags);

/**
 * @brief Send a UDP datagram or TCP data to an address via RPC
 *
 * Requests larger than one RPC frame are fragmented.
 *
 * @param fd Socket descriptor
 * @param buf Data to send
 * @param len Data length
 * @param flags MSG_DONTWAIT
 * @param addr sockaddr_in destination, NULL for connected sockets
 * @param addrlen Size of addr
 * @return Bytes sent on success, -1 with errno on failure
 */
ssize_t socket_offload_sendto(int fd, const void *buf, size_t len, int flags,
                            const struct sockaddr *addr, socklen_t addrlen);

/**
 * @brief Receive data from a socket via RPC
 *
 * @param fd Socket descriptor
 * @param buf Receive buffer
 * @param len Buffer size
 * @param flags MSG_DONTWAIT, MSG_PEEK
 * @return Bytes received, 0 on peer close or an empty UDP datagram, -1 with errno on failure
 */
ssize_t socket_offload_recv(int fd, void *buf, size_t len, int flags);

/**
 * @brief Receive a datagram or TCP data with source address via RPC
 *
 * @param fd Socket descriptor
 * @param buf Receive buffer
 * @param len Buffer size
 * @param flags MSG_DONTWAIT, MSG_PEEK
 * @param addr Source address, may be NULL
 * @param addrlen addr size in/out, may be NULL only with addr NULL
 * @return Bytes received, 0 on peer close or an empty UDP datagram, -1 with errno on failure
 */
ssize_t socket_offload_recvfrom(int fd, void *buf, size_t len, int flags,
                              struct sockaddr *addr, socklen_t *addrlen);

/**
 * @brief Set a socket option via RPC
 *
 * @param fd Socket descriptor
 * @param level Option level, SOL_SOCKET, IPPROTO_TCP or IPPROTO_IP
 * @param opt Option name
 * @param value Option value
 * @param len Option value length
 * @return 0 on success, -1 with errno on failure
 */
int socket_offload_setsockopt(int fd, int level, int opt, const void *value, socklen_t len);

/**
 * @brief Get a socket option via RPC
 *
 * @param fd Socket descriptor
 * @param level Option level
 * @param opt Option name
 * @param value Option value buffer
 * @param len Value length in/out
 * @return 0 on success, -1 with errno on failure
 */
int socket_offload_getsockopt(int fd, int level, int opt, void *value, socklen_t *len);

/**
 * @brief Unsupported local-address query, retained for API compatibility
 *
 * @param fd Socket descriptor
 * @param addr Local address
 * @param len addr size in/out
 * @return -1 with ENOTSUP for valid arguments
 */
int socket_offload_getsockname(int fd, struct sockaddr *addr, socklen_t *len);

/**
 * @brief Unsupported peer-address query, retained for API compatibility
 *
 * @param fd Connected socket descriptor
 * @param addr Peer address
 * @param len addr size in/out
 * @return -1 with ENOTSUP for valid arguments
 */
int socket_offload_getpeername(int fd, struct sockaddr *addr, socklen_t *len);

/**
 * @brief Shut down part of a full-duplex connection via RPC
 *
 * @param fd Socket descriptor
 * @param how SHUT_RD, SHUT_WR or SHUT_RDWR
 * @return 0 on success, -1 with errno on failure
 */
int socket_offload_shutdown(int fd, int how);

#ifdef __cplusplus
}
#endif

#ifdef CONFIG_SOCKET_OFFLOAD_CLIENT
#undef socket
#define socket socket_offload_socket
#undef closesocket
#define closesocket socket_offload_close
#undef close
#define close socket_offload_close
#undef bind
#define bind socket_offload_bind
#undef connect
#define connect socket_offload_connect
#undef ioctl
#define ioctl socket_offload_ioctl
#undef ioctlsocket
#define ioctlsocket socket_offload_ioctl
#undef select
#define select socket_offload_select
#undef listen
#define listen socket_offload_listen
#undef accept
#define accept socket_offload_accept
#undef send
#define send socket_offload_send
#undef sendto
#define sendto socket_offload_sendto
#undef recv
#define recv socket_offload_recv
#undef recvfrom
#define recvfrom socket_offload_recvfrom
#undef setsockopt
#define setsockopt socket_offload_setsockopt
#undef getsockopt
#define getsockopt socket_offload_getsockopt
#undef getsockname
#define getsockname socket_offload_getsockname
#undef getpeername
#define getpeername socket_offload_getpeername
#undef shutdown
#define shutdown socket_offload_shutdown
#endif

#endif /* SOCKET_OFFLOAD_H */
