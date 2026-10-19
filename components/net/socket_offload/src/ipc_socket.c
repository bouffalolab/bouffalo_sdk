#include <errno.h>
#include <limits.h>
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
#include "ipc_socket.h"
extern struct rpmsg_device *rpmsg_service_get_device(void);

#define RPC_MAX_SOCKETS 64
#define RPC_QUEUE_LEN   8

/* The caller owns this context until completion or cancellation is acknowledged. */
struct rpc_ctx {
    SemaphoreHandle_t sem;
    uint32_t sequence;
    uint32_t rpc_id;
    int remote_fd;
    int result;
    size_t data_len;
    size_t received;
    uint8_t *recv_data;
    size_t recv_capacity;
    uint8_t addr[sizeof(struct sockaddr_storage)];
    const void *request_data;
    size_t request_size;
    const void *send_data;
    size_t send_size;
};

enum rpc_event_type {
    RPC_SUBMIT, RPC_CANCEL, RPC_RESPONSE,
    RPC_ALLOC_FD, RPC_FREE_FD, RPC_STORE_FD, RPC_STOP
};

struct rpc_event {
    enum rpc_event_type type;
    struct rpc_ctx *ctx;
    void *data;
    size_t len;
    int fd;
    int value;
    SemaphoreHandle_t done;
};

static struct rpmsg_rpc_clt g_rpc_client;
static struct rpmsg_device *g_rpmsg_dev;
static bool g_rpc_initialized;
static QueueHandle_t rpc_queue;
static StaticQueue_t rpc_queue_storage;
static uint8_t rpc_queue_items[RPC_QUEUE_LEN * sizeof(struct rpc_event *)];
static TaskHandle_t rpc_task;
static StaticTask_t rpc_task_storage;
static StackType_t rpc_stack[512];

/* Only rpc_client_task accesses the request table and descriptor mappings. */
static struct rpc_ctx **rpc_pending;
static size_t rpc_capacity;
static uint32_t rpc_sequence;
static int socket_fd_table[RPC_MAX_SOCKETS];
static uint32_t socket_close_sequence[RPC_MAX_SOCKETS];

static void rpc_event_call(struct rpc_event *event)
{
    StaticSemaphore_t storage;

    event->done = xSemaphoreCreateBinaryStatic(&storage);
    xQueueSend(rpc_queue, &event, portMAX_DELAY);
    xSemaphoreTake(event->done, portMAX_DELAY);
    vSemaphoreDelete(event->done);
}

static int alloc_socket_fd(void)
{
    struct rpc_event event = { .type = RPC_ALLOC_FD };
    rpc_event_call(&event);
    return event.value;
}

static void free_socket_fd(int fd)
{
    struct rpc_event event = { .type = RPC_FREE_FD, .fd = fd };
    rpc_event_call(&event);
}

static void store_remote_sockfd(int fd, int remote_fd)
{
    struct rpc_event event = { .type = RPC_STORE_FD, .fd = fd, .value = remote_fd };
    rpc_event_call(&event);
}

/* One attempt per frame; the queue task exclusively owns the frame. */
static int send_rpc_frame(uint32_t sequence, uint32_t rpc_id, const void *params,
                          size_t params_len, const void *payload, size_t payload_len,
                          int remote_fd)
{
    static struct rpmsg_rpc_request frame;
    struct rpc_request_fragment *request = (struct rpc_request_fragment *)frame.params;
    size_t length = MAX_FUNC_ID_LEN + offsetof(struct rpc_request_fragment, data) +
                    params_len + payload_len;
    int ret;

    frame.id = RPC_SOCKET_REQUEST;
    request->sequence = sequence;
    request->rpc_id = rpc_id;
    request->total_len = params_len + payload_len;
    request->offset = 0;
    memcpy(request->data, params, params_len);
    if (rpc_id != RPC_SOCKET_CREATE) { memcpy(request->data, &remote_fd, sizeof(remote_fd)); }
    if (payload_len) { memcpy(request->data + params_len, payload, payload_len); }
    ret = rpmsg_trysend(&g_rpc_client.ept, &frame, length);
    if (ret == RPMSG_ERR_NO_BUFF) { return -ENOBUFS; }
    return ret == (int)length ? 0 : (ret < 0 ? ret : -EPROTO);
}

