#include "socket_rpc_service.h"
#include "ipc_socket.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>
#include <mm.h>
#include <rpmsg_service.h>
#include <openamp/rpmsg_rpc_client_server.h>
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOCKET_OFFLOAD_LOG_LEVEL
#include <log.h>

#ifdef CONFIG_SHELL
#include "shell.h"
#endif

extern struct rpmsg_device *rpmsg_service_get_device(void);

/* RPC service configuration */
#define RPC_SERVICE_NAME           "socket-rpc-service"
#define MAX_SOCKETS                64
#define SOCKET_FD_BASE             1000
#define RPC_BUFFER_SIZE            (RPC_MAX_BUF_LEN + sizeof(struct sockaddr_storage))
#define RPC_QUEUE_LEN              8

#if !LWIP_NETCONN_FULLDUPLEX || !LWIP_NETCONN_SEM_PER_THREAD
#error "Socket RPC tasks require LWIP_NETCONN_FULLDUPLEX and LWIP_NETCONN_SEM_PER_THREAD"
#endif

/* Socket tracking structure */
struct socket_entry {
    int fd;                          /* Client FD (returned to socket client) */
    int real_fd;                     /* Real socket FD on socket service */
    int family;
    int type;
    int protocol;
    bool is_used;
    bool is_connected;
    bool is_bound;
    bool receiving;
    unsigned references;
    struct sockaddr_storage local_addr;
    struct sockaddr_storage remote_addr;
    SemaphoreHandle_t lock;
};

struct rpc_work {
    struct rpmsg_rpc_svr rpc; /* First member: socket handlers receive this context. */
    struct rpc_work *next;
    TaskHandle_t task;
    StaticTask_t task_storage;
    StackType_t stack[512];
    struct socket_entry *entry; /* Pinned by the dispatcher until the response is sent. */
    uint32_t sequence;
    int result;
    int error;
    size_t data_len;
    bool busy;
    bool stopped;
    uint8_t buffer[RPC_BUFFER_SIZE] __attribute__((aligned(sizeof(uint32_t))));
};

enum service_event_type {
    SERVICE_REQUEST, SERVICE_RESPONSE, SERVICE_ALLOC, SERVICE_FREE, SERVICE_FIND,
    SERVICE_STOPPED, SERVICE_STOP, SERVICE_EXIT
};

struct service_event {
    enum service_event_type type;
    void *data;
    size_t len;
    struct rpc_work *work;
    struct socket_entry *entry;
    uint32_t rpc_id;
    int result;
    int error;
    int args[3];
    SemaphoreHandle_t done;
};

/* RPC server state */
struct rpc_server_state {
    struct rpmsg_rpc_svr rpc_server;
    struct socket_entry sockets[MAX_SOCKETS];
    QueueHandle_t queue;
    StaticQueue_t queue_storage;
    uint8_t queue_items[RPC_QUEUE_LEN * sizeof(struct service_event *)];
    TaskHandle_t task;
    StaticTask_t task_storage;
    StackType_t stack[512];
    struct rpc_work *workers;
    bool stopping;
    int next_fd;
    bool initialized;
};

static struct rpc_server_state g_rpc_server __attribute__((section(".wifi_ram")));

static void socket_service_task(void *arg);

static void service_event_call(struct service_event *event)
{
    StaticSemaphore_t storage;

    event->done = xSemaphoreCreateBinaryStatic(&storage);
    xQueueSend(g_rpc_server.queue, &event, portMAX_DELAY);
    xSemaphoreTake(event->done, portMAX_DELAY);
    vSemaphoreDelete(event->done);
}

static __attribute__((unused)) const char *errno_to_str(int err)
{
    switch (err) {
#ifdef EREMOTEIO
    case EREMOTEIO: return "EREMOTEIO (Remote I/O error)";
#endif
    case EAGAIN: return "EAGAIN (Resource temporarily unavailable)";
    case EINVAL: return "EINVAL (Invalid argument)";
    case EBADF: return "EBADF (Bad file descriptor)";
    case ECONNREFUSED: return "ECONNREFUSED (Connection refused)";
    case EHOSTUNREACH: return "EHOSTUNREACH (No route to host)";
    default: return "Unknown error";
    }
}

/**
 * @brief Allocate a socket file descriptor
 */
static int alloc_socket_fd(int family, int type, int protocol)
{
    int i, fd;

    if (xTaskGetCurrentTaskHandle() != g_rpc_server.task) {
        struct service_event event = { .type = SERVICE_ALLOC, .args = {family, type, protocol} };
        service_event_call(&event);
        return event.result;
    }

    for (i = 0; i < MAX_SOCKETS; i++) {
        if (!g_rpc_server.sockets[i].is_used && !g_rpc_server.sockets[i].references) {
            fd = g_rpc_server.next_fd++;

            g_rpc_server.sockets[i].fd = fd;
            g_rpc_server.sockets[i].real_fd = -1;  /* Will be set when real socket is created */
            g_rpc_server.sockets[i].family = family;
            g_rpc_server.sockets[i].type = type;
            g_rpc_server.sockets[i].protocol = protocol;
            g_rpc_server.sockets[i].is_used = true;
            g_rpc_server.sockets[i].is_connected = false;
            g_rpc_server.sockets[i].is_bound = false;

            /* Per-socket locks persist across descriptor reuse. */
            if (!g_rpc_server.sockets[i].lock) {
                g_rpc_server.sockets[i].lock = xSemaphoreCreateRecursiveMutex();
                if (!g_rpc_server.sockets[i].lock) {
                    g_rpc_server.sockets[i].is_used = false;
                    return -ENOMEM;
                }
            }

            return fd;
        }
    }

    return -EMFILE;
}

/**
 * @brief Free a socket file descriptor
 */
static int free_socket_fd(int fd)
{
    int i;

    if (xTaskGetCurrentTaskHandle() != g_rpc_server.task) {
        struct service_event event = { .type = SERVICE_FREE, .args = {fd} };
        service_event_call(&event);
        return event.result;
    }

    for (i = 0; i < MAX_SOCKETS; i++) {
        if (g_rpc_server.sockets[i].fd == fd && g_rpc_server.sockets[i].is_used) {
            g_rpc_server.sockets[i].is_used = false;
            g_rpc_server.sockets[i].is_connected = false;
            g_rpc_server.sockets[i].is_bound = false;

            return 0;
        }
    }

    return -EBADF;
}

/**
 * @brief Find socket entry by file descriptor
 */
static struct socket_entry *find_socket_entry(int fd)
{
    int i;

    if (xTaskGetCurrentTaskHandle() != g_rpc_server.task) {
        struct service_event event = { .type = SERVICE_FIND, .args = {fd} };
        service_event_call(&event);
        return event.entry;
    }

