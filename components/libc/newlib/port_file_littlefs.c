/*
 * LittleFS POSIX Port Layer for Bouffalo SDK
 *
 * Provides POSIX file API compatibility for LittleFS filesystem
 */

#include <errno.h>
#include <reent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#include "lfs.h"
#include "port_file_littlefs.h"
#include "bflb_flash.h"

/* Use MTD API for partition info (default: enabled for backward compatibility)
 * Disable to use pure LHAL boot2 API instead */
#ifndef CONFIG_LFS_USE_MTD
#define CONFIG_LFS_USE_MTD 1
#endif

#if CONFIG_LFS_USE_MTD
#include "bflb_mtd.h"
#else
#include "bflb_boot2.h"
#endif

#ifdef CONFIG_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#endif

/* Error code mapping: LittleFS error to errno */
static int lfs_err_to_errno(int err)
{
    switch (err) {
        case LFS_ERR_OK:
            return 0;
        case LFS_ERR_IO:
            return EIO;
        case LFS_ERR_CORRUPT:
            return EIO;
        case LFS_ERR_NOENT:
            return ENOENT;
        case LFS_ERR_EXIST:
            return EEXIST;
        case LFS_ERR_NOTDIR:
            return ENOTDIR;
        case LFS_ERR_ISDIR:
            return EISDIR;
        case LFS_ERR_NOTEMPTY:
            return ENOTEMPTY;
        case LFS_ERR_BADF:
            return EBADF;
        case LFS_ERR_FBIG:
            return EFBIG;
        case LFS_ERR_INVAL:
            return EINVAL;
        case LFS_ERR_NOSPC:
            return ENOSPC;
        case LFS_ERR_NOMEM:
            return ENOMEM;
        case LFS_ERR_NOATTR:
            return ENODATA;
        case LFS_ERR_NAMETOOLONG:
            return ENAMETOOLONG;
        default:
            return EINVAL;
    }
}

/* Convert POSIX open flags to LittleFS flags */
static int posix_flags_to_lfs(int flags)
{
    int lfs_flags = 0;

    switch (flags & O_ACCMODE) {
        case O_RDONLY:
            lfs_flags = LFS_O_RDONLY;
            break;
        case O_WRONLY:
            lfs_flags = LFS_O_WRONLY;
            break;
        case O_RDWR:
            lfs_flags = LFS_O_RDWR;
            break;
        default:
            lfs_flags = LFS_O_RDONLY;
            break;
    }

    if ((flags & (O_CREAT | O_TRUNC)) == (O_CREAT | O_TRUNC)) {
        lfs_flags |= LFS_O_CREAT | LFS_O_TRUNC;
    } else if (flags & O_APPEND) {
        lfs_flags |= LFS_O_CREAT | LFS_O_APPEND;
    } else if ((flags & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL)) {
        lfs_flags |= LFS_O_CREAT | LFS_O_EXCL;
    } else if (flags & O_CREAT) {
        lfs_flags |= LFS_O_CREAT;
    }

    if (flags & O_TRUNC) {
        lfs_flags |= LFS_O_TRUNC;
    }

    return lfs_flags;
}

/* File descriptor entry for LittleFS */
struct lfs_fd_entry {
    lfs_file_t *file;
    lfs_t *lfs;
    uint8_t in_use;
};

/* Filesystem registry entry */
struct lfs_fs_entry {
    lfs_t *lfs;
    char mount_point[LFS_NAME_MAX + 1];
    uint8_t in_use;
};

/* File descriptor table */
static struct lfs_fd_entry lfs_fd_table[LFS_FOPEN_MAX];

/* Filesystem registry */
static struct lfs_fs_entry lfs_fs_registry[LFS_PARTITION_MAX];

/* Default filesystem (for paths without explicit mount point) */
static lfs_t *lfs_default_fs = NULL;

/* Current working directory for LittleFS */
static char lfs_cwd[LFS_NAME_MAX + 1] = "/";

/* Helper function to check if LittleFS cwd is active */
int lfs_port_cwd_is_lfs(void)
{
    return strncmp(lfs_cwd, "/lfs", 4) == 0;
}

/* Mutex for thread safety (if needed, can be added later) */

/*****************************************************************************
 * Filesystem Registration Functions
 *****************************************************************************/

