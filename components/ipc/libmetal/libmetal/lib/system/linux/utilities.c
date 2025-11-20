/*
 * Copyright (c) 2015, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	utils.c
 * @brief	Linux libmetal utility functions.
 */

#include <metal/utilities.h>
#include <metal/sys.h>

/**
 * @brief	Open (or create) a file.
 *
 * This function opens or creates a file with read/write permissions and the
 * O_CLOEXEC flag set.
 *
 * @param[in]	path	File path to open.
 * @param[in]	shm	Open shared memory (via shm_open) if non-zero.
 * @return	File descriptor.
 */
int metal_open(const char *path, int shm)
{
	const int flags = O_RDWR | O_CREAT | O_CLOEXEC;
	const int mode = S_IRUSR | S_IWUSR;
	int fd;

	if (!path || !strlen(path))
		return -EINVAL;

	fd = shm ? shm_open(path, flags, mode) : open(path, flags, mode);
	return fd < 0 ? -errno : fd;
}

/**
 * @brief	Map a segment of a file/device.
 *
 * This function maps a segment of a file or device into the process address
 * space, after optionally expanding the file if necessary.  If required, the
 * file is expanded to hold the requested map area.  This is done under and
 * advisory lock, and therefore the called must not have an advisory lock on
 * the file being mmapped.
 *
 * @param[in]	fd	File descriptor to map.
 * @param[in]	offset	Offset in file to map.
 * @param[in]	size	Size of region to map.
 * @param[in]	expand	Allow file expansion via ftruncate if non-zero.
 * @param[in]	flags	Flags for mmap(), MAP_SHARED included implicitly.
 * @param[out]	result	Returned pointer to new memory map.
 * @return	0 on success, or -errno on error.
 */
int metal_map(int fd, off_t offset, size_t size, int expand, int flags,
	      void **result)
{
	int prot = PROT_READ | PROT_WRITE, error;
	void *mem;

	flags |= MAP_SHARED;

	if (fd < 0) {
		fd = -1;
		flags = MAP_PRIVATE | MAP_ANONYMOUS;
	} else if (expand) {
		off_t reqsize = offset + size;
		struct stat stat;

		error = flock(fd, LOCK_EX) < 0 ? -errno : 0;
		if (!error)
			error = fstat(fd, &stat);
		if (!error && stat.st_size < reqsize)
			error = ftruncate(fd, reqsize);
		if (!error)
			flock(fd, LOCK_UN);
		if (error)
			return -errno;
	}

	mem = mmap(NULL, size, prot, flags, fd, offset);
	if (mem == MAP_FAILED)
		return -errno;
	*result = mem;
	return 0;
}

/**
 * @brief	Unmap a segment of the process address space.
 *
 * This function unmaps a segment of the process address space.
 *
 * @param[in]	mem	Segment to unmap.
 * @param[in]	size	Size of region to unmap.
 * @return	0 on success, or -errno on error.
 */
int metal_unmap(void *mem, size_t size)
{
	return munmap(mem, size) < 0 ? -errno : 0;
}