    for (i = 0; i < MAX_SOCKETS; i++) {
        if (g_rpc_server.sockets[i].fd == fd && g_rpc_server.sockets[i].is_used) {
            struct socket_entry *entry = &g_rpc_server.sockets[i];
            return entry;
        }
    }

    return NULL;
}

/**
 * @brief Get real socket FD from client FD
 */
static int __attribute__((unused)) get_real_socket_fd(int client_fd)
{
    struct socket_entry *entry = find_socket_entry(client_fd);
    if (!entry) {
        return -EBADF;
    }
    return entry->real_fd;
}

/**
 * @brief Update real socket FD mapping
 */
static int update_real_socket_fd(int client_fd, int real_fd)
{
    struct socket_entry *entry = find_socket_entry(client_fd);
    if (!entry) {
        return -EBADF;
    }

    xSemaphoreTakeRecursive(entry->lock, portMAX_DELAY);
    entry->real_fd = real_fd;
    xSemaphoreGiveRecursive(entry->lock);

    LOG_RD("[DBG] NP: Mapped client_fd=%d to real_fd=%d\r\n", client_fd, real_fd);
    return 0;
}

/* Executed only by socket_service_task, which owns the response frame. */
static int transmit_rpc_response(uint32_t sequence, uint32_t rpc_id, int result,
                                  int error, const void *data, size_t data_len)
{
    static struct rpmsg_rpc_answer frame __attribute__((aligned(sizeof(uint32_t))));
    struct rpc_response *response = (struct rpc_response *)frame.params;
    size_t offset = 0;

    do {
        size_t fragment_len = LWIP_MIN(data_len - offset, RPC_MAX_BUF_LEN);
        size_t frame_len = offsetof(struct rpmsg_rpc_answer, params) +
                           offsetof(struct rpc_response, data) + fragment_len;
        int ret;

        frame.id = rpc_id;
        frame.status = RPMSG_RPC_OK;
        response->sequence = sequence;
        response->result = result;
        response->errno_val = error;
        response->data_len = data_len;
        if (fragment_len) { memcpy(response->data, (const uint8_t *)data + offset, fragment_len); }
        ret = rpmsg_trysend(&g_rpc_server.rpc_server.ept, &frame, frame_len);
        if (ret != (int)frame_len) {
            LOG_RE("[ERR] NP: Failed to send RPC response (rpc_id=%u, ret=%d)\r\n", (unsigned)rpc_id, ret);
            if (ret == RPMSG_ERR_NO_BUFF) { return -ENOBUFS; }
            return ret < 0 ? ret : -EPROTO;
        }
        offset += fragment_len;
    } while (offset < data_len);
    return 0;
}

static int send_rpc_response(struct rpmsg_rpc_svr *rpcs, uint32_t rpc_id,
                             int result, int error, const void *data, size_t data_len)
{
    struct rpc_work *work = (struct rpc_work *)rpcs;

    (void)rpc_id;
    work->result = result;
    work->error = error;
    work->data_len = data_len;
    /* The handler's stack can be released before the service task transmits. */
    if (data_len && data != work->buffer) { memcpy(work->buffer, data, data_len); }
    return 0;
}

/**
 * @brief Handle socket create RPC
 */
static int handle_socket_create(void *data, struct rpmsg_rpc_svr *rpcs)
{
    /* Data format: [4 bytes RPC ID] + [struct rpc_socket_create] */
    uint32_t rpc_id = *(uint32_t *)data;
    struct rpc_socket_create *req = (struct rpc_socket_create *)((uint8_t *)data + MAX_FUNC_ID_LEN);
    int real_fd, client_fd;

    /* Extract socket parameters */
    int family = req->family;
    int type = req->type;
    int proto = req->proto;

    client_fd = alloc_socket_fd(family, type, proto);
    if (client_fd < 0) {
        LOG_RE("[ERR] NP: Failed to allocate socket FD\r\n");
        LOG_RE("[NP_RPC] Cleaned up real_fd=%d due to client_fd allocation failure\r\n", -1);
        return send_rpc_response(rpcs, rpc_id, -1, -client_fd, NULL, 0);
    }

    /* Get socket entry for this client_fd */
    struct socket_entry *entry = find_socket_entry(client_fd);
    if (!entry) {
        LOG_RE("[ERR] NP: Failed to find socket entry for client_fd=%d\r\n", client_fd);
        return send_rpc_response(rpcs, rpc_id, -1, EBADF, NULL, 0);
    }

    /* Create real socket on socket service using lwIP socket API */
    real_fd = lwip_socket(family, type, proto);
    if (real_fd < 0) {
        int error = errno;
        LOG_RE("[ERR] NP: Failed to create real socket: errno=%d\r\n", error);
        /* Clean up the allocated client FD */
        xSemaphoreTakeRecursive(entry->lock, portMAX_DELAY);
        free_socket_fd(client_fd);
        xSemaphoreGiveRecursive(entry->lock);
        return send_rpc_response(rpcs, rpc_id, -1, error, NULL, 0);
    }

    /* Map client_fd to real_fd */
    int map_ret = update_real_socket_fd(client_fd, real_fd);
    if (map_ret < 0) {
        LOG_RE("[ERR] NP: Failed to map FDs: ret=%d\r\n", map_ret);
        lwip_close(real_fd);
        xSemaphoreTakeRecursive(entry->lock, portMAX_DELAY);
        free_socket_fd(client_fd);
        xSemaphoreGiveRecursive(entry->lock);
        return send_rpc_response(rpcs, rpc_id, -1, -map_ret, NULL, 0);
    }

    LOG_RD("[DBG] NP: Socket created successfully (client_fd=%d, real_fd=%d)\r\n",
           client_fd, real_fd);

    int resp_ret = send_rpc_response(rpcs, rpc_id, client_fd, 0, NULL, 0);
    LOG_RD("[DBG] NP: Sent socket create response (rpc_id=%u, fd=%d, ret=%d)\r\n",
           (unsigned int)rpc_id, client_fd, resp_ret);
    return resp_ret;
}

/**
 * @brief Handle socket close RPC
 */
static int handle_socket_close(void *data, struct rpmsg_rpc_svr *rpcs)
{
    /* Data format: [4 bytes RPC ID] + [int fd] */
    uint32_t rpc_id = *(uint32_t *)data;
    int fd = *(int *)((uint8_t *)data + MAX_FUNC_ID_LEN);
    struct socket_entry *entry;
    int ret = 0;
    int error = 0;

    entry = ((struct rpc_work *)rpcs)->entry;
    if (!entry) {
        LOG_RE("[ERR] NP: Socket close failed - fd not found (fd=%d)\r\n", fd);
        return send_rpc_response(rpcs, rpc_id, -1, EBADF, NULL, 0);
    }

    xSemaphoreTakeRecursive(entry->lock, portMAX_DELAY);

    if (entry->real_fd >= 0) {
        ret = lwip_close(entry->real_fd);
        error = ret < 0 ? errno : 0;
        if (ret < 0) {
            LOG_RE("[ERR] NP: Failed to close real socket (real_fd=%d): errno=%d\r\n",
                   entry->real_fd, error);
        } else {
            LOG_RD("[DBG] NP: Real socket closed successfully (real_fd=%d)\r\n",
                   entry->real_fd);
        }
        entry->real_fd = -1;
    } else {
        LOG_RW("[WRN] NP: No real socket to close for client_fd=%d\r\n", fd);
    }
    free_socket_fd(fd);
    xSemaphoreGiveRecursive(entry->lock);

    LOG_RD("[DBG] NP: Socket closed successfully (fd=%d)\r\n", fd);
    return send_rpc_response(rpcs, rpc_id, ret, error, NULL, 0);
}