static void submit_rpc_request(struct rpc_ctx *ctx)
{
    size_t slot;

    /* Resolve the caller's descriptor while submitting, before building the wire frame. */
    if (!ctx->sequence && ctx->rpc_id != RPC_SOCKET_CREATE) {
        int fd;
        memcpy(&fd, ctx->request_data, sizeof(fd));
        if (fd < 0 || fd >= RPC_MAX_SOCKETS || socket_fd_table[fd] < 0) {
            ctx->result = -EBADF;
            xSemaphoreGive(ctx->sem);
            return;
        }
        ctx->remote_fd = socket_fd_table[fd];
    }

    for (slot = 0; slot < rpc_capacity && rpc_pending[slot]; slot++) {}
    if (slot == rpc_capacity) {
        size_t capacity = rpc_capacity ? rpc_capacity * 2 : 4;
        struct rpc_ctx **pending = kmalloc(capacity * sizeof(*pending), MM_FLAG_HEAP_OCRAM_0);

        if (!pending) {
            ctx->result = -ENOMEM;
            xSemaphoreGive(ctx->sem);
            return;
        }
        memset(pending, 0, capacity * sizeof(*pending));
        if (rpc_capacity) { memcpy(pending, rpc_pending, rpc_capacity * sizeof(*pending)); }
        kfree(rpc_pending);
        rpc_pending = pending;
        rpc_capacity = capacity;
    }
    ctx->sequence = ++rpc_sequence;
    ctx->received = 0;
    rpc_pending[slot] = ctx;
    ctx->result = send_rpc_frame(ctx->sequence, ctx->rpc_id, ctx->request_data,
                                 ctx->request_size, ctx->send_data, ctx->send_size, ctx->remote_fd);
    if (ctx->result < 0) {
        rpc_pending[slot] = NULL;
        xSemaphoreGive(ctx->sem);
    }
}

static void receive_rpc_response(struct rpc_event *event)
{
    struct rpc_response response;
    struct rpc_ctx *ctx;
    size_t slot, payload_len;
    const size_t header_len = offsetof(struct rpc_response, data);
    const size_t rpc_header_len = offsetof(struct rpmsg_rpc_answer, params);
    const uint8_t *fragment = (const uint8_t *)event->data + header_len;
    size_t fragment_len;

    if (event->len < rpc_header_len + header_len) { return; }
    memcpy(&response, event->data, header_len);
    fragment_len = event->len - rpc_header_len - header_len;
    for (slot = 0; slot < rpc_capacity; slot++) {
        if (rpc_pending[slot] && rpc_pending[slot]->sequence == response.sequence) { break; }
    }
    if (slot == rpc_capacity) {
        for (int fd = 0; fd < RPC_MAX_SOCKETS; fd++) {
            if (socket_close_sequence[fd] == response.sequence) {
                if (response.result == 0 || response.errno_val == EBADF) { socket_fd_table[fd] = -1; }
                socket_close_sequence[fd] = 0;
            }
        }
        const struct rpmsg_rpc_answer *answer =
            (const struct rpmsg_rpc_answer *)((const uint8_t *)event->data - rpc_header_len);
        if ((answer->id == RPC_SOCKET_CREATE || answer->id == RPC_SOCKET_ACCEPT) &&
            response.result >= 0) {
            int ret = send_rpc_frame(++rpc_sequence, RPC_SOCKET_CLOSE, &response.result,
                                      sizeof(response.result), NULL, 0, response.result);
            if (ret < 0) { LOG_RW("[WRN] Failed to close orphaned socket %d\r\n", response.result); }
        }
        return;
    }
    ctx = rpc_pending[slot];
    if (ctx->received == 0) {
        ctx->result = response.result < 0 ?
                      (response.errno_val > 0 ? -response.errno_val : -EIO) : response.result;
        ctx->data_len = response.data_len;
    }
    payload_len = ctx->rpc_id == RPC_SOCKET_RECVFROM && ctx->result >= 0 ?
                  (size_t)ctx->result : ctx->data_len;
    if (payload_len > ctx->data_len || payload_len > ctx->recv_capacity ||
        ctx->data_len - payload_len > sizeof(ctx->addr) ||
        fragment_len > ctx->data_len - ctx->received) {
        ctx->result = -EPROTO;
    } else {
        size_t offset = ctx->received;
        size_t remaining = fragment_len;
        if (offset < payload_len) {
            size_t copy_len = LWIP_MIN(remaining, payload_len - offset);
            memcpy(ctx->recv_data + offset, fragment, copy_len);
            offset += copy_len;
            fragment += copy_len;
            remaining -= copy_len;
        }
        if (remaining) { memcpy(ctx->addr + offset - payload_len, fragment, remaining); }
        ctx->received += fragment_len;
        if (ctx->received != ctx->data_len) { return; }
    }
    rpc_pending[slot] = NULL;
    xSemaphoreGive(ctx->sem);
}

