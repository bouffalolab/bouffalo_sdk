/**
 * @file _ansi.h
 * @brief Minimal ANSI compatibility header for Picolibc
 *
 * This header provides compatibility between Picolibc and traditional Newlib
 * by defining common macros and types.
 */

#ifndef _ANSI_H_
#define _ANSI_H_

#ifdef __cplusplus
#if defined(__GNUC__)
#define __BEGIN_DECLS extern "C" {
#define __END_DECLS }
#endif
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif

/* Inline keyword */
#ifndef __inline
#define __inline inline
#endif

/* Attribute macros */
#ifndef __attribute__
#define __attribute__(x)
#endif

/* Const attribute */
#ifndef __const
#define __const __attribute__((__const__))
#endif

/* Pure function attribute */
#ifndef __pure
#define __pure __attribute__((__pure__))
#endif

/* Unused variable attribute */
#ifndef __unused
#define __unused __attribute__((__unused__))
#endif

/* Used attribute */
#ifndef __used
#define __used __attribute__((__used__))
#endif

/* Weak symbol attribute */
#ifndef __weak
#define __weak __attribute__((__weak__))
#endif

/* Aligned attribute */
#ifndef __aligned
#define __aligned(x) __attribute__((__aligned__(x)))
#endif

/* Packed attribute */
#ifndef __packed
#define __packed __attribute__((__packed__))
#endif

/* Deprecated attribute */
#ifndef __deprecated
#define __deprecated __attribute__((__deprecated__))
#endif

/* Printf format attribute */
#ifndef __printflike
#define __printflike(a, b) __attribute__((__format__(__printf__, a, b)))
#endif

/* Scanf format attribute */
#ifndef __scanflike
#define __scanflike(a, b) __attribute__((__format__(__scanf__, a, b)))
#endif

/* Returns nonnull attribute */
#ifndef __returns_nonnull
#define __returns_nonnull __attribute__((__returns_nonnull__))
#endif

/* Nonnull attribute */
#ifndef __nonnull
#define __nonnull(params) __attribute__((__nonnull__ params))
#endif

/* Warn unused result attribute */
#ifndef __warn_unused_result
#define __warn_unused_result __attribute__((__warn_unused_result__))
#endif

#endif /* _ANSI_H_ */
