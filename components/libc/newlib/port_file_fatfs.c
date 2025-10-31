#include <errno.h>
#include <reent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#include "ff.h"

int fresult_table[20] = {
    0,      /* (0) Succeeded */
    EIO,    /* (1) A hard error occurred in the low level disk I/O layer */
    EINVAL, /* (2) Assertion failed */
    EIO,    /* (3) The physical drive cannot work */
    ENOENT, /* (4) Could not find the file */
    ENOENT, /* (5) Could not find the path */
    EBADF,  /* (6) The path name format is invalid */
    EACCES, /* (7) Access denied due to prohibited access or directory full */
    EEXIST, /* (8) Access denied due to prohibited access */
    ENOENT, /* (9) The file/directory object is invalid */
    EROFS,  /* (10) The physical drive is write protected */
    ENODEV, /* (11) The logical drive number is invalid */
    ENODEV, /* (12) The volume has no work area */
    EPIPE,  /* (13) There is no valid FAT volume */
    EIO,    /* (14) The f_mkfs() aborted due to any problem */
    EBUSY,  /* (15) Could not get a grant to access the volume within defined period */
    EACCES, /* (16) The operation is rejected according to the file sharing policy */
    ENOMEM, /* (17) LFN working buffer could not be allocated */
    EMFILE, /* (18) Number of open files > FF_FS_LOCK */
    EINVAL  /* (19) Given parameter is invalid */
};

static void *fd_table[FOPEN_MAX] = { NULL };

/*****************************************************************************
* @brief        c99 mode to fatfs mode
* 
* @param[in]    mode        c99 mode
* 
* @retval uint8_t           fatfs mode
*****************************************************************************/
static uint8_t _open_r_helper_mode_to_fatfs(int mode)
{
    uint8_t fatfs_mode;

    switch (mode & O_ACCMODE) {
        case O_RDONLY:
            fatfs_mode = FA_READ;
            break;
        case O_WRONLY:
            fatfs_mode = FA_WRITE;
            break;
        case O_RDWR:
            fatfs_mode = (FA_READ | FA_WRITE);
            break;
        default:
            fatfs_mode = FA_READ;
            break;
    }

    if ((mode & (O_CREAT | O_TRUNC)) == (O_CREAT | O_TRUNC)) {
        fatfs_mode |= FA_CREATE_ALWAYS;
    } else if (mode & O_APPEND) {
        fatfs_mode |= FA_OPEN_APPEND;
    } else if (mode & O_EXCL) {
        fatfs_mode |= FA_CREATE_NEW;
    } else if (mode & O_CREAT) {
        fatfs_mode |= FA_OPEN_ALWAYS;
    }

    return fatfs_mode;
}

/*****************************************************************************
* @brief        open
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    path        file path string pointer
* @param[in]    flags       open mode in fcntl.h
* @param[in]    mode        permission mode
* 
* @retval int               >=0:fd -1:Error
*****************************************************************************/
int _open_file_r(struct _reent *reent, const char *path, int flags, int mode)
{
    FRESULT fresult;
    FIL *fp;
    int fd = -1;

    for (int i = 0; i < FOPEN_MAX; i++) {
        if (fd_table[i] == NULL) {
            fd_table[i] = (void *)0xffffffff;
            fd = i;
            break;
        }
    }

    if (fd == -1) {
        reent->_errno = EMFILE;
        return -1;
    }

    fp = (FIL *)malloc(sizeof(FIL));
    if (fp == NULL) {
        reent->_errno = ENOMEM;
        fd_table[fd] = NULL;
        return -1;
    }

    fresult = f_open(fp, path, _open_r_helper_mode_to_fatfs(flags));

    if (fresult != FR_OK) {
        if (fresult <= FR_INVALID_PARAMETER) {
            reent->_errno = fresult_table[fresult];
        }
        free(fp);
        fd_table[fd] = NULL;
        return -1;
    }

    fd_table[fd] = fp;

    return 0x4000 | fd;
}