static void rpc_client_task(void *arg)
{
    struct rpc_event *event;

    (void)arg;
    for (;;) {
        xQueueReceive(rpc_queue, &event, portMAX_DELAY);
        switch (event->type) {
        case RPC_SUBMIT:
            submit_rpc_request(event->ctx);
            /* Completion can release the caller's context and submit event. */
            continue;
        case RPC_CANCEL:
            for (size_t i = 0; i < rpc_capacity; i++) {
                if (rpc_pending[i] == event->ctx) {
                    if (event->ctx->rpc_id == RPC_SOCKET_CLOSE) {
                        int fd;
                        memcpy(&fd, event->ctx->request_data, sizeof(fd));
                        socket_close_sequence[fd] = event->ctx->sequence;
                    }
                    rpc_pending[i] = NULL;
                    event->ctx->result = -ETIMEDOUT;
                    break;
                }
            }
            break;
        case RPC_RESPONSE:
            receive_rpc_response(event);
            break;
        case RPC_ALLOC_FD:
            event->value = -EMFILE;
            for (int fd = 0; fd < RPC_MAX_SOCKETS; fd++) {
                if (socket_fd_table[fd] == -1) {
                    socket_fd_table[fd] = fd;
                    event->value = fd;
                    break;
                }
            }
            break;
        case RPC_FREE_FD:
            if (event->fd >= 0 && event->fd < RPC_MAX_SOCKETS) {
                socket_fd_table[event->fd] = -1;
                socket_close_sequence[event->fd] = 0;
            }
            break;
        case RPC_STORE_FD:
            socket_fd_table[event->fd] = event->value;
            break;
        case RPC_STOP:
            kfree(rpc_pending);
            rpc_pending = NULL;
            rpc_capacity = 0;
            xSemaphoreGive(event->done);
            vTaskSuspend(NULL);
            break;
        }
        xSemaphoreGive(event->done);
    }
}

void rpc_callback(struct rpmsg_rpc_clt *rpc, int status, void *data, size_t len)
{
    struct rpc_event event = { .type = RPC_RESPONSE, .data = data, .len = len };
    (void)rpc;
    if (data && status == RPMSG_RPC_OK) { rpc_event_call(&event); }
}

