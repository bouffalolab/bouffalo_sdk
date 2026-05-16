/*
 * stdio_streams.c - Standard I/O stream pointers for picolibc
 *
 * Provides stdin, stdout, and stderr FILE pointers required by picolibc.
 * These are simple placeholders that redirect to console I/O.
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "reent_compat.h"

/* Forward declarations for I/O functions */
extern int _read_r(struct _reent *reent, int fd, void *ptr, size_t size);
extern int _write_r(struct _reent *reent, int fd, const void *ptr, size_t size);
extern int _close_r(struct _reent *reent, int fd);

/* Simple FILE structure for console I/O
 * We only need the basics for stdio to work */
#define FILE_FLAG_WRITE 0x01
#define FILE_FLAG_READ  0x02

typedef struct {
    int fd;             /* File descriptor (0=stdin, 1=stdout, 2=stderr) */
    int flags;          /* File flags */
    int eof;            /* EOF flag */
    int error;          /* Error flag */
    unsigned char *buf; /* Buffer pointer (unused) */
    int ungot;          /* Ungotten character */
} _FILE;

/* Static FILE structures for stdin, stdout, stderr */
static _FILE __stdin_file = {
    .fd = 0,
    .flags = FILE_FLAG_READ,
    .eof = 0,
    .error = 0,
    .buf = NULL,
    .ungot = -1
};

static _FILE __stdout_file = {
    .fd = 1,
    .flags = FILE_FLAG_WRITE,
    .eof = 0,
    .error = 0,
    .buf = NULL,
    .ungot = -1
};

static _FILE __stderr_file = {
    .fd = 2,
    .flags = FILE_FLAG_WRITE,
    .eof = 0,
    .error = 0,
    .buf = NULL,
    .ungot = -1
};

/* Standard I/O stream pointers - cast to FILE* */
FILE *const stdin = (FILE *)&__stdin_file;
FILE *const stdout = (FILE *)&__stdout_file;
FILE *const stderr = (FILE *)&__stderr_file;
