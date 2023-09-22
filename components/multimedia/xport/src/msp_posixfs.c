/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/unistd.h>

int msp_open(const char *path, int flags)
{
	int ret = open(path, flags);
	return (ret > 0) ? ret : 0;
}

int msp_stat(const char *path, struct stat *st)
{
	return stat(path, st);
}

int msp_close(int fd)
{
	return close(fd);
}

int msp_read(int fd, void *buf, size_t nbytes)
{
	return read(fd, buf, nbytes);
}

int msp_write(int fd, const void *buf, size_t nbytes)
{
	return write(fd, buf, nbytes);
}

int msp_lseek(int fd, off_t offset, int whence)
{
	return lseek(fd, offset, whence);
}
