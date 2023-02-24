/* syscalls.c -- renntrant syscalls for bouffalo sdk.
 */

#include <errno.h>
#include <reent.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

/*****************************************************************************
* @brief        get thread reent
* 
* 
* @retval struct _reent*    pointer to reentrant struct
*****************************************************************************/
struct _reent *__getreent(void)
{
    return _impure_ptr;
}

/*****************************************************************************
* @brief        exit
* 
* @param[in]    rc          exit status
* 
*****************************************************************************/
void _exit(int rc)
{
    fprintf(stderr, "exit status %d\r\n", rc);
    for (;;) {}
}

void exit(int code)
{
    _exit(code);
}

/*****************************************************************************
* @brief        execve (not supported)
* 
* @param[in]    reent       
* @param[in]    name        
* @param[in]    argv        
* @param[in]    env         
* 
* @retval int               
*****************************************************************************/
int _execve_r(struct _reent *reent, const char *name, char *const *argv, char *const *env)
{
    reent->_errno = ENOSYS;
    return -1;
}

/*****************************************************************************
* @brief        fork (not supported)
* 
* @param[in]    reent       
* 
* @retval int               
*****************************************************************************/
int _fork_r(struct _reent *reent)
{
    reent->_errno = ENOSYS;
    return -1;
}

/*****************************************************************************
* @brief        getpid (not supported)
* 
* @param[in]    reent       
* 
* @retval int               
*****************************************************************************/
int _getpid_r(struct _reent *reent)
{
    reent->_errno = ENOSYS;
    return -1;
}

/*****************************************************************************
* @brief        kill (not supported)
* 
* @param[in]    reent       
* @param[in]    pid         
* @param[in]    sig         
* 
* @retval int               
*****************************************************************************/
int _kill_r(struct _reent *reent, int pid, int sig)
{
    reent->_errno = ENOSYS;
    return -1;
}

/*****************************************************************************
* @brief        wait (not supported)
* 
* @param[in]    reent       
* @param[in]    status      
* 
* @retval int               
*****************************************************************************/
int _wait_r(struct _reent *reent, int *status)
{
    reent->_errno = ENOSYS;
    return -1;
}

/** @addtogroup file_stub
-----------------------------------------------------------------------------
* @{
----------------------------------------------------------------------------*/
extern int _open_tty_r(struct _reent *reent, const char *path, int flags, int mode);
extern int _close_tty_r(struct _reent *reent, int fd);
extern _ssize_t _read_tty_r(struct _reent *reent, int fd, void *ptr, size_t size);
extern _ssize_t _write_tty_r(struct _reent *reent, int fd, const void *ptr, size_t size);
extern int _fstat_tty_r(struct _reent *reent, int fd, struct stat *st);
extern int _stat_tty_r(struct _reent *reent, const char *path, struct stat *st);

extern int _open_file_r(struct _reent *reent, const char *path, int flags, int mode);
extern int _close_file_r(struct _reent *reent, int fd);
extern _ssize_t _read_file_r(struct _reent *reent, int fd, void *ptr, size_t size);
extern _ssize_t _write_file_r(struct _reent *reent, int fd, const void *ptr, size_t size);
extern _off_t _lseek_file_r(struct _reent *reent, int fd, _off_t offset, int whence);
extern int _rename_file_r(struct _reent *reent, const char *oldname, const char *newname);
extern int _unlink_file_r(struct _reent *reent, const char *path);
extern int _fstat_file_r(struct _reent *reent, int fd, struct stat *st);
extern int _stat_file_r(struct _reent *reent, const char *path, struct stat *st);
extern int _mkdir_file_r(struct _reent *reent, const char *path, int mode);
extern int _rmdir_file_r(struct _reent *reent, const char *path);
extern int _chmod_file_r(struct _reent *reent, const char *path, mode_t mode);
extern int _chdir_file_r(struct _reent *reent, const char *path);
extern char *_getcwd_file_r(struct _reent *reent, char *buf, size_t size);
extern int _chroot_file_r(struct _reent *reent, const char *path);