/*****************************************************************************
* @brief        close 
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    fd          file descriptors
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int _close_file_r(struct _reent *reent, int fd)
{
    FRESULT fresult;
    FIL *fp;

    fd &= ~0x4000;

    if ((fd < 0) || (fd >= FOPEN_MAX) || (fd_table[fd] == NULL)) {
        reent->_errno = EBADF;
        return -1;
    }

    fp = fd_table[fd];

    fresult = f_close(fp);
    if (fresult != FR_OK) {
        if (fresult <= FR_INVALID_PARAMETER) {
            reent->_errno = fresult_table[fresult];
        }
        return -1;
    }

    free(fp);
    fd_table[fd] = NULL;

    return 0;
}

/*****************************************************************************
* @brief        read
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    fd          file descriptors
* @param[in]    ptr         pointer to buffer
* @param[in]    size        number of bytes read
* 
* @retval _ssize_t          actual number of bytes read
*****************************************************************************/
_ssize_t _read_file_r(struct _reent *reent, int fd, void *ptr, size_t size)
{
    size_t bytes = 0;
    FRESULT fresult;
    FIL *fp;

    fd &= ~0x4000;

    if ((fd < 0) || (fd >= FOPEN_MAX) || (fd_table[fd] == NULL)) {
        reent->_errno = EBADF;
        return -1;
    }

    fp = fd_table[fd];

    fresult = f_read(fp, ptr, size, &bytes);
    if (fresult != FR_OK) {
        if (fresult <= FR_INVALID_PARAMETER) {
            reent->_errno = fresult_table[fresult];
        }
    }

    return bytes;
}

/*****************************************************************************
* @brief        write
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    fd          file descriptors
* @param[in]    ptr         pointer to buffer
* @param[in]    size        number of bytes write
* 
* @retval _ssize_t          actual number of bytes write
*****************************************************************************/
_ssize_t _write_file_r(struct _reent *reent, int fd, const void *ptr, size_t size)
{
    size_t bytes = 0;
    FRESULT fresult;
    FIL *fp;

    fd &= ~0x4000;

    if ((fd < 0) || (fd >= FOPEN_MAX) || (fd_table[fd] == NULL)) {
        reent->_errno = EBADF;
        return -1;
    }

    fp = fd_table[fd];

    fresult = f_write(fp, ptr, size, &bytes);
    if (fresult != FR_OK) {
        if (fresult <= FR_INVALID_PARAMETER) {
            reent->_errno = fresult_table[fresult];
        }
    } else {
        f_sync(fp);
    }

    return bytes;
}

/*****************************************************************************
* @brief        lseek
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    fd          file descriptors
* @param[in]    offset      number of offset bytes
* @param[in]    whence      offset type
* 
* @retval _off_t            absolute offset of file pointer
*****************************************************************************/
_off_t _lseek_file_r(struct _reent *reent, int fd, _off_t offset, int whence)
{
    FRESULT fresult;
    FIL *fp;

    fd &= ~0x4000;
    if ((fd < 0) || (fd >= FOPEN_MAX) || (fd_table[fd] == NULL)) {
        reent->_errno = EBADF;
        return -1;
    }

    fp = fd_table[fd];

    switch (whence) {
        case SEEK_SET:
            fresult = f_lseek(fp, offset);
            break;
        case SEEK_CUR:
            fresult = f_lseek(fp, f_tell((FIL *)fp) + offset);
            break;
        case SEEK_END:
            fresult = f_lseek(fp, f_size((FIL *)fp) + offset);
            break;
        default:
            reent->_errno = EINVAL;
            return -1;
    }

    if (fresult != FR_OK) {
        if (fresult <= FR_INVALID_PARAMETER) {
            errno = fresult_table[fresult];
        }
        return EOF;
    }

    return f_tell((FIL *)fp);
}

