#include <errno.h>
#include <reent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include "lfs.h"
#include "lfs_port.h"
#include "at_fs.h"

static void *fd_table[FOPEN_MAX] = { NULL };
static lfs_t *lfs = NULL;

#ifndef LFS_MOUNTPOINT
#define LFS_MOUNTPOINT "/lfs"
#endif 

#ifndef LFS_MOUNT_PT_NAME
#define LFS_MOUNT_PT_NAME "media"
#endif 

static struct lfs_context lfs_ctx = { .partition_name = LFS_MOUNT_PT_NAME };
static struct lfs_config lfs_cfg = { .read_size = 256,
                                     .prog_size = 256,
                                     .lookahead_size = 256,
                                     .cache_size = 512,
                                     .block_size = 4096,
                                     .block_cycles = 500
                                   };

static int32_t mode_convert(int32_t flags)
{
    int32_t mode, res = 0;

    mode = flags & O_ACCMODE;
    if (mode == O_RDONLY) {
        res |= LFS_O_RDONLY;
    } else if (mode == O_WRONLY) {
        res |= LFS_O_WRONLY;
    } else if (mode == O_RDWR) {
        res |= LFS_O_RDWR;
    }

    if (flags & O_CREAT) {
        res |= LFS_O_CREAT;
    }
    if (flags & O_EXCL) {
        res |= LFS_O_EXCL;
    }
    if (flags & O_TRUNC) {
        res |= LFS_O_TRUNC;
    }
    if (flags & O_APPEND) {
        res |= LFS_O_CREAT | LFS_O_APPEND;
    }
    return res;
}

static int lfs_ret_value_convert(int lfs_ret)
{
    int ret;

    if (lfs_ret > 0) {
        return lfs_ret;
    }

    switch(lfs_ret) {
        case LFS_ERR_OK: ret = 0; break;
        case LFS_ERR_IO: ret = -EIO; break;
        case LFS_ERR_CORRUPT: ret = -EIO; break;
        case LFS_ERR_NOENT: ret = -ENOENT; break;
        case LFS_ERR_EXIST: ret = -EEXIST; break;
        case LFS_ERR_NOTDIR: ret = -ENOTDIR; break;
        case LFS_ERR_ISDIR: ret = -EISDIR; break;
        case LFS_ERR_NOTEMPTY: ret = -ENOTEMPTY; break;
        case LFS_ERR_BADF: ret = -EBADF; break;
        case LFS_ERR_FBIG: ret = -EFBIG; break;
        case LFS_ERR_INVAL: ret = -EINVAL; break;
        case LFS_ERR_NOSPC: ret = -ENOSPC; break;
        case LFS_ERR_NOMEM: ret = -ENOMEM; break;
        case LFS_ERR_NOATTR: ret = -ENODATA; break;
        case LFS_ERR_NAMETOOLONG: ret = -ENAMETOOLONG; break;
        default: ret = lfs_ret; break;
    }

    return ret;
}

int at_fs_open(const char *path, int flags)
{
    int fd = -1;
    lfs_file_t *fp = NULL;
    int res;
    char dirpath[AT_FILE_NAME_MAX];

    if (lfs == NULL) {
        return -1;
    }

    for (int i = 0; i < FOPEN_MAX; i++) {
        if (fd_table[i] == NULL) {
            fd_table[i] = (void *)0xffffffff;
            fd = i;
            break;
        }
    }

    if (fd == -1) {
        return -1;
    }

    fp = (lfs_file_t *)malloc(sizeof(lfs_file_t));
    if (fp == NULL) {
        fd_table[fd] = NULL;
        return -1;
    }
    snprintf(dirpath, sizeof(dirpath), LFS_MOUNTPOINT"/%s", path);

    res = lfs_file_open(lfs, fp,  dirpath, mode_convert(flags));

    if (res != LFS_ERR_OK) {
        free(fp);
        fd_table[fd] = NULL;
        return -1;
    }

    fd_table[fd] = fp;

    return 0x4000 | fd;
}

at_dir_t at_fs_opendir(const char *path)
{
    int fresult;
    lfs_dir_t *ldir;
    char dirpath[AT_FILE_NAME_MAX];

    if (lfs == NULL) {
        return NULL;
    }

    ldir = malloc(sizeof(lfs_dir_t));
    if (ldir == NULL) {
        return NULL;
    }

    snprintf(dirpath, sizeof(dirpath), LFS_MOUNTPOINT"/%s", path);
    fresult = lfs_dir_open(lfs, ldir, dirpath); 
    if (fresult != LFS_ERR_OK) {
        free(ldir);
        return NULL;
    }

    return (at_dir_t)ldir;
}