/**
 * @brief Handle socket connect RPC
 */
static int handle_socket_connect(void *data, struct rpmsg_rpc_svr *rpcs)
{
    /* Data format: [4 bytes RPC ID] + [struct rpc_socket_connect] */
    uint32_t rpc_id = *(uint32_t *)data;
    struct rpc_socket_connect *req = (struct rpc_socket_connect *)((uint8_t *)data + MAX_FUNC_ID_LEN);
    struct socket_entry *entry;
    int error = 0;
    int ret;

#if CONFIG_LOG_LEVEL >= 3
    if (req->addr.sa_family == AF_INET) {
        struct sockaddr_in *addr_in = (struct sockaddr_in *)&req->addr;
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr_in->sin_addr, ip_str, sizeof(ip_str));
        LOG_RI("[INF] NP: Target Address: %s:%d (IPv4)\r\n", ip_str, ntohs(addr_in->sin_port));
    }
#endif

    entry = ((struct rpc_work *)rpcs)->entry;
    if (!entry) {
        LOG_RE("[ERR] NP: Socket connect failed - fd not found (fd=%d)\r\n", req->sockfd);
        return send_rpc_response(rpcs, rpc_id, -1, EBADF, NULL, 0);
    }

    xSemaphoreTakeRecursive(entry->lock, portMAX_DELAY);

    /* Call connect on the real socket */
    ret = lwip_connect(entry->real_fd, (struct sockaddr *)&req->addr, req->addrlen);
    if (ret < 0) {
        error = errno;
        if (error != EINPROGRESS && error != EALREADY && error != EWOULDBLOCK) {
            LOG_RE("[ERR] NP: Failed to connect real socket (real_fd=%d): errno=%d\r\n",
                   entry->real_fd, error);
        }
    } else {
        LOG_RD("[DBG] NP: Socket connected successfully (fd=%d, real_fd=%d)\r\n",
               req->sockfd, entry->real_fd);
        entry->is_connected = true;
        memcpy(&entry->remote_addr, &req->addr, req->addrlen);
    }

    xSemaphoreGiveRecursive(entry->lock);

    return send_rpc_response(rpcs, rpc_id, ret, error, NULL, 0);
}

/**
 * @brief Handle socket control RPC
 */
static int handle_socket_ioctl(void *data, struct rpmsg_rpc_svr *rpcs)
{
    uint32_t rpc_id = *(uint32_t *)data;
    struct rpc_socket_ioctl *req = (struct rpc_socket_ioctl *)((uint8_t *)data + MAX_FUNC_ID_LEN);
    struct socket_entry *entry = ((struct rpc_work *)rpcs)->entry;
    unsigned long value = req->value;
    int ret;
    int error;

    if (!entry) {
        return send_rpc_response(rpcs, rpc_id, -1, EBADF, NULL, 0);
    }
    if (req->cmd != FIONBIO) {
        return send_rpc_response(rpcs, rpc_id, -1, EINVAL, NULL, 0);
    }
    xSemaphoreTakeRecursive(entry->lock, portMAX_DELAY);
    ret = lwip_ioctl(entry->real_fd, req->cmd, &value);
    error = ret < 0 ? errno : 0;
    xSemaphoreGiveRecursive(entry->lock);
    return send_rpc_response(rpcs, rpc_id, ret, error, NULL, 0);
}

/**
 * @brief Handle socket readiness RPC
 */
static int handle_socket_poll(void *data, struct rpmsg_rpc_svr *rpcs)
{
    uint32_t rpc_id = *(uint32_t *)data;
    struct rpc_socket_poll *req = (struct rpc_socket_poll *)((uint8_t *)data + MAX_FUNC_ID_LEN);
    struct socket_entry *entry = ((struct rpc_work *)rpcs)->entry;
    struct pollfd pfd;
    int ret;
    int error;

    if (!entry) {
        return send_rpc_response(rpcs, rpc_id, -1, EBADF, NULL, 0);
    }
    pfd.events = req->events;
    pfd.revents = 0;
    xSemaphoreTakeRecursive(entry->lock, portMAX_DELAY);
    pfd.fd = entry->real_fd;
    ret = lwip_poll(&pfd, 1, 0);
    error = ret < 0 ? errno : 0;
    xSemaphoreGiveRecursive(entry->lock);
    return send_rpc_response(rpcs, rpc_id, ret, error,
                             ret < 0 ? NULL : &pfd.revents,
                             ret < 0 ? 0 : sizeof(pfd.revents));
}

/**
 * @brief Handle socket bind RPC
 */
static int handle_socket_bind(void *data, struct rpmsg_rpc_svr *rpcs)
{
    /* Data format: [4 bytes RPC ID] + [struct rpc_socket_bind] */
    uint32_t rpc_id = *(uint32_t *)data;
    struct rpc_socket_bind *req = (struct rpc_socket_bind *)((uint8_t *)data + MAX_FUNC_ID_LEN);
    struct socket_entry *entry;
    int ret;

#if CONFIG_LOG_LEVEL >= 3
    if (req->addr.sa_family == AF_INET) {
        struct sockaddr_in *addr_in = (struct sockaddr_in *)&req->addr;
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr_in->sin_addr, ip_str, sizeof(ip_str));
        LOG_RI("[INF] NP: Address: %s:%d (IPv4)\r\n", ip_str, ntohs(addr_in->sin_port));
    }
#endif
    entry = ((struct rpc_work *)rpcs)->entry;
    if (!entry) {
        return send_rpc_response(rpcs, rpc_id, -1, EBADF, NULL, 0);
    }

    xSemaphoreTakeRecursive(entry->lock, portMAX_DELAY);

#if CONFIG_LOG_LEVEL >= 4
    if (req->addr.sa_family == AF_INET) {
        struct sockaddr_in *addr_in = (struct sockaddr_in *)&req->addr;
        LOG_RD("[DBG] NP:   bind_addr_raw: 0x%02X%02X%02X%02X\r\n",
               (unsigned int)(addr_in->sin_addr.s_addr & 0xFF),
               (unsigned int)((addr_in->sin_addr.s_addr >> 8) & 0xFF),
               (unsigned int)((addr_in->sin_addr.s_addr >> 16) & 0xFF),
               (unsigned int)((addr_in->sin_addr.s_addr >> 24) & 0xFF));
    }
