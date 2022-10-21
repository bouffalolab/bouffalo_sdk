#include "bflb_uart.h"
#include "stdarg.h"

struct bflb_device_s *console = NULL;

int puts(const char *s)
{
    int counter = 0;
    char c;

    while ('\0' != (c = *(s++))) {
        bflb_uart_putchar(console, c);
        counter++;
    }
    return counter;
}

int bflb_printf(const char *fmt, ...)
{
    char print_buf[128];
    uint32_t len;
    va_list ap;

    va_start(ap, fmt);
    len = vsnprintf(print_buf, sizeof(print_buf), fmt, ap);
    va_end(ap);

    len = (len > sizeof(print_buf)) ? sizeof(print_buf) : len;

    for (size_t i = 0; i < len; i++) {
        bflb_uart_putchar(console, print_buf[i]);
    }
    return 0;
}

void bflb_reg_dump(uint32_t addr)
{
    printf("%08lx[31:0]=%08lx\r\n", addr, *(volatile uint32_t *)(addr));
}

void bflb_uart_set_console(struct bflb_device_s *dev)
{
    console = dev;
}