int at_fs_readdir(at_dir_t dirp, struct at_direct *res)
{
    struct lfs_info info;
    int fresult;

    if (lfs == NULL) {
        return -1;
    }

    fresult = lfs_dir_read(lfs, (lfs_dir_t *)dirp, &info);
    if (fresult != true) {
        return -1;
    }
    if (info.name[0] == '\0') {
        return -1;
    }
    res->d_type = info.type;
    strlcpy(res->d_name, info.name, sizeof(res->d_name));
    return 0;
}

int at_fs_closedir(at_dir_t dir)
{
    int fresult;

    if (lfs == NULL) {
        return -1;
    }

    fresult = lfs_dir_close(lfs, (lfs_dir_t *)dir); 
    if (fresult != LFS_ERR_OK) {
        return fresult;
    }

    free(dir);

    return 0;
}

int at_fs_remove(const char *path)
{
    int fresult;
    char dirpath[AT_FILE_NAME_MAX];

    if (lfs == NULL) {
        return -1;
    }

    snprintf(dirpath, sizeof(dirpath), LFS_MOUNTPOINT"/%s", path);
    fresult = lfs_remove(lfs, dirpath);
    if (fresult != LFS_ERR_OK) {
        return fresult;
    }

    return 0;
}

int at_fs_close(int fd)
{
    int fresult;
    lfs_file_t *fp;

    if (lfs == NULL) {
        return -1;
    }
    fd &= ~0x4000;

    if ((fd < 0) || (fd >= FOPEN_MAX) || (fd_table[fd] == NULL)) {
        return -1;
    }

    fp = fd_table[fd];

    fresult = lfs_file_close(lfs, fp);
    if (fresult != LFS_ERR_OK) {
        return -1;
    }

    free(fp);
    fd_table[fd] = NULL;

    return 0;
}

size_t at_fs_read(int fd, void *ptr, size_t size)
{
    int fresult;
    lfs_file_t *fp;

    if (lfs == NULL) {
        return -1;
    }
    fd &= ~0x4000;

    if ((fd < 0) || (fd >= FOPEN_MAX) || (fd_table[fd] == NULL)) {
        return -1;
    }

    fp = fd_table[fd];

    fresult = lfs_file_read(lfs, fp, ptr, size);
    if (fresult < 0) {
        //reent->_errno = lfs_ret_value_convert(fresult);
    }

    return fresult;
}

size_t at_fs_write(int fd, const void *ptr, size_t size)
{
    int fresult;
    lfs_file_t *fp;

    if (lfs == NULL) {
        return -1;
    }
    fd &= ~0x4000;

    if ((fd < 0) || (fd >= FOPEN_MAX) || (fd_table[fd] == NULL)) {
        return -1;
    }

    fp = fd_table[fd];

    fresult = lfs_file_write(lfs, fp, ptr, size);
    if (fresult < 0) {
        //reent->_errno = lfs_ret_value_convert(fresult);
    }

    return fresult;
}

size_t at_fs_lseek(int fd, _off_t offset, int whence)
{
    int fresult;
    lfs_file_t *fp;

    if (lfs == NULL) {
        return -1;
    }
    fd &= ~0x4000;
    if ((fd < 0) || (fd >= FOPEN_MAX) || (fd_table[fd] == NULL)) {
        return -1;
    }

    fp = fd_table[fd];

    fresult = lfs_file_seek(lfs, fp, offset, whence);
    if (fresult != LFS_ERR_OK) {
        //reent->_errno = lfs_ret_value_convert(fresult);
    }

    return fresult;
}

int at_fs_register(void)
{
    int ret = 0;
    struct lfs_info stat;

    lfs = lfs_xip_init(&lfs_ctx, &lfs_cfg);
    if (lfs == NULL) {
        return -1;
    }

    ret = lfs_stat(lfs, LFS_MOUNTPOINT, &stat);
    if (ret == LFS_ERR_OK) {
        if (stat.type == LFS_TYPE_DIR) {
            return 0;
        } else if (stat.type == LFS_TYPE_REG) {
            return -1;
        }
    }

    ret = lfs_mkdir(lfs, LFS_MOUNTPOINT);
    if (ret != LFS_ERR_OK) {
        errno = -ret;
        return -1;
    }
    return ret;
}