#endif

    ret = lwip_bind(entry->real_fd, (struct sockaddr *)&req->addr, req->addrlen);
    int error = ret < 0 ? errno : 0;

    if (ret < 0) {
        LOG_RE("[ERR] NP: Failed to bind real socket (real_fd=%d): errno=%d (%s)\r\n",
               entry->real_fd, error, strerror(error));
    } else {
        LOG_RD("[DBG] NP: Socket bound successfully (fd=%d, real_fd=%d)\r\n",
               req->sockfd, entry->real_fd);
        entry->is_bound = true;
        memcpy(&entry->local_addr, &req->addr, req->addrlen);
    }

    xSemaphoreGiveRecursive(entry->lock);

    return send_rpc_response(rpcs, rpc_id, ret, error, NULL, 0);
}

/**
 * @brief Handle socket send and sendto RPC fragments
 */
static int handle_socket_send(void *data, struct rpmsg_rpc_svr *rpcs)
{
    uint32_t rpc_id = *(uint32_t *)data;
    const uint8_t *fragment = (uint8_t *)data + MAX_FUNC_ID_LEN;
    size_t fragment_len = rpcs->request_len - MAX_FUNC_ID_LEN - offsetof(struct rpc_request_fragment, data);
    size_t base_header_len = rpc_id == RPC_SOCKET_SEND ?
                             sizeof(struct rpc_socket_send) : offsetof(struct rpc_socket_sendto, data);
    struct rpc_socket_sendto request = {0};
    size_t header_len;
    struct socket_entry *entry;
    int error;
    int ret;

    if (fragment_len < base_header_len) {
        return send_rpc_response(rpcs, rpc_id, -1, EPROTO, NULL, 0);
    }
    memcpy(&request, fragment, base_header_len);
    header_len = base_header_len + request.addrlen;
    if (header_len > fragment_len || request.addrlen > sizeof(struct sockaddr_storage)) {
        return send_rpc_response(rpcs, rpc_id, -1, EPROTO, NULL, 0);
    }
    entry = ((struct rpc_work *)rpcs)->entry;
    if (!entry) {
        return send_rpc_response(rpcs, rpc_id, -1, EBADF, NULL, 0);
    }
    size_t payload_len = fragment_len - header_len;

    xSemaphoreTakeRecursive(entry->lock, portMAX_DELAY);
    if (entry->type == SOCK_STREAM) {
        ret = lwip_send(entry->real_fd, fragment + header_len, payload_len, request.flags);
    } else {
        ret = lwip_sendto(entry->real_fd, fragment + header_len, payload_len,
                          request.flags,
                          request.addrlen ? (const struct sockaddr *)(fragment + base_header_len) : NULL,
                          request.addrlen);
    }
    error = ret < 0 ? errno : 0;
    xSemaphoreGiveRecursive(entry->lock);
    return send_rpc_response(rpcs, rpc_id, ret, error, NULL, 0);
}

/**
 * @brief Handle socket listen RPC
 */
static int handle_socket_listen(void *data, struct rpmsg_rpc_svr *rpcs)
{
    /* Data format: [4 bytes RPC ID] + [struct rpc_socket_listen] */
    uint32_t rpc_id = *(uint32_t *)data;
    struct rpc_socket_listen *req = (struct rpc_socket_listen *)((uint8_t *)data + MAX_FUNC_ID_LEN);
    struct socket_entry *entry;
    int ret;

    entry = ((struct rpc_work *)rpcs)->entry;
    if (!entry) {
        LOG_RE("[ERR] NP: Socket listen failed - fd not found (fd=%d)\r\n", req->sockfd);
        return send_rpc_response(rpcs, rpc_id, -1, EBADF, NULL, 0);
    }

    if (!entry->is_bound) {
        LOG_RE("[ERR] NP: Socket listen failed - socket not bound (fd=%d)\r\n", req->sockfd);
        return send_rpc_response(rpcs, rpc_id, -1, EINVAL, NULL, 0);
    }

    xSemaphoreTakeRecursive(entry->lock, portMAX_DELAY);

    /* Call listen on the real socket */
    ret = lwip_listen(entry->real_fd, req->backlog);
    int error = ret < 0 ? errno : 0;
    if (ret < 0) {
        LOG_RE("[ERR] NP: Failed to listen on real socket (real_fd=%d): errno=%d\r\n",
               entry->real_fd, error);
    } else {
        LOG_RD("[DBG] NP: Socket listening successfully (fd=%d, real_fd=%d, backlog=%d)\r\n",
               req->sockfd, entry->real_fd, req->backlog);
    }

    xSemaphoreGiveRecursive(entry->lock);

    return send_rpc_response(rpcs, rpc_id, ret, error, NULL, 0);
}

/**
 * @brief Handle socket accept RPC
 */
static int handle_socket_accept(void *data, struct rpmsg_rpc_svr *rpcs)
{
    /* Data format: [4 bytes RPC ID] + [struct rpc_socket_accept] */
    uint32_t rpc_id = *(uint32_t *)data;
    struct rpc_socket_accept *req = (struct rpc_socket_accept *)((uint8_t *)data + MAX_FUNC_ID_LEN);
    struct socket_entry *entry, *new_entry;
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    int real_new_fd, client_new_fd;
    int error;
    int ret;

    LOG_RD("[DBG] NP: Received socket accept RPC (fd=%d)\r\n", req->sockfd);
    LOG_RD("[DBG] NP: RPC ID: %u\r\n", (unsigned int)rpc_id);

    entry = find_socket_entry(req->sockfd);
    if (!entry) {
        LOG_RE("[ERR] NP: Socket accept failed - fd not found (fd=%d)\r\n", req->sockfd);
        return send_rpc_response(rpcs, rpc_id, -1, EBADF, NULL, 0);
    }

    xSemaphoreTakeRecursive(entry->lock, portMAX_DELAY);

    /* Call accept on the real socket */
    real_new_fd = lwip_accept(entry->real_fd, (struct sockaddr *)&addr, &addr_len);
    if (real_new_fd < 0) {
        error = errno;
        LOG_RW("[WRN] NP: Failed to accept connection on real socket (real_fd=%d): errno=%d\r\n",
               entry->real_fd, error);
        xSemaphoreGiveRecursive(entry->lock);
        return send_rpc_response(rpcs, rpc_id, -1, error, NULL, 0);
    }

    /* Allocate a new client FD for the accepted connection */
    client_new_fd = alloc_socket_fd(entry->family, entry->type, entry->protocol);
    if (client_new_fd < 0) {
        LOG_RE("[ERR] NP: Failed to allocate client FD for accepted connection\r\n");
        lwip_close(real_new_fd);
        xSemaphoreGiveRecursive(entry->lock);
        return send_rpc_response(rpcs, rpc_id, -1, -client_new_fd, NULL, 0);
    }

    /* Map the new client FD to the real FD */
    new_entry = find_socket_entry(client_new_fd);
    if (new_entry) {
        new_entry->real_fd = real_new_fd;
        new_entry->is_connected = true;
        memcpy(&new_entry->remote_addr, &addr, addr_len);
    }

    LOG_RD("[DBG] NP: Connection accepted (client_fd=%d, real_fd=%d)\r\n",
           client_new_fd, real_new_fd);

    xSemaphoreGiveRecursive(entry->lock);

    /* Send response with the new client FD. */
    ret = send_rpc_response(rpcs, rpc_id, client_new_fd, 0, NULL, 0);
    LOG_RD("[DBG] NP: Sent socket accept response (client_fd=%d, ret=%d)\r\n",
           client_new_fd, ret);

    return ret;
}

