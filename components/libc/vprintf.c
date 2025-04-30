#ifdef CONFIG_CONSOLE_WO
#include "bflb_wo.h"
#else
#include "bflb_uart.h"
#endif
#include "stdarg.h"

extern struct bflb_device_s *console;

#if defined(CONFIG_VSNPRINTF_NANO) && CONFIG_VSNPRINTF_NANO
int vprintf(const char *fmt, va_list ap)
{
    char print_buf[512];
    int len;

    if (console == NULL) {
        return 0;
    }

    len = vsnprintf(print_buf, sizeof(print_buf), fmt, ap);

    len = (len > sizeof(print_buf)) ? sizeof(print_buf) : len;

#ifdef CONFIG_CONSOLE_WO
    bflb_wo_uart_put(console, (uint8_t *)print_buf, len);
#else
    bflb_uart_put(console, (uint8_t *)print_buf, len);
#endif

    return len;
}
#else
extern int console_vsnprintf(const char *fmt, va_list args);
int vprintf(const char *fmt, va_list ap)
{
    int len;

    if (console == NULL) {
        return 0;
    }

    len = console_vsnprintf(fmt, ap);

    return len;
}
#endif
