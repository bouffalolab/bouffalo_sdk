#ifndef IPC_SOCKET_H
#define IPC_SOCKET_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <lwip/def.h>
#include <lwip/sockets.h>
#include <openamp/rpmsg_rpc_client_server.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Socket RPC wire format and client API: service IDs, per-operation
 * request layouts, the response layout, and the client functions the adapter
 * calls. The OpenAMP RPC layer carries the service ID and status; the
 * response payload carries the socket result, errno and returned data.
 */

/* Control RPC response timeout, in milliseconds (>0). */
#ifndef RPC_TIMEOUT_MS
#define RPC_TIMEOUT_MS          5000
#endif

/* Blocking connect/accept/send/recv/close response timeout; 0 waits indefinitely. */
#ifndef RPC_SOCKET_TIMEOUT_MS
#define RPC_SOCKET_TIMEOUT_MS   0
#endif

/**
 * @brief Socket RPC service IDs
 * Shared by the RPC client and the socket service
 */
#define RPC_SOCKET_CREATE        1
#define RPC_SOCKET_CLOSE         2
#define RPC_SOCKET_BIND          3
#define RPC_SOCKET_CONNECT       4
#define RPC_SOCKET_LISTEN        5
#define RPC_SOCKET_ACCEPT        6
#define RPC_SOCKET_SEND          7
#define RPC_SOCKET_SENDTO        8
#define RPC_SOCKET_RECV          9
#define RPC_SOCKET_RECVFROM      10
#define RPC_SOCKET_SETSOCKOPT    11
#define RPC_SOCKET_GETSOCKOPT    12
#define RPC_SOCKET_GETSOCKNAME   13
#define RPC_SOCKET_GETPEERNAME   14
#define RPC_SOCKET_SHUTDOWN      15
#define RPC_SOCKET_IOCTL         16
#define RPC_SOCKET_POLL          17
#define RPC_SOCKET_REQUEST       18
#define RPC_REQUEST_DATA_LEN     (MAX_BUF_LEN - MAX_FUNC_ID_LEN - offsetof(struct rpc_request_fragment, data))
/* Available payload after transport and response metadata. */
#define RPC_MAX_BUF_LEN          (MAX_BUF_LEN - offsetof(struct rpmsg_rpc_answer, params) - offsetof(struct rpc_response, data))

/** @brief Socket RPC request fragment */
struct rpc_request_fragment {
    uint32_t sequence;
    uint32_t total_len;
    uint32_t offset;
    uint32_t rpc_id;
    uint8_t data[];
};

/** @brief Socket RPC response fragment */
struct rpc_response {
    uint32_t sequence;
    int result;
    int errno_val;
    uint32_t data_len;
    uint8_t data[];
};

/**
 * @brief Socket create parameters
 */
struct rpc_socket_create {
    int family;
    int type;
    int proto;
};

/**
 * @brief Socket bind parameters
 */
struct rpc_socket_bind {
    int sockfd;
    socklen_t addrlen;
    struct sockaddr addr;    /* Socket address */
};

/**
 * @brief Socket connect parameters
 */
struct rpc_socket_connect {
    int sockfd;
    socklen_t addrlen;
    struct sockaddr addr;    /* Socket address */
};

/** @brief Socket control request parameters */
struct rpc_socket_ioctl {
    int sockfd;
    long cmd;
    unsigned long value;
};

/** @brief Socket readiness request parameters */
struct rpc_socket_poll {
    int sockfd;
    short events;
};

/**
 * @brief Socket listen parameters
 */
struct rpc_socket_listen {
    int sockfd;
    int backlog;
};

/**
 * @brief Socket accept parameters
 */
struct rpc_socket_accept {
    int sockfd;
    socklen_t addrlen;
    /* Response result: new_sockfd, without peer address data. */
};

/**
 * @brief Socket send parameters
 */
struct rpc_socket_send {
    int sockfd;
    int flags;
    char data[];              /* Flexible array for data */
};

/**
 * @brief Socket sendto parameters
 */
struct rpc_socket_sendto {
    int sockfd;
    int flags;
    socklen_t addrlen;
    struct sockaddr addr;    /* Reserved */
    char data[];              /* Destination address followed by payload */
};

/**
 * @brief Socket recv parameters
 */
struct rpc_socket_recv {
    int sockfd;
    int flags;
    size_t len;
};

/**
 * @brief Socket recvfrom parameters
 */
struct rpc_socket_recvfrom {
    int sockfd;
    int flags;
    size_t len;
    socklen_t addrlen;
};

/**
 * @brief Socket setsockopt parameters
 */
struct rpc_socket_setsockopt {
    int sockfd;
    int level;
    int optname;
    socklen_t optlen;
    char data[];              /* Flexible array for option value */
};

/**
 * @brief Socket getsockopt parameters
 */
struct rpc_socket_getsockopt {
    int sockfd;
    int level;
    int optname;
    socklen_t optlen;
};

/**
 * @brief Socket getsockname parameters
 */
struct rpc_socket_getsockname {
    int sockfd;
    socklen_t addrlen;
};

/**
 * @brief Socket getpeername parameters
 */
struct rpc_socket_getpeername {
    int sockfd;
    socklen_t addrlen;
};

/**
 * @brief IPC socket initialization hook; RPC setup is performed separately
 *
 * @return 0 on success, negative errno on failure
 */
int ipc_socket_init(void);

/**
 * @brief Check if the RPC client is initialized
 *
 * @return true if initialized, false otherwise
 */
bool ipc_socket_is_initialized(void);

/** @brief Initialize socket RPC on the SDK's existing RPMsg device. */
int rpmsg_and_rpc_init(void);