/**
 * @brief Handle socket receive RPC
 */
static int handle_socket_receive(void *data, struct rpmsg_rpc_svr *rpcs)
{
    uint32_t rpc_id = *(uint32_t *)data;
    struct rpc_work *work = (struct rpc_work *)rpcs;
    struct rpc_socket_recv request;
    bool with_address = rpc_id == RPC_SOCKET_RECVFROM;
    struct socket_entry *entry;
    struct sockaddr_storage src_addr = {0};
    socklen_t src_addr_len = sizeof(src_addr);
    size_t recv_len;
    size_t data_len = 0;
    int error = 0;
    int ret;

    memcpy(&request, (uint8_t *)data + MAX_FUNC_ID_LEN, sizeof(request));
    entry = work->entry;
    if (!entry) { return send_rpc_response(rpcs, rpc_id, -1, EBADF, NULL, 0); }
    if (!with_address && !entry->is_connected) {
        return send_rpc_response(rpcs, rpc_id, -1, ENOTCONN, NULL, 0);
    }
    xSemaphoreTakeRecursive(entry->lock, portMAX_DELAY);
    if (entry->receiving) {
        xSemaphoreGiveRecursive(entry->lock);
        return send_rpc_response(rpcs, rpc_id, -1, EBUSY, NULL, 0);
    }
    entry->receiving = true;
    xSemaphoreGiveRecursive(entry->lock);
    recv_len = LWIP_MIN(request.len, RPC_MAX_BUF_LEN);
    if (entry->type == SOCK_DGRAM) {
        struct iovec iov = { .iov_base = work->buffer, .iov_len = recv_len ? recv_len : 1 };
        struct msghdr msg = { .msg_name = &src_addr, .msg_namelen = sizeof(src_addr),
                             .msg_iov = &iov, .msg_iovlen = 1 };

        ret = lwip_recvmsg(entry->real_fd, &msg, request.flags);
        src_addr_len = msg.msg_namelen;
        if (ret > (int)RPC_MAX_BUF_LEN && request.len > RPC_MAX_BUF_LEN) {
            ret = -1;
            errno = EMSGSIZE;
        } else if (ret >= 0) {
            ret = LWIP_MIN((size_t)ret, recv_len);
        }
    } else {
        ret = lwip_recvfrom(entry->real_fd, work->buffer, recv_len, request.flags,
                            (struct sockaddr *)&src_addr, &src_addr_len);
    }
    if (ret < 0) {
        error = errno;
        LOG_RW("[WRN] NP: Failed to recvfrom from real socket (real_fd=%d): errno=%d\r\n",
               entry->real_fd, error);
    }

    if (ret >= 0) {
        data_len = (size_t)ret;
        if (with_address && (ret > 0 || entry->type == SOCK_DGRAM)) {
            memcpy(work->buffer + data_len, &src_addr, src_addr_len);
            data_len += src_addr_len;
        }
    }

    xSemaphoreTakeRecursive(entry->lock, portMAX_DELAY);
    entry->receiving = false;
    xSemaphoreGiveRecursive(entry->lock);
    return send_rpc_response(rpcs, rpc_id, ret, error, work->buffer, data_len);
}

/**
 * @brief Handle socket setsockopt RPC
 */
static int handle_socket_setsockopt(void *data, struct rpmsg_rpc_svr *rpcs)
{
    /* Data format: [4 bytes RPC ID] + [struct rpc_socket_setsockopt] + optval */
    uint32_t rpc_id = *(uint32_t *)data;
    struct rpc_socket_setsockopt *req = (struct rpc_socket_setsockopt *)((uint8_t *)data + MAX_FUNC_ID_LEN);
    struct socket_entry *entry;
    char *optval;
    int ret;

    LOG_RD("[DBG] NP: Received socket setsockopt RPC (fd=%d, level=%d, optname=%d, optlen=%d)\r\n",
           req->sockfd, req->level, req->optname, (int)req->optlen);
    LOG_RD("[DBG] NP: RPC ID: %u\r\n", (unsigned int)rpc_id);

    /* Option value follows the header struct */
    optval = (char *)((uint8_t *)data + MAX_FUNC_ID_LEN + offsetof(struct rpc_socket_setsockopt, data));

    entry = ((struct rpc_work *)rpcs)->entry;
    if (!entry) {
        LOG_RE("[ERR] NP: Socket setsockopt failed - fd not found (fd=%d)\r\n", req->sockfd);
        return send_rpc_response(rpcs, rpc_id, -1, EBADF, NULL, 0);
    }

    xSemaphoreTakeRecursive(entry->lock, portMAX_DELAY);

    /* Call setsockopt on the real socket */
    ret = lwip_setsockopt(entry->real_fd, req->level, req->optname, optval, req->optlen);
    int error = ret < 0 ? errno : 0;
    if (ret < 0) {
        LOG_RE("[ERR] NP: Failed to setsockopt on real socket (real_fd=%d): errno=%d\r\n",
               entry->real_fd, error);
    } else {
        LOG_RD("[DBG] NP: setsockopt completed successfully (real_fd=%d)\r\n", entry->real_fd);
    }

    xSemaphoreGiveRecursive(entry->lock);

    return send_rpc_response(rpcs, rpc_id, ret, error, NULL, 0);
}

/**
 * @brief Handle socket getsockopt RPC
 */
