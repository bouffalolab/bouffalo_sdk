#include <errno.h>
#include <stdarg.h>
#include "bflb_uart.h"
#include "vlibc_stdio.h"

#define IOCONSOLE_IO   ((uint32_t)0x00000001)
#define IOCONSOLE_NAME "console"

struct bflb_device_s *console = NULL;

void bflb_reg_dump(uint32_t addr)
{
    printf("%08lx[31:0]=%08lx\r\n", addr, *(volatile uint32_t *)(uintptr_t)(addr));
}

uint16_t __console_output(void *ptr, uint16_t size)
{
    for (size_t i = 0; i < size; i++) {
        bflb_uart_putchar(console, ((char *)ptr)[i]);
    }
    return size;
}

void bflb_uart_set_console(struct bflb_device_s *dev)
{
    console = dev;
    bflb_uart_putchar(console, '\r');
    vlibc_stdout = vlibc_fopen("<" IOCONSOLE_NAME, "w");
    vlibc_setvbuf(vlibc_stdout, NULL, _IONBF, 0);
    vlibc_stderr = vlibc_stdout;

    extern void log_init(void);
    log_init();
}

__WEAK uint32_t __vlibc_io_init(const char *name, uint8_t mode)
{
    (void)mode;
    if (strcmp(name, IOCONSOLE_NAME) == 0) {
        return IOCONSOLE_IO;
    } else {
    }

    return ENOENT;
}

__WEAK uint32_t __vlibc_io_deinit(struct __vlibc_io *io)
{
    if (io->dev == IOCONSOLE_IO) {
        return IOCONSOLE_IO;
    } else {
    }

    return EOF;
}

__WEAK size_t __vlibc_io_mem2dev(struct __vlibc_io *io, const void *ptr, size_t size)
{
    if (io->dev == IOCONSOLE_IO) {
        for (size_t i = 0; i < size; i++) {
            bflb_uart_putchar(console, ((char *)ptr)[i]);
        }
        return size;
    } else {
    }

    return 0;
}