int lfs_port_register_fs(lfs_t *lfs, const char *mount_point)
{
    if (lfs == NULL || mount_point == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* Find empty slot */
    for (int i = 0; i < LFS_PARTITION_MAX; i++) {
        if (!lfs_fs_registry[i].in_use) {
            lfs_fs_registry[i].lfs = lfs;
            strncpy(lfs_fs_registry[i].mount_point, mount_point, LFS_NAME_MAX);
            lfs_fs_registry[i].mount_point[LFS_NAME_MAX] = '\0';
            lfs_fs_registry[i].in_use = 1;

            /* Set as default if first filesystem */
            if (lfs_default_fs == NULL) {
                lfs_default_fs = lfs;
            }

            /* Initialize CWD to mount point if not already set to a LittleFS path */
            if (!lfs_port_cwd_is_lfs()) {
                strncpy(lfs_cwd, mount_point, LFS_NAME_MAX);
                lfs_cwd[LFS_NAME_MAX] = '\0';
            }

            return 0;
        }
    }

    errno = ENOSPC;
    return -1;
}

int lfs_port_unregister_fs(lfs_t *lfs)
{
    if (lfs == NULL) {
        errno = EINVAL;
        return -1;
    }

    for (int i = 0; i < LFS_PARTITION_MAX; i++) {
        if (lfs_fs_registry[i].in_use && lfs_fs_registry[i].lfs == lfs) {
            /* Close all files belonging to this filesystem */
            for (int j = 0; j < LFS_FOPEN_MAX; j++) {
                if (lfs_fd_table[j].in_use && lfs_fd_table[j].lfs == lfs) {
                    lfs_file_close(lfs, lfs_fd_table[j].file);
                    free(lfs_fd_table[j].file);
                    lfs_fd_table[j].in_use = 0;
                }
            }

            /* Clear default if this was it */
            if (lfs_default_fs == lfs) {
                lfs_default_fs = NULL;
            }

            lfs_fs_registry[i].in_use = 0;
            return 0;
        }
    }

    errno = ENOENT;
    return -1;
}

int lfs_port_set_default(lfs_t *lfs)
{
    lfs_default_fs = lfs;
    return 0;
}

lfs_t *lfs_port_get_default(void)
{
    return lfs_default_fs;
}

lfs_t *lfs_port_find_by_path(const char *path, const char **out_path)
{
    if (path == NULL) {
        return NULL;
    }

    /* Skip leading slash for matching */
    const char *p = path;
    while (*p == '/') {
        p++;
    }

    /* Search for matching mount point */
    for (int i = 0; i < LFS_PARTITION_MAX; i++) {
        if (lfs_fs_registry[i].in_use) {
            const char *mp = lfs_fs_registry[i].mount_point;
            /* Skip leading slash in mount point */
            while (*mp == '/') {
                mp++;
            }

            size_t mp_len = strlen(mp);
            if (mp_len > 0 && strncmp(p, mp, mp_len) == 0) {
                /* Check if path ends at mount point or has separator */
                char next_char = p[mp_len];
                if (next_char == '\0' || next_char == '/') {
                    if (out_path) {
                        /* Return path relative to mount point */
                        *out_path = p + mp_len;
                        if (**out_path == '\0') {
                            *out_path = "/";
                        }
                    }
                    return lfs_fs_registry[i].lfs;
                }
            }
        }
    }

    /* No match found, use default filesystem */
    if (lfs_default_fs) {
        if (out_path) {
            *out_path = path;
        }
        return lfs_default_fs;
    }

    return NULL;
}

/*****************************************************************************
 * Syscall Implementations
 *****************************************************************************/

int _open_file_lfs_r(struct _reent *reent, const char *path, int flags, int mode)
{
    (void)mode; /* LittleFS doesn't support file permissions */

    lfs_t *lfs;
    const char *lfs_path;
    int fd = -1;
    int ret;

    /* Find filesystem and get relative path */
    lfs = lfs_port_find_by_path(path, &lfs_path);
    if (lfs == NULL) {
        reent->_errno = ENOENT;
        return -1;
    }

    /* Find free file descriptor */
    for (int i = 0; i < LFS_FOPEN_MAX; i++) {
        if (!lfs_fd_table[i].in_use) {
            fd = i;
            break;
        }
    }

    if (fd == -1) {
        reent->_errno = EMFILE;
        return -1;
    }

    /* Allocate file structure */
    lfs_file_t *file = (lfs_file_t *)malloc(sizeof(lfs_file_t));
    if (file == NULL) {
        reent->_errno = ENOMEM;
        return -1;
    }

    /* Open file */
    ret = lfs_file_open(lfs, file, lfs_path, posix_flags_to_lfs(flags));
    if (ret < 0) {
        reent->_errno = lfs_err_to_errno(ret);
        free(file);
        return -1;
    }

    /* Store in descriptor table */
    lfs_fd_table[fd].file = file;
    lfs_fd_table[fd].lfs = lfs;
    lfs_fd_table[fd].in_use = 1;

    return LFS_FD_ENCODE(fd);
}

int _close_file_lfs_r(struct _reent *reent, int fd)
{
    int idx = LFS_FD_INDEX(fd);

    if (idx < 0 || idx >= LFS_FOPEN_MAX || !lfs_fd_table[idx].in_use) {
        reent->_errno = EBADF;
        return -1;
    }

    int ret = lfs_file_close(lfs_fd_table[idx].lfs, lfs_fd_table[idx].file);
    if (ret < 0) {
        reent->_errno = lfs_err_to_errno(ret);
        /* Still free the entry even on error */
        free(lfs_fd_table[idx].file);
        lfs_fd_table[idx].in_use = 0;
        return -1;
    }

    free(lfs_fd_table[idx].file);
    lfs_fd_table[idx].in_use = 0;

    return 0;
}

_ssize_t _read_file_lfs_r(struct _reent *reent, int fd, void *ptr, size_t size)
{
    int idx = LFS_FD_INDEX(fd);

    if (ptr == NULL) {
        reent->_errno = EINVAL;
        return -1;
    }

    if (idx < 0 || idx >= LFS_FOPEN_MAX || !lfs_fd_table[idx].in_use) {
        reent->_errno = EBADF;
        return -1;
    }

    lfs_ssize_t ret = lfs_file_read(lfs_fd_table[idx].lfs,
                                    lfs_fd_table[idx].file,
                                    ptr, size);
    if (ret < 0) {
        reent->_errno = lfs_err_to_errno(ret);
        return -1;
    }

    return (_ssize_t)ret;
}

_ssize_t _write_file_lfs_r(struct _reent *reent, int fd, const void *ptr, size_t size)
{
    int idx = LFS_FD_INDEX(fd);

    if (ptr == NULL) {
        reent->_errno = EINVAL;
        return -1;
    }

    if (idx < 0 || idx >= LFS_FOPEN_MAX || !lfs_fd_table[idx].in_use) {
        reent->_errno = EBADF;
        return -1;
    }

    lfs_ssize_t ret = lfs_file_write(lfs_fd_table[idx].lfs,
                                     lfs_fd_table[idx].file,
                                     ptr, size);
    if (ret < 0) {
        reent->_errno = lfs_err_to_errno(ret);
        return -1;
    }

    /* Sync after write for reliability */
    lfs_file_sync(lfs_fd_table[idx].lfs, lfs_fd_table[idx].file);

    return (_ssize_t)ret;
}

_off_t _lseek_file_lfs_r(struct _reent *reent, int fd, _off_t offset, int whence)
{
    int idx = LFS_FD_INDEX(fd);

    if (idx < 0 || idx >= LFS_FOPEN_MAX || !lfs_fd_table[idx].in_use) {
        reent->_errno = EBADF;
        return -1;
    }

    int lfs_whence;
    switch (whence) {
        case SEEK_SET:
            lfs_whence = LFS_SEEK_SET;
            break;
        case SEEK_CUR:
            lfs_whence = LFS_SEEK_CUR;
            break;
        case SEEK_END:
            lfs_whence = LFS_SEEK_END;
            break;
        default:
            reent->_errno = EINVAL;
            return -1;
    }

    lfs_soff_t ret = lfs_file_seek(lfs_fd_table[idx].lfs,
                                   lfs_fd_table[idx].file,
                                   offset, lfs_whence);
    if (ret < 0) {
        reent->_errno = lfs_err_to_errno(ret);
        return -1;
    }

    return (_off_t)ret;
}

int _fstat_file_lfs_r(struct _reent *reent, int fd, struct stat *st)
{
    int idx = LFS_FD_INDEX(fd);

    if (st == NULL) {
        reent->_errno = EINVAL;
        return -1;
    }

    if (idx < 0 || idx >= LFS_FOPEN_MAX || !lfs_fd_table[idx].in_use) {
        reent->_errno = EBADF;
        return -1;
    }

    memset(st, 0, sizeof(*st));

    st->st_mode = S_IFREG | 0644;

    lfs_soff_t size = lfs_file_size(lfs_fd_table[idx].lfs, lfs_fd_table[idx].file);
    if (size < 0) {
        reent->_errno = lfs_err_to_errno(size);
        return -1;
    }

    st->st_size = size;

    return 0;
}

int _stat_file_lfs_r(struct _reent *reent, const char *path, struct stat *st)
{
    lfs_t *lfs;
    const char *lfs_path;
    struct lfs_info info;

    if (st == NULL || path == NULL) {
        reent->_errno = EINVAL;
        return -1;
    }

    memset(st, 0, sizeof(*st));

    lfs = lfs_port_find_by_path(path, &lfs_path);
    if (lfs == NULL) {
        reent->_errno = ENOENT;
        return -1;
    }

    int ret = lfs_stat(lfs, lfs_path, &info);
    if (ret < 0) {
        reent->_errno = lfs_err_to_errno(ret);
        return -1;
    }

    st->st_mode = 0644;

    if (info.type == LFS_TYPE_DIR) {
        st->st_mode |= S_IFDIR;
    } else {
        st->st_mode |= S_IFREG;
    }

    st->st_size = info.size;

    return 0;
}

int _unlink_file_lfs_r(struct _reent *reent, const char *path)
{
    lfs_t *lfs;
    const char *lfs_path;

    lfs = lfs_port_find_by_path(path, &lfs_path);
    if (lfs == NULL) {
        reent->_errno = ENOENT;
        return -1;
    }

    int ret = lfs_remove(lfs, lfs_path);
    if (ret < 0) {
        reent->_errno = lfs_err_to_errno(ret);
        return -1;
    }

    return 0;
}

int _rename_file_lfs_r(struct _reent *reent, const char *oldname, const char *newname)
{
    lfs_t *lfs_old, *lfs_new;
    const char *lfs_old_path, *lfs_new_path;

    lfs_old = lfs_port_find_by_path(oldname, &lfs_old_path);
    lfs_new = lfs_port_find_by_path(newname, &lfs_new_path);

    if (lfs_old == NULL || lfs_new == NULL) {
        reent->_errno = ENOENT;
        return -1;
    }

    /* Cross-filesystem rename not supported */
    if (lfs_old != lfs_new) {
        reent->_errno = EXDEV;
        return -1;
    }

    int ret = lfs_rename(lfs_old, lfs_old_path, lfs_new_path);
    if (ret < 0) {
        reent->_errno = lfs_err_to_errno(ret);
        return -1;
    }

    return 0;
}

int _mkdir_file_lfs_r(struct _reent *reent, const char *path, int mode)
{
    (void)mode; /* LittleFS doesn't support directory permissions */

    lfs_t *lfs;
    const char *lfs_path;

    lfs = lfs_port_find_by_path(path, &lfs_path);
    if (lfs == NULL) {
        reent->_errno = ENOENT;
        return -1;
    }

    int ret = lfs_mkdir(lfs, lfs_path);
    if (ret < 0) {
        reent->_errno = lfs_err_to_errno(ret);
        return -1;
    }

    return 0;
}

int _rmdir_file_lfs_r(struct _reent *reent, const char *path)
{
    /* LittleFS uses lfs_remove for both files and directories */
    return _unlink_file_lfs_r(reent, path);
}

int _chdir_file_lfs_r(struct _reent *reent, const char *path)
{
    /* LittleFS doesn't have built-in chdir, so we maintain cwd locally */
    lfs_t *lfs;
    const char *lfs_path;

    if (path == NULL) {
        reent->_errno = EINVAL;
        return -1;
    }

    lfs = lfs_port_find_by_path(path, &lfs_path);
    if (lfs == NULL) {
        reent->_errno = ENOENT;
        return -1;
    }

    /* Verify directory exists */
    struct lfs_info info;
    int ret = lfs_stat(lfs, lfs_path, &info);
    if (ret < 0) {
        reent->_errno = lfs_err_to_errno(ret);
        return -1;
    }

    if (info.type != LFS_TYPE_DIR) {
        reent->_errno = ENOTDIR;
        return -1;
    }

    /* Store the current working directory */
    strncpy(lfs_cwd, path, LFS_NAME_MAX);
    lfs_cwd[LFS_NAME_MAX] = '\0';

    return 0;
}

char *_getcwd_file_lfs_r(struct _reent *reent, char *buf, size_t size)
{
    if (buf == NULL) {
        reent->_errno = EINVAL;
        return NULL;
    }

    if (size == 0) {
        reent->_errno = EINVAL;
        return NULL;
    }

    if (strlen(lfs_cwd) >= size) {
        reent->_errno = ERANGE;
        return NULL;
    }

    strcpy(buf, lfs_cwd);
    return buf;
}

/*****************************************************************************
 * Partition-based LittleFS Initialization Functions
 *****************************************************************************/

/**
 * @brief Read a region in a block from flash
 *
 * @param c         LittleFS config
 * @param block     Block number
 * @param off       Offset within block
 * @param buffer    Output buffer
 * @param size      Size to read
 * @return 0 on success, negative error code on failure
 */
int lfs_port_flash_read(const struct lfs_config *c, lfs_block_t block,
                        lfs_off_t off, void *buffer, lfs_size_t size)
{
    struct lfs_port_context *ctx = (struct lfs_port_context *)c->context;
    uint32_t addr = ctx->flash_addr + block * c->block_size + off;
    return bflb_flash_read(addr, (uint8_t *)buffer, size);
}

/**
 * @brief Program a region in a block to flash
 *
 * @param c         LittleFS config
 * @param block     Block number
 * @param off       Offset within block
 * @param buffer    Data buffer
 * @param size      Size to write
 * @return 0 on success, negative error code on failure
 */
int lfs_port_flash_prog(const struct lfs_config *c, lfs_block_t block,
                        lfs_off_t off, const void *buffer, lfs_size_t size)
{
    struct lfs_port_context *ctx = (struct lfs_port_context *)c->context;
    uint32_t addr = ctx->flash_addr + block * c->block_size + off;
    return bflb_flash_write(addr, (uint8_t *)buffer, size);
}

/**
 * @brief Erase a block in flash
 *
 * @param c         LittleFS config
 * @param block     Block number
 * @return 0 on success, negative error code on failure
 */
int lfs_port_flash_erase(const struct lfs_config *c, lfs_block_t block)
{
    struct lfs_port_context *ctx = (struct lfs_port_context *)c->context;
    uint32_t addr = ctx->flash_addr + block * c->block_size;
    return bflb_flash_erase(addr, c->block_size);
}

/**
 * @brief Sync flash (no-op for bare metal)
 *
 * @param c         LittleFS config
 * @return 0 always
 */
int lfs_port_flash_sync(const struct lfs_config *c)
{
    (void)c;
    return 0;
}

#if defined(LFS_THREADSAFE) && defined(CONFIG_FREERTOS)
/**
 * @brief Lock the filesystem for thread safety
 *
 * @param c         LittleFS config
 * @return 0 always
 */
static int lfs_port_lock(const struct lfs_config *c)
{
    struct lfs_port_context *ctx = (struct lfs_port_context *)c->context;
    if (ctx->lock != NULL) {
#if configUSE_RECURSIVE_MUTEXES
        xSemaphoreTakeRecursive(ctx->lock, portMAX_DELAY);
#else
        xSemaphoreTake(ctx->lock, portMAX_DELAY);
#endif
    }
    return 0;
}

/**
 * @brief Unlock the filesystem
 *
 * @param c         LittleFS config
 * @return 0 always
 */
static int lfs_port_unlock(const struct lfs_config *c)
{
    struct lfs_port_context *ctx = (struct lfs_port_context *)c->context;
    if (ctx->lock != NULL) {
#if configUSE_RECURSIVE_MUTEXES
        xSemaphoreGiveRecursive(ctx->lock);
#else
        xSemaphoreGive(ctx->lock);
#endif
    }
    return 0;
}
#endif

/**
 * @brief Initialize LittleFS configuration from partition
 *
 * @param lfs       LittleFS instance
 * @param ctx       LittleFS context (partition_name must be set)
 * @param cfg       LittleFS config to populate
 * @return 0 on success, -1 on failure
 */
int lfs_port_init(lfs_t *lfs, struct lfs_port_context *ctx, struct lfs_config *cfg)
{
    int ret;

    if (lfs == NULL || ctx == NULL || cfg == NULL) {
        return -1;
    }

    if (ctx->partition_name == NULL) {
        ctx->partition_name = LFS_DEFAULT_PARTITION_NAME;
    }

#if CONFIG_LFS_USE_MTD
    /* MTD implementation (original) */
    bflb_mtd_handle_t handle;
    bflb_mtd_info_t info;

    ret = bflb_mtd_open(ctx->partition_name, &handle, BFLB_MTD_OPEN_FLAG_BUSADDR);
    if (ret < 0) {
        printf("LittleFS: Partition '%s' not found\r\n", ctx->partition_name);
        return -1;
    }

    bflb_mtd_info(handle, &info);
    ctx->flash_addr = info.offset;
    ctx->flash_size = info.size;

    printf("LittleFS: Partition '%s' found at 0x%08x, size %u bytes\r\n",
           ctx->partition_name, info.offset, info.size);

    bflb_mtd_close(handle);
#else
    /* Pure LHAL boot2 implementation */
    uint32_t addr = 0;
    uint32_t size = 0;

    ret = bflb_boot2_partition_addr_active(ctx->partition_name, &addr, &size);
    if (ret != 0 || addr == 0) {
        printf("LittleFS: Partition '%s' not found\r\n", ctx->partition_name);
        return -1;
    }

    ctx->flash_addr = addr;
    ctx->flash_size = size;

    printf("LittleFS: Partition '%s' found at 0x%08x, size %u bytes\r\n",
           ctx->partition_name, addr, size);
#endif

    /* Configure lfs_config */
    memset(cfg, 0, sizeof(struct lfs_config));
    cfg->context = ctx;
    cfg->read = lfs_port_flash_read;
    cfg->prog = lfs_port_flash_prog;
    cfg->erase = lfs_port_flash_erase;
    cfg->sync = lfs_port_flash_sync;

#if defined(LFS_THREADSAFE) && defined(CONFIG_FREERTOS)
    cfg->lock = lfs_port_lock;
    cfg->unlock = lfs_port_unlock;

    /* Create mutex for thread safety */
#if configUSE_RECURSIVE_MUTEXES
    ctx->lock = xSemaphoreCreateRecursiveMutex();
#else
    ctx->lock = xSemaphoreCreateMutex();
#endif
#endif

    /* Block configuration - use standard 4KB erase block size */
    cfg->read_size = 256;       /* Minimum read unit */
    cfg->prog_size = 256;       /* Minimum program unit */
    cfg->block_size = 4096;     /* Erase block size (typically 4KB) */
    cfg->block_count = ctx->flash_size / cfg->block_size;

    /* Cache configuration */
    cfg->cache_size = 512;      /* Read/write cache size */
    cfg->lookahead_size = 256;  /* Lookahead buffer size */
    cfg->block_cycles = 500;    /* Wear leveling cycles */

    /* Allocate read/write/lookahead buffers if not provided */
    if (ctx->read_buffer == NULL) {
        ctx->read_buffer = (uint8_t *)malloc(cfg->cache_size);
        if (ctx->read_buffer == NULL) {
            printf("LittleFS: Failed to allocate read buffer\r\n");
            return -1;
        }
    }
    cfg->read_buffer = ctx->read_buffer;

    if (ctx->prog_buffer == NULL) {
        ctx->prog_buffer = (uint8_t *)malloc(cfg->cache_size);
        if (ctx->prog_buffer == NULL) {
            printf("LittleFS: Failed to allocate prog buffer\r\n");
            free(ctx->read_buffer);
            return -1;
        }
    }
    cfg->prog_buffer = ctx->prog_buffer;

    if (ctx->lookahead_buffer == NULL) {
        ctx->lookahead_buffer = (uint8_t *)malloc(cfg->lookahead_size);
        if (ctx->lookahead_buffer == NULL) {
            printf("LittleFS: Failed to allocate lookahead buffer\r\n");
            free(ctx->read_buffer);
            free(ctx->prog_buffer);
            return -1;
        }
    }
    cfg->lookahead_buffer = ctx->lookahead_buffer;

    return 0;
}

/**
 * @brief Check if filesystem is formatted by attempting to mount
 *
 * @param cfg       LittleFS config
 * @return true if formatted, false otherwise
 */
bool lfs_port_is_formatted(struct lfs_config *cfg)
{
    lfs_t lfs;
    int ret = lfs_mount(&lfs, cfg);
    if (ret == 0) {
        lfs_unmount(&lfs);
        return true;
    }
    return false;
}

/**
 * @brief Mount LittleFS with auto-format on first use
 *
 * @param lfs       LittleFS instance
 * @param cfg       LittleFS config
 * @return 0 on success, -1 on failure
 */
int lfs_port_mount(lfs_t *lfs, struct lfs_config *cfg)
{
    int ret;

    if (lfs == NULL || cfg == NULL) {
        return -1;
    }

    /* Try to mount */
    ret = lfs_mount(lfs, cfg);

    if (ret != 0) {
        /* Mount failed, likely first use or corrupted - try formatting */
        printf("LittleFS: Mount failed (ret=%d), formatting...\r\n", ret);

        ret = lfs_format(lfs, cfg);
        if (ret != 0) {
            printf("LittleFS: Format failed (ret=%d)\r\n", ret);
            return -1;
        }

        printf("LittleFS: Format successful, remounting...\r\n");

        /* Try mounting again after format */
        ret = lfs_mount(lfs, cfg);
        if (ret != 0) {
            printf("LittleFS: Remount failed (ret=%d)\r\n", ret);
            return -1;
        }

        printf("LittleFS: Mounted successfully (fresh format)\r\n");
    } else {
        printf("LittleFS: Mounted successfully\r\n");
    }

    return 0;
}

/**
 * @brief Full LittleFS initialization with partition and POSIX registration
 *
 * This is a convenience function that:
 * 1. Initializes flash
 * 2. Opens partition via MTD
 * 3. Configures LittleFS
 * 4. Mounts with auto-format
 * 5. Registers with POSIX layer
 *
 * @param lfs           LittleFS instance (static allocation)
 * @param ctx           LittleFS context (static allocation)
 * @param cfg           LittleFS config (static allocation)
 * @param partition_name Partition name (e.g., "PSM", "LittleFS")
 * @param mount_point   POSIX mount point (e.g., "/lfs")
 * @return 0 on success, -1 on failure
 */
int lfs_port_full_init(lfs_t *lfs, struct lfs_port_context *ctx,
                       struct lfs_config *cfg,
                       const char *partition_name,
                       const char *mount_point)
{
    int ret;

    if (lfs == NULL || ctx == NULL || cfg == NULL) {
        return -1;
    }

#if CONFIG_LFS_USE_MTD
    /* Initialize MTD */
    bflb_mtd_init();
#else
    /* Initialize boot2 partition table */
    bflb_boot2_init();
#endif

    /* Set partition name */
    ctx->partition_name = partition_name ? partition_name : LFS_DEFAULT_PARTITION_NAME;

    /* Initialize config from partition */
    ret = lfs_port_init(lfs, ctx, cfg);
    if (ret != 0) {
        printf("LittleFS: Init failed\r\n");
        return -1;
    }

    /* Mount with auto-format */
    ret = lfs_port_mount(lfs, cfg);
    if (ret != 0) {
        printf("LittleFS: Mount failed\r\n");
        return -1;
    }

    /* Register with POSIX layer */
    ret = lfs_port_register_fs(lfs, mount_point ? mount_point : LFS_DEFAULT_MOUNT_POINT);
    if (ret != 0) {
        printf("LittleFS: POSIX registration failed\r\n");
        lfs_unmount(lfs);
        return -1;
    }

    printf("LittleFS: Ready at %s (%u KB)\r\n",
           mount_point ? mount_point : LFS_DEFAULT_MOUNT_POINT,
           ctx->flash_size / 1024);

    return 0;
}

/**
 * @brief Cleanup LittleFS resources
 *
 * @param lfs       LittleFS instance
 * @param ctx       LittleFS context
 */
void lfs_port_cleanup(lfs_t *lfs, struct lfs_port_context *ctx)
{
    if (lfs != NULL) {
        lfs_port_unregister_fs(lfs);
        lfs_unmount(lfs);
    }

    if (ctx != NULL) {
#if defined(LFS_THREADSAFE) && defined(CONFIG_FREERTOS)
        if (ctx->lock != NULL) {
            vSemaphoreDelete(ctx->lock);
            ctx->lock = NULL;
        }
#endif
        if (ctx->read_buffer != NULL) {
            free(ctx->read_buffer);
            ctx->read_buffer = NULL;
        }
        if (ctx->prog_buffer != NULL) {
            free(ctx->prog_buffer);
            ctx->prog_buffer = NULL;
        }
        if (ctx->lookahead_buffer != NULL) {
            free(ctx->lookahead_buffer);
            ctx->lookahead_buffer = NULL;
        }
    }
}