/* Only the calling thread waits for the remote socket operation. */
static int send_rpc_request(struct rpc_ctx *ctx, uint32_t rpc_id, const void *request_data,
                            size_t request_size, const void *send_data, size_t send_size)
{
    struct rpc_ctx local_ctx = {0};
    StaticSemaphore_t storage;
    bool sending = send_size != 0;
    bool blocking = rpc_id == RPC_SOCKET_RECV || rpc_id == RPC_SOCKET_RECVFROM ||
                    rpc_id == RPC_SOCKET_SEND || rpc_id == RPC_SOCKET_SENDTO ||
                    rpc_id == RPC_SOCKET_CONNECT || rpc_id == RPC_SOCKET_ACCEPT ||
                    rpc_id == RPC_SOCKET_CLOSE;
    TickType_t timeout = blocking ?
        (RPC_SOCKET_TIMEOUT_MS ? pdMS_TO_TICKS(RPC_SOCKET_TIMEOUT_MS) : portMAX_DELAY) :
        pdMS_TO_TICKS(RPC_TIMEOUT_MS);
    size_t sent = 0;
    int ret;

    if (request_size > RPC_REQUEST_DATA_LEN || send_size > INT_MAX) { return -EMSGSIZE; }
    if (!g_rpc_initialized || !is_rpmsg_ept_ready(&g_rpc_client.ept)) { return -ENOTCONN; }
    if (!ctx) { ctx = &local_ctx; }
    ctx->sem = xSemaphoreCreateBinaryStatic(&storage);
    ctx->sequence = 0;
    ctx->rpc_id = rpc_id;
    ctx->request_data = request_data;
    ctx->request_size = request_size;
    do {
        struct rpc_event event = { .type = RPC_SUBMIT, .ctx = ctx };
        struct rpc_event *queued = &event;
        size_t data_len = LWIP_MIN(send_size, RPC_REQUEST_DATA_LEN - request_size);

        xSemaphoreTake(ctx->sem, 0);
        ctx->send_data = send_data;
        ctx->send_size = data_len;
        xQueueSend(rpc_queue, &queued, portMAX_DELAY);
        if (xSemaphoreTake(ctx->sem, timeout) != pdTRUE) {
            /* Acknowledge removal before the caller's buffers leave scope. */
            struct rpc_event cancel = { .type = RPC_CANCEL, .ctx = ctx };
            rpc_event_call(&cancel);
        }
        ret = ctx->result;
        if (ret < 0 || !sending) { break; }
        sent += ret;
        if ((size_t)ret != data_len) { break; }
        send_data = (const uint8_t *)send_data + data_len;
        send_size -= data_len;
    } while (send_size);
    vSemaphoreDelete(ctx->sem);
    return sending && sent ? (int)sent : ret;
}

/* RPC client service table */
static const struct rpmsg_rpc_client_services socket_rpc_services[] = {
    { .id = RPC_SOCKET_CREATE, .cb = rpc_callback },
    { .id = RPC_SOCKET_CLOSE, .cb = rpc_callback },
    { .id = RPC_SOCKET_BIND, .cb = rpc_callback },
    { .id = RPC_SOCKET_CONNECT, .cb = rpc_callback },
    { .id = RPC_SOCKET_LISTEN, .cb = rpc_callback },
    { .id = RPC_SOCKET_ACCEPT, .cb = rpc_callback },
    { .id = RPC_SOCKET_SEND, .cb = rpc_callback },
    { .id = RPC_SOCKET_SENDTO, .cb = rpc_callback },
    { .id = RPC_SOCKET_RECV, .cb = rpc_callback },
    { .id = RPC_SOCKET_RECVFROM, .cb = rpc_callback },
    { .id = RPC_SOCKET_SETSOCKOPT, .cb = rpc_callback },
    { .id = RPC_SOCKET_GETSOCKOPT, .cb = rpc_callback },
    { .id = RPC_SOCKET_GETSOCKNAME, .cb = rpc_callback },
    { .id = RPC_SOCKET_GETPEERNAME, .cb = rpc_callback },
    { .id = RPC_SOCKET_SHUTDOWN, .cb = rpc_callback },
    { .id = RPC_SOCKET_IOCTL, .cb = rpc_callback },
    { .id = RPC_SOCKET_POLL, .cb = rpc_callback },
};

void rpc_client_shutdown(void)
{
    /* Retained for source compatibility. */
}

int ipc_socket_init(void)
{
    return 0;
}

int ipc_socket_create(int family, int type, int proto)
{
    struct rpc_socket_create params;
    int local_fd, remote_fd, ret;

    /* Allocate socket client socket FD */
    local_fd = alloc_socket_fd();
    if (local_fd < 0) {
        return local_fd;
    }

    /* Prepare parameters */
    params.family = family;
    params.type = type;
    params.proto = proto;

    /* Send RPC request */
    ret = send_rpc_request(NULL, RPC_SOCKET_CREATE, &params, sizeof(params), NULL, 0);
    if (ret < 0) {
        free_socket_fd(local_fd);
        return ret;
    }

    remote_fd = ret;

    /* Store socket service socket FD */
    store_remote_sockfd(local_fd, remote_fd);
    return local_fd;
}

