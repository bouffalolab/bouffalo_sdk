#include <errno.h>
#include <reent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include "at_fs.h"
__attribute__((weak)) int at_fs_open(const char *path, int flags)
{
    return -1;
}

__attribute__((weak)) at_dir_t at_fs_opendir(const char *path)
{
    return NULL;
}

__attribute__((weak)) int at_fs_readdir(at_dir_t dirp, struct at_direct *res)
{
    return -1;
}

__attribute__((weak)) int at_fs_closedir(at_dir_t dir)
{
    return -1;
}

__attribute__((weak)) int at_fs_remove(const char *path)
{
    return -1;
}

__attribute__((weak)) int at_fs_close(int fd)
{
    return -1;
}

__attribute__((weak)) size_t at_fs_read(int fd, void *ptr, size_t size)
{
    return 0;
}

__attribute__((weak)) size_t at_fs_write(int fd, const void *ptr, size_t size)
{
    return 0;
}

__attribute__((weak)) size_t at_fs_lseek(int fd, _off_t offset, int whence)
{
    return 0;
}

__attribute__((weak)) int at_fs_register(void)
{
    return -1;
}

