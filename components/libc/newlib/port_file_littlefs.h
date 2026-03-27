/*
 * LittleFS POSIX Port Layer for Bouffalo SDK
 *
 * Provides POSIX file API compatibility for LittleFS filesystem
 */

#ifndef _PORT_FILE_LITTLEFS_H_
#define _PORT_FILE_LITTLEFS_H_

#include <stddef.h>
#include <stdbool.h>
#include "lfs.h"

#ifdef CONFIG_FREERTOS
#include <FreeRTOS.h>
#include <semphr.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "port_file_fd.h"

/* LittleFS-specific FD helpers (derived from port_file_fd.h) */
#define LFS_FD_IS_LFS(fd)   LFS_FD_IS(fd)

/* Maximum number of open files for LittleFS */
#ifndef LFS_FOPEN_MAX
#define LFS_FOPEN_MAX       16
#endif

/* Maximum number of registered LittleFS partitions */
#ifndef LFS_PARTITION_MAX
#define LFS_PARTITION_MAX   4
#endif

/* Default mount point for LittleFS */
#ifndef LFS_DEFAULT_MOUNT_POINT
#define LFS_DEFAULT_MOUNT_POINT "/lfs"
#endif

/* Default partition name for LittleFS */
#ifndef LFS_DEFAULT_PARTITION_NAME
#define LFS_DEFAULT_PARTITION_NAME "PSM"
#endif

/**
 * @brief LittleFS port context structure
 *
 * Contains flash address, size, and buffers for LittleFS operations
 */
struct lfs_port_context {
    const char *partition_name;     /*!< Partition name (e.g., "PSM", "LittleFS") */
    uint32_t flash_addr;            /*!< Flash address from partition */
    uint32_t flash_size;            /*!< Partition size in bytes */
    uint8_t *read_buffer;           /*!< Read cache buffer (allocated or user-provided) */
    uint8_t *prog_buffer;           /*!< Program cache buffer (allocated or user-provided) */
    uint8_t *lookahead_buffer;      /*!< Lookahead buffer (allocated or user-provided) */
#ifdef CONFIG_FREERTOS
    SemaphoreHandle_t lock;         /*!< Mutex for thread safety */
#endif
};

/*****************************************************************************
 * Partition-based Initialization Functions
 *****************************************************************************/

/**
 * @brief Initialize LittleFS configuration from partition
 *
 * Opens the partition via MTD API and populates the lfs_config structure.
 * Allocates read/prog/lookahead buffers if not provided in context.
 *
 * @param lfs       LittleFS instance (statically allocated)
 * @param ctx       LittleFS context (partition_name must be set)
 * @param cfg       LittleFS config to populate
 * @return 0 on success, -1 on failure
 */
int lfs_port_init(lfs_t *lfs, struct lfs_port_context *ctx, struct lfs_config *cfg);

/**
 * @brief Check if filesystem is formatted
 *
 * Attempts to mount the filesystem to check if it's formatted.
 *
 * @param cfg       LittleFS config
 * @return true if formatted, false otherwise
 */
bool lfs_port_is_formatted(struct lfs_config *cfg);

/**
 * @brief Mount LittleFS with auto-format on first use
 *
 * Attempts to mount the filesystem. If mount fails (unformatted or corrupted),
 * it formats the filesystem and retries mount.
 *
 * @param lfs       LittleFS instance
 * @param cfg       LittleFS config
 * @return 0 on success, -1 on failure
 */
int lfs_port_mount(lfs_t *lfs, struct lfs_config *cfg);

/**
 * @brief Full LittleFS initialization with partition and POSIX registration
 *
 * Convenience function that:
 * 1. Initializes MTD
 * 2. Opens partition via MTD
 * 3. Configures LittleFS
 * 4. Mounts with auto-format
 * 5. Registers with POSIX layer
 *
 * @param lfs           LittleFS instance (static allocation)
 * @param ctx           LittleFS context (static allocation)
 * @param cfg           LittleFS config (static allocation)
 * @param partition_name Partition name (e.g., "PSM", "LittleFS"), NULL for default
 * @param mount_point   POSIX mount point (e.g., "/lfs"), NULL for default
 * @return 0 on success, -1 on failure
 */
