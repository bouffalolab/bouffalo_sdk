#include <errno.h>
#include <reent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include "at_fs.h"

int at_write_file(const char *path, int offset, const char *buf, int len)
{
    int ret;
    int f = -1;

    if ((f = at_fs_open(path, O_WRONLY)) < 0) {
        return -1;
    }

    at_fs_lseek(f, offset, SEEK_SET);

    ret = at_fs_write(f, buf, len);

    at_fs_close(f);

    return ret;
}

int at_read_file(const char *path, int offset, char *buf, int len)
{
    int ret;
    int f = -1;

    if ((f = at_fs_open(path, O_RDONLY)) < 0) {
        return -1;
    }

    at_fs_lseek(f, offset, SEEK_SET);

    ret = at_fs_read(f, buf, len);

    at_fs_close(f);

    return ret;
}

int at_filesize_get(const char *path)
{
    int size;
    int f = -1;

    if ((f = at_fs_open(path, O_RDONLY)) < 0) {
        return -1;
    }

    at_fs_lseek(f, 0, SEEK_END);
    size = at_fs_lseek(f, 0, SEEK_CUR);
    at_fs_lseek(f, 0, SEEK_SET);

    at_fs_close(f);

    return size;
}

int at_load_file(const char *path, char **buf, int *n)
{
    int f = -1;
    long size;

    if ((f = at_fs_open(path, O_RDONLY)) < 0) {
        return -1;
    }

    at_fs_lseek(f, 0, SEEK_END);
    if ((size = at_fs_lseek(f, 0, SEEK_CUR)) == -1) {
        at_fs_close(f);
        return -1;
    }
    at_fs_lseek(f, 0, SEEK_SET);

    *n = (size_t) size;

    if (*n + 1 == 0 ||
        (*buf = calloc(1, *n + 1)) == NULL) {
        at_fs_close(f);
        return -1;
    }

    if (at_fs_read(f, *buf, *n) != *n) {
        at_fs_close(f);
        free(*buf);
        *buf = NULL;
        return -1;
    }

    at_fs_close(f);

    (*buf)[*n] = '\0';

    if (strstr((const char *) *buf, "-----BEGIN ") != NULL) {
        ++*n;
    }
    return 0;
}

