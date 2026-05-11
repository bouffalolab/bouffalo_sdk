#ifdef CONFIG_CONSOLE_WO
#include "bflb_wo.h"
#else
#include "bflb_uart.h"
#endif
#include <stdarg.h>
#include <stdio.h>

/* Undefine to provide our own implementation for picolibc */
#undef putchar
#undef puts
#undef fputc
#undef fprintf

/* Console device is defined and initialized in port_tty.c */
extern struct bflb_device_s *console;

int putchar(int c)
{
    if (console == NULL) {
        return EOF;
    }

#ifdef CONFIG_CONSOLE_WO
    bflb_wo_uart_putchar(console, (uint8_t)c);
#else
    bflb_uart_putchar(console, (uint8_t)c);
#endif

    return c;
}

/* fputc - optimized version of putchar used by picolibc */
int fputc(int c, FILE *stream)
{
    (void)stream;  /* Ignore stream parameter, always use console */

    if (console == NULL) {
        return EOF;
    }

#ifdef CONFIG_CONSOLE_WO
    bflb_wo_uart_putchar(console, (uint8_t)c);
#else
    bflb_uart_putchar(console, (uint8_t)c);
#endif

    return c;
}

int puts(const char *c)
{
    int len;

    len = strlen(c);

    if (console == NULL) {
        return 0;
    }

#ifdef CONFIG_CONSOLE_WO
    bflb_wo_uart_put(console, (uint8_t *)c, len);
    /* C standard: puts() appends only LF */
    if(c[len - 1] != '\n') {
        bflb_wo_uart_putchar(console, '\r');
        bflb_wo_uart_putchar(console, '\n');
    }
#else
    bflb_uart_put(console, (uint8_t *)c, len);
    /* C standard: puts() appends only LF */
    if(c[len - 1] != '\n') {
        bflb_uart_putchar(console, '\r');
        bflb_uart_putchar(console, '\n');
    }
#endif

    if(c[len - 1] != '\n')
        return len + 2;  /* Return length plus the LF */
    else
        return len;
}

int putstring(const char *c)
{
    return puts(c);
}

/*
 * Custom printf implementation for picolibc
 * Uses custom vsnprintf for formatting, then direct UART output
 * to avoid newline conversion issues in picolibc's stdio layer
 */
extern int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap);

int printf(const char *fmt, ...)
{
    char print_buf[512];
    int len;
    va_list ap;

    if (console == NULL) {
        return 0;
    }

    va_start(ap, fmt);
    len = vsnprintf(print_buf, sizeof(print_buf), fmt, ap);
    va_end(ap);

    if (len < 0) {
        return 0;
    }

    if ((size_t)len > sizeof(print_buf)) {
        len = sizeof(print_buf);
    }

#ifdef CONFIG_CONSOLE_WO
    bflb_wo_uart_put(console, (uint8_t *)print_buf, len);
#else
    bflb_uart_put(console, (uint8_t *)print_buf, len);
#endif

    return len;
}

/*
 * Custom fprintf implementation for picolibc
 * Uses custom vsnprintf for formatting, then direct UART output
 * to avoid newline conversion issues in picolibc's stdio layer
 */
extern int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap);

int fprintf(FILE *stream, const char *fmt, ...)
{
    char print_buf[512];
    int len;
    va_list ap;

    (void)stream;  /* Ignore stream parameter, always use console */

    if (console == NULL) {
        return 0;
    }

    va_start(ap, fmt);
    len = vsnprintf(print_buf, sizeof(print_buf), fmt, ap);
    va_end(ap);

    if (len < 0) {
        return 0;
    }

    if ((size_t)len > sizeof(print_buf)) {
        len = sizeof(print_buf);
    }

#ifdef CONFIG_CONSOLE_WO
    bflb_wo_uart_put(console, (uint8_t *)print_buf, len);
#else
    bflb_uart_put(console, (uint8_t *)print_buf, len);
#endif

    return len;
}

#define __is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')
void bflb_hexdump(const void *ptr, uint32_t buflen)
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

void bflb_dump(uint8_t *data, uint32_t len)
{
    uint32_t i = 0;

    for (i = 0; i < len; i++) {
        if (i % 16 == 0) {
            printf("\r\n");
        }

        printf("%02x ", data[i]);
    }

    printf("\r\n");
}

void bflb_regdump(uint32_t addr)
{
    printf("%08lx[31:0]=%08lx\r\n", addr, *(volatile uint32_t *)(uintptr_t)(addr));
}

/*
 * Override fwrite() for Lua and other stdio users
 * Directly writes to UART console to avoid picolibc stdio buffering issues
 */
extern _ssize_t write(int fd, const void *ptr, size_t size);

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t total_bytes = size * nmemb;
    const uint8_t *data = (const uint8_t *)ptr;
    size_t i;

    (void)stream;  /* Ignore stream parameter, always use console */

    if (console == NULL) {
        return 0;
    }

    /* For stdout and stderr, convert \n to \r\n for proper terminal display */
    if (stream == stdout || stream == stderr) {
        for (i = 0; i < total_bytes; i++) {
            if (data[i] == '\n') {
                /* Output \r before \n */
#ifdef CONFIG_CONSOLE_WO
                bflb_wo_uart_putchar(console, '\r');
                bflb_wo_uart_putchar(console, '\n');
#else
                bflb_uart_putchar(console, '\r');
                bflb_uart_putchar(console, '\n');
#endif
            } else {
                /* Output character as-is */
#ifdef CONFIG_CONSOLE_WO
                bflb_wo_uart_putchar(console, data[i]);
#else
                bflb_uart_putchar(console, data[i]);
#endif
            }
        }
        return nmemb;
    }

    /* For other streams, output directly */
    for (i = 0; i < total_bytes; i++) {
        if (data[i] == '\n') {
#ifdef CONFIG_CONSOLE_WO
            bflb_wo_uart_putchar(console, '\r');
            bflb_wo_uart_putchar(console, '\n');
#else
            bflb_uart_putchar(console, '\r');
            bflb_uart_putchar(console, '\n');
#endif
        } else {
#ifdef CONFIG_CONSOLE_WO
            bflb_wo_uart_putchar(console, data[i]);
#else
            bflb_uart_putchar(console, data[i]);
#endif
        }
    }

    return nmemb;
}

/*
 * Override fflush() for Lua and other stdio users
 * For UART console, this is a no-op since we write directly
 */
int fflush(FILE *stream)
{
    (void)stream;  /* Nothing to flush, writes are direct */

    /* Always return success */
    return 0;
}

/* bflb_uart_set_console() is defined in port_tty.c */