int lfs_port_full_init(lfs_t *lfs, struct lfs_port_context *ctx,
                       struct lfs_config *cfg,
                       const char *partition_name,
                       const char *mount_point);

/**
 * @brief Cleanup LittleFS resources
 *
 * Unregisters from POSIX layer, unmounts filesystem, and frees buffers.
 *
 * @param lfs       LittleFS instance
 * @param ctx       LittleFS context
 */
void lfs_port_cleanup(lfs_t *lfs, struct lfs_port_context *ctx);

/*****************************************************************************
 * Flash Operation Callbacks
 *****************************************************************************/

/**
 * @brief Read a region in a block from flash
 */
int lfs_port_flash_read(const struct lfs_config *c, lfs_block_t block,
                        lfs_off_t off, void *buffer, lfs_size_t size);

/**
 * @brief Program a region in a block to flash
 */
int lfs_port_flash_prog(const struct lfs_config *c, lfs_block_t block,
                        lfs_off_t off, const void *buffer, lfs_size_t size);

/**
 * @brief Erase a block in flash
 */
int lfs_port_flash_erase(const struct lfs_config *c, lfs_block_t block);

/**
 * @brief Sync flash (no-op for bare metal)
 */
int lfs_port_flash_sync(const struct lfs_config *c);

/*****************************************************************************
 * Filesystem Registration Functions
 *****************************************************************************/

/**
 * @brief Register a LittleFS filesystem instance
 *
 * @param lfs       Pointer to the lfs_t instance
 * @param mount_point Mount point path (e.g., "/lfs", "/data")
 * @return 0 on success, -1 on error (errno set)
 */
int lfs_port_register_fs(lfs_t *lfs, const char *mount_point);

/**
 * @brief Unregister a LittleFS filesystem instance
 *
 * @param lfs       Pointer to the lfs_t instance
 * @return 0 on success, -1 on error (errno set)
 */
int lfs_port_unregister_fs(lfs_t *lfs);

/**
 * @brief Set the default LittleFS filesystem
 *
 * The default filesystem is used when path doesn't match any mount point
 *
 * @param lfs       Pointer to the lfs_t instance
 * @return 0 on success, -1 on error (errno set)
 */
int lfs_port_set_default(lfs_t *lfs);

/**
 * @brief Get the default LittleFS filesystem
 *
 * @return Pointer to default lfs_t, or NULL if not set
 */
lfs_t *lfs_port_get_default(void);

/**
 * @brief Find LittleFS instance by path
 *
 * @param path      File path to match against mount points
 * @param out_path  Output pointer to path with mount point stripped (optional)
 * @return Pointer to matching lfs_t, or NULL if not found
 */
lfs_t *lfs_port_find_by_path(const char *path, const char **out_path);

/**
 * @brief Check if current working directory is in LittleFS
 *
 * @return 1 if cwd is in LittleFS, 0 otherwise
 */
int lfs_port_cwd_is_lfs(void);

/*****************************************************************************
 * Syscall Implementations (called from syscalls.c)
 *****************************************************************************/

int _open_file_lfs_r(struct _reent *reent, const char *path, int flags, int mode);
int _close_file_lfs_r(struct _reent *reent, int fd);
_ssize_t _read_file_lfs_r(struct _reent *reent, int fd, void *ptr, size_t size);
_ssize_t _write_file_lfs_r(struct _reent *reent, int fd, const void *ptr, size_t size);
_off_t _lseek_file_lfs_r(struct _reent *reent, int fd, _off_t offset, int whence);
int _fstat_file_lfs_r(struct _reent *reent, int fd, struct stat *st);
int _stat_file_lfs_r(struct _reent *reent, const char *path, struct stat *st);
int _unlink_file_lfs_r(struct _reent *reent, const char *path);
int _rename_file_lfs_r(struct _reent *reent, const char *oldname, const char *newname);
int _mkdir_file_lfs_r(struct _reent *reent, const char *path, int mode);
int _rmdir_file_lfs_r(struct _reent *reent, const char *path);
int _chdir_file_lfs_r(struct _reent *reent, const char *path);
char *_getcwd_file_lfs_r(struct _reent *reent, char *buf, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* _PORT_FILE_LITTLEFS_H_ */