static int handle_socket_getsockopt(void *data, struct rpmsg_rpc_svr *rpcs)
{
    /* Data format: [4 bytes RPC ID] + [struct rpc_socket_getsockopt] */
    uint32_t rpc_id = *(uint32_t *)data;
    struct rpc_socket_getsockopt *req = (struct rpc_socket_getsockopt *)((uint8_t *)data + MAX_FUNC_ID_LEN);
    struct socket_entry *entry;
    char optval_buffer[256];
    socklen_t optlen = req->optlen;
    int ret;

    LOG_RD("[DBG] NP: Received socket getsockopt RPC (fd=%d, level=%d, optname=%d, optlen=%d)\r\n",
           req->sockfd, req->level, req->optname, (int)req->optlen);
    LOG_RD("[DBG] NP: RPC ID: %u\r\n", (unsigned int)rpc_id);

    entry = ((struct rpc_work *)rpcs)->entry;
    if (!entry) {
        LOG_RE("[ERR] NP: Socket getsockopt failed - fd not found (fd=%d)\r\n", req->sockfd);
        return send_rpc_response(rpcs, rpc_id, -1, EBADF, NULL, 0);
    }

    if (optlen > sizeof(optval_buffer)) {
        optlen = sizeof(optval_buffer);
    }

    xSemaphoreTakeRecursive(entry->lock, portMAX_DELAY);

    /* Call getsockopt on the real socket */
    ret = lwip_getsockopt(entry->real_fd, req->level, req->optname, optval_buffer, &optlen);
    int error = ret < 0 ? errno : 0;
    if (ret == 0 && entry->type == SOCK_STREAM && req->level == SOL_SOCKET &&
        req->optname == SO_ERROR && optlen == sizeof(int)) {
        int socket_error;

        memcpy(&socket_error, optval_buffer, sizeof(socket_error));
        if (socket_error == 0) {
            entry->is_connected = true;
        }
    }
    if (ret < 0) {
        LOG_RE("[ERR] NP: Failed to getsockopt on real socket (real_fd=%d): errno=%d\r\n",
               entry->real_fd, error);
    } else {
        LOG_RD("[DBG] NP: getsockopt completed successfully (real_fd=%d, optlen=%d)\r\n",
               entry->real_fd, (int)optlen);
    }

    xSemaphoreGiveRecursive(entry->lock);

    /* Send response with option value */
    return send_rpc_response(rpcs, rpc_id, ret, error,
                                  ret == 0 ? optval_buffer : NULL, ret == 0 ? optlen : 0);
}

/**
 * @brief Handle socket getsockname RPC
 */
static int handle_socket_getsockname(void *data, struct rpmsg_rpc_svr *rpcs)
{
    /* Data format: [4 bytes RPC ID] + [int sockfd] */
    uint32_t rpc_id = *(uint32_t *)data;
    int fd = *(int *)((uint8_t *)data + MAX_FUNC_ID_LEN);
    struct socket_entry *entry;
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    int ret;

    LOG_RD("[DBG] NP: Received socket getsockname RPC (fd=%d)\r\n", fd);
    LOG_RD("[DBG] NP: RPC ID: %u\r\n", (unsigned int)rpc_id);

    entry = find_socket_entry(fd);
    if (!entry) {
        LOG_RE("[ERR] NP: Socket getsockname failed - fd not found (fd=%d)\r\n", fd);
        return send_rpc_response(rpcs, rpc_id, -1, EBADF, NULL, 0);
    }

    xSemaphoreTakeRecursive(entry->lock, portMAX_DELAY);

    /* Call getsockname on the real socket */
    ret = lwip_getsockname(entry->real_fd, (struct sockaddr *)&addr, &addr_len);
    int error = ret < 0 ? errno : 0;
    if (ret < 0) {
        LOG_RE("[ERR] NP: Failed to getsockname on real socket (real_fd=%d): errno=%d\r\n",
               entry->real_fd, error);
    } else {
        LOG_RD("[DBG] NP: getsockname completed successfully (real_fd=%d, addr_len=%d)\r\n",
               entry->real_fd, (int)addr_len);
    }

    xSemaphoreGiveRecursive(entry->lock);

    /* Send response with socket address */
    return send_rpc_response(rpcs, rpc_id, ret, error,
                                  ret == 0 ? &addr : NULL, ret == 0 ? addr_len : 0);
}

/**
 * @brief Handle socket getpeername RPC
 */
static int handle_socket_getpeername(void *data, struct rpmsg_rpc_svr *rpcs)
{
    /* Data format: [4 bytes RPC ID] + [int sockfd] */
    uint32_t rpc_id = *(uint32_t *)data;
    int fd = *(int *)((uint8_t *)data + MAX_FUNC_ID_LEN);
    struct socket_entry *entry;
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    int ret;

    LOG_RD("[DBG] NP: Received socket getpeername RPC (fd=%d)\r\n", fd);
    LOG_RD("[DBG] NP: RPC ID: %u\r\n", (unsigned int)rpc_id);

    entry = find_socket_entry(fd);
    if (!entry) {
        LOG_RE("[ERR] NP: Socket getpeername failed - fd not found (fd=%d)\r\n", fd);
        return send_rpc_response(rpcs, rpc_id, -1, EBADF, NULL, 0);
    }

    if (!entry->is_connected) {
        LOG_RE("[ERR] NP: Socket getpeername failed - not connected (fd=%d)\r\n", fd);
        return send_rpc_response(rpcs, rpc_id, -1, ENOTCONN, NULL, 0);
    }

    xSemaphoreTakeRecursive(entry->lock, portMAX_DELAY);

    /* Call getpeername on the real socket */
    ret = lwip_getpeername(entry->real_fd, (struct sockaddr *)&addr, &addr_len);
    int error = ret < 0 ? errno : 0;
    if (ret < 0) {
        LOG_RE("[ERR] NP: Failed to getpeername on real socket (real_fd=%d): errno=%d\r\n",
               entry->real_fd, error);
    } else {
        LOG_RD("[DBG] NP: getpeername completed successfully (real_fd=%d, addr_len=%d)\r\n",
               entry->real_fd, (int)addr_len);
    }

    xSemaphoreGiveRecursive(entry->lock);

    /* Send response with peer address */
    return send_rpc_response(rpcs, rpc_id, ret, error,
                                  ret == 0 ? &addr : NULL, ret == 0 ? addr_len : 0);
}

/**
 * @brief Handle socket shutdown RPC
 */
static int handle_socket_shutdown(void *data, struct rpmsg_rpc_svr *rpcs)
{
    /* Data format: [4 bytes RPC ID] + [int sockfd] + [int how] */
    uint32_t rpc_id = *(uint32_t *)data;
    int fd = *(int *)((uint8_t *)data + MAX_FUNC_ID_LEN);
    int how = *(int *)((uint8_t *)data + MAX_FUNC_ID_LEN + sizeof(int));
    struct socket_entry *entry;
    int ret;

    LOG_RD("[DBG] NP: Received socket shutdown RPC (fd=%d, how=%d)\r\n", fd, how);
    LOG_RD("[DBG] NP: RPC ID: %u\r\n", (unsigned int)rpc_id);

    entry = find_socket_entry(fd);
    if (!entry) {
        LOG_RE("[ERR] NP: Socket shutdown failed - fd not found (fd=%d)\r\n", fd);
        return send_rpc_response(rpcs, rpc_id, -1, EBADF, NULL, 0);
    }

    xSemaphoreTakeRecursive(entry->lock, portMAX_DELAY);

    /* Call shutdown on the real socket */
    ret = lwip_shutdown(entry->real_fd, how);
    int error = ret < 0 ? errno : 0;
    if (ret < 0) {
        LOG_RE("[ERR] NP: Failed to shutdown real socket (real_fd=%d): errno=%d\r\n",
               entry->real_fd, error);
    } else {
        LOG_RD("[DBG] NP: Socket shutdown successfully (real_fd=%d)\r\n", entry->real_fd);
        entry->is_connected = false;
    }

    xSemaphoreGiveRecursive(entry->lock);

    return send_rpc_response(rpcs, rpc_id, ret, error, NULL, 0);
}

