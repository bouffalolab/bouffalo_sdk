#include <errno.h>
#include <string.h>

#include "reent.h"
#include "mm.h"

#define DBG_TAG "LIBC"
#include "log.h"

/*****************************************************************************
* @brief        strlcpy - string copy with size limit
*
* @param[in]    dst         destination buffer
* @param[in]    src         source string
* @param[in]    siz         size of destination buffer
*
* @retval size_t            length of src (excluding null)
*****************************************************************************/
#if defined(__PICOLIBC__)
size_t strlcpy(char *dst, const char *src, size_t siz)
{
    char *d = dst;
    const char *s = src;
    size_t n = siz;

    /* Copy as many bytes as will fit */
    if (n != 0) {
        while (--n != 0) {
            if ((*d++ = *s++) == '\0') {
                break;
            }
        }
    }

    /* Not enough room in dst, add NUL and traverse rest of src */
    if (n == 0) {
        if (siz != 0) {
            *d = '\0';        /* NUL-terminate dst */
        }
        while (*s++) {
            ;                /* traverse rest of src */
        }
    }

    return (s - src - 1);    /* count does not include NUL */
}
#endif

/*****************************************************************************
* @brief        allocates space (disable)
*
* @param[in]    reent       pointer to reentrant struct
* @param[in]    incr        bytes of increment
*
* @retval void*             pointer to allocated memory
*****************************************************************************/
void *_sbrk_r(struct _reent *reent, ptrdiff_t incr)
{
    reent->_errno = ENOSYS;
    return NULL;
}

/*****************************************************************************
* @brief        allocate memory
*
* @param[in]    reent       pointer to reentrant struct
* @param[in]    size        bytes of allocated memory
*
* @retval void*             pointer to allocated memory
*****************************************************************************/
void *_malloc_r(struct _reent *reent, size_t size)
{
    void *mem;

    mem = kmalloc(size, MM_FLAG_ALIGN_CACHE);

    if (mem == NULL) {
        reent->_errno = ENOMEM;
        LOG_W("Failed to allocate %zu bytes\r\n", size);
    }

    return mem;
}

/*****************************************************************************
* @brief        reallocate memory
*
* @param[in]    reent       pointer to reentrant struct
* @param[in]    old         pointer to old allocated memory
* @param[in]    newlen      bytes of new allocated memory
*
* @retval void*             pointer to new allocated memory
*****************************************************************************/
void *_realloc_r(struct _reent *reent, void *old, size_t newlen)
{
    void *mem;

    mem = krealloc(old, newlen);

    if (mem == NULL) {
        reent->_errno = ENOMEM;
        LOG_W("Failed to reallocate %zu bytes\r\n", newlen);
    }

    return mem;
}

/*****************************************************************************
* @brief        allocate and zero memory
*
* @param[in]    reent       pointer to reentrant struct
* @param[in]    nmenb
* @param[in]    size
*
* @retval void*             pointer to allocated memory
*****************************************************************************/
void *_calloc_r(struct _reent *reent, size_t nmenb, size_t size)
{
    void *mem;

    mem = kmalloc(size * nmenb, MM_FLAG_ALIGN_CACHE | MM_FLAG_PROP_ZERO);

    if (mem == NULL) {
        reent->_errno = ENOMEM;
        LOG_W("Failed to allocate %zu bytes\r\n", size);
    }

    return mem;
}

/*****************************************************************************
* @brief        allocate aligned memory
*
* @param[in]    reent       pointer to reentrant struct
* @param[in]    align       align bytes
* @param[in]    size        bytes of allocated memory
*
* @retval void*             pointer to new allocated memory
*****************************************************************************/
void *_memalign_r(struct _reent *reent, size_t align, size_t size)
{
    void *mem;

    mem = kmemalign(size, align);

    if (mem == NULL) {
        reent->_errno = ENOMEM;
        LOG_W("Failed to allocate %zu bytes\r\n", size);
    }

    return mem;
}

/*****************************************************************************
* @brief        free memory
*
* @param[in]    reent       pointer to reentrant struct
* @param[in]    addr        pointer to allocated memory
*
*****************************************************************************/
void _free_r(struct _reent *reent, void *addr)
{
    if (addr == NULL) {
        reent->_errno = EINVAL;
        return;
    }

    kfree(addr);
}

/*****************************************************************************
* @brief        allocate memory (non-reentrant, overrides picolibc)
*
* @param[in]    size        bytes to allocate
*
* @retval void*             pointer to allocated memory
*****************************************************************************/
#if defined(__PICOLIBC__)
void *malloc(size_t size)
{
    struct _reent *reent = _REENT;
    return _malloc_r(reent, size);
}
#endif

/*****************************************************************************
* @brief        free memory (non-reentrant, overrides picolibc)
*
* @param[in]    addr        pointer to memory to free
*****************************************************************************/
#if defined(__PICOLIBC__)
void free(void *addr)
{
    struct _reent *reent = _REENT;
    _free_r(reent, addr);
}
#endif

/*****************************************************************************
* @brief        allocate and zero memory (non-reentrant, overrides picolibc)
*
* @param[in]    nmenb       number of elements
* @param[in]    size        bytes per element
*
* @retval void*             pointer to allocated memory
*****************************************************************************/
#if defined(__PICOLIBC__)
void *calloc(size_t nmenb, size_t size)
{
    struct _reent *reent = _REENT;
    return _calloc_r(reent, nmenb, size);
}
#endif

/*****************************************************************************
* @brief        reallocate memory (non-reentrant, overrides picolibc)
*
* @param[in]    old         pointer to old memory
* @param[in]    newlen      new size in bytes
*
* @retval void*             pointer to reallocated memory
*****************************************************************************/
#if defined(__PICOLIBC__)
void *realloc(void *old, size_t newlen)
{
    struct _reent *reent = _REENT;
    return _realloc_r(reent, old, newlen);
}
#endif

/*****************************************************************************
* @brief        allocate aligned memory (non-reentrant, overrides picolibc)
*
* @param[in]    align       alignment in bytes
* @param[in]    size        bytes to allocate
*
* @retval void*             pointer to allocated memory
*****************************************************************************/
#if defined(__PICOLIBC__)
void *memalign(size_t align, size_t size)
{
    struct _reent *reent = _REENT;
    return _memalign_r(reent, align, size);
}
#endif


///* POSIX random 要求返回 0 到 2^31-1 (2147483647) */
//#define RANDOM_MAX 2147483647
//
//static uint32_t g_random_seed = 1;
//
///* 设置种子 */
//void srandom(unsigned seed) {
//    g_random_seed = seed ? seed : 1;  /* POSIX: seed为0时视为1 */
//}
//
///* 返回 0 到 2147483647 之间的随机数 */
//long random(void) {
//    /* 最简单的线性同余，但保证31位输出 */
//    g_random_seed = (g_random_seed * 1103515245 + 12345) & RANDOM_MAX;
//    return (long)g_random_seed;
//}