/* Call shutdown only after socket callers and RPC callbacks are quiescent.
 * The shared RPMsg device is owned by the SDK, not this component.
 */
void rpmsg_and_rpc_shutdown(void);
bool rpmsg_is_initialized(void);
struct rpmsg_device *get_rpmsg_device(void);

/** @brief Compatibility hook; actual release is in rpmsg_and_rpc_shutdown(). */
void rpc_client_shutdown(void);

/** @brief Handle a socket RPC response. */
void rpc_callback(struct rpmsg_rpc_clt *rpc, int status, void *data, size_t len);

/**
 * @brief Create a socket via IPC to the socket service
 *
 * @param family Address family
 * @param type Socket type
 * @param proto Protocol
 * @return Socket descriptor on success, negative errno on failure
 */
int ipc_socket_create(int family, int type, int proto);

/**
 * @brief Close a socket via IPC to the socket service
 *
 * @param sockfd Socket descriptor
 * @return 0 on success, negative errno on failure
 */
int ipc_socket_close(int sockfd);

/**
 * @brief Bind a socket to an address via IPC
 *
 * @param sockfd Socket descriptor
 * @param addr Address to bind to
 * @param addrlen Length of address
 * @return 0 on success, negative errno on failure
 */
int ipc_socket_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/**
 * @brief Connect a socket to an address via IPC
 *
 * @param sockfd Socket descriptor
 * @param addr Address to connect to
 * @param addrlen Length of address
 * @return 0 on success, negative errno on failure
 */
int ipc_socket_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/**
 * @brief Configure a socket via IPC
 *
 * @param sockfd Socket descriptor
 * @param cmd Socket control command (FIONBIO)
 * @param value Command argument value
 * @return 0 on success, negative errno on failure
 */
int ipc_socket_ioctl(int sockfd, long cmd, unsigned long value);

/**
 * @brief Query socket readiness via IPC
 *
 * @param sockfd Socket descriptor
 * @param events Requested poll events
 * @param revents Returned poll events
 * @return Number of ready descriptors, negative errno on failure
 */
int ipc_socket_poll(int sockfd, short events, short *revents);

/**
 * @brief Listen for connections on a socket via IPC
 *
 * @param sockfd Socket descriptor
 * @param backlog Maximum pending connections
 * @return 0 on success, negative errno on failure
 */
int ipc_socket_listen(int sockfd, int backlog);

/**
 * @brief Accept a connection on a listening socket via IPC
 *
 * @param sockfd Listening socket descriptor
 * @param addr Optional peer address buffer, not populated by this protocol
 * @param addrlen Address length in/out, set to zero when addr and addrlen are provided
 * @return New socket descriptor on success, negative errno on failure
 */
int ipc_socket_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

/**
 * @brief Send data on a connected socket via IPC
 *
 * @param sockfd Socket descriptor
 * @param buf Data buffer
 * @param len Data length
 * @param flags Flags (MSG_*)
 * @return Bytes sent on success, negative errno on failure
 */
ssize_t ipc_socket_send(int sockfd, const void *buf, size_t len, int flags);

/** @brief Send data to a specific address via IPC. */
ssize_t ipc_socket_sendto(int sockfd, const void *buf, size_t len, int flags,
                          const struct sockaddr *dest_addr, socklen_t addrlen);

/** @brief Receive data from a socket via IPC. */
ssize_t ipc_socket_recv(int sockfd, void *buf, size_t len, int flags);

/**
 * @brief Receive data from a socket via IPC with source address
 *
 * @param sockfd Socket descriptor
 * @param buf Receive buffer
 * @param len Buffer size
 * @param flags Flags (MSG_*)
 * @param src_addr Source address (can be NULL)
 * @param addrlen Length of address (input/output)
 * @return Bytes received on success, negative errno on failure
 */
ssize_t ipc_socket_recvfrom(int sockfd, void *buf, size_t len, int flags,
                            struct sockaddr *src_addr, socklen_t *addrlen);

/**
 * @brief Set socket options via IPC
 *
 * @param sockfd Socket descriptor
 * @param level Option level
 * @param optname Option name
 * @param optval Option value
 * @param optlen Option value length
 * @return 0 on success, negative errno on failure
 */
int ipc_socket_setsockopt(int sockfd, int level, int optname,
                          const void *optval, socklen_t optlen);

/**
 * @brief Get socket options via IPC
 *
 * @param sockfd Socket descriptor
 * @param level Option level
 * @param optname Option name
 * @param optval Option value buffer
 * @param optlen Value length (input/output)
 * @return 0 on success, negative errno on failure
 */
int ipc_socket_getsockopt(int sockfd, int level, int optname,
                          void *optval, socklen_t *optlen);

/**
 * @brief Query the local socket address
 *
 * @param sockfd Socket descriptor
 * @param addr Local address
 * @param addrlen Address length (input/output)
 * @return -ENOTSUP
 */
int ipc_socket_getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

/**
 * @brief Query the peer socket address
 *
 * @param sockfd Connected socket descriptor
 * @param addr Peer address
 * @param addrlen Address length (input/output)
 * @return -ENOTSUP
 */
int ipc_socket_getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

/**
 * @brief Shut down part of a full-duplex connection via IPC
 *
 * @param sockfd Socket descriptor
 * @param how SHUT_RD, SHUT_WR or SHUT_RDWR
 * @return 0 on success, negative errno on failure
 */
int ipc_socket_shutdown(int sockfd, int how);

#ifdef __cplusplus
}
#endif

#endif /* IPC_SOCKET_H */