/* Socket operation handlers. */
static const struct rpmsg_rpc_services socket_rpc_services[] = {
    { .id = RPC_SOCKET_CREATE, .cb_function = handle_socket_create },
    { .id = RPC_SOCKET_CLOSE, .cb_function = handle_socket_close },
    { .id = RPC_SOCKET_BIND, .cb_function = handle_socket_bind },
    { .id = RPC_SOCKET_CONNECT, .cb_function = handle_socket_connect },
    { .id = RPC_SOCKET_LISTEN, .cb_function = handle_socket_listen },
    { .id = RPC_SOCKET_ACCEPT, .cb_function = handle_socket_accept },
    { .id = RPC_SOCKET_SEND, .cb_function = handle_socket_send },
    { .id = RPC_SOCKET_SENDTO, .cb_function = handle_socket_send },
    { .id = RPC_SOCKET_RECV, .cb_function = handle_socket_receive },
    { .id = RPC_SOCKET_RECVFROM, .cb_function = handle_socket_receive },
    { .id = RPC_SOCKET_SETSOCKOPT, .cb_function = handle_socket_setsockopt },
    { .id = RPC_SOCKET_GETSOCKOPT, .cb_function = handle_socket_getsockopt },
    { .id = RPC_SOCKET_GETSOCKNAME, .cb_function = handle_socket_getsockname },
    { .id = RPC_SOCKET_GETPEERNAME, .cb_function = handle_socket_getpeername },
    { .id = RPC_SOCKET_SHUTDOWN, .cb_function = handle_socket_shutdown },
    { .id = RPC_SOCKET_IOCTL, .cb_function = handle_socket_ioctl },
    { .id = RPC_SOCKET_POLL, .cb_function = handle_socket_poll },
};

static void socket_rpc_task(void *arg)
{
    struct rpc_work *work = arg;
    /* Kept alive until the service task sends the response and reuses this worker. */
    struct service_event response = { .type = SERVICE_RESPONSE, .work = work };
    struct service_event *queued = &response;

    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (g_rpc_server.stopping) { break; }
        uint32_t rpc_id = *(uint32_t *)work->buffer;
        size_t i;

        for (i = 0; i < LWIP_ARRAYSIZE(socket_rpc_services) && socket_rpc_services[i].id != rpc_id; i++) {}
        if (i == LWIP_ARRAYSIZE(socket_rpc_services)) {
            send_rpc_response(&work->rpc, rpc_id, -1, EINVAL, NULL, 0);
        } else {
            socket_rpc_services[i].cb_function(work->buffer, &work->rpc);
        }
        response.rpc_id = rpc_id;
        response.result = work->result;
        response.error = work->error;
        response.data = work->buffer;
        response.len = work->data_len;
        xQueueSend(g_rpc_server.queue, &queued, portMAX_DELAY);
    }
    LWIP_NETCONN_THREAD_SEM_FREE();
    struct service_event event = { .type = SERVICE_STOPPED, .work = work };
    service_event_call(&event);
    vTaskSuspend(NULL);
}

/* Copy each request into an idle worker and return to the RPMsg task. */
static int dispatch_socket_request(void *data, size_t request_len)
{
    struct rpc_request_fragment *request = (struct rpc_request_fragment *)((uint8_t *)data + MAX_FUNC_ID_LEN);
    const size_t header_len = offsetof(struct rpc_request_fragment, data);
    struct rpc_work *work;
    size_t fragment_len;

    if (g_rpc_server.stopping) { return -ENOTCONN; }
    if (request_len <= MAX_FUNC_ID_LEN + header_len) {
        return -EPROTO;
    }
    fragment_len = request_len - MAX_FUNC_ID_LEN - header_len;
    if (fragment_len > RPC_REQUEST_DATA_LEN || request->offset != 0 || request->total_len != fragment_len) {
        return transmit_rpc_response(request->sequence, request->rpc_id, -1, EMSGSIZE, NULL, 0);
    }
    if (fragment_len < sizeof(int)) {
        return transmit_rpc_response(request->sequence, request->rpc_id, -1, EPROTO, NULL, 0);
    }

    for (work = g_rpc_server.workers; work && work->busy; work = work->next) {}
    if (!work) {
        work = kmalloc(sizeof(*work), MM_FLAG_HEAP_OCRAM_0);
        if (!work) {
            return transmit_rpc_response(request->sequence, request->rpc_id, -1, ENOMEM, NULL, 0);
        }
        memset(work, 0, sizeof(*work));
        work->task = xTaskCreateStatic(socket_rpc_task, "socket-rpc", LWIP_ARRAYSIZE(work->stack),
                                       work, 16, work->stack, &work->task_storage);
        work->next = g_rpc_server.workers;
        g_rpc_server.workers = work;
    }
    work->sequence = request->sequence;
    work->rpc.request_len = request_len;
    work->busy = true;
    memcpy(work->buffer, &request->rpc_id, MAX_FUNC_ID_LEN + fragment_len);
    if (request->rpc_id != RPC_SOCKET_CREATE) {
        int fd;
        memcpy(&fd, request->data, sizeof(fd));
        work->entry = find_socket_entry(fd);
        if (work->entry) { work->entry->references++; }
    }
    xTaskNotifyGive(work->task);
    return 0;
}

/* The OpenAMP callback's buffer remains valid until dispatch acknowledges it. */
static int handle_socket_request(void *data, struct rpmsg_rpc_svr *rpcs)
{
    struct service_event event = { .type = SERVICE_REQUEST, .data = data, .len = rpcs->request_len };

    service_event_call(&event);
    return event.result;
}