/*****************************************************************************
* @brief        chekc fd is a tty
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    fd          file descriptors
* 
* @retval int               0:not tty 1:tty
*****************************************************************************/
int _isatty_r(struct _reent *reent, int fd)
{
    struct stat buf;

    if (_fstat_r(reent, fd, &buf) < 0) {
        reent->_errno = EBADF;
        return 0;
    }

    if (S_ISCHR(buf.st_mode)) {
        return 1;
    }

    reent->_errno = ENOTTY;
    return 0;
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
int _open_r(struct _reent *reent, const char *path, int flags, int mode)
{
    if (path == NULL) {
        reent->_errno = EINVAL;
        return -1;
    }

    if (strncmp("/dev", path, 4) == 0) {
        return _open_tty_r(reent, path, flags, mode);
    } else {
        return _open_file_r(reent, path, flags, mode);
    }
}

/*****************************************************************************
* @brief        close 
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    fd          file descriptors
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int _close_r(struct _reent *reent, int fd)
{
    if ((fd & 0x4000) == 0) {
        return _close_tty_r(reent, fd);
    } else {
        return _close_file_r(reent, fd);
    }
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
_ssize_t _read_r(struct _reent *reent, int fd, void *ptr, size_t size)
{
    if (ptr == NULL) {
        reent->_errno = EINVAL;
        return 0;
    }

    if ((fd & 0x4000) == 0) {
        return _read_tty_r(reent, fd, ptr, size);
    } else {
        return _read_file_r(reent, fd, ptr, size);
    }
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
_ssize_t _write_r(struct _reent *reent, int fd, const void *ptr, size_t size)
{
    if (ptr == NULL) {
        reent->_errno = EINVAL;
        return 0;
    }

    if ((fd & 0x4000) == 0) {
        return _write_tty_r(reent, fd, ptr, size);
    } else {
        return _write_file_r(reent, fd, ptr, size);
    }
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
_off_t _lseek_r(struct _reent *reent, int fd, _off_t offset, int whence)
{
    if ((fd & 0x4000) == 0) {
        reent->_errno = ESPIPE;
        return -1;
    } else {
        return _lseek_file_r(reent, fd, offset, whence);
    }
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
int _rename_r(struct _reent *reent, const char *oldname, const char *newname)
{
    if ((oldname == NULL) || (newname == NULL)) {
        reent->_errno = EINVAL;
        return -1;
    }

    return _rename_file_r(reent, oldname, newname);
}

/*****************************************************************************
* @brief        unlink
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    path        file path
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int _unlink_r(struct _reent *reent, const char *path)
{
    if (path == NULL) {
        reent->_errno = EINVAL;
        return -1;
    }

    return _unlink_file_r(reent, path);
}

/*****************************************************************************
* @brief        link (not supported)
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    existing    old file path
* @param[in]    new         new file path
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int _link_r(struct _reent *reent, const char *existing, const char *new)
{
    reent->_errno = ENOSYS;
    return -1;
}

/*****************************************************************************
* @brief        fcntl (not supported)
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    fd          file descriptors
* @param[in]    cmd         command
* @param[in]    arg         argument
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int _fcntl_r(struct _reent *reent, int fd, int cmd, int arg)
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
int _fstat_r(struct _reent *reent, int fd, struct stat *st)
{
    if (st == NULL) {
        reent->_errno = EINVAL;
        return -1;
    }

    memset(st, 0, sizeof(*st));

    if ((fd & 0x4000) == 0) {
        return _fstat_tty_r(reent, fd, st);
    } else {
        return _fstat_file_r(reent, fd, st);
    }
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
int _stat_r(struct _reent *reent, const char *path, struct stat *st)
{
    if ((path == NULL) || (st == NULL)) {
        reent->_errno = EINVAL;
        return -1;
    }

    memset(st, 0, sizeof(*st));

    if (strncmp("/dev", path, 4) == 0) {
        return _stat_tty_r(reent, path, st);
    } else {
        return _stat_file_r(reent, path, st);
    }
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
int _mkdir_r(struct _reent *reent, const char *path, int mode)
{
    if (path == NULL) {
        reent->_errno = EINVAL;
        return -1;
    }

    return _mkdir_file_r(reent, path, mode);
}

int mkdir(const char *path, mode_t mode)
{
    return _mkdir_r(_REENT, path, mode);
}

/*****************************************************************************
* @brief        rmdir 
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    path        dir path
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int _rmdir_r(struct _reent *reent, const char *path)
{
    if (path == NULL) {
        reent->_errno = EINVAL;
        return -1;
    }

    return _rmdir_file_r(reent, path);
}

int rmdir(const char *path)
{
    return _rmdir_r(_REENT, path);
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
int _chmod_r(struct _reent *reent, const char *path, mode_t mode)
{
    if (path == NULL) {
        reent->_errno = EINVAL;
        return -1;
    }

    if (strncmp("/dev", path, 4) == 0) {
        reent->_errno = EPERM;
        return -1;
    } else {
        return _chmod_file_r(reent, path, mode);
    }
}

int chmod(const char *path, mode_t mode)
{
    return _chmod_r(_REENT, path, mode);
}

/*****************************************************************************
* @brief        chdir
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    path        dir path
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int _chdir_r(struct _reent *reent, const char *path)
{
    if (path == NULL) {
        reent->_errno = EINVAL;
        return -1;
    }

    return _chdir_file_r(reent, path);
}

int chdir(const char *path)
{
    return _chdir_r(_REENT, path);
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
char *_getcwd_r(struct _reent *reent, char *buf, size_t size)
{
    if (buf == NULL) {
        reent->_errno = EINVAL;
        return NULL;
    }

    return _getcwd_file_r(reent, buf, size);
}

char *getcwd(char *buf, size_t size)
{
    return _getcwd_r(_REENT, buf, size);
}

/*****************************************************************************
* @brief        chroot
* 
* @param[in]    path        path
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int _chroot_r(struct _reent *reent, const char *path)
{
    if (path == NULL) {
        reent->_errno = EINVAL;
        return 0;
    }

    return _chroot_file_r(reent, path);
}

int chroot(const char *path)
{
    return _chroot_r(_REENT, path);
}

/*---------------------------------------------------------------------------
* @}            file_stub
----------------------------------------------------------------------------*/