int ipc_socket_close(int sockfd)
{
    int ret;

    if (sockfd < 0 || sockfd >= RPC_MAX_SOCKETS) {
        return -EBADF;
    }

    /* Send RPC request */
    ret = send_rpc_request(NULL, RPC_SOCKET_CLOSE, &sockfd, sizeof(sockfd), NULL, 0);

    if (ret == 0 || ret == -EBADF) {
        free_socket_fd(sockfd);
    }

    return ret;
}

int ipc_socket_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    struct rpc_socket_bind *params;
    int ret;
    size_t params_size;

    if (sockfd < 0 || sockfd >= RPC_MAX_SOCKETS || !addr || addrlen != sizeof(struct sockaddr_in) || addr->sa_family != AF_INET) {
        return -EINVAL;
    }

    params_size = sizeof(*params) + addrlen - sizeof(params->addr);
    params = pvPortMalloc(params_size);
    if (!params) {
        return -ENOMEM;
    }

    params->sockfd = sockfd;
    params->addrlen = addrlen;
    memcpy(&params->addr, addr, addrlen);

    ret = send_rpc_request(NULL, RPC_SOCKET_BIND, params, params_size, NULL, 0);

    vPortFree(params);
    return ret;
}

int ipc_socket_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    struct rpc_socket_connect *params;
    int ret;
    size_t params_size;

    if (sockfd < 0 || sockfd >= RPC_MAX_SOCKETS || !addr || addrlen != sizeof(struct sockaddr_in) || addr->sa_family != AF_INET) {
        return -EINVAL;
    }

    params_size = sizeof(*params) + addrlen - sizeof(params->addr);
    params = pvPortMalloc(params_size);
    if (!params) {
        return -ENOMEM;
    }

    params->sockfd = sockfd;
    params->addrlen = addrlen;
    memcpy(&params->addr, addr, addrlen);

    ret = send_rpc_request(NULL, RPC_SOCKET_CONNECT, params, params_size, NULL, 0);

    vPortFree(params);
    return ret;
}

int ipc_socket_ioctl(int sockfd, long cmd, unsigned long value)
{
    struct rpc_socket_ioctl params;

    params.sockfd = sockfd;
    params.cmd = cmd;
    params.value = value;
    return send_rpc_request(NULL, RPC_SOCKET_IOCTL, &params, sizeof(params), NULL, 0);
}

int ipc_socket_poll(int sockfd, short events, short *revents)
{
    struct rpc_ctx ctx = { .recv_data = (uint8_t *)revents, .recv_capacity = sizeof(*revents) };
    struct rpc_socket_poll params;
    int ret;

    params.sockfd = sockfd;
    params.events = events;
    ret = send_rpc_request(&ctx, RPC_SOCKET_POLL, &params, sizeof(params), NULL, 0);
    if (ret < 0) {
        return ret;
    }
    if (ctx.data_len != sizeof(*revents)) {
        ret = -EPROTO;
    }
    return ret;
}

int ipc_socket_listen(int sockfd, int backlog)
{
    struct rpc_socket_listen params;
    int ret;

    if (sockfd < 0 || sockfd >= RPC_MAX_SOCKETS) {
        return -EINVAL;
    }

    params.sockfd = sockfd;
    params.backlog = backlog;

    ret = send_rpc_request(NULL, RPC_SOCKET_LISTEN, &params, sizeof(params), NULL, 0);
    return ret;
}

