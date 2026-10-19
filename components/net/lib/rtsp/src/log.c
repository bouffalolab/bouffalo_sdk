#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include "log.h"


#if LOG_SWITCH
static char *log_color[] = {
    [LOG_EMERG]     = COLOR_EMERG,
    [LOG_ALERT]     = COLOR_ALERT,
    [LOG_CRIT]      = COLOR_CRIT,
    [LOG_ERR]       = COLOR_ERR,
    [LOG_WARNING]   = COLOR_WARNING,
    [LOG_NOTICE]    = COLOR_NOTICE,
    [LOG_INFO]      = COLOR_INFO,
    [LOG_DEBUG]     = COLOR_DEBUG,
};
#endif

int log_msg(const char *fmt, ...)
{
    int n = 0;
#if LOG_SWITCH
    int level = DFL_LOG_LEVEL;
    va_list ap = NULL;
    const char *ptr = NULL;
    const char *log_fmt = NULL;

    /* Find log level. */
    ptr = strstr(fmt, LOG_FMT);
    if (ptr) {
        log_fmt = LOG_FMT;
        ptr += strlen(LOG_FMT);
    } else {
        ptr = fmt;
    }
    if (*ptr == '<' &&       /* Found '<' */
        *(ptr + 2) == '>' && /* Found '>' */
        *(ptr + 1) >= '0' &&
        *(ptr + 1) <= '7') {
        level = *(ptr + 1) - '0';
        ptr += 3;
    }

    /* Log the message. */
    if (level <= THRESHOLD_LOG_LEVEL) {
        va_start(ap, fmt);
        n += fprintf(stderr, "%s", log_color[level]);
        if (log_fmt) {
            n += vfprintf(stderr, log_fmt, ap);
            va_arg(ap, char *);     /* Skip __FILE__ */
            va_arg(ap, char *);     /* Skip __FUNCTION__ */
            va_arg(ap, int);        /* Skip __LINE__ */
        }
        n += vfprintf(stderr, ptr, ap);
        va_end(ap);
    }
#endif    
    return n;
}

void test_log_color(void)
{
#if LOG_SWITCH
    log_msg("hello world\n");
    printf("Test for log color ^_^\n");
    printf(EMERG"Test for log color ^_^\n");
    printf(ALERT"Test for log color ^_^\n");
    printf(CRIT"Test for log color ^_^\n");
    printf(ERR"Test for log color ^_^\n");
    printf(WARNING"Test for log color ^_^\n");
    printf(NOTICE"Test for log color ^_^\n");
    printf(INFO"Test for log color ^_^\n");
    printf(DEBUG"Test for log color ^_^\n");
    usleep(1);
    exit(1);
#endif
    return;
}
