/*
 * port_file_fd.h - File Descriptor Encoding Scheme (Public Header)
 *
 * This header defines the unified file descriptor encoding convention
 * used by the newlib POSIX syscall layer. All filesystem port layers
 * MUST use these macros instead of hardcoded magic numbers.
 *
 * IMPORTANT: newlib stores fd in FILE._file which is a "short" (signed
 * 16-bit).  All FD masks MUST stay below 0x8000 to remain positive when
 * stored in a short, otherwise fseek/ftell/fstat will malfunction.
 *
 * FD Encoding Format (must fit in signed 16-bit short: 0 .. 0x7FFF):
 *   0x0000 - 0x0FFF : TTY/Console (stdin, stdout, stderr, /dev/*)
 *   0x4000 - 0x4FFF : FatFS
 *   0x5000 - 0x5FFF : LittleFS
 *   0x6000 - 0x7FFF : Reserved for future filesystems
 *
 * When adding a new filesystem, pick an unused segment in [0x4000, 0x7FFF]
 * and register it here. Keep the index within the lower 12 bits.
 */

#ifndef _PORT_FILE_FD_H_
#define _PORT_FILE_FD_H_

/* Common index mask: lower 12 bits hold the fd-table index */
#define FD_INDEX_MASK           0x0FFF

/* Filesystem type mask: bits [15:12] identify the filesystem */
#define FD_TYPE_MASK            0xF000

/*-----------------------------------------------------------------------
 * FatFS file descriptor helpers
 *-----------------------------------------------------------------------*/
#define FATFS_FD_MASK           0x4000
#define FATFS_FD_ENCODE(idx)    ((idx) | FATFS_FD_MASK)
#define FATFS_FD_INDEX(fd)      ((fd) & FD_INDEX_MASK)
#define FATFS_FD_IS(fd)         (((fd) & FD_TYPE_MASK) == FATFS_FD_MASK)

/*-----------------------------------------------------------------------
 * LittleFS file descriptor helpers
 *
 * NOTE: The mask was changed from 0x8000 to 0x5000 because newlib's
 * FILE._file is a signed short.  0x8000 overflows to -32768, causing
 * is_file_fd() to return false and routing all I/O to the TTY layer.
 *-----------------------------------------------------------------------*/
#define LFS_FD_MASK             0x5000
#define LFS_FD_ENCODE(idx)      ((idx) | LFS_FD_MASK)
#define LFS_FD_INDEX(fd)        ((fd) & FD_INDEX_MASK)
#define LFS_FD_IS(fd)           (((fd) & FD_TYPE_MASK) == LFS_FD_MASK)

/*-----------------------------------------------------------------------
 * Generic helpers used by syscalls.c routing layer
 *-----------------------------------------------------------------------*/

/** Check whether an fd belongs to any filesystem (not TTY) */
#define IS_FILE_FD(fd)          ((fd) >= 0x4000)

#endif /* _PORT_FILE_FD_H_ */
