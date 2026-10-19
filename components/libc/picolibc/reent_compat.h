/**
 * @file reent_compat.h
 * @brief Reentrant structure compatibility layer for Picolibc and Newlib
 *
 * This header provides compatibility between Picolibc and traditional Newlib
 * by defining a common interface for struct _reent operations.
 */

#ifndef __REENT_COMPAT_H__
#define __REENT_COMPAT_H__

#include <errno.h>

/* Forward declarations */
struct _reent;

/* Picolibc doesn't have traditional reent.h - provide minimal stubs */
#include <stddef.h>

#ifndef _REENT_SMALL_DECLARED
#define _REENT_SMALL_DECLARED

/* Minimal reent structure for picolibc compatibility */
struct _reent {
    int _errno;
};

/* Global reentrant structure pointer (for compatibility) */
extern struct _reent *_impure_ptr;

/* Get current reentrant structure */
static inline struct _reent *__getreent(void) {
    return _impure_ptr;
}

#define _REENT_SMALL 1
#define _REENT  _impure_ptr

#endif /* _REENT_SMALL_DECLARED */

/*****************************************************************************
 * Errno access macros for Picolibc
 *****************************************************************************/

/* Set errno in reent structure */
#define REENT_SET_ERRNO(reent, val) do { \
    (void)(reent); \
    errno = (val); \
} while(0)

/* Get errno from reent structure */
#define REENT_GET_ERRNO(reent) (errno)

/*****************************************************************************
 * Common Compatibility Macros
 *****************************************************************************/

/* Helper macros for errno access that work with both libraries */
#define SET_REENT_ERRNO(reent, val) REENT_SET_ERRNO(reent, val)
#define GET_REENT_ERRNO(reent)      REENT_GET_ERRNO(reent)

#endif /* __REENT_COMPAT_H__ */
