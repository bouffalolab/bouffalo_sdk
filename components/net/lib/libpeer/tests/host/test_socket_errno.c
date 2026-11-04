/* Exercise the real UDP wrapper with log sinks that deliberately change errno. */
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "socket.h"

static int socket_error, send_calls, log_calls, stringify_calls;

static ssize_t test_sendto(int fd, const void *buffer, size_t length, int flags,
                           const struct sockaddr *address, socklen_t size)
{
    (void)buffer;
    (void)flags;
    assert(fd == 7 && address != NULL && size > 0);
    send_calls++;
    errno = socket_error;
    return socket_error ? -1 : (ssize_t)length;
}

static char *test_strerror(int error)
{
    assert(error == socket_error);
    stringify_calls++;
    errno = EIO;
    return "injected socket error";
}

static int test_printf(const char *format, ...)
{
    (void)format;
    log_calls++;
    errno = EAGAIN;
    return 0;
}

#define sendto test_sendto
#define strerror test_strerror
#define printf test_printf
#include "../../src/socket.c"
#undef sendto
#undef strerror
#undef printf

int main(void)
{
    const int errors[] = { EAGAIN, EWOULDBLOCK, ENOMEM, ENOBUFS, ENOSPC, EINTR, EBADF };
    const unsigned previous_failures[] = { 0, 1, 63 };
    const uint8_t data[] = { 1, 2, 3, 4 };
    Address address = { .family = AF_INET };
    UdpSocket socket = { .fd = 7 };

    for (unsigned family = 0; family < 2; family++) {
        address.family = family ? AF_INET6 : AF_INET;
        for (unsigned i = 0; i < sizeof(errors) / sizeof(errors[0]); i++) {
            socket_error = errors[i];
            for (unsigned j = 0; j < sizeof(previous_failures) / sizeof(previous_failures[0]); j++) {
                socket.send_error_count = previous_failures[j];
                log_calls = stringify_calls = 0;
                assert(udp_socket_sendto(&socket, &address, data, sizeof(data)) == -1);
                assert(errno == socket_error);
                assert(socket.send_error_count == previous_failures[j] + 1);
                unsigned expected_logs = j == 1 ? 0 : 1;
                assert((unsigned)log_calls == expected_logs);
                assert((unsigned)stringify_calls == expected_logs);
            }
        }
    }
    socket_error = 0;
    unsigned failed = socket.send_error_count;
    assert(udp_socket_sendto(&socket, &address, data, sizeof(data)) == sizeof(data));
    assert(socket.send_error_count == failed);
    socket.fd = -1;
    int previous_calls = send_calls;
    errno = ENOMEM;
    assert(udp_socket_sendto(&socket, &address, data, sizeof(data)) == -1);
    assert(errno == EBADF && send_calls == previous_calls);
    puts("UDP send errno survives clobbering logs, log throttling and invalid descriptors passed");
}
