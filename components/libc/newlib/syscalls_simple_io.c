#include <unistd.h>
#include <stdio.h>
#include <reent.h>

#ifdef CONFIG_CONSOLE_WO
#include "bflb_wo.h"
#else
#include "bflb_uart.h"
#endif

struct bflb_device_s *console = NULL;

#ifdef CONFIG_CONSOLE_WO
void bflb_wo_set_console(struct bflb_device_s *dev)
#else
void bflb_uart_set_console(struct bflb_device_s *dev)
#endif
{
    console = dev;
}

#ifdef CONFIG_CONSOLE_WO
    #define CONSOLE_PUTCHAR(console, x) bflb_wo_uart_putchar(console, x)
#else
    #define CONSOLE_PUTCHAR(console, x) bflb_uart_putchar(console, x)
#endif

ssize_t _console_write_r(struct _reent *reent, int fd, const void *ptr, size_t size)
{
    const char* cptr = (const char*) ptr;
    if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        for (size_t i = 0; i < size; ++i) {
            if (cptr[i] == '\n') {
                CONSOLE_PUTCHAR(console, '\r');
            }
            CONSOLE_PUTCHAR(console, cptr[i]);
        }
        return size;
    }
    return -1;
}

#ifndef CONFIG_CONSOLE_WO
ssize_t _console_read_r(struct _reent *reent, int fd, void *ptr, size_t size)
{
    char* cptr = (char*) ptr;
    if (fd == STDIN_FILENO) {
        size_t recv_num;
        for (recv_num = 0; recv_num < size; ++recv_num) {
            int ch = bflb_uart_getchar(console);
	    cptr[recv_num] = ch;
            if (cptr[recv_num] == '\r') {
                cptr[recv_num] = '\n';
            }
        }
        return recv_num;
    }
    return -1;
}
#endif

#ifndef CONFIG_CONSOLE_WO
ssize_t _read_r(struct _reent *r, int fd, void *dst, size_t size)
__attribute__((weak, alias("_console_read_r")));
#endif
ssize_t _write_r(struct _reent *r, int fd, const void *data, size_t size)
__attribute__((weak, alias("_console_write_r")));
