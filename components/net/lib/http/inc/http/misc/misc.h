/** @file
 * @brief HTTP QC adapter
 *
 */

#ifndef __MISC_QC_H__
#define __MISC_QC_H__

#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include "FreeRTOS.h"
#include "__assert.h"
#include "lwip/sockets.h"
#include "https_wrapper.h"

typedef TickType_t k_ticks_t;
typedef k_ticks_t k_timeout_t;
typedef struct { k_ticks_t ticks; } k_timepoint_t;

/**
 * @brief Check for macro definition in compiler-visible expressions
 *
 * This trick was pioneered in Linux as the config_enabled() macro.
 * The madness has the effect of taking a macro value that may be
 * defined to "1" (e.g. CONFIG_MYFEATURE), or may not be defined at
 * all and turning it into a literal expression that can be used at
 * "runtime".  That is, it works similarly to
 * "defined(CONFIG_MYFEATURE)" does except that it is an expansion
 * that can exist in a standard expression and be seen by the compiler
 * and optimizer.  Thus much ifdef usage can be replaced with cleaner
 * expressions like:
 *
 *     if (IS_ENABLED(CONFIG_MYFEATURE))
 *             myfeature_enable();
 *
 * INTERNAL
 * First pass just to expand any existing macros, we need the macro
 * value to be e.g. a literal "1" at expansion time in the next macro,
 * not "(1)", etc...  Standard recursive expansion does not work.
 */
#define IS_ENABLED(config_macro) _IS_ENABLED1(config_macro)

/* Now stick on a "_XXXX" prefix, it will now be "_XXXX1" if config_macro
 * is "1", or just "_XXXX" if it's undefined.
 *   ENABLED:   _IS_ENABLED2(_XXXX1)
 *   DISABLED   _IS_ENABLED2(_XXXX)
 */
#define _IS_ENABLED1(config_macro) _IS_ENABLED2(_XXXX##config_macro)

/* Here's the core trick, we map "_XXXX1" to "_YYYY," (i.e. a string
 * with a trailing comma), so it has the effect of making this a
 * two-argument tuple to the preprocessor only in the case where the
 * value is defined to "1"
 *   ENABLED:    _YYYY,    <--- note comma!
 *   DISABLED:   _XXXX
 */
#define _XXXX1 _YYYY,

/* Then we append an extra argument to fool the gcc preprocessor into
 * accepting it as a varargs macro.
 *                         arg1   arg2  arg3
 *   ENABLED:   _IS_ENABLED3(_YYYY,    1,    0)
 *   DISABLED   _IS_ENABLED3(_XXXX 1,  0)
 */
#define _IS_ENABLED2(one_or_two_args) _IS_ENABLED3(one_or_two_args 1, 0)

/* And our second argument is thus now cooked to be 1 in the case
 * where the value is defined to 1, and 0 if not:
 */
#define _IS_ENABLED3(ignore_this, val, ...) val

#ifndef __fallthrough 
#define __fallthrough __attribute__((fallthrough))
#endif 

#ifndef offsetof
#define offsetof(type, member) ((size_t) &((type *)0)->member)
#endif

#ifndef CONTAINER_OF
#define CONTAINER_OF(ptr, type, field) \
    ((type *)(((char *)(ptr)) - offsetof(type, field)))
#endif

#ifndef MIN
# define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#define snprintk                                                   snprintf 

#define SYS_FOREVER_MS                                             portMAX_DELAY
#define K_FOREVER                                                  portMAX_DELAY
#define K_MSEC(t)                                                  ((t)*portTICK_PERIOD_MS)
#define k_ticks_to_ms_floor32(t)                                   (uint32_t)K_MSEC((t))

#define ZSOCK_POLLOUT                                              POLLOUT
#define ZSOCK_POLLIN                                               POLLIN
#define ZSOCK_POLLERR                                              POLLERR
#define ZSOCK_POLLNVAL                                             POLLNVAL 
#define ZSOCK_POLLHUP                                              POLLHUP

#define sys_timepoint_timeout(x)                                   x
#define zsock_send(s, buf, len, flags)                             https_wrapper_send((https_wrapper_handle_t)s, (const void *)buf, len, flags)
#define zsock_getsockopt(s,level,optname,opval,optlen)             getsockopt(https_wrapper_socketfd_get((https_wrapper_handle_t)s),level,optname,opval,optlen)
#define zsock_recv(s,mem,len,flags)                                https_wrapper_recv((https_wrapper_handle_t)s,mem,len,flags)

struct zsock_pollfd {
    struct pollfd fds;
    int fd;
    short events;
    short revents;
};

static inline int zsock_poll(struct zsock_pollfd *fds, nfds_t nfds, int timeout)
{
    int ret;

    fds->fds.events = fds->events;
    fds->fds.revents = fds->revents;
    fds->fds.fd = https_wrapper_socketfd_get((https_wrapper_handle_t)fds->fd);

    ret = poll(&fds->fds, nfds, timeout);

    fds->events = fds->fds.events;
    fds->revents = fds->fds.revents;
    return ret;
}

#define NET_DBG                                                    //printf
#define NET_ERR                                                    printf
#define LOG_HEXDUMP_DBG
#define CONFIG_NET_HTTP_LOG_LEVEL_DBG  0

static inline k_timepoint_t sys_timepoint_calc(k_timeout_t timeout)
{
    k_timepoint_t timepoint = {
        .ticks = timeout,
    };
    return timepoint;
}
#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __MISC_QC_H__ */