int ipc_socket_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    struct rpc_socket_accept params;
    int new_remote_sockfd, new_local_fd, ret;

    if (sockfd < 0 || sockfd >= RPC_MAX_SOCKETS) {
        return -EINVAL;
    }

    /* Allocate new socket client socket FD */
    new_local_fd = alloc_socket_fd();
    if (new_local_fd < 0) {
        return new_local_fd;
    }

    params.sockfd = sockfd;
    params.addrlen = (addr && addrlen) ? *addrlen : 0;

    ret = send_rpc_request(NULL, RPC_SOCKET_ACCEPT, &params, sizeof(params), NULL, 0);
    if (ret < 0) {
        free_socket_fd(new_local_fd);
        return ret;
    }

    new_remote_sockfd = ret;

    /* Store the accepted socket's remote FD. */
    store_remote_sockfd(new_local_fd, new_remote_sockfd);

    if (addr && addrlen) {
        *addrlen = 0;
    }

    return new_local_fd;
}

ssize_t ipc_socket_sendto(int sockfd, const void *buf, size_t len, int flags,
                          const struct sockaddr *dest_addr, socklen_t addrlen)
{
    uint8_t params_buffer[offsetof(struct rpc_socket_sendto, data) + sizeof(struct sockaddr_storage)]
        __attribute__((aligned(sizeof(uint32_t)))) = {0};
    struct rpc_socket_sendto *params = (struct rpc_socket_sendto *)params_buffer;
    size_t params_size;

    if (sockfd < 0 || sockfd >= RPC_MAX_SOCKETS || !buf || len == 0) {
        return -EINVAL;
    }

    size_t address_size = dest_addr ? addrlen : 0;
    if (address_size > sizeof(struct sockaddr_storage)) {
        return -EMSGSIZE;
    }
    params_size = offsetof(struct rpc_socket_sendto, data) + address_size;

    params->sockfd = sockfd;
    params->flags = flags;
    params->addrlen = address_size;

    /* Copy destination address if provided */
    if (address_size) {
        memcpy(params->data, dest_addr, address_size);
    }

    return send_rpc_request(NULL, RPC_SOCKET_SENDTO, params, params_size, buf, len);
}

ssize_t ipc_socket_send(int sockfd, const void *buf, size_t len, int flags)
{
    return ipc_socket_sendto(sockfd, buf, len, flags, NULL, 0);
}

ssize_t ipc_socket_recvfrom(int sockfd, void *buf, size_t len, int flags,
                            struct sockaddr *src_addr, socklen_t *addrlen)
{
    struct rpc_ctx ctx = { .recv_data = buf, .recv_capacity = len };
    struct rpc_socket_recvfrom params;
    int ret;

    if (sockfd < 0 || sockfd >= RPC_MAX_SOCKETS || !buf || len == 0) {
        return -EINVAL;
    }

    params.sockfd = sockfd;
    params.flags = flags;
    params.len = len;
    params.addrlen = (src_addr && addrlen) ? *addrlen : 0;

    ret = send_rpc_request(&ctx, RPC_SOCKET_RECVFROM, &params, sizeof(params), NULL, 0);
    if (ret < 0) {
        return ret;
    }

    /* Copy source address if requested */
    if (src_addr && addrlen) {
        socklen_t source_len = ctx.data_len - (size_t)ret;
        memcpy(src_addr, ctx.addr, LWIP_MIN(*addrlen, source_len));
        *addrlen = source_len;
    }

    return ret;
}

ssize_t ipc_socket_recv(int sockfd, void *buf, size_t len, int flags)
{
    return ipc_socket_recvfrom(sockfd, buf, len, flags, NULL, NULL);
}

int ipc_socket_setsockopt(int sockfd, int level, int optname,
                          const void *optval, socklen_t optlen)
{
    struct rpc_socket_setsockopt *params;
    int ret;
    size_t params_size;

    if (sockfd < 0 || sockfd >= RPC_MAX_SOCKETS || !optval || optlen == 0) {
        return -EINVAL;
    }

    if (optlen > RPC_REQUEST_DATA_LEN - sizeof(*params)) {
        return -EMSGSIZE;
    }

    params_size = sizeof(*params) + optlen;
    params = pvPortMalloc(params_size);
    if (!params) {
        return -ENOMEM;
    }

    params->sockfd = sockfd;
    params->level = level;
    params->optname = optname;
    params->optlen = optlen;
    memcpy(params->data, optval, optlen);

    ret = send_rpc_request(NULL, RPC_SOCKET_SETSOCKOPT, params, params_size, NULL, 0);

    vPortFree(params);
    return ret;
}

