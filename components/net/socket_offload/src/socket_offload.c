#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <FreeRTOS.h>
#include <task.h>
#include "socket_offload.h"
#include "ipc_socket.h"
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOCKET_OFFLOAD_LOG_LEVEL
#include <log.h>

/* Socket API adapter for the RPC client. */
static int begin_call(void)
{
    if (!ipc_socket_is_initialized()) { errno = ENOTCONN; return -1; }
    return 0;
}

static int end_call(int ret)
{
    if (ret < 0) { errno = -ret; return -1; }
    return ret;
}

static bool socket_offload_is_supported(int family, int type, int proto)
{
    if (family != AF_INET) {
        return false;
    }
    if (type != SOCK_STREAM && type != SOCK_DGRAM) {
        return false;
    }
    if (proto != IPPROTO_TCP && proto != IPPROTO_UDP && proto != 0) {
        return false;
    }
    if (type == SOCK_STREAM && proto == IPPROTO_UDP) {
        return false;
    }
    return true;
}

int socket_offload_socket(int domain, int type, int protocol)
{
    int fd;

    if (!socket_offload_is_supported(domain, type, protocol)) {
        LOG_RE("[ERR] socket_offload_socket: Unsupported socket parameters (family=%d, type=%d, proto=%d)\r\n",
               domain, type, protocol);
        errno = EAFNOSUPPORT;
        return -1;
    }
    if (begin_call()) { return -1; }
    fd = ipc_socket_create(domain, type, protocol);
    if (fd < 0) {
        LOG_RE("[ERR] socket_offload_socket: Failed to create NP socket (ret=%d, errno=%d)\r\n", fd, -fd);
    }
    return end_call(fd);
}

ssize_t socket_offload_recv(int fd, void *buf, size_t len, int flags)
{
    ssize_t ret;

    if (fd < 0) {
        LOG_RE("[ERR] socket_offload_recv: Invalid socket object (np_sockfd=%d)\r\n", fd);
        errno = EBADF;
        return -1;
    }
    if (!buf || len == 0) {
        LOG_RE("[ERR] socket_offload_recv: Invalid buffer (buf=%p, len=%zu)\r\n", buf, len);
        errno = EINVAL;
        return -1;
    }

    ret = socket_offload_recvfrom(fd, buf, len, flags, NULL, NULL);
    if (ret < 0) {
        LOG_RW("[WRN] socket_offload_recv: Failed to read from NP socket %d (ret=%zd, errno=%d)\r\n", fd, ret, errno);
    }
    return ret;
}

ssize_t socket_offload_send(int fd, const void *buf, size_t len, int flags)
{
    ssize_t ret;

    if (fd < 0) {
        LOG_RE("[ERR] socket_offload_send: Invalid socket object (np_sockfd=%d)\r\n", fd);
        errno = EBADF;
        return -1;
    }
    if (!buf || len == 0) {
        LOG_RE("[ERR] socket_offload_send: Invalid buffer (buf=%p, len=%zu)\r\n", buf, len);
        errno = EINVAL;
        return -1;
    }

    ret = socket_offload_sendto(fd, buf, len, flags, NULL, 0);
    if (ret < 0) {
        if (errno == ENOMEM || errno == ENOBUFS || errno == EAGAIN) {
            LOG_RW("[WRN] socket_offload_send: Failed to write to NP socket %d (ret=%zd, errno=%d)\r\n", fd, ret, errno);
        } else {
            LOG_RE("[ERR] socket_offload_send: Failed to write to NP socket %d (ret=%zd, errno=%d)\r\n", fd, ret, errno);
        }
    }
    return ret;
}

int socket_offload_close(int fd)
{
    int ret;

    if (fd < 0) {
        LOG_RE("[ERR] socket_offload_close: Invalid socket object (np_sockfd=%d)\r\n", fd);
        errno = EBADF;
        return -1;
    }
    if (begin_call()) { return -1; }
    ret = ipc_socket_close(fd);
    if (ret < 0) {
        LOG_RE("[ERR] socket_offload_close: Failed to close NP socket %d (ret=%d, errno=%d)\r\n", fd, ret, -ret);
    }
    return end_call(ret);
}

