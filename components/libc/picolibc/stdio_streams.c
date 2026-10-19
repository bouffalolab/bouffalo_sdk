/*
 * stdio_streams.c - Standard I/O streams for picolibc
 */

#include <stdio.h>
#include <unistd.h>

#include "reent_compat.h"

extern ssize_t _read_r(struct _reent *reent, int fd, void *ptr, size_t size);
extern ssize_t _write_r(struct _reent *reent, int fd, const void *ptr,
                        size_t size);

static int stdio_get(FILE *stream)
{
    unsigned char c;
    ssize_t ret;

    (void)stream;
    ret = _read_r(_REENT, STDIN_FILENO, &c, 1);
    if (ret == 1) {
        return c;
    }

    return ret == 0 ? _FDEV_EOF : _FDEV_ERR;
}

static int stdio_put(int fd, char c)
{
    return _write_r(_REENT, fd, &c, 1) == 1 ? 0 : _FDEV_ERR;
}

static int stdout_put(char c, FILE *stream)
{
    (void)stream;
    return stdio_put(STDOUT_FILENO, c);
}

static int stderr_put(char c, FILE *stream)
{
    (void)stream;
    return stdio_put(STDERR_FILENO, c);
}

static FILE stdin_file =
    FDEV_SETUP_STREAM(NULL, stdio_get, NULL, _FDEV_SETUP_READ);
static FILE stdout_file =
    FDEV_SETUP_STREAM(stdout_put, NULL, NULL, _FDEV_SETUP_WRITE);
static FILE stderr_file =
    FDEV_SETUP_STREAM(stderr_put, NULL, NULL, _FDEV_SETUP_WRITE);

FILE *const stdin = &stdin_file;
FILE *const stdout = &stdout_file;
FILE *const stderr = &stderr_file;
