#include <errno.h>
#include <reent.h>

#include "mem.h"

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

    mem = bflb_malloc(PMEM_HEAP, size);

    if (mem == NULL) {
        reent->_errno = ENOMEM;
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

    mem = bflb_realloc(PMEM_HEAP, old, newlen);

    if (mem == NULL) {
        reent->_errno = ENOMEM;
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

    mem = bflb_calloc(PMEM_HEAP, nmenb, size);

    if (mem == NULL) {
        reent->_errno = ENOMEM;
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

    mem = bflb_malloc_align(PMEM_HEAP, align, size);

    if (mem == NULL) {
        reent->_errno = ENOMEM;
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

    bflb_free(PMEM_HEAP, addr);
}