int socket_offload_bind(int fd, const struct sockaddr *addr, socklen_t len)
{
    int ret;

    if (fd < 0) {
        LOG_RE("[ERR] socket_offload_bind: Invalid socket object (np_sockfd=%d)\r\n", fd);
        errno = EBADF;
        return -1;
    }
    if (!addr || len == 0) {
        LOG_RE("[ERR] socket_offload_bind: Invalid address (addr=%p, addrlen=%d)\r\n", addr, (int)len);
        errno = EINVAL;
        return -1;
    }
    if (begin_call()) { return -1; }
    ret = ipc_socket_bind(fd, addr, len);
    if (ret < 0) {
        LOG_RE("[ERR] socket_offload_bind: Failed to bind NP socket %d (ret=%d, errno=%d)\r\n", fd, ret, -ret);
    }
    return end_call(ret);
}

int socket_offload_connect(int fd, const struct sockaddr *addr, socklen_t len)
{
    int ret;

    if (fd < 0) {
        LOG_RE("[ERR] socket_offload_connect: Invalid socket object (np_sockfd=%d)\r\n", fd);
        errno = EBADF;
        return -1;
    }
    if (!addr || len == 0) {
        LOG_RE("[ERR] socket_offload_connect: Invalid address (addr=%p, addrlen=%d)\r\n", addr, (int)len);
        errno = EINVAL;
        return -1;
    }
    if (begin_call()) { return -1; }
    ret = ipc_socket_connect(fd, addr, len);
    if (ret < 0 && ret != -EINPROGRESS && ret != -EALREADY && ret != -EWOULDBLOCK) {
        LOG_RE("[ERR] socket_offload_connect: Failed to connect NP socket %d (ret=%d, errno=%d)\r\n", fd, ret, -ret);
    }
    return end_call(ret);
}

int socket_offload_ioctl(int fd, long cmd, void *argp)
{
    int ret;

    if (fd < 0) { errno = EBADF; return -1; }
    if (cmd != FIONBIO || !argp) { errno = EINVAL; return -1; }
    if (begin_call()) { return -1; }
    ret = ipc_socket_ioctl(fd, cmd, *(unsigned long *)argp);
    return end_call(ret);
}

static int socket_offload_poll(int fd, short events, short *revents)
{
    int ret;

    if (fd < 0) { errno = EBADF; return -1; }
    if (!revents) { errno = EINVAL; return -1; }
    if (begin_call()) { return -1; }
    ret = ipc_socket_poll(fd, events, revents);
    return end_call(ret);
}

int socket_offload_select(int maxfdp1, fd_set *readset, fd_set *writeset,
                          fd_set *exceptset, struct timeval *timeout)
{
    fd_set requested_read, requested_write, requested_except;
    TickType_t start = xTaskGetTickCount();
    TickType_t wait_ticks = 0;
    bool timed = timeout != NULL;
    int fd;

    if (maxfdp1 < 0 || maxfdp1 > FD_SETSIZE ||
        (timed && (timeout->tv_sec < 0 || timeout->tv_usec < 0 || timeout->tv_usec >= 1000000))) {
        errno = EINVAL;
        return -1;
    }
    if (timed) {
        uint64_t us = (uint64_t)timeout->tv_sec * 1000000ULL + timeout->tv_usec;
        uint64_t ticks = (us + portTICK_PERIOD_MS * 1000ULL - 1) /
                         (portTICK_PERIOD_MS * 1000ULL);
        wait_ticks = (TickType_t)(ticks > portMAX_DELAY - 1 ? portMAX_DELAY - 1 : ticks);
    }
    if (readset) { requested_read = *readset; }
    if (writeset) { requested_write = *writeset; }
    if (exceptset) { requested_except = *exceptset; }

    for (;;) {
        int ready = 0;

        if (readset) { FD_ZERO(readset); }
        if (writeset) { FD_ZERO(writeset); }
        if (exceptset) { FD_ZERO(exceptset); }

        for (fd = 0; fd < maxfdp1; fd++) {
            bool want_read = readset && FD_ISSET(fd, &requested_read);
            bool want_write = writeset && FD_ISSET(fd, &requested_write);
            bool want_except = exceptset && FD_ISSET(fd, &requested_except);
            short events, revents = 0;

            if (!want_read && !want_write && !want_except) { continue; }
            events = (want_read ? POLLIN : 0) | (want_write ? POLLOUT : 0) |
                     (want_except ? POLLPRI : 0);
            if (socket_offload_poll(fd, events, &revents) < 0) { return -1; }
            if (revents & POLLNVAL) { errno = EBADF; return -1; }
            if (want_read && (revents & (POLLIN | POLLERR | POLLHUP))) {
                FD_SET(fd, readset);
                ready++;
            }
            if (want_write && (revents & (POLLOUT | POLLERR | POLLHUP))) {
                FD_SET(fd, writeset);
                ready++;
            }
            if (want_except && (revents & (POLLPRI | POLLERR))) {
                FD_SET(fd, exceptset);
                ready++;
            }
        }
        if (ready || (timed && (TickType_t)(xTaskGetTickCount() - start) >= wait_ticks)) {
            return ready;
        }
        vTaskDelay(pdMS_TO_TICKS(10) ? pdMS_TO_TICKS(10) : 1);
    }
}

