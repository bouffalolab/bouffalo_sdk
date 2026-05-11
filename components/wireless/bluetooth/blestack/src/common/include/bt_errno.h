/* errno.h - errno numbers */

/*
 * Copyright (c) 1984-1999, 2012 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)errno.h	7.1 (Berkeley) 6/4/86
 */

#ifndef __INCerrnoh
#define __INCerrnoh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef errno
extern int *__errno(void);
#define errno (*__errno())
#endif

/*
 * POSIX Error codes
 */


#ifdef EPERM
#undef EPERM
#endif
#define EPERM 1		/* Not owner */
#ifdef ENOENT
#undef ENOENT
#endif
#define ENOENT 2	/* No such file or directory */
#ifdef ESRCH
#undef ESRCH
#endif
#define ESRCH 3		/* No such context */
#ifdef EINTR
#undef EINTR
#endif
#define EINTR 4		/* Interrupted system call */
#ifdef EIO
#undef EIO
#endif
#define EIO 5		/* I/O error */
#ifdef ENXIO
#undef ENXIO
#endif
#define ENXIO 6		/* No such device or address */
#ifdef E2BIG
#undef E2BIG
#endif
#define E2BIG 7		/* Arg list too long */
#ifdef ENOEXEC
#undef ENOEXEC
#endif
#define ENOEXEC 8       /* Exec format error */
#ifdef EBADF
#undef EBADF
#endif
#define EBADF 9		/* Bad file number */
#ifdef ECHILD
#undef ECHILD
#endif
#define ECHILD 10       /* No children */
#ifdef EAGAIN
#undef EAGAIN
#endif
#define EAGAIN 11       /* No more contexts */
#ifdef ENOMEM
#undef ENOMEM
#endif
#define ENOMEM 12       /* Not enough core */
#ifdef EACCES
#undef EACCES
#endif
#define EACCES 13       /* Permission denied */
#ifdef EFAULT
#undef EFAULT
#endif
#define EFAULT 14       /* Bad address */
#ifdef ENOTEMPTY
#undef ENOTEMPTY
#endif
#define ENOTEMPTY 15    /* Directory not empty */
#ifdef EBUSY
#undef EBUSY
#endif
#define EBUSY 16	/* Mount device busy */
#ifdef EEXIST
#undef EEXIST
#endif
#define EEXIST 17       /* File exists */
#ifdef EXDEV
#undef EXDEV
#endif
#define EXDEV 18	/* Cross-device link */
#ifdef ENODEV
#undef ENODEV
#endif
#define ENODEV 19       /* No such device */
#ifdef ENOTDIR
#undef ENOTDIR
#endif
#define ENOTDIR 20      /* Not a directory */
#ifdef EISDIR
#undef EISDIR
#endif
#define EISDIR 21       /* Is a directory */
#ifdef EINVAL
#undef EINVAL
#endif
#define EINVAL 22       /* Invalid argument */
#ifdef ENFILE
#undef ENFILE
#endif
#define ENFILE 23       /* File table overflow */
#ifdef EMFILE
#undef EMFILE
#endif
#define EMFILE 24       /* Too many open files */
#ifdef ENOTTY
#undef ENOTTY
#endif
#define ENOTTY 25       /* Not a typewriter */
#ifdef ENAMETOOLONG
#undef ENAMETOOLONG
#endif
#define ENAMETOOLONG 26 /* File name too long */
#ifdef EFBIG
#undef EFBIG
#endif
#define EFBIG 27	/* File too large */
#ifdef ENOSPC
#undef ENOSPC
#endif
#define ENOSPC 28       /* No space left on device */
#ifdef ESPIPE
#undef ESPIPE
#endif
#define ESPIPE 29       /* Illegal seek */
#ifdef EROFS
#undef EROFS
#endif
#define EROFS 30	/* Read-only file system */
#ifdef EMLINK
#undef EMLINK
#endif
#define EMLINK 31       /* Too many links */
#ifdef EPIPE
#undef EPIPE
#endif
#define EPIPE 32	/* Broken pipe */
#ifdef EDEADLK
#undef EDEADLK
#endif
#define EDEADLK 33      /* Resource deadlock avoided */
#ifdef ENOLCK
#undef ENOLCK
#endif
#define ENOLCK 34       /* No locks available */
#ifdef ENOTSUP
#undef ENOTSUP
#endif
#define ENOTSUP 35      /* Unsupported value */
#ifdef EMSGSIZE
#undef EMSGSIZE
#endif
#define EMSGSIZE 36     /* Message size */

/* ANSI math software */
#ifndef EDOM
#define EDOM 37   /* Argument too large */
#endif
#ifndef ERANGE
#define ERANGE 38 /* Result too large */
#endif

/* ipc/network software */

