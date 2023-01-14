/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-01-10 23:45:59
 * @LastEditTime: 2020-06-05 17:13:00
 * @Description: the code belongs to jiejie, please keep the author information
 * and source code according to the license.
 */
#include "platform_net_socket.h"
#include "mqtt_error.h"

int platform_net_socket_connect(const char* host, const char* port, int proto) {
    int fd, ret = MQTT_SOCKET_UNKNOWN_HOST_ERROR;
    struct addrinfo hints, *addr_list, *cur;

    /* Do name resolution with both IPv6 and IPv4 */
    __platform_memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype =
        (proto == PLATFORM_NET_PROTO_UDP) ? SOCK_DGRAM : SOCK_STREAM;
    hints.ai_protocol =
        (proto == PLATFORM_NET_PROTO_UDP) ? IPPROTO_UDP : IPPROTO_TCP;

    if (__platform_getaddrinfo(host, port, &hints, &addr_list) != 0) {
        return ret;
    }

    for (cur = addr_list; cur != NULL; cur = cur->ai_next) {
        fd = __platform_socket(cur->ai_family, cur->ai_socktype,
                               cur->ai_protocol);
        if (fd < 0) {
            ret = MQTT_SOCKET_FAILED_ERROR;
            continue;
        }

        if (__platform_connect(fd, cur->ai_addr, cur->ai_addrlen) == 0) {
            ret = fd;
            break;
        }

        __platform_close(fd);
        ret = MQTT_CONNECT_FAILED_ERROR;
    }

    __platform_freeaddrinfo(addr_list);
    return ret;
}

int platform_net_socket_recv(int fd, void* buf, size_t len, int flags) {
    return __platform_recv(fd, buf, len, flags);
}

int platform_net_socket_recv_timeout(int fd,
                                     unsigned char* buf,
                                     int len,
                                     int timeout) {
    int nread;
    int nleft = len;
    unsigned char* ptr;
    ptr = buf;

    struct timeval tv = {timeout / 1000, (timeout % 1000) * 1000};

    if (tv.tv_sec < 0 || (tv.tv_sec == 0 && tv.tv_usec <= 0)) {
        tv.tv_sec = 0;
        tv.tv_usec = 100;
    }

    platform_net_socket_setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv,
                                   sizeof(struct timeval));

    while (nleft > 0) {
        nread = platform_net_socket_recv(fd, ptr, nleft, 0);
        if (nread < 0) {
            return -1;
        } else if (nread == 0) {
            break;
        }

        nleft -= nread;
        ptr += nread;
    }
    return len - nleft;
}

int platform_net_socket_write(int fd, void* buf, size_t len) {
    return __platform_write(fd, buf, len);
}

int platform_net_socket_write_timeout(int fd,
                                      unsigned char* buf,
                                      int len,
                                      int timeout) {
    struct timeval tv = {timeout / 1000, (timeout % 1000) * 1000};

    if (tv.tv_sec < 0 || (tv.tv_sec == 0 && tv.tv_usec <= 0)) {
        tv.tv_sec = 0;
        tv.tv_usec = 100;
    }

    __platform_setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv,
                          sizeof(struct timeval));

    return __platform_write(fd, buf, len);
}

int platform_net_socket_close(int fd) {
    return __platform_close(fd);
}

PIKA_WEAK int platform_net_socket_set_block(int fd) {
#ifdef __linux
    return __platform_fcntl(
        fd, F_SETFL, __platform_fcntl(fd, F_GETFL, F_GETFL) & ~O_NONBLOCK);
#elif PIKA_LWIP_ENABLE
    unsigned long mode = 0;
    return ioctlsocket(fd, FIONBIO, &mode);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int platform_net_socket_set_nonblock(int fd) {
#ifdef __linux
    return __platform_fcntl(
        fd, F_SETFL, __platform_fcntl(fd, F_GETFL, F_GETFL) | O_NONBLOCK);
#elif PIKA_LWIP_ENABLE
    unsigned long mode = 1;
    return ioctlsocket(fd, FIONBIO, &mode);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int platform_net_socket_setsockopt(int fd,
                                             int level,
                                             int optname,
                                             const void* optval,
                                             socklen_t optlen) {
    return __platform_setsockopt(fd, level, optname, optval, optlen);
}