static void socket_service_task(void *arg)
{
    struct service_event *event;
    SemaphoreHandle_t exit_done = NULL;

    (void)arg;
    for (;;) {
        xQueueReceive(g_rpc_server.queue, &event, portMAX_DELAY);
        SemaphoreHandle_t done = event->done;
        switch (event->type) {
        case SERVICE_REQUEST:
            event->result = dispatch_socket_request(event->data, event->len);
            break;
        case SERVICE_RESPONSE:
            event->result = transmit_rpc_response(event->work->sequence, event->rpc_id,
                                                  event->result, event->error, event->data, event->len);
            if (event->work->entry) { event->work->entry->references--; }
            event->work->entry = NULL;
            event->work->busy = false;
            break;
        case SERVICE_ALLOC:
            event->result = alloc_socket_fd(event->args[0], event->args[1], event->args[2]);
            break;
        case SERVICE_FREE:
            event->result = free_socket_fd(event->args[0]);
            break;
        case SERVICE_FIND:
            event->entry = find_socket_entry(event->args[0]);
            break;
        case SERVICE_STOPPED:
            event->work->stopped = true;
            break;
        case SERVICE_STOP:
            g_rpc_server.stopping = true;
            for (struct rpc_work *work = g_rpc_server.workers; work; work = work->next) {
                xTaskNotifyGive(work->task);
            }
            break;
        case SERVICE_EXIT:
            exit_done = done;
            done = NULL;
            break;
        }
        if (done) { xSemaphoreGive(done); }
        if (exit_done) {
            struct rpc_work *work;
            for (work = g_rpc_server.workers; work && work->stopped; work = work->next) {}
            if (!work) {
                xSemaphoreGive(exit_done);
                vTaskSuspend(NULL);
            }
        }
    }
}

/* RPMsg request transport services. */
static const struct rpmsg_rpc_services socket_rpc_transport_services[] = {
    { .id = RPC_SOCKET_REQUEST, .cb_function = handle_socket_request },
};

/**
 * @brief Initialize RPC server
 */
static int init_rpc_server(struct rpmsg_device *rdev)
{
    int ret;

    LOG_RD("[DBG] NP: Initializing RPC server\r\n");

    /* Initialize socket table */
    memset(&g_rpc_server, 0, sizeof(g_rpc_server));
    g_rpc_server.next_fd = SOCKET_FD_BASE;
    g_rpc_server.queue = xQueueCreateStatic(RPC_QUEUE_LEN, sizeof(struct service_event *),
                                           g_rpc_server.queue_items, &g_rpc_server.queue_storage);
    g_rpc_server.task = xTaskCreateStatic(socket_service_task, "socket-service",
                                         LWIP_ARRAYSIZE(g_rpc_server.stack), NULL, 16,
                                         g_rpc_server.stack, &g_rpc_server.task_storage);

    LOG_RD("[DBG] NP: Initializing rpmsg_rpc_server with %d services\r\n",
           (int)LWIP_ARRAYSIZE(socket_rpc_transport_services));
    ret = rpmsg_rpc_server_init(&g_rpc_server.rpc_server, rdev,
                               socket_rpc_transport_services,
                               LWIP_ARRAYSIZE(socket_rpc_transport_services),
                               NULL);
    if (ret < 0) {
        LOG_RE("[ERR] NP: Failed to initialize RPC server (ret=%d)\r\n", ret);
        goto error;
    }

    g_rpc_server.initialized = true;
    LOG_RD("[DBG] NP: RPC server initialized successfully\r\n");

    return 0;

error:
    g_rpc_server.initialized = true;
    rpc_service_shutdown();
    return ret;
}

/**
 * @brief Initialize RPC service for socket offload
 */
int rpc_service_init(void)
{
    int ret;
    struct rpmsg_device *rdev;

    if (g_rpc_server.initialized) {
        return 0;
    }

    LOG_RD("[DBG] NP: Initializing RPC service\r\n");

    rdev = rpmsg_service_get_device();
    if (!rdev) {
        LOG_RE("[ERR] NP: Failed to get RPMsg device\r\n");
        return -ENODEV;
    }

    LOG_RD("[DBG] NP: RPMsg device obtained successfully\r\n");

    ret = init_rpc_server(rdev);
    if (ret < 0) {
        return ret;
    }

    LOG_RD("[DBG] NP: RPC service initialization completed\r\n");

    return 0;
}

#ifdef CONFIG_SHELL
static int __attribute__((unused)) cmd_rpc_status(int argc, char **argv)
{
    int active_sockets = 0;

    (void)argc;
    (void)argv;

    printf("Network Core RPC Service Status:\r\n");
    printf("  RPC Server: %s\r\n", g_rpc_server.initialized ? "Active" : "Inactive");

    for (int i = 0; i < MAX_SOCKETS; i++) {
        if (g_rpc_server.sockets[i].is_used) {
            active_sockets++;
        }
    }

    printf("  Active Sockets: %d/%d\r\n", active_sockets, MAX_SOCKETS);
    return 0;
}

static int __attribute__((unused)) cmd_rpc_list_sockets(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("Active Sockets:\r\n");
    printf("  FD    Type    State\r\n");
    printf("  --    ----    -----\r\n");

    for (int i = 0; i < MAX_SOCKETS; i++) {
        struct socket_entry *entry = &g_rpc_server.sockets[i];
        if (entry->is_used) {
            const char *type_str = entry->type == SOCK_STREAM ? "TCP" : "UDP";
            const char *state_str = entry->is_connected ? "CONNECTED" :
                                   (entry->is_bound ? "BOUND" : "CREATED");
            printf("  %-4d %-7s %s\r\n", entry->fd, type_str, state_str);
        }
    }

    return 0;
}

#endif

void rpc_service_shutdown(void)
{
    struct service_event event = { .type = SERVICE_STOP };

    if (!g_rpc_server.initialized) {
        return;
    }

    LOG_RD("[DBG] NP: Shutting down RPC service\r\n");
    g_rpc_server.initialized = false;
    if (g_rpc_server.rpc_server.ept.rdev) { rpmsg_destroy_ept(&g_rpc_server.rpc_server.ept); }
    service_event_call(&event);

    for (int i = 0; i < MAX_SOCKETS; i++) {
        struct socket_entry *entry = &g_rpc_server.sockets[i];
        if (entry->is_used && entry->real_fd >= 0) {
            lwip_close(entry->real_fd);
        }
    }
    event.type = SERVICE_EXIT;
    service_event_call(&event);
    vTaskDelete(g_rpc_server.task);
    vQueueDelete(g_rpc_server.queue);
    while (g_rpc_server.workers) {
        struct rpc_work *work = g_rpc_server.workers;
        vTaskDelete(work->task);
        g_rpc_server.workers = work->next;
        kfree(work);
    }
    for (int i = 0; i < MAX_SOCKETS; i++) {
        struct socket_entry *entry = &g_rpc_server.sockets[i];
        if (entry->lock) {
            vSemaphoreDelete(entry->lock);
        }
    }
    memset(&g_rpc_server, 0, sizeof(g_rpc_server));
    LOG_RD("[DBG] NP: RPC service shutdown completed\r\n");
}

bool rpc_service_is_initialized(void)
{
    return g_rpc_server.initialized;
}
