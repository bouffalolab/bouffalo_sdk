#include <errno.h>
#include <stdarg.h>
#include "bflb_uart.h"
#include "vlibc_stdio.h"

#define IOCONSOLE_IO   ((uint32_t)0x00000001)
#define IOCONSOLE_NAME "console"

struct bflb_device_s *console = NULL;

int printf(const char *fmt, ...)
{
    char print_buf[1024];
    uint32_t len;
    va_list ap;

    if (console == NULL) {
        return 0;
    }

    va_start(ap, fmt);
    len = vsnprintf(print_buf, sizeof(print_buf), fmt, ap);
    va_end(ap);

    len = (len > sizeof(print_buf)) ? sizeof(print_buf) : len;

    bflb_uart_put(console, (uint8_t *)print_buf, len);

    return 0;
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

#define __is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')
void bflb_dump_hex(const void *ptr, uint32_t buflen)
{
    unsigned char *buf = (unsigned char *)ptr;
    int i, j;

    for (i = 0; i < buflen; i += 16) {
        printf("%08X:", i);

        for (j = 0; j < 16; j++)
            if (i + j < buflen) {
                if ((j % 8) == 0) {
                    printf("  ");
                }

                printf("%02X ", buf[i + j]);
            } else
                printf("   ");
        printf(" ");

        for (j = 0; j < 16; j++)
            if (i + j < buflen)
                printf("%c", __is_print(buf[i + j]) ? buf[i + j] : '.');
        printf("\n");
    }
}

void bflb_reg_dump(uint32_t addr)
{
    printf("%08lx[31:0]=%08lx\r\n", addr, *(volatile uint32_t *)(uintptr_t)(addr));
}

int bflb_data_compare(const uint8_t *expected, uint8_t *input, uint32_t len)
{
    int i = 0;
    for (i = 0; i < len; i++) {
        if (input[i] != expected[i]) {
            printf("Compare fail at %d,input %02x, but expect %02x\r\n", i, input[i], expected[i]);
            return -1;
        }
    }

    return 0;
}

void bflb_uart_set_console(struct bflb_device_s *dev)
{
    console = dev;

    /*!< vlibc_stdout and vlibc_stderr should config by user */
    // vlibc_stdout = vlibc_fopen("<" IOCONSOLE_NAME, "w");
    // vlibc_setvbuf(vlibc_stdout, NULL, _IONBF, 0);
    // vlibc_stderr = vlibc_stdout;
}