int socket_offload_listen(int fd, int backlog)
{
    int ret;

    if (fd < 0) {
        LOG_RE("[ERR] socket_offload_listen: Invalid socket object (np_sockfd=%d)\r\n", fd);
        errno = EBADF;
        return -1;
    }
    if (backlog < 0) {
        LOG_RE("[ERR] socket_offload_listen: Invalid backlog=%d\r\n", backlog);
        errno = EINVAL;
        return -1;
    }
    if (begin_call()) { return -1; }
    ret = ipc_socket_listen(fd, backlog);
    if (ret < 0) {
        LOG_RE("[ERR] socket_offload_listen: Failed to listen on NP socket %d (ret=%d, errno=%d)\r\n", fd, ret, -ret);
    }
    return end_call(ret);
}

int socket_offload_accept(int fd, struct sockaddr *addr, socklen_t *len)
{
    if (begin_call()) { return -1; }
    return end_call(ipc_socket_accept(fd, addr, len));
}

ssize_t socket_offload_sendto(int fd, const void *buf, size_t len, int flags,
                            const struct sockaddr *addr, socklen_t addrlen)
{
    if (!buf || len == 0) { errno = EINVAL; return -1; }
    if (begin_call()) { return -1; }
    return end_call(ipc_socket_sendto(fd, buf, len, flags, addr, addrlen));
}

ssize_t socket_offload_recvfrom(int fd, void *buf, size_t len, int flags,
                              struct sockaddr *addr, socklen_t *addrlen)
{
    if (!buf || len == 0) { errno = EINVAL; return -1; }
    if (begin_call()) { return -1; }
    return end_call(ipc_socket_recvfrom(fd, buf, len, flags, addr, addrlen));
}

int socket_offload_getsockopt(int fd, int level, int opt, void *value, socklen_t *len)
{
    if (!value || !len) { errno = EINVAL; return -1; }
    if (begin_call()) { return -1; }
    return end_call(ipc_socket_getsockopt(fd, level, opt, value, len));
}

int socket_offload_setsockopt(int fd, int level, int opt, const void *value, socklen_t len)
{
    if (!value || len == 0) { errno = EINVAL; return -1; }
    if (begin_call()) { return -1; }
    return end_call(ipc_socket_setsockopt(fd, level, opt, value, len));
}

int socket_offload_getsockname(int fd, struct sockaddr *addr, socklen_t *len)
{
    if (!addr || !len) { errno = EINVAL; return -1; }
    if (begin_call()) { return -1; }
    return end_call(ipc_socket_getsockname(fd, addr, len));
}

int socket_offload_getpeername(int fd, struct sockaddr *addr, socklen_t *len)
{
    if (!addr || !len) { errno = EINVAL; return -1; }
    if (begin_call()) { return -1; }
    return end_call(ipc_socket_getpeername(fd, addr, len));
}

int socket_offload_shutdown(int fd, int how)
{
    if (begin_call()) { return -1; }
    return end_call(ipc_socket_shutdown(fd, how));
}

int socket_offload_init(void)
{
    int ret;

    if (!rpmsg_is_initialized()) {
        LOG_RE("[ERR] socket_offload_init: RPMsg not initialized, cannot initialize socket offload\r\n");
        return -ENODEV;
    }

    ret = ipc_socket_init();
    if (ret < 0) {
        LOG_RE("[ERR] socket_offload_init: Failed to initialize IPC socket layer: %d\r\n", ret);
        return ret;
    }

    return 0;
}
