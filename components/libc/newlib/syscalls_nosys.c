#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <stddef.h>

#include <reent.h>

/* Global reentrant structure */
#if defined(__PICOLIBC__)
static struct _reent _global_reent = {0};
struct _reent *_impure_ptr = &_global_reent;
#endif

char *__env[1] = { 0 };
char **environ = __env;

void _exit(int rc)
{
    for (;;)
        ;
}

int _chown(const char *path, uid_t owner, gid_t group)
{
    errno = ENOSYS;
    return -1;
}

int _close(int fildes)
{
    errno = ENOSYS;
    return -1;
}

int _execve(char *name, char **argv, char **env)
{
    errno = ENOSYS;
    return -1;
}

int _fork(void)
{
    errno = ENOSYS;
    return -1;
}

int _fstat(int fildes, struct stat *st)
{
    errno = ENOSYS;
    return -1;
}

int _getpid(void)
{
    errno = ENOSYS;
    return -1;
}

int _gettimeofday(struct timeval *ptimeval, void *ptimezone)
{
    errno = ENOSYS;
    return -1;
}

int _isatty(int file)
{
    errno = ENOSYS;
    return 0;
}

int _kill(int pid, int sig)
{
    errno = ENOSYS;
    return -1;
}

int _link(char *existing, char *new)
{
    errno = ENOSYS;
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    errno = ENOSYS;
    return -1;
}

int _open(char *file, int flags, int mode)
{
    errno = ENOSYS;
    return -1;
}

int _read(int file, char *ptr, int len)
{
    errno = ENOSYS;
    return -1;
}

int _readlink(const char *path, char *buf, size_t bufsize)
{
    errno = ENOSYS;
    return -1;
}

void *_sbrk(int incr)
{
    errno = ENOSYS;
    return NULL;
}

int _stat(const char *file, struct stat *st)
{
    errno = ENOSYS;
    return -1;
}

int _symlink(const char *path1, const char *path2)
{
    errno = ENOSYS;
    return -1;
}

clock_t _times(struct tms *buf)
{
    errno = ENOSYS;
    return -1;
}

int _unlink(char *name)
{
    errno = ENOSYS;
    return -1;
}

int _wait(int *status)
{
    errno = ENOSYS;
    return -1;
}

int _write(int file, char *ptr, int len)
{
    errno = ENOSYS;
    return -1;
}

#if defined(__PICOLIBC__)
int chroot(const char *path)
{
    errno = ENOSYS;
    return -1;
}

int open(const char *path, int flags, ...)
{
    errno = ENOSYS;
    return -1;
}

int close(int fd)
{
    errno = ENOSYS;
    return -1;
}

_ssize_t read(int fd, void *ptr, size_t size)
{
    errno = ENOSYS;
    return -1;
}

_ssize_t write(int fd, const void *ptr, size_t size)
{
    errno = ENOSYS;
    return -1;
}

_off_t lseek(int fd, _off_t offset, int whence)
{
    errno = ENOSYS;
    return -1;
}

int unlink(const char *path)
{
    errno = ENOSYS;
    return -1;
}

int rename(const char *oldpath, const char *newpath)
{
    errno = ENOSYS;
    return -1;
}
#endif
