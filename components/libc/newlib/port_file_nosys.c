#include <errno.h>
#include <reent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

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
    reent->_errno = ENOSYS;
    return -1;
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
    reent->_errno = ENOSYS;
    return -1;
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
    reent->_errno = ENOSYS;
    return -1;
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
    reent->_errno = ENOSYS;
    return -1;
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
    reent->_errno = ENOSYS;
    return -1;
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
    reent->_errno = ENOSYS;
    return -1;
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
    reent->_errno = ENOSYS;
    return -1;
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
    reent->_errno = ENOSYS;
    return -1;
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
    reent->_errno = ENOSYS;
    return -1;
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
    reent->_errno = ENOSYS;
    return -1;
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
    reent->_errno = ENOSYS;
    return -1;
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
    reent->_errno = ENOSYS;
    return -1;
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
    reent->_errno = ENOSYS;
    return -1;
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
    reent->_errno = ENOSYS;
    return NULL;
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
    reent->_errno = ENOSYS;
    return -1;
}