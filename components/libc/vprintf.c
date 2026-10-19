#include <stdio.h>
#include "stdarg.h"

#include "console_output.h"

#if defined(CONFIG_VSNPRINTF_NANO)
int vprintf(const char *fmt, va_list ap)
{
    char print_buf[512];
    int len;

    len = vsnprintf(print_buf, sizeof(print_buf), fmt, ap);

    len = (len > sizeof(print_buf)) ? sizeof(print_buf) : len;
    (void)bflb_console_write(print_buf, (size_t)len);

    return len;
}
#else
extern int console_vsnprintf(const char *fmt, va_list args);
int vprintf(const char *fmt, va_list ap)
{
    int len;

    len = console_vsnprintf(fmt, ap);

    return len;
}
#endif
