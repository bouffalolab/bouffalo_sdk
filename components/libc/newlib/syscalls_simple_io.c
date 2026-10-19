#include <unistd.h>
#include <stdio.h>
#include <reent.h>

#include "console_output.h"

#ifdef CONFIG_BSP_CONSOLE_USB_CDC
#include "usb_console.h"
#elif defined(CONFIG_CONSOLE_WO)
#include "bflb_wo.h"
#else
#include "bflb_uart.h"
#endif

struct bflb_device_s *console = NULL;
static char console_previous_char;

#ifdef CONFIG_CONSOLE_WO
void bflb_wo_set_console(struct bflb_device_s *dev)
#else
void bflb_uart_set_console(struct bflb_device_s *dev)
#endif
{
    console = dev;
}

static ssize_t console_backend_write(const void *data, size_t size)
{
#ifdef CONFIG_BSP_CONSOLE_USB_CDC
    return usb_console_write(data, size);
#elif defined(CONFIG_CONSOLE_WO)
    bflb_wo_uart_put(console, (uint8_t *)data, (uint32_t)size);
#else
    (void)bflb_uart_put(console, (uint8_t *)data, (uint32_t)size);
#endif
    return (ssize_t)size;
}

ssize_t bflb_console_write(const void *data, size_t size)
{
    const char *bytes = data;
    size_t start = 0U;

    if (size == 0U) {
        return 0;
    }
    if (data == NULL) {
        return -1;
    }

#ifndef CONFIG_BSP_CONSOLE_USB_CDC
    if (console == NULL) {
        return -1;
    }
#endif

    for (size_t i = 0U; i < size; i++) {
        if ((bytes[i] == '\n') && (console_previous_char != '\r')) {
            if ((i > start) && (console_backend_write(&bytes[start], i - start) < 0)) {
                return -1;
            }
            if (console_backend_write("\r", 1U) < 0) {
                return -1;
            }
            start = i;
        }
        console_previous_char = bytes[i];
    }

    if ((start < size) && (console_backend_write(&bytes[start], size - start) < 0)) {
        return -1;
    }

    return (ssize_t)size;
}

ssize_t _console_write_r(struct _reent *reent, int fd, const void *ptr, size_t size)
{
    if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        return bflb_console_write(ptr, size);
    }
    return -1;
}

#ifndef CONFIG_CONSOLE_WO
ssize_t _console_read_r(struct _reent *reent, int fd, void *ptr, size_t size)
{
#ifdef CONFIG_BSP_CONSOLE_USB_CDC
    (void)reent;
    (void)ptr;
    (void)size;
    return -1;
#else
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
#endif
}
#endif

#ifndef CONFIG_CONSOLE_WO
ssize_t _read_r(struct _reent *r, int fd, void *dst, size_t size)
__attribute__((weak, alias("_console_read_r")));
#endif
ssize_t _write_r(struct _reent *r, int fd, const void *data, size_t size)
__attribute__((weak, alias("_console_write_r")));