/* argument errors */
#ifdef EDESTADDRREQ
#undef EDESTADDRREQ
#endif
#define EDESTADDRREQ 40    /* Destination address required */
#ifdef EPROTOTYPE
#undef EPROTOTYPE
#endif
#define EPROTOTYPE 41      /* Protocol wrong type for socket */
#ifdef ENOPROTOOPT
#undef ENOPROTOOPT
#endif
#define ENOPROTOOPT 42     /* Protocol not available */
#ifdef EPROTONOSUPPORT
#undef EPROTONOSUPPORT
#endif
#define EPROTONOSUPPORT 43 /* Protocol not supported */
#ifdef ESOCKTNOSUPPORT
#undef ESOCKTNOSUPPORT
#endif
#define ESOCKTNOSUPPORT 44 /* Socket type not supported */
#ifdef EOPNOTSUPP
#undef EOPNOTSUPP
#endif
#define EOPNOTSUPP 45      /* Operation not supported on socket */
#ifdef EPFNOSUPPORT
#undef EPFNOSUPPORT
#endif
#define EPFNOSUPPORT 46    /* Protocol family not supported */
#ifdef EAFNOSUPPORT
#undef EAFNOSUPPORT
#endif
#define EAFNOSUPPORT 47    /* Addr family not supported */
#ifdef EADDRINUSE
#undef EADDRINUSE
#endif
#define EADDRINUSE 48      /* Address already in use */
#ifdef EADDRNOTAVAIL
#undef EADDRNOTAVAIL
#endif
#define EADDRNOTAVAIL 49   /* Can't assign requested address */
#ifdef ENOTSOCK
#undef ENOTSOCK
#endif
#define ENOTSOCK 50	/* Socket operation on non-socket */

/* operational errors */
#ifdef ENETUNREACH
#undef ENETUNREACH
#endif
#define ENETUNREACH 51  /* Network is unreachable */
#ifdef ENETRESET
#undef ENETRESET
#endif
#define ENETRESET 52    /* Network dropped connection on reset */
#ifdef ECONNABORTED
#undef ECONNABORTED
#endif
#define ECONNABORTED 53 /* Software caused connection abort */
#ifdef ECONNRESET
#undef ECONNRESET
#endif
#define ECONNRESET 54   /* Connection reset by peer */
#ifdef ENOBUFS
#undef ENOBUFS
#endif
#define ENOBUFS 55      /* No buffer space available */
#ifdef EISCONN
#undef EISCONN
#endif
#define EISCONN 56      /* Socket is already connected */
#ifdef ENOTCONN
#undef ENOTCONN
#endif
#define ENOTCONN 57     /* Socket is not connected */
#ifdef ESHUTDOWN
#undef ESHUTDOWN
#endif
#define ESHUTDOWN 58    /* Can't send after socket shutdown */
#ifdef ETOOMANYREFS
#undef ETOOMANYREFS
#endif
#define ETOOMANYREFS 59 /* Too many references: can't splice */
#ifdef ETIMEDOUT
#undef ETIMEDOUT
#endif
#define ETIMEDOUT 60    /* Connection timed out */
#ifdef ECONNREFUSED
#undef ECONNREFUSED
#endif
#define ECONNREFUSED 61 /* Connection refused */
#ifdef ENETDOWN
#undef ENETDOWN
#endif
#define ENETDOWN 62     /* Network is down */
#ifdef ETXTBSY
#undef ETXTBSY
#endif
#define ETXTBSY 63      /* Text file busy */
#ifdef ELOOP
#undef ELOOP
#endif
#define ELOOP 64	/* Too many levels of symbolic links */
#ifdef EHOSTUNREACH
#undef EHOSTUNREACH
#endif
#define EHOSTUNREACH 65 /* No route to host */
#ifdef ENOTBLK
#undef ENOTBLK
#endif
#define ENOTBLK 66      /* Block device required */
#ifdef EHOSTDOWN
#undef EHOSTDOWN
#endif
#define EHOSTDOWN 67    /* Host is down */

/* non-blocking and interrupt i/o */
#ifdef EINPROGRESS
#undef EINPROGRESS
#endif
#define EINPROGRESS 68 /* Operation now in progress */
#ifdef EALREADY
#undef EALREADY
#endif
#define EALREADY 69    /* Operation already in progress */
#ifndef EWOULDBLOCK
#define EWOULDBLOCK EAGAIN /* Operation would block */
#endif

#ifdef ENOSYS
#undef ENOSYS
#endif
#define ENOSYS 71 /* Function not implemented */

/* aio errors (should be under posix) */
#ifdef ECANCELED
#undef ECANCELED
#endif
#define ECANCELED 72 /* Operation canceled */

#ifndef ERRMAX
#define ERRMAX 81
#endif

/* specific STREAMS errno values */

#ifdef ENOSR
#undef ENOSR
#endif
#define ENOSR 74   /* Insufficient memory */
#ifdef ENOSTR
#undef ENOSTR
#endif
#define ENOSTR 75  /* STREAMS device required */
#ifdef EPROTO
#undef EPROTO
#endif
#define EPROTO 76  /* Generic STREAMS error */
#ifdef EBADMSG
#undef EBADMSG
#endif
#define EBADMSG 77 /* Invalid STREAMS message */
#ifdef ENODATA
#undef ENODATA
#endif
#define ENODATA 78 /* Missing expected message data */
#ifdef ETIME
#undef ETIME
#endif
#define ETIME 79   /* STREAMS timeout occurred */
#ifdef ENOMSG
#undef ENOMSG
#endif
#define ENOMSG 80  /* Unexpected message type */

#ifdef __cplusplus
}
#endif


#endif /* __INCerrnoh */
