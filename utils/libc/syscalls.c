#include <reent.h>
#include <errno.h>
#include <unistd.h>
#include "mmheap.h"
#include "bflb_uart.h"

extern struct heap_info mmheap_root;

extern struct bflb_device_s *console;

/* Reentrant versions of system calls.  */

/* global errno */
static volatile int _sys_errno = 0;

#ifndef _REENT_ONLY
int *__errno()
{
    return (int *)&_sys_errno;
}
#endif

int _getpid_r(struct _reent *ptr)
{
    return 0;
}

int _execve_r(struct _reent *ptr, const char *name, char *const *argv, char *const *env)
{
    /* return "not supported" */
    ptr->_errno = -ENOSYS;
    return -1;
}

int _fcntl_r(struct _reent *ptr, int fd, int cmd, int arg)
{
    /* return "not supported" */
    ptr->_errno = -ENOSYS;
    return -1;
}

int _fork_r(struct _reent *ptr)
{
    /* return "not supported" */
    ptr->_errno = -ENOSYS;
    return -1;
}

int _fstat_r(struct _reent *ptr, int fd, struct stat *pstat)
{
    /* return "not supported" */
    ptr->_errno = -ENOSYS;
    return -1;
}

int _isatty_r(struct _reent *ptr, int fd)
{
    /* return "not supported" */
    ptr->_errno = -ENOSYS;
    return -1;
}

int _kill_r(struct _reent *ptr, int pid, int sig)
{
    /* return "not supported" */
    ptr->_errno = -ENOSYS;
    return -1;
}

int _link_r(struct _reent *ptr, const char *old, const char *new)
{
    /* return "not supported" */
    ptr->_errno = -ENOSYS;
    return -1;
}

_off_t _lseek_r(struct _reent *ptr, int fd, _off_t pos, int whence)
{
    /* return "not supported" */
    ptr->_errno = -ENOSYS;
    return -1;
}

int _mkdir_r(struct _reent *ptr, const char *name, int mode)
{
    /* return "not supported" */
    ptr->_errno = -ENOSYS;
    return -1;
}

int _open_r(struct _reent *ptr, const char *file, int flags, int mode)
{
    /* return "not supported" */
    ptr->_errno = -ENOSYS;
    return -1;
}

int _close_r(struct _reent *ptr, int fd)
{
    /* return "not supported" */
    ptr->_errno = -ENOSYS;
    return -1;
}

_ssize_t _read_r(struct _reent *ptr, int fd, void *buf, size_t nbytes)
{
    /* return "not supported" */
    ptr->_errno = -ENOSYS;
    return -1;
}

int _rename_r(struct _reent *ptr, const char *old, const char *new)
{
    /* return "not supported" */
    ptr->_errno = -ENOSYS;
    return -1;
}

int _stat_r(struct _reent *ptr, const char *file, struct stat *pstat)
{
    /* return "not supported" */
    ptr->_errno = -ENOSYS;
    return -1;
}

int _unlink_r(struct _reent *ptr, const char *file)
{
    /* return "not supported" */
    ptr->_errno = -ENOSYS;
    return -1;
}

int _wait_r(struct _reent *ptr, int *status)
{
    /* return "not supported" */
    ptr->_errno = -ENOSYS;
    return -1;
}

_ssize_t _write_r(struct _reent *ptr, int fd, const void *buf, size_t nbytes)
{
    if ((STDOUT_FILENO == fd) || (STDERR_FILENO == fd)) {
        bflb_uart_put(console, (uint8_t *)buf, nbytes);
    }
    return 0;
}

void *_malloc_r(struct _reent *ptr, size_t size)
{
    void *result;
#ifdef CONFIG_MEM_USE_FREERTOS
    result = pvPortMalloc(size);
#else
    result = (void *)mmheap_alloc(&mmheap_root, size);
    if (result == NULL) {
        ptr->_errno = -ENOMEM;
    }
#endif
    return result;
}

void *_realloc_r(struct _reent *ptr, void *old, size_t newlen)
{
    void *result;
#ifdef CONFIG_MEM_USE_FREERTOS
#else
    result = (void *)mmheap_realloc(&mmheap_root, old, newlen);
#endif
    if (result == NULL) {
        ptr->_errno = -ENOMEM;
    }
    return result;
}

void *_calloc_r(struct _reent *ptr, size_t size, size_t len)
{
    void *result;
#ifdef CONFIG_MEM_USE_FREERTOS
    result = pvPortMalloc(size * len);
    if (result) {
        memset(result, 0, size * len);
    }
#else
    result = (void *)mmheap_calloc(&mmheap_root, size, len);
    if (result == NULL) {
        ptr->_errno = -ENOMEM;
    }
#endif
    return result;
}

void *_memalign_r(struct _reent *ptr, size_t align, size_t size)
{
    void *result;
#ifdef CONFIG_MEM_USE_FREERTOS
#else
    result = (void *)mmheap_align_alloc(&mmheap_root, align, size);
#endif
    if (result == NULL) {
        ptr->_errno = -ENOMEM;
    }

    return result;
}

void _free_r(struct _reent *ptr, void *addr)
{
#ifdef CONFIG_MEM_USE_FREERTOS
    vPortFree(addr);
#else
    mmheap_free(&mmheap_root, addr);
#endif
}

void *_sbrk_r(struct _reent *ptr, ptrdiff_t incr)
{
    void *ret;
    ptr->_errno = ENOMEM;
    ret = (void *)-1;

    return ret;
}

/* for exit() and abort() */
void __attribute__((noreturn))
_exit(int status)
{
    while (1) {
    }
}

void _system(const char *s)
{
}

void __libc_init_array(void)
{
    /* we not use __libc init_aray to initialize C++ objects */
}

mode_t umask(mode_t mask)
{
    return 022;
}

int flock(int fd, int operation)
{
    return 0;
}

/*
These functions are implemented and replaced by the 'common/time.c' file
int _gettimeofday_r(struct _reent *ptr, struct timeval *__tp, void *__tzp);
_CLOCK_T_  _times_r(struct _reent *ptr, struct tms *ptms);
*/