/*****************************************************************************
* @brief        rename
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    oldname     old file path
* @param[in]    newname     new file path
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int _rename_file_r(struct _reent *reent, const char *oldname, const char *newname)
{
    FRESULT fresult;

    fresult = f_rename(oldname, newname);

    if (fresult != FR_OK) {
        if (fresult <= FR_INVALID_PARAMETER) {
            reent->_errno = fresult_table[fresult];
        }
        return -1;
    }

    return 0;
}

/*****************************************************************************
* @brief        unlink
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    path        file path
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int _unlink_file_r(struct _reent *reent, const char *path)
{
    FRESULT fresult;

    fresult = f_unlink(path);

    if (fresult != FR_OK) {
        if (fresult <= FR_INVALID_PARAMETER) {
            reent->_errno = fresult_table[fresult];
        }
        return -1;
    }

    return 0;
}

/*****************************************************************************
* @brief        fstat (not supported)
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    fd          file descriptors
* @param[out]   st          file stat
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int _fstat_file_r(struct _reent *reent, int fd, struct stat *st)
{
    FIL *fp;

    fd &= ~0x4000;

    if ((fd < 0) || (fd >= FOPEN_MAX) || (fd_table[fd] == NULL)) {
        reent->_errno = EBADF;
        return -1;
    }

    fp = fd_table[fd];

    st->st_mode = 0444 | S_IFREG;

    if (!(fp->obj.attr & AM_RDO)) {
        st->st_mode |= 0222;
    }

    st->st_size = f_size(fp);
    // st->st_atime = 0;
    // st->st_mtime = 0;
    // st->st_ctime = 0;

    return 0;
}

/*****************************************************************************
* @brief        stat
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    path        file path
* @param[out]   st          file stat
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int _stat_file_r(struct _reent *reent, const char *path, struct stat *st)
{
    FRESULT fresult;
    FILINFO fno;

    fresult = f_stat(path, &fno);

    if (fresult != FR_OK) {
        if (fresult <= FR_INVALID_PARAMETER) {
            reent->_errno = fresult_table[fresult];
        }
        return -1;
    }

    st->st_mode = 0444;

    if (!(fno.fattrib & AM_RDO)) {
        st->st_mode |= 0222;
    }

    if (fno.fattrib & AM_DIR) {
        st->st_mode |= S_IFDIR;
    } else {
        st->st_mode |= S_IFREG;
    }

    st->st_size = fno.fsize;

    time_t ftime_sec = ((fno.ftime >> 0) & 0x1F) * 2 +
                       ((fno.ftime >> 5) & 0x3F) * 60 +
                       ((fno.ftime >> 11) & 0x1F) * 3600;

    st->st_atime = ftime_sec;
    st->st_mtime = ftime_sec;
    st->st_ctime = ftime_sec;

    return 0;
}

/*****************************************************************************
* @brief        mkdir
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    path        dir path
* @param[in]    mode        mode
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int _mkdir_file_r(struct _reent *reent, const char *path, int mode)
{
    int fresult;

    fresult = f_mkdir(path);

    if (fresult != FR_OK) {
        if (fresult <= FR_INVALID_PARAMETER) {
            reent->_errno = fresult_table[fresult];
        }
        return -1;
    }

    return 0;
}

/*****************************************************************************
* @brief        rmdir 
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    path        dir path
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int _rmdir_file_r(struct _reent *reent, const char *path)
{
    int fresult;

    fresult = f_rmdir(path);

    if (fresult != FR_OK) {
        if (fresult <= FR_INVALID_PARAMETER) {
            reent->_errno = fresult_table[fresult];
        }
        return -1;
    }

    return 0;
}

/*****************************************************************************
* @brief        chmod
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    path        dir path
* @param[in]    mode        mode
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int _chmod_file_r(struct _reent *reent, const char *path, mode_t mode)
{
    FRESULT fresult;

    if ((mode & S_IRWXU) == S_IRUSR) {
        fresult = f_chmod(path, AM_RDO, AM_RDO);
    } else {
        fresult = f_chmod(path, 0, AM_RDO);
    }

    if (fresult != FR_OK) {
        if (fresult <= FR_INVALID_PARAMETER) {
            reent->_errno = fresult_table[fresult];
        }
        return -1;
    }

    return 0;
}

/*****************************************************************************
* @brief        chdir
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    path        dir path
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int _chdir_file_r(struct _reent *reent, const char *path)
{
    FRESULT fresult;

    fresult = f_chdir(path);

    if (fresult != FR_OK) {
        if (fresult <= FR_INVALID_PARAMETER) {
            reent->_errno = fresult_table[fresult];
        }
        return -1;
    }

    return 0;
}

/*****************************************************************************
* @brief        getcwd
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    buf         buf for path
* @param[in]    size        buf size
* 
* @retval char*             pointer to path string NULL:Error
*****************************************************************************/
char *_getcwd_file_r(struct _reent *reent, char *buf, size_t size)
{
    FRESULT fresult;

    fresult = f_getcwd(buf, size);

    if (fresult != FR_OK) {
        if (fresult <= FR_INVALID_PARAMETER) {
            reent->_errno = fresult_table[fresult];
        }
        return NULL;
    }

    return buf;
}

/*****************************************************************************
* @brief        chroot
* 
* @param[in]    path        path
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int _chroot_file_r(struct _reent *reent, const char *path)
{
    FRESULT fresult;

    fresult = f_chdrive("sd:/");

    if (fresult != FR_OK) {
        if (fresult <= FR_INVALID_PARAMETER) {
            reent->_errno = fresult_table[fresult];
        }
        return -1;
    }

    return 0;
}