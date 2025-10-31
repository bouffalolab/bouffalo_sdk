/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include "rws_network.h"
#include "sys/time.h"
#include "librws.h"

static int net_read(network_t *n, unsigned char *buf, int count, int timeout_ms)
{
    fd_set fds;
    int lfirst = 1;
    int rc, len = 0, fd;
    struct timeval tv;
    unsigned long long delta;
    struct timeval t1 = {0}, t2 = {0};

    if (!(n && buf && count)) {
        return -1;
    }

    // if (timeout_ms < 3000) {
    //     timeout_ms = 3000;
    // }

    fd = n->fd;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    gettimeofday(&t1, NULL);
    int __attribute__((unused)) err = 0;
    while (len < count) {
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        /* FIXME: patch for select. */
        if (lfirst) {
            lfirst = 0;
        } else {
            gettimeofday(&t2, NULL);
            delta = (t2.tv_sec  - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec) / 1000;
            if (delta >= timeout_ms)
                break;
        }

        rc = select(fd + 1, &fds, NULL, NULL, &tv);
        if (rc < 0) {
            if ((errno == EINTR) || (errno == EAGAIN)) {
                rws_thread_sleep(20);
                continue;
            }
            err = 1;
            return -1;
        } else if (rc == 0) {
            err = 2;
            /* time out */
            break;
        }

        if (!FD_ISSET(fd, &fds)) {
            rws_thread_sleep(20);
            continue;
        }

        rc = recv(fd, buf + len, count - len, 0);
        if (rc < 0) {
            if ((errno == EINTR) || (errno == EAGAIN)) {
                rws_thread_sleep(20);
                continue;
            }
            err = 3;
            return -1;
        } else if (rc == 0) {
            /* the sockfd may be closed */
            err = 4;
            break;
        }

        len += rc;
    }

    return len;
}

/**
 * @brief  write n bytes from a sockfd with timeout
 * @param  [in] fd
 * @param  [in] buf
 * @param  [in] count
 * @param  [in] timeout_ms
 * @return -1 on err
 */
static int net_write(network_t *n, unsigned char *buf, int count, int timeout_ms)
{
    fd_set fds;
    int lfirst = 1;
    int rc, len = 0, fd;
    struct timeval tv;
    unsigned long long delta;
    struct timeval t1 = {0}, t2 = {0};

    if (!(n && buf && count)) {
        return -1;
    }

    fd = n->fd;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    gettimeofday(&t1, NULL);
    while (len < count) {
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        /* FIXME: patch for select. */
        if (lfirst) {
            lfirst = 0;
        } else {
            gettimeofday(&t2, NULL);
            delta = (t2.tv_sec  - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec) / 1000;
            if (delta >= timeout_ms)
                break;
        }

        rc = select(fd + 1, NULL, &fds, NULL, &tv);
        if (rc < 0) {
            if ((errno == EINTR) || (errno == EAGAIN)) {
                rws_thread_sleep(20);
                continue;
            }
            return -1;
        } else if (rc == 0) {
            /* time out */
            break;
        }

        if (!FD_ISSET(fd, &fds)) {
            rws_thread_sleep(20);
            continue;
        }

        rc = sendto(fd, buf + len, count - len, 0,
                    (struct sockaddr *)&(n->address),
                    sizeof(n->address));
        if (rc < 0) {
            if ((errno == EINTR) || (errno == EAGAIN)) {
                rws_thread_sleep(20);
                continue;
            }
            return -1;
        } else if (rc == 0) {
            /* the sockfd may be closed */
            break;
        }

        len += rc;
    }

    return len;
}

static int net_connect(network_t *n, char *addr, int port, int net_type)
{
    int rc = -1;
    sa_family_t family = AF_INET;
    struct addrinfo *result = NULL;
    struct addrinfo hints = {0, AF_UNSPEC, net_type, 0, 0, NULL, NULL, NULL};

    if ((rc = getaddrinfo(addr, NULL, &hints, &result)) == 0) {
        struct addrinfo *res = result;

        /* prefer ip4 addresses */
        while (res) {
            if (res->ai_family == AF_INET) {
                result = res;
                break;
            }

            res = res->ai_next;
        }

        if (result->ai_family == AF_INET) {
            n->address.sin_port = htons(port);
            n->address.sin_family = family = AF_INET;
            n->address.sin_addr = ((struct sockaddr_in *)(result->ai_addr))->sin_addr;
        } else {
            rc = -1;
        }

        freeaddrinfo(result);
    }

    if (rc == 0) {
        n->fd = socket(family, net_type, 0);

        if (n->fd != -1) {
            rc = connect(n->fd, (struct sockaddr *)&n->address, sizeof(n->address));
        }

        if (rc < 0) {
            close(n->fd);
            n->fd = 0;
        }
    }

    return rc;
}

static void net_disconnect(network_t *n)
{
    close(n->fd);
    n->fd = 0;
}

void rws_net_init(network_t *n)
{
    n->fd = 0;
    n->net_read       = net_read;
    n->net_write      = net_write;
    n->net_connect    = net_connect;
    n->net_disconncet = net_disconnect;
}
