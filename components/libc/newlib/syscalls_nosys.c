#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/stat.h>

#undef errno
int errno;

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