int ipc_socket_getsockopt(int sockfd, int level, int optname,
                          void *optval, socklen_t *optlen)
{
    struct rpc_ctx ctx = {0};
    struct rpc_socket_getsockopt params;
    int ret;

    if (sockfd < 0 || sockfd >= RPC_MAX_SOCKETS || !optval || !optlen) {
        return -EINVAL;
    }

    params.sockfd = sockfd;
    params.level = level;
    params.optname = optname;
    params.optlen = *optlen;

    ctx.recv_data = optval;
    ctx.recv_capacity = *optlen;
    ret = send_rpc_request(&ctx, RPC_SOCKET_GETSOCKOPT, &params, sizeof(params), NULL, 0);
    if (ret < 0) {
        return ret;
    }

    *optlen = ctx.data_len;

    return 0;
}

int ipc_socket_getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    (void)sockfd;
    (void)addr;
    (void)addrlen;
    return -ENOTSUP;
}

int ipc_socket_getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    (void)sockfd;
    (void)addr;
    (void)addrlen;
    return -ENOTSUP;
}

int ipc_socket_shutdown(int sockfd, int how)
{
    int params[2];
    int ret;

    if (sockfd < 0 || sockfd >= RPC_MAX_SOCKETS) {
        return -EINVAL;
    }

    params[0] = sockfd;
    params[1] = how;

    ret = send_rpc_request(NULL, RPC_SOCKET_SHUTDOWN, &params, sizeof(params), NULL, 0);
    return ret;
}

/* RPC client initialization and management functions */
int rpmsg_and_rpc_init(void)
{
    int ret;
    struct rpmsg_device *rdev;

    if (g_rpc_initialized) {
        return 0;
    }

    rdev = rpmsg_service_get_device();
    if (!rdev) {
        LOG_RE("[ERR] Failed to get RPMsg device\r\n");
        return -ENODEV;
    }

    g_rpmsg_dev = rdev;

    rpc_queue = xQueueCreateStatic(RPC_QUEUE_LEN, sizeof(struct rpc_event *),
                                   rpc_queue_items, &rpc_queue_storage);
    rpc_task = xTaskCreateStatic(rpc_client_task, "socket-client", LWIP_ARRAYSIZE(rpc_stack),
                                NULL, 16, rpc_stack, &rpc_task_storage);

    for (int i = 0; i < RPC_MAX_SOCKETS; i++) {
        socket_fd_table[i] = -1;
        socket_close_sequence[i] = 0;
    }

    ret = rpmsg_rpc_client_init(&g_rpc_client, rdev, NULL,
                               socket_rpc_services, LWIP_ARRAYSIZE(socket_rpc_services));
    if (ret < 0) {
        LOG_RE("[ERR] Failed to initialize RPC client: %d\r\n", ret);
        goto error;
    }

    g_rpc_initialized = true;
    return 0;

error:
    vTaskDelete(rpc_task);
    vQueueDelete(rpc_queue);
    rpc_task = NULL;
    rpc_queue = NULL;
    g_rpmsg_dev = NULL;
    return ret;
}

void rpmsg_and_rpc_shutdown(void)
{
    struct rpc_event event = { .type = RPC_STOP };

    if (!g_rpc_initialized) { return; }
    g_rpc_initialized = false;
    rpmsg_rpc_client_release(&g_rpc_client);
    rpc_event_call(&event);
    vTaskDelete(rpc_task);
    vQueueDelete(rpc_queue);
    rpc_task = NULL;
    rpc_queue = NULL;
    g_rpmsg_dev = NULL;
}

bool rpmsg_is_initialized(void)
{
    return g_rpc_initialized;
}

struct rpmsg_device *get_rpmsg_device(void)
{
    return g_rpmsg_dev;
}

bool ipc_socket_is_initialized(void)
{
    return g_rpc_initialized;
}
