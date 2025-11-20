#include <errno.h>
#include <reent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include "at_fs.h"
#include "at_main.h"
#include "at_core.h"
#include "at_pal.h"

#define AT_FS_PRINTF AT_CMD_PRINTF

int at_write_file(const char *path, int offset, const char *buf, int len)
{
    if (!path || !buf || len <= 0) {
        AT_FS_PRINTF("Invalid arguments to at_write_file\r\n");
        return -1;
    }
    int ret;
    int f = -1;

    if ((f = at_fs_open(path, O_WRONLY)) < 0) {
        AT_FS_PRINTF("Failed to open file for writing: %s\r\n", path);
        return -1;
    }

    at_fs_lseek(f, offset, SEEK_SET);

    ret = at_fs_write(f, buf, len);

    at_fs_close(f);

    if (ret != len) {
        AT_FS_PRINTF("Failed to write all data to file: %s\r\n", path);
    }

    return ret;
}

int at_read_file(const char *path, int offset, char *buf, int len)
{
    if (!path || !buf || len <= 0) {
        AT_FS_PRINTF("Invalid arguments to at_read_file\r\n");
        return -1;
    }
    int ret;
    int f = -1;

    if ((f = at_fs_open(path, O_RDONLY)) < 0) {
        AT_FS_PRINTF("Failed to open file for reading: %s\r\n", path);
        return -1;
    }

    at_fs_lseek(f, offset, SEEK_SET);

    ret = at_fs_read(f, buf, len);

    at_fs_close(f);

    if (ret != len) {
        AT_FS_PRINTF("Failed to read all data from file: %s\r\n", path);
    }

    return ret;
}

int at_filesize_get(const char *path)
{
    if (!path) {
        AT_FS_PRINTF("Invalid arguments to at_filesize_get\r\n");
        return -1;
    }
    int size;
    int f = -1;

    if ((f = at_fs_open(path, O_RDONLY)) < 0) {
        AT_FS_PRINTF("Failed to open file to get size: %s\r\n", path);
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
    if (!path || !buf || !n) {
        AT_FS_PRINTF("Invalid arguments to at_load_file\r\n");
        return -1;
    }
    int f = -1;
    long size;

    if ((f = at_fs_open(path, O_RDONLY)) < 0) {
        AT_FS_PRINTF("Failed to open file to load: %s\r\n", path);
        return -1;
    }

    at_fs_lseek(f, 0, SEEK_END);
    if ((size = at_fs_lseek(f, 0, SEEK_CUR)) == -1) {
        AT_FS_PRINTF("Failed to get file size: %s\r\n", path);
        at_fs_close(f);
        return -1;
    }
    at_fs_lseek(f, 0, SEEK_SET);

    *n = (size_t) size;

    if (*n < 0 || *n > SIZE_MAX - 1 ||
        (*buf = at_calloc(1, *n + 1)) == NULL) {
        AT_FS_PRINTF("Failed to allocate memory for file: %s\r\n", path);
        at_fs_close(f);
        return -1;
    }

    if (at_fs_read(f, *buf, *n) != *n) {
        AT_FS_PRINTF("Failed to read file into buffer: %s\r\n", path);
        at_fs_close(f);
        at_free(*buf);
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

