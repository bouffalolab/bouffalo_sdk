#ifndef __LOG_H__
#define __LOG_H__

#include <errno.h>


#define LOG_SWITCH 0              /* Log on or off. */
#define LOG_COLOR 1               /* Log color or non-color. */
#define DFL_LOG_LEVEL   LOG_DEBUG /* If not specify level, use the default one. */
#define THRESHOLD_LOG_LEVEL LOG_DEBUG /* Only log from [THRESHOLD_LOG_LEVEL, LOG_EMERG. */

/**
 * priorities/facilities are encoded into a single 32-bit quantity, where the
 * bottom 3 bits are the priority (0-7) and the top 28 bits are the facility
 * (0-big number).  Both the priorities and the facilities map roughly
 * one-to-one to strings in the syslogd(8) source code.  This mapping is
 * included in this file.
 *
 * priorities (these are ordered)
 */
#define	LOG_EMERG	0           /* system is unusable */
#define	LOG_ALERT	1           /* action must be taken immediately */
#define	LOG_CRIT	2           /* critical conditions */
#define	LOG_ERR		3           /* error conditions */
#define	LOG_WARNING	4           /* warning conditions */
#define	LOG_NOTICE	5           /* normal but significant condition */
#define	LOG_INFO	6           /* informational */
#define	LOG_DEBUG	7           /* debug-level messages */

#define EMERG      "<0>"
#define ALERT      "<1>"
#define CRIT       "<2>"
#define ERR        "<3>"
#define WARNING    "<4>"
#define NOTICE     "<5>"
#define INFO       "<6>"
#define DEBUG      "<7>"


/* Color print. */
#if LOG_COLOR
#define COLOR_EMERG   "\033[1;31m*EMERG*\033[m" /* Highlight, red foreground */
#define COLOR_ALERT   "\033[1;33m*ALERT*\033[m" /* Highlight, yellow foreground. */
#define COLOR_CRIT    "\033[35m*CRIT*\033[m"    /* Purple forground. */
#define COLOR_ERR     "\033[31m*ERR*\033[m"     /* Red foreground. */
#define COLOR_WARNING "\033[33m*WARNING*\033[m" /* Yellow foreground. */
#define COLOR_NOTICE  "\033[36m*NOTICE*\033[m"  /* Cyan foreground. */
#define COLOR_INFO    "\033[34m*INFO*\033[m"    /* Blue foreground. */
#define COLOR_DEBUG   "\033[32m*DEBUG*\033[m"   /* Green foreground. */
#else
#define COLOR_NONE
#define COLOR_EMERG   "*EMERG*"
#define COLOR_ALERT   "*ALERT*"
#define COLOR_CRIT    "*CRIT*"
#define COLOR_ERR     "*ERR*"
#define COLOR_WARNING "*WARNING*"
#define COLOR_NOTICE  "*NOTICE*"
#define COLOR_INFO    "*INFO*"
#define COLOR_DEBUG   "*DEBUG*"
#endif



#if 0//LOG_SWITCH    /* ================ OPEN log. ================ */

#define LOG_FMT "[%s, %s, %d] "

/* Never use the function directly, `printf()` instead. */
int log_msg(const char *fmt, ...);

/**
 * Usage:
 *     printf(INFO "Hello %s!\n", "world");
 * this is equal to following:
 *     printf("<6>Hello %s!\n", "world");
 *
 * If you haven't specified the log level,
 * then it makes the macro `DFL_LOG_LEVEL` as default.
 */
#define printf(fmt, args...)                                \
    ({                                                      \
        log_msg(LOG_FMT fmt,                                \
                __FILE__, __FUNCTION__, __LINE__, ##args);  \
    })

#define printf(fmt)                                                \
    ({                                                              \
        log_msg(LOG_FMT fmt ": %s.\n",                              \
                __FILE__, __FUNCTION__, __LINE__, strerror(errno)); \
    })                                                              \

/* Print pretty RTSP request message. */
#define print_req_msg(msg)                                              \
    ({                                                                  \
        log_msg(INFO "\n\033[1;33;41m[Begin]===================== Request Message ========================[Begin]\033[0m\n" \
                "\033[32m%s\033[0m"                                     \
                "\033[33m[End]==================================================================[End]\033[0m\n\n\n", msg); \
    })

/* Notify when entering or leaving a thread. */
#define entering_thread()                                               \
    printf(INFO"Entering thread %s[pid: %u] ...\n", __FUNCTION__, getpid())
#define leaving_thread()                                                \
    printf(INFO"Leaving thread %s[pid: %u] ...\n", __FUNCTION__, getpid())

#else    /* ================ CLOSE log. ================ */
//#define printf(fmt, args...)
//#define printf(fmt) 
#define print_req_msg(msg) 
#define leaving_thread()
#define entering_thread()
#endif

void test_log_color(void);


#endif /* __LOG_H